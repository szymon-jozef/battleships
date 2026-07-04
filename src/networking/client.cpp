#include "client.hpp"
#include "data_types.hpp"
#include "messages.hpp"
#include <boost/asio/ip/basic_resolver.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/name_generator.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <exception>
#include <memory>
#include <spdlog/spdlog.h>

namespace battleship {
namespace networking {
// === General use methods ===
Client::~Client() {
  disconnect();
}

bool Client::connect(const std::string &host, const uint16_t port) {
  try {
    boost::asio::ip::tcp::resolver resolver(context);
    boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(port));

    connection =
        std::make_shared<Connection>(Connection::Owner::CLIENT, context, boost::asio::ip::tcp::socket(context), queIn);

    connection->connectToServer(endpoints);

    thread = std::thread([this]() { context.run(); });
  } catch (std::exception &e) {
    spdlog::error("[Client] failed trying to connect: {}", e.what());
    return false;
  }
  return true;
}

void Client::disconnect() {
  if (isConnected()) {
    connection->disconnect();
  }

  context.stop();
  if (thread.joinable()) {
    if (std::this_thread::get_id() != thread.get_id()) {
      thread.join();
    } else {
      thread.detach();
    }
  }
  connection.reset();
}

bool Client::isConnected() const {
  return connection ? connection->isConnected() : false;
}

void Client::send(const Message &msg) {
  if (isConnected()) {
    connection->send(msg);
  }
}

void Client::update(size_t maxMessages, bool wait) {
  if (wait) {
    queIn.wait();
  }

  size_t messageCount = 0;
  while (messageCount < maxMessages && !queIn.empty()) {
    auto msg = queIn.pop_front();
    onMessage(msg.msg);
  }
}

void Client::onMessage(Message &msg) {
  switch (msg.header.id) {
  case battleship::networking::MessageType::SERVER_GAME_STATUS: {
    GameStatus newGameStatus = msg.body.pop<GameStatus>();
    spdlog::info("[Client] server decided that current game status is {}", static_cast<int>(newGameStatus));
    currentGameStatus = newGameStatus;
    break;
  }
  case battleship::networking::MessageType::SERVER_GAME_END:
    handleGameEnd(msg);
    break;
  case battleship::networking::MessageType::CLIENT_SEND_ATTACK: {
    // we were attacked here
    handleIncomingAttack(msg);
    break;
  }
  case battleship::networking::MessageType::CLIENT_RECEIVE_ATTACK: {
    // we received the result of our attack here
    handleShotResult(msg);
    break;
  }
  case battleship::networking::MessageType::SERVER_HANDSHAKE: {
    handleServerHandshake(msg);
    break;
  }
  case battleship::networking::MessageType::SERVER_BROADCAST_PLAYERS: {
    handleBroadcastingPlayers(msg);
    break;
  }
  default:
    break;
  }
}

// === Sending messages ===

void Client::sendHandshake(std::string name) {
  Message msg;
  this->name = name;

  msg.header.id = MessageType::CLIENT_HANDSHAKE;
  msg.header.receiver = boost::uuids::nil_uuid();

  PlayerNameMessage pname;

  std::strncpy(pname.name, name.c_str(), sizeof(pname.name) - 1);
  if (sizeof(pname.name) < name.length()) {
    spdlog::warn("[Client] Given username is longer than {}, so it will be cut short!", sizeof(pname.name));
  }

  msg.push(pname);

  send(msg);
}

void Client::sendGameStatus(GameStatus status) {
  spdlog::info("[Client] sending {} game status to the server...", static_cast<int>(status));
  Message msg;
  msg.header.id = MessageType::CLIENT_GAME_STATUS;
  msg.header.sender = id;
  msg.header.receiver = boost::uuids::nil_uuid();
  msg.push(status);
  send(msg);
}

void Client::sendAttack(unsigned short int row, unsigned short int column) {
  spdlog::info("[Client] sending attack at ({},{})", row, column);
  Message msg;
  msg.header.id = MessageType::CLIENT_SEND_ATTACK;
  msg.header.sender = id;
  msg.header.receiver = enemyId;
  msg.push(row);
  msg.push(column);
  send(msg);
}

void Client::sendLost() {
  spdlog::info("[Client] game lost, sending message to the server...");
  Message msg;
  msg.header.id = MessageType::CLIENT_LOST;
  msg.header.sender = id;
  msg.header.receiver = boost::uuids::nil_uuid();
  // we don't add body since server gets id by the connection
  send(msg);
}

// === Handling responses ===

void Client::handleIncomingAttack(Message &msg) {
  if (!recieveAttackFunc) {
    spdlog::error("[Client] recieveAttackFunc is not set!!!!");
    return;
  }
  unsigned short int column = msg.pop<unsigned short int>();
  unsigned short int row = msg.pop<unsigned short int>();
  FieldState result = recieveAttackFunc(row, column);
  spdlog::info("[Client] We were attacked at ({},{}) and the result was {}", row, column, static_cast<int>(result));

  Message resultMsg;
  resultMsg.header.id = MessageType::CLIENT_RECEIVE_ATTACK;
  resultMsg.header.sender = id;
  resultMsg.header.receiver = enemyId;

  resultMsg.push(result);
  resultMsg.push(row);
  resultMsg.push(column);
  send(resultMsg);
}

void Client::handleShotResult(Message &msg) {
  if (!markResultFunc) {
    spdlog::error("[Client] markResultFunc not set!!!");
    return;
  }
  FieldState result;
  unsigned short int row, column;

  column = msg.pop<unsigned short int>();
  row = msg.pop<unsigned short int>();
  result = msg.pop<FieldState>();

  markResultFunc(result, row, column);
}

void Client::handleServerHandshake(Message &msg) {
  auto id = msg.pop<boost::uuids::uuid>();
  spdlog::info("[Client] got id {} from the server", boost::uuids::to_string(id));
  this->id = id;
}

void Client::handleBroadcastingPlayers(Message &msg) {
  auto name1 = msg.pop<PlayerNameMessage>();
  auto id1 = msg.pop<boost::uuids::uuid>();
  auto name2 = msg.pop<PlayerNameMessage>();
  auto id2 = msg.pop<boost::uuids::uuid>();

  // we are the first player
  if (id1 == this->id) {
    enemyName = std::string(name2.name);
    enemyId = id2;
  } else { // we are the second player
    enemyName = std::string(name1.name);
    enemyId = id1;
  }
}

void Client::handleGameEnd(Message &msg) {
  auto loserId = msg.pop<boost::uuids::uuid>();

  if (loserId == id) {
    spdlog::info("[Client] we lost!");
    loserName = name;
  } else {
    spdlog::info("[Client] we won!");
    loserName = enemyName;
  }
  disconnect();
}

// === Setters ===

void Client::setRecievingAttackFunc(std::function<FieldState(unsigned short int, unsigned short int)> func) {
  recieveAttackFunc = func;
}

void Client::setMarkResultFunc(std::function<void(FieldState, unsigned short int, unsigned short int)> func) {
  markResultFunc = func;
}

} // namespace networking
} // namespace battleship

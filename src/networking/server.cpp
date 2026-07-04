#include "server.hpp"
#include "connection.hpp"
#include "data_types.hpp"
#include "messages.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/uuid/uuid.hpp>
#include <exception>
#include <spdlog/spdlog.h>
#include <system_error>
#include <thread>

namespace battleship {
namespace networking {
Server::Server(uint16_t port)
    : acceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

Server::~Server() {
  stop();
}

bool Server::start() {
  try {
    waitForConnection();
    thread = std::thread([this]() { context.run(); });
  } catch (std::exception &e) {
    spdlog::error("[Server] Exception: {}", e.what());
    return false;
  }
  spdlog::info("[Server] started");
  return true;
}

void Server::stop() {
  acceptor.close();

  context.stop();

  if (thread.joinable()) {
    if (thread.get_id() != std::this_thread::get_id()) {
      thread.join();
    } else {
      thread.detach();
    }
  }
  playerList.clear();

  spdlog::info("[Server] Stopped!");
}

void Server::waitForConnection() {
  acceptor.async_accept([this](std::error_code ec, boost::asio::ip::tcp::socket socket) {
    if (ec) {
      spdlog::error("[Server] New connection errror: {}", ec.message());
      return;
    }

    spdlog::info("[Server] New connection: {}", boost::lexical_cast<std::string>(socket.remote_endpoint()));

    std::shared_ptr<Connection> conn =
        std::make_shared<Connection>(Connection::Owner::SERVER, context, std::move(socket), queIn);

    conn->onDisconnect = [this](auto c) { this->onClientDisconnect(c); };

    if (!onClientConnect(conn)) {
      spdlog::warn("[Server] New connection refused!");

      waitForConnection();
      return;
    }

    auto newPlayer = std::make_shared<NetworkPlayer>(conn);
    playerList.push(newPlayer);
    conn->startListening();
    spdlog::info("[Server] Connection approved!");

    Message msg;
    msg.header.id = MessageType::SERVER_REQUEST_HANDSHAKE;
    messageClient(newPlayer, msg);

    waitForConnection();
  });
}

void Server::messageClient(std::shared_ptr<NetworkPlayer> player, const Message &msg) {
  if (!player->connection || !player->connection->isConnected()) {
    onClientDisconnect(player->connection);
    return;
  }

  player->connection->send(msg);
}

void Server::broadcast(const Message &msg) {
  for (const auto &player : playerList.getPlayers()) {
    messageClient(player, msg);
  }
}

void Server::update(size_t maxMessages, bool wait) {
  if (wait) {
    queIn.wait();
  }

  size_t messageCount = 0;
  while (messageCount < maxMessages && !queIn.empty()) {
    auto msg = queIn.pop_front();
    onMessage(msg.remote, msg.msg);
    messageCount++;
  }
}

bool Server::isGameEnd() const {
  return playerList.isEmpty() && globalGameStatus == GameStatus::GAME_FINISH;
}

// === Event handlers ===
bool Server::onClientConnect(std::shared_ptr<Connection> client) {
  if (playerList.isFull()) {
    return false;
  }
  return true;
}

void Server::onClientDisconnect(std::shared_ptr<Connection> client) {
  auto player = playerList.getPlayerById(client->getId());
  if (!player) {
    return;
  }

  player->connection.reset();
  playerList.remove(player);

  if (isGameEnd()) {
    stop();
    Message msg;
    msg.header.id = MessageType::SERVER_GAME_END;
    queIn.push_front({nullptr, msg});
  }
}

void Server::onMessage(std::shared_ptr<Connection> client, Message &msg) {
  switch (msg.header.id) {
  case MessageType::CLIENT_HANDSHAKE:
    handleHandshake(client, msg);
    break;
  case MessageType::CLIENT_GAME_STATUS:
    handleGameStatusChange(client, msg);
    break;
  case MessageType::CLIENT_RECEIVE_ATTACK:
    handleClientRecievingAttack(client, msg);
    break;
  case MessageType::CLIENT_SEND_ATTACK:
    handleClientSendingAttack(client, msg);
    break;
  case MessageType::CLIENT_LOST:
    handleGameEnd(client, msg);
    break;
  default:
    break;
  }
}

void Server::handleHandshake(std::shared_ptr<Connection> client, Message &msg) {
  auto newtworkPlayer = playerList.getPlayerById(client->getId());
  if (!newtworkPlayer) {
    spdlog::error("[Server] player not on playerlist sent connnection status!");
    return;
  }
  auto name = msg.pop<PlayerNameMessage>();
  newtworkPlayer->name = std::string(name.name);
  spdlog::info("[Server] >>> Player {} sent a handshake :D (ID: {})",
               newtworkPlayer->name.value(),
               boost::lexical_cast<std::string>(newtworkPlayer->connection->getId()));
  Message newMsg;
  newMsg.header.id = MessageType::SERVER_HANDSHAKE;
  newMsg.push(newtworkPlayer->getId());
  messageClient(newtworkPlayer, newMsg);

  if (playerList.isLobbyReady()) {
    handleGameBeginning();
  }
}

void Server::handleGameBeginning() {
  Message msg;
  msg.header.id = MessageType::SERVER_BROADCAST_PLAYERS;
  auto player1 = playerList.getCurrentTurn();
  auto player1Id = player1->getId();
  PlayerNameMessage player1Name;
  std::strncpy(player1Name.name, player1->name->c_str(), sizeof(player1Name.name) - 1);

  auto player2 = playerList.getPassivePlayer();
  auto player2Id = player2->getId();
  PlayerNameMessage player2Name;
  std::strncpy(player2Name.name, player2->name->c_str(), sizeof(player2Name.name) - 1);

  msg.push(player1Id);
  msg.push(player1Name);
  msg.push(player2Id);
  msg.push(player2Name);
  broadcast(msg);
}

void Server::handleGameStatusChange(std::shared_ptr<Connection> client, Message &msg) {
  auto networkPlayer = playerList.getPlayerById(client->getId());
  GameStatus status = msg.pop<GameStatus>();
  networkPlayer->currentGameStatus = status;
  spdlog::info("[Server] Player {} sent his new game status is {}",
               boost::uuids::to_string(networkPlayer->getId()),
               static_cast<int>(status));

  auto newGameStatus = playerList.updateGameStatus();
  if (newGameStatus) {
    globalGameStatus = newGameStatus.value();
    spdlog::info("[Server] Global game status was updated to {}. Notifying players...",
                 static_cast<int>(globalGameStatus));

    Message msg;
    msg.header.id = MessageType::SERVER_GAME_STATUS;
    msg.push(globalGameStatus);
    broadcast(msg);
  }
}

void Server::handleClientSendingAttack(std::shared_ptr<Connection> client, Message &msg) {
  auto attacker = playerList.getPlayerById(client->getId());
  if (attacker != playerList.getCurrentTurn()) {
    spdlog::warn("[Server] player whose turn it is not tried to attack. Ignoring...");
    return;
  }

  auto victim = playerList.getPassivePlayer();
  if (!victim) {
    spdlog::error("[Server] victim in sending attack is nullptr...");
    return;
  }
  victim->connection->send(msg);
}

void Server::handleClientRecievingAttack(std::shared_ptr<Connection> client, Message &msg) {
  auto attacker = playerList.getCurrentTurn();
  if (!attacker) {
    spdlog::error("[Server] attacker in recieving attack is nullptr...");
    return;
  }
  attacker->connection->send(msg);
  playerList.switchTurn();
  broadcastCurrentTurn();
}

void Server::handleGameEnd(std::shared_ptr<Connection> client, Message &msg) {
  globalGameStatus = GameStatus::GAME_FINISH;

  Message endMsg;
  auto loser = client->getId();
  spdlog::info("[Server] GAME FINISHED! Broadcasting GAME_END message...");

  endMsg.header.id = MessageType::SERVER_GAME_END;
  endMsg.push(loser);
  broadcast(endMsg);

  spdlog::info("[Server] Players notified. Waiting for players to leave...");
}

void Server::broadcastCurrentTurn() {
  Message msg;
  msg.header.id = MessageType::SERVER_CURRENT_TURN;
  msg.push(playerList.getCurrentTurn()->getId());
  broadcast(msg);
}

} // namespace networking
} // namespace battleship

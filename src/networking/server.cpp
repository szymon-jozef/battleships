#include "server.hpp"
#include "connection.hpp"
#include "messages.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <exception>
#include <spdlog/spdlog.h>
#include <system_error>

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
  context.stop();

  if (thread.joinable()) {
    thread.join();
  }

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

    if (!onClientConnect(conn)) {
      spdlog::warn("[Server] New connection refused!");

      waitForConnection();
      return;
    }

    playerList.push(std::make_shared<NetworkPlayer>("example_name", conn));
    conn->startListening();
    spdlog::info("[Server] Connection approved!");

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

// === Event handlers ===
bool Server::onClientConnect(std::shared_ptr<Connection> client) {
  return true;
}
void Server::onClientDisconnect(std::shared_ptr<Connection> client) {
  auto player = playerList.getPlayerById(client->getId());
  player->connection.reset();
  playerList.remove(player);
}
void Server::onMessage(std::shared_ptr<Connection> client, Message &msg) {
  switch (msg.header.id) {
  case MessageType::CLIENT_CONNECTION_STATUS:
    handleConnectionStatus(client, msg);
    break;
  case MessageType::CLIENT_RECEIVE_ATTACK:
    break;
  case MessageType::CLIENT_GAME_STATUS:
    break;
  case MessageType::CLIENT_SEND_ATTACK:
    break;
  case MessageType::SERVER_GAME_END:
    break;
  case MessageType::SERVER_GAME_STATUS:
    break;
  }
}

void Server::handleConnectionStatus(std::shared_ptr<Connection> client, Message &msg) {
  auto newtworkPlayer = playerList.getPlayerById(client->getId());
  if (!newtworkPlayer) {
    spdlog::error("[Server] player not on playerlist sent connnection status!");
    return;
  }
  auto name = msg.body.pop<PlayerNameMessage>();
  newtworkPlayer->name = std::string(name.name);
}

} // namespace networking
} // namespace battleship

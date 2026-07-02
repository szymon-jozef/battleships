#include "client.hpp"
#include "data_types.hpp"
#include <boost/asio/ip/basic_resolver.hpp>
#include <exception>
#include <memory>
#include <spdlog/spdlog.h>

namespace battleship {
namespace networking {
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
    thread.join();
  }
}

void Client::send(const Message &msg) {
  if (isConnected()) {
    connection->send(msg);
  }
}

void Client::sendHandshake(std::string name) {
  Message msg;
  msg.header.id = MessageType::CLIENT_HANDSHAKE;
  PlayerNameMessage pname;

  std::strncpy(pname.name, name.c_str(), sizeof(pname.name) - 1);
  if (sizeof(pname.name) < name.length()) {
    spdlog::warn("[Client] Given username is longer than {}, so it will be cut short!", sizeof(pname.name));
  }

  msg.body.push(pname);
  send(msg);
}

} // namespace networking
} // namespace battleship

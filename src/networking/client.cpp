#include "client.hpp"
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

} // namespace networking
} // namespace battleship

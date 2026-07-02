#include "connection.hpp"
#include "data_types.hpp"
#include "messages.hpp"
#include <boost/asio/io_context.hpp>
#include <cstdint>
#include <string>
#include <thread>

namespace battleship {
namespace networking {
class Client {
  boost::asio::io_context context;
  std::thread thread;
  std::shared_ptr<Connection> connection;
  MessageQueue queIn;

public:
  Client() = default;
  ~Client();

  bool connect(const std::string &host, const uint16_t port);
  void disconnect();
  bool isConnected() const;
  void send(const Message &msg);
  void sendHandshake(std::string name);
};
} // namespace networking
} // namespace battleship

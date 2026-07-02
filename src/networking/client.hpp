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
  GameStatus currentGameStatus;

public:
  Client() = default;
  ~Client();

  bool connect(const std::string &host, const uint16_t port);
  void disconnect();
  bool isConnected() const;
  void send(const Message &msg);
  void update(size_t maxMessages = -1, bool wait = true);
  void onMessage(Message &msg);

  void sendHandshake(std::string name);
  void sendGameStatus(GameStatus status);
  void sendAttack(unsigned short int row, unsigned short int column);
  void recieveAttack();
};
} // namespace networking
} // namespace battleship

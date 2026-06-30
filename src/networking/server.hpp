#include "data_types.hpp"
#include "messages.hpp"
#include "network_player.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <thread>

namespace battleship {
namespace networking {

class Server {
  MessageQueue queIn;
  PlayerList playerList;
  // server has it's own context
  boost::asio::io_context context;
  std::thread thread;
  /// Handles new incoming connections
  boost::asio::ip::tcp::acceptor acceptor;

  /// Stop function is private since server should stop itself, when the game is ended.
  void stop();

  // Event handlers

  /// Allows to veto a connection or do something with it __before__ it's added to playerList
  bool onClientConnect(std::shared_ptr<Connection> client);
  void onClientDisconnect(std::shared_ptr<Connection> client);
  void onMessage(std::shared_ptr<Connection> client, Message &msg);

public:
  Server(uint16_t port);
  ~Server();

  bool start();
  void waitForConnection();
  void messageClient(std::shared_ptr<NetworkPlayer> player, const Message &msg);
  void broadcast(const Message &msg);
  void update(size_t maxMessages = -1,
              bool wait = false); // since size_t is unsigned -1 will be changed to max int value
};
} // namespace networking
} // namespace battleship

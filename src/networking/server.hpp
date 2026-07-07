#include "data_types.hpp"
#include "messages.hpp"
#include "network_player.hpp"
#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
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
  GameStatus globalGameStatus = GameStatus::LOBBY;
  boost::asio::steady_timer shutdownTimer;

  bool isRunning = false;

  // Event handlers

  /// Allows to veto a connection or do something with it __before__ it's added to playerList
  bool onClientConnect(std::shared_ptr<Connection> client);
  void onClientDisconnect(std::shared_ptr<Connection> client);
  void onMessage(std::shared_ptr<Connection> client, Message &msg);

  /// @brief Set player name
  void handleHandshake(std::shared_ptr<Connection> client, Message &msg);
  /// @brief Receives new game status from the player, updates global and notifies players about it
  void handleGameStatusChange(std::shared_ptr<Connection> client, Message &msg);
  /// @brief Validate message and route it
  void handleClientSendingAttack(std::shared_ptr<Connection> client, Message &msg);
  /// @brief Validate message and route it
  void handleClientRecievingAttack(std::shared_ptr<Connection> client, Message &msg);
  void broadcastCurrentTurn();
  /// @brief Broadcasts players
  void handleGameBeginning();
  /// @brief Broadcasts that one player has lost the game and closes up the server
  void handleGameEnd(std::shared_ptr<Connection> client, Message &msg);
  void waitForConnection();
  void messageClient(std::shared_ptr<NetworkPlayer> player, const Message &msg);
  void broadcast(const Message &msg);

public:
  Server(uint16_t port);
  ~Server();

  bool start();
  void stop();
  void update(size_t maxMessages = -1,
              bool wait = true); // since size_t is unsigned -1 will be changed to max int value
  bool isGameEnd() const;
  bool isServerRunning() const;
};
} // namespace networking
} // namespace battleship

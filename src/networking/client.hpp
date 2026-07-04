#include "../game_logic/logic_models.hpp"
#include "connection.hpp"
#include "data_types.hpp"
#include "messages.hpp"
#include <boost/asio/io_context.hpp>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>

namespace battleship {
namespace networking {
class Client {
  boost::asio::io_context context;

  std::thread thread;
  std::shared_ptr<Connection> connection;
  MessageQueue queIn;

  std::function<battleship::logic::FieldState(unsigned short int, unsigned short int)> recieveAttackFunc;
  std::function<void(battleship::logic::FieldState, unsigned short int, unsigned short int)> markResultFunc;

  void send(const Message &msg);
  void onMessage(Message &msg);

  void handleIncomingAttack(Message &msg);
  void handleShotResult(Message &msg);
  void handleServerHandshake(Message &msg);
  void handleBroadcastingPlayers(Message &msg);
  void handleGameEnd(Message &msg);

public:
  boost::uuids::uuid id;
  std::string name;
  boost::uuids::uuid enemyId;
  std::string enemyName;

  // TODO! Change  player and enemy id,name to some more sensible data structure
  std::string loserName;

  GameStatus currentGameStatus;

  Client() = default;
  ~Client();

  bool connect(const std::string &host, const uint16_t port);
  void disconnect();
  bool isConnected() const;
  void update(size_t maxMessages = -1, bool wait = true);

  void sendHandshake(std::string name);
  void sendGameStatus(GameStatus status);
  void sendAttack(unsigned short int row, unsigned short int column);
  void sendLost();

  /// @brief Function that handles being attacked by the other player. It takes a function from game_logic that should
  /// return the state of attacked field. This __needs__ to be set
  void
  setRecievingAttackFunc(std::function<battleship::logic::FieldState(unsigned short int, unsigned short int)> func);
  /// @brief Function that handles marking player radar.
  void
  setMarkResultFunc(std::function<void(battleship::logic::FieldState, unsigned short int, unsigned short int)> func);
};
} // namespace networking
} // namespace battleship

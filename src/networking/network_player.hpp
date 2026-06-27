#include "connection.hpp"
#include <boost/uuid.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace battleship {
namespace networking {

/// @brief Class made for the server to store it's players
struct NetworkPlayer {
  std::string name;
  std::shared_ptr<Connection> connection;

  NetworkPlayer(std::string name, std::shared_ptr<Connection> connection)
      : name(name)
      , connection(connection) {}

  boost::uuids::uuid getId() const {
    return connection->getId();
  }
};

/// @brief Container for storing players
class PlayerList {
  std::mutex mutex;
  std::array<std::shared_ptr<NetworkPlayer>, 2> playerList;
  std::shared_ptr<NetworkPlayer> currentTurn = nullptr;
  uint8_t size = 0;

public:
  PlayerList() = default;
  ~PlayerList() {
    clear();
  }

  /// @brief Add a player to the list
  void push(std::shared_ptr<NetworkPlayer> player) {
    std::scoped_lock lock(mutex);
    if (size >= 2) {
      spdlog::error("[Network] Tried to push into playerList but it was full!");
      return;
    }
    playerList[size++] = player;

    // if currentTurn wasn't set yet we default it to the first connected player
    currentTurn = currentTurn == nullptr ? player : currentTurn;
  }

  /// @brief Remove every player from the list
  void clear() {
    std::scoped_lock lock(mutex);
    for (int i = 0; i < size; i++) {
      playerList[i] = nullptr;
    }
    size = 0;
  }

  void switchTurn() {
    std::scoped_lock lock(mutex);
    for (int i = 0; i < size; i++) {
      if (currentTurn != playerList[i]) {
        currentTurn = playerList[i];
        return;
      }
    }
  }

  std::shared_ptr<NetworkPlayer> getCurrentTurn() {
    std::scoped_lock lock(mutex);
    return currentTurn;
  }
};
} // namespace networking
} // namespace battleship

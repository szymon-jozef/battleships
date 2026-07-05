#include "connection.hpp"
#include <boost/uuid.hpp>
#include <memory>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>

namespace battleship {
namespace networking {

/// @brief Class made for the server to store it's players
struct NetworkPlayer {
  std::optional<std::string> name;
  GameStatus currentGameStatus;
  std::shared_ptr<Connection> connection;

  NetworkPlayer(std::shared_ptr<Connection> connection)
      : connection(connection) {}

  boost::uuids::uuid getId() const {
    return connection->getId();
  }
};

/// @brief Container for storing players
class PlayerList {
  mutable std::mutex mutex;
  std::vector<std::shared_ptr<NetworkPlayer>> playerList;
  std::shared_ptr<NetworkPlayer> currentTurn = nullptr;

public:
  PlayerList() = default;
  ~PlayerList() {
    clear();
  }

  /// @brief Add a player to the list
  void push(std::shared_ptr<NetworkPlayer> player) {
    std::scoped_lock lock(mutex);
    if (playerList.size() >= 2) {
      spdlog::error("[Network] Tried to push into playerList but it was full!");
      return;
    }
    playerList.push_back(player);

    // if currentTurn wasn't set yet we default it to the first connected player
    currentTurn = currentTurn == nullptr ? player : currentTurn;
  }

  /// @brief Remove every player from the list
  void clear() {
    std::scoped_lock lock(mutex);
    playerList.clear();
    currentTurn = nullptr;
  }

  void remove(std::shared_ptr<NetworkPlayer> player) {
    std::scoped_lock lock(mutex);
    std::erase(playerList, player);

    if (currentTurn == player) {
      currentTurn = playerList.empty() ? nullptr : playerList[0];
    }
  }

  void switchTurn() {
    std::scoped_lock lock(mutex);
    for (auto &player : playerList) {
      if (currentTurn != player) {
        currentTurn = player;
        return;
      }
    }
  }

  std::shared_ptr<NetworkPlayer> getCurrentTurn() {
    std::scoped_lock lock(mutex);
    return currentTurn;
  }

  std::shared_ptr<NetworkPlayer> getPassivePlayer() {
    std::scoped_lock lock(mutex);
    for (auto &player : playerList) {
      if (player != currentTurn) {
        return player;
      }
    }
    return nullptr;
  }

  std::vector<std::shared_ptr<NetworkPlayer>> getPlayers() const {
    std::scoped_lock lock(mutex);
    return playerList;
  }

  std::shared_ptr<NetworkPlayer> getPlayerById(boost::uuids::uuid id) const {
    std::scoped_lock lock(mutex);
    for (auto &player : playerList) {
      if (player->connection->getId() == id) {
        return player;
      }
    }
    return nullptr;
  }

  /// @brief Check if every player has the same gameStatus and act accordingly
  /// @return GameStatus::PLACING_SHIPS when every player is ready, etc.
  std::optional<GameStatus> updateGameStatus() {
    std::scoped_lock lock(mutex);

    if (playerList.size() != 2) {
      return std::nullopt;
    }

    if (playerList[0]->currentGameStatus == playerList[1]->currentGameStatus) {
      switch (playerList[0]->currentGameStatus) {
      case GameStatus::LOBBY:
        return GameStatus::PLACING_SHIPS;
      case GameStatus::WAR:
        return GameStatus::WAR;
      default:
        return std::nullopt;
      }
    }
    return std::nullopt;
  }

  bool isLobbyReady() const {
    std::scoped_lock lock(mutex);
    return playerList.size() >= 2 && playerList[0]->name.has_value() && playerList[1]->name.has_value();
  }

  bool isFull() const {
    std::scoped_lock lock(mutex);
    return playerList.size() >= 2;
  }

  bool isEmpty() const {
    std::scoped_lock lock(mutex);
    return playerList.size() <= 0;
  }
};
} // namespace networking
} // namespace battleship

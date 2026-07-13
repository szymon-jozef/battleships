#include "game_manager.hpp"
#include "data_types.hpp"
#include <../game_logic/logic_models.hpp>
#include <cstdint>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gameManager {

// TODO! Server should verify that both players boards are of the same size.
GameManager::GameManager(const std::string &playerName, const std::string &serverUrl, const uint16_t serverPort)
    : player(playerName)
    , serverUrl(serverUrl)
    , serverPort(serverPort) {
  spdlog::info("[GameManager] constructor was run");

  client.setRecievingAttackFunc(
      [this](unsigned short int row, unsigned short int column) { return this->player.recieveShot(row, column); });

  client.setMarkResultFunc([this](logic::FieldState state, unsigned short int row, unsigned short int column) {
    return this->player.markShotResult(state, row, column);
  });

  client.setOnShotRecieve([this]() {
    if (player.isDead()) {
      client.sendLost();
      return true;
    }
    return false;
  });
}

bool GameManager::connect(std::function<void(bool, std::string)> onResult) {
  return (client.connect(player.getName(), serverUrl, serverPort, onResult));
}

void GameManager::disconnect() {
  client.disconnect();
}

void GameManager::updateClient(bool wait) {
  client.update(-1, wait);
}

bool GameManager::placeShip(unsigned short int row, unsigned short int column, bool isHorizontal) {
  bool result = player.placeShip(row, column, isHorizontal);
  if (!result) {
    return false;
  }

  if (!player.hasShips()) {
    client.sendGameStatus(networking::GameStatus::WAR);
  }

  return true;
}

// TODO! Change client so it puts FieldStates on some kind of queue, so we can play sounds, etc... (GUI_EVENTS)

void GameManager::makeShot(unsigned short int row, unsigned short int column) {
  if (client.isMyTurn) {
    client.sendAttack(row, column);
  }
}

networking::GameStatus GameManager::getCurrentGameStatus() const {
  return client.currentGameStatus;
}

logic::FieldState GameManager::getBoardField(unsigned short int row, unsigned short int column) {
  return player.getBoardState(row, column);
}

logic::FieldState GameManager::getRadarField(unsigned short int row, unsigned short int column) {
  return player.getRadarState(row, column);
}

std::string GameManager::getPlayerName() const {
  return client.name;
}

std::string GameManager::getEnemyName() const {
  return client.enemyName;
}

std::string GameManager::getLoserName() const {
  return client.loserName;
}

bool GameManager::isGameWon() const {
  return client.isGameWon;
}

bool GameManager::isConnected() const {
  return client.isConnected();
}

unsigned short int GameManager::getBoardWidth() const {
  return player.getBoardWidth();
}

unsigned short int GameManager::getBoardHeight() const {
  return player.getBoardHeight();
}

std::optional<logic::ShipType> GameManager::getCurrentShip() {
  return player.getShipType();
}

bool GameManager::isMyTurn() {
  return client.isMyTurn;
}

} // namespace gameManager
} // namespace battleship

#include "game_manager.hpp"
#include <../game_logic/logic_models.hpp>
#include <cstdint>

namespace battleship {
namespace gameManager {

// TODO! Server should verify that both players boards are of the same size.
GameManager::GameManager(std::string playerName, const std::string &serverUrl, const uint16_t serverPort)
    : player(playerName)
    , serverUrl(serverUrl)
    , serverPort(serverPort) {
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

void GameManager::connect() {
  client.connect(player.getName(), serverUrl, serverPort);
}

void GameManager::updateClient() {
  client.update();
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
  // TODO! Server should send to the clients which turn it is and client shouldn't be able to shoot when it's not its
  // turn
  client.sendAttack(row, column);
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

bool GameManager::isGameWon() {
  return client.enemyName == client.loserName;
}
} // namespace gameManager
} // namespace battleship

#pragma once
#include <../game_logic/logic_models.hpp>
#include <../networking/client.hpp>
#include <cstdint>

namespace battleship {
namespace gameManager {

/// @brief Class that's the main abstraction layer over network client and game logic.
class GameManager {
  battleship::networking::Client client;
  battleship::logic::Player player;

  const std::string serverUrl;
  const uint16_t serverPort;

public:
  GameManager(const std::string &playerName, const std::string &serverUrl, const uint16_t serverPort = 6767);

  void connect();
  void disconnect();
  void updateClient(bool wait = true);
  bool placeShip(unsigned short int row, unsigned short int column, bool isHorizontal);
  void makeShot(unsigned short int row, unsigned short int column);

  networking::GameStatus getCurrentGameStatus() const;
  logic::FieldState getBoardField(unsigned short int row, unsigned short int column);
  logic::FieldState getRadarField(unsigned short int row, unsigned short int column);

  std::string getPlayerName() const;
  std::string getEnemyName() const;
  std::string getLoserName() const;

  bool isGameWon();
};
} // namespace gameManager
} // namespace battleship

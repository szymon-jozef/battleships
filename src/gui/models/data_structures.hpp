#pragma once
#include <cstdint>
#include <string>

namespace battleship {
namespace gui {

enum class GuiState {
  MAIN_MENU,
  MODE_SELECTION,
  GAME,
  JOIN_SERVER,
  SETTINGS,
  QUIT,
};

struct GameContext {
  std::string playerName;
  GuiState guiState = GuiState::MAIN_MENU;

  std::string serverUrl = "127.0.0.1";
  uint16_t serverPort = 6767;

  enum class GameMode { HOSTING, JOINING };
  GameMode currentGameMode = GameMode::HOSTING;
};

} // namespace gui
} // namespace battleship

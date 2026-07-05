#pragma once

#include <string>

namespace battleship {
namespace gui {

enum class GuiState {
  MAIN_MENU,
  SETTINGS,
};

struct GameContext {
  std::string playerName;
  GuiState guiState;
};

} // namespace gui
} // namespace battleship

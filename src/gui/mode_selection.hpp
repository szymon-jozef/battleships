#pragma once
#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class ModeSelection : public Scene {
  WidgetsVector widgets;
  /// @brief Go back to the main menu screen
  void goBack();

public:
  ModeSelection(GameContext &gameContext, Texture2D &background);
  ~ModeSelection();

  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

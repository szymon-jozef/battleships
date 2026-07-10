#pragma once
#include "../../models/gui_models.hpp"
#include "../scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  WidgetsVector widgets;

public:
  Settings(GameContext &gameContext, Texture2D &background);
  ~Settings() = default;

  void update() override;
  void draw() override;
};
} // namespace gui
} // namespace battleship

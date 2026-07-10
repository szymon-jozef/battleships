#pragma once

#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {
class MainMenu : public Scene {
  WidgetsVector widgets;

public:
  MainMenu(GameContext &gameContext, Texture2D &background);
  ~MainMenu();

  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

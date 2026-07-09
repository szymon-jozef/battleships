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
  MainMenu(GameContext &gameContext, Texture2D &background)
      : Scene(gameContext, background)
      , widgets(gameContext, 0.2f, 0.02f, 0.5f, 0.1f) {
    widgets.push_back_button("Play", [&gameContext]() { gameContext.guiState = GuiState::MODE_SELECTION; });
    widgets.push_back_button("Settings", [&gameContext]() { gameContext.guiState = GuiState::SETTINGS; });
    widgets.push_back_button("Quit", [&gameContext]() { gameContext.guiState = GuiState::QUIT; });
  }

  ~MainMenu() {}

  void update() override {
    Scene::update();
    widgets.update_all();
  }

  void draw() override {
    Scene::draw();
    widgets.draw_all();
  }
};

} // namespace gui
} // namespace battleship

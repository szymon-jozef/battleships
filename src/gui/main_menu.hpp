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
  MainMenu(GameContext &gameContext)
      : Scene(gameContext)
      , widgets(gameContext, 10, 10, GetScreenWidth() / 3.0f, 20, 12) {
    background = LoadTexture("assets/gfx/bg1.jpg");

    widgets.push_back_button("Play", [&gameContext]() { gameContext.guiState = GuiState::MODE_SELECTION; });
    widgets.push_back_button("Settings", [&gameContext]() { gameContext.guiState = GuiState::SETTINGS; });
    widgets.push_back_button("Quit", [&gameContext]() { gameContext.guiState = GuiState::QUIT; });
  }

  ~MainMenu() {
    UnloadTexture(background);
  }

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

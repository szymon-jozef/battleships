#pragma once
#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  WidgetsVector widgets;

public:
  Settings(GameContext &gameContext, Texture2D &background)
      : Scene(gameContext, background)
      , widgets(gameContext, 0.1, 0.1, 0.4f, 0.1) {
    backgroundTint = LIGHTGRAY;

    widgets.push_back_label("Player options", BLACK);
    widgets.push_back_textInput_with_label("Player name: ", gameContext.playerName, BLACK, gameContext.playerName);
    widgets.push_back_button("Go back", [&gameContext]() { gameContext.guiState = GuiState::MAIN_MENU; });
  }

  ~Settings() {}

  void update() override {
    Scene::update();
    if (GetKeyPressed() == KEY_ESCAPE) {
      gameContext.guiState = GuiState::MAIN_MENU;
    }
    widgets.update_all();
  }

  void draw() override {
    Scene::draw();
    widgets.draw_all();
  }
};
} // namespace gui
} // namespace battleship

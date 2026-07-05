#pragma once
#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  Texture2D background;
  ButtonVector buttons;

public:
  Settings(GameContext &gameContext)
      : Scene(gameContext)
      , buttons(10, 10, GetScreenWidth() / 3.0f, GetScreenHeight() / 5.0f) {
    background = LoadTexture("assets/gfx/bg1.jpg");
    buttons.push_back("idk", 12, []() {});
    buttons.push_back("Go back", 12, [&]() { gameContext.guiState = GuiState::MAIN_MENU; });
  }

  ~Settings() {
    UnloadTexture(background);
  }

  void update() override {
    if (GetKeyPressed() == KEY_ESCAPE) {
      gameContext.guiState = GuiState::MAIN_MENU;
    }
    buttons.update_all();
  }

  void draw() override {
    DrawTexture(background, 0, 0, LIGHTGRAY);
    buttons.draw_all();
  }
};
} // namespace gui
} // namespace battleship

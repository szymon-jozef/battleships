#pragma once
#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  Texture2D background;

public:
  Settings(GameContext &gameContext)
      : Scene(gameContext) {
    background = LoadTexture("assets/gfx/bg1.jpg");
  }
  ~Settings() {
    UnloadTexture(background);
  }

  void update() override {
    if (GetKeyPressed() == KEY_BACKSPACE) {
      gameContext.guiState = GuiState::MAIN_MENU;
    }
  }

  void draw() override {
    DrawTexture(background, 0, 0, BLACK);
  }
};
} // namespace gui
} // namespace battleship

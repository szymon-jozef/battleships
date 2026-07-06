#pragma once
#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  Texture2D background;
  ButtonVector buttons;
  TextInput nameInput;

public:
  Settings(GameContext &gameContext)
      : Scene(gameContext)
      , buttons(10, 10, GetScreenWidth() / 3.0f, GetScreenHeight() / 5.0f, 12)
      , nameInput(
            GetScreenWidth() / 2.0f, 3 * GetScreenHeight() / 5.0f, GetScreenWidth() / 4.0f, 100, "Podaj swe imie", 12) {
    background = LoadTexture("assets/gfx/bg1.jpg");
    buttons.push_back("idk", []() {});
    buttons.push_back("Go back", [&]() { gameContext.guiState = GuiState::MAIN_MENU; });
  }

  ~Settings() {
    UnloadTexture(background);
  }

  void update() override {
    if (GetKeyPressed() == KEY_ESCAPE) {
      gameContext.guiState = GuiState::MAIN_MENU;
    }
    buttons.update_all();
    nameInput.update();
  }

  void draw() override {
    DrawTexture(background, 0, 0, LIGHTGRAY);
    buttons.draw_all();
    nameInput.draw();
  }
};
} // namespace gui
} // namespace battleship

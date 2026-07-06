#pragma once
#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  Texture2D background;
  WidgetsVector widgets;

public:
  Settings(GameContext &gameContext)
      : Scene(gameContext)
      , widgets(10, 10, GetScreenWidth() / 3.0f, GetScreenHeight() / 5.0f, 12) {
    background = LoadTexture("assets/gfx/bg1.jpg");
    widgets.push_back_button("idk", []() { spdlog::info("I was clicked and idk"); });
    widgets.push_back_textInput(gameContext.playerName);
    widgets.push_back_button("Go back", [&gameContext]() { gameContext.guiState = GuiState::MAIN_MENU; });
  }

  ~Settings() {
    UnloadTexture(background);
  }

  void update() override {
    if (GetKeyPressed() == KEY_ESCAPE) {
      gameContext.guiState = GuiState::MAIN_MENU;
    }
    widgets.update_all();
  }

  void draw() override {
    DrawTexture(background, 0, 0, LIGHTGRAY);
    widgets.draw_all();
  }
};
} // namespace gui
} // namespace battleship

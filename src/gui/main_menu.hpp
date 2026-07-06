#pragma once

#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {
class MainMenu : public Scene {
  Texture2D background;
  WidgetsVector widgets;

public:
  MainMenu(GameContext &gameContext)
      : Scene(gameContext)
      , widgets(10, 10, GetScreenWidth() / 3.0f, 20, 12) {
    background = LoadTexture("assets/gfx/bg1.jpg");

    widgets.push_back_button("Play", []() { spdlog::info("I want to play!"); });
    widgets.push_back_button("Settings", [&gameContext]() {
      spdlog::info("I was clicked");
      gameContext.guiState = GuiState::SETTINGS;
    });
    widgets.push_back_button("Quit", [&gameContext]() { gameContext.guiState = GuiState::QUIT; });
  }

  ~MainMenu() {
    UnloadTexture(background);
  }

  void update() override {
    widgets.update_all();
  }

  void draw() override {
    DrawTexture(background, 0, 0, WHITE);
    widgets.draw_all();
  }
};

} // namespace gui
} // namespace battleship

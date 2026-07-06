#pragma once

#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {
class MainMenu : public Scene {
  Texture2D background;
  ButtonVector buttons;

public:
  MainMenu(GameContext &gameContext)
      : Scene(gameContext)
      , buttons(10, 10, GetScreenWidth() / 3.0f, 20, 12) {
    background = LoadTexture("assets/gfx/bg1.jpg");

    buttons.push_back("Play", []() { spdlog::info("I want to play!"); });
    buttons.push_back("Settings", [&gameContext]() {
      spdlog::info("I was clicked");
      gameContext.guiState = GuiState::SETTINGS;
    });
    buttons.push_back("Quit", [&gameContext]() { gameContext.guiState = GuiState::QUIT; });
  }

  ~MainMenu() {
    UnloadTexture(background);
  }

  void update() override {
    buttons.update_all();
  }

  void draw() override {
    DrawTexture(background, 0, 0, WHITE);
    buttons.draw_all();
  }
};

} // namespace gui
} // namespace battleship

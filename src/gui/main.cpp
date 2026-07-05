#include "main_menu.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include <memory>
#include <raylib.h>

using namespace battleship::gui;

int main() {
  const int screenWidth = 800;
  const int screenHeight = 450;
  GameContext gameContext = {std::string("Szymon")};

  InitWindow(screenWidth, screenHeight, "Battleships");
  SetTargetFPS(60);

  GuiState &currentState = gameContext.guiState;
  GuiState previousState = currentState;
  bool shouldClose = false;

  std::unique_ptr<Scene> currentScene;
  currentScene = std::make_unique<MainMenu>(gameContext);

  while (!WindowShouldClose() && !shouldClose) {
    if (currentState != previousState) {
      previousState = currentState;

      switch (currentState) {
      case GuiState::MAIN_MENU:
        currentScene = std::make_unique<MainMenu>(gameContext);
        break;
      case GuiState::SETTINGS:
        currentScene = std::make_unique<Settings>(gameContext);
        break;
      case GuiState::QUIT:
        shouldClose = true;
      }
    }

    currentScene->update();

    BeginDrawing();
    currentScene->draw();
    EndDrawing();
  }

  CloseWindow();
  return 0;
}

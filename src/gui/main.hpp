#include "main_menu.hpp"
#include "mode_selection.hpp"
#include "scene.hpp"
#include "settings.hpp"
#include <memory>
#include <raylib.h>

namespace battleship {
namespace gui {

inline int run() {
  const int screenWidth = 720;
  const int screenHeight = 480;
  GameContext gameContext = {std::string("Szymon")};

  // TODO! Uncomment this when updating elements pos is implemented
  // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(screenWidth, screenHeight, "Battleships");
  SetTargetFPS(60);

  GuiState &currentState = gameContext.guiState;
  GuiState previousState = currentState;
  bool shouldClose = false;

  std::unique_ptr<Scene> currentScene;
  currentScene = std::make_unique<MainMenu>(gameContext);

  while (!WindowShouldClose() && !shouldClose) {
    // === Switching or dominating ===
    if (currentState != previousState) {
      previousState = currentState;

      switch (currentState) {
      case GuiState::MAIN_MENU:
        currentScene = std::make_unique<MainMenu>(gameContext);
        break;
      case GuiState::MODE_SELECTION:
        currentScene = std::make_unique<ModeSelection>(gameContext);
        break;
      case GuiState::SETTINGS:
        currentScene = std::make_unique<Settings>(gameContext);
        break;
      case GuiState::QUIT:
        shouldClose = true;
      }
    }

    // === Updating ===
    currentScene->update();

    // === Drawing ===
    BeginDrawing();
    ClearBackground(BLACK);

    currentScene->draw();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

} // namespace gui
} // namespace battleship

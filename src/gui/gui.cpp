#include "gui.hpp"
#include "scenes/all_scenes.hpp"
#include "scenes/game_finish/game_finish.hpp"
#include <memory>
#include <raylib.h>

namespace battleship {
namespace gui {

int run() {
  const int screenWidth = 1220;
  const int screenHeight = 720;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Battleships");
  // MaximizeWindow();
  SetWindowMinSize(GetScreenWidth() * 0.3f, GetScreenHeight() * 0.3f);
  // SetTargetFPS(60);

  GameContext gameContext = GameContext();

  std::atomic<GuiState> &currentState = gameContext.guiState;
  GuiState previousState = currentState;
  bool shouldClose = false;

  std::unique_ptr<Scene> currentScene;
  currentScene = std::make_unique<MainMenu>(gameContext, gameContext.assetsManager.bg1);

  while (!WindowShouldClose() && !shouldClose) {
    // === Switching or dominating ===
    if (currentState != previousState) {
      previousState = currentState;

      switch (currentState) {
      case GuiState::MAIN_MENU:
        currentScene = std::make_unique<MainMenu>(gameContext, gameContext.assetsManager.bg1);
        break;
      case GuiState::MODE_SELECTION:
        currentScene = std::make_unique<ModeSelection>(gameContext, gameContext.assetsManager.bg2);
        break;
      case GuiState::GAME:
        currentScene = std::make_unique<Game>(gameContext, gameContext.assetsManager.playBackground);
        break;
      case GuiState::JOIN_SERVER:
        currentScene = std::make_unique<JoinServer>(gameContext, gameContext.assetsManager.bg2);
        break;
      case GuiState::GAME_FINISH:
        currentScene = std::make_unique<GameFinish>(gameContext, gameContext.assetsManager.bg3);
        break;
      case GuiState::SETTINGS:
        currentScene = std::make_unique<Settings>(gameContext, gameContext.assetsManager.bg1);
        break;
      case GuiState::QUIT:
        shouldClose = true;
        break;
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

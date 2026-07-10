#include "scenes/all_scenes.hpp"
#include "scenes/game_finish/game_finish.hpp"
#include <memory>
#include <raylib.h>

namespace battleship {
namespace gui {

struct Backgrounds {
  Texture2D bg1;
  Texture2D bg2;
  Texture2D bg3;
  Texture2D playBackground;

  Backgrounds()
      : bg1(LoadTexture("assets/gfx/bg1.jpg"))
      , bg2(LoadTexture("assets/gfx/bg2.jpg"))
      , bg3(LoadTexture("assets/gfx/bg3.jpg"))
      , playBackground(LoadTexture("assets/gfx/play_background.jpg")) {}

  ~Backgrounds() {
    UnloadTexture(bg1);
    UnloadTexture(bg2);
    UnloadTexture(bg3);
    UnloadTexture(playBackground);
  }
};

int run() {
  const int screenWidth = 1220;
  const int screenHeight = 720;
  GameContext gameContext = {std::string("Szymon")};

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Battleships");
  // SetTargetFPS(60);

  GuiState &currentState = gameContext.guiState;
  GuiState previousState = currentState;
  bool shouldClose = false;

  Backgrounds bg;

  std::unique_ptr<Scene> currentScene;
  currentScene = std::make_unique<MainMenu>(gameContext, bg.bg1);

  while (!WindowShouldClose() && !shouldClose) {
    // === Switching or dominating ===
    if (currentState != previousState) {
      previousState = currentState;

      switch (currentState) {
      case GuiState::MAIN_MENU:
        currentScene = std::make_unique<MainMenu>(gameContext, bg.bg1);
        break;
      case GuiState::MODE_SELECTION:
        currentScene = std::make_unique<ModeSelection>(gameContext, bg.bg2);
        break;
      case GuiState::GAME:
        currentScene = std::make_unique<Game>(gameContext, bg.playBackground);
        break;
      case GuiState::GAME_FINISH:
        currentScene = std::make_unique<GameFinish>(gameContext, bg.bg3);
        break;
      case GuiState::SETTINGS:
        currentScene = std::make_unique<Settings>(gameContext, bg.bg1);
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

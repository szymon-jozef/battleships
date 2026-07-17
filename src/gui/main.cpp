#include "scenes/all_scenes.hpp"
#include "scenes/game_finish/game_finish.hpp"
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <raylib.h>

namespace battleship {
namespace gui {

class assetsManager {
  std::vector<std::filesystem::path> expectedPaths = {std::filesystem::path("/usr/share/battleship"),
                                                      std::filesystem::path(GetApplicationDirectory()) /
                                                          std::filesystem::path("./assets"),
                                                      std::filesystem::path("./assets")};
  std::filesystem::path pathPrefix;

  // we first change to string then to c_str, because windows is bad
  void loadPaths() {
    bg1 = LoadTexture(std::filesystem::path(pathPrefix / std::filesystem::path("gfx/bg1.jpg")).string().c_str());
    bg2 = LoadTexture(std::filesystem::path(pathPrefix / std::filesystem::path("gfx/bg2.jpg")).string().c_str());
    bg3 = LoadTexture(std::filesystem::path(pathPrefix / std::filesystem::path("gfx/bg3.jpg")).string().c_str());
    playBackground = LoadTexture(
        std::filesystem::path(pathPrefix / std::filesystem::path("gfx/play_background.jpg")).string().c_str());
  }

public:
  Texture2D bg1;
  Texture2D bg2;
  Texture2D bg3;
  Texture2D playBackground;

  assetsManager() {
    char *envPath = std::getenv("BATTLESHIPS_ASSETS_DIR");

    if (envPath) {
      pathPrefix = std::filesystem::path(envPath);
      spdlog::info("[GUI] Env variable BATTLESHIPS_ASSETS_DIR was set. Setting pathPrefix as: {}", envPath);
      loadPaths();
      return;
    }

    for (const auto &path : expectedPaths) {
      if (std::filesystem::is_directory(path)) {
        pathPrefix = std::filesystem::absolute(path);
        spdlog::info("[GUI] found assets path at: {}", pathPrefix.string());
        break;
      }
    }
    if (!pathPrefix.empty()) {
      loadPaths();
      return;
    }

    spdlog::warn("[GUI] could not fine any viable asset directory");
  }

  ~assetsManager() {
    UnloadTexture(bg1);
    UnloadTexture(bg2);
    UnloadTexture(bg3);
    UnloadTexture(playBackground);
  }
};

int run() {
  const int screenWidth = 1220;
  const int screenHeight = 720;
  GameContext gameContext;
  gameContext.settings.load();

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
  InitWindow(screenWidth, screenHeight, "Battleships");
  // MaximizeWindow();
  SetWindowMinSize(GetScreenWidth() * 0.3f, GetScreenHeight() * 0.3f);
  // SetTargetFPS(60);

  std::atomic<GuiState> &currentState = gameContext.guiState;
  GuiState previousState = currentState;
  bool shouldClose = false;

  assetsManager bg;

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
      case GuiState::JOIN_SERVER:
        currentScene = std::make_unique<JoinServer>(gameContext, bg.bg2);
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

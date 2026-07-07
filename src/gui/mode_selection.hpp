#include "gui_models.hpp"
#include "scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class ModeSelection : public Scene {
  Texture2D background;
  WidgetsVector widgets;

  void goBack() {
    gameContext.guiState = GuiState::MAIN_MENU;
  }

public:
  ModeSelection(GameContext &gameContext)
      : Scene(gameContext)
      , widgets(gameContext, 10, 10, GetScreenWidth() / 3.0f, 20, 12) {
    background = LoadTexture("assets/gfx/bg2.jpg");

    widgets.push_back_button("Host Game", [&gameContext]() {
      // TODO! all server enabling
      gameContext.currentGameMode = GameContext::GameMode::HOSTING;
      gameContext.guiState = GuiState::GAME;
    });

    widgets.push_back_button("Join Game", [&gameContext]() {
      gameContext.currentGameMode = GameContext::GameMode::JOINING;
      gameContext.guiState = GuiState::JOIN_SERVER;
    });
    widgets.push_back_button("Go back", [this]() { goBack(); });
  }

  ~ModeSelection() {
    UnloadTexture(background);
  }

  void update() override {
    if (IsKeyPressed(KEY_ESCAPE)) {
      goBack();
    }

    widgets.update_all();
  }

  void draw() override {
    DrawTexture(background, 0, 0, GRAY);
    widgets.draw_all();
  }
};

} // namespace gui

} // namespace battleship

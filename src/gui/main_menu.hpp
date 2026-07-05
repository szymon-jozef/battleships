#include "game_context.hpp"
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
      , buttons(10, 10) {
    background = LoadTexture("assets/gfx/bg1.jpg");
    const float width = GetScreenWidth() / 3.0f, height = 20;
    const int fontSize = 12;

    buttons.push_back("Play", width, height, fontSize, []() { spdlog::info("I want to play!"); });
    buttons.push_back("Settings", width, height, fontSize, []() { spdlog::info("I was clicked"); });
    buttons.push_back("Quit", width, height, fontSize, []() { CloseWindow(); });
  }

  ~MainMenu() {
    UnloadTexture(background);
  }

  void update() override {
    mousePos = GetMousePosition();
    buttons.update_all();
  }

  void draw() override {
    DrawTexture(background, 0, 0, WHITE);
    buttons.draw_all();
  }
};

} // namespace gui
} // namespace battleship

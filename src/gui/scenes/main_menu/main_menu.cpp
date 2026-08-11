#include "main_menu.hpp"

namespace battleship {
namespace gui {

MainMenu::MainMenu(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.2f, 0.02f, 0.5f, 0.1f) {
  widgets.push_back_button("Play", [this, &gameContext]() {
    onEveryClick();
    gameContext.guiState = GuiState::MODE_SELECTION;
  });

  widgets.push_back_button("Settings", [this, &gameContext]() {
    onEveryClick();
    gameContext.guiState = GuiState::SETTINGS;
  });

  widgets.push_back_button("Game information", [this, &gameContext]() {
    onEveryClick();
    gameContext.guiState = GuiState::INFORMATION;
  });

  widgets.push_back_button("Quit", [this, &gameContext]() {
    onEveryClick();
    gameContext.guiState = GuiState::QUIT;
  });
}

MainMenu::~MainMenu() {}

void MainMenu::update() {
  Scene::update();
  widgets.update_all();
}

void MainMenu::draw() {
  Scene::draw();
  widgets.draw_all();
}

} // namespace gui
} // namespace battleship

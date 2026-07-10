#include "main_menu.hpp"

namespace battleship {
namespace gui {

MainMenu::MainMenu(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.2f, 0.02f, 0.5f, 0.1f) {
  widgets.push_back_button("Play", [&gameContext]() { gameContext.guiState = GuiState::MODE_SELECTION; });
  widgets.push_back_button("Settings", [&gameContext]() { gameContext.guiState = GuiState::SETTINGS; });
  widgets.push_back_button("Quit", [&gameContext]() { gameContext.guiState = GuiState::QUIT; });
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

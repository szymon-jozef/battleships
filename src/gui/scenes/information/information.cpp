#include "information.hpp"

namespace battleship::gui {

Information::Information(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.1, 0.1, 0.4f, 0.1) {
  backgroundTint = DARKGRAY;

  widgets.push_back_label(TextFormat("Battleships version: %s", GAME_VERSION), BLACK);
  widgets.push_back_button("Author: Szymon Jozef", []() { OpenURL("https://github.com/szymon-jozef"); });
  widgets.push_back_button("Source code", []() { OpenURL("https://github.com/szymon-jozef/battleships"); });
  widgets.push_back_button("Send a bug report",
                           []() { OpenURL("https://github.com/szymon-jozef/battleships/issues"); });
  widgets.push_back_button("Go back", [&gameContext]() { gameContext.guiState = GuiState::MAIN_MENU; });
}

void Information::update() {
  Scene::update();
  widgets.update_all();
}

void Information::draw() {
  Scene::draw();
  widgets.draw_all();
}

} // namespace battleship::gui

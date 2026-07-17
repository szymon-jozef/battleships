#include "join_server.hpp"

namespace battleship {
namespace gui {

JoinServer::JoinServer(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.1, 0.1, 0.4, 0.1) {
  widgets.push_back_ipInput(gameContext.settings.serverUrl);
  widgets.push_back_button("Join", [&gameContext]() {
    gameContext.settings.save();
    gameContext.guiState = GuiState::GAME;
  });
  widgets.push_back_button("Go back", [&gameContext]() { gameContext.guiState = GuiState::MAIN_MENU; });
}

void JoinServer::update() {
  Scene::update();
  widgets.update_all();
}

void JoinServer::draw() {
  Scene::draw();
  widgets.draw_all();
}

} // namespace gui
} // namespace battleship

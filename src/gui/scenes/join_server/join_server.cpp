#include "join_server.hpp"

namespace battleship {
namespace gui {

JoinServer::JoinServer(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.1, 0.1, 0.4, 0.1) {
  widgets.push_back_textInput_with_label("Server address", "127.0.0.1", RED, gameContext.settings.serverUrl);
  widgets.push_back_button("Join", [&gameContext]() { gameContext.guiState = GuiState::GAME; });
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

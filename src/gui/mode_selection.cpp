#include "mode_selection.hpp"

namespace battleship {
namespace gui {

ModeSelection::ModeSelection(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.2f, 0.02f, 0.5f, 0.1f) {

  widgets.push_back_button("Host Game", [&gameContext]() {
    gameContext.currentGameMode = GameContext::GameMode::HOSTING;
    gameContext.guiState = GuiState::GAME;
  });

  widgets.push_back_button("Join Game", [&gameContext]() {
    gameContext.currentGameMode = GameContext::GameMode::JOINING;
    // TODO! Add a scene for setting server url
    gameContext.guiState = GuiState::GAME;
  });
  widgets.push_back_button("Go back", [this]() { goBack(); });
}

ModeSelection::~ModeSelection() {}

void ModeSelection::update() {
  Scene::update();
  if (IsKeyPressed(KEY_ESCAPE)) {
    goBack();
  }

  widgets.update_all();
}

void ModeSelection::draw() {
  Scene::draw();
  widgets.draw_all();
}

void ModeSelection::goBack() {
  gameContext.guiState = GuiState::MAIN_MENU;
}

} // namespace gui
} // namespace battleship

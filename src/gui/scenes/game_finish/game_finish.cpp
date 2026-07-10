#include "game_finish.hpp"
#include "../../models/gui_models.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

GameFinish::GameFinish(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.1, 0.1, 0.4, 0.1) {
  const std::string &loserName = gameContext.loserName;
  summaryText = TextFormat("%s lost!!", loserName.c_str());

  // TODO! Perhaps someday add some info about the game?

  if (gameContext.isWon) {
    additionalText = "Congratulations!";
    summaryColor = GREEN;
  } else {
    additionalText = "Better luck next time!";
    summaryColor = RED;
  }
  widgets.push_back_label(summaryText, summaryColor);
  widgets.push_back_label(additionalText, WHITE);
  widgets.push_back_button("Exit", [&gameContext]() { gameContext.guiState = GuiState::MAIN_MENU; });
}

void GameFinish::update() {
  Scene::update();
  widgets.update_all();
}

void GameFinish::draw() {
  Scene::draw();
  widgets.draw_all();
}

} // namespace gui
} // namespace battleship

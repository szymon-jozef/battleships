#include "settings.hpp"

namespace battleship {
namespace gui {

Settings::Settings(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , widgets(gameContext, 0.1, 0.1, 0.4f, 0.1) {
  backgroundTint = LIGHTGRAY;

  widgets.push_back_label("Player options", BLACK);

  widgets.push_back_nameInput(name);
  widgets.push_back_volumeLevelInput((volumeLevel));

  widgets.push_back_button("Go back", [this, &gameContext]() {
    onEveryClick();

    if (name.empty()) {
      spdlog::warn("[GUI] trying to save empty name. That's a nono!");
      return;
    }

    gameContext.gameSettings.setPlayerName(name);
    gameContext.gameSettings.setVolumeLevel(volumeLevel);

    // TODO! Server should not allow connections with nonames
    // Especially since you can just set empty name in the config file
    gameContext.gameSettings.save();
    gameContext.guiState = GuiState::MAIN_MENU;
  });
}

void Settings::update() {
  Scene::update();
  if (GetKeyPressed() == KEY_ESCAPE) {
    gameContext.guiState = GuiState::MAIN_MENU;
  }
  widgets.update_all();
}

void Settings::draw() {
  Scene::draw();
  widgets.draw_all();
}

} // namespace gui
} // namespace battleship

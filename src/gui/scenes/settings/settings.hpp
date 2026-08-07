#pragma once
#include "../../models/gui_models.hpp"
#include "../scene.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Settings : public Scene {
  WidgetsVector widgets;
  std::string name = gameContext.gameSettings.getPlayerName(), volumeLevel = gameContext.gameSettings.getVolumeLevel();

public:
  Settings(GameContext &gameContext, Texture2D &background);
  ~Settings() override = default;

  void update() override;
  void draw() override;
};
} // namespace gui
} // namespace battleship

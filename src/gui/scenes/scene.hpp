#pragma once

#include "../models/gui_models.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Scene {
  void updateSceneRect() {
    sceneRect = {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
  }

protected:
  GameContext &gameContext;
  Texture2D &background;
  Color backgroundTint = WHITE;

  Rectangle sceneRect = {0, 0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight())};
  NPatchInfo patchInfo = {sceneRect, 0, 0};

public:
  Scene(GameContext &gameContext, Texture2D &background);
  virtual ~Scene() = default;

  /// @brief Updates scene data. Calls updateSceneRect which updates sceneRect with current screen size.
  virtual void update();

  /// @brief Draw background image scaled to the window size
  virtual void draw();
};
} // namespace gui
} // namespace battleship

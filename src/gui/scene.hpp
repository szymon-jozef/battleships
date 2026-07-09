#pragma once

#include "gui_models.hpp"
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
  Scene(GameContext &gameContext, Texture2D &background)
      : gameContext(gameContext)
      , background(background) {}
  virtual ~Scene() = default;

  virtual void update() {
    updateSceneRect();
  }

  virtual void draw() {
    DrawTexturePro(background,
                   {0.0f, 0.0f, static_cast<float>(background.width), static_cast<float>(background.height)},
                   sceneRect,
                   {0.0f, 0.0f},
                   0.0f,
                   backgroundTint);
  }
};
} // namespace gui
} // namespace battleship

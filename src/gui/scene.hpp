#pragma once

#include "game_context.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Scene {

protected:
  GameContext &gameContext;
  Vector2 mousePos = {0.0f, 0.0f};

public:
  Scene(GameContext &gameContext)
      : gameContext(gameContext) {}
  virtual ~Scene() = default;

  virtual void update() = 0;
  virtual void draw() = 0;
};

} // namespace gui

} // namespace battleship

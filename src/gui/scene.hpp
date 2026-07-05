#pragma once

#include "gui_models.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class Scene {

protected:
  GameContext &gameContext;

public:
  Scene(GameContext &gameContext)
      : gameContext(gameContext) {}
  virtual ~Scene() = default;

  virtual void update() = 0;
  virtual void draw() = 0;
};

} // namespace gui

} // namespace battleship

#pragma once

#include "../scene.hpp"
#include "models/data_structures.hpp"
#include "models/widgets/widgets_vector/widgets_vector.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class JoinServer : public Scene {
  WidgetsVector widgets;

public:
  JoinServer(GameContext &gameContext, Texture2D &background);
  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

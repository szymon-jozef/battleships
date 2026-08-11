#pragma once

#include "../scene.hpp"
#include "models/widgets/widgets_vector/widgets_vector.hpp"

namespace battleship::gui {

class Information : public Scene {
  WidgetsVector widgets;

public:
  Information(GameContext &gameContext, Texture2D &background);
  ~Information() override = default;

  void update() override;
  void draw() override;
};

} // namespace battleship::gui

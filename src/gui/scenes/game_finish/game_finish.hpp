#pragma once

#include "../../models/gui_models.hpp"
#include "../scene.hpp"
#include "models/widgets/widgets_vector/widgets_vector.hpp"

namespace battleship {
namespace gui {

class GameFinish : public Scene {
  std::string summaryText, additionalText; // TODO! <-- this name really doesn't tell much, but I have no better idea rn
  Color summaryColor;

  WidgetsVector widgets;

  void updatePos();

public:
  GameFinish(GameContext &gameContext, Texture2D &background);
  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

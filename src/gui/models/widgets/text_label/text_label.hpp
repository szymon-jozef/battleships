#pragma once
#include "../widget.hpp"

namespace battleship {
namespace gui {

class TextLabel : public Widget {
  Color color;

public:
  TextLabel(std::string text, float pos_y, Rectangle scaleRect, Color color);

  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

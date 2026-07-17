#pragma once
#include "../widget.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class TextLabel : public Widget {
  Color color;

public:
  TextLabel(std::string text, float pos_y, Rectangle scaleRect, Color color);
  TextLabel(float pos_y, Rectangle scaleRect, Color color);

  void setLabel(std::string label);

  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

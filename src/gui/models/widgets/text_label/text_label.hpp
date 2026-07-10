#pragma once
#include "../widget.hpp"

namespace battleship {
namespace gui {

class TextLabel : public Widget {
  Color color;

public:
  TextLabel(std::string text, float pos_y, Rectangle scaleRect, Color color)
      : Widget(text, pos_y, scaleRect, 0.5f, false)
      , color(color) {}

  void update() override {
    Widget::update();
  }

  void draw() override {
    drawLabelInTheMiddle(color);
    Widget::draw();
  }
};

} // namespace gui
} // namespace battleship

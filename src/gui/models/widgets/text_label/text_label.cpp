#include "text_label.hpp"
#include "../widget.hpp"

namespace battleship {
namespace gui {

TextLabel::TextLabel(std::string text, float pos_y, Rectangle scaleRect, Color color)
    : Widget(text, pos_y, scaleRect, 0.5f, false)
    , color(color) {}

void TextLabel::update() {
  Widget::update();
}

void TextLabel::draw() {
  drawLabel(color);
  Widget::draw();
}

} // namespace gui
} // namespace battleship

#include "text_label.hpp"
#include "../widget.hpp"

namespace battleship {
namespace gui {

TextLabel::TextLabel(const std::string &text, float pos_y, Rectangle scaleRect, Color color)
    : Widget(text, pos_y, scaleRect, 0.5f, false)
    , color(color) {}

TextLabel::TextLabel(float pos_y, Rectangle scaleRect, Color color)
    : Widget("", pos_y, scaleRect, 0.5f, false)
    , color(color) {}

void TextLabel::draw() {
  drawLabel(color);
  Widget::draw();
}

void TextLabel::setInnerLabel(const std::string &label) {
  Widget::setLabel(label);
}

} // namespace gui
} // namespace battleship

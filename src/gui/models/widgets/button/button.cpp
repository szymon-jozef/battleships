#include "button.hpp"

namespace battleship {
namespace gui {

Button::Button(std::string label, float pos_y, Rectangle rect)
    : Widget(label, pos_y, rect, 0.8f) {}

void Button::draw() {
  DrawRectangleRec(finalPositionRect, WHITE);
  drawLabelInTheMiddle(BLACK);
  Widget::draw();
}

void Button::update() {
  Widget::update();
  if (onClick && ((CheckCollisionPointRec(GetMousePosition(), finalPositionRect) &&
                   IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) ||
                  (isFocused && IsKeyPressed(KEY_ENTER)))) {
    onClick();
  }
}

void Button::setOnClick(std::function<void()> onClick) {
  this->onClick = onClick;
}

} // namespace gui
} // namespace battleship

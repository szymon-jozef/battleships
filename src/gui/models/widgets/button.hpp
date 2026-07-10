#include "widget.hpp"
#include <functional>

namespace battleship {
namespace gui {

class Button : public Widget {
  std::function<void()> onClick;

public:
  Button(std::string label, float pos_y, Rectangle rect)
      : Widget(label, pos_y, rect, 0.8f) {}

  void draw() {
    DrawRectangleRec(finalPositionRect, WHITE);
    drawLabelInTheMiddle(BLACK);
    Widget::draw();
  }

  void update() {
    Widget::update();
    if (onClick && ((CheckCollisionPointRec(GetMousePosition(), finalPositionRect) &&
                     IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT)) ||
                    (isFocused && IsKeyPressed(KEY_ENTER)))) {
      onClick();
    }
  }

  void setOnClick(std::function<void()> onClick) {
    this->onClick = onClick;
  }
};

} // namespace gui
} // namespace battleship

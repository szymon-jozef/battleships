#include <functional>
#include <raylib.h>
#include <string>

namespace battleship {
namespace gui {
struct Button {
  std::string label;
  float pos_x, pos_y, width, height;
  int fontSize;
  Rectangle rect;
  std::function<void()> onClick;

  Button(std::string label, float pos_x, float pos_y, float width, float height, int fontSize = 12)
      : label(label)
      , pos_x(pos_x - width / 2)
      , pos_y(pos_y)
      , width(width)
      , height(height)
      , rect({this->pos_x, pos_y, width, height})
      , fontSize(fontSize) {}

  void draw() {
    DrawRectangleRec(rect, WHITE);

    int textWidth = MeasureText(label.c_str(), fontSize);
    float text_x = (pos_x + width / 2.0f) - (textWidth / 2.0f);
    float text_y = (pos_y + height / 2.0f) - (fontSize / 2.0f);

    DrawText(label.c_str(), text_x, text_y, fontSize, BLACK);
  }

  void update() {
    if (CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT) &&
        onClick) {
      onClick();
    }
  }
};

class ButtonVector {
  std::vector<Button> buttons;
  int margin, start_y;

public:
  ButtonVector(int start_y, int margin)
      : start_y(start_y)
      , margin(margin) {}

  void push_back(std::string label, float width, float height, int fontSize, std::function<void()> onClick) {
    Button btn =
        Button(label, GetScreenWidth() / 2.0f, start_y + (margin + height) * buttons.size(), width, height, fontSize);
    btn.onClick = onClick;
    buttons.push_back(btn);
  }

  void update_all() {
    for (auto &btn : buttons) {
      btn.update();
    }
  }

  void draw_all() {
    for (auto &btn : buttons) {
      btn.draw();
    }
  }
};

} // namespace gui
} // namespace battleship

#pragma once

#include <algorithm>
#include <raylib.h>
#include <string>

namespace battleship {
namespace gui {

class Widget {
  Rectangle scaleRect;

  void updateTextPos() {
    fontSize = std::min(finalPositionRect.height, finalPositionRect.width) * fontScale;

    textWidth = MeasureText(label.c_str(), fontSize);
    text_x = (finalPositionRect.x + finalPositionRect.width / 2.0f) - (textWidth / 2.0f);
    text_y = (finalPositionRect.y + finalPositionRect.height / 2.0f) - (fontSize / 2.0f);
  }

  void updatePos() {
    finalPositionRect.width = GetScreenWidth() * scaleRect.width;
    finalPositionRect.height = GetScreenHeight() * scaleRect.height;

    finalPositionRect.x = GetScreenWidth() * scaleRect.x - finalPositionRect.width * scaleRect.x;
  }

protected:
  std::string label;
  float text_x, text_y, textWidth, fontScale;
  int fontSize;

  bool isFocused = false;
  /// @brief Rectangle that's used in calculating necessary space for the widget
  Rectangle finalPositionRect;

  void drawLabelInTheMiddle(Color color) {
    DrawText(label.c_str(), text_x, text_y, fontSize, color);
  }

  void drawTextInTheMiddle(char *text, Color color) {
    DrawText(text, text_x, text_y, fontSize, color);
  }

public:
  bool isFocusable = false;

  Widget(std::string label, float pos_y, Rectangle scaleRect, float fontScale, bool isFocusable = true)
      : label(label)
      , finalPositionRect({1, pos_y, 200, 200})
      , scaleRect(scaleRect)
      , fontScale(fontScale)
      , isFocusable(isFocusable) {
    updateEveryPos(true);
  }
  virtual ~Widget() = default;

  /// @brief Draws focus
  /// NOTE: should be called at the end of each draw function, so it's not covered
  virtual void draw() {
    // focused border
    if (isFocusable && isFocused) {
      DrawRectangleLines(scaleRect.x, scaleRect.y, scaleRect.width, scaleRect.height, GRAY);
    }
  }

  virtual void update() {
    updateEveryPos();
  };

  void updateEveryPos(bool force = false) {
    if (force || IsWindowResized()) {
      updatePos();
      updateTextPos();
    }
  }

  void focus() {
    isFocused = true;
  }

  void unFocus() {
    isFocused = false;
  }

  void setY(float y) {
    finalPositionRect.y = y;
  }

  Rectangle getRect() {
    return finalPositionRect;
  }
};

} // namespace gui
} // namespace battleship

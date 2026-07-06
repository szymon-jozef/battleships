#pragma once
#include <functional>
#include <memory>
#include <raylib.h>
#include <spdlog/spdlog.h>
#include <string>

namespace battleship {
namespace gui {

enum class GuiState {
  MAIN_MENU,
  MODE_SELECTION,
  GAME,
  JOIN_SERVER,
  SETTINGS,
  QUIT,
};

struct GameContext {
  std::string playerName;
  GuiState guiState = GuiState::MAIN_MENU;
};

// === Widgets and stuff ===

class Widget {
protected:
  std::string label;
  float pos_x, pos_y, width, height;
  int fontSize;
  Rectangle rect;

public:
  Widget(std::string label, float pos_x, float pos_y, float width, float height, int fontSize = 12)
      : label(label)
      , pos_x(pos_x - width / 2)
      , pos_y(pos_y)
      , width(width)
      , height(height)
      , rect({this->pos_x, pos_y, width, height})
      , fontSize(fontSize) {}
  virtual ~Widget() = default;

  virtual void draw() = 0;
  virtual void update() = 0;
};

class Button : public Widget {
  std::function<void()> onClick;

public:
  Button(std::string label, float pos_x, float pos_y, float width, float height, int fontSize = 12)
      : Widget(label, pos_x, pos_y, width, height, fontSize) {}

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

  void setOnClick(std::function<void()> onClick) {
    this->onClick = onClick;
  }
};

class TextInput : public Widget {
  // 31 chars, because 32 is the max we can send through the network

  const static int MAX_INPUT_CHARS = 31;
  // TODO! Buffer should have players name ootb
  char buffer[MAX_INPUT_CHARS + 1];
  int letterCount = 0;
  std::string prompt;
  bool isMouseOnText = false;

public:
  TextInput(std::string prompt, float pos_x, float pos_y, float width, float height, int fontSize)
      : Widget(prompt, pos_x, pos_y, width, height) {}

  void update() override {
    isMouseOnText = CheckCollisionPointRec(GetMousePosition(), rect);
    // input

    if (isMouseOnText) {
      SetMouseCursor(MOUSE_CURSOR_IBEAM);
      int key = GetCharPressed();

      while (key > 0) {
        if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS)) {
          buffer[letterCount] = (char)key;
          buffer[letterCount + 1] = '\0';
          letterCount++;
        }
        key = GetCharPressed();
      }

      // delete one char
      if (IsKeyPressed(KEY_BACKSPACE)) {
        letterCount--;
        if (letterCount < 0) {
          letterCount = 0;
        }
        buffer[letterCount] = '\0';
      }

      // delete everything
      if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_U)) ||
          (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_BACKSPACE))) {
        letterCount = 0;
        buffer[letterCount] = '\0';
      }

    } else {
      SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }
  }

  void draw() override {

    DrawRectangleRec(rect, LIGHTGRAY);
    if (isMouseOnText) {
      DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, RED);
    } else {
      DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, DARKGRAY);
    }

    int textWidth = MeasureText(prompt.c_str(), fontSize);
    float text_x = (rect.x + rect.width / 2.0f) - (textWidth / 2.0f);
    float text_y = (rect.y + rect.height / 2.0f) - (fontSize / 2.0f);

    if (prompt != "" && std::string(buffer).empty()) {
      DrawText(prompt.c_str(), text_x, text_y, fontSize, MAROON);
    } else {
      DrawText(buffer, text_x, text_y, fontSize, MAROON);
    }

    DrawText(TextFormat("%i/%i", letterCount, MAX_INPUT_CHARS),
             rect.x + rect.width / 2,
             rect.y + rect.height + 4,
             fontSize / 2,
             DARKGRAY);
  }

  std::string getInput() const {
    return std::string(buffer);
  }
};

/// @brief A "layout manager" if you can call it that. It has a vector of widgets. It places one under the other in the
/// order they were addded
class WidgetsVector {
  std::vector<std::unique_ptr<Widget>> widgets;
  int margin, start_y, fontSize;
  float width, height;
  int pos_x = GetScreenWidth() / 2;

  float getCurrentDistance() {
    return start_y + (margin + height) * widgets.size();
  }

public:
  WidgetsVector(int start_y, int margin, float width, float height, int fontSize)
      : start_y(start_y)
      , margin(margin)
      , width(width)
      , height(height)
      , fontSize(fontSize) {}

  void push_back_button(std::string label, std::function<void()> onClick) {
    std::unique_ptr<Button> btn = std::make_unique<Button>(label, pos_x, getCurrentDistance(), width, height, fontSize);

    btn->setOnClick(onClick);
    widgets.push_back(std::move(btn));
  }

  void push_back_textInput(std::string prompt) {
    std::unique_ptr<TextInput> input =
        std::make_unique<TextInput>(prompt, pos_x, getCurrentDistance(), width, height, fontSize);
    widgets.push_back(std::move(input));
  }

  void update_all() {
    for (auto &widget : widgets) {
      widget->update();
    }
  }

  void draw_all() {
    for (auto &widget : widgets) {
      widget->draw();
    }
  }

  // TODO! Save all the informations that's possible to file/GameContext whatever
  void save() {}
};

} // namespace gui
} // namespace battleship

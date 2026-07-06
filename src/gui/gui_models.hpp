#pragma once
#include <functional>
#include <raylib.h>
#include <string>

namespace battleship {
namespace gui {

enum class GuiState {
  MAIN_MENU,
  SETTINGS,
  QUIT,
};

struct GameContext {
  std::string playerName;
  GuiState guiState = GuiState::MAIN_MENU;
};

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
  int margin, start_y, fontSize;
  float width, height;

public:
  ButtonVector(int start_y, int margin, float width, float height, int fontSize)
      : start_y(start_y)
      , margin(margin)
      , width(width)
      , height(height)
      , fontSize(fontSize) {}

  void push_back(std::string label, std::function<void()> onClick) {
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

class TextInput {
  // 31 chars, because 32 is the max we can send through the network
  const static int MAX_INPUT = 31;

  char buffer[MAX_INPUT + 1];
  int letterCount = 0;
  Rectangle textBox;
  std::string prompt;
  bool isMouseOnText = false;
  int fontSize;

public:
  TextInput(float pos_x, float pos_y, float width, float height, std::string prompt, int fontSize)
      : textBox({pos_x, pos_y, width, height})
      , prompt(prompt)
      , fontSize(fontSize)
      , buffer() {}

  void update() {
    isMouseOnText = CheckCollisionPointRec(GetMousePosition(), textBox);
    // input

    if (isMouseOnText) {
      SetMouseCursor(MOUSE_CURSOR_IBEAM);
      int key = GetCharPressed();

      while (key > 0) {
        if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT)) {
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

  void draw() {

    DrawRectangleRec(textBox, LIGHTGRAY);
    if (isMouseOnText) {
      DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, RED);
    } else {
      DrawRectangleLines(textBox.x, textBox.y, textBox.width, textBox.height, DARKGRAY);
    }

    int textWidth = MeasureText(prompt.c_str(), fontSize);
    float text_x = (textBox.x + textBox.width / 2.0f) - (textWidth / 2.0f);
    float text_y = (textBox.y + textBox.height / 2.0f) - (fontSize / 2.0f);

    if (prompt != "" && std::string(buffer).empty()) {
      DrawText(prompt.c_str(), text_x, text_y, fontSize, MAROON);
    } else {
      DrawText(buffer, text_x, text_y, fontSize, MAROON);
    }

    DrawText(TextFormat("%i/%i", letterCount, MAX_INPUT),
             textBox.x + textBox.width / 2,
             textBox.y + textBox.height + 4,
             fontSize / 2,
             DARKGRAY);
  }

  std::string getInput() {
    return std::string(buffer);
  }
};

} // namespace gui
} // namespace battleship

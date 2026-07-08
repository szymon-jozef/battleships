#pragma once
#include <algorithm>
#include <cctype>
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

  std::string serverUrl = "127.0.0.1";
  uint16_t serverPort = 6767;

  enum class GameMode { HOSTING, JOINING };
  GameMode currentGameMode = GameMode::HOSTING;
};

// === Widgets and stuff ===

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
      , isFocusable(isFocusable) {}
  virtual ~Widget() = default;

  virtual void draw() {
    // focused border
    // NOTE: should be called at the end of each draw function, so it's not covered
    if (isFocusable && isFocused) {
      DrawRectangleLines(scaleRect.x, scaleRect.y, scaleRect.width, scaleRect.height, GRAY);
    }
  }

  virtual void update() {
    updatePos();
    updateTextPos();
  };

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

class TextInput : public Widget {
  // 31 chars, because 32 is the max we can send through the network

  const static int MAX_INPUT_CHARS = 31;
  std::string &target;
  char buffer[MAX_INPUT_CHARS + 1] = "";
  int letterCount = 0;
  std::string prompt;
  bool isMouseOnText = false;

  bool handleKeyboardInput() {
    isMouseOnText = CheckCollisionPointRec(GetMousePosition(), finalPositionRect);

    if (isMouseOnText || isFocused) {

      bool isEdited = false;
      if (isMouseOnText) {
        SetMouseCursor(MOUSE_CURSOR_IBEAM);
      } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
      }

      int key = GetCharPressed();

      while (key > 0) {
        if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS)) {
          buffer[letterCount] = (char)key;
          buffer[letterCount + 1] = '\0';
          letterCount++;
        }
        key = GetCharPressed();
        isEdited = true;
      }

      // delete one char
      if (IsKeyPressed(KEY_BACKSPACE)) {
        letterCount--;
        if (letterCount < 0) {
          letterCount = 0;
        }
        buffer[letterCount] = '\0';
        isEdited = true;
      }

      // delete everything
      if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_U)) ||
          (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_BACKSPACE))) {
        letterCount = 0;
        buffer[letterCount] = '\0';
        isEdited = true;
      }

      return isEdited;
    }
    return false;
  }

  bool handleClipboardInput() {
    if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
      const char *clipboardText_c = GetClipboardText();
      if (!clipboardText_c) {
        return false;
      }

      std::string clipboardText(clipboardText_c);

      if (clipboardText.empty()) {
        return false;
      }

      clipboardText.erase(std::remove(clipboardText.begin(), clipboardText.end(), '\n'), clipboardText.end());
      clipboardText.erase(std::remove_if(clipboardText.begin(),
                                         clipboardText.end(),
                                         [](auto const &c) -> bool { return !std::isalnum(c); }),
                          clipboardText.end());

      if (clipboardText.size() >= MAX_INPUT_CHARS) {
        clipboardText.resize(MAX_INPUT_CHARS);
      }

      std::copy(clipboardText.begin(), clipboardText.end(), buffer);
      letterCount = clipboardText.size();
      buffer[letterCount] = '\0';
      return true;
    }
    return false;
  }

public:
  TextInput(std::string prompt, float pos_y, Rectangle scaleRect, std::string &target)
      : Widget(prompt, pos_y, scaleRect, 0.8f)
      , target(target) {
    if (target.size() < MAX_INPUT_CHARS) {
      std::copy(target.begin(), target.end(), buffer);
      letterCount = target.size();
      buffer[letterCount] = '\0';
    } else {
      spdlog::warn("[GUI] Loaded text was to long");
    }
  }

  void update() override {
    Widget::update();
    if (handleKeyboardInput() || handleClipboardInput()) {
      target = std::string(buffer);
      letterCount = target.size();
      buffer[letterCount] = '\0';
    }

    // TODO! Move this to widget
    int textWidth = MeasureText(prompt.c_str(), fontSize);
    float text_x = (finalPositionRect.x + finalPositionRect.width / 2.0f) - (textWidth / 2.0f);
    float text_y = (finalPositionRect.y + finalPositionRect.height / 2.0f) - (fontSize / 2.0f);
  }

  void draw() override {

    DrawRectangleRec(finalPositionRect, LIGHTGRAY);
    if (isMouseOnText) {
      DrawRectangleLines(
          finalPositionRect.x, finalPositionRect.y, finalPositionRect.width, finalPositionRect.height, RED);
    } else {
      DrawRectangleLines(
          finalPositionRect.x, finalPositionRect.y, finalPositionRect.width, finalPositionRect.height, DARKGRAY);
    }

    if (prompt != "" && std::string(buffer).empty()) {
      DrawText(prompt.c_str(), text_x, text_y, fontSize, MAROON);
      drawLabelInTheMiddle(MAROON); // TODO! Change to some other color maybe?
    } else {
      drawTextInTheMiddle(buffer, MAROON);
    }

    std::string charactersLeft = std::string(TextFormat("%i/%i", letterCount, MAX_INPUT_CHARS));

    TextLabel charactersLeftLabel =
        TextLabel(charactersLeft, finalPositionRect.y + finalPositionRect.height * 2, {2, 0.1f, 1, 0.4f}, DARKGRAY);
    charactersLeftLabel.draw();
    Widget::draw();
  }

  std::string getInput() const {
    return std::string(buffer);
  }
};

/// @brief A "layout manager" if you can call it that. It has a vector of widgets. It places one under the other in the
/// order they were added
class WidgetsVector {
  GameContext &gameContext;

  std::vector<std::unique_ptr<Widget>> widgets;
  float margin, start_y, fontSize;
  float width, height;
  int currentFocus = 0;

  const Rectangle scale;

  float getCurrentDistance() {
    return start_y + (GetScreenHeight() * margin + height) * widgets.size();
  }

  void handleFocus() {
    if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_TAB)) {
      do {
        currentFocus--;

        if (currentFocus < 0) {
          currentFocus = widgets.size() - 1;
        };

      } while (!widgets[currentFocus]->isFocusable);

    } else if (IsKeyPressed(KEY_TAB)) {
      do {
        currentFocus = (currentFocus + 1) % widgets.size();
      } while (!widgets[currentFocus]->isFocusable);
    }

    widgets[currentFocus]->focus();
  }

public:
  WidgetsVector(GameContext &gameContext, float start_y, float margin, float width, float height)
      : gameContext(gameContext)
      , start_y(start_y)
      , margin(margin)
      , width(width)
      , height(height)
      , scale({0.5f, 1.0f, width, height}) {}

  void push_back_button(std::string label, std::function<void()> onClick) {
    std::unique_ptr<Button> btn = std::make_unique<Button>(label, getCurrentDistance(), scale);

    btn->setOnClick(onClick);
    widgets.push_back(std::move(btn));
  }

  void push_back_textInput(std::string prompt, std::string &target) {
    std::unique_ptr<TextInput> input = std::make_unique<TextInput>(prompt, getCurrentDistance(), scale, target);
    widgets.push_back(std::move(input));
  }

  void push_back_label(std::string text, Color color) {
    std::unique_ptr<TextLabel> label = std::make_unique<TextLabel>(text, getCurrentDistance(), scale, color);
    widgets.push_back(std::move(label));
  }

  void push_back_textInput_with_label(std::string label, std::string prompt, Color color, std::string &target) {
    std::unique_ptr<TextLabel> labelWidget = std::make_unique<TextLabel>(label, getCurrentDistance(), scale, color);

    widgets.push_back(std::move(labelWidget));

    std::unique_ptr<TextInput> input =
        std::make_unique<TextInput>(prompt, getCurrentDistance() - height, scale, target);

    widgets.push_back(std::move(input));
  }

  void update_all() {
    float absolute_y = GetScreenHeight() * start_y;
    float absolute_margin = GetScreenHeight() * margin;

    for (auto &widget : widgets) {
      widget->setY(absolute_y);

      widget->update();
      widget->unFocus();

      absolute_y += widget->getRect().height + absolute_margin;
    }
    handleFocus();
  }

  void draw_all() {
    for (auto &widget : widgets) {
      widget->draw();
    }
  }
};

} // namespace gui
} // namespace battleship

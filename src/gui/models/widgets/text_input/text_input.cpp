#include "text_input.hpp"
#include "../text_label/text_label.hpp"
#include "../widget.hpp"
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {

TextInput::TextInput(std::string prompt, float pos_y, Rectangle scaleRect, std::string &target, InputType inputType)
    : Widget(prompt, pos_y, scaleRect, 0.8f)
    , target(target)
    , inputType(inputType) {
  if (target.size() < MAX_INPUT_CHARS) {
    std::copy(target.begin(), target.end(), buffer);
    letterCount = target.size();
    buffer[letterCount] = '\0';
  } else {
    spdlog::warn("[GUI] Loaded text was to long");
  }
}

bool TextInput::handleKeyboardInput() {
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

bool TextInput::handleClipboardInput() {
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

void TextInput::update() {
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

void TextInput::draw() {
  drawInputRect();
  drawPrompt();
  drawCharactersLeftPrompt();
}

void TextInput::drawInputRect() {
  DrawRectangleRec(finalPositionRect, LIGHTGRAY);
  if (isMouseOnText) {
    DrawRectangleLines(
        finalPositionRect.x, finalPositionRect.y, finalPositionRect.width, finalPositionRect.height, RED);
  } else {
    DrawRectangleLines(
        finalPositionRect.x, finalPositionRect.y, finalPositionRect.width, finalPositionRect.height, DARKGRAY);
  }
}

void TextInput::drawPrompt() {
  if (!prompt.empty() && std::string(buffer).empty()) {
    DrawText(prompt.c_str(), text_x, text_y, fontSize, MAROON);
    drawLabelInTheMiddle(MAROON); // TODO! Change to some other color maybe?
  } else {
    drawTextInTheMiddle(buffer, MAROON);
  }
}

void TextInput::drawCharactersLeftPrompt() {
  charactersLeft = TextFormat("%i/%i", letterCount, MAX_INPUT_CHARS);

  TextLabel charactersLeftLabel = TextLabel(
      charactersLeft.data(), finalPositionRect.y + finalPositionRect.height * 2, {2, 0.1f, 1, 0.4f}, DARKGRAY);
  charactersLeftLabel.draw();
  Widget::draw();
}

std::string TextInput::getInput() const {
  return std::string(buffer);
}

} // namespace gui
} // namespace battleship

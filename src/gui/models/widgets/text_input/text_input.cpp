#include "text_input.hpp"
#include "../text_label/text_label.hpp"
#include "../widget.hpp"
#include <raylib.h>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {

TextInput::TextInput(float pos_y, Rectangle scaleRect, std::string &target, InputType inputType)
    : Widget(target, pos_y, scaleRect, 0.8f)
    , target(target)
    , inputType(inputType) {
  target.reserve(MAX_INPUT_CHARS);
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
    }
    isEdited = getKeyboardInput() || removeCharFromBuffer() || clearBuffer();

    normaliseText();

    return isEdited;
  }
  SetMouseCursor(MOUSE_CURSOR_DEFAULT);

  return false;
}

bool TextInput::getKeyboardInput() {
  int key = GetCharPressed();
  while (key > 0) {
    if (letterCount < MAX_INPUT_CHARS) {
      buffer[letterCount] = (char)key;
      buffer[letterCount + 1] = '\0';
      letterCount++;
    }
    key = GetCharPressed();
    return true;
  }
  return false;
}

bool TextInput::removeCharFromBuffer() {
  if (IsKeyPressed(KEY_BACKSPACE)) {
    letterCount--;
    if (letterCount < 0) {
      letterCount = 0;
    }
    buffer[letterCount] = '\0';
    return true;
  }
  return false;
}

bool TextInput::clearBuffer() {
  if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_U)) ||
      (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_BACKSPACE))) {
    letterCount = 0;
    buffer[letterCount] = '\0';
    return true;
  }
  return false;
}

bool TextInput::handleClipboardInput() {
  if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
    const char *clipboardText_c = GetClipboardText();
    if (!clipboardText_c) {
      return false;
    }

    strncpy(buffer, clipboardText_c, MAX_INPUT_CHARS);
    if (strlen(clipboardText_c) > MAX_INPUT_CHARS) {
      letterCount = MAX_INPUT_CHARS;
      buffer[letterCount] = '\0';
    }

    normaliseText();
    return true;
  }
  return false;
}

void TextInput::normaliseText() {
  // remove end lines
  target = buffer;
  target.erase(std::remove(target.begin(), target.end(), '\n'), target.end());

  // we delete different things depending on input type
  switch (inputType) {
  case InputType::NAME: {
    // remove non-alphanumeric characters
    target.erase(std::remove_if(target.begin(), target.end(), [](auto const &c) -> bool { return !std::isalnum(c); }),
                 target.end());

    // trim if target is to large
    if (target.size() >= MAX_INPUT_CHARS) {
      target.resize(MAX_INPUT_CHARS);
    }
    break;
  }
  case InputType::IP: {
    // we remove everything that's not number/dot
    target.erase(std::remove_if(
                     target.begin(), target.end(), [](auto const &c) -> bool { return (c < 48 || c > 57) && c != 46; }),
                 target.end());

    // xxx.xxx.xxx.xxx = 3 * 4 + 3 * 1 = 12 + 3 + 15
    if (target.size() >= 15) {
      target.resize(15);
    }
    break;
  }
  default: {
    break;
  }
  }
  // TODO! Change this to something cheaper maybe
  strncpy(buffer, target.c_str(), MAX_INPUT_CHARS);
  letterCount = target.size();
  buffer[letterCount] = '\0';
}

void TextInput::update() {
  Widget::update();
  if (handleClipboardInput() || handleKeyboardInput()) {
    target = std::string(buffer);
    normaliseText();
    updateEveryPos(true);
    setLabel(target);
  }

  if (inputType == InputType::NAME) {
    updateCharactersLeftPrompt();
  }
}

void TextInput::updateCharactersLeftPrompt() {
  charactersLeft = TextFormat("%i/%i", letterCount, MAX_INPUT_CHARS);
}

void TextInput::draw() {
  drawInputRect();
  drawLabel(MAROON); // draw the current buffer

  if (inputType == InputType::NAME) {
    drawCharactersLeftPrompt();
  }
  Widget::draw();
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

void TextInput::drawCharactersLeftPrompt() {
  TextLabel charactersLeftLabel =
      TextLabel(charactersLeft.data(), finalPositionRect.y + finalPositionRect.height, {1, 0.1f, 1, 0.05f}, DARKGRAY);
  charactersLeftLabel.draw();
  Widget::draw();
}

} // namespace gui
} // namespace battleship

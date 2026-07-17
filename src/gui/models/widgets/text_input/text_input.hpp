#pragma once

#include "../widget.hpp"
#include <raylib.h>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {

class TextInput : public Widget {
  // 31 chars, because 32 is the max we can send through the network
public:
  enum class InputType { NAME, IP };
  TextInput(float pos_y, Rectangle scaleRect, std::string &target, InputType inputType);

  void update() override;
  void draw() override;

private:
  const static int MAX_INPUT_CHARS = 31;
  int letterCount = 0;
  int promptWidth = 0;
  float text_x = 0, text_y = 0;

  char buffer[MAX_INPUT_CHARS + 1] = "";

  std::string &target;
  std::string_view charactersLeft;
  std::string inputName;

  bool isMouseOnText = false;
  InputType inputType;

  // --- handlers ---
  bool handleKeyboardInput();
  bool handleClipboardInput();

  // --- text utils ---
  /// @brief Remove bad characters from `bufferString` depending on `inputType`, modifying it in-place.
  /// Then save it to the `buffer`
  void normaliseText();
  bool clearBuffer();
  bool removeCharFromBuffer();
  bool getKeyboardInput();

  // --- drawing ---
  void drawInputRect();
  void drawCharactersLeftPrompt();
  void drawInputName();

  // --- updating ---
  void updateCharactersLeftPrompt();
};

} // namespace gui
} // namespace battleship

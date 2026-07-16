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
  TextInput(std::string prompt, float pos_y, Rectangle scaleRect, std::string &target, InputType inputType);

  void update() override;
  void draw() override;
  std::string getInput() const;

private:
  const static int MAX_INPUT_CHARS = 31;
  int letterCount = 0;

  char buffer[MAX_INPUT_CHARS + 1] = "";

  std::string prompt, &target;
  std::string_view charactersLeft;

  bool isMouseOnText = false;

  bool handleKeyboardInput();
  bool handleClipboardInput();

  void drawInputRect();
  void drawPrompt();
  void drawCharactersLeftPrompt();

  InputType inputType;
};

} // namespace gui
} // namespace battleship

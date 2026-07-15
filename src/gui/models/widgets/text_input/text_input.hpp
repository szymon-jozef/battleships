#pragma once

#include "../widget.hpp"
#include <raylib.h>
#include <spdlog/spdlog.h>

namespace battleship {
namespace gui {

class TextInput : public Widget {
  // 31 chars, because 32 is the max we can send through the network

  const static int MAX_INPUT_CHARS = 31;
  int letterCount = 0;

  char buffer[MAX_INPUT_CHARS + 1] = "";

  std::string prompt, &target;

  bool isMouseOnText = false;

  bool handleKeyboardInput();
  bool handleClipboardInput();

public:
  TextInput(std::string prompt, float pos_y, Rectangle scaleRect, std::string &target);

  void update() override;
  void draw() override;
  std::string getInput() const;
};

} // namespace gui
} // namespace battleship

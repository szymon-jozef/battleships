#pragma once

#include "../widget.hpp"
#include "models/gui_models.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

/// @brief A "layout manager" if you can call it that. It has a vector of widgets. It places one under the other in the
/// order they were added
class WidgetsVector {
  GameContext &gameContext;

  std::vector<std::unique_ptr<Widget>> widgets;
  float margin, start_y, fontSize;
  float width, height;
  int currentFocus = 0;

  const Rectangle scale;

  float getCurrentDistance();
  void handleFocusChange();

public:
  // TODO! I don't think I want to set this every time in the constructor
  WidgetsVector(GameContext &gameContext, float start_y, float margin, float width, float height);

  void push_back_button(std::string label, std::function<void()> onClick);
  void push_back_nameInput(std::string &target);
  void push_back_ipInput(std::string &target);

  void push_back_label(std::string text, Color color);
  void push_back_textInput_with_label(std::string label, Color color, std::string &target);

  void update_all();
  void draw_all();
};

} // namespace gui
} // namespace battleship

#pragma once

#include "../widget.hpp"
#include "models/gui_models.hpp"

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

      widget->updateEveryPos(true);
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

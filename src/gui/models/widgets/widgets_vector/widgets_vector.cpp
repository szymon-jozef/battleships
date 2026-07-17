#include "widgets_vector.hpp"

namespace battleship {
namespace gui {

WidgetsVector::WidgetsVector(GameContext &gameContext, float start_y, float margin, float width, float height)
    : gameContext(gameContext)
    , margin(margin)
    , start_y(start_y)
    , width(width)
    , height(height)
    , scale({0.5f, 1.0f, width, height}) {}

float WidgetsVector::getCurrentDistance() {
  return start_y + (GetScreenHeight() * margin + height) * widgets.size();
}

void WidgetsVector::handleFocusChange() {
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

void WidgetsVector::push_back_button(std::string label, std::function<void()> onClick) {
  std::unique_ptr<Button> btn = std::make_unique<Button>(label, getCurrentDistance(), scale);

  btn->setOnClick(onClick);
  widgets.push_back(std::move(btn));
}

void WidgetsVector::push_back_nameInput(std::string &target) {
  std::unique_ptr<TextInput> input =
      std::make_unique<TextInput>(getCurrentDistance(), scale, target, TextInput::InputType::NAME);
  widgets.push_back(std::move(input));
}

void WidgetsVector::push_back_ipInput(std::string &target) {
  std::unique_ptr<TextInput> input =
      std::make_unique<TextInput>(getCurrentDistance(), scale, target, TextInput::InputType::IP);
  widgets.push_back(std::move(input));
}

void WidgetsVector::push_back_label(std::string text, Color color) {
  std::unique_ptr<TextLabel> label = std::make_unique<TextLabel>(text, getCurrentDistance(), scale, color);
  widgets.push_back(std::move(label));
}

void WidgetsVector::update_all() {
  float absolute_y = GetScreenHeight() * start_y;
  float absolute_margin = GetScreenHeight() * margin;

  for (auto &widget : widgets) {
    widget->setY(absolute_y);

    widget->updateEveryPos(true);
    widget->update();
    widget->unFocus();

    absolute_y += widget->getRect().height + absolute_margin;
  }
  handleFocusChange();
}

void WidgetsVector::draw_all() {
  for (auto &widget : widgets) {
    widget->draw();
  }
}

} // namespace gui
} // namespace battleship

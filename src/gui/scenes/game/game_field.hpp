#pragma once
#include "logic_models.hpp"

namespace battleship {
namespace gui {
class GameField {
  logic::FieldState state = logic::FieldState::EMPTY;
  bool isClickable = true;

public:
  GameField() = default;

  void setClickable(bool newIsClickable) {
    this->isClickable = newIsClickable;
  }

  bool getIsClickable() const {
    return isClickable && state == logic::FieldState::EMPTY; // we can only click empty fields
  }

  void setState(logic::FieldState newState) {
    this->state = newState;
  }

  logic::FieldState getState() const {
    return state;
  }
};
} // namespace gui
} // namespace battleship

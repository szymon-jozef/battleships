#include "logic_models.hpp"
#include <sstream>
#include <tuple>

namespace battleship {
namespace logic {
BaseGrid::BaseGrid(const unsigned short int width, const unsigned short int height)
    : grid(height * width, Field())
    , WIDTH(width)
    , HEIGHT(height) {}

FieldState BaseGrid::getFieldState(unsigned short int row, unsigned short int column) const {
  return grid[row * WIDTH + column].getState();
}

void BaseGrid::setFieldState(unsigned short int row, unsigned short int column, FieldState state) {
  grid[row * WIDTH + column].setState(state);
}

std::string BaseGrid::asString() const {
  std::stringstream s;
  for (size_t fieldIndex = 0; fieldIndex < WIDTH * HEIGHT; fieldIndex++) {
    s << static_cast<char>(grid[fieldIndex].getState());
    if (fieldIndex % WIDTH == 0) {
      s << '\n';
    }
  }
  return s.str();
}

void BaseGrid::markNearbyAsSunk(unsigned short int row, unsigned short int column) {
  FieldState baseFieldState = getFieldState(row, column);

  std::vector<std::tuple<unsigned short int, unsigned short int>> toCheck;

  if (baseFieldState == FieldState::SUNK) {
    toCheck.push_back(std::make_tuple(row, column));
  }

  while (!toCheck.empty()) {
    auto [r, c] = toCheck.back();
    toCheck.pop_back();

    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {

        unsigned short int checkRow = r + i;
        unsigned short int checkColumn = c + j;
        if (checkRow >= HEIGHT || checkColumn >= WIDTH) {
          continue;
        }

        if (checkRow >= 0 && checkRow < HEIGHT && checkColumn >= 0 && checkColumn < WIDTH) {

          if (getFieldState(checkRow, checkColumn) == FieldState::HIT) {
            toCheck.push_back(std::make_tuple(checkRow, checkColumn));
            setFieldState(checkRow, checkColumn, FieldState::SUNK);
          }
        }
      }
    }
  }
}

} // namespace logic
} // namespace battleship

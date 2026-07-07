#include "logic_models.hpp"
#include <sstream>
#include <tuple>

namespace battleship {
namespace logic {
BaseGrid::BaseGrid(const unsigned short int width, const unsigned short int height)
    : WIDTH(width)
    , HEIGHT(height)
    , grid(height, std::vector<Field>(width)) {}

FieldState BaseGrid::getFieldState(unsigned short int row, unsigned short int column) const {
  return grid[row][column].getState();
}

std::string BaseGrid::asString() const {
  std::stringstream s;
  for (const auto &row : grid) {
    for (const auto &field : row) {
      s << static_cast<char>(field.getState());
    }
    s << '\n';
  }
  return s.str();
}

void BaseGrid::markNearbyAsSunk(unsigned short int row, unsigned short int column) {
  FieldState baseFieldState = grid[row][column].getState();

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

          if (grid[checkRow][checkColumn].getState() == FieldState::HIT) {
            toCheck.push_back(std::make_tuple(checkRow, checkColumn));
            grid[checkRow][checkColumn].setState(FieldState::SUNK);
          }
        }
      }
    }
  }
}

} // namespace logic
} // namespace battleship

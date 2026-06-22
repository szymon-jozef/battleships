#include "logic_models.hpp"
#include <sstream>

BaseGrid::BaseGrid(const unsigned short int width,
                   const unsigned short int height)
    : WIDTH(width), HEIGHT(height), grid(HEIGHT, std::vector<Field>(WIDTH)) {}

FieldState BaseGrid::getFieldState(unsigned short int row,
                                   unsigned short int column) const {
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

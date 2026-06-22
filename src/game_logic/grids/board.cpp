#include <algorithm>
#include <logic_models.hpp>
#include <stdexcept>

Board::Board(const unsigned short int width, const unsigned short int height)
    : BaseGrid(width, height) {}

bool Board::placeShip(std::shared_ptr<Ship> ship, unsigned short int startRow,
                      unsigned short int startColumn, bool isHorizontal) {
  if (startRow >= HEIGHT || startColumn >= WIDTH) {
    throw std::invalid_argument(
        "Given of the beggining of the ship are out of bounds!");
  }
  unsigned short int shipLength =
      static_cast<unsigned short int>(ship->getType());

  unsigned short int endRow =
      isHorizontal ? startRow : startRow + shipLength - 1;
  unsigned short int endColumn =
      isHorizontal ? startColumn + shipLength - 1 : startColumn;

  // we don't check < 0 since unsigned int + unsigned int cannot be less than 0
  if (endRow >= HEIGHT || endColumn >= WIDTH) {
    throw std::invalid_argument(
        "Coordinates of the end of the ship are out of bounds!");
  }

  int rectangleTop = std::max(0, static_cast<int>(startRow) - 1);
  int rectangleLeft = std::max(0, static_cast<int>(startColumn) - 1);

  int rectangleBottom =
      std::min(static_cast<int>(HEIGHT) - 1, static_cast<int>(endRow) + 1);
  int rectangleRight =
      std::min(static_cast<int>(WIDTH) - 1, static_cast<int>(endColumn) + 1);

  for (int row = rectangleTop; row <= rectangleBottom; row++) {
    for (int column = rectangleLeft; column <= rectangleRight; column++) {
      if (grid[row][column].getState() == FieldState::TAKEN) {
        throw std::invalid_argument(
            "Cannot place ship here, because of collision nearby!");
      }
    }
  }

  for (int i = 0; i < shipLength; i++) {
    if (isHorizontal) {
      grid[startRow][startColumn + i].placeShip(ship);
      grid[startRow][startColumn + i].setState(FieldState::TAKEN);
    } else {
      grid[startRow + i][startColumn].placeShip(ship);
      grid[startRow + i][startColumn].setState(FieldState::TAKEN);
    }
  }

  return true;
}

FieldState Board::takeShot(unsigned short int row, unsigned short int column) {
  if (row >= HEIGHT || column >= WIDTH) {
    throw std::invalid_argument("Given coordinates are out of bounds!");
  }

  if (grid[row][column].getState() == FieldState::TAKEN) {
    Ship *ship = grid[row][column].getShip();
    ship->hit();

    if (ship->isSunk()) {
      grid[row][column].setState(FieldState::SUNK);
      markNearbyAsSunk(row, column);
      return FieldState::SUNK;
    }
    return FieldState::HIT;
  }

  grid[row][column].setState(FieldState::MISSED);
  return FieldState::MISSED;
}

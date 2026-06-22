#include <algorithm>
#include <logic_models.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

Board::Board() : BaseGrid(10, 10) {
  spdlog::info("[Logic] creating default 10x10 board");
}

Board::Board(const unsigned short int width, const unsigned short int height)
    : BaseGrid(width, height) {
  spdlog::info("[Logic] creating custom {}x{} board", WIDTH, HEIGHT);
}

bool Board::placeShip(std::shared_ptr<Ship> ship, unsigned short int startRow,
                      unsigned short int startColumn, bool isHorizontal) {
  spdlog::info("[Logic] attempting to place a ship of type {} at ({}, {})",
               static_cast<char>(ship->getType()), startRow, startColumn);

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
      spdlog::info("[Logic] Settings ({}, {}) as taken", startRow,
                   startColumn + i);
      grid[startRow][startColumn + i].placeShip(ship);
      grid[startRow][startColumn + i].setState(FieldState::TAKEN);
    } else {
      spdlog::info("[Logic] Settings ({}, {}) as taken", startRow + i,
                   startColumn);
      grid[startRow + i][startColumn].placeShip(ship);
      grid[startRow + i][startColumn].setState(FieldState::TAKEN);
    }
  }
  return true;
}

FieldState Board::takeShot(unsigned short int row, unsigned short int column) {
  spdlog::info("[Logic] Player board was shot at ({},{})", row, column);
  if (row >= HEIGHT || column >= WIDTH) {
    throw std::invalid_argument("Given coordinates are out of bounds!");
  }

  if (grid[row][column].getState() == FieldState::TAKEN) {
    spdlog::info("[Logic] there was a ship at ({},{})!", row, column);
    Ship *ship = grid[row][column].getShip();
    if (!ship) {
      throw std::logic_error("Taken field couldn't retrieve the ship. This "
                             "__should never happen!__");
    }
    ship->hit();

    if (ship->isSunk()) {
      spdlog::info("[Logic] ship was sunk!");
      grid[row][column].setState(FieldState::SUNK);
      markNearbyAsSunk(row, column);
      return FieldState::SUNK;
    }
    spdlog::info("[Logic] ship was hit!");
    grid[row][column].setState(FieldState::HIT);
    return FieldState::HIT;
  }

  spdlog::info("[Logic] Enemy attempted to hit ({},{}) but he missed!", row,
               column);
  grid[row][column].setState(FieldState::MISSED);
  return FieldState::MISSED;
}

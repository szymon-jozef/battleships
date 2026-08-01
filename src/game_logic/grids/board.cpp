#include <algorithm>
#include <logic_models.hpp>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace battleship {
namespace logic {

Board::Board()
    : BaseGrid(10, 10) {
  spdlog::info("[Logic] creating default 10x10 board");
}

Board::Board(const unsigned short int width, const unsigned short int height)
    : BaseGrid(width, height) {
  spdlog::info("[Logic] creating custom {}x{} board", WIDTH, HEIGHT);
}

bool Board::placeShip(std::shared_ptr<Ship> ship,
                      unsigned short int startRow,
                      unsigned short int startColumn,
                      bool isHorizontal) {
  spdlog::info("[Logic] attempting to place a ship of type {} at ({}, {})",
               static_cast<char>(ship->getType()),
               startRow,
               startColumn);

  if (!isPlacementValid(startRow, startColumn, ship.get(), isHorizontal)) {
    return false;
  }

  unsigned short int shipLength = static_cast<unsigned short int>(ship->getType());
  for (int i = 0; i < shipLength; i++) {
    if (isHorizontal) {
      spdlog::info("[Logic] Settings ({}, {}) as taken", startRow, startColumn + i);
      grid[startRow * WIDTH + (startColumn + i)].placeShip(ship);
      setFieldState(startRow, startColumn + i, FieldState::TAKEN);
    } else {
      spdlog::info("[Logic] Settings ({}, {}) as taken", startRow + i, startColumn);
      grid[(startRow + i) * WIDTH + startColumn].placeShip(ship);
      setFieldState(startRow + i, startColumn, FieldState::TAKEN);
    }
  }
  return true;
}

FieldState Board::recieveShot(unsigned short int row, unsigned short int column) {
  spdlog::info("[Logic] Player board was shot at ({},{})", row, column);
  if (row >= HEIGHT || column >= WIDTH) {
    throw std::invalid_argument("Given coordinates are out of bounds!");
  }

  if (getFieldState(row, column) == FieldState::TAKEN) {
    spdlog::info("[Logic] there was a ship at ({},{})!", row, column);
    Ship *ship = grid[row * WIDTH + column].getShip();
    if (!ship) {
      throw std::logic_error("Taken field couldn't retrieve the ship. This "
                             "__should never happen!__");
    }
    ship->hit();

    if (ship->isSunk()) {
      spdlog::info("[Logic] ship was sunk!");
      setFieldState(row, column, FieldState::SUNK);
      markNearbyAsSunk(row, column);
      return FieldState::SUNK;
    }
    spdlog::info("[Logic] ship was hit!");
    setFieldState(row, column, FieldState::HIT);
    return FieldState::HIT;
  }

  spdlog::info("[Logic] Enemy attempted to hit ({},{}) but he missed!", row, column);
  setFieldState(row, column, FieldState::MISSED);
  return FieldState::MISSED;
}

bool Board::isPlacementValid(unsigned short int row,
                             unsigned short int column,
                             const Ship *ship,
                             bool isHorizontal) const {

  if (row >= HEIGHT || column >= WIDTH) {
    spdlog::info("[Logic] Given coordinates ({}, {}) of the beggining of the ship are out of bounds!", row, column);
    return false;
  }

  unsigned short int shipLength = static_cast<unsigned short int>(ship->getType());

  unsigned short int endRow = isHorizontal ? row : row + shipLength - 1;
  unsigned short int endColumn = isHorizontal ? column + shipLength - 1 : column;

  // we don't check < 0 since unsigned int + unsigned int cannot be less than 0
  if (endRow >= HEIGHT || endColumn >= WIDTH) {
    spdlog::info("[Logic] Coordinates ({},{}) of the end of the ship are out of bounds!", endRow, endColumn);
    return false;
  }

  int rectangleTop = std::max(0, static_cast<int>(row) - 1);
  int rectangleLeft = std::max(0, static_cast<int>(column) - 1);

  int rectangleBottom = std::min(static_cast<int>(HEIGHT) - 1, static_cast<int>(endRow) + 1);
  int rectangleRight = std::min(static_cast<int>(WIDTH) - 1, static_cast<int>(endColumn) + 1);

  for (int currentRow = rectangleTop; currentRow <= rectangleBottom; currentRow++) {
    for (int currentColumn = rectangleLeft; currentColumn <= rectangleRight; currentColumn++) {
      if (getFieldState(currentRow, currentColumn) == FieldState::TAKEN) {
        spdlog::info(
            "[Logic] Cannot place ship here ({}, {}), because of collision nearby!", currentRow, currentColumn);
        return false;
      }
    }
  }

  return true;
}

} // namespace logic
} // namespace battleship

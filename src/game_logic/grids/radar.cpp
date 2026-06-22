#include "logic_models.hpp"
#include <spdlog/spdlog.h>

Radar::Radar() : BaseGrid(10, 10) {}

Radar::Radar(const unsigned short int width, const unsigned short int height)
    : BaseGrid(width, height) {}

void Radar::markShotResult(FieldState shotResult, unsigned short int row,
                           unsigned short int column) {

  if (shotResult == FieldState::SUNK) {
    spdlog::info("[Logic] We sunk a ship at ({},{})! Marking all nearby hit "
                 "fields as sunk",
                 row, column);
    grid[row][column].setState(FieldState::SUNK);
    markNearbyAsSunk(row, column);
    return;
  }
  spdlog::info("[Logic] Field at ({}, {}) was {}", row, column,
               static_cast<char>(shotResult));
  grid[row][column].setState(shotResult);
}

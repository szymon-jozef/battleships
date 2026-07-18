#include "game_grid.hpp"
#include <algorithm>
#include <spdlog/spdlog.h>

// === GameGrid ===

namespace battleship {
namespace gui {

void GameGrid::updateData() {
  padding_y = GetScreenHeight() / 10.0f;
  padding_x = GetScreenWidth() / 10.0f;

  gridRect.width = GetScreenWidth() * 0.5f - (2 * padding_x);

  gridRect.y = 0 + padding_y;
  gridRect.height = GetScreenHeight() - (2 * padding_y);

  if (gridType == GridType::BOARD) {
    gridRect.x = 0 + padding_x;
  } else {
    gridRect.x = GetScreenWidth() / 2.0f + padding_x;
  }

  // rectangle beginning
  begin_y_pos = gridRect.y;
  current_y_pos = begin_y_pos;
  current_x_pos = gridRect.x;

  // fields size
  calculatedFieldWidth = gridRect.width / (((columns - 1.0f) * multiplier) + 1.0f);
  calculatedFieldHight = gridRect.height / (((rows - 1.0f) * multiplier) + 1.0f);

  fieldSize = std::min(calculatedFieldWidth, calculatedFieldHight);
  deltaSize = fieldSize * multiplier;
}

void GameGrid::updateLabelContent() {
  switch (gridType) {
  case GridType::BOARD:
    label = TextFormat("Player: %s",
                       gameManager.getPlayerName().c_str()); // TODO! This could be set once - player name never changes
    break;
  case GridType::RADAR:
    if (gameManager.getEnemyName().empty()) {
      label = TextFormat("Waiting for the enemy");
    } else {
      label = TextFormat("Enemy: %s", gameManager.getEnemyName().c_str());
    }
    break;
  }
}

/// @brief Update the position of grid rectangle, making it fit the last field.
void GameGrid::updateGridRect() {
  float lastFieldX = gridRect.x + (columns - 1) * deltaSize;
  float lastFieldY = gridRect.y + (rows - 1) * deltaSize;

  gridRect = Rectangle{
      gridRect.x, gridRect.y, ((lastFieldX + fieldSize) - gridRect.x), ((lastFieldY + fieldSize) - gridRect.y)};
}

/// @brief Update the position of label
void GameGrid::updateLabelPos() {
  fontSize = gridRect.y * 0.5f;
  label_x = gridRect.x;
  label_y = gridRect.y * 0.5;

  textWidth = MeasureText(label.c_str(), fontSize);
}

/// @brief Set if the whole grid is clickable
void GameGrid::setGridClickable(bool isClickable) {
  isActive = isClickable;
}

/// @brief Update the state of every field in the grid.
void GameGrid::updateFieldsState() {
  switch (gridType) {
  case GridType::BOARD:
    for (size_t column = 0; column < columns; column++) {
      for (size_t row = 0; row < rows; row++) {
        getField(row, column).setState(gameManager.getBoardField(row, column));
      }
    }
    break;
  case GridType::RADAR:
    for (size_t column = 0; column < columns; column++) {
      for (size_t row = 0; row < rows; row++) {
        getField(row, column).setState(gameManager.getRadarField(row, column));
      }
    }
    break;
  }
}

// TODO! Use this only on status change
/// @brief Set the active grid clickable
void GameGrid::updateGridState() {
  switch (gameManager.getCurrentGameStatus()) {
  case battleship::networking::GameStatus::LOBBY:
    break;
  case battleship::networking::GameStatus::PLACING_SHIPS:
    if (gridType == GridType::BOARD) {
      setGridClickable(true);
    } else {
      setGridClickable(false);
    }
    break;
  case battleship::networking::GameStatus::WAR:
    if (gridType == GridType::BOARD) {
      setGridClickable(false);
    } else {
      setGridClickable(true);
    }
    break;
  case battleship::networking::GameStatus::GAME_FINISH:
    // TODO! Add some summary screen
    break;
  }
}

GameField &GameGrid::getField(unsigned short int row, unsigned short int column) {
  return fields[row * columns + column];
}

const GameField &GameGrid::getField(unsigned short int row, unsigned short int column) const {
  return fields[row * columns + column];
}

GameGrid::GameGrid(gameManager::GameManager &gameManager, GridType type)
    : gameManager(gameManager)
    , gridRect({0, 0, GetScreenWidth() / 2.0f, GetScreenHeight() / 1.0f})
    , columns(gameManager.getBoardWidth())
    , rows(gameManager.getBoardWidth())
    , gridType(type) {
  spdlog::info("[GUI] GameGrid created. Size: {} by {}", rows, columns);
  updateData();
  fields.resize(columns * rows, GameField());

  updateData();
  updateGridRect();
  updateLabelPos();
  updateLabelContent();
}

void GameGrid::toggleHorizontal() {
  isHorizontal = !isHorizontal;
}

bool GameGrid::getIsHorizontal() {
  return isHorizontal;
}

void GameGrid::update() {
  hoveredRow = std::nullopt;
  hoveredColumn = std::nullopt;

  if (IsWindowResized()) {
    updateData();
    updateGridRect();
    updateLabelPos();
  }
  updateLabelContent();
  updateGridState();
  updateFieldsState();

  if (CheckCollisionPointRec(GetMousePosition(), gridRect) && isActive) {
    int relativeX = GetMouseX() - gridRect.x;
    int relativeY = GetMouseY() - gridRect.y;

    int offsetX = relativeX % static_cast<int>(deltaSize);
    int offsetY = relativeY % static_cast<int>(deltaSize);

    int fieldSizeInt = static_cast<int>(fieldSize);

    hoveredColumn = static_cast<unsigned short int>(relativeX / deltaSize);
    hoveredRow = static_cast<unsigned short int>(relativeY / deltaSize);

    if (isHoverValid() && offsetX <= fieldSize && offsetY <= fieldSizeInt &&
        getField(hoveredRow.value(), hoveredColumn.value()).getIsClickable()) {
      SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);

      if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        handleFieldClick();
      }
    }
  }
}

void GameGrid::handleFieldClick() {
  auto &field = getField(hoveredRow.value(), hoveredColumn.value());
  if (field.getIsClickable()) {
    switch (gridType) {
    case GridType::BOARD:
      try {
        gameManager.placeShip(hoveredRow.value(), hoveredColumn.value(), isHorizontal);
        field.setClickable(false);

      } catch (std::invalid_argument &e) {
        spdlog::warn("[GUI] cannot place ship here: {}", e.what());
      }
      break;
    case GridType::RADAR:
      if (gameManager.isMyTurn()) {
        gameManager.makeShot(hoveredRow.value(), hoveredColumn.value());
        field.setClickable(false); // we set as unclickable only if shot was made during players turn
      }
      break;
    }
  }
}

bool GameGrid::isHoverValid() const {
  return (hoveredRow && hoveredRow.value() < rows && hoveredColumn && hoveredColumn.value() < columns);
}

void GameGrid::draw() {
  DrawRectangleRec(gridRect, WHITE);
  for (size_t column = 0; column < columns; column++) {
    for (size_t row = 0; row < rows; row++) {
      float x = gridRect.x + column * deltaSize;
      float y = gridRect.y + row * deltaSize;
      const auto &field = getField(row, column);

      switch (field.getState()) {
      case battleship::logic::FieldState::EMPTY:
        DrawRectangleRec({x, y, fieldSize, fieldSize}, BLUE);
        break;
      case battleship::logic::FieldState::HIT:
        DrawRectangleRec({x, y, fieldSize, fieldSize}, ORANGE);
        break;
      case battleship::logic::FieldState::MISSED:
        DrawRectangleRec({x, y, fieldSize, fieldSize}, GRAY);
        break;
      case battleship::logic::FieldState::SUNK:
        DrawRectangleRec({x, y, fieldSize, fieldSize}, RED);
        break;
      case battleship::logic::FieldState::TAKEN:
        DrawRectangleRec({x, y, fieldSize, fieldSize}, BLACK);
        break;
      }
    }
  }
  DrawText(label.c_str(), label_x, label_y, fontSize, WHITE);
  drawHighlitedField();
}

Rectangle GameGrid::getGridRect() {
  return gridRect;
}

void GameGrid::drawHighlitedField() {
  if (isHoverValid()) {
    const auto &field = getField(hoveredRow.value(), hoveredColumn.value());
    if (isActive && field.getIsClickable()) {
      float x = gridRect.x + hoveredColumn.value() * deltaSize;
      float y = gridRect.y + hoveredRow.value() * deltaSize;
      DrawRectangleRec(Rectangle{x, y, fieldSize, fieldSize}, GREEN);
    }
  }
}

} // namespace gui
} // namespace battleship

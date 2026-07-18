#pragma once

#include "game_manager.hpp"
#include "scenes/game/game_field.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

class GameGrid {
  gameManager::GameManager &gameManager;
  Rectangle gridRect;
  std::vector<GameField> fields;
  std::optional<unsigned short int> hoveredRow = std::nullopt, hoveredColumn = std::nullopt;

  bool isHorizontal = true;
  bool isActive = false;

  float padding_x, padding_y, multiplier = 1.2f;
  float begin_y_pos;
  float current_y_pos;
  float current_x_pos;
  // size of the board doesn't change through the game
  const float columns, rows;
  float calculatedFieldWidth, calculatedFieldHight;

  float fieldSize;
  float deltaSize;

  float label_x, label_y, fontSize = 12, textWidth;
  std::string label;

  /// @brief Update every field dependent on window size
  void updateData();

  /// @brief Update the position of grid rectangle, making it fit the last field.
  void updateGridRect();

  /// @brief Update the position of label
  void updateLabelPos();
  void updateLabelContent();

  /// @brief Make the grid clickable
  void setGridClickable(bool isClickable);

  /// @brief Update the state of every field in the grid.
  void updateFieldsState();

  // TODO! Use this only on status change
  /// @brief Set the active grid clickable
  void updateGridState();

  void drawHighlitedField();

  void handleFieldClick();

  GameField &getField(unsigned short int row, unsigned short int column);
  const GameField &getField(unsigned short int row, unsigned short int column) const;

  bool isHoverValid() const;

public:
  enum class GridType { BOARD, RADAR };
  GridType gridType;

  GameGrid(gameManager::GameManager &gameManager, GridType type);

  void toggleHorizontal();
  bool getIsHorizontal();

  void update();
  void draw();
  Rectangle getGridRect();
};

} // namespace gui
} // namespace battleship

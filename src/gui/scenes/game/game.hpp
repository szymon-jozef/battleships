#pragma once

#include "../../models/gui_models.hpp"
#include "../game_logic/logic_models.hpp"
#include "../scene.hpp"
#include "data_types.hpp"
#include "game_manager.hpp"
#include "server.hpp"
#include <raylib.h>
#include <thread>

namespace battleship {
namespace gui {

class GameField {
  Rectangle fieldRect;
  logic::FieldState state = logic::FieldState::EMPTY;
  bool isClickable = false;
  std::function<void()> onClick;

public:
  GameField(Rectangle fieldRect);

  void update();
  void draw();

  void setClickable(bool isClickable);
  void setState(logic::FieldState state);
  void setOnClick(std::function<void()> onClick);
  void setPos(Rectangle pos);

  Rectangle *getRect();
  void onHover();
};

class GameGrid {
  gameManager::GameManager &gameManager;
  Rectangle gridRect;
  std::vector<std::vector<GameField>> fields;

  Rectangle *lastFieldRect = nullptr;
  bool isHorizontal = true;

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
  void updateLabel();

  /// @brief Make the grid clickable
  void setGridClickable(bool isClickable);

  /// @brief Update the state of every field in the grid.
  void updateFieldsState();

  // TODO! Use this only on status change
  /// @brief Set the active grid clickable
  void updateGridState();

  /// @brief Update the position of every field in the grid
  void updateFieldsPos();

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

class Game : public Scene {
  gameManager::GameManager gameManager;
  GameGrid board, radar;

  std::unique_ptr<networking::Server> server;
  std::thread serverThread, clientThread;

  // TODO! This sucks, fix it and make it pretty
  std::string gameStatusLabel;
  Color labelColor = WHITE;
  float label_x, label_y, labelFontSize, labelWidth, labelBottomEdge, labelRemainingSpace, labelMaxTotalTextHeight,
      labelActualTextHeight, defaultLabelSize;
  int lineCount = 1;

  networking::GameStatus prevGameStatus = gameManager.getCurrentGameStatus();

  void updateLabels();
  void drawLabels();
  bool isGameStatusChanged();

public:
  Game(GameContext &gameContext, Texture2D &background);
  ~Game();

  void update() override;
  void draw() override;
};

} // namespace gui
} // namespace battleship

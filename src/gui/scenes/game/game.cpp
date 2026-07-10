#include "game.hpp"
#include "models/data_structures.hpp"
#include <raylib.h>

namespace battleship {
namespace gui {

// === GameField ===
GameField::GameField(Rectangle fieldRect)
    : fieldRect(fieldRect) {}

void GameField::update() {
  if (onClick && isClickable && CheckCollisionPointRec(GetMousePosition(), fieldRect) &&
      IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    onClick();
  }
}

void GameField::draw() {
  switch (state) {
  case battleship::logic::FieldState::EMPTY:
    DrawRectangleRec(fieldRect, BLUE);
    break;
  case battleship::logic::FieldState::HIT:
    DrawRectangleRec(fieldRect, ORANGE);
    break;
  case battleship::logic::FieldState::MISSED:
    DrawRectangleRec(fieldRect, GRAY);
    break;
  case battleship::logic::FieldState::SUNK:
    DrawRectangleRec(fieldRect, RED);
    break;
  case battleship::logic::FieldState::TAKEN:
    DrawRectangleRec(fieldRect, BLACK);
    break;
  }
  onHover();
}

void GameField::setClickable(bool isClickable) {
  this->isClickable = isClickable;
}

void GameField::setState(logic::FieldState state) {
  this->state = state;
}

void GameField::setOnClick(std::function<void()> onClick) {
  this->onClick = onClick;
}

void GameField::setPos(Rectangle pos) {
  fieldRect = pos;
}

Rectangle *GameField::getRect() {
  return &fieldRect;
}

void GameField::onHover() {
  if (isClickable && CheckCollisionPointRec(GetMousePosition(), fieldRect)) {
    DrawRectangleLinesEx(fieldRect, 1, GREEN);
  }
}

// === GameGrid ===

void GameGrid::updateData() {
  padding_y = GetScreenHeight() / 10.0f;
  padding_x = GetScreenWidth() / 10.0f;

  gridRect.width = GetScreenWidth() * 0.5f - (2 * padding_x);

  gridRect.y = 0 + padding_y;
  gridRect.height = GetScreenHeight() - (2 * padding_y);

  switch (gridType) {
  case GridType::BOARD:
    gridRect.x = 0 + padding_x;
    label = TextFormat("Player: %s",
                       gameManager.getPlayerName().c_str()); // TODO! This could be set once - player name never changes
    break;
  case GridType::RADAR:
    gridRect.x = GetScreenWidth() / 2.0f + padding_x;

    if (gameManager.getEnemyName().empty()) {
      label = TextFormat("Waiting for the enemy");
    } else {
      label = TextFormat("Enemy: %s", gameManager.getEnemyName().c_str());
    }
    break;
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

/// @brief Update the position of grid rectangle, making it fit the last field.
void GameGrid::updateGridRect() {
  if (!lastFieldRect) {
    spdlog::warn("[GUI] lastFieldRect is a nullptr?");
    return;
  }

  gridRect = Rectangle{gridRect.x,
                       gridRect.y,
                       ((lastFieldRect->x + lastFieldRect->width) - gridRect.x),
                       ((lastFieldRect->y + lastFieldRect->height) - gridRect.y)};
}

/// @brief Update the position of label
void GameGrid::updateLabel() {
  fontSize = gridRect.y * 0.5f;
  label_x = gridRect.x;
  label_y = gridRect.y * 0.5;

  textWidth = MeasureText(label.c_str(), fontSize);
}

void GameGrid::setGridClickable(bool isClickable) {
  for (auto &column : fields) {
    for (auto &field : column) {
      field.setClickable(isClickable);
    }
  }
}

/// @brief Update the state of every field in the grid.
void GameGrid::updateFieldsState() {
  switch (gridType) {
  case GridType::BOARD:
    for (size_t column = 0; column < columns; column++) {
      for (size_t row = 0; row < rows; row++) {
        fields[column][row].setState(gameManager.getBoardField(row, column));
        fields[column][row].update();
      }
    }
    break;
  case GridType::RADAR:
    for (size_t column = 0; column < columns; column++) {
      for (size_t row = 0; row < rows; row++) {
        fields[column][row].setState(gameManager.getRadarField(row, column));
        fields[column][row].update();
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

/// @brief Update the position of every field in the grid
void GameGrid::updateFieldsPos() {
  current_x_pos = gridRect.x;
  current_y_pos = gridRect.y;

  for (unsigned short int columnIndex = 0; columnIndex < columns; columnIndex++) {
    auto &column = fields[columnIndex];

    for (int fieldIndex = 0; fieldIndex < rows; fieldIndex++) {
      fields[columnIndex][fieldIndex].setPos(Rectangle{current_x_pos, current_y_pos, fieldSize, fieldSize});
      current_y_pos += deltaSize;
    }
    current_y_pos = begin_y_pos;
    current_x_pos += deltaSize;
  }
}

GameGrid::GameGrid(gameManager::GameManager &gameManager, GridType type)
    : gameManager(gameManager)
    , gridType(type)
    , columns(gameManager.getBoardWidth())
    , rows(gameManager.getBoardWidth())
    , gridRect({0, 0, GetScreenWidth() / 2.0f, GetScreenHeight() / 1.0f}) {
  spdlog::info("[GUI] GameGrid created. Size: {} by {}", rows, columns);
  updateData();
  fields.resize(columns);

  for (unsigned short int columnIndex = 0; columnIndex < columns; columnIndex++) {
    auto &column = fields[columnIndex];

    for (int fieldIndex = 0; fieldIndex < rows; fieldIndex++) {
      column.emplace_back(Rectangle{current_x_pos, current_y_pos, fieldSize, fieldSize});

      switch (gridType) {
      case GridType::BOARD:
        column.back().setOnClick([this, fieldIndex, columnIndex, &gameManager]() {
          try {
            gameManager.placeShip(fieldIndex, columnIndex, isHorizontal);
          } catch (std::invalid_argument &e) {
            spdlog::warn("[GUI] {}", e.what());
          }
        });
        break;
      case GridType::RADAR:
        column[fieldIndex].setOnClick(
            [this, fieldIndex, columnIndex, &gameManager]() { gameManager.makeShot(fieldIndex, columnIndex); });
        break;
      }
    }

    current_y_pos = begin_y_pos;
    current_x_pos += deltaSize;
  }

  lastFieldRect = fields.back().back().getRect();

  gridRect.x += padding_x;
  gridRect.width -= padding_x * 2.0f;

  gridRect.y += padding_y;
  gridRect.height -= padding_y * 2.0f;

  updateData();
  updateFieldsPos();
  updateGridRect();
  updateLabel();
}

void GameGrid::toggleHorizontal() {
  isHorizontal = !isHorizontal;
}

bool GameGrid::getIsHorizontal() {
  return isHorizontal;
}

void GameGrid::update() {
  if (IsWindowResized()) {
    updateData();
    updateFieldsPos();
    updateGridRect();
    updateLabel();
  }
  updateGridState();
  updateFieldsState();
}

void GameGrid::draw() {
  DrawRectangleRec(gridRect, WHITE);
  for (auto &column : fields) {
    for (auto &field : column) {
      field.draw();
    }
  }
  DrawText(label.c_str(), label_x, label_y, fontSize, WHITE);
}

Rectangle GameGrid::getGridRect() {
  return gridRect;
}

/// === Game Scene ===
void Game::updateLabels() {
  // set text
  switch (gameManager.getCurrentGameStatus()) {
  case networking::GameStatus::LOBBY:
    gameStatusLabel = "Waiting for players...\n";
    labelColor = WHITE;
    break;
  case networking::GameStatus::PLACING_SHIPS: {
    auto shipType = gameManager.getCurrentShip();
    if (shipType) {
      // TODO! Show how many ships of given type are left
      gameStatusLabel =
          TextFormat("Place your ships!\nYour current ship has %d masts\nShip will be placed %s\nPress space to turn",
                     static_cast<int>(shipType.value()),
                     board.getIsHorizontal() ? "horizontally" : "vertically");

      labelColor = GREEN;
    } else {
      gameStatusLabel = "No ships left!";
      labelColor = GRAY;
    }
    break;
  }
  case networking::GameStatus::WAR:
    if (gameManager.isMyTurn()) {
      gameStatusLabel = "Shot captain!";
      labelColor = BLUE;

    } else {
      gameStatusLabel = "Brace yourself!";
      labelColor = RED;
    }
    break;
  case networking::GameStatus::GAME_FINISH:
    spdlog::info("[GUI] game has ended. GAME_FINISH screen should show now");
    gameStatusLabel = "Game has ended!";
    gameContext.loserName = gameManager.getLoserName();
    gameContext.isWon = gameManager.isGameWon();
    gameContext.guiState = GuiState::GAME_FINISH; // TODO! Perhaps to this on key press?
    break;
  }

  // measure it and stuff

  Rectangle boardGrid = board.getGridRect();
  labelBottomEdge = boardGrid.y + boardGrid.height;
  labelRemainingSpace = GetScreenHeight() - labelBottomEdge;

  lineCount = std::count(gameStatusLabel.begin(), gameStatusLabel.end(), '\n') + 1;
  labelMaxTotalTextHeight = labelRemainingSpace * 0.8f;

  defaultLabelSize = boardGrid.y * 0.6f;

  labelFontSize = std::min(defaultLabelSize, (labelRemainingSpace / 0.8f) / lineCount);

  labelWidth = MeasureText(gameStatusLabel.c_str(), labelFontSize);
  if (labelWidth > GetScreenWidth()) {
    labelFontSize *= GetScreenWidth() / labelWidth;
    labelWidth = MeasureText(gameStatusLabel.c_str(), labelFontSize);
  }

  label_x = (GetScreenWidth() - labelWidth) * 0.5f;
  labelActualTextHeight = labelFontSize * lineCount;
  label_y = labelBottomEdge + (labelRemainingSpace - labelActualTextHeight) * 0.5f;
}

void Game::drawLabels() {
  DrawText(gameStatusLabel.c_str(), label_x, label_y, labelFontSize, labelColor);
}

bool Game::isGameStatusChanged() {
  if (prevGameStatus != gameManager.getCurrentGameStatus()) {
    prevGameStatus = gameManager.getCurrentGameStatus();
    return true;
  }
  return false;
}

Game::Game(GameContext &gameContext, Texture2D &background)
    : Scene(gameContext, background)
    , gameManager(gameContext.playerName, gameContext.serverUrl, gameContext.serverPort)
    , board(gameManager, GameGrid::GridType::BOARD)
    , radar(gameManager, GameGrid::GridType::RADAR) {
  spdlog::info("[GUI] Game constructor run");

  std::string prevUrl = gameContext.serverUrl;
  if (gameContext.currentGameMode == GameContext::GameMode::HOSTING) {
    server = std::make_unique<networking::Server>(gameContext.serverPort);
    if (!server->start()) {
      spdlog::error("[GUI] could not start the server...");
      return;
    }

    spdlog::info("[GUI] Initializing server thread");
    serverThread = std::thread([this]() {
      while (!server->isGameEnd() && server->isServerRunning()) {
        spdlog::info("[GUI] server updated!");
        server->update();
      }
    });
    spdlog::info("[GUI] server thread initialized!");
    gameContext.serverUrl = "127.0.0.1";
  }

  if (!gameManager.connect()) {
    spdlog::error("[GUI] could not connect to the server!");
    return;
  }
  gameContext.serverUrl = prevUrl;

  spdlog::info("[GUI] Initializing server client");
  clientThread = std::thread([this]() {
    while (gameManager.isConnected()) {
      gameManager.updateClient();
    }
  });
  spdlog::info("[GUI] client thread initialized!");
}

Game::~Game() {
  if (server) {
    server->stop();
  }

  gameManager.disconnect();

  if (serverThread.joinable()) {
    serverThread.join();
  }

  if (clientThread.joinable()) {
    clientThread.join();
  }
}

void Game::update() {
  Scene::update();
  board.update();
  radar.update();

  updateLabels(); // we update labels all the time, because checking every frame if the ship type has changed is too
                  // much. TODO! Change this someday

  // TODO! Move this somewhere more appropriate
  if (gameManager.getCurrentGameStatus() == networking::GameStatus::PLACING_SHIPS && IsKeyPressed(KEY_SPACE)) {
    board.toggleHorizontal();
  }
}

void Game::draw() {
  Scene::draw();
  board.draw();
  radar.draw();
  drawLabels();
}

} // namespace gui
} // namespace battleship

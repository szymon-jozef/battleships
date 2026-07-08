#include "../game_logic/logic_models.hpp"
#include "../networking/server.hpp"
#include "data_types.hpp"
#include "game_manager.hpp"
#include "gui_models.hpp"
#include "scene.hpp"
#include <algorithm>
#include <raylib.h>
#include <stdexcept>
#include <thread>

namespace battleship {
namespace gui {

class GameField {
  Rectangle fieldRect;
  logic::FieldState state = logic::FieldState::EMPTY;
  bool isClickable;
  std::function<void()> onClick;

public:
  GameField(Rectangle fieldRect)
      : fieldRect(fieldRect) {}

  void update() {
    if (onClick && isClickable && CheckCollisionPointRec(GetMousePosition(), fieldRect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      onClick();
    }
  }

  void draw() {
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
  }

  void setClickable(bool isClickable) {
    this->isClickable = isClickable;
  }

  void setState(logic::FieldState state) {
    this->state = state;
  }

  void setOnClick(std::function<void()> onClick) {
    this->onClick = onClick;
  }

  void setPos(Rectangle pos) {
    fieldRect = pos;
  }
};

class GameGrid {
  gameManager::GameManager &gameManager;
  Rectangle rect;
  std::vector<std::vector<GameField>> fields;

  bool isHorizontal = true;

  float padding_x, padding_y, multiplier;
  float begin_y_pos;
  float current_y_pos;
  float current_x_pos;
  // size of the board doesn't change through the game
  const float columns, rows;
  float calculatedFieldWidth, calculatedFieldHight;

  float fieldSize;
  float deltaSize;

  void updateData() {
    padding_y = GetScreenHeight() / 10.0f;
    padding_x = GetScreenWidth() / 10.0f;

    rect.width = GetScreenWidth() * 0.5f - (2 * padding_x);

    rect.y = 0 + padding_y;
    rect.height = GetScreenHeight() - (2 * padding_y);

    switch (gridType) {
    case GridType::BOARD:
      rect.x = 0 + padding_x;
      break;
    case GridType::RADAR:
      rect.x = GetScreenWidth() / 2.0f + padding_x;
      break;
    }

    multiplier = 1.2;

    begin_y_pos = rect.y;
    current_y_pos = begin_y_pos;
    current_x_pos = rect.x;

    calculatedFieldWidth = rect.width / (((columns - 1.0f) * multiplier) + 1.0f);
    calculatedFieldHight = rect.height / (((rows - 1.0f) * multiplier) + 1.0f);

    fieldSize = std::min(calculatedFieldWidth, calculatedFieldHight);
    deltaSize = fieldSize * multiplier;
  }

  void setFieldsClickable(bool isClickable) {
    for (auto &column : fields) {
      for (auto &field : column) {
        field.setClickable(isClickable);
      }
    }
  }

  void updateFields() {
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
  void updateState() {
    switch (gameManager.getCurrentGameStatus()) {
    case battleship::networking::GameStatus::LOBBY:
      break;
    case battleship::networking::GameStatus::PLACING_SHIPS:
      if (gridType == GridType::BOARD) {
        setFieldsClickable(true);
      } else {
        setFieldsClickable(false);
      }
      break;
    case battleship::networking::GameStatus::WAR:
      if (gridType == GridType::BOARD) {
        setFieldsClickable(false);
      } else {
        setFieldsClickable(true);
      }
      break;
    case battleship::networking::GameStatus::GAME_FINISH:
      // TODO! Add some summary screen
      break;
    }
  }

  void updateFieldsPos() {

    current_x_pos = rect.x;
    current_y_pos = rect.y;

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

public:
  enum class GridType { BOARD, RADAR };
  GridType gridType;

  GameGrid(gameManager::GameManager &gameManager, GridType type)
      : gameManager(gameManager)
      , gridType(type)
      , columns(gameManager.getBoardWidth())
      , rows(gameManager.getBoardWidth())
      , rect({0, 0, GetScreenWidth() / 2.0f, GetScreenHeight() / 1.0f}) {
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

    rect.x += padding_x;
    rect.width -= padding_x * 2.0f;

    rect.y += padding_y;
    rect.height -= padding_y * 2.0f;

    updateFieldsPos();
  }

  // TODO implement
  void toggleHorizontal() {
    isHorizontal = !isHorizontal;
  }

  void update() {
    updateData();
    updateState();
    updateFields();
    updateFieldsPos();
  }

  void draw() {
    DrawRectangleRec(rect, BLACK);
    for (auto &column : fields) {
      for (auto &field : column) {
        field.draw();
      }
    }
  }
};

class Game : public Scene {
  gameManager::GameManager gameManager;
  GameGrid board, radar;

  std::unique_ptr<networking::Server> server;

  std::thread serverThread, clientThread;

public:
  Game(GameContext &gameContext)
      : Scene(gameContext)
      , gameManager(gameContext.playerName, gameContext.serverUrl, gameContext.serverPort)
      , board(gameManager, GameGrid::GridType::BOARD)
      , radar(gameManager, GameGrid::GridType::RADAR) {
    spdlog::info("[GUI] Game constructor run");

    background = LoadTexture("assets/gfx/play_background.jpg");

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

  ~Game() {
    UnloadTexture(background);

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

  void update() override {
    Scene::update();
    board.update();
    radar.update();
  }

  void draw() override {
    Scene::draw();
    board.draw();
    radar.draw();
  }
};

} // namespace gui
} // namespace battleship

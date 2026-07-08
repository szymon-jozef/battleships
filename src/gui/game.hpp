#include "../game_logic/logic_models.hpp"
#include "../networking/server.hpp"
#include "data_types.hpp"
#include "game_manager.hpp"
#include "gui_models.hpp"
#include "scene.hpp"
#include <algorithm>
#include <raylib.h>
#include <thread>

namespace battleship {
namespace gui {

class GameField {
  Rectangle rect;
  logic::FieldState state = logic::FieldState::EMPTY;
  bool isClickable;
  std::function<void()> onClick;

public:
  GameField(float pos_x, float pos_y, float width, float height) {
    rect = {pos_x, pos_y, width, height};
  }

  void update() {
    if (onClick && isClickable && CheckCollisionPointRec(GetMousePosition(), rect) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      onClick();
    }
  }

  void draw() {
    switch (state) {
    case battleship::logic::FieldState::EMPTY:
      DrawRectangleRec(rect, BLUE);
      break;
    case battleship::logic::FieldState::HIT:
      DrawRectangleRec(rect, ORANGE);
      break;
    case battleship::logic::FieldState::MISSED:
      DrawRectangleRec(rect, GRAY);
      break;
    case battleship::logic::FieldState::SUNK:
      DrawRectangleRec(rect, RED);
      break;
    case battleship::logic::FieldState::TAKEN:
      DrawRectangleRec(rect, BLACK);
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
};

class GameGrid {

public:
  enum class GridType { BOARD, RADAR };
  GameGrid(gameManager::GameManager &gameManager, GridType type, float grid_pos_x, float grid_pos_y)
      : gameManager(gameManager)
      , gridType(type)
      , rect({grid_pos_x, grid_pos_y, GetScreenWidth() / 2.0f, static_cast<float>(GetScreenHeight())}) {
    spdlog::info("[GUI] GameGrid created. Size: {} by {}", gameManager.getBoardWidth(), gameManager.getBoardHeight());

    float padding_y = GetScreenHeight() / 10.0f;
    float padding_x = GetScreenWidth() / 10.0f;

    float multiplier = 1.2;

    rect.x += padding_x;
    rect.width -= padding_x * 2.0f;

    rect.y += padding_y;
    rect.height -= padding_y * 2.0f;

    float begin_y_pos = rect.y;
    float current_y_pos = begin_y_pos;
    float current_x_pos = rect.x;

    float columns = gameManager.getBoardWidth();
    float rows = gameManager.getBoardHeight();

    float calculatedWidth = rect.width / (((columns - 1.0f) * multiplier) + 1.0f);
    float calculatedHeight = rect.height / (((rows - 1.0f) * multiplier) + 1.0f);

    float fieldSize = std::min(calculatedWidth, calculatedHeight);

    float delta = fieldSize * multiplier;

    fields.resize(gameManager.getBoardWidth());

    for (unsigned short int columnIndex = 0; columnIndex < gameManager.getBoardWidth(); columnIndex++) {
      auto &column = fields[columnIndex];

      for (int fieldIndex = 0; fieldIndex < gameManager.getBoardHeight(); fieldIndex++) {
        column.emplace_back(current_x_pos, current_y_pos, fieldSize, fieldSize);
        current_y_pos += delta;

        switch (gridType) {
        case GridType::BOARD:
          column.back().setOnClick([this, &gameManager, fieldIndex, columnIndex]() {
            gameManager.placeShip(fieldIndex, columnIndex, isHorizontal);
          });
          break;
        case GridType::RADAR:
          column.back().setOnClick(
              [this, &gameManager, fieldIndex, columnIndex]() { gameManager.makeShot(fieldIndex, columnIndex); });
          break;
        }
      }
      current_y_pos = begin_y_pos;
      current_x_pos += delta;
    }
  }

  void toggleHorizontal() {
    isHorizontal = !isHorizontal;
  }

  void update() {
    updateState();
    updateFields();
  }

  void draw() {
    DrawRectangleRec(rect, BLACK);
    for (auto &column : fields) {
      for (auto &field : column) {
        field.draw();
      }
    }
  }

private:
  gameManager::GameManager &gameManager;

  GridType gridType;
  Rectangle rect;
  std::vector<std::vector<GameField>> fields;

  bool isHorizontal = false;

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
      for (size_t column = 0; column < gameManager.getBoardWidth(); column++) {
        for (size_t row = 0; row < gameManager.getBoardHeight(); row++) {
          fields[column][row].setState(gameManager.getBoardField(row, column));
        }
      }
      break;
    case GridType::RADAR:
      break;
    }
  }

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
      // TODO! add some summary screen
      break;
    }
  }
};

class Game : public Scene {
  Texture2D background;
  gameManager::GameManager gameManager;
  GameGrid board, radar;

  std::unique_ptr<networking::Server> server;

  std::thread serverThread, clientThread;

public:
  Game(GameContext &gameContext)
      : Scene(gameContext)
      , gameManager(gameContext.playerName, gameContext.serverUrl, gameContext.serverPort)
      , board(gameManager, GameGrid::GridType::BOARD, 0, 0)
      , radar(gameManager, GameGrid::GridType::RADAR, GetScreenWidth() / 2.0f, 0) {
    spdlog::info("[GUI] Game constructor run");

    background = LoadTexture("assets/gfx/play_background.jpg");

    switch (gameContext.currentGameMode) {
    case GameContext::GameMode::HOSTING: {
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

      std::string prevUrl = gameContext.serverUrl;
      gameContext.serverUrl = "127.0.0.1";

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

      break;
    }
    case GameContext::GameMode::JOINING: {
      break;
    }
    }
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
    board.update();
  }

  void draw() override {
    DrawTexture(background, 0, 0, WHITE);
    board.draw();
    radar.draw();
  }
};

} // namespace gui
} // namespace battleship

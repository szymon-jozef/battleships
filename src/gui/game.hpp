#include "../game_logic/logic_models.hpp"
#include "../networking/server.hpp"
#include "data_types.hpp"
#include "game_manager.hpp"
#include "gui_models.hpp"
#include "scene.hpp"
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
};

class GameGrid {

public:
  enum class GridType { BOARD, RADAR };
  GameGrid(gameManager::GameManager &gameManager, GridType type)
      : gameManager(gameManager)
      , gridType(type) {
    spdlog::info("[GUI] GameGrid created. Size: {} by {}", gameManager.getBoardWidth(), gameManager.getBoardHeight());

    float begin_y_pos = 10;
    float current_x_pos = begin_y_pos, current_y_pos = begin_y_pos; // TODO! change this later
    float width = 40, height = width;

    float deltaPos = width + 10;

    fields.resize(gameManager.getBoardWidth());

    for (auto &column : fields) {
      for (int fieldIndex = 0; fieldIndex < gameManager.getBoardHeight(); fieldIndex++) {
        column.emplace_back(current_x_pos, current_y_pos, width, height);
        current_y_pos += deltaPos;
      }
      current_y_pos = begin_y_pos;
      current_x_pos += deltaPos;
    }
  }

  void update() {
    updateState();
    updateFields();
  }

  void draw() {
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
      , board(gameManager, GameGrid::GridType::BOARD)
      , radar(gameManager, GameGrid::GridType::RADAR) {
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
    DrawTexture(background, 0, 0, GRAY);
    board.draw();
  }
};

} // namespace gui
} // namespace battleship

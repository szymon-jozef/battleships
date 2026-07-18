#include "game.hpp"
#include "models/data_structures.hpp"
#include <raylib.h>
#include <thread>

namespace battleship {
namespace gui {

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
    , gameManager(gameContext.settings.playerName, gameContext.settings.serverUrl, gameContext.settings.serverPort)
    , board(gameManager, GameGrid::GridType::BOARD)
    , radar(gameManager, GameGrid::GridType::RADAR) {
  spdlog::info("[GUI] Game constructor run");

  std::string prevUrl = gameContext.settings.serverUrl;
  if (gameContext.currentGameMode == GameContext::GameMode::HOSTING) {
    server = std::make_unique<networking::Server>(gameContext.settings.serverPort);
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
    gameContext.settings.serverUrl = "127.0.0.1";
  }

  gameManager.connect([&gameContext, this, prevUrl](
                          bool success, const std::string &message) { // TODO! Show the message in some sort of label
    if (!success) {
      spdlog::error("[GUI] connection error: {}", message);
      if (gameContext.currentGameMode == GameContext::GameMode::JOINING) {
        gameContext.guiState = GuiState::MAIN_MENU;
      } else {
        gameContext.settings.serverUrl = prevUrl;
      }
    } else {
      spdlog::info("[GUI] connected to the server!");
      spdlog::info("[GUI] Initializing server client");
      clientThread = std::thread([this]() {
        while (gameManager.isConnected()) {
          gameManager.updateClient();
        }
      });
      spdlog::info("[GUI] client thread initialized!");
    }
  });
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
  SetMouseCursor(MOUSE_CURSOR_DEFAULT);
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

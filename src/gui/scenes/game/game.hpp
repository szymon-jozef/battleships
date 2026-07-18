#pragma once

#include "../../models/gui_models.hpp"
#include "../scene.hpp"
#include "data_types.hpp"
#include "game_grid.hpp"
#include "game_manager.hpp"
#include "server.hpp"
#include <raylib.h>
#include <thread>

namespace battleship {
namespace gui {

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

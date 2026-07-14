#include "gui/main.cpp"
#include <spdlog/spdlog.h>

using namespace battleship::gui;

int main() {
  // TODO args parsing
  // for example for running server standalone

  spdlog::info("[MAIN] Running the game. Current version is: {}", GAME_VERSION);
  return run();
}

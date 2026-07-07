#include "gui/main.hpp"
#include <spdlog/spdlog.h>

using namespace battleship::gui;

int main() {
  // TODO args parsing
  // for example for running server standalone

  spdlog::flush_on(spdlog::level::info);
  return run();
}

// This file is an example use of server, meant for testing purposes

#include "server.hpp"
#include <spdlog/spdlog.h>

int main() {
  spdlog::info("Running server....");
  battleship::networking::Server server(6767);
  if (server.start()) {
    spdlog::info("Serwer is listening...");

    while (true) {
      server.update();
    }
  } else {
    spdlog::error("Server didn't run, you stupid fuck!");
  }

  return 0;
}

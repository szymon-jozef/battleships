// This file is an example use of client, meant for testing purposes

#include "client.hpp"
#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>

int main() {
  spdlog::info("Running client...");
  battleship::networking::Client client;

  if (client.connect("127.0.0.1", 6767)) {
    spdlog::info("Connected!");
    // waiting for connection to establish
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::string name("morbius");

    spdlog::info("Sending handshake...");
    client.sendHandshake(name);
    std::this_thread::sleep_for(std::chrono::seconds(2));
    spdlog::info("Press any key to disconnect");
    getchar();
    spdlog::info("Disconnecting...");
    client.disconnect();
  } else {
    spdlog::error("Server no respondy :(");
  }

  return 0;
}

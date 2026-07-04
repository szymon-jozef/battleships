// This file is an example use of client, meant for testing purposes

#include "client.hpp"
#include <chrono>
#include <iostream>
#include <spdlog/spdlog.h>
#include <thread>

int main() {
  spdlog::info("Running client...");
  battleship::networking::Client client;

  if (client.connect("127.0.0.1", 6767)) {
    spdlog::info("Connected!");
    // waiting for connection to establish
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::string name;
    std::cout << "Enter your name: \n";
    std::cin >> name;

    spdlog::info("Sending handshake...");
    client.sendHandshake(name);
    std::cout << "Click any button when the other client is connected...\n";
    std::cin.ignore(1, '\n');
    getchar();
    client.update();

    spdlog::info("My name and id: {} - {}", boost::uuids::to_string(client.id), client.name);
    spdlog::info("Current game state is: {}", static_cast<int>(client.currentGameStatus));
    int input;
    std::cout << "This client should (1 - win, 2 - lose)\n";
    std::cin >> input;
    switch (input) {
    case 1:
      break;
    case 2:
      client.sendLost();
      break;
    default:
      std::cout << "Invalid input";
      return -1;
    }

    while (true) {
      client.update();

      if (!client.isConnected()) {
        break;
      }
    }

  } else {
    spdlog::error("Server no respondy :(");
  }

  return 0;
}

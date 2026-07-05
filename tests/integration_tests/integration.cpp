#include "../../src/game_manager/game_manager.hpp"
#include "../../src/networking/server.hpp"
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <thread>

TEST_CASE("Full integration test") {
  uint16_t port = 6767;
  battleship::networking::Server server(port);
  server.start();

  battleship::gameManager::GameManager player1("Morbius", "127.0.0.1", port);
  battleship::gameManager::GameManager player2("Spider-mid", "127.0.0.1", port);

  std::thread serverUpdate([&server]() {
    while (true) {
      server.update();
      if (server.isGameEnd()) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  });

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  player1.connect();
  player2.connect();

  std::thread p1Thread([&]() {
    while (true) {
      player1.updateClient();
      if (!player1.isConnected()) {
        break;
      }
    }
  });

  std::thread p2Thread([&]() {
    while (true) {
      player2.updateClient();
      if (!player2.isConnected()) {
        break;
      }
    }
  });

  auto waitForState = [](battleship::gameManager::GameManager &p, battleship::networking::GameStatus target) {
    int timeout = 50;
    while (p.getCurrentGameStatus() != target && timeout-- > 0) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  };

  auto safeShot = [](battleship::gameManager::GameManager &p, int row, int col) {
    p.makeShot(row, col);
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
  };

  waitForState(player1, battleship::networking::GameStatus::PLACING_SHIPS);
  waitForState(player2, battleship::networking::GameStatus::PLACING_SHIPS);

  REQUIRE(player1.getCurrentGameStatus() == battleship::networking::GameStatus::PLACING_SHIPS);
  REQUIRE(player2.getCurrentGameStatus() == battleship::networking::GameStatus::PLACING_SHIPS);

  player1.placeShip(0, 0, true);
  player1.placeShip(2, 0, true);
  player1.placeShip(4, 0, true);
  player1.placeShip(6, 0, true);
  player1.placeShip(8, 0, true);
  player1.placeShip(0, 6, true);
  player1.placeShip(2, 6, true);
  player1.placeShip(4, 6, true);
  player1.placeShip(6, 6, true);
  player1.placeShip(8, 6, true);

  player2.placeShip(0, 0, true);
  player2.placeShip(2, 0, true);
  player2.placeShip(4, 0, true);
  player2.placeShip(6, 0, true);
  player2.placeShip(8, 0, true);
  player2.placeShip(0, 6, true);
  player2.placeShip(2, 6, true);
  player2.placeShip(4, 6, true);
  player2.placeShip(6, 6, true);
  player2.placeShip(8, 6, true);

  waitForState(player1, battleship::networking::GameStatus::WAR);
  waitForState(player2, battleship::networking::GameStatus::WAR);

  REQUIRE(player1.getCurrentGameStatus() == battleship::networking::GameStatus::WAR);
  REQUIRE(player2.getCurrentGameStatus() == battleship::networking::GameStatus::WAR);

  safeShot(player1, 1, 0);
  safeShot(player1, 1, 0);

  safeShot(player2, 0, 0);
  safeShot(player2, 0, 1);
  safeShot(player2, 0, 2);
  safeShot(player2, 0, 3);

  safeShot(player2, 2, 0);
  safeShot(player2, 2, 1);
  safeShot(player2, 2, 2);

  safeShot(player2, 4, 0);
  safeShot(player2, 4, 1);
  safeShot(player2, 4, 2);

  safeShot(player2, 6, 0);
  safeShot(player2, 6, 1);

  safeShot(player2, 8, 0);
  safeShot(player2, 8, 1);

  safeShot(player2, 0, 6);
  safeShot(player2, 0, 7);

  safeShot(player2, 2, 6);
  safeShot(player2, 4, 6);
  safeShot(player2, 6, 6);
  safeShot(player2, 8, 6);

  std::this_thread::sleep_for(std::chrono::milliseconds(200));

  REQUIRE(player1.isGameWon() == false);
  REQUIRE(player2.isGameWon() == true);

  p1Thread.join();
  p2Thread.join();
  server.stop();
  serverUpdate.join();
}

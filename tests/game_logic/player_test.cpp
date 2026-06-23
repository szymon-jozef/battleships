#include "logic_models.hpp"
#include <catch2/catch_test_macros.hpp>
#include <iostream>

TEST_CASE("Player tests", "[player]") {
  std::string name = "morbius";
  std::vector<std::shared_ptr<Ship>> ships;
  ships.emplace_back(std::make_shared<Ship>(ShipType::TwoMaster));
  ships.emplace_back(std::make_shared<Ship>(ShipType::TwoMaster));
  ships.emplace_back(std::make_shared<Ship>(ShipType::TwoMaster));
  Player player = Player(name, ships);

  SECTION("Test player name") {
    REQUIRE(player.getName() == name);
  }

  SECTION("Test player placing all ships") {
    REQUIRE(player.hasShips());
    REQUIRE_THROWS(player.placeShip(-6, 7, true));
    REQUIRE_THROWS(player.placeShip(66, 7, true));
    REQUIRE(player.placeShip(0, 0, true));
    REQUIRE(player.placeShip(2, 0, true));
    REQUIRE(player.placeShip(4, 0, true));
    REQUIRE_FALSE(player.placeShip(6, 7, true));
    REQUIRE_FALSE(player.hasShips());
    std::cout << "Player placed all his ships. Current board state: \n" << player.boardAsString() << std::endl;
  }

  SECTION("Test player marking the radar") {
    REQUIRE(player.getRadarState(0, 0) == FieldState::EMPTY);
    player.markShotResult(FieldState::HIT, 0, 0);
    REQUIRE(player.getRadarState(0, 0) == FieldState::HIT);
    player.markShotResult(FieldState::SUNK, 0, 1);
    REQUIRE(player.getRadarState(0, 1) == FieldState::SUNK);
    player.markShotResult(FieldState::SUNK, 0, 0);
    REQUIRE_THROWS(player.markShotResult(FieldState::HIT, -1, 2));
    REQUIRE_THROWS(player.markShotResult(FieldState::HIT, 11, 22));
    REQUIRE_THROWS(player.markShotResult(FieldState::HIT, 1, 22));
    std::cout << "Player marked his radar. Radar state: \n" << player.radarAsString() << std::endl;
  }

  SECTION("Test player recieving shots") {
    player.placeShip(0, 0, true);
    player.placeShip(2, 0, true);
    player.placeShip(4, 0, true);
    REQUIRE_FALSE(player.isDead());

    REQUIRE(player.recieveShot(0, 0) == FieldState::HIT);
    REQUIRE(player.recieveShot(0, 1) == FieldState::SUNK);
    REQUIRE(player.getBoardState(0, 0) == FieldState::SUNK);
    REQUIRE_FALSE(player.isDead());

    REQUIRE(player.recieveShot(2, 0) == FieldState::HIT);
    REQUIRE(player.recieveShot(2, 1) == FieldState::SUNK);
    REQUIRE_FALSE(player.isDead());

    REQUIRE(player.recieveShot(4, 0) == FieldState::HIT);
    REQUIRE(player.recieveShot(4, 1) == FieldState::SUNK);

    REQUIRE(player.isDead());
    std::cout << "Player has lost! His board state: \n" << player.radarAsString() << std::endl;
  }
}

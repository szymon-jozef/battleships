#include "logic_models.hpp"
#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <stdexcept>

TEST_CASE("Board logic: Ship placement and shooting", "[board]") {

  Board board;

  SECTION("Placing a horizontal ship updates field states properly") {
    auto ship = std::make_shared<Ship>(ShipType::ThreeMaster);
    board.placeShip(ship, 0, 0, true);

    REQUIRE(board.getFieldState(0, 0) == FieldState::TAKEN);
    REQUIRE(board.getFieldState(0, 1) == FieldState::TAKEN);
    REQUIRE(board.getFieldState(0, 2) == FieldState::TAKEN);

    REQUIRE(board.getFieldState(0, 3) == FieldState::EMPTY);
    REQUIRE(board.getFieldState(1, 0) == FieldState::EMPTY);
  }

  SECTION("Taking shots and sinking a ship") {
    auto ship = std::make_shared<Ship>(ShipType::ThreeMaster);
    board.placeShip(ship, 0, 0, true);

    REQUIRE(board.takeShot(0, 0) == FieldState::HIT);
    REQUIRE(board.getFieldState(0, 0) == FieldState::HIT);
    REQUIRE(board.takeShot(0, 1) == FieldState::HIT);

    REQUIRE(board.takeShot(0, 2) == FieldState::SUNK);

    REQUIRE(board.getFieldState(0, 0) == FieldState::SUNK);
    REQUIRE(board.getFieldState(0, 1) == FieldState::SUNK);
  }

  SECTION("Placing a vertical ship (not horizontal)") {
    auto ship = std::make_shared<Ship>(ShipType::TwoMaster);
    board.placeShip(ship, 5, 5, false);

    REQUIRE(board.getFieldState(5, 5) == FieldState::TAKEN);
    REQUIRE(board.getFieldState(6, 5) == FieldState::TAKEN);

    REQUIRE(board.takeShot(5, 5) == FieldState::HIT);
    REQUIRE(board.takeShot(6, 5) == FieldState::SUNK);
  }
}

TEST_CASE("Board throws exceptions for out of bounds inputs",
          "[board][exceptions]") {
  Board board;

  SECTION("Out of bounds in placeShip") {
    auto ship = std::make_shared<Ship>(ShipType::OneMaster);

    REQUIRE_THROWS_AS(board.placeShip(ship, -1, 5, true),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(board.placeShip(ship, 5, -1, true),
                      std::invalid_argument);
    REQUIRE_THROWS_AS(board.placeShip(ship, 2342, 3243, true),
                      std::invalid_argument);
  }

  SECTION("Out of bounds in takeShot") {
    REQUIRE_THROWS(board.takeShot(-10, -10));
    REQUIRE_THROWS(board.takeShot(11, 11));
  }
}

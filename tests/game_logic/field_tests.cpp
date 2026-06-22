#include "logic_models.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Create field and tests it parameters") {
  Field f = Field();
  REQUIRE(f.getState() == FieldState::EMPTY);
  f.setState(FieldState::SUNK);
  REQUIRE(f.getState() == FieldState::SUNK);
}

TEST_CASE("Test placing ship on the field") {
  std::shared_ptr ship = std::make_shared<Ship>(ShipType::OneMaster);
  Field f = Field();
  f.placeShip(ship);
  REQUIRE(f.getShip() == ship.get());
}

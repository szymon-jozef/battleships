#include "logic_models.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Ship sunking test") {
  Ship four = Ship(ShipType::FourMaster);
  REQUIRE(!four.isSunk());
  four.hit();
  REQUIRE(!four.isSunk());
  four.hit();
  REQUIRE(!four.isSunk());
  four.hit();
  four.hit();
  REQUIRE(four.isSunk());
  Ship one = Ship(ShipType::OneMaster);
  REQUIRE(!one.isSunk());
  one.hit();
  REQUIRE(one.isSunk());
};

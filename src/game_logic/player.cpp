#include "logic_models.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

namespace battleship {
namespace logic {

Player::Player(std::string name, unsigned short int width, unsigned short int height)
    : name(std::move(name))
    , board(Board(width, height))
    , radar(Radar(width, height)) {
  spdlog::info("[Logic] Player named {} was created!", this->name);

  const std::array fleet{
      std::pair{ShipType::OneMaster, 4},
      std::pair{ShipType::TwoMaster, 3},
      std::pair{ShipType::ThreeMaster, 2},
      std::pair{ShipType::FourMaster, 1},
  };

  shipsBay.clear();
  shipsBay.reserve(fleet.size());

  for (const auto &[type, count] : fleet) {
    for (int i = 0; i < count; ++i) {
      shipsBay.emplace_back(std::make_shared<Ship>(type));
    }
  }

  ships = shipsBay;
}

Player::Player(const std::string &name)
    : Player(name, 10, 10) {}

Player::Player(std::string name, std::vector<std::shared_ptr<Ship>> ships)
    : name(std::move(name))
    , board()
    , radar()
    , shipsBay(std::move(ships))
    , ships(shipsBay) {
  spdlog::info("[Logic] created test player!");
}

std::optional<ShipType> Player::getShipType() {
  if (!shipsBay.empty()) {
    return shipsBay.back()->getType();
  }
  return std::nullopt;
}

bool Player::placeShip(unsigned short int row, unsigned short int column, bool isHorizontal) {
  if (shipsBay.empty()) {
    spdlog::warn("[Logic] Tried placing a ship at ({}, {}), but the ships vector was empty!", row, column);
    return false;
  }

  std::shared_ptr<Ship> currentShip = shipsBay.back();
  bool result = board.placeShip(currentShip, row, column, isHorizontal);
  if (result) {
    shipsBay.pop_back(); // we take one ship off the stack, after we place it
    return true;
  }
  return false;
}

FieldState Player::recieveShot(unsigned short int row, unsigned short int column) {
  return board.recieveShot(row, column);
}

void Player::markShotResult(FieldState shotResult, unsigned short int row, unsigned short int column) {
  radar.markShotResult(shotResult, row, column);
}

bool Player::isDead() {
  return std::all_of(ships.begin(), ships.end(), [](std::shared_ptr<Ship> s) { return s->isSunk(); });
}

FieldState Player::getBoardState(unsigned short int row, unsigned short int column) {
  return board.getFieldState(row, column);
}

FieldState Player::getRadarState(unsigned short int row, unsigned short int column) {
  return radar.getFieldState(row, column);
}

bool Player::hasShips() {
  return !shipsBay.empty();
}

const std::string &Player::getName() const {
  return name;
}

std::string Player::boardAsString() {
  return board.asString();
}

std::string Player::radarAsString() {
  return radar.asString();
}

bool Player::isPlacementValid(unsigned short int row, unsigned short int column, bool isHorizontal) const {
  if (shipsBay.empty()) {
    return false;
  }

  const auto ship = shipsBay.back().get();
  if (ship) {
    return board.isPlacementValid(row, column, ship, isHorizontal);
  }
  return false;
}

bool Player::hasShips() const {
  return !shipsBay.empty();
}

} // namespace logic
} // namespace battleship

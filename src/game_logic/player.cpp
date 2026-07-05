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
  // there are 10 ships at total
  shipsBay.reserve(10);
  for (int i = 0; i < 4; ++i)
    shipsBay.emplace_back(std::make_shared<Ship>(ShipType::OneMaster));
  for (int i = 0; i < 3; ++i)
    shipsBay.emplace_back(std::make_shared<Ship>(ShipType::TwoMaster));
  for (int i = 0; i < 2; ++i)
    shipsBay.emplace_back(std::make_shared<Ship>(ShipType::ThreeMaster));

  shipsBay.emplace_back(std::make_shared<Ship>(ShipType::FourMaster));
  ships = shipsBay;
}

Player::Player(std::string name)
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
  board.placeShip(currentShip, row, column, isHorizontal);
  shipsBay.pop_back(); // we take one ship off the stack, after we place it
  return true;
}

FieldState Player::recieveShot(unsigned short int row, unsigned short int column) {
  return board.recieveShot(row, column);
}

void Player::markShotResult(FieldState shotResult, unsigned short int row, unsigned short int column) {
  radar.markShotResult(shotResult, row, column);
}

bool Player::isDead() {
  for (auto ship : ships) {
    if (!ship->isSunk()) {
      return false;
    }
  }
  return true;
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

std::string Player::getName() {
  return name;
}

std::string Player::boardAsString() {
  return board.asString();
}

std::string Player::radarAsString() {
  return radar.asString();
}

} // namespace logic
} // namespace battleship

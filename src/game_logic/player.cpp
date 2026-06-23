#include "logic_models.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>

Player::Player(std::string name, unsigned short int width, unsigned short int height)
    : name(std::move(name))
    , id(boost::uuids::random_generator()())
    , board(Board(width, height))
    , radar(Radar(width, height)) {
  spdlog::info("[Logic] Player named {} with ID: {} was created!", name, boost::lexical_cast<std::string>(id));
  // there are 10 ships at total
  ships.reserve(10);
  for (int i = 0; i < 4; ++i)
    ships.emplace_back(std::make_shared<Ship>(ShipType::OneMaster));
  for (int i = 0; i < 3; ++i)
    ships.emplace_back(std::make_shared<Ship>(ShipType::TwoMaster));
  for (int i = 0; i < 2; ++i)
    ships.emplace_back(std::make_shared<Ship>(ShipType::ThreeMaster));

  ships.emplace_back(std::make_shared<Ship>(ShipType::FourMaster));
}

Player::Player(std::string name)
    : Player(name, 10, 10) {}

Player::Player(std::string name, std::vector<std::shared_ptr<Ship>> ships)
    : name(std::move(name))
    , id(boost::uuids::random_generator()())
    , board()
    , radar()
    , ships(std::move(ships)) {
  spdlog::info("[Logic] created test player!");
}

std::optional<ShipType> Player::getShipType() {
  if (!ships.empty()) {
    return ships.back()->getType();
  }
  return std::nullopt;
}

void Player::placeShip(unsigned short int row, unsigned short int column, bool isHorizontal) {
  if (ships.empty()) {
    spdlog::warn("[Logic] Tried placing a ship at ({}, {}), but the ships vector was empty!", row, column);
    return;
  }

  std::shared_ptr<Ship> currentShip = ships.back();
  ships.pop_back(); // we take one ship off the stack

  board.placeShip(currentShip, row, column, isHorizontal);
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
  return !ships.empty();
}

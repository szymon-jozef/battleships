#pragma once

#include <boost/uuid/uuid.hpp>
#include <memory>
#include <string>
#include <vector>

/* --- Ships --- */

/// ShipType defines how many field the ship takes
enum class ShipType {
  FourMaster = 4,
  ThreeMaster = 3,
  TwoMaster = 2,
  OneMaster = 1
};

/// Every ship has it's own type, UUID and hp.
/// HP is the same as ship length
class Ship {
  boost::uuids::uuid id;
  ShipType type;
  unsigned int health;

public:
  Ship(ShipType type);
  bool isSunk() const;
  /// Decrement ship health by one
  void hit();
};

/* --- Fields --- */
enum class FieldState { EMPTY, TAKEN, MISSED, HIT, SUNK };

/// Every field stores it's  current state and a pointer to the ship
/// that's there, if any. Otherwise nullptr. By default every FieldState is
/// FieldState::EMPTY
class Field {
  FieldState state;
  // One ship may be on multiple fields so we use an shared_ptr
  std::shared_ptr<Ship> morred_ship;

public:
  Field();
  void placeShip(std::shared_ptr<Ship> ship);
  void setState(FieldState new_state);
  FieldState getState() const;
  Ship *getShip() const;
};

enum class ShotResult {
  MISS,
  HIT,
  SUNK,
  ALREADY_SHOT,
  OUT_OF_BOUNDS,
};

/* --- Grids --- */
class BaseGrid {
  // we use short int, because no one wants to play with 23894723984723489
  // fields
  const unsigned short int WIDTH;
  const unsigned short int HEIGHT;
  // should reserve width * height in the constructor
  std::vector<std::vector<Field>> grid;

public:
  BaseGrid(const unsigned short int width, const unsigned short int height);
  FieldState getFieldState(unsigned short int row,
                           unsigned short int column) const;
  virtual std::string asString() const;
};

class Board : public BaseGrid {
public:
  Board(const unsigned short int width, const unsigned short int height);
  bool placeShip(ShipType shipType, unsigned short int startRow,
                 unsigned short int startColumn, bool isHorizontal);
  ShotResult takeShot(unsigned short int row, unsigned short int column);
  virtual std::string asString() const override;
};

class Radar : public BaseGrid {
public:
  Radar(const unsigned short int width, const unsigned short int height);
  void markShotResult(unsigned short int row, unsigned short int);
};

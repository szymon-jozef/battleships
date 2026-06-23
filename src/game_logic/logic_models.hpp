#pragma once

#include <boost/uuid/uuid.hpp>
#include <memory>
#include <string>
#include <vector>

/* --- Ships --- */

/// @brief ShipType defines how many field the ship takes
enum class ShipType { FourMaster = 4, ThreeMaster = 3, TwoMaster = 2, OneMaster = 1 };

/// @brief Every ship has it's own type, UUID and hp.
/// HP is the same as ship length
class Ship {
  ShipType type;
  unsigned int health;

public:
  Ship(ShipType type);
  bool isSunk() const;
  /// Decrement ship health by one
  void hit();
  ShipType getType() const;
};

/* --- Fields --- */
enum class FieldState : char { EMPTY = 'O', TAKEN = 'T', MISSED = 'M', HIT = 'x', SUNK = 'X' };

/// @brief Every field stores it's  current state and a pointer to the ship
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

class BaseGrid {
  // we use short int, because no one wants to play with 23894723984723489
  // fields
protected:
  const unsigned short int WIDTH;
  const unsigned short int HEIGHT;
  std::vector<std::vector<Field>> grid;
  /// @brief Uses BFS to mark all nearby Fields that are hit as sunk
  void markNearbyAsSunk(unsigned short int row, unsigned short int column);

public:
  BaseGrid(const unsigned short int width, const unsigned short int height);
  FieldState getFieldState(unsigned short int row, unsigned short int column) const;

  std::string asString() const;
};

class Board : public BaseGrid {
public:
  /// @brief Create 10x10 board
  Board();
  /// @brief Create `width` x `height` board
  Board(const unsigned short int width, const unsigned short int height);

  /// @brief Attempts to place a ship at given coordinates.
  ///
  /// @throws:
  /// - std::invalid_argument when input is out of bounds
  bool
  placeShip(std::shared_ptr<Ship> ship, unsigned short int startRow, unsigned short int startColumn, bool isHorizontal);

  /// @brief Check if there is any ship at given coordinates mark the field as
  /// the result. Hits the ship if present.
  ///
  /// @return result of the shot as `FieldState`
  ///
  /// @throws
  /// - logic_error when can't retrieve the ship under taken field
  /// - invalid_argument when coords are out of bounds
  FieldState recieveShot(unsigned short int row, unsigned short int column);
};

class Radar : public BaseGrid {
public:
  /// @brief Default constructor defaults to 10x10 size
  Radar();
  Radar(const unsigned short int width, const unsigned short int height);
  void markShotResult(FieldState shotResult, unsigned short int row, unsigned short int column);
};

/* --- Player --- */

/// @brief  Main abstraction over the game logic. All logic operations should be done through this class.
///
/// Every player has it's own instance of board and radar.
/// Ships live under `std::vector<Ship>`. There are:
/// - 4 onemasters
/// - 3 twomasters
/// - 2 threemasterse
/// - 1 fourmaster
///
/// Every player has his own unique ID.
///
/// Available methods:
/// - `placeShip` for placing the ship
/// - `recieveShot` for taking the shot and returning the result
/// - markShotResult for marking the shot result on your radar
/// - `isDead` for checking if player has any ship left
/// - `getBoardState`
/// - `getRadarState`
/// - `hasShips`
class Player {
  std::string name;
  boost::uuids::uuid id;
  Board board;
  Radar radar;
  std::vector<std::shared_ptr<Ship>> ships;

public:
  /// Defaults to board and radar size `10 x 10`
  Player(std::string name);
  Player(std::string name, unsigned short int width, unsigned short int height);
  /// Constructor for test purpose
  Player(std::string name, std::vector<std::shared_ptr<Ship>> ships);

  /// @return Ship type at the end of ships vector. Return nullopt if empty
  std::optional<ShipType> getShipType();
  /// @brief Places the ship at the end of ships vector.
  void placeShip(unsigned short int row, unsigned short int column, bool isHorizontal);
  FieldState recieveShot(unsigned short int row, unsigned short int column);
  void markShotResult(FieldState shotResult, unsigned short int row, unsigned short int column);
  bool isDead();
  FieldState getBoardState(unsigned short int row, unsigned short int column);
  FieldState getRadarState(unsigned short int row, unsigned short int column);
  /// @return True if ships vector isn't empty. False otherwise
  bool hasShips();
};

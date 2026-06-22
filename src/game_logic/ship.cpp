#include "logic_models.hpp"
#include <boost/uuid.hpp>

Ship::Ship(ShipType type)
    : type(type), health(static_cast<unsigned int>(type)) {
  boost::uuids::random_generator gen;
  id = gen();
}

bool Ship::isSunk() const { return health <= 0; }

void Ship::hit() { health--; }

ShipType Ship::getType() const { return type; }

#include <logic_models.hpp>
#include <memory>

Field::Field() : state(FieldState::EMPTY), morred_ship(nullptr) {}

void Field::placeShip(std::shared_ptr<Ship> ship) { morred_ship = ship; }

void Field::setState(FieldState new_state) { state = new_state; }

FieldState Field::getState() const { return state; }

Ship *Field::getShip() const { return morred_ship.get(); }

#include "connection.hpp"
#include <boost/uuid.hpp>
#include <memory>
#include <string>

namespace battleship {
namespace networking {

/// @brief Class made for the server to store it's players
struct NetworkPlayer {
  boost::uuids::uuid id;
  std::string name;
  std::shared_ptr<Connection> connection;

  NetworkPlayer(std::string name, std::shared_ptr<Connection> connection)
      : name(name)
      , connection(connection) {}

  boost::uuids::uuid getId() const {
    return connection->getId();
  }
};
} // namespace networking
} // namespace battleship

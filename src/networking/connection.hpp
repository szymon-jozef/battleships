#pragma once
#include "data_types.hpp"
#include "messages.hpp"
#include <boost/asio.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <functional>
#include <memory>

namespace battleship {
namespace networking {

class Connection : public std::enable_shared_from_this<Connection> {
public:
  std::function<void(std::shared_ptr<Connection>)> onDisconnect;

  enum class Owner { SERVER, CLIENT };
  Connection(Owner owner, boost::asio::io_context &context, boost::asio::ip::tcp::socket socket, MessageQueue &qIn);
  boost::uuids::uuid getId() const {
    return id;
  };
  void setId(boost::uuids::uuid id);
  void connectToServer(const boost::asio::ip::tcp::resolver::results_type &endpoints);
  void disconnect();
  bool isConnected() const;
  void startListening();
  void send(const Message &msg);

private:
  boost::asio::ip::tcp::socket socket;
  boost::asio::io_context &context;
  MessageQueue &queIn, queOut;
  Message temporaryMessage;
  Owner owner;
  boost::uuids::uuid id;

  void writeHeader();
  void writeBody();
  void readHeader();
  void readBody();
  void addIncomingMessageQueue();
};
} // namespace networking
} // namespace battleship

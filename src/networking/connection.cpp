#include "connection.hpp"
#include <boost/asio/connect.hpp>
#include <boost/asio/error.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/read.hpp>
#include <boost/asio/write.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <spdlog/spdlog.h>

namespace battleship {
namespace networking {

Connection::Connection(Owner owner,
                       boost::asio::io_context &context,
                       boost::asio::ip::tcp::socket socket,
                       MessageQueue &qIn)
    : owner(owner)
    , socket(std::move(socket))
    , context(context)
    , queIn(qIn)
    , id(boost::uuids::random_generator()()) {}

void Connection::connectToServer(const boost::asio::ip::tcp::resolver::results_type &endpoints,
                                 std::function<void(bool, std::string)> onResult) {
  switch (owner) {
  case Owner::CLIENT:
    boost::asio::async_connect(
        socket,
        endpoints,
        [this, self = shared_from_this(), onResult](std::error_code ec, boost::asio::ip::tcp::endpoint endpoint) {
          if (!ec) {
            readHeader();

            if (onResult) {
              onResult(true, "Connected to the server");
            }
          } else {
            spdlog::error("[Network] Could not connect: {}", ec.message());
            if (onResult) {
              onResult(false, ec.message());
            }
          }
        });
    break;
  case Owner::SERVER:
    // only client can connect to server
    return;
  }
}

void Connection::disconnect() {
  if (isConnected()) {
    boost::asio::post(context, [this]() {
      if (onDisconnect) {
        onDisconnect(shared_from_this());
      } else {
        socket.close();
      }
    });
  }
}

bool Connection::isConnected() const {
  return socket.is_open();
}

void Connection::send(const Message &msg) {
  boost::asio::post(context, [this, msg]() {
    bool writingMessage = !queOut.empty();
    queOut.push_back({nullptr, msg});
    if (!writingMessage) {
      writeHeader();
    }
  });
}

void Connection::startListening() {
  readHeader();
}

void Connection::writeHeader() {
  boost::asio::async_write(socket,
                           boost::asio::buffer(&queOut.front().msg.header, sizeof(MessageHeader)),
                           [this, self = shared_from_this()](std::error_code ec, size_t length) {
                             if (ec) {
                               disconnect();
                               return;
                             }
                             if (queOut.front().msg.body.size() > 0) {
                               writeBody();
                             } else {
                               queOut.pop_front();
                               if (!queOut.empty()) {
                                 writeHeader();
                               }
                             }
                           });
}

void Connection::writeBody() {
  boost::asio::async_write(socket,
                           boost::asio::buffer(queOut.front().msg.body.msg.data(), queOut.front().msg.body.size()),
                           [this, self = shared_from_this()](std::error_code ec, size_t length) {
                             if (ec) {
                               spdlog::error("[Network] {} Write body fail", boost::uuids::to_string(id));
                               disconnect();
                               return;
                             }
                             queOut.pop_front();
                             if (!queOut.empty()) {
                               writeHeader();
                             }
                           });
}

void Connection::readHeader() {
  boost::asio::async_read(socket,
                          boost::asio::buffer(&temporaryMessage.header, sizeof(MessageHeader)),
                          [this, self = shared_from_this()](boost::system::error_code ec, size_t length) {
                            if (ec) {
                              if (ec == boost::asio::error::eof || ec == boost::asio::error::connection_reset) {
                                spdlog::info("[Network] {} disconnected", boost::uuids::to_string(id));
                              } else {
                                spdlog::error("[Network] {} Read header fail.", boost::uuids::to_string(id));
                              }
                              disconnect();
                              return;
                            }

                            if (temporaryMessage.header.size > 0) {
                              if (temporaryMessage.header.size > 100000) {
                                spdlog::error("[Network] {} Huge message size detected: {}, disconnecting.",
                                              boost::uuids::to_string(id),
                                              temporaryMessage.header.size);
                                disconnect();
                                return;
                              }
                              temporaryMessage.body.msg.resize(temporaryMessage.header.size);
                              readBody();
                            } else {
                              addIncomingMessageQueue();
                            }
                          });
}

void Connection::readBody() {
  boost::asio::async_read(socket,
                          boost::asio::buffer(temporaryMessage.body.msg.data(), temporaryMessage.body.size()),
                          [this, self = shared_from_this()](std::error_code ec, size_t length) {
                            if (ec) {
                              spdlog::error("[Network] {} Read body fail.", boost::uuids::to_string(id));
                              disconnect();
                              return;
                            }

                            addIncomingMessageQueue();
                          });
}

void Connection::addIncomingMessageQueue() {
  switch (owner) {
  case Owner::SERVER:
    queIn.push_back({shared_from_this(), temporaryMessage});
    break;
  case Owner::CLIENT:
    queIn.push_back({nullptr, temporaryMessage});
    break;
  }
  readHeader();
}

} // namespace networking
} // namespace battleship

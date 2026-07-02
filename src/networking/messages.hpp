#pragma once
#include <boost/uuid.hpp>
#include <cstdint>
#include <cstring>
#include <vector>

namespace battleship {
namespace networking {
enum class MessageType {
  CLIENT_HANDSHAKE,
  CLIENT_CONNECTION_STATUS,
  CLIENT_SEND_ATTACK,
  CLIENT_RECEIVE_ATTACK,

  SERVER_GAME_STATUS,
  SERVER_GAME_END
};

struct MessageHeader {
  MessageType id{};
  boost::uuids::uuid sender;
  boost::uuids::uuid receiver;
  uint32_t size = 0;

  MessageHeader() = default;
  MessageHeader(MessageType type, boost::uuids::uuid from, boost::uuids::uuid to);
};

/// @brief LIFO with messages info
struct MessageBody {
  std::vector<uint8_t> msg;

public:
  MessageBody() = default;
  size_t size() const {
    return msg.size();
  };

  /// @brief Add information to the message
  template <typename T> void push(const T &data) {
    static_assert(std::is_standard_layout<T>::value, "Data is too complex to be pushed");

    size_t current_size = msg.size();
    msg.resize(current_size + sizeof(T));
    std::memcpy(msg.data() + current_size, &data, sizeof(T));
  }
  /// @brief Read information from the message body and remove it
  template <typename T> T pop() {
    static_assert(std::is_standard_layout<T>::value, "Data is too complex to be popped");

    T data;
    size_t new_size = msg.size() - sizeof(T);
    std::memcpy(&data, msg.data() + new_size, sizeof(T));
    msg.resize(new_size);
    return data;
  }
};

struct Message {
  MessageHeader header;
  MessageBody body;

  Message() = default;
  Message(MessageHeader header, MessageBody body);

  template <typename T> void push(const T &data) {
    body.push(data);
    header.size = body.size();
  }

  template <typename T> T pop() {
    T data = body.template pop<T>();
    header.size = body.size();
    return data;
  }
};

} // namespace networking
} // namespace battleship

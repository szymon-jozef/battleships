#pragma once
#include "messages.hpp"
#include <boost/uuid.hpp>
#include <condition_variable>
#include <deque>
#include <mutex>

namespace battleship {
namespace networking {
class Connection;
class OwnedMessage;

class MessageQueue {
  std::deque<OwnedMessage> queue;
  mutable std::mutex mutex;
  std::condition_variable cvBlock;

public:
  void push_back(const OwnedMessage &item);
  void push_front(const OwnedMessage &item);
  OwnedMessage pop_back();
  OwnedMessage pop_front();
  const OwnedMessage &back();
  const OwnedMessage &front();
  size_t size() const;
  bool empty() const;
  void wait();

  MessageQueue() = default;
};

struct OwnedMessage {
  std::shared_ptr<Connection> remote;
  Message msg;
};

struct PlayerNameMessage {
  char name[32]{};
};
} // namespace networking
} // namespace battleship

#pragma once
#include "messages.hpp"
#include <deque>
#include <mutex>

namespace battleship {
namespace networking {
class MessageQueue {
  std::deque<Message> queue;
  mutable std::mutex mutex;

public:
  void push_back(const Message &item);
  void push_front(const Message &item);
  Message pop_back();
  Message pop_front();
  Message back();
  Message front();
  size_t size() const;
  bool empty() const;

  MessageQueue() = default;
};
} // namespace networking
} // namespace battleship

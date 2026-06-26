#include "data_types.hpp"

namespace battleship {
namespace networking {
void MessageQueue::push_back(const Message &item) {
  std::scoped_lock lock(mutex);
  queue.push_back(item);
}
void MessageQueue::push_front(const Message &item) {
  std::scoped_lock lock(mutex);
  queue.push_front(item);
}

bool MessageQueue::empty() const {
  std::scoped_lock lock(mutex);
  return queue.empty();
}

Message MessageQueue::pop_back() {
  std::scoped_lock lock(mutex);
  Message msg = std::move(queue.back());
  queue.pop_back();
  return msg;
}

const Message &MessageQueue::back() {
  std::scoped_lock lock(mutex);
  return queue.back();
}

Message MessageQueue::pop_front() {
  std::scoped_lock lock(mutex);
  Message msg = std::move(queue.front());
  queue.pop_front();
  return msg;
}

const Message &MessageQueue::front() {
  std::scoped_lock lock(mutex);
  return queue.front();
}

size_t MessageQueue::size() const {
  std::scoped_lock lock(mutex);
  return queue.size();
}
} // namespace networking
} // namespace battleship

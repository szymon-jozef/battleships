#include "data_types.hpp"

namespace battleship {
namespace networking {
void MessageQueue::push_back(const OwnedMessage &item) {
  std::scoped_lock lock(mutex);
  queue.push_back(item);
  cvBlock.notify_one();
}
void MessageQueue::push_front(const OwnedMessage &item) {
  std::scoped_lock lock(mutex);
  queue.push_front(item);
  cvBlock.notify_one();
}

bool MessageQueue::empty() const {
  std::scoped_lock lock(mutex);
  return queue.empty();
}

OwnedMessage MessageQueue::pop_back() {
  std::scoped_lock lock(mutex);
  OwnedMessage msg = std::move(queue.back());
  queue.pop_back();
  return msg;
}

const OwnedMessage &MessageQueue::back() {
  std::scoped_lock lock(mutex);
  return queue.back();
}

OwnedMessage MessageQueue::pop_front() {
  std::scoped_lock lock(mutex);
  OwnedMessage msg = std::move(queue.front());
  queue.pop_front();
  return msg;
}

const OwnedMessage &MessageQueue::front() {
  std::scoped_lock lock(mutex);
  return queue.front();
}

size_t MessageQueue::size() const {
  std::scoped_lock lock(mutex);
  return queue.size();
}

void MessageQueue::wait() {
  std::unique_lock<std::mutex> lock(mutex);
  cvBlock.wait(lock, [this]() { return !queue.empty(); });
}
} // namespace networking
} // namespace battleship

#pragma once

#include "core2/Utility.h"

#include <atomic>
#include <stdexcept>

namespace core2 {

template <typename T> class AtomicStack {
private:
  struct StackNode {
    T value;
    StackNode* next;

    StackNode(const T& value_) : value(value_), next(nullptr) {}
    StackNode(T&& value_) : value(core2::move(value_)), next(nullptr) {}
  };

  struct StackTop {
    StackNode* topNode;
    size_t popCount;

    StackTop() : topNode(nullptr), popCount(0ull) {}
  };

  std::atomic<StackTop> top;

public:
  AtomicStack() = default;

  void push(const T& value) {
    StackNode* newNode = new StackNode(value);

    StackTop oldTop, newTop;

    do {
      oldTop = top.load();
      newTop = oldTop;

      newNode->next = oldTop.topNode;
      newTop.topNode = newNode;
    } while (!top.compare_exchange_strong(oldTop, newTop,
                                          std::memory_order_release));
  }

  T pop() {
    StackTop oldTop, newTop;
    do {
      oldTop = top.load();

      if (oldTop.topNode == nullptr) {
        throw std::runtime_error("Empty queue");
      }

      newTop.topNode = oldTop.topNode->next;
      newTop.popCount = oldTop.popCount + 1;
    } while (!top.compare_exchange_strong(oldTop, newTop,
                                          std::memory_order_relaxed));

    T value = oldTop.topNode->value;
    delete oldTop.topNode;

    return value;
  }
};

} // namespace core2
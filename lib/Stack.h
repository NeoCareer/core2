#pragma once

#include "lib/All.h"

namespace core {

template <typename T, typename Allocator = std::allocator<T>> class Stack {
private:
  struct Node {
    T value;
    Node *next = nullptr;

    template <typename T_>
    Node(T_&& value_) : value(std::forward<T_>(value_)) {}
  };

  using AllocatorTraits = std::allocator_traits<Allocator>;

  using NodeAllocator = typename AllocatorTraits::template rebind_alloc<Node>;
  NodeAllocator nodeAllocator;
  using NodeAllocatorTraits = std::allocator_traits<NodeAllocator>;

  std::atomic<Node *> head;

public:
  Stack() : head(nullptr) {}

  template <typename T_> void push(T_ &&value) {
    Node *node = nodeAllocator.allocate(1);
    NodeAllocatorTraits::construct(nodeAllocator, node, std::forward<T_>(value));

    Node *currentHead;

    do {
      currentHead = head;
      node->next = currentHead;
    } while (!head.compare_exchange_weak(currentHead, node));
  }

  T&& pop() {
    for (;;) {
      Node *currentHead = head;
      if (currentHead == nullptr) {
        continue;
      }

      Node *newHead = currentHead->next;

      if (head.compare_exchange_weak(currentHead, newHead)) {
        T value = std::move(currentHead->value);

        NodeAllocatorTraits::destroy(nodeAllocator, currentHead);
        nodeAllocator.deallocate(currentHead, 1);

        return std::move(value);
      }
    }
  }
};

} // namespace core

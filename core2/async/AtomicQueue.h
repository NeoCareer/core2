#pragma once

#include <atomic>
#include <memory>
#include <stdexcept>

#include "core2/Utility.h"

// namespace core2 {

// template <typename T> class AtomicQueue {
// private:
//   struct QueueNode {
//     T value;
//     std::atomic<QueueNode*> next;

//     QueueNode() = default;
//     QueueNode(const T& value_) : value(value_), next(nullptr) {}
//     QueueNode(T&& value_) : value(core2::move(value_)), next(nullptr) {}
//   };

//   struct QueueHead {
//     QueueNode* head;
//     std::size_t popCount;

//     QueueHead(QueueNode* head_) : head(head_), popCount(0) {}
//     QueueHead() : popCount(0) {}
//   };

// public:
//   // std::atomic<QueueNode*> head, tail;
//   std::atomic<QueueHead> queueHead;
//   std::atomic<QueueNode*> queueTail;

//   AtomicQueue() {
//     QueueNode* dummyNode = new QueueNode();

//     // head.store(dummyNode);
//     // tail.store(dummyNode);
//     queueHead.store(QueueHead(dummyNode));
//     queueTail.store(dummyNode);
//   }

//   void push(const T& value) {
//     // Push after the tail
//     QueueNode* newNode = new QueueNode(value);
//     QueueNode* tailNode;

//     while (true) {
//       tailNode = queueTail.load();

//       QueueNode* nullNode = nullptr;

//       if (tailNode->next.compare_exchange_weak(nullNode, newNode)) {
//         break;
//       }
//     }

//     queueTail.compare_exchange_weak(tailNode, newNode);
//   }

//   T pop() {
//     QueueHead oldHead;
//     QueueHead newHead;

//     while (true) {
//       // headNode = head.load();
//       oldHead = queueHead.load();

//       if (oldHead.head->next.load() == nullptr) {
//         throw std::runtime_error("Queue empty");
//       }

//       newHead.head = oldHead.head->next.load();
//       newHead.popCount = oldHead.popCount + 1;

//       if (queueHead.compare_exchange_weak(oldHead, newHead)) {
//         break;
//       }
//     }

//     // T value = headNode->next.load()->value;
//     T value = newHead.head->value;
//     delete oldHead.head;
//     return value;
//   }
// };

// } // namespace core2

namespace core2 {

template <typename T> class AtomicQueue {
private:
  struct QueueNode {
    T value;
    std::atomic<std::shared_ptr<QueueNode>> next;

    QueueNode() : next(nullptr) {}
    QueueNode(const T& value_) : value(value_), next(nullptr) {}
    QueueNode(T&& value_) : value(core2::move(value_)), next(nullptr) {}
  };

public:
  // std::atomic<QueueNode*> head, tail;
  std::atomic<std::shared_ptr<QueueNode>> queueHead;
  std::atomic<std::shared_ptr<QueueNode>> queueTail;

  AtomicQueue() {
    std::shared_ptr<QueueNode> dummyNode = std::make_shared<QueueNode>();

    // head.store(dummyNode);
    // tail.store(dummyNode);
    queueHead.store(dummyNode);
    queueTail.store(dummyNode);
  }

  void push(const T& value) {
    // Push after the tail
    std::shared_ptr<QueueNode> newNode = std::make_shared<QueueNode>(value);

    std::shared_ptr<QueueNode> tailNode;

    while (true) {
      tailNode = queueTail.load();

      std::shared_ptr<QueueNode> nullNode;

      if (tailNode->next.compare_exchange_weak(nullNode, newNode)) {
        break;
      }
    }

    assert(queueTail.compare_exchange_weak(tailNode, newNode));
  }

  T pop() {
    std::shared_ptr<QueueNode> oldHead, newHead;
    // QueueHead newHead;

    while (true) {
      oldHead = queueHead.load();
      newHead = oldHead->next.load();

      if (newHead == nullptr) {
        throw std::runtime_error("Queue empty");
      }

      if (queueHead.compare_exchange_weak(oldHead, newHead)) {
        break;
      }
    }

    // T value = headNode->next.load()->value;
    T value = newHead->value;
    return value;
  }
};

} // namespace core2
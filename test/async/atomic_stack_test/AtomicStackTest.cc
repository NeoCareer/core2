#include "core2/async/AtomicStack.h"
#include "core2/Bit.h"

#include <stdexcept>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace {

constexpr const size_t SIZE = 1ull << 12ull;
constexpr const size_t PUSHER_COUNT = 1ull << 6;
constexpr const size_t POPER_COUNT = 1ull << 3;

TEST(AtomicStackTest, AtomicStackTestCorrectness) {
  core2::AtomicStack<core2::u64> atomicStack;

  std::atomic_uint64_t globalCounter;

  const auto pusher = [&]() {
    for (core2::u64 i = 0; i < SIZE; ++i) {
      atomicStack.push(i);
    }

    globalCounter.fetch_add(SIZE / 2 * (SIZE - 1));
  };

  const auto poper = [&]() {
    core2::u64 failureLimit = 16;

    core2::u64 popedSum = 0;

    while (true) {
      try {
        core2::u64 value = atomicStack.pop();
        popedSum += value;
      } catch (...) {
        if (failureLimit-- == 0) {
          break;
        }
      }
    }

    globalCounter.fetch_sub(popedSum);
  };

  std::vector<std::thread> threads;

  for (size_t i = 0; i < PUSHER_COUNT; ++i) {
    threads.emplace_back(pusher);
  }

  for (size_t i = 0; i < POPER_COUNT; ++i) {
    threads.emplace_back(poper);
  }

  for (auto& thread : threads) {
    thread.join();
  }

  poper();

  ASSERT_EQ(globalCounter.load(), 0);
}

} // namespace

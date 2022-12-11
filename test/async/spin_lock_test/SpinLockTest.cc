#include "core2/Core2.h"
#include <gtest/gtest.h>

namespace {

constexpr const size_t SIZE = 1 << 6;
constexpr const size_t THREAD_COUNT = 1 << 6;

TEST(SpinLockTest, SimpleSpinLockCorrectness) {
  core2::SpinLock lock;

  size_t count = 0;

  const auto incrementation = [&]() {
    for (size_t i = 0; i < SIZE; ++i) {
      lock.lock();
      count++;
      lock.unlock();
    }
  };

  std::vector<std::thread> threads;

  for (size_t i = 0; i < THREAD_COUNT; ++i) {
    threads.emplace_back(incrementation);
  }

  for (auto& thread : threads) {
    thread.join();
  }

  ASSERT_EQ(SIZE * THREAD_COUNT, count);
}

} // namespace

#include "core2/Core2.h"
#include <gtest/gtest.h>

namespace {

constexpr const size_t SIZE = 1 << 6;

TEST(VectorTest, SimpleFifo) {
  core2::Fifo<SIZE> fifo;

  constexpr const size_t ITERATION = SIZE << 4;

  const auto enqueueThread = [&]() {
    size_t inputIndex = 0;
    core2::byte_t inBuffer[128];

    while (inputIndex < ITERATION) {
      size_t i;
      for (i = 0; i + inputIndex < ITERATION && i < 128; ++i) {
        inBuffer[i] =
            static_cast<core2::byte_t>((i + inputIndex) * 117ull % 91ull);
      }

      inputIndex += fifo.enqueue(inBuffer, i);
    }
  };

  const auto dequeueThread = [&]() {
    size_t outputIndex = 0;
    core2::byte_t outBuffer[128];

    while (outputIndex < ITERATION) {
      size_t size = fifo.dequeue(outBuffer, 128);

      for (size_t i = 0; i < size; ++i) {
        ASSERT_EQ(
            static_cast<core2::byte_t>((outputIndex + i) * 117ull % 91ull),
            outBuffer[i]);
      }

      outputIndex += size;
    }

    ASSERT_EQ(ITERATION, outputIndex);
  };

  std::thread enqueue(enqueueThread);
  std::thread dequeue(dequeueThread);

  enqueue.join();
  dequeue.join();
}

} // namespace
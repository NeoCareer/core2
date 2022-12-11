#pragma once

#include "core2/Bit.h"
#include "core2/STL.h"

namespace core2 {

template <size_t size = 32> class Fifo {
  static_assert(!(size & (size - 1)));

private:
  byte_t buffer[size];
  size_t begin = 0;
  size_t end = 0;

public:
  size_t enqueue(byte_t* data, size_t length) {
    // length is guaranteed to be in range [0, size]
    length = std::min(length, size + begin - end);

    std::atomic_thread_fence(std::memory_order_seq_cst);

    size_t endLength = std::min(length, size - (end & (size - 1)));

    memcpy(buffer + (end & (size - 1)), data, endLength);
    memcpy(buffer, data + endLength, length - endLength);

    std::atomic_thread_fence(std::memory_order_seq_cst);

    end += length;
    return length;
  }

  size_t dequeue(byte_t* data, size_t length) {
    length = std::min(length, end - begin);

    std::atomic_thread_fence(std::memory_order_seq_cst);

    size_t beginLength = std::min(length, size - (begin & (size - 1)));
    memcpy(data, buffer + (begin & (size - 1)), beginLength);
    memcpy(data + beginLength, buffer, length - beginLength);

    std::atomic_thread_fence(std::memory_order_seq_cst);

    begin += length;

    return length;
  }
};

} // namespace core2

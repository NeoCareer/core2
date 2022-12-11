#pragma once

#include "core2/Bit.h"
#include "core2/simd/AVX2.h"
#include <assert.h>

namespace core2::BitAllocator {

struct BitMap {
  static_assert(CACHE_LINE_SIZE % sizeof(qword_t) == 0);
  u64 data[CACHE_LINE_SIZE / sizeof(qword_t)];

  static constexpr u64 BIT_PER_QWORD = BIT_PER_BYTE * sizeof(qword_t);
  static constexpr u64 BIT_PER_CACHE_LINE = CACHE_LINE_SIZE * BIT_PER_BYTE;
  static constexpr u64 NPOS = 512ULL;
  static constexpr u64 QWORD_PER_M256I = sizeof(AVX2::u256) / sizeof(qword_t);
  static constexpr u64 QWORD_PER_CACHE_LINE = CACHE_LINE_SIZE / sizeof(qword_t);

  inline void set(u64 offset) {
    // assert(offset < BIT_PER_CACHE_LINE);
    data[offset / BIT_PER_QWORD] |= (0x1ull << (offset % BIT_PER_QWORD));
  }

  inline void reset(u64 offset) {
    // assert(offset < BIT_PER_CACHE_LINE);
    data[offset / BIT_PER_QWORD] &= ~(0x1ull << (offset % BIT_PER_QWORD));
  }

  inline bool get(u64 offset) {
    // assert(offset < BIT_PER_CACHE_LINE);

    return (data[offset / BIT_PER_QWORD] >> (offset % BIT_PER_QWORD)) & 0x1;
  }

  inline bool empty() noexcept {
    AVX2::u512 avx2Array = reinterpret_cast<AVX2::u512>(data);
    return AVX2::u256IsZero(avx2Array[0]) && AVX2::u256IsZero(avx2Array[1]);
  }

  inline bool full() noexcept {
    AVX2::u512 avx2Array = reinterpret_cast<AVX2::u512>(data);
    return AVX2::u256IsFull(avx2Array[0]) && AVX2::u256IsFull(avx2Array[1]);
  }

  inline u64 getAvailableBlock() noexcept {
#pragma GCC unroll QWORD_PER_CACHE_LINE
    for (unsigned index = 0; index < QWORD_PER_CACHE_LINE; ++index) {
      if (data[index] != ~0ULL) {
        u64 offset = BIT_PER_QWORD * index + std::countr_one(data[index]);
        return offset;
      }
    }

    return NPOS;
  }

  inline u64 getAvailableBlockAndSet() noexcept {
#pragma GCC unroll QWORD_PER_CACHE_LINE
    for (unsigned index = 0; index < QWORD_PER_CACHE_LINE; ++index) {
      if (data[index] != ~0ULL) {
        u64 trailingOne = std::countr_one(data[index]);

        data[index] |= (0x1ull << trailingOne);

        return BIT_PER_QWORD * index + trailingOne;
      }
    }

    return NPOS;
  }

  inline void clear() noexcept {
    AVX2::u512 avx2Array = reinterpret_cast<AVX2::u512>(data);
    _mm256_store_si256(&avx2Array[0], U256_ZERO);
    _mm256_store_si256(&avx2Array[1], U256_ZERO);
  }
};

} // namespace core2::BitAllocator

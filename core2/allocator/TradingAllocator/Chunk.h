#pragma once

#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>

#include "core2/Bit.h"
#include "core2/allocator/TradingAllocator/AllocatorUtility.h"
#include "core2/allocator/TradingAllocator/BitMap.h"

namespace core2::TradingAllocator {

inline size_t chunkSizeToType(int size) {
  assert(size <= 96);

  constexpr size_t SIMD_ALIGN = 0x7ull;

  bool exceed = static_cast<bool>(size & SIMD_ALIGN);
  size >>= 3;

  /**
   * 16 -> 10
   * 24 -> 11
   * 32 -> 100
   * 48 -> 110
   * 64 -> 1000
   * 96 -> 1100
   **/
}

/**
 * Small chunk sizes are: 16, 24, 32, 48, 64, 96, 128 bytes
 *
 * Layout:
 **/
struct ChunkHeader {
  pthread_t owner;
  qword_t type;

  ChunkHeader* prev;
  ChunkHeader* next;

  BitMap topLevelMap;

  size_t allocatedSize;

  size_t blockSize;

  size_t bitMapIndex;
  size_t bitMapRange;

  // Bitmap is dynamic
  // For 16 size chunks, the size is 254
  // And the maximum size is 254
  BitMap bitMap[];

  explicit ChunkHeader(qword_t type_) : prev(this), next(this), bitMapIndex(0) {
    owner = gettid();
    type = type_;

    // 0 - 5 corresponds to 16, 24, 32, 48, 64, 96
    if (type < 6) {
      size_t base = 16ull << (type >> 1);
      blockSize = base + (base >> 1) * (type & 0x1);

      bitMapRange = 508ull * 4096ull / blockSize / BitMap::BIT_PER_CACHE_LINE;

    } else {
      allocatorAbort("Type not supported");
    }

    topLevelMap.clear();

    for (size_t mapIndex = bitMapRange; mapIndex < BitMap::BIT_PER_CACHE_LINE;
         ++mapIndex) {
      topLevelMap.set(mapIndex);
    }

    for (size_t mapIndex = 0; mapIndex < bitMapRange; ++mapIndex) {
      bitMap[mapIndex].clear();
    }
  }

  void resetBitMap() {
    assert((reinterpret_cast<u64>(this) & CHUNK_MASK) == 0);

    for (size_t mapIndex = 0; mapIndex < bitMapRange; ++mapIndex) {
      topLevelMap.reset(mapIndex);
    }

    for (size_t mapIndex = 0; mapIndex < bitMapRange; ++mapIndex) {
      bitMap[mapIndex].clear();
    }
  }
};

static_assert(offsetof(ChunkHeader, bitMap) == 128);

} // namespace core2::TradingAllocator
#pragma once

#include <assert.h>
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>

#include "core2/Bit.h"
#include "core2/allocator/bit_allocator/BitMap.h"
#include "core2/allocator/bit_allocator/Utility.h"

namespace core2::BitAllocator {

inline u64 chunkSizeToType(int size) {
  assert(size <= 96);

  constexpr u64 SIMD_ALIGN = 0x7ull;

  bool exceed = static_cast<bool>(size & SIMD_ALIGN);
  size >>= 3;

  (void)exceed;
  (void)size;

  /**
   * 16 -> 10
   * 24 -> 11
   * 32 -> 100
   * 48 -> 110
   * 64 -> 1000
   * 96 -> 1100
   **/
  return 0;
}

struct ChunkHeaderDelegate;

/**
 * Small chunk sizes are: 16, 24, 32, 48, 64, 96, 128 bytes
 *
 * Layout:
 **/
struct ChunkHeader {
  ChunkHeader* prev;
  ChunkHeader* next;

  pthread_t owner;
  qword_t type;

  BitMap topLevelMap;

  u64 allocatedSize;

  u64 blockSize;

  u64 bitMapIndex;
  u64 bitMapRange;

  // Bitmap is dynamic
  // For 16 size chunks, the size is 254
  // And the maximum size is 254
  BitMap bitMap[];

  explicit ChunkHeader(qword_t type_) : prev(this), next(this), bitMapIndex(0) {
    owner = gettid();
    type = type_;

    // 0 - 5 corresponds to 16, 24, 32, 48, 64, 96
    if (type < 6) {
      u64 base = 16ull << (type >> 1);
      blockSize = base + (base >> 1) * (type & 0x1);

      bitMapRange = 508ull * 4096ull / blockSize / BitMap::BIT_PER_CACHE_LINE;

    } else {
      allocatorAbort("Type not supported");
    }

    for (u64 mapIndex = bitMapRange; mapIndex < BitMap::BIT_PER_CACHE_LINE;
         ++mapIndex) {
      topLevelMap.set(mapIndex);
    }
  }

  void resetBitMap() {
    assert((reinterpret_cast<u64>(this) & CHUNK_MASK) == 0);

    for (u64 mapIndex = 0; mapIndex < bitMapRange; ++mapIndex) {
      topLevelMap.reset(mapIndex);
    }

    for (u64 mapIndex = 0; mapIndex < bitMapRange; ++mapIndex) {
      bitMap[mapIndex].clear();
    }
  }

  inline void* allocate(size_t size) {
    if (allocatedSize == bitMapRange * BitMap::BIT_PER_CACHE_LINE)
        [[unlikely]] {
      return nullptr;
    }

    if (topLevelMap.get(bitMapIndex)) [[unlikely]] {
      bitMapIndex = topLevelMap.getAvailableBlock();
    }

    assert(bitMapIndex != BitMap::NPOS);

    size_t offset = bitMap[bitMapIndex].getAvailableBlockAndSet();

    if (bitMap[bitMapIndex].full()) [[unlikely]] {
      topLevelMap.set(bitMapIndex);
    }

    allocatedSize++;

    return reinterpret_cast<void*>(reinterpret_cast<u64>(this) + PAGE_SIZE * 4 +
                                   16 * (bitMapIndex * 512 + offset));
  }
};

static_assert(offsetof(ChunkHeader, bitMap) == 128);

struct ChunkHeaderDelegate {
  ChunkHeaderDelegate* prev;
  ChunkHeaderDelegate* next;

  ChunkHeaderDelegate() : prev(this), next(this) {}
};

} // namespace core2::BitAllocator

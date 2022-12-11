#pragma once

#include <assert.h>
#include <bit>
#include <cstdint>
#include <immintrin.h>
#include <stdalign.h>
#include <stddef.h>
#include <unistd.h>

#include "core2/Bit.h"
#include "core2/Trait.h"
#include "core2/allocator/bit_allocator/Arena.h"
#include "core2/allocator/bit_allocator/Chunk.h"
#include "core2/allocator/bit_allocator/Utility.h"
#include "core2/simd/AVX2.h"

namespace core2::BitAllocator {

// static thread_local ChunkHeader* localChunk[6];
static thread_local Arena threadArena;

// Currently only allocate 16 byte chunks
inline void* allocate(size_t size) noexcept {
  return threadArena.allocate(size);
}

inline void deallocateAll() noexcept { threadArena.deallocateAll(); }

inline void deallocate(void* addr) noexcept {
  if ((reinterpret_cast<u64>(addr) & CHUNK_MASK) == 0) {
    deallocateAlignedChunk(addr, CHUNK_SIZE);
    return;
  }

  assert((reinterpret_cast<u64>(addr) & 0xfull) == 0);

  ChunkHeader* header = reinterpret_cast<ChunkHeader*>(
      reinterpret_cast<u64>(addr) & (~CHUNK_MASK));

  size_t offset =
      ((reinterpret_cast<u64>(addr) - reinterpret_cast<u64>(header)) -
       PAGE_SIZE * 4) /
      16;

  header->bitMapIndex = offset / BitMap::BIT_PER_CACHE_LINE;

  assert(header->bitMap[header->bitMapIndex].get(offset %
                                                 BitMap::BIT_PER_CACHE_LINE));

  header->topLevelMap.reset(header->bitMapIndex);

  header->bitMap[header->bitMapIndex].reset(offset %
                                            BitMap::BIT_PER_CACHE_LINE);

  header->allocatedSize--;
}

} // namespace core2::BitAllocator

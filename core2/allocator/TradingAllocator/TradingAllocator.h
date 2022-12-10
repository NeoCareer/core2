#pragma once

#include <assert.h>
#include <bit>
#include <cstdint>
#include <immintrin.h>
#include <stdalign.h>
#include <stddef.h>
#include <unistd.h>

#include "core2/Bit.h"
#include "core2/allocator/TradingAllocator/AllocatorUtility.h"
#include "core2/allocator/TradingAllocator/Chunk.h"
#include "core2/simd/AVX2.h"

namespace core2::TradingAllocator {

static thread_local ChunkHeader* localChunk[6];

/**
 * Allocated huge page is guaranteed to align with 2MB
 *
 * This function is inspired by jemalloc's aligned chunk allocation function
 **/
inline void* allocateAlignedChunk(size_t size) {
  // size must be chunk-aligned
  assert((size & CHUNK_MASK) == 0);

  // Check for wrap around
  if (CHUNK_SIZE + size < size) {
    return nullptr;
  }

  void* addr = mmap(nullptr, CHUNK_SIZE + size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if (addr == nullptr) {
    return nullptr;
  }

  size_t offset = chunkOffset(addr);

  if (offset != 0) {
    munmap(addr, CHUNK_SIZE - offset);

    addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) +
                                   CHUNK_SIZE - offset);

    munmap(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) + size),
           offset);
  } else {
    munmap(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr) + size),
           CHUNK_SIZE);
  }

  return addr;
}

inline void deallocateAlignedChunk(void* addr, size_t size) {
  // addr & size must be chunk-aligned
  assert((reinterpret_cast<uintptr_t>(addr) & CHUNK_MASK) == 0);
  assert((size & CHUNK_MASK) == 0);

  munmap(addr, size);
}

// Currently only allocate 16 byte chunks
inline void* allocate(size_t size) noexcept {
  assert(size < 128);

  if (localChunk[0] == nullptr) {
    localChunk[0] =
        reinterpret_cast<ChunkHeader*>(allocateAlignedChunk(CHUNK_SIZE));
    ::new (localChunk[0]) ChunkHeader(0);
  }

  assert(localChunk[0] != nullptr);
  assert((reinterpret_cast<uintptr_t>(localChunk[0]) & CHUNK_MASK) == 0);

  ChunkHeader* header = reinterpret_cast<ChunkHeader*>(localChunk[0]);

  if (header->topLevelMap.get(header->bitMapIndex)) {
    header->bitMapIndex = header->topLevelMap.getAvailableBlock();
  }

  if (header->bitMapIndex == BitMap::NPOS) {
    return nullptr;
  }

  size_t offset = header->bitMap[header->bitMapIndex].getAvailableBlock();

  assert(offset != BitMap::NPOS);

  header->bitMap[header->bitMapIndex].set(offset);

  if (header->bitMap[header->bitMapIndex].full()) {
    header->topLevelMap.set(header->bitMapIndex);
  }

  header->allocatedSize++;

  return reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(header) +
                                 PAGE_SIZE * 4 +
                                 16 * (header->bitMapIndex * 512 + offset));
}

inline void deallocateAll() noexcept {
  for (size_t chunkIndex = 0; chunkIndex < 6; ++chunkIndex) {
    ChunkHeader* header = localChunk[chunkIndex];
    if (header == nullptr) {
      continue;
    }

    for (ChunkHeader *ptr = header->next, *nextPtr = nullptr; ptr != header;
         ptr = nextPtr) {
      nextPtr = ptr->next;
      if (nextPtr == nullptr) {
      }
      deallocateAlignedChunk(ptr, CHUNK_SIZE);
    }

    deallocateAlignedChunk(header, CHUNK_SIZE);

    localChunk[chunkIndex] = nullptr;
  }
}

inline void deallocate(void* addr) noexcept {
  assert((reinterpret_cast<uintptr_t>(addr) & 0xfull) == 0);

  ChunkHeader* header = reinterpret_cast<ChunkHeader*>(
      reinterpret_cast<uintptr_t>(addr) & (~CHUNK_MASK));

  size_t offset = ((reinterpret_cast<uintptr_t>(addr) -
                    reinterpret_cast<uintptr_t>(header)) -
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

} // namespace core2::TradingAllocator

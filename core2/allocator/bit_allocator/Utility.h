#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/mman.h>

#include "core2/Bit.h"

namespace core2::BitAllocator {

// Chunk consts
constexpr size_t CHUNK_SIZE = 1ULL << 21;
constexpr size_t CHUNK_MASK = CHUNK_SIZE - 1;

// Get the offset within the chunk
inline size_t chunkOffset(void* addr) {
  return static_cast<size_t>(reinterpret_cast<size_t>(addr) & CHUNK_MASK);
}

inline void allocatorAbort(const char* msg = "") {
  fprintf(stderr, "TradingAllocator aborted\n");
  fprintf(stderr, "MSG: %s\n", msg);
  abort();
}

// static bool previouslyAllocationAligned = false;

inline void* allocateAlignedChunkSlow(size_t size) {
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

    addr = reinterpret_cast<void*>(reinterpret_cast<u64>(addr) + CHUNK_SIZE -
                                   offset);

    munmap(reinterpret_cast<void*>(reinterpret_cast<u64>(addr) + size), offset);
  } else {
    munmap(reinterpret_cast<void*>(reinterpret_cast<u64>(addr) + size),
           CHUNK_SIZE);
  }

  return addr;
}

/**
 * Allocated huge page is guaranteed to align with 2MB
 *
 * This function is inspired by jemalloc's aligned chunk allocation function
 **/
inline void* allocateAlignedChunk(size_t size) {
  // size must be chunk-aligned
  assert((size & CHUNK_MASK) == 0);

  void* addr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  if ((reinterpret_cast<u64>(addr) & CHUNK_MASK) == 0) {
    return addr;
  } else {
    munmap(addr, size);
    return allocateAlignedChunkSlow(size);
  }

  // // Check for wrap around
  // if (CHUNK_SIZE + size < size) {
  //   return nullptr;
  // }

  // void* addr = mmap(nullptr, CHUNK_SIZE + size, PROT_READ | PROT_WRITE,
  //                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

  // if (addr == nullptr) {
  //   return nullptr;
  // }

  // size_t offset = chunkOffset(addr);

  // if (offset != 0) {
  //   munmap(addr, CHUNK_SIZE - offset);

  //   addr = reinterpret_cast<void*>(reinterpret_cast<u64>(addr) +
  //                                  CHUNK_SIZE - offset);

  //   munmap(reinterpret_cast<void*>(reinterpret_cast<u64>(addr) + size),
  //          offset);
  // } else {
  //   munmap(reinterpret_cast<void*>(reinterpret_cast<u64>(addr) + size),
  //          CHUNK_SIZE);
  // }

  // return addr;
}

inline void deallocateAlignedChunk(void* addr, size_t size) {
  // addr & size must be chunk-aligned
  assert((reinterpret_cast<u64>(addr) & CHUNK_MASK) == 0);
  assert((size & CHUNK_MASK) == 0);

  munmap(addr, size);
}

} // namespace core2::BitAllocator

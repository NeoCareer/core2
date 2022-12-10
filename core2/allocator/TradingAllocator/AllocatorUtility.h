#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

namespace core2::TradingAllocator {

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

} // namespace core2::TradingAllocator
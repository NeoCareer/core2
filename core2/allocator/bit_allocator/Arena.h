#pragma once

#include "core2/Bit.h"
#include "core2/Log.h"
#include "core2/allocator/bit_allocator/AlignedAllocator.h"
#include "core2/allocator/bit_allocator/Chunk.h"
#include "core2/allocator/bit_allocator/Utility.h"

namespace core2::BitAllocator {

static PageServer pageServer;

struct Arena {
  ChunkHeaderDelegate smallChunk[6];

  explicit Arena() = default;

  inline void deallocateAll() noexcept {
    for (u64 index = 0; index < 6; ++index) {
      for (ChunkHeaderDelegate* chunk = smallChunk[index].next;
           chunk != &smallChunk[index];) {
        ChunkHeaderDelegate* currentChunk = chunk;
        chunk = chunk->next;
        deallocateAlignedChunk(currentChunk, CHUNK_SIZE);
      }

      smallChunk[index].next = &smallChunk[index];
      smallChunk[index].prev = &smallChunk[index];
    }
  }

  void* allocate(size_t size) noexcept {
    if (size == CHUNK_SIZE) {
      return allocateAlignedChunk(CHUNK_SIZE);
    }

    for (ChunkHeaderDelegate* chunk = smallChunk[0].next;
         chunk != &smallChunk[0]; chunk = chunk->next) {
      if (void* addr = reinterpret_cast<ChunkHeader*>(chunk)->allocate(size);
          addr != nullptr) {
        return addr;
      }
    }

    // Arriving here implies new block is required
    ChunkHeader* allocatedChunk =
        reinterpret_cast<ChunkHeader*>(allocateAlignedChunk(CHUNK_SIZE));

    construct<ChunkHeader>(allocatedChunk, 0);

    ChunkHeader* header = reinterpret_cast<ChunkHeader*>(&smallChunk[0]);

    allocatedChunk->next = header->next;
    header->next->prev = allocatedChunk;
    allocatedChunk->prev = header;
    header->next = allocatedChunk;

    return allocatedChunk->allocate(size);
  }
};

} // namespace core2::BitAllocator

#pragma once

#include "absl/strings/str_format.h"
#include "core2/Utility.h"
#include <assert.h>
#include <cstdint>
#include <stddef.h>
#include <unistd.h>

namespace core2::SimpleAllocator {

inline qword_t qwordToByteOffset(qword_t offset) {
  return offset * sizeof(qword_t) / sizeof(byte_t);
}

inline qword_t byteToQWordOffset(qword_t offset) {
  return offset * sizeof(byte_t) / sizeof(qword_t);
}

/**
 * Design:
 *
 * Allocated block:
 * 1. Minimum size 32: 8 byte header, 8 byte footer, 16 byte payload
 *
 * Free block:
 * 1. Minimum size 32: 8 byte header, 8 byte footer, 8 byte prev pointer
 *    8 byte next pointer
 *
 **/

struct GlobalState {
  qwordptr_t base;
  qwordptr_t freeListRoot;
  qwordptr_t top;

  explicit GlobalState(qwordptr_t base_, qwordptr_t top_)
      : base(base_), freeListRoot(base_ + 1), top(top_) {}
};

// One page
constexpr qword_t BASE_SIZE = (1 << 12) - sizeof(qword_t);
constexpr qword_t ALIGNMENT = 0x10;
constexpr qword_t MIN_BLOCK_SIZE = sizeof(qword_t) * 2 + ALIGNMENT;

class FreeBlockReference {
private:
  struct FreeBlockHeader {
    qword_t size;
    qwordptr_t prev;
    qwordptr_t next;
  };

  struct FreeBlockSize {
    qwordptr_t addr;

    FreeBlockSize(qwordptr_t addr_) : addr(addr_) {}

    FreeBlockSize& operator=(qword_t size) {
      assert((size & 0xfull) == 0);

      *addr = size;
      *(addr + byteToQWordOffset(size) - 1) = size;

      return *this;
    }

    operator qword_t() const { return *addr; }
  };

  qwordptr_t addr;

  FreeBlockHeader& header() {
    return *reinterpret_cast<FreeBlockHeader*>(addr);
  }

public:
  FreeBlockReference(qwordptr_t addr_) : addr(addr_) {}

  operator qwordptr_t() { return addr; }

  qwordptr_t& prev() { return header().prev; }

  qwordptr_t& next() { return header().next; }

  FreeBlockReference prevReference() {
    return FreeBlockReference(header().prev);
  }

  FreeBlockReference nextReference() {
    return FreeBlockReference(header().next);
  }

  FreeBlockSize size() { return FreeBlockSize(addr); }

  FreeBlockReference& operator++() {
    addr = header().next;
    return *this;
  }

  FreeBlockReference& operator--() {
    addr = header().prev;
    return *this;
  }
};

class AllocatedBlockReference {
private:
  qwordptr_t addr;

  struct AllocatedBlockSize {
    qwordptr_t addr;
    AllocatedBlockSize(qwordptr_t addr_) : addr(addr_) {}

    AllocatedBlockSize& operator=(qword_t size) {
      assert((size & 0xfull) == 0);

      *addr = (size | 1);
      *(addr + byteToQWordOffset(size) - 1) = size | 1;

      return *this;
    }

    operator qword_t() const {
      assert(*addr & 0x1);
      return (*addr) & (~0x1ull);
    }

    // tailAddr is the address of the next block
    static AllocatedBlockSize fromTail(qwordptr_t tailAddr) {
      qword_t size = *tailAddr & ~0x1ull;

      qwordptr_t headAddr = tailAddr - byteToQWordOffset(size);

      return AllocatedBlockSize(headAddr);
    }
  };

public:
  AllocatedBlockReference(qwordptr_t addr_) : addr(addr_) {}

  AllocatedBlockSize size() { return AllocatedBlockSize(addr); }

  operator qwordptr_t() const { return addr; }

  static AllocatedBlockReference fromTail(qwordptr_t tailAddr) {
    qword_t blockSize = AllocatedBlockSize::fromTail(tailAddr);

    return AllocatedBlockReference(tailAddr - byteToQWordOffset(blockSize));
  }
};

inline GlobalState initHeap() {
  qwordptr_t base = static_cast<qwordptr_t>(sbrk(BASE_SIZE));
  qwordptr_t freeListRoot = base + 1;

  FreeBlockReference rootReference(freeListRoot);
  rootReference.size() = sizeof(qword_t) * 2 + sizeof(qwordptr_t) * 2;
  rootReference.prev() = freeListRoot + 4;
  rootReference.next() = freeListRoot + 4;

  FreeBlockReference primaryReference(freeListRoot + 4);
  primaryReference.size() = BASE_SIZE - sizeof(qword_t) - MIN_BLOCK_SIZE;
  primaryReference.prev() = freeListRoot;
  primaryReference.next() = freeListRoot;

  GlobalState state(base, static_cast<qwordptr_t>(sbrk(0)));

  return state;
}

inline GlobalState& global() {
  static GlobalState state = initHeap();
  return state;
}

inline void logHeap() {
  qwordptr_t base = global().base;
  qwordptr_t freeListRoot = global().freeListRoot;
  FreeBlockReference rootBlock(freeListRoot);
  qwordptr_t top = global().top;

  absl::PrintF("========\n");
  absl::PrintF("Base addr: %p\n", base);
  absl::PrintF("FreeListRoot addr: %p -> %p, %p\n", freeListRoot,
               static_cast<qwordptr_t>(rootBlock.prev()),
               static_cast<qwordptr_t>(rootBlock.next()));
  absl::PrintF("Top addr: %p\n", top);
  absl::PrintF("--------\n");

  // FreeBlockIterator it = FreeBlockIterator(base).next();
  for (FreeBlockReference ref = FreeBlockReference(freeListRoot).next();
       ref != freeListRoot; ++ref) {
    absl::PrintF("Free block: %p, %u\n", static_cast<qwordptr_t>(ref),
                 ref.size());
    absl::PrintF("            %p, %p\n", static_cast<qwordptr_t>(ref.prev()),
                 static_cast<qwordptr_t>(ref.next()));
  }
}

void* allocate(qword_t size) {
  qwordptr_t base = global().base;
  qwordptr_t freeListRoot = global().freeListRoot;

  qword_t allocationSize = (size & ~0xfull) +
                           ((size & 0xfull) == 0 ? 0 : ALIGNMENT) +
                           sizeof(qword_t) * 2;

  for (FreeBlockReference ref = FreeBlockReference(freeListRoot).next();
       ref != freeListRoot; ++ref) {
    if (ref.size() >= allocationSize) {
      if (ref.size() < allocationSize + MIN_BLOCK_SIZE) {
        // Assign entire free block to user
        ref.prevReference().next() = ref.next();
        ref.nextReference().prev() = ref.prev();

        qword_t blockSize = ref.size();

        AllocatedBlockReference allocatedBlock(ref);
        allocatedBlock.size() = blockSize;

        return static_cast<void*>(ref + 1);
      } else {
        // Give user a section
        qword_t splittedSize = ref.size() - allocationSize;
        qwordptr_t prev = ref.prev();
        qwordptr_t next = ref.next();

        FreeBlockReference splittedBlockReference(
            ref + (allocationSize / sizeof(qwordptr_t)));
        splittedBlockReference.prev() = prev;
        splittedBlockReference.next() = next;
        splittedBlockReference.size() = splittedSize;

        ref.prevReference().next() = splittedBlockReference;
        ref.nextReference().prev() = splittedBlockReference;

        AllocatedBlockReference allocatedBlock(ref);
        allocatedBlock.size() = allocationSize;

        return static_cast<void*>(allocatedBlock + 1);
      }
    }
  }

  return nullptr;
}

template <typename T, typename... Args> T* construct(Args&&... args) {
  T* p = static_cast<T*>(allocate(sizeof(T)));
  assert(p != nullptr);

  ::new (p) T(std::forward<Args>(args)...);

  return p;
}

void free(void* addr) {
  qwordptr_t freeListRoot = global().freeListRoot;

  FreeBlockReference rootBlock(freeListRoot);

  AllocatedBlockReference allocatedBlock(static_cast<qwordptr_t>(addr) - 1);

  qword_t size = allocatedBlock.size();

  FreeBlockReference freedBlock(allocatedBlock);

  freedBlock.size() = size;
  freedBlock.prev() = rootBlock;
  freedBlock.next() = rootBlock.next();

  rootBlock.nextReference().prev() = freedBlock;
  rootBlock.next() = freedBlock;
}

template <typename T> void destroy(T* obj) {
  obj->~T();
  free(static_cast<void*>(obj));
}

} // namespace core2::SimpleAllocator

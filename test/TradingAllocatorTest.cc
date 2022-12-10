#include "core2/Core2.h"
#include <gtest/gtest.h>
#include <unordered_set>

namespace {

constexpr size_t CHUNK_SIZE = 1ull << 21;
constexpr size_t CHUNK_MASK = CHUNK_SIZE - 1;

TEST(TradingAllocatorTest, allocateAlignedChunk4MB) {
  void* addr = core2::TradingAllocator::allocateAlignedChunk(1ull << 22);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(addr) & CHUNK_MASK, 0);
}

TEST(TradingAllocatorTest, allocateAlignedChunk32MB) {
  void* addr = core2::TradingAllocator::allocateAlignedChunk(1ull << 25);
  EXPECT_EQ(reinterpret_cast<uintptr_t>(addr) & CHUNK_MASK, 0);
}

TEST(TradingAllocatorTest, allocate16Byte) {
  uintptr_t prevAddr = 0;

  for (unsigned i = 0; i < 254 * 512; ++i) {
    uintptr_t addr =
        reinterpret_cast<uintptr_t>(core2::TradingAllocator::allocate(16));
    EXPECT_EQ(prevAddr == 0 ? 16 : addr - prevAddr, (uintptr_t)16);
    EXPECT_NE(addr, 0);
    prevAddr = addr;
  }

  EXPECT_EQ(core2::TradingAllocator::allocate(16), nullptr);
}

TEST(TradingAllocatorTest, deallocate16Byte) {
  core2::TradingAllocator::deallocateAll();

  std::unordered_set<void*> addrs;

  void* addr;

  while ((addr = core2::TradingAllocator::allocate(16)) != nullptr) {
    addrs.insert(addr);
  }

  EXPECT_EQ(addrs.size(), 254 * 512);

  for (void* p : addrs) {
    core2::TradingAllocator::deallocate(p);
  }

  addrs.clear();

  while ((addr = core2::TradingAllocator::allocate(16)) != nullptr) {
    addrs.insert(addr);
  }

  EXPECT_EQ(addrs.size(), 254 * 512);
}

} // namespace
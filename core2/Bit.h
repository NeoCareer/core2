#pragma once

namespace core2 {

using byte_t = unsigned char;
using qword_t = unsigned long long;
using dword_t = unsigned int;

static_assert(sizeof(byte_t) == 1);
static_assert(sizeof(qword_t) == 8);
static_assert(sizeof(dword_t) == 4);

using byteptr_t = byte_t*;
using qwordptr_t = qword_t*;
using dwordptr_t = dword_t*;

static_assert(sizeof(byteptr_t) == 8);
static_assert(sizeof(qwordptr_t) == 8);
static_assert(sizeof(dwordptr_t) == 8);

using u64 = qword_t;
using u32 = dword_t;
using u8 = byte_t;

constexpr u64 CACHE_LINE_SIZE = 64;
constexpr u64 BIT_PER_BYTE = 8;

// Page consts
constexpr u64 PAGE_SIZE = 1ULL << 12;
constexpr u64 PAGE_MASK = PAGE_SIZE - 1;

template <typename T> constexpr u64 bitSize() noexcept {
  return sizeof(T) * BIT_PER_BYTE;
}

} // namespace core2

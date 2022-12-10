#pragma once

#include <immintrin.h>

namespace core2 {

namespace AVX2 {

using u256 = __m256i;
using u512 = u256[2];

#define U256_FULL_MASK _mm256_set1_epi64x(-1ll)
#define U256_ZERO _mm256_setzero_si256()

inline bool u256IsZero(__m256i value) noexcept {
  return _mm256_testz_si256(value, U256_FULL_MASK);
}

inline __m256i u256Flip(__m256i value) noexcept {
  return _mm256_xor_si256(value, U256_FULL_MASK);
}

inline bool u256IsFull(__m256i value) noexcept {
  return _mm256_testz_si256(AVX2::u256Flip(value), U256_FULL_MASK);
}

} // namespace AVX2

inline u64 countLZero(AVX2::u256 value) noexcept {
  AVX2::u256 compareResult = _mm256_cmpeq_epi8(value, U256_ZERO);

  u32 mask = _mm256_movemask_epi8(compareResult);

  if (mask == 0) {
    return bitSize<AVX2::u256>();
  }

  u64 leadingZero = std::countl_one(mask);
  u8* array = reinterpret_cast<u8*>(&value);
  return leadingZero * BIT_PER_BYTE +
         std::countl_zero(array[bitSize<u32>() - 1 - leadingZero]);
}

inline u64 countLZero(AVX2::u512 value) noexcept {
  AVX2::u256 lower = _mm256_cmpeq_epi8(value[0], U256_ZERO);
  AVX2::u256 upper = _mm256_cmpeq_epi8(value[1], U256_ZERO);

  u64 mask = (static_cast<u64>(_mm256_movemask_epi8(upper)) << bitSize<u32>()) |
             (static_cast<u64>(_mm256_movemask_epi8(lower)));

  if (mask == UINT64_MAX) {
    return bitSize<AVX2::u512>();
  }

  u64 leadingMatch = std::countl_one(mask);
  u8* array = reinterpret_cast<u8*>(value);
  return leadingMatch * BIT_PER_BYTE +
         std::countl_zero(array[bitSize<u64>() - 1 - leadingMatch]);
}

inline u64 countLOne(AVX2::u512 value) noexcept {
  AVX2::u256 lower = _mm256_cmpeq_epi8(value[0], U256_FULL_MASK);
  AVX2::u256 upper = _mm256_cmpeq_epi8(value[1], U256_FULL_MASK);

  unsigned lowerMask = _mm256_movemask_epi8(lower);
  unsigned upperMask = _mm256_movemask_epi8(upper);

  u64 mask = (static_cast<u64>(upperMask) << bitSize<u32>()) |
             static_cast<u64>(lowerMask);

  if (mask == UINT64_MAX) {
    return bitSize<AVX2::u512>();
  }

  u64 leadingMatch = std::countl_one(mask);
  u8* array = reinterpret_cast<u8*>(value);
  return leadingMatch * BIT_PER_BYTE +
         std::countl_one(array[bitSize<u64>() - 1 - leadingMatch]);
}

inline u64 countROne(AVX2::u512 value) noexcept {
  AVX2::u256 lower = _mm256_cmpeq_epi8(value[0], U256_FULL_MASK);
  AVX2::u256 upper = _mm256_cmpeq_epi8(value[1], U256_FULL_MASK);

  unsigned lowerMask = _mm256_movemask_epi8(lower);
  unsigned upperMask = _mm256_movemask_epi8(upper);

  u64 mask = (static_cast<u64>(upperMask) << bitSize<u32>()) |
             static_cast<u64>(lowerMask);

  if (mask == UINT64_MAX) {
    return bitSize<AVX2::u512>();
  }

  u64 trailingMatch = std::countr_one(mask);
  u8* array = reinterpret_cast<u8*>(value);
  return trailingMatch * BIT_PER_BYTE + std::countr_one(array[trailingMatch]);
}

} // namespace core2

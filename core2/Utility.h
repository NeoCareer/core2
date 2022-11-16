#pragma once

#include "core2/STL.h"

namespace core2 {

using namespace std::chrono;

long long millsecondSinceEpoch() {
  auto millsecond =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  return millsecond;
}

float millsecondDuration() {
  static long long previousTime = 0;

  float millsecondDuration =
      (static_cast<float>(millsecondSinceEpoch() - previousTime)) / 1000;
  previousTime = millsecondSinceEpoch();
  return millsecondDuration;
}

template <typename T>
inline typename std::remove_reference_t<T>&& move(T&& value) {
  using ReturnType = typename std::remove_reference_t<T>&&;
  return static_cast<ReturnType>(value);
}

template <typename T> inline T&& forward(std::remove_reference_t<T>& value) {
  return static_cast<T&&>(value);
}

template <typename IntegralType = size_t>
constexpr inline IntegralType nextPowerOf2(IntegralType integral) {
  if (integral & (integral - 1) == 0) {
    return integral;
  } else {
    return 0x1 << (sizeof(IntegralType) - std::__countl_zero(integral));
  }
}

using byte_t = unsigned char;

struct DefaultConstructTag {};

} // namespace core2
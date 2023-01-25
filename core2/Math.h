#pragma once

#include "core2/Bit.h"

namespace core2 {

template <typename IntegralType = u64>
constexpr inline IntegralType nextPowerOf2(IntegralType integral) {
  if ((integral & (integral - 1)) == 0) {
    return integral;
  } else {
    return 0x1 << (sizeof(IntegralType) - std::__countl_zero(integral));
  }
}

} // namespace core2

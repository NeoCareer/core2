#pragma once

#include "core2/Utility.h"
#include <type_traits>

namespace core2 {

template <typename T, unsigned IDX,
          bool IsEmptyBase = std::is_empty_v<T> && !std::is_final_v<T>>
class CompressedPairElement {
private:
  T value;

public:
  CompressedPairElement() = default;

  CompressedPairElement(DefaultConstructTag) : value() {}

  template <typename T_>
  CompressedPairElement(T_&& value_) : value(core2::forward<T_>(value_)) {}

  inline T& get() noexcept { return value; }
  inline const T& get() const noexcept { return value; }
};

template <typename T, unsigned IDX>
class CompressedPairElement<T, IDX, true> : private T {
public:
  CompressedPairElement() = default;

  template <typename T_>
  CompressedPairElement(T_&& value_) : T(core2::forward<T_>(value_)) {}

  CompressedPairElement(DefaultConstructTag) : T() {}

  inline T& get() noexcept { return *this; }
  inline const T& get() const noexcept { return *this; }
};

template <typename T, typename U>
class CompressedPair : private CompressedPairElement<T, 0>,
                       private CompressedPairElement<U, 1> {
private:
  using BaseT = CompressedPairElement<T, 0>;
  using BaseU = CompressedPairElement<U, 1>;

public:
  CompressedPair() = default;

  template <typename T_, typename U_>
  CompressedPair(T_&& first_, U_&& second_)
      : BaseT(core2::forward<T_>(first_)), BaseU(core2::forward<U_>(second_)) {}

  inline T& first() noexcept { return static_cast<BaseT&>(*this).get(); }

  inline const T& first() const noexcept {
    return static_cast<BaseT const&>(*this).get();
  }

  inline U& second() noexcept { return static_cast<BaseU&>(*this).get(); }

  inline const U& second() const noexcept {
    return static_cast<BaseU const&>(*this).get();
  }
};

} // namespace core2
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

template <size_t BS = 24> union TypeErasedOptimizedBuffer {
public:
  template <typename T, typename... Args> void construct(Args&&... args) {
    if constexpr (sizeof(T) <= BS) {
      // Construct on buffer
      ::new (static_cast<void*>(buffer)) T(core2::forward<Args>(args)...);
    } else {
      // Construct on heap
      obj = static_cast<void*>(new T(core2::forward<Args>(args)...));
    }
  }

  template <typename T> void destroy() noexcept {
    if constexpr (sizeof(T) <= BS) {
      ((T*)(&buffer[0]))->~T();
    } else {
      delete static_cast<T*>(obj);
    }
  }

  // Obtain the pointer to object
  template <typename T> T& data() noexcept {
    if constexpr (sizeof(T) <= BS) {
      return *((T*)(&buffer[0]));
    } else {
      return *static_cast<T*>(obj);
    }
  }

  // template <typename T> void swap(TypeErasedOptimizedBuffer& other) {
  //   if constexpr (sizeof(T) <= BS) {
  //     std::swap(*static_cast<T*>(buffer), *static_cast<T*>(other.buffer));
  //   } else {
  //     // Just swap the pointer
  //     std::swap(obj, other.ojb);
  //   }
  // }

private:
  alignas(8) unsigned char buffer[BS];
  alignas(8) void* obj = nullptr;
};

} // namespace core2
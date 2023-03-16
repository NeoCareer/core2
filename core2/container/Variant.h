#pragma once

#include "core2/Bit.h"
#include "core2/Log.h"
#include "core2/Utility.h"
#include <algorithm>
#include <inttypes.h>
#include <stdexcept>
#include <tuple>
#include <utility>

namespace core2 {

namespace detail {

template <typename T, typename... Ts> struct IndexOfType;

template <typename T, typename... Ts> struct IndexOfType<T, T, Ts...> {
  static constexpr size_t index = 0;
};

template <typename T> struct IndexOfType<T> {
  static constexpr size_t index = 0;
};

template <typename T, typename U, typename... Ts>
struct IndexOfType<T, U, Ts...> {
  static constexpr size_t index = 1 + IndexOfType<T, Ts...>::index;
};

template <typename T, typename... Ts>
constexpr size_t IndexOfTypeV = IndexOfType<T, Ts...>::index;

template <u64 idx, typename... Ts>
using TypeAtIndexT = std::remove_cvref_t<decltype(std::get<idx>(
    std::declval<std::tuple<Ts...>>()))>;

} // namespace detail

template <typename... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};

template <typename... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename... Ts> class Variant {
private:
  alignas(Ts...) byte_t data_[std::max({sizeof(Ts)...})];
  size_t index_;

  template <size_t idx> void destroyAt(size_t index) {
    if (idx == index) {
      using DataType = detail::TypeAtIndexT<idx, Ts...>;
      reinterpret_cast<DataType*>(data_)->~DataType();
    }
  }

  template <size_t... Is>
  void destroy(size_t index, std::index_sequence<Is...>) {
    (destroyAt<Is>(index), ...);
  }

  template <size_t idx, typename Fn> void visitAt(Fn&& fn, size_t index) {
    if (idx == index) {
      using DataType = detail::TypeAtIndexT<idx, Ts...>;

      fn(*reinterpret_cast<DataType*>(data_));
    }
  }

  template <typename Fn, size_t... Is>
  void visitIndices(Fn&& fn, size_t index, std::index_sequence<Is...>) {
    (visitAt<Is>(fn, index), ...);
  }

  // Assuming object is empty
  void copyConstructFromVariant(Variant& other) {
    index_ = other.index_;

    other.visit([this](auto& value) {
      using ElemType = std::remove_cvref_t<decltype(value)>;

      construct<ElemType>(value);
    });
  }

  void moveConstructFromVariant(Variant&& other) {
    index_ = other.index_;

    other.visit([this](auto& value) {
      using ElemType = std::remove_cvref_t<decltype(value)>;

      construct<ElemType>(core2::move(value));
    });

    other.reset();
  }

public:
  Variant() : index_(sizeof...(Ts)) {}

  Variant(Variant& other) { copyConstructFromVariant(other); }

  Variant(Variant&& other) { moveConstructFromVariant(core2::move(other)); }

  template <typename T> Variant(T&& initialValue) {
    construct<T>(core2::forward<T>(initialValue));
  }

  Variant& operator=(Variant& other) {
    reset();

    copyConstructFromVariant(other);

    return *this;
  }

  Variant& operator=(Variant&& other) {
    reset();
    moveConstructFromVariant(core2::move(other));
    return *this;
  }

  void reset() {
    if (index_ != sizeof...(Ts)) {
      // It was previously constructed, so destroy it first
      destroy(index_, std::make_integer_sequence<size_t, sizeof...(Ts)>{});

      index_ = sizeof...(Ts);
    }
  }

  // Assuming object is in default state
  template <typename T, typename... Args> void construct(Args&&... args) {
    constexpr size_t constexprIdx = detail::IndexOfTypeV<T, Ts...>;

    ::new (reinterpret_cast<T*>(data_)) T(core2::forward<Args>(args)...);

    index_ = constexprIdx;
  }

  template <typename T, typename... Args> void emplace(Args&&... args) {
    reset();
    construct<T>(core2::forward<Args>(args)...);
  }

  size_t index() const { return index_; }

  template <typename Fn> void visitSingle(Fn&& fn) {
    if (index_ == sizeof...(Ts)) {
      return;
    }

    visitIndices(core2::forward<Fn>(fn), index_,
                 std::make_integer_sequence<size_t, sizeof...(Ts)>{});
  }

  template <typename... Fn> void visit(Fn&&... fn) {
    visitSingle(overloaded{core2::forward<Fn>(fn)...});
  }

  ~Variant() { reset(); }

  void swap(Variant& other) {
    Variant temp(core2::move(other));
    other = core2::move(*this);
    *this = core2::move(temp);
  }
};

} // namespace core2

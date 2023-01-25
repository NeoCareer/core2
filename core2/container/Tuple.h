#pragma once

#include "core2/Bit.h"
#include "core2/TypeTrait.h"
#include "core2/Utility.h"
#include <type_traits>

namespace core2 {

namespace detail {

template <u64 IDX, typename T, bool = __is_empty(T) && !__is_final(T)>
struct TupleElem {
  T value;

  T& get() & noexcept { return value; }

  T&& get() && { return move(value); }
};

template <u64 IDX, typename T> struct TupleElem<IDX, T, true> : public T {
  T& get() & noexcept { return *this; }

  T&& get() && { return move(*this); }
};

template <u64, typename... T> struct TupleImpl;

template <u64 IDX, typename T> struct TupleImpl<IDX, T> : TupleElem<IDX, T> {};

template <u64 IDX, typename T, typename... U>
struct TupleImpl<IDX, T, U...> : public TupleImpl<IDX + 1, U...>,
                                 public TupleElem<IDX, T> {};

} // namespace detail

template <typename... T> struct Tuple : detail::TupleImpl<0, T...> {};

namespace detail {

template <u64 IDX, typename Tu> struct TupleElemType;

template <u64 IDX, typename... TuElem>
struct TupleElemType<IDX, Tuple<TuElem...>> {
  using type = NthTypeT<IDX, TuElem...>;
};

template <u64 IDX, typename Tu>
using TupleElemTypeT = typename TupleElemType<IDX, Tu>::type;

} // namespace detail

template <u64 IDX, typename... TuElem>
typename detail::TupleElemTypeT<IDX, Tuple<TuElem...>>&
tupleElem(Tuple<TuElem...>& tuple) {
  using ElemType = detail::TupleElemTypeT<IDX, Tuple<TuElem...>>;
  detail::TupleElem<IDX, ElemType>& elem =
      static_cast<detail::TupleElem<IDX, ElemType>&>(tuple);
  return elem.get();
}

} // namespace core2

#pragma once

#include "core2/Bit.h"
#include "core2/Utility.h"

namespace core2 {

namespace detail {

template <typename T> struct RemoveReference {
  using type = T;
};

template <typename T> struct RemoveReference<T&> {
  using type = T;
};

template <typename T> struct RemoveReference<T&&> {
  using type = T;
};

} // namespace detail

template <typename T>
using RemoveReferenceT = typename detail::RemoveReference<T>::type;

namespace detail {

template <u64 IDX, typename T, typename... U> struct NthTypeHelper {
  using type = typename NthTypeHelper<IDX - 1, U...>::type;
};

template <typename T, typename... U> struct NthTypeHelper<0, T, U...> {
  using type = T;
};

template <u64 IDX, typename... T> struct NthType {
  static_assert(IDX < sizeof...(T), "Index must be in the range");

  using type = typename NthTypeHelper<IDX, T...>::type;
};

} // namespace detail

template <u64 IDX, typename... T>
using NthTypeT = typename detail::NthType<IDX, T...>::type;

namespace detail {

/**
 * Takes a templated argument, and only provide the wrapper
 *
 * Example:
 *   std::tuple<T, U> => std::tuple<>;
 */
template <typename> struct ExtractWrapper;

template <template <typename...> typename Wrapper, typename... Args>
struct ExtractWrapper<Wrapper<Args...>> {
  using type = Wrapper<>;
};

/**
 * A holder of template arguments
 */
template <typename... Args> struct TypeList {};

/**
 * Extract out template arguments in TemplatedType and put it into TypeList
 *
 * It is used because some wrapper types only allow certain template arguments
 * format, which could be violated in recursive deduction
 *
 * Examples:
 *   std::pair<T, U> => TypeList<T, U>
 */
template <typename TemplatedType> struct ExtractArgsList;

template <template <typename...> typename Wrapper, typename... Args>
struct ExtractArgsList<Wrapper<Args...>> {
  using type = TypeList<Args...>;
};

/**
 * Swap tmeplate arguments of two templated type
 *
 * Example:
 *   std::tuple<T, U>, std::pair<G, H> => std::tuple<G, H>, std::pair<T, U>
 */
template <typename LeftTemplate, typename RightTemplate>
struct SwapTemplateArgsList;

template <template <typename...> typename LeftWrapper,
          template <typename...> typename RightWrapper, typename... LeftArgs,
          typename... RightArgs>
struct SwapTemplateArgsList<LeftWrapper<LeftArgs...>,
                            RightWrapper<RightArgs...>> {
  using leftType = LeftWrapper<RightArgs...>;
  using rightType = RightWrapper<LeftArgs...>;
};

/**
 * Reverse template arguments, and it is implemented using recursive deduction
 */
template <typename LeftTemplateList,
          typename RightTemplateList =
              typename ExtractWrapper<LeftTemplateList>::type>
struct ReverseTemplateList;

template <template <typename...> typename Wrapper, typename LeftHead,
          typename... LeftTail, typename... RightTail>
struct ReverseTemplateList<Wrapper<LeftHead, LeftTail...>,
                           Wrapper<RightTail...>> {
  using type =
      typename ReverseTemplateList<Wrapper<LeftTail...>,
                                   Wrapper<LeftHead, RightTail...>>::type;
};

// The base case where the left side is an empty wrapper
template <template <typename...> typename Wrapper, typename... RightTail>
struct ReverseTemplateList<Wrapper<>, Wrapper<RightTail...>> {
  using type = Wrapper<RightTail...>;
};

} // namespace detail

template <typename TemplatedType>
using ExtractArgsListT = typename detail::ExtractArgsList<TemplatedType>::type;

template <typename LeftTemplatedType, typename RightTemplatedType>
using SwapTemplateArgsListLeftT =
    typename detail::SwapTemplateArgsList<LeftTemplatedType,
                                          RightTemplatedType>::leftType;

template <typename LeftTemplatedType, typename RightTemplatedType>
using SwapTemplateArgsListRightT =
    typename detail::SwapTemplateArgsList<LeftTemplatedType,
                                          RightTemplatedType>::rightType;

template <typename TemplatedType>
using ReverseTemplateListT =
    SwapTemplateArgsListLeftT<TemplatedType,
                              typename detail::ReverseTemplateList<
                                  ExtractArgsListT<TemplatedType>>::type>;

} // namespace core2

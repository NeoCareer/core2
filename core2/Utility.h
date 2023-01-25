#pragma once

#include "core2/STL.h"

namespace core2 {

template <typename T>
inline typename std::remove_reference_t<T>&& move(T&& value) {
  using ReturnType = typename std::remove_reference_t<T>&&;
  return static_cast<ReturnType>(value);
}

template <typename T> inline T&& forward(std::remove_reference_t<T>& value) {
  return static_cast<T&&>(value);
}

struct DefaultConstructTag {};

template <typename T, typename... Args>
inline T* construct(void* object, Args&&... args) {
  ::new (object) T(core2::forward<Args>(args)...);
  return reinterpret_cast<T*>(object);
}

template <typename T> inline void destroy(T* object) { object->~T(); }

} // namespace core2

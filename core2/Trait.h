#pragma once

#include "core2/Utility.h"

namespace core2 {

template <typename T, typename... Args>
inline T* construct(void* object, Args&&... args) {
  ::new (object) T(core2::forward<Args>(args)...);
  return reinterpret_cast<T*>(object);
}

template <typename T> inline void destroy(T* object) { object->~T(); }

} // namespace core2

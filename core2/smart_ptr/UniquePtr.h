#pragma once

#include <iostream>

#include "core2/trait/Memory.h"

namespace core2 {

template <typename T> struct DefaultDeleter {
public:
  DefaultDeleter() = default;
  DefaultDeleter(const DefaultDeleter&) noexcept {}

  inline void operator()(T* p) const noexcept { delete p; }
};

template <typename T, typename Deleter = DefaultDeleter<T>> class UniquePtr {
private:
  CompressedPair<T*, Deleter> dataPair;

  inline void reset(T* newPtr = nullptr) noexcept {
    T* oldPtr = dataPair.first();
    dataPair.first() = newPtr;
    if (oldPtr) {
      dataPair.second()(oldPtr);
    }
  }

  inline T* release() noexcept {
    T* oldPtr = dataPair.first();
    dataPair.first() = nullptr;
    return oldPtr;
  }

public:
  UniquePtr(nullptr_t) : dataPair(nullptr, DefaultConstructTag{}) {}

  UniquePtr(T* p_) : dataPair(p_, DefaultConstructTag()) {}

  UniquePtr(const UniquePtr& other) = delete;

  UniquePtr(UniquePtr&& other)
      : dataPair(other.release(),
                 core2::forward<Deleter>(other.dataPair.second())) {}

  inline UniquePtr& operator=(const UniquePtr& other) = delete;
  inline UniquePtr& operator=(UniquePtr&& other) {
    reset(other.release());
    dataPair.second() = core2::forward<Deleter>(other.deleter);
  }

  inline T& operator*() noexcept { return *dataPair.first(); }

  inline T* operator->() noexcept { return dataPair.first(); }

  ~UniquePtr() { reset(); }
};

} // namespace core2

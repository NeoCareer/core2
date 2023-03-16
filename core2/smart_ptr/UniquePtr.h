#pragma once

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

public:
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

  // bool() noexcept { return dataPair.first() == nullptr; }

  Deleter& getDeleter() noexcept { return dataPair.second(); }
  const Deleter& getDeleter() const noexcept { return dataPair.second(); }

  T* get() noexcept { return dataPair.first(); }

  void swap(UniquePtr& other) noexcept {
    std::swap(dataPair.first(), other.dataPair.first());
    std::swap(dataPair.second(), other.dataPair.second());
  }

public:
  UniquePtr() : dataPair(nullptr, DefaultConstructTag{}) {}
  UniquePtr(nullptr_t) : dataPair(nullptr, DefaultConstructTag{}) {}

  UniquePtr(T* p_) : dataPair(p_, DefaultConstructTag{}) {}

  UniquePtr(T* p_, const Deleter& other) : dataPair(p_, other) {}

  UniquePtr(T* p_, Deleter&& other) : dataPair(p_, core2::move(other)) {}

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

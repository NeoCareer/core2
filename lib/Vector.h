#pragma once

#include "lib/All.h"

namespace core {

template <typename T, typename Allocator = std::allocator<T>> class Vector {
private:
  static constexpr const size_t MIN_CAPACITY = 32;
  Allocator allocator;

  using AllocatorTrait = std::allocator_traits<Allocator>;

  T *data;
  size_t capacity = MIN_CAPACITY;
  size_t size = 0;

  void tryExpand() {
    if (size == capacity) {
      capacity *= 2;

      T *newMemory = allocator.allocate(capacity);
      for (size_t i = 0; i < size; ++i) {
        if constexpr (std::is_copy_assignable_v<T>) {
          newMemory[i] = data[i];
        } else {
          newMemory[i] = core::move(data[i]);
        }
      }

      for (size_t i = 0; i < size; ++i) {
        AllocatorTrait::destroy(allocator, &data[i]);
      }

      allocator.deallocate(data, size);
      data = newMemory;
    }
  }

  void tryShrink() {
    if (size * 4 == capacity && capacity > MIN_CAPACITY) {
      T *newMemory = allocator.allocate(capacity / 2);

      for (size_t i = 0; i < size; ++i) {
        if constexpr (std::is_copy_assignable_v<T>) {
          newMemory[i] = data[i];
        } else {
          newMemory[i] = core::move(data[i]);
        }
      }

      for (size_t i = 0; i < size; ++i) {
        AllocatorTrait::destroy(allocator, &data[i]);
      }

      allocator.deallocate(data, capacity);

      capacity /= 2;
      data = newMemory;
    }
  }

public:
  explicit Vector() { data = allocator.allocate(capacity); }

  void pushBack(const T &value) {
    tryExpand();

    data[size++] = value;
  }

  void pushBack(T &&value) {
    tryExpand();

    data[size++] = std::move(value);
  }

  template <typename... Args> void emplaceBack(Args &&...args) {
    tryExpand();

    AllocatorTrait::construct(allocator, &data[size++],
                              std::forward<Args>(args)...);
  }

  T &operator[](size_t index) {
    assert(index < size);
    return data[index];
  }

  bool isEmpty() const { return size == 0; }

  size_t getSize() const { return size; }

  void popBack() {
    assert(size > 0);

    tryShrink();
    size--;
    AllocatorTrait::destroy(allocator, &data[size]);
  }

  ~Vector() {
    for (size_t i = 0; i < size; ++i) {
      AllocatorTrait::destroy(allocator, &data[i]);
    }

    allocator.deallocate(data, capacity);
  }
};

} // namespace core

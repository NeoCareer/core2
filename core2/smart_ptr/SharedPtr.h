#pragma once

#include "core2/Bit.h"
#include "core2/Utility.h"
#include <atomic>

namespace core2 {

struct CtrlBlockBase {
  // All are initialized to 0
  std::atomic<u64> sharedCount;
  std::atomic<u64> weakCount;

  virtual void onZeroShared() = 0;
  virtual void onZeroWeak() = 0;

  void addShared() { sharedCount.fetch_add(1, std::memory_order_release); }

  void addWeak() { weakCount.fetch_add(1, std::memory_order_release); }

  // Return true if it is time to release resource
  void releaseShared() {
    if (sharedCount.fetch_sub(1) == 0) {
      onZeroShared();
      releaseWeak();
    }
  }

  void releaseWeak() {
    if (weakCount.fetch_sub(1) == 0) {
      onZeroWeak();
    }
  }
};

namespace detail {

template <typename T> struct DefaultDeleter {
  void operator()(T* obj) { delete obj; }
};

} // namespace detail

template <typename T, typename Deleter = detail::DefaultDeleter<T>,
          typename Allocator = std::allocator<T>>
struct CtrlBlockImpl : CtrlBlockBase {
  T* obj;
  Deleter deleter;
  Allocator allocator;

  CtrlBlockImpl(T* obj_) : obj(obj_) {}

  CtrlBlockImpl(T* obj_, Deleter deleter_) : obj(obj_), deleter(deleter_) {}

  CtrlBlockImpl(T* obj_, Deleter deleter_, Allocator allocator_)
      : obj(obj_), deleter(deleter_), allocator(allocator_) {}

  void onZeroShared() override {
    deleter(obj);
    deleter.~Deleter();
  }

  void onZeroWeak() override {
    using CtrlBlockAllocator = typename std::allocator_traits<
        Allocator>::template rebind_alloc<CtrlBlockImpl>;

    CtrlBlockAllocator realAllocator(allocator);

    allocator.~Allocator();

    realAllocator.deallocate(this, 1);
  }
};

template <typename T> class SharedPtr {
private:
  T* obj;
  CtrlBlockBase* ctrlBlock;

public:
  SharedPtr() : obj(nullptr), ctrlBlock(nullptr) {}
  SharedPtr(nullptr_t) : SharedPtr() {}

  void swap(SharedPtr& other) {
    swap(obj, other.obj);
    swap(ctrlBlock, other.ctrlBlock);
  }

  SharedPtr(T* obj_) : obj(obj_) { ctrlBlock = new CtrlBlockImpl<T>(obj); }

  template <typename Deleter> SharedPtr(T* obj_, Deleter deleter_) : obj(obj_) {
    ctrlBlock = new CtrlBlockImpl<T, Deleter>(obj, core2::move(deleter_));
  }

  template <typename Deleter, typename Allocator>
  SharedPtr(T* obj_, Deleter deleter_, Allocator allocator_) : obj(obj_) {
    ctrlBlock = new CtrlBlockImpl<T, Deleter, Allocator>(
        obj, core2::move(deleter_), core2::move(allocator_));
  }

  SharedPtr(SharedPtr& rhs) : obj(rhs.obj), ctrlBlock(rhs.ctrlBlock) {
    if (ctrlBlock) {
      ctrlBlock->addShared();
    }
  }

  SharedPtr(SharedPtr&& rhs) : obj(rhs.obj), ctrlBlock(rhs.ctrlBlock) {
    rhs.obj = nullptr;
    rhs.ctrlBlock = nullptr;
  }

  template <typename U>
  SharedPtr(const SharedPtr<U>& other, T* objElem)
      : obj(objElem), ctrlBlock(other.ctrlBlock) {
    if (ctrlBlock) {
      ctrlBlock->addShared();
    }
  }

  u64 refCount() const { return ctrlBlock ? ctrlBlock->sharedCount + 1 : 0; }

  T* get() { return obj; }

  ~SharedPtr() {
    if (ctrlBlock) {
      ctrlBlock->releaseShared();
    }
  }

  template <typename U> friend class SharedPtr;
};

} // namespace core2
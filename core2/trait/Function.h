#pragma once

#include "core2/Utility.h"
#include "core2/trait/Memory.h"

namespace core2 {

template <typename Signature> class Function;
template <typename Signature> class OptimizedFunction;

namespace detail {

// template <typename Signature> class CallableBase;
// template <typename Signature> class CallableImpl;

template <typename ReturnType, typename... Args> class CallableBase {
public:
  inline virtual ReturnType invoke(void* func, Args&&... args) = 0;
  inline virtual void destroy(void* func) = 0;

  virtual ~CallableBase() {}
};

template <typename Fn, typename ReturnType, typename... Args>
class CallableImpl : public CallableBase<ReturnType, Args...> {
public:
  inline ReturnType invoke(void* func, Args&&... args) override {
    return (*static_cast<Fn*>(func))(core2::forward<Args>(args)...);
  }

  inline void destroy(void* func) override { delete static_cast<Fn*>(func); }

  ~CallableImpl() override {}
};

} // namespace detail

template <typename ReturnType, typename... Args>
class Function<ReturnType(Args...)> {
private:
  std::unique_ptr<detail::CallableBase<ReturnType, Args...>> callable;
  void* func;

public:
  template <typename Fn>
  Function(Fn&& callable_)
      : callable(new detail::CallableImpl<std::remove_reference_t<Fn>,
                                          ReturnType, Args...>()),
        func(new std::remove_reference_t<Fn>(core2::forward<Fn>(callable_))) {}

  inline ReturnType operator()(Args&&... args) {
    return callable->invoke(func, core2::forward<Args>(args)...);
  }

  ~Function() { callable->destroy(func); }
};

namespace detail {

template <typename R, typename... A> class FunctorManagerBase {
public:
  inline virtual R invoke(TypeErasedOptimizedBuffer<>& buffer, A&&... args) = 0;
  inline virtual void destroy(TypeErasedOptimizedBuffer<>& buffer) = 0;
  // virtual void swap(TypeErasedOptimizedBuffer<>& lhs,
  // TypeErasedOptimizedBuffer<>& rhs) = 0;

  inline virtual const std::type_info& type() = 0;

  virtual ~FunctorManagerBase() {}
};

template <typename T, typename R, typename... A>
class FunctorManagerImpl : public FunctorManagerBase<R, A...> {
public:
  inline R invoke(TypeErasedOptimizedBuffer<>& buffer, A&&... args) override {
    return buffer.data<T>()(core2::forward<A>(args)...);
  }

  inline void destroy(TypeErasedOptimizedBuffer<>& buffer) override {
    buffer.destroy<T>();
  }

  inline const std::type_info& type() override { return typeid(T); }

  ~FunctorManagerImpl() {}
};

} // namespace detail

template <typename R, typename... A> class OptimizedFunction<R(A...)> {
  // Operations to support
  // 1. Construct by a callable object
  // 2. Assignment by a callable
  // 3. Swap
  // 4. Get underlying type
  // 5. Invoke

public:
  OptimizedFunction() {}

  template <typename Fn>
  OptimizedFunction(Fn&& func)
      : callable(new detail::FunctorManagerImpl<std::remove_reference_t<Fn>, R,
                                                A...>()) {
    buffer.construct<std::remove_reference_t<Fn>>(core2::forward<Fn>(func));
  }

  template <typename Fn> OptimizedFunction& operator=(Fn&& other) {
    if (callable) {
      callable->destroy(buffer);
      callable.reset();
    }

    // Make a new callable and buffer
    // callable = std::unique_ptr<detail::OptimizedCallableBase<R, A...>>(
    //     new detail::OptimizedCallableImpl<Fn, R, A...>());
    callable.reset(
        new detail::FunctorManagerImpl<std::remove_reference_t<Fn>, R, A...>());
    // callable = std::make_unique<detail::OptimizedCallableBase<R, A...>>(
    //     new detail::OptimizedCallableImpl<Fn, R, A...>());
    buffer.construct<std::remove_reference_t<Fn>>(core2::forward<Fn>(other));

    return *this;
  }

  // void swap(OptimizedFunction& other) {
  //   callable.swap(other.callable);
  //   buffer.swap(other.buffer);
  // }

  inline R operator()(A&&... args) {
    return callable->invoke(buffer, core2::forward<A>(args)...);
  }

  std::type_info type() const { return callable->type(); }

private:
  std::unique_ptr<detail::FunctorManagerBase<R, A...>> callable;
  TypeErasedOptimizedBuffer<> buffer;
};

} // namespace core2

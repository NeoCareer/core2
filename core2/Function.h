#pragma once

#include "core2/Utility.h"

namespace core2 {

template <typename Signature> class Function;

namespace detail {

// template <typename Signature> class CallableBase;
// template <typename Signature> class CallableImpl;

template <typename ReturnType, typename... Args> class CallableBase {
public:
  virtual ReturnType invoke(void* func, Args&&... args) = 0;
  virtual void destroy(void* func) = 0;

  virtual ~CallableBase() {}
};

template <typename Fn, typename ReturnType, typename... Args>
class CallableImpl : public CallableBase<ReturnType, Args...> {
public:
  ReturnType invoke(void* func, Args&&... args) override {
    return (*static_cast<Fn*>(func))(core2::forward<Args>(args)...);
  }

  void destroy(void* func) override { delete static_cast<Fn*>(func); }

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
      : callable(new detail::CallableImpl<Fn, ReturnType, Args...>()),
        func(new Fn(core2::forward<Fn>(callable_))) {}

  inline ReturnType operator()(Args&&... args) {
    return callable->invoke(func, core2::forward<Args>(args)...);
  }

  ~Function() { callable->destroy(func); }
};

} // namespace core2
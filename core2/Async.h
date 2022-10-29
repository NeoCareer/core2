#pragma once

#include "core2/STL.h"
#include "core2/Utility.h"

namespace core2 {

template <typename T> class Future {
private:
  template <typename T_> struct FutureInternal {
    std::mutex mux;
    std::condition_variable available;
    std::optional<T_> value;
  };

  FutureInternal<T>* data;

public:
  Future() { data = new FutureInternal<T>(); }

  ~Future() {}

  Future(Future&& rhs) {
    data = rhs.data;
    rhs.data = nullptr;
  }

  template <typename T_> void set(T_&& result) {
    assert(data);

    std::unique_lock lock(data->mux);

    assert(!data->value.has_value());

    data->value.emplace(core2::forward<T_>(result));

    data->available.notify_all();
  }

  T& get() {
    assert(data);

    std::unique_lock lock(data->mux);

    data->available.wait(lock, [this]() { return data->value.has_value(); });

    return *(data->value);
  }
};

}; // namespace core2
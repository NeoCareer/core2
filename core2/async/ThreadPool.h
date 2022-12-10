#pragma once

#include "core2/STL.h"
#include "core2/Utility.h"

namespace core2 {

class ThreadPool {
private:
  size_t size;
  std::vector<std::thread> threads;

  std::queue<std::function<void()>> taskQueue;
  std::condition_variable readyToContinue;
  std::mutex mux;

  bool stopped = false;

public:
  explicit ThreadPool(size_t size_) : size(size_) {
    const auto worker = [this]() {
      for (;;) {
        std::function<void()> func;

        {
          std::unique_lock lock(mux);
          readyToContinue.wait(
              lock, [this]() { return stopped || !taskQueue.empty(); });

          if (stopped && taskQueue.empty()) {
            return;
          }

          func = core2::move(taskQueue.front());
          taskQueue.pop();
        }

        func();
      }
    };

    for (size_t i = 0; i < size; ++i) {
      threads.emplace_back(worker);
    }
  }

  ~ThreadPool() {
    {
      std::unique_lock lock(mux);
      stopped = true;
    }

    readyToContinue.notify_all();

    for (std::thread& thread : threads) {
      if (thread.joinable()) {
        thread.join();
      }
    }
  }

  template <typename Fn, typename... Args>
  auto addTask(Fn&& func, Args&&... args) {
    using ReturnType = std::result_of_t<Fn(Args...)>;

    auto taskWrapper = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<Fn>(func), std::forward<Args>(args)...));

    auto result = taskWrapper->get_future();

    std::unique_lock lock(mux);

    taskQueue.emplace([taskWrapper]() { (*taskWrapper)(); });

    readyToContinue.notify_all();

    return result;
  }
};

} // namespace core2
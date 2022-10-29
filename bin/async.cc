#include "core2/Core2.h"

using namespace std;
using namespace core2;

constexpr const size_t SIZE = 1 << 16;

size_t compute(size_t val) {
  for (size_t i = 0; i < (1 << 8); ++i) {
    val *= val;
  }

  return val;
}

int main() {
  ThreadPool threadPool(8);
  vector<future<size_t>> futures;

  std::atomic<unsigned> a;

  millsecondDuration();

  auto task = [](size_t index) { return compute(index); };

  for (size_t i = 0; i < SIZE; ++i) {
    futures.emplace_back(threadPool.addTask(task, i));
  }

  for (size_t i = 0; i < SIZE; ++i) {
    assert(futures[i].get() == compute(i));
  }

  cout << "Thread pool: " << millsecondDuration() << endl;

  vector<thread> threads;
  vector<size_t> results(SIZE);

  for (size_t i = 0; i < SIZE; ++i) {
    threads.emplace_back(
        [&results](size_t index) { results[index] = compute(index); }, i);
  }

  for (size_t i = 0; i < SIZE; ++i) {
    threads[i].join();
    assert(results[i] == compute(i));
  }

  cout << "Threads: " << millsecondDuration() << endl;

  return 0;
}
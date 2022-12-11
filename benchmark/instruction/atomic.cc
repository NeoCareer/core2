#include <atomic>
#include <benchmark/benchmark.h>
#include <mutex>
#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>

static void BENCH_ATOMIC(benchmark::State& state) {
  std::atomic_uint64_t value(0);

  for (auto _ : state) {
    benchmark::DoNotOptimize(value.fetch_add(4));
  }
}

static void BENCH_VOLATILE(benchmark::State& state) {
  volatile uint64_t value(0);

  for (auto _ : state) {
    benchmark::DoNotOptimize(value += 4);
  }
}

static uint64_t globalUInt64 = 0;
static std::mutex mux;

struct SyncTool {
  pthread_spinlock_t spinLock;

  uint64_t buffer[128];
  uint64_t size = 0;
  uint64_t begin = 0, end = 0;

  SyncTool() { pthread_spin_init(&spinLock, 0); }

  ~SyncTool() { pthread_spin_destroy(&spinLock); }

  void push(uint64_t value) {
    pthread_spin_lock(&spinLock);

    if (size == 128) {
      pthread_spin_unlock(&spinLock);
      return;
    }

    buffer[end++] = value;

    if (end == 128) {
      end = 0;
    }

    size++;

    pthread_spin_unlock(&spinLock);
  }

  uint64_t pop() {
    pthread_spin_lock(&spinLock);

    if (size == 0) {
      pthread_spin_unlock(&spinLock);
      return 0;
    }

    uint64_t value = buffer[begin++];

    if (begin == 128) {
      begin = 0;
    }

    size--;

    pthread_spin_unlock(&spinLock);

    return value;
  }
};

static SyncTool syncTool;

static void BENCH_THRAED(benchmark::State& state) {
  for (auto _ : state) {
    if (state.thread_index() % 2 == 0) {
      syncTool.push(114514);
    } else {
      benchmark::DoNotOptimize(syncTool.pop());
    }
  }
}

BENCHMARK(BENCH_ATOMIC);
BENCHMARK(BENCH_VOLATILE);
BENCHMARK(BENCH_THRAED)->Threads(4);

BENCHMARK_MAIN();

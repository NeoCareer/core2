#include "core2/Core2.h"
#include <benchmark/benchmark.h>
#include <stdlib.h>
#include <sys/mman.h>

static void BENCH_TRADING_ALLOCATOR(benchmark::State& state) {
  size_t count = 0;
  for (auto _ : state) {
    if (count++ < 254 * 512) {

      void* addr = core2::TradingAllocator::allocate(16);

      benchmark::DoNotOptimize(addr);
    }
  }

  core2::TradingAllocator::deallocateAll();
}

static void BENCH_GLIBC_ALLOCATOR(benchmark::State& state) {
  for (auto _ : state) {
    void* addr = malloc(16);
    benchmark::DoNotOptimize(addr);
  }
}

static void BENCH_TRADING_ALLOCATOR_MIXED_WORKLOAD(benchmark::State& state) {
  core2::TradingAllocator::deallocateAll();

  void* addrs[512 * 254];
  int index = 0;
  int count = 0;
  bool direction = true;

  for (auto _ : state) {
    if (direction) {
      addrs[index] = core2::TradingAllocator::allocate(16);
      assert(addrs[index] != nullptr);
      benchmark::DoNotOptimize(addrs[index]);

      index++;
      count++;

      if (count >= 512 * 254) {
        direction = false;
        index = 0;
        count = 0;
      }
    } else {
      core2::TradingAllocator::deallocate(addrs[index]);
      index = (index + 7) % (512 * 254);
      count++;

      if (count >= 512 * 254) {
        direction = true;
        index = 0;
        count = 0;
      }
    }
  }
}

static void BENCH_GLIBC_ALLOCATOR_MIXED_WORKLOAD(benchmark::State& state) {
  void* addrs[512 * 254];
  int index = 0;
  int count = 0;
  bool direction = true;
  for (auto _ : state) {
    if (true) {
      addrs[index] = malloc(16);
      benchmark::DoNotOptimize(addrs[index]);

      index++;
      count++;

      if (count >= 512 * 254) {
        direction = false;
        index = 0;
        count = 0;
      }
    } else {

      free(addrs[index]);
      index = (index + 7) % (512 * 254);
      count++;

      if (count >= 512 * 254) {
        direction = true;
        index = 0;
        count = 0;
      }
    }
  }
}

static void BENCH_TRADING_ALLOCATOR_RANDOM_WORKLOAD(benchmark::State& state) {
  core2::TradingAllocator::deallocateAll();

  std::unordered_set<void*> allocated;

  for (unsigned i = 0; i < 256 * 254; ++i) {
    void* p = core2::TradingAllocator::allocate(16);
    allocated.insert(p);
  }

  bool direction = true;
  size_t step = 0;

  for (auto _ : state) {
    if (direction) {
      allocated.insert(core2::TradingAllocator::allocate(16));
      step++;

      if (step == 256 * 254) {
        step = 0;
        direction = false;
      }
    } else {
      void* addr = *allocated.begin();
      core2::TradingAllocator::deallocate(addr);
      allocated.erase(addr);
      step++;

      if (step == 256 * 254) {
        step = 0;
        direction = true;
      }
    }
  }
}

static void BENCH_GLIBC_ALLOCATOR_RANDOM_WORKLOAD(benchmark::State& state) {
  std::unordered_set<void*> allocated;

  for (unsigned i = 0; i < 256 * 254; ++i) {
    allocated.insert(malloc(16));
  }

  bool direction = true;
  size_t step = 0;

  for (auto _ : state) {
    if (direction) {
      allocated.insert(malloc(16));
      step++;

      if (step == 256 * 254) {
        step = 0;
        direction = false;
      }
    } else {
      free(*allocated.begin());
      allocated.erase(allocated.begin());
      step++;

      if (step == 256 * 254) {
        step = 0;
        direction = true;
      }
    }
  }
}

static void BENCH_UNORDERED_MAP_RANDOM_WORKLOAD(benchmark::State& state) {
  std::unordered_set<size_t> allocated;

  size_t t = 16;

  for (unsigned i = 0; i < 256 * 254; ++i) {
    allocated.insert(t += 16);
  }

  bool direction = true;
  size_t step = 0;

  for (auto _ : state) {
    if (direction) {
      allocated.insert(t += 16);
      step++;

      if (step == 256 * 254) {
        step = 0;
        direction = false;
        t = 256 * 254 * 16;
      }
    } else {
      allocated.erase(allocated.begin());
      step++;

      if (step == 256 * 254) {
        step = 0;
        direction = true;
      }
    }
  }
}

BENCHMARK(BENCH_TRADING_ALLOCATOR);
BENCHMARK(BENCH_GLIBC_ALLOCATOR);

BENCHMARK(BENCH_TRADING_ALLOCATOR_MIXED_WORKLOAD);
BENCHMARK(BENCH_GLIBC_ALLOCATOR_MIXED_WORKLOAD);

BENCHMARK(BENCH_TRADING_ALLOCATOR_RANDOM_WORKLOAD);
BENCHMARK(BENCH_GLIBC_ALLOCATOR_RANDOM_WORKLOAD);

BENCHMARK(BENCH_UNORDERED_MAP_RANDOM_WORKLOAD);

BENCHMARK_MAIN();

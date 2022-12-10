
#include <benchmark/benchmark.h>
#include <stdlib.h>
#include <sys/mman.h>

constexpr intptr_t PAGE_SIZE = 1ull << 12;
constexpr intptr_t LARGE_PAGE_SIZE = 1ull << 21;
constexpr intptr_t HUGE_PAGE_SIZE = 1ull << 30;
constexpr size_t STEP = 1ull << 12;

static void BENCH_MALLOC_4KB(benchmark::State& state) {
  bool direction = true;
  size_t step = 0;

  void* addrs[STEP];

  for (auto _ : state) {
    addrs[step] = malloc(PAGE_SIZE);

    benchmark::DoNotOptimize(addrs[step]);

    step++;

    if (step == STEP) {
      state.PauseTiming();

      for (size_t i = 0; i < STEP; ++i) {
        free(addrs[i]);
      }

      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_MALLOC_2MB(benchmark::State& state) {
  bool direction = true;
  size_t step = 0;

  void* addrs[STEP];

  for (auto _ : state) {
    addrs[step] = malloc(LARGE_PAGE_SIZE);

    benchmark::DoNotOptimize(addrs[step]);

    step++;

    if (step == STEP) {
      state.PauseTiming();

      for (size_t i = 0; i < STEP; ++i) {
        free(addrs[i]);
      }

      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_MALLOC_1GB(benchmark::State& state) {
  bool direction = true;
  size_t step = 0;

  void* addrs[STEP];

  for (auto _ : state) {
    addrs[step] = malloc(HUGE_PAGE_SIZE);

    benchmark::DoNotOptimize(addrs[step]);

    step++;

    if (step == STEP) {
      state.PauseTiming();

      for (size_t i = 0; i < STEP; ++i) {
        free(addrs[i]);
      }

      step = 0;

      state.ResumeTiming();
    }
  }
}

BENCHMARK(BENCH_MALLOC_4KB);
BENCHMARK(BENCH_MALLOC_2MB);
BENCHMARK(BENCH_MALLOC_1GB);

BENCHMARK_MAIN();

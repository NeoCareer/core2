
#include <benchmark/benchmark.h>
#include <stdlib.h>
#include <sys/mman.h>

constexpr intptr_t PAGE_SIZE = 1ull << 12;
constexpr intptr_t LARGE_PAGE_SIZE = 1ull << 21;
constexpr intptr_t HUGE_PAGE_SIZE = 1ull << 30;
constexpr size_t STEP = 1ull << 12;

static void BENCH_SBRK_4KB(benchmark::State& state) {
  size_t step = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(sbrk(PAGE_SIZE));

    step++;

    if (step == STEP) {
      state.PauseTiming();

      sbrk(-PAGE_SIZE * STEP);
      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_SBRK_2MB(benchmark::State& state) {
  size_t step = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(sbrk(LARGE_PAGE_SIZE));

    step++;

    if (step == STEP) {
      state.PauseTiming();

      sbrk(-LARGE_PAGE_SIZE * STEP);
      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_SBRK_1GB(benchmark::State& state) {
  size_t step = 0;
  for (auto _ : state) {
    benchmark::DoNotOptimize(sbrk(HUGE_PAGE_SIZE));

    step++;

    if (step == STEP) {
      state.PauseTiming();

      sbrk(-HUGE_PAGE_SIZE * STEP);
      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_MMAP_4KB(benchmark::State& state) {
  bool direction = true;
  size_t step = 0;

  void* addrs[STEP];

  for (auto _ : state) {
    addrs[step] = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    benchmark::DoNotOptimize(addrs[step]);

    step++;

    if (step == STEP) {
      state.PauseTiming();

      for (size_t i = 0; i < STEP; ++i) {
        munmap(addrs[i], PAGE_SIZE);
      }

      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_MMAP_2MB(benchmark::State& state) {
  bool direction = true;
  size_t step = 0;

  void* addrs[STEP];

  for (auto _ : state) {
    addrs[step] = mmap(NULL, LARGE_PAGE_SIZE, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    benchmark::DoNotOptimize(addrs[step]);

    step++;

    if (step == STEP) {
      state.PauseTiming();

      for (size_t i = 0; i < STEP; ++i) {
        munmap(addrs[i], LARGE_PAGE_SIZE);
      }

      step = 0;

      state.ResumeTiming();
    }
  }
}

static void BENCH_MMAP_1GB(benchmark::State& state) {
  bool direction = true;
  size_t step = 0;

  void* addrs[STEP];

  for (auto _ : state) {
    addrs[step] = mmap(NULL, HUGE_PAGE_SIZE, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    benchmark::DoNotOptimize(addrs[step]);

    step++;

    if (step == STEP) {
      state.PauseTiming();

      for (size_t i = 0; i < STEP; ++i) {
        munmap(addrs[i], HUGE_PAGE_SIZE);
      }

      step = 0;

      state.ResumeTiming();
    }
  }
}

BENCHMARK(BENCH_SBRK_4KB);
BENCHMARK(BENCH_SBRK_2MB);
BENCHMARK(BENCH_SBRK_1GB);
BENCHMARK(BENCH_MMAP_4KB);
BENCHMARK(BENCH_MMAP_2MB);
BENCHMARK(BENCH_MMAP_1GB);

BENCHMARK_MAIN();

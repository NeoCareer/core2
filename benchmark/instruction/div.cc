
#include <benchmark/benchmark.h>
#include <stdlib.h>
#include <sys/mman.h>

constexpr intptr_t PAGE_SIZE = 1ull << 12;
constexpr intptr_t LARGE_PAGE_SIZE = 1ull << 21;
constexpr intptr_t HUGE_PAGE_SIZE = 1ull << 30;
constexpr size_t STEP = 1ull << 12;

static void BENCH_MUL3(benchmark::State& state) {
  unsigned long long value = 114514;

  for (auto _ : state) {
    benchmark::DoNotOptimize(value *= 3);
  }
}

static void BENCH_DIV3(benchmark::State& state) {
  unsigned long long value = 114514;

  for (auto _ : state) {
    benchmark::DoNotOptimize(value /= 3);
  }
}

BENCHMARK(BENCH_MUL3);
BENCHMARK(BENCH_DIV3);

BENCHMARK_MAIN();

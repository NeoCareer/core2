#include "core2/Core2.h"
#include "core2/Log.h"
#include <benchmark/benchmark.h>
#include <stdlib.h>
#include <sys/mman.h>

using namespace core2::BitAllocator;

static void BENCH_BIT_IMMEDIATE_FREE_WITH_SIZE(benchmark::State& state) {
  for (auto _ : state) {
    void* addr = core2::BitAllocator::allocate(16);
    benchmark::DoNotOptimize(addr);
    core2::BitAllocator::deallocate(addr);
  }
}

BENCHMARK(BENCH_BIT_IMMEDIATE_FREE_WITH_SIZE)
    ->RangeMultiplier(2)
    ->Range(16, 16);

static void BENCH_LIBC_IMMEDIATE_FREE_WITH_SIZE(benchmark::State& state) {
  for (auto _ : state) {
    void* addr = malloc(16);
    benchmark::DoNotOptimize(addr);
    free(addr);
  }
}

BENCHMARK(BENCH_LIBC_IMMEDIATE_FREE_WITH_SIZE)
    ->RangeMultiplier(2)
    ->Range(16, 16);

static void BENCH_BIT_ALLOCATE_A_LOT_WITH_SIZE(benchmark::State& state) {
  for (auto _ : state) {
    void* addr = allocate(16);
    benchmark::DoNotOptimize(addr);
  }
}

BENCHMARK(BENCH_BIT_ALLOCATE_A_LOT_WITH_SIZE)
    ->RangeMultiplier(2)
    ->Range(16, 16);

static void BENCH_LIBC_ALLOCATE_A_LOT_WITH_SIZE(benchmark::State& state) {
  for (auto _ : state) {
    void* addr = malloc(16);
    benchmark::DoNotOptimize(addr);
  }
}

BENCHMARK(BENCH_LIBC_ALLOCATE_A_LOT_WITH_SIZE)
    ->RangeMultiplier(2)
    ->Range(16, 16);

BENCHMARK_MAIN();

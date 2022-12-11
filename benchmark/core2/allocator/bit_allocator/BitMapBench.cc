#include "core2/Core2.h"
#include <benchmark/benchmark.h>

using namespace core2::BitAllocator;

static void BENCH_BITMAP_SINGLE_GET(benchmark::State& state) {
  BitMap bitMap;

  // Get the bit on offset given by benchmark range
  for (auto _ : state) {
    benchmark::DoNotOptimize(bitMap.get(state.range(0)));
  }
}

BENCHMARK(BENCH_BITMAP_SINGLE_GET)->Arg(0)->Arg(255)->Arg(511);

static void BENCH_BITMAP_SINGLE_SET(benchmark::State& state) {
  BitMap bitMap;

  // Get the bit on offset given by benchmark range
  for (auto _ : state) {
    bitMap.set(state.range(0));
    benchmark::ClobberMemory();
  }
}

BENCHMARK(BENCH_BITMAP_SINGLE_SET)->Arg(0)->Arg(255)->Arg(511);

static void BENCH_BITMAP_SINGLE_RESET(benchmark::State& state) {
  BitMap bitMap;

  // Get the bit on offset given by benchmark range
  for (auto _ : state) {
    bitMap.reset(state.range(0));
    benchmark::ClobberMemory();
  }
}

BENCHMARK(BENCH_BITMAP_SINGLE_RESET)->Arg(0)->Arg(255)->Arg(511);

BENCHMARK_MAIN();

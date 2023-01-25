#include "core2/container/Tuple.h"
#include <gtest/gtest.h>
#include <tuple>

namespace {

struct EmptyWidget {};

TEST(TupleTest, EmptyBase) {
  std::tuple<EmptyWidget, int, EmptyWidget> stdTuple;
  core2::Tuple<EmptyWidget, int, EmptyWidget> core2Tuple;
  // Make sure the behavior is the same as standard library
  ASSERT_EQ(sizeof(stdTuple), sizeof(core2Tuple));
}

} // namespace

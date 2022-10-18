#include "lib/Lib.h"
#include <gtest/gtest.h>

TEST(VectorTest, EmptyVector) {
  core::Vector<unsigned> emptyVector;
  ASSERT_EQ(0, emptyVector.getSize());
  ASSERT_TRUE(emptyVector.isEmpty());
}

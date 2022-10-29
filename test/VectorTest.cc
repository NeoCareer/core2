#include "lib/Lib.h"
#include <gtest/gtest.h>

namespace {

constexpr const size_t SIZE = 1 << 10;

struct NonCopyable {
  unsigned value;
  NonCopyable(unsigned value_) : value(value_) {}

  NonCopyable &operator[](const NonCopyable &rhs) = delete;
};

TEST(VectorTest, EmptyVector) {
  core::Vector<unsigned> emptyVector;
  ASSERT_EQ(0, emptyVector.getSize());
  ASSERT_TRUE(emptyVector.isEmpty());
}

TEST(VectorTest, PushBack) {
  core::Vector<unsigned> vector;

  for (size_t i = 0; i < SIZE; ++i) {
    vector.pushBack(i);
  }

  ASSERT_EQ(SIZE, vector.getSize());

  for (size_t i = 0; i < SIZE; ++i) {
    ASSERT_EQ(i, vector[i]);
  }
}

TEST(VectorTest, EmplaceBack) {
  core::Vector<unsigned> vector;

  for (size_t i = 0; i < SIZE; ++i) {
    vector.emplaceBack(i);
  }

  ASSERT_EQ(SIZE, vector.getSize());

  for (size_t i = 0; i < SIZE; ++i) {
    ASSERT_EQ(i, vector[i]);
  }
}

TEST(VectorTest, PopBack) {
  core::Vector<unsigned> vector;

  for (size_t i = 0; i < SIZE; ++i) {
    vector.emplaceBack(i);
  }

  for (size_t i = 0; i < SIZE; ++i) {
    ASSERT_EQ(SIZE - i, vector.getSize());
    vector.popBack();
  }

  ASSERT_TRUE(vector.isEmpty());
}

TEST(VectorTest, NonCopyable) {
  core::Vector<NonCopyable> vector;

  for (size_t i = 0; i < SIZE; ++i) {
    vector.emplaceBack(i);
  }

  ASSERT_EQ(SIZE, vector.getSize());

  for (size_t i = 0; i < SIZE; ++i) {
    ASSERT_EQ(i, vector[i].value);
  }
}

} // namespace
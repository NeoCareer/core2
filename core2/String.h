#pragma once

#include "core2/Math.h"
#include "core2/STL.h"
#include "core2/Utility.h"

namespace core2 {

class String {
public:
  using CharType = char;

  static constexpr const size_t MAX_CAPACITY = 1ull << 40;
  static constexpr const size_t MIN_CAPACITY = 32;

private:
  std::allocator<char> allocator;

  CharType* data;
  size_t capacity = MIN_CAPACITY;
  size_t length = 0;

public:
  explicit String() { data = allocator.allocate(MIN_CAPACITY); }

  explicit String(const char* s) {
    length = strnlen(s, MAX_CAPACITY);
    capacity = std::max(MIN_CAPACITY, core2::nextPowerOf2(length));
    data = allocator.allocate(capacity);
    memcpy(data, s, length);
  }

  CharType* getData() const { return data; }

  size_t getLength() const { return length; }
};

class StringRef {
private:
  friend class String;

  using CharType = String::CharType;

  const CharType* data;
  size_t length;

public:
  explicit StringRef(const String& s) {
    data = s.getData();
    length = s.getLength();
  }

  explicit StringRef(char* s) {
    data = s;
    length = strnlen(data, String::MAX_CAPACITY);

    std::future<int> a;
  }

  const CharType operator[](const size_t index) { return data[index]; }
};

} // namespace core2

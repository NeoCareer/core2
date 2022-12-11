#pragma once

#include "core2/STL.h"

namespace core2 {

class SpinLock {
private:
  std::atomic_flag flag;

public:
  SpinLock() : flag(false) {}

  void lock() {
    while (flag.test_and_set())
      ;
  }

  void unlock() { flag.clear(); }
};

} // namespace core2

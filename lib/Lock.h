#pragma once

#include "lib/All.h"

namespace core {

class SpinLock {
private:
  std::atomic_flag flag;

public:
  SpinLock() : flag(false) {}

  void lock() {
    bool temporary = false;
    while (flag.test_and_set())
      ;
  }

  void unlock() { flag.clear(); }
};

} // namespace core
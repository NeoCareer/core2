#pragma once

#include "lib/All.h"

namespace core {

using namespace std::chrono;

long long millsecondSinceEpoch() {
  auto millsecond =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  return millsecond;
}

float millsecondDuration() {
  static long long previousTime = 0;

  float millsecondDuration =
      (static_cast<float>(millsecondSinceEpoch() - previousTime)) / 1000;
  previousTime = millsecondSinceEpoch();
  return millsecondDuration;
}

} // namespace core
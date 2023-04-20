#pragma once

#include <chrono>

namespace core2::Time {

using namespace std::chrono;

long long millsecondSinceEpoch() {
  auto millsecond =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
  return millsecond;
}

float secondDuration() {
  static long long previousTime = 0;

  float millsecondDuration =
      (static_cast<float>(millsecondSinceEpoch() - previousTime)) / 1000;
  previousTime = millsecondSinceEpoch();
  return millsecondDuration;
}

} // namespace core2::Time

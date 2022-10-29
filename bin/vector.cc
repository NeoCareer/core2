#include "core2/Core2.h"

using namespace core2;
using namespace std;

constexpr const size_t SIZE = 1 << 24;

int main() {
  Vector<unsigned> coreVector;
  vector<unsigned> standardVector;

  millsecondDuration();

  for (unsigned i = 0; i < SIZE; ++i) {
    coreVector.pushBack(i);
  }

  for (unsigned i = 0; i < SIZE; ++i) {
    assert(coreVector[i] == i);
  }

  while (!coreVector.isEmpty()) {
    coreVector.popBack();
  }

  cout << "Core vector: " << millsecondDuration() << endl;

  for (unsigned i = 0; i < SIZE; ++i) {
    standardVector.push_back(i);
  }

  for (unsigned i = 0; i < SIZE; ++i) {
    assert(standardVector[i] == i);
  }

  while (!standardVector.empty()) {
    standardVector.pop_back();
  }

  cout << "Standard vector: " << millsecondDuration() << endl;

  return 0;
}

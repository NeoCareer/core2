#include "core2/Core2.h"

using namespace std;
using namespace core2;

constexpr const unsigned SIZE = 1 << 20;

int main() {
  HashMap<unsigned, unsigned> coreMap;
  unordered_map<unsigned, unsigned> standardMap;
  millsecondDuration();

  for (unsigned i = 0; i < SIZE; ++i) {
    coreMap.insert(i, i);
  }

  for (unsigned i = 0; i < SIZE; ++i) {
    assert(coreMap.get(i) == i);
  }

  cout << "Core hashmap: " << millsecondDuration() << endl;

  for (unsigned i = 0; i < SIZE; ++i) {
    standardMap[i] = i;
  }

  for (unsigned i = 0; i < SIZE; ++i) {
    assert(standardMap[i] == i);
  }

  cout << "Standard hashmap: " << millsecondDuration() << endl;

  return 0;
}

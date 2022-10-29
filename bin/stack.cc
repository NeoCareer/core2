#include "core2/Core2.h"

using namespace std;
using namespace core2;

int main() {
  Stack<int> stack;
  stack.push(1);
  assert(stack.pop() == 1);
  return 0;
}

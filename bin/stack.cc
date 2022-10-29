#include "lib/Lib.h"

using namespace std;
using namespace core;

int main() {
  Stack<int> stack;
  stack.push(1);
  assert(stack.pop() == 1);
  return 0;
}

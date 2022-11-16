#include "absl/strings/str_format.h"
#include "core2/Function.h"
#include "core2/smart_ptr/UniquePtr.h"
#include <iostream>

int f() { return 2; }

int main() {
  // int* value = new int(23);

  // core2::UniquePtr<int> a(value);
  // std::cout << *a << std::endl;
  // std::cout << sizeof(a) << std::endl;

  // auto b = core2::move(a);
  // std::cout << *b << std::endl;
  core2::Function<int()> c1 = []() { return 1; };
  std::cout << c1() << std::endl;

  core2::Function<int()> c2 = &f;
  std::cout << c2() << std::endl;

  return 0;
}

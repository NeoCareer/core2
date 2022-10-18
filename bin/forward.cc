#include "lib/Lib.h"

using namespace core;
using namespace std;

class Widget {
  string v;

public:
  Widget() {}

  Widget(string a, bool b) { v = b ? a : ""; }
};

template <typename T> class V {
  T t;

public:
  template <typename... Args> void emplaceBack(Args &&...args) {
    t = T(core::forward<Args>(args)...);
  }
};

int main() {
  V<Widget> v;

  string s = "123ads fds";

  v.emplaceBack(core::move(s), false);
  v.emplaceBack(core::forward<std::string>(s), true);
  return 0;
}

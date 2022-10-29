#include "core2/Core2.h"

using namespace core2;
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
  template <typename... Args> void emplaceBack(Args&&... args) {
    t = T(core2::forward<Args>(args)...);
  }
};

int main() {
  V<Widget> v;

  string s = "123ads fds";

  v.emplaceBack(core2::move(s), false);
  v.emplaceBack(core2::forward<std::string>(s), true);
  return 0;
}

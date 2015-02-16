#include <iostream>

#include "../cereal/archives/json.hpp"

struct X {
  std::string s;
  template<class A> void serialize(A& ar) {
    ar(CEREAL_NVP(s));
  }
};

template<typename T> void foo(T&& x) {
  std::cout << std::boolalpha << cereal::traits::is_output_serializable<T, cereal::JSONOutputArchive>::value << std::endl;
}

int main() {
  X x;
  const X& y(x);
  foo(x);  // true
  foo(y);  // false
}

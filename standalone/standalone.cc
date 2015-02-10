#include <iostream>

#include "../cereal/archives/json.hpp"

class X {
  public:
    X() = default;
    std::string s;

  private:
    friend class cereal::access;
    template<class A> void serialize(A& ar) {
        ar(CEREAL_NVP(s));
    }
};

int main() {
    {
        X x;
        x.s = "foo";
        {
            cereal::JSONOutputArchive ar(std::cout);
            ar(x);
        }
        std::cout << std::endl;
    }
    {
        cereal::JSONInputArchive ar(std::cin);
        X x;
        while (true) {
            ar(x);
            std::cout << x.s << std::endl;
        }
    }
}

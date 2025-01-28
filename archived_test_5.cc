#include "common.hpp"

template <typename Compare>
class A {
public:
    static Compare cmp;
};
template <typename Compare>
Compare A<Compare>::cmp = {};

template <typename Compare>
void test_func(A<Compare> a) {
    std::cout << a.cmp(0, 1) << "\n";
}

int main() {
    A<mfwu::less<int>> a1;
    A<mfwu::greater<int>> a2;
    test_func(a1);
    test_func(a2);
}
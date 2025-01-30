#include "tuple.hpp"
#include <iostream>

using namespace mfwu;

int main() {
    mfwu::tuple<int> a = 1;
    mfwu::tuple<int, char> b = {1, 'c'};
    mfwu::tuple<int, int, mfwu::tuple<char>> c = {1, 2, 97};
    mfwu::tuple<int, char> b1 = b;
    mfwu::tuple<int, char> b2 = {1, 'd'};

    std::cout << mfwu::get<0>(a) << "\n";
    std::cout << mfwu::get<1>(c) << " " << mfwu::get<0>(mfwu::get<2>(c)) << "\n";
    std::cout << "b == b1 ? " << (b == b1) << "\n";
    std::cout << "b == b2 ? " << (b == b2) << "\n";
    auto [x, y, z] = c;  // failed
    std::cout << x << " " << y << " " << z.first_ << "\n";

    return 0;
}
#include "common.hpp"

int main() {
    // std::cout << "size_t limit: " << std::numeric_limits<size_t>::max() << "\n";
    // for (int i = 0; i < 10; i++) {
    //     unsigned long long n = mfwu::primer_list[i];
    //     size_t res = 1;
    //     for (int i = 2; i <= n; i++) {
    //         res = std::lcm(res, i);
    //     }
    //     std::cout << "n = " << n << ", "
    //               << "res = " << res << "\n";
    // }

    std::cout << "size_t limit: " << std::numeric_limits<size_t>::max() << "\n";
    size_t res = 1;
    for (int n = 2; n < 53; n++) {
        res = std::lcm(res, n);
        std::cout << "n = " << n << ", "
                  << "res = " << res << "\n";
    }

    return 0;
}
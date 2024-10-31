#include "algorithm.hpp"
#include "ut_vector.hpp"

int main() {

    mfwu::vector<int> a = {1, 3, 3, 8, 2, 34, 4, 9, 6};
    mfwu::vector<int> b = {2, 2, 5, 8, 2, 34, 6, 4, 6, 8};
    mfwu::vector<int> c = {1, 3, 6, 5, 0, 4, 4, 4, 6, 1, 5};
    mfwu::vector<int> d = {1, 4, 5, 2, 2, 34, 4, 9, 5, 0, 98, 5};
    mfwu::vector<int> e = {1, 3, 1, 6, -3, 6, 4, 4, 7, 3, 1, 7, 9};

    mfwu::sort(a.begin(), a.end());
    mfwu::sort(b.begin(), b.end());
    mfwu::sort(c.begin(), c.end());
    mfwu::sort(d.begin(), d.end());
    mfwu::sort(e.begin(), e.end());

    mfwu::print_detailed_info(a);
    mfwu::print_detailed_info(b);
    mfwu::print_detailed_info(c);
    mfwu::print_detailed_info(d);
    mfwu::print_detailed_info(e);

    return 0;
}
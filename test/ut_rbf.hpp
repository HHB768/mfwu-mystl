#define __UNIT_TEST_RBF__

#include "common.hpp"
#include "ringbuffer.hpp"

namespace mfwu {

class unit_test_rbf {
public:
    bool use_mfwu_rbf();
private:
    template <typename T, size_t sz>
    void print_rbf(const mfwu::ringbuffer<T, sz>& rbf) {
        for (auto it = rbf.begin(); )
    }
};  // endof class unit_test_rbf


bool unit_test_rbf::use_mfwu_rbf() {
    mfwu::ringbuffer<int, 8> rbf1;
    mfwu::ringbuffer<char, 8> rbf2(5);

    return 0;
}
}  // endof namespace mfwu



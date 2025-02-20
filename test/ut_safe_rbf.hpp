#define __UNIT_TEST_SAFE_RBF__

#include "ringbuffer_with_mutex.hpp"

namespace mfwu {

class unit_test_safe_rbf {
public:
    bool use_rbf_with_mtx();

};  // endof class unit_test_safe_rbf

bool unit_test_safe_rbf::use_rbf_with_mtx() {
    mfwu_rbf::ringbuffer_with_mutex<mfwu::string<char>, 16> rbf(16);

    return 0;
}


}  // endof namespace mfwu
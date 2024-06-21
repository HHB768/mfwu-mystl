#define __UNIT_TEST_VECTOR__

#include "vector.hpp"
#include <iostream>

namespace mfwu {

// friend and test of vector-related funcs
class unit_test_vector {
public:
    bool use_vector_iterator();
    bool use_mfwu_vector();
    bool use_vector_algo();
private:
    struct pod_data {
        int a;
        char b, c, d, e;
    };
    struct npod_data {
        int a;
        npod_data() : a(0) {}
        npod_data(int a_) : a(a_) {}
        ~npod_data() { a = 0x2E; }
    };    
};  // endof class unit_test_vector

bool unit_test_vector::use_vector_iterator() {
    mfwu::vector<int> vec = {1, 2, 3, 4};
    for (mfwu::vector<int>::iterator it = vec.begin(); 
         it != vec.end(); it++) {
        //
    }
}

bool unit_test_vector::use_mfwu_vector() {
    return 0;
}

bool unit_test_vector::use_vector_algo() {
    return 0;
}


}  // endof namespace mfwu

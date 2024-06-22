#define __UNIT_TEST_VECTOR__

#include "vector.hpp"
#include <iostream>

namespace mfwu {

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
    std::cout << "\n------- Test: use vector_iterator -------\n";
    mfwu::vector<int> vec = {1, 2, 3, 4, 5, 6};
    for (mfwu::vector<int>::iterator it = vec.begin(); 
         it < vec.end(); it+=2, --it, it++, --it, ++it) {
        std::cout << mfwu::distance(vec.begin(), it) 
                  << " : " << (++(*it))-- << "  ";
    }
    std::cout << "\n";
    return 0;
}

bool unit_test_vector::use_mfwu_vector() {
    std::cout << "\n------- Test: use mfwu::vector -------\n";
    mfwu::vector<pod_data> vec1;
    mfwu::vector<pod_data> vec2(1);
    mfwu::vector<pod_data> vec3(1, pod_data{});
    mfwu::vector<pod_data> vec4(mfwu::move(vec3));

    mfwu::vector<pod_data> vec5 = {{1, 'a', 'b', 'c', 'd'}, {2, 'e', 'f', 'g', 'h'}};
    mfwu::vector<npod_data> nvec1;
    mfwu::vector<npod_data> nvec2(1);
    mfwu::vector<npod_data> nvec3(1, npod_data());
    mfwu::vector<npod_data> nvec4(mfwu::move(nvec4));
    mfwu::vector<npod_data> nvec5 = {1, 2};
    return 0;
}

bool unit_test_vector::use_vector_algo() {
    return 0;
}


}  // endof namespace mfwu

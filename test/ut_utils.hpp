#include "iterator.hpp"
#include "utils.hpp"

#define __UNIT_TEST_UTILS__

namespace mfwu {

class unit_test_utils {
public:    
    bool use_construct_destroy();
    bool use_uninitialized_op();
    bool use_advance_distance();
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
    char mem[17] = {};
    void print_mem();
};  // endof class unit_test_utils

void unit_test_utils::print_mem() {
#ifndef __UNIT_TEST_UTILS_BRIEF__
    for (char c : mem) {
        if (c == '\0') {
            std::cout << "_" << " ";
        } else {
            std::cout << c << " ";
        }
    }
    std::cout << "\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__
}

bool unit_test_utils::use_construct_destroy() {
    std::cout << "\n------- Test: use construct destroy -------\n";
    
    std::cout << "constructing pod data\n";
    mfwu::construct((pod_data*)mem, 0x31, 'b', 'c', 'd', 'e');
    print_mem();
    std::cout << "destroying pod data\n";
    mfwu::destroy((pod_data*)mem);
    print_mem();

    std::cout << "constructing non-pod data\n";
    mfwu::construct((npod_data*)mem, 0x32);
    print_mem();
    std::cout << "destroying non-pod data\n";
    mfwu::destroy((npod_data*)mem);
    print_mem();
    
    mfwu::random_access_iterator<pod_data> first((pod_data*)mem), last((pod_data*)mem+2);
    std::cout << "constructing pod data with iterator\n";
    mfwu::construct(first, last, 0x33, 'b', 'c', 'd', 'e');
    print_mem();
    std::cout << "destroying pod data with iterator\n";
    mfwu::destroy(first, last);
    print_mem();

    mfwu::random_access_iterator<npod_data> nfirst((npod_data*)mem), nlast((npod_data*)mem+4);
    std::cout << "constructing non-pod data with iterator\n";
    mfwu::construct(nfirst, nlast, 0x34);
    print_mem();
    std::cout << "destroying non-pod data with iterator\n";
    mfwu::destroy(nfirst, nlast);
    print_mem();
    
    return 0;
}


}  // endof namespace mfwu

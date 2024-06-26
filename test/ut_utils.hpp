#define __UNIT_TEST_UTILS__

#include "iterator.hpp"
#include "utils.hpp"

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
        ~npod_data() { a = 0x2E; }  // '.'  <- 可爱捏
    };
    char mem[17] = {};
    void print_mem();
    void set_mem_zero() {
        for (char c : mem) {
            c = 0;
        }
    }
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
    set_mem_zero();
    
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

bool unit_test_utils::use_uninitialized_op() {
    std::cout << "\n------- Test: use uninitialized operations -------\n";
    set_mem_zero();
    mfwu::random_access_iterator<pod_data> first((pod_data*)mem), last((pod_data*)mem+2);
    std::cout << "filling pod data with iterator\n";
    mfwu::uninitialized_fill(first, last, pod_data{0x33, 'b', 'c', 'd', 'e'});
    print_mem();
    std::cout << "filling non-pod data with iterator\n";
    mfwu::random_access_iterator<npod_data> nfirst((npod_data*)mem), nlast((npod_data*)mem+4);
    mfwu::uninitialized_fill(nfirst, nlast, npod_data{0x34});
    print_mem();

    std::cout << "filling pod data n times\n";
    mfwu:uninitialized_fill_n(first, 2, pod_data{0x35, 'e', 'd', 'c', 'b'});
    print_mem();
    std::cout << "filling non-pod data n times\n";
    mfwu::uninitialized_fill_n(nfirst, 4, npod_data{0x36});
    print_mem();

    std::cout << "copying pod data with iterator\n";
    std::vector<pod_data>  vec(2, pod_data{0X37, 'Q', 'E', 'W', 'R'});
    mfwu::uninitialized_copy(vec.begin(), vec.end(), first);
    print_mem();
    std::cout << "copying non-pod data with iterator\n";
    std::vector<npod_data>  nvec(4, npod_data{0X38});
    mfwu::uninitialized_copy(nvec.begin(), nvec.end(), nfirst);
    print_mem();
    
    return 0;
}

bool unit_test_utils::use_advance_distance() {
    std::cout << "\n------- Test: use uninitialized operations -------\n";
    set_mem_zero();
    std::cout << "testing distance of iterator\n";
    mfwu::input_iterator<pod_data> first((pod_data*)mem), last((pod_data*)mem+2);
    mfwu::random_access_iterator<pod_data> rfirst((pod_data*)mem), rlast((pod_data*)mem+2);
#ifdef __UNIT_TEST_UTILS_BRIEF__
    mfwu::distance(first, last), mfwu::distance(rlast, rfirst);
#else // !__UNIT_TEST_UTILS_BRIEF__
    std::cout << "distance(first, last) = " << mfwu::distance(first, last) << "\n";
    std::cout << "distance(rlast, rfirst) = " << mfwu::distance(rlast, rfirst) << "\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__
    mfwu::input_iterator<npod_data> nfirst((npod_data*)mem), nlast((npod_data*)mem+4);
    mfwu::random_access_iterator<npod_data> nrfirst((npod_data*)mem), nrlast((npod_data*)mem+4);
#ifdef __UNIT_TEST_UTILS_BRIEF__
    mfwu::distance(nfirst, nlast), mfwu::distance(nrlast, nrfirst);
#else  // __UNIT_TEST_UTILS_BRIEF__
    std::cout << "distance(nfirst, nlast) = " << mfwu::distance(nfirst, nlast) << "\n";
    std::cout << "distance(nrlast, nrfirst) = " << mfwu::distance(nrlast, nrfirst) << "\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__
    
    std::cout << "testing advance of iterator\n";
    mfwu::advance(first, 2);
#ifndef __UNIT_TEST_UTILS_BRIEF__
    std::cout << "after proper advance, we have: \n";
    std::cout << "first == last : " << (first == last ? "true" : "false") << "\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__
    mfwu::advance(nfirst, 4);
#ifndef __UNIT_TEST_UTILS_BRIEF__
    std::cout << "nfirst == nlast : " << (nfirst == nlast ? "true" : "false") << "\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__

    return 0;
}

}  // endof namespace mfwu

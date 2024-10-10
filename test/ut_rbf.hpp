#define __UNIT_TEST_RBF__

#include "ringbuffer.hpp"

namespace mfwu {

class unit_test_rbf {
public:
    struct test_string {
        char data[32];
        test_string() {
            bzero(data, 32);
        }
        test_string(const char* str) {
            bzero(data, 32);
            strcpy(data, str);
        }
        operator char*() const {
            return (char*)data;
        }
    };  // endof struct test_string
    bool use_mfwu_rbf();
private:
    template <typename T, size_t sz>
    void print_rbf(const mfwu::ringbuffer<T, sz>& rbf) {
        for (auto it = rbf.begin(); it != rbf.end(); ++it) {
#ifdef __UNIT_TEST_RBF__
#ifndef __UNIT_TEST_RBF_BRIEF__
            std::cout << *it << "|\n";
#else   // __UNIT_TEST_RBF_BRIEF__
            *it = 42;
#endif  // __UNIT_TEST_RBF_BRIEF__
#endif  // __UNIT_TEST_RBF__
        }
#ifdef __UNIT_TEST_RBF__
#ifndef __UNIT_TEST_RBF_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_RBF_BRIEF__
#endif  // __UNIT_TEST_RBF__
    }
    template <typename T, size_t sz>
    void print_rbf_with_padding(const mfwu::ringbuffer<T, sz>& rbf) {
        for (auto it = rbf.begin(); it != rbf.end(); ++it) {
#ifdef __UNIT_TEST_RBF__
#ifndef __UNIT_TEST_RBF_BRIEF__
            std::cout << it.get_ptr()->full_string('_') << "|\n";
#else   // __UNIT_TEST_RBF_BRIEF__
            it.get_ptr()->full_string('_');
#endif  // __UNIT_TEST_RBF_BRIEF__
#endif  // __UNIT_TEST_RBF__
        }
#ifdef __UNIT_TEST_RBF__
#ifndef __UNIT_TEST_RBF_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_RBF_BRIEF__
#endif  // __UNIT_TEST_RBF__
    }
};  // endof class unit_test_rbf


bool unit_test_rbf::use_mfwu_rbf() {
    std::cout << "\n------- Test: use mfwu::ringbuffer -------\n";
    std::cout << "constructing & copying & moving\n";
    mfwu::ringbuffer<int, 8> rbf1;
    mfwu::ringbuffer<char, 8> rbf2(5);
    mfwu::ringbuffer<test_string, 35> rbf3(10);
    mfwu::ringbuffer<int, 8> rbf4 = rbf1;
    mfwu::ringbuffer<int, 8> rbf5 = mfwu::move(rbf1);
    mfwu::ringbuffer<int, 8> rbf6;
    rbf6 = rbf4;
    rbf6 = mfwu::move(rbf5);
    mfwu::ringbuffer<char, 8> rbf7 = rbf2;
    mfwu::ringbuffer<char, 8> rbf8 = mfwu::move(rbf7);
    mfwu::ringbuffer<char, 8> rbf9;
    rbf9 = rbf8;
    rbf9 = mfwu::move(rbf8);
    // i wonder what will happen when a = std::move(a)
    // GPT ans:
    /*
        No guarantee there. In C++'s STL, doing a = std::move(a) 
        typically leads to undefined behavior.
        X-Q3 24.10.07
    */

    std::cout << "\nwrite/read ops\n";
    rbf2.write(0x21);
    rbf2.write(0x23);
    // std::cout << "test\n";
    print_rbf_with_padding(rbf2);
    rbf2.read();
    print_rbf_with_padding(rbf2);

    rbf3.write("abcdefg\0");
    rbf3.write("hello ngmay\0");
    print_rbf_with_padding(rbf3);
    test_string str = rbf3.read();
    print_rbf_with_padding(rbf3);
#ifdef __UNIT_TEST_RBF__
#ifndef __UNIT_TEST_RBF_BRIEF__
    std::cout << str << "\n";
#endif  // __UNIT_TEST_RBF_BRIEF__
#endif  // __UNIT_TEST_RBF__
    std::cout << "\nread/write ptr pos ctrl\n";
    // TODO

    return 0;
}
}  // endof namespace mfwu



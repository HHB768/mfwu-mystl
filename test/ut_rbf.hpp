#define __UNIT_TEST_RBF__

#include "common.hpp"
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
            std::cout << *it << "|\n";
        }
        std::cout << "\n";
    }
    template <typename T, size_t sz>
    void print_rbf_with_padding(const mfwu::ringbuffer<T, sz>& rbf) {
        for (auto it = rbf.begin(); it != rbf.end(); ++it) {
            std::cout << it.get_ptr()->full_string('_') << "|\n";
        }
        std::cout << "\n";
    }
};  // endof class unit_test_rbf


bool unit_test_rbf::use_mfwu_rbf() {
    mfwu::ringbuffer<int, 8> rbf1;
    mfwu::ringbuffer<char, 8> rbf2(5);
    mfwu::ringbuffer<test_string, 35> rbf3(10);

    rbf2.write(0x21);
    rbf2.write(0x23);
    // std::cout << "test\n";
    print_rbf_with_padding(rbf2);
    rbf2.read();
    print_rbf_with_padding(rbf2);

    rbf3.write("abcdefg\0");
    rbf3.write("hello ngmay\0");
    print_rbf_with_padding(rbf3);
    rbf3.read();
    print_rbf_with_padding(rbf3);
    return 0;
}
}  // endof namespace mfwu



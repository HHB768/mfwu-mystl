#define __UNIT_TEST_STRING__

#include "string.hpp"

namespace mfwu{

class unit_test_string {
public:
    bool use_mfwu_string();
    bool use_mfwu_tiny_string();

private:
    template <typename Str>
    void print_basic_info(const Str& str) {
        // try new pattern ! 122124
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
        std::cout << "size: " << str.size() << "\n";
        std::cout << "capacity: " << str.capacity() << "\n";
        std::cout << "empty: " << (str.empty() ? "true" : "false") << "\n";
        if (!str.empty()) {
            std::cout << "front: " << str.front() << "\n";
            std::cout << "back: " << str.back() << "\n";
        }
#else  // __UNIT_TEST_STRING_BRIEF__
        str.size();
        str.capacity();
        str.empty();
        if (!str.empty()) {
            str.front();
            str.back();
        }
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
    }
    template <typename C, typename Alloc>
    void print_detailed_info(const mfwu::string<C, Alloc>& str) {
        print_basic_info(str);
        for (auto it = str.begin(); it != str.last_; ++it) {
            if (it < str.end()) {
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
                std::cout << *it;
#else  // __UNIT_TEST_STRING_BRIEF__
                *it;
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
            } else {
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
                std::cout << "_";
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
            }
        }
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
    }
    template <typename C, size_t Bfsz, typename Alloc>
    void print_detailed_info(const mfwu::tiny_string<C, Bfsz, Alloc>& str) {
        print_basic_info(str);
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
        std::cout << "is_tiny ? " << (str.is_tiny_ ? "true" : "false") << "\n";
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
        for (auto it = str.begin(); it != str.last_; ++it) {
            if (it < str.end()) {
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
                std::cout << *it;
#else  // __UNIT_TEST_STRING_BRIEF__
                *it;
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
            } else {
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
                std::cout << "_";
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
            }
        }
#ifdef __UNIT_TEST_STRING__
#ifndef __UNIT_TEST_STRING_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_STRING_BRIEF__
#endif  // __UNIT_TEST_STRING__
    }
};  // endof class unit_test_string

bool unit_test_string::use_mfwu_string() {
    std::cout << "\n------- Test: use mfwu::string -------\n";
    mfwu::string str1;
    print_detailed_info(str1);
    mfwu::string str2 = "niganma-aiyo";  // const char[13]
    print_detailed_info(str2);  // size = 12
    mfwu::string str3("niagnma-aiyo\0");  // const char[14]
    print_detailed_info(str3);  // size = 12
    mfwu::string<char> str4 = {'a', 'b', 'c'};
    mfwu::string<char> str5(str4.begin(), str4.end());
    mfwu::string<char> str6 = mfwu::move(str5);
    str4 = mfwu::move(str6);
    print_detailed_info(str4);
    // TODO: test
    return 0;
}

bool unit_test_string::use_mfwu_tiny_string() {
    std::cout << "\n------- Test: use mfwu::tiny_string -------\n";
    mfwu::tiny_string str1;
    print_detailed_info(str1);
    mfwu::tiny_string<char, 16> str2 = "niganma-aiyo";  // const char[13]
    print_detailed_info(str2);  // size = 12
    mfwu::tiny_string<char, 16> str3 = "niagnma-aiyo\0";  // const char[14]
    print_detailed_info(str3);  // size = 12
    // TODO: test
    return 0;
}

}  // endof namespace mfwu


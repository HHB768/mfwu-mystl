#define __UNIT_TEST_STACK__

#include "stack.hpp"

namespace mfwu {

class unit_test_stack {
public:
    bool use_mfwu_stack();
private:
    template <typename T, typename Container>
    void print_stack_basic_info(const mfwu::stack<T, Container>& stk) {
#ifdef __UNIT_TEST_STACK__
#ifndef  __UNIT_TEST_STACK_BRIEF__
        std::cout << "empty : " 
                  << (stk.empty() ? "true" : "false") << ", "
                  << "size : " << stk.size();
        stk.empty() ? std::cout << "\n" 
                    : std::cout << ", top : " << stk.top() << "\n";
#else  // __UNIT_TEST_STACK_BRIEF__
        stk.empty();
#endif  // __UNIT_TEST_STACK_BRIEF__
#endif  // __UNIT_TEST_STACK__
    }
    template <typename T, typename Container>
    void print_stack_detailed_info(const mfwu::stack<T, Container>& stk) {
        print_stack_basic_info(stk);
        for (auto it = stk.stk_.begin(); it != stk.stk_.end(); ++it) {
#ifdef __UNIT_TEST_STACK__
#ifndef  __UNIT_TEST_STACK_BRIEF__
            std::cout << *it << " ";
#else  // __UNIT_TEST_STACK_BRIEF__
            *it;
#endif  // __UNIT_TEST_STACK_BRIEF__
#endif  // __UNIT_TEST_STACK__
        }
#ifdef __UNIT_TEST_STACK__
#ifndef  __UNIT_TEST_STACK_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_STACK_BRIEF__
#endif  // __UNIT_TEST_STACK__
    }
};  // endof class unit_test_stack

bool unit_test_stack::use_mfwu_stack() {
    std::cout << "\n------- Test: use mfwu::stack -------\n";
    std::cout << "building & copying & moving\n";
    mfwu::stack<int> stk1;
    print_stack_detailed_info(stk1);
    mfwu::stack<char> stk2 = {1, 2, 3, 4, 5};
    print_stack_detailed_info(stk2);
    mfwu::stack<int, mfwu::deque<int, 4>> stk3(stk2.stk_.begin() + 1, stk2.stk_.end());
    print_stack_detailed_info(stk3);
    mfwu::stack<int> stk4(6, 6);
    print_stack_detailed_info(stk4);
    mfwu::stack<int> stk5 = stk4;
    print_stack_detailed_info(stk5);
    mfwu::stack<int> stk6 = mfwu::move(stk5);
    print_stack_detailed_info(stk6);
    stk6 = stk4;
    stk6 = mfwu::move(stk4);
    print_stack_detailed_info(stk6);

    mfwu::swap(stk6, mfwu::stack<int>{});
    print_stack_detailed_info(stk6);
    stk6.emplace(7);
    stk6.push(8);
    print_stack_detailed_info(stk6);
    for (int i = 0; i < 10; i++) {
        int num = rand() % 10;
        stk6.push(num);
        print_stack_detailed_info(stk6);
    }
    for (int i = 0; i < 12; i++) {
        int num = rand() % 10;
        if (num & 1) {
            stk6.push(num);
        } else {
            stk6.pop();
        }
        print_stack_detailed_info(stk6);
    }

    return 0;
}
}  // ndof namespace mfwu
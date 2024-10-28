#define __UNIT_TEST_QUEUE__

#include "queue.hpp"

namespace mfwu {

class unit_test_queue {
public:
    bool use_mfwu_queue();
private:
    template <typename T, size_t size, typename Container>
    void print_queue_basic_info(const mfwu::queue<T, size, Container>& stk) {
        std::cout << "empty : " << stk.empty() << " "
                  << "size : " << stk.size() << " "
                  << "top : " << stk.top() << "\n";
    }
    template <typename T, size_t size, typename Container>
    void print_queue_detailed_info(const mfwu::queue<T, size, Container>& stk) {
        print_queue_basic_info(stk);
        for (auto it = stk.stk_.begin(); it != stk.stk_.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";
    }
};  // endof class unit_test_queue

bool unit_test_queue::use_mfwu_queue() {
    std::cout << "\n------- Test: use mfwu::queue -------\n";
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

    stk6.emplace(7);
    stk6.push(8);
    for (int i = 0; i < 10; i++) {
        int num = rand() % 10;
        stk6.push(num);
    }
    for (int i = 0; i < 15; i++) {
        int num = rand() % 10;
        if (num & 1) {
            stk6.push(num);
        } else {
            stk6.pop();
        }
    }

    return 0;
}
}  // ndof namespace mfwu
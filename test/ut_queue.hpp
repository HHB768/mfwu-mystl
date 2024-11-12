#define __UNIT_TEST_QUEUE__

#include "queue.hpp"

namespace mfwu {

class unit_test_queue {
public:
    bool use_mfwu_queue();
private:
    template <typename T, typename Container>
    void print_queue_basic_info(const mfwu::queue<T, Container>& q) {
#ifdef __UNIT_TEST_QUEUE__
#ifndef  __UNIT_TEST_QUEUE_BRIEF__
        std::cout << "empty : " 
                  << (q.empty() ? "true" : "false") << ", "
                  << "size : " << q.size();
        q.empty() ? std::cout << "\n" 
                    : std::cout << ", front : " << q.front() << "\n";
#else  // __UNIT_TEST_QUEUE_BRIEF__
        q.empty();
#endif  // __UNIT_TEST_QUEUE_BRIEF__
#endif  // __UNIT_TEST_QUEUE__
    }
    template <typename T, typename Container>
    void print_queue_detailed_info(const mfwu::queue<T, Container>& q) {
        print_queue_basic_info(q);
        for (auto it = q.q_.begin(); it != q.q_.end(); ++it) {
#ifdef __UNIT_TEST_QUEUE__
#ifndef  __UNIT_TEST_QUEUE_BRIEF__
            // std::cout << mfwu::distance(it, q.q_.end()) << ":";
            // std::cout << it.get_pos() << ":" << it.get_cur() << ":";
            std::cout << *it << " ";
#else  // __UNIT_TEST_QUEUE_BRIEF__
            *it;
#endif  // __UNIT_TEST_QUEUE_BRIEF__
#endif  // __UNIT_TEST_QUEUE__
        }
#ifdef __UNIT_TEST_QUEUE__
#ifndef  __UNIT_TEST_QUEUE_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_QUEUE_BRIEF__
#endif  // __UNIT_TEST_QUEUE__
    }
};  // endof class unit_test_queue

bool unit_test_queue::use_mfwu_queue() {
    std::cout << "\n------- Test: use mfwu::queue -------\n";
    std::cout << "building & copying & moving\n";
    mfwu::queue<int> q1;
    print_queue_detailed_info(q1);
    mfwu::queue<char> q2 = {1, 2, 3, 4, 5};
    print_queue_detailed_info(q2);
    mfwu::queue<int, mfwu::deque<int, 4>> q3(q2.q_.begin() + 1, q2.q_.end());
    print_queue_detailed_info(q3);
    mfwu::queue<int> q4(6, 6);
    print_queue_detailed_info(q4);
    mfwu::queue<int> q5 = q4;
    print_queue_detailed_info(q5);
    mfwu::queue<int> q6 = mfwu::move(q5);
    print_queue_detailed_info(q6);
    q6 = q4;
    q6 = mfwu::move(q4);
    print_queue_detailed_info(q6);

    mfwu::swap(q6, mfwu::queue<int>{});
    print_queue_detailed_info(q6);
    q6.emplace(7);
    q6.push(8);
    print_queue_detailed_info(q6);
    // for (int i = 0; i < 10; i++) {
    //     int num = rand() % 10;
    //     q6.push(num);
    //     print_queue_detailed_info(q6);
    // }
    // for (int i = 0; i < 12; i++) {
    //     int num = rand() % 10;
    //     if (num & 1) {
    //         q6.push(num);
    //     } else {
    //         q6.pop();
    //     }
    //     print_queue_detailed_info(q6);
    // }

    return 0;
}
}  // ndof namespace mfwu
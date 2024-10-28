#define __UNIT_TEST_QUEUE__

#include "queue.hpp"

namespace mfwu {

class unit_test_queue {
public:
    bool use_mfwu_queue();
private:
    template <typename T, typename Container>
    void print_queue_basic_info(const mfwu::queue<T, Container>& q) {
        std::cout << "empty : " << q.empty() << " "
                  << "size : " << q.size() << " "
                  << "top : " << q.top() << "\n";
    }
    template <typename T, typename Container>
    void print_queue_detailed_info(const mfwu::queue<T, Container>& q) {
        print_queue_basic_info(q);
        for (auto it = q.q_.begin(); it != q.q_.end(); ++it) {
            std::cout << *it << " ";
        }
        std::cout << "\n";
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

    q6.emplace(7);
    q6.push(8);
    for (int i = 0; i < 10; i++) {
        int num = rand() % 10;
        q6.push(num);
    }
    for (int i = 0; i < 15; i++) {
        int num = rand() % 10;
        if (num & 1) {
            q6.push(num);
        } else {
            q6.pop();
        }
    }

    return 0;
}
}  // ndof namespace mfwu
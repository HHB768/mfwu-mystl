#define __UNIT_TEST_DEQUE__

#include "deque.hpp"

namespace mfwu {

class unit_test_deque {
public:
    bool use_deque_block();
    bool use_deque_iterator();
    bool use_mfwu_deque();
private:
    template <typename T, size_t size, typename Alloc>
    void print_block(const typename mfwu::deque<T, size, Alloc>::block& blk) {
        for (auto it = blk.start(); it != blk.finish(); ++it) {
            if (blk.begin() <= it && it < blk.end()) {
                std::cout << *it << " ";
            } else {
                std::cout << "_" << " ";
            }
        }
        std::cout << "\n";
    }
    // if you add template <>, you're adding a explicit specialization
    // if you comment it, it is a overloaded function
    // you cannot give a explicit specialization, you can only
    // explicit specialize them in namespace bcz in class scope
    // they are all member functions, not related ones
    // sorted from GPT answers 0813:2145
    // template <>
    void print_block(const typename mfwu::deque<int, 16UL,
        mfwu::DefaultAllocator<int, mfwu::malloc_alloc>>::block& blk) {
        for (auto it = blk.start(); it != blk.finish(); ++it) {
            if (blk.begin() <= it && it < blk.end()) {
                std::cout << *it << " ";
            } else {
                std::cout << "_" << " ";
            }
        }
        std::cout << "\n";
    }
    template <typename T, size_t size, typename Alloc>
    void print_deque(const mfwu::deque<T, size, Alloc>& deq) {
        std::cout << "size: " << deq.size() << "\n";
        for (auto blk = deq.begin_; blk != deq.end_; ++blk) {
            print_block<T, size, Alloc>(**blk);
        }
        std::cout << "\n";
    }
    
};  // endof class unit_test_deque


bool unit_test_deque::use_deque_block() {
    mfwu::deque<int, 16>::block blk1;
    // print_block<int, 16, 
    //      mfwu::DefaultAllocator<int, mfwu::malloc_alloc>>(blk1);
    print_block(blk1);
    mfwu::deque<int, 16>::block blk2(0);
    print_block(blk2);
    mfwu::deque<int, 16>::block blk3(3);
    print_block(blk3);
    mfwu::deque<int, 16>::block blk4(2, 4, true);
    print_block(blk4);
    auto blk5 = blk4;
    print_block(blk5);
    auto blk6 = mfwu::move(blk4);
    print_block(blk6);
    blk1.assign(blk6.begin(), blk6.end() - 1, blk1.begin() + 1);
    print_block(blk1);

    if (blk5.has_front_space()) blk5.push_front(1);
    blk5.pop_back();
    if (blk5.has_back_space()) blk5.push_back(5);
    print_block(blk5);

    if (blk6.has_back_space()) blk6.insert(blk6.begin(), 3, false);
    print_block(blk6);
    blk6.pop_back();
    print_block(blk6);
    blk6.insert(blk6.begin(), 3, true);
    print_block(blk6);
    return 0;
}

bool unit_test_deque::use_deque_iterator() {
    mfwu::deque<int, 4> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (auto it = dq.begin() + 1; it < dq.end(); ++it) {
        std::cout << --*(++--it) << " ";
    }
    std::cout << "\n";

    return 0;
}

bool unit_test_deque::use_mfwu_deque() {
    using deque4 = mfwu::deque<int, 4>;
    using deque8 = mfwu::deque<int, 8>;

    deque4 dq1;
    print_deque(dq1);
    deque4 dq2 = {1, 2};
    print_deque(dq2);
    deque4 dq3 = {3, 4, 5, 6};
    print_deque(dq3);
    deque4 dq4 = {7, 8, 9, 0, 1};
    print_deque(dq4);
    deque8 dq5 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    print_deque(dq5);
    
    deque4 dq6 = dq2;
    print_deque(dq6);
    deque4 dq7 = mfwu::move(dq3);
    print_deque(dq7);
    deque8 dq8;
    dq8 = dq5 = {1, 3, 5, 7, 9};
    print_deque(dq8);

    dq4.pop_back();
    dq4.push_back(8);
    dq4.pop_front();
    dq4.pop_front();
    dq4.insert(dq4.begin(), 6);
    dq4.push_front(1);
    dq4.push_front(2);
    dq4.erase(dq4.begin() + 3);
    print_deque(dq4);
    // TODO: 直接爆炸
    

    return 0;
}

}  // endof namespace mfwu


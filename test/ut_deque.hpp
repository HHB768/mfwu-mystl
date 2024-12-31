#ifndef __UNIT_TEST_DEQUE__
#define __UNIT_TEST_DEQUE__

#include "deque.hpp"

namespace mfwu {

class unit_test_deque {
public:
    bool use_deque_block();
    bool use_deque_iterator();
    bool use_mfwu_deque();
    bool use_ranged_insert_erase();
    using deque4 = mfwu::deque<int, 4>;
    using deque8 = mfwu::deque<int, 8>;
private:
    template <typename T, size_t size, typename Alloc>
    static void print_block(const typename mfwu::deque<T, size, Alloc>::block& blk) {
        for (auto it = blk.start(); it != blk.finish(); ++it) {
            if (blk.begin() <= it && it < blk.end()) {
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
                std::cout << *it << " ";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
            } else {
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
                std::cout << "_" << " ";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
            }
        }
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
    }
    // if you add template <>, you're adding a explicit specialization
    // if you comment it, it is a overloaded function
    // you cannot give a explicit specialization, you can only
    // explicit specialize them in namespace bcz in class scope
    // they are all member functions, not related ones
    // sorted from GPT answers 0813:2145
    // template <>
    static void print_block(const typename mfwu::deque<int, 16UL,
        mfwu::DefaultAllocator<int, mfwu::malloc_alloc>>::block& blk) {
        for (auto it = blk.start(); it != blk.finish(); ++it) {
            if (blk.begin() <= it && it < blk.end()) {
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
                std::cout << *it << " ";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
            } else {
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
                std::cout << "_" << " ";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
            }
        }
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
    }
    template <typename T, size_t size, typename Alloc>
        static void print_deque(const mfwu::deque<T, size, Alloc>& deq) {
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
        std::cout << "size: " << deq.size() << "\n";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
        for (auto blk = deq.begin_; blk != deq.end_; ++blk) {
            print_block<T, size, Alloc>(**blk);
        }
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
        std::cout << "\n";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
    }
    
};  // endof class unit_test_deque


bool unit_test_deque::use_deque_block() {
    std::cout << "\n------- Test: use deque block -------\n";
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
    std::cout << "\n------- Test: use deque iterator -------\n";
    mfwu::deque<int, 4> dq = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    for (auto it = dq.begin() + 1; it < dq.end(); ++it) {
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
        std::cout << --*(++--it) << " ";
#else  // __UNIT_TEST_DEQUE_BRIEF__
        --*(++--it);
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__
    }
#ifdef __UNIT_TEST_DEQUE__
#ifndef  __UNIT_TEST_DEQUE_BRIEF__
    std::cout << "\n";
#endif  // __UNIT_TEST_DEQUE_BRIEF__
#endif  // __UNIT_TEST_DEQUE__

    return 0;
}

bool unit_test_deque::use_mfwu_deque() {
    std::cout << "\n------- Test: use mfwu::deque -------\n";

    std::cout << "constructing\n";
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

    std::cout << "push/pop/insert/erasing\n";
    dq4.pop_back();  // TODO
    print_deque(dq4);
    dq4.push_back(8);
    print_deque(dq4);
    dq4.pop_front();
    print_deque(dq4);
    dq4.pop_front();
    print_deque(dq4);
    dq4.insert(dq4.begin(), 6);
    print_deque(dq4);
    dq4.push_front(1);
    print_deque(dq4);
    dq4.push_front(2);
    print_deque(dq4);
    dq4.erase(dq4.begin() + 3);
    print_deque(dq4);
    dq4.erase(dq4.begin() + 1);
    print_deque(dq4);
    dq4.erase(dq4.begin() + 1);
    print_deque(dq4);
    dq4.erase(dq4.begin() + 1);  // TODO
    print_deque(dq4);
    dq4.erase(dq4.begin() + 1);
    print_deque(dq4);

    return 0;
}

bool unit_test_deque::use_ranged_insert_erase() {
    std::cout << "\n------- Test: use deque ranged insert/erase -------\n";
    /*deque8*/ deque4 dq1 = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    /*deque8*/ deque4 dq2 = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    /*deque4*/ deque8 dq3 = {1, 3, 5, 7, 9, 1};
    /*deque4*/ deque8 dq4 = {0, 8, 6, 4, 2, 0};
    print_deque(dq1);
    dq1.insert(dq1.begin() + 5, dq2.begin(), dq2.end());
    print_deque(dq1);
    dq1.insert(dq1.begin() + 5, 1);
    print_deque(dq1);
    dq1.erase(dq1.begin() + 2, dq1.begin() + 7);
    print_deque(dq1);
    dq1.insert(dq1.begin(), dq3.begin() + 1, dq3.end() - 2);
    print_deque(dq1);
    dq1.erase(dq1.end() - 2);
    print_deque(dq1);
    dq1.insert(dq1.end() - 5, dq4.begin() + 4, dq4.end());
    print_deque(dq1);
    dq1.erase(dq1.begin() + 7, dq1.end() - 1);
    print_deque(dq1);
    return 0;

    // possible bugs remain :P
    // need more test  2024.10.27
}


}  // endof namespace mfwu



#endif  // __UNIT_TEST_DEQUE__
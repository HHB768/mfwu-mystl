#define __UNIT_TEST_HEAP__

#include "heap.hpp"

namespace mfwu {

class unit_test_heap {
public:
    bool use_mfwu_heap();
    bool use_cmp_functor();
private:
    template <typename T, typename Container, typename CmpFunctor>
    void print_heap_vec(const mfwu::heap<T, Container, CmpFunctor>& h);
    template <typename T, typename Container, typename CmpFunctor>
    void print_heap_struct(const mfwu::heap<T, Container, CmpFunctor>& h);
    template <typename T, typename Container, typename CmpFunctor>
    void print_heap_detail(const mfwu::heap<T, Container, CmpFunctor>& h);
};  // endof class unit_test_heap

template <typename T, typename Container, typename CmpFunctor>
void unit_test_heap::print_heap_vec(const mfwu::heap<T, Container, CmpFunctor>& h) {
#ifndef __UNIT_TEST_HEAP_BRIEF__
    std::cout << "arr_: ";
    for (auto&& element : h.arr_) {
        std::cout << element << " ";
    }
    std::cout << "\n";
#endif  // __UNIT_TEST_HEAP_BRIEF__
}
template <typename T, typename Container, typename CmpFunctor>
void unit_test_heap::print_heap_struct(const mfwu::heap<T, Container, CmpFunctor>& h) {
#ifndef __UNIT_TEST_HEAP_BRIEF__
    mfwu::print_seq_tree(h.arr_);
#endif  // __UNIT_TEST_HEAP_BRIEF__
}
template <typename T, typename Container, typename CmpFunctor>
void unit_test_heap::print_heap_detail(const mfwu::heap<T, Container, CmpFunctor>& h) {
#ifndef __UNIT_TEST_HEAP_BRIEF__
    std::cout << "size(): " << h.size() << ", top(): " << h.top() << "\n";
    print_heap_vec(h);
    print_heap_struct(h);
#endif  // __UNIT_TEST_HEAP_BRIEF__
}

bool unit_test_heap::use_mfwu_heap() {
    std::cout << "\n------- Test: use mfwu heap -------\n";
    std::cout << "making heap\n";
    mfwu::heap<int> hp1 = {1, 2, 3, 4, 5, 6, 7, 2,
                           2, 5, 6, 6, 8, 8, 3};
    print_heap_detail(hp1);
    mfwu::heap<int> hp2 = {1, 2, 4, 5, 1};
    print_heap_detail(hp2);
    mfwu::heap<int> hp3 = hp1;
    print_heap_detail(hp3);
    mfwu::heap<int> hp4 = mfwu::move(hp2);
    print_heap_detail(hp4);
    mfwu::heap<int> hp5;
    mfwu::heap<int> hp6;
    hp6 = hp1;
    print_heap_detail(hp6);

    std::cout << "random push/pop test: part 1\n";
    hp5.push(3);
    print_heap_struct(hp5);
    hp5.push(5);
    print_heap_struct(hp5);
    hp5.push(4);
    print_heap_struct(hp5);
    hp5.push(4);
    print_heap_struct(hp5);
    hp5.pop();
    print_heap_struct(hp5);
    hp5.push(8);
    print_heap_struct(hp5);
    hp5.pop();
    print_heap_struct(hp5);
    std::cout << "random push/pop test: part 2\n";
    hp6.push(1);
    print_heap_struct(hp6);
    hp6.push(9);
    print_heap_struct(hp6);
    hp6.push(2); 
    print_heap_struct(hp6);
    hp6.pop();
    print_heap_struct(hp6);
    hp6.pop();
    print_heap_struct(hp6);
    hp6.pop();
    print_heap_struct(hp6);
    hp6.pop();
    print_heap_struct(hp6);
    hp6.push(5);
    print_heap_struct(hp6);

    std::cout << "sorting\n";
    hp5.sort();
    print_heap_detail(hp5);
    hp6.sort();
    print_heap_detail(hp6);

    return 0;
}

template <typename T>
struct greater {
    bool operator()(const T& a, const T& b) {
        return a > b;
    }
};  // endof struct greater

bool unit_test_heap::use_cmp_functor() {
    std::cout << "\n------- Test: use cmp functor -------\n";
    std::cout << "making heap and sorting with default mfwu::less<int>\n";
    mfwu::heap<int> hp1 = {1, 2, 3, 4, 5, 6, 7, 8,
                           1, 3, 5, 7, 2, 4, 6, 8};
    print_heap_detail(hp1);
    hp1.sort();
    print_heap_detail(hp1);
    std::cout << "making heap and sorting with new mfwu::greater<int>\n";
    mfwu::heap<int, mfwu::vector<int>, mfwu::greater<int>> hp2 = 
        {1, 2, 3, 4, 5, 6, 7, 8, 1, 3, 5, 7, 2, 4, 6, 8};
    print_heap_detail(hp2);
    hp2.sort();
    print_heap_detail(hp2);

    return 0;
}

}  // endof namespace mfwu
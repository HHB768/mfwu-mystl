#define __UNIT_TEST_HEAP__

#include "heap.hpp"

namespace mfwu {

class unit_test_heap {
public:
    bool use_mfwu_heap();
private:
    template <typename T, typename Container, typename CmpFunctor>
    void print_heap_vec(const mfwu::heap<T, Container, CmpFunctor>& h);
    template <typename T, typename Container, typename CmpFunctor>
    void print_heap_struct(const mfwu::heap<T, Container, CmpFunctor>& h);
    template <typename T, typename Container, typename CmpFunctor>
    void print_heap_detail(const mfwu::heap<T, Container, CmpFunctor>& h);
    static void print_space(int n) { for (int i=0; i<n; i++) { std::cout << " "; } }
};  // endof class unit_test_heap

template <typename T, typename Container, typename CmpFunctor>
void unit_test_heap::print_heap_vec(const mfwu::heap<T, Container, CmpFunctor>& h) {
    for (auto&& element : h.arr) {
        std::cout << element << " ";
    }
    std::cout << "\n";
}
template <typename T, typename Container, typename CmpFunctor>
void unit_test_heap::print_heap_struct(const mfwu::heap<T, Container, CmpFunctor>& h) {
    int rounded_size = roundup22(h.size());
    int max_width = rounded_size;
    int line_idx = 0;
    int col_num = 1;
    int col_idx = 0;
    for (int i = 0; i < h.size(); i++) {
        print_space(max_width / col_num);
        std::cout << h.arr[i];
        print_space(max_width / col_num - 1);
        col_idx++;
        if (col_idx >= col_num) {
            line_idx++;
            col_num *= 2;
            col_idx = 0;
            std::cout << "\n";
        }
    }
    if (col_idx) { std::cout << "\n"; }
}
template <typename T, typename Container, typename CmpFunctor>
void unit_test_heap::print_heap_detail(const mfwu::heap<T, Container, CmpFunctor>& h) {
    print_heap_vec(h);
    print_heap_struct(h);
}

bool unit_test_heap::use_mfwu_heap() {
    mfwu::heap<int> exheap = {1, 2, 3, 4, 5, 6, 7, 2, 2, 5, 6, 6, 8, 8, 3};
    print_heap_detail(exheap);
    return 0;
}

}  // endof namespace mfwu
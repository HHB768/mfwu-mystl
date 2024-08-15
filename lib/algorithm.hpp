#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include "common.hpp"
#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {

template <typename RandomAccessIterator>
void copy_backward(RandomAccessIterator first, 
                   RandomAccessIterator last,
                   RandomAccessIterator res) {
    --last;
    for (RandomAccessIterator pos = res + (last - first) - 1;
            pos >= res; --last, --pos) {
        *pos = *last;
    }
}

template <typename RandomAccessIterator, typename CmpFunctor>
RandomAccessIterator partition(RandomAccessIterator first,
                               RandomAccessIterator last,
                               CmpFunctor cmp) {
    // typename mfwu::iterator_traits<RandomAccessIterator>::value_type 
    auto pivot = *first;
    RandomAccessIterator left = first, right = last - 1;

    while (left != right) {
        while (left < right && (cmp(pivot, *right) || pivot == *right)) { right--; }
        // wow! we should guarantee right-- when pivot == *right
        // bcz user may pass a cmp like "<" instead of "<="  7.23/24
        *left = *right;
        while (left < right && (cmp(*left,  pivot) || pivot == *left)) { left++; }
        *right = *left;
    }
    *left = pivot;
    return left;
}

template <typename RandomAccessIterator, typename CmpFunctor>
void sort(RandomAccessIterator first, 
          RandomAccessIterator last, 
          CmpFunctor cmp) {
    if (last - first <= 1) return ;

    RandomAccessIterator it = first + rand() % (last - first);
    mfwu::swap(*first, *it);

    it = partition(first, last, cmp);
    sort(first, it, cmp);
    sort(it + 1, last, cmp);
}

template <typename RandomAccessIterator>
void sort(RandomAccessIterator first, 
          RandomAccessIterator last) {
    using CmpFunctor = mfwu::less<
        typename mfwu::iterator_traits<RandomAccessIterator>::value_type>;
    CmpFunctor cmp;
    sort(first, last, cmp);
}

// int partition(mfwu::vector<int>& vec, int start, int finish) {
//     int pivot = vec[start];
//     int left = start;
//     int right = finish - 1;

//     while (left != right) {
//         while (left < right && vec[right] >= pivot) { right--; }
//         vec[left] = vec[right];
//         while (left < right && vec[left] <= pivot) { left++; }
//         vec[right] = vec[left];
//     }
//     vec[left] = pivot;
//     return left;
// }

// void sort(mfwu::vector<int>& vec, int start, int finish) {
//     if (finish - start <= 1) return ;
//     int idx = rand() % (finish - start) + start;
//     mfwu::swap(vec[start], vec[idx]);
//     idx = partition(vec, start, finish);
//     sort(vec, start, idx);
//     sort(vec, idx + 1, finish);
// }

// void sort(mfwu::vector<int>& vec) {
//     sort(vec, 0, vec.size());
// }
template <typename T>
inline T max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <typename T>
inline T min(const T& a, const T& b) {
    return a < b ? a : b;
}

inline void print_space(int n) { 
    for (int i=0; i<n; i++) {
        std::cout << " "; 
    } 
}
template <typename Sequence>
void print_heap_struct(const Sequence& seq) {
    int rounded_size = roundup22(seq.size());
    int max_width = rounded_size;
    int line_idx = 0;
    int col_num = 1;
    int col_idx = 0;

    for (int i = 0; i < seq.size(); i++) {
        print_space(max_width / col_num);
        std::cout << seq[i];
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
template <typename Sequence>
void print_rbtree_heap_struct(const Sequence& seq) {
    int rounded_size = roundup22(seq.size());
    int max_width = rounded_size;
    int line_idx = 0;
    int col_num = 1;
    int col_idx = 0;

    for (int i = 0; i < seq.size(); i++) {
        print_space(max_width / col_num);
        auto node = seq[i];
        if (node == nullptr) {
            std::cout << "\033[0;32m" << "." << "\033[0m";
        } else if (node->color) {
            std::cout << "\033[0;32m" << seq[i]->val << "\033[0m";
        } else {
            std::cout << "\033[0;31m" << seq[i]->val << "\033[0m";
        }
        
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

}  // endof namespace mfwu

#endif  // __ALGORITHM_HPP__
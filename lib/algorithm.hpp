#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {

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

}  // endof namespace mfwu

#endif  // __ALGORITHM_HPP__
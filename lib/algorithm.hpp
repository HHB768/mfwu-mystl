#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include "vector.hpp"

namespace mfwu {

template <typename ForwardIterator, typename CmpFunctor>
void sort(ForwardIterator first, ForwardIterator last, CmpFunctor cmp) {

}

int someFunc(mfwu::vector<int>& vec, int start, int finish) {
    int pivot = vec[start];
    int left = start;
    int right = finish - 1;
    while (true) {
        while (vec[right] > pivot) { right--; }
        if (right == left) return left;
        vec[left] = vec[right];
        left++;
        while (vec[left] < pivot) { left++; }
        if (right == left) return left;
        vec[right] = vec[left];
        right--;
    }
}

void sort(mfwu::vector<int>& vec, int start, int finish) {
    if (finish - start <= 1) return ;
    int idx = start;
    int pivot = vec[idx];
    mfwu::swap(vec[start], vec[idx]);
    idx = someFunc(vec, start, finish);
    sort(vec, start, idx);
    sort(vec, idx + 1, finish);
}

void sort(mfwu::vector<int>& vec) {
    sort(vec, 0, vec.size());
}

}  // endof namespace mfwu

#endif  // __ALGORITHM_HPP__
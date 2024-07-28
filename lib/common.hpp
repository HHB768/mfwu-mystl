#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <algorithm>
#include <new>
#include <type_traits>
#include <initializer_list>
#include <cstring>
#include <vector>
#include <memory>
#include <cassert>

#include <bits/stdc++.h>  // TODO: in test

namespace mfwu {

typedef long ptrdiff_t;

template <typename T, T v>
struct integral_constant {
    static constexpr T value = v;
    using value_type = T;
    using type = integral_constant<T, v>;
    constexpr operator value_type() const noexcept { return value; }
};

using true_type = mfwu::integral_constant<bool, true>;
using false_type = mfwu::integral_constant<bool, false>;

template <typename T>
struct remove_reference { typedef T type; };
template <typename T>
struct remove_reference<T&> { typedef T type; };
template <typename T>
struct remove_reference<T&&> { typedef T type; };

template <typename T>
constexpr T&& forward(typename mfwu::remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

template <typename T>
typename mfwu::remove_reference<T>::type&&
move(T&& t) {
    return static_cast<typename mfwu::remove_reference<T>::type&&>(t);
}

// a tiny fix from
// https://blog.csdn.net/mightbxg/article/details/108382265

// case1: T* can cast to C*
template <template <typename...> class C, typename...Ts>
std::true_type is_base_of_template_impl(const C<Ts...>*); 
// case2: T* cannot cast to C*
template <template <typename...> class C>
std::false_type is_base_of_template_impl(...);

template <template <typename...> class C, typename T>
using is_base_of_template = decltype(is_base_of_template_impl<C>(std::declval<T*>()));
// TODO: fix other containers

// standard solutions :
// c++11:
// template<typename _InputIterator,
//     typename = std::_RequireInputIter<_InputIterator>>
// where:
// template<typename _InIter>
// using _RequireInputIter =
//     __enable_if_t<is_convertible<__iter_category_t<_InIter>,
//                 input_iterator_tag>::value>;
// c++98:
// Check whether it's an integral type.  If so, it's not an iterator.
// typedef typename std::__is_integer<_InputIterator>::__type _Integral;
// _M_initialize_dispatch(__first, __last, _Integral());

template <typename T>
inline void swap(T&& a, T&& b) {
    T temp = mfwu::move(a);
    a = mfwu::move(b);
    b = mfwu::move(temp);
}
template <typename T>
inline void swap(T& a, T& b) {
    swap(mfwu::move(a), mfwu::move(b));
}
template <typename T>
inline void swap(T&& a, T& b) {
    swap(mfwu::move(a), mfwu::move(b));
}
template <typename T>
inline void swap(T& a, T&& b) {
    swap(mfwu::move(a), mfwu::move(b));
}

inline int roundup22(int n) {
    n -= 1;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    return n < 0 ? 1 : n + 1; 
}

template <typename T>
struct less {
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};  // endof struct less

template <typename T>
class double_linked_node;
template <typename T>
using list_node = double_linked_node<T>;

template <typename T>
class bst_node;
template <typename T>
using tree_node = bst_node<T>;

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
void print_seq_tree(Sequence seq) {
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

}  // endof namespace mfwu

#endif  // __COMMON_HPP__
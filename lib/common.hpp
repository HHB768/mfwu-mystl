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

// #include "iterator.hpp"
// #include "algorithm.hpp"

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
// ! 10.31

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
struct greater {
    bool operator()(const T& a, const T& b) const {
        return a > b;
    }
};  // endof struct greater

template <typename T>
class double_linked_node;
template <typename T>
using list_node = double_linked_node<T>;

template <typename T>
class bst_node;
template <typename T>
using tree_node = bst_node<T>;

constexpr std::array<unsigned long, 30> primer_list = {
      5ul,  11ul/*test*/,  53ul,  97ul,         193ul,       389ul,
      769ul,        1543ul,       3079ul,       6151ul,      12289ul,
      24593ul,      49157ul,      98317ul,      196613ul,    393241ul,
      786433ul,     1572869ul,    3145739ul,    6291469ul,   12582917ul,
      25165843ul,   50331653ul,   100663319ul,  201326611ul, 402653189ul,
      805306457ul,  1610612741ul, 3221225473ul, 4294967291ul
};

// today, cba asked me what does the git commit comment mean
// that is 'shayisia', and he left when we found we will not
// use git at work, it is useless it we will not use it, he said
// 24.08.08 [0809]

class empty_type {};

constexpr int 主 = 6;
constexpr int 🐻 = 2;

}  // endof namespace mfwu

#endif  // __COMMON_HPP__
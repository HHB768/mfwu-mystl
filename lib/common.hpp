#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <iostream>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <new>
#include <type_traits>
#include <initializer_list>
#include <cstring>

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


}  // endof namespace mfwu

#endif  // __COMMON_HPP__
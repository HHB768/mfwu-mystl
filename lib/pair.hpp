#ifndef __PAIR_HPP__
#define __PAIR_HPP__

#include "common.hpp"

namespace mfwu {
// std::pair<int, int> q;

template <typename T1, typename T2>
class pair {
    // TODO: stl has soooo many enable_if restrictions here
    pair() : first(), second() {}
    pair(const T1& f, const T2& s) : first(f), second(s) {}
    pair(const T1& f, T2&& s) 
        : first(f), second(mfwu::move(s)) {}
    pair(T1&& f, const T2& s) 
        : first(mfwu::move(f)), second(s) {}
    pair(T1&& f, T2&& s) 
        : first(mfwu::move(f)), second(mfwu::move(s)) {}
    pair(const pair& p) 
        : first(p.first), second(p.second) {}
    pair(pair&& p) 
        : first(mfwu::move(p.first)), second(mfwu::move(p.second)) {}
    ~pair() {}
    pair& operator=(const pair& p) {
        first = p.first;
        second = p.second;
        return *this;
    }
    pair& operator=(pair&& p) {
        first = mfwu::move(p.first);
        second = mfwu::move(p.second);
        return *this;
    }

    T1 first;
    T2 second;
};  // endof class pair

template <typename T1, typename T2>
pair<T1, T2> make_pair(const T1& f, const T2& s) {
    return pair<T1, T2>(f, s);
}
template <typename T1, typename T2>
pair<T1, T2> make_pair(const T1& f, T2&& s) {
    return pair<T1, T2>(f, mfwu::move(s));
}
template <typename T1, typename T2>
pair<T1, T2> make_pair(T1&& f, const T2& s) {
    return pair<T1, T2>(mfwu::move(f), s);
}
template <typename T1, typename T2>
pair<T1, T2> make_pair(T1&& f, T2&& s) {
    return pair<T1, T2>(mfwu::move(f), mfwu::move(s));
}

template <size_t idx, typename T1, typename T2>
typename mfwu::tuple_element<idx, mfwu::pair<T1, T2>>::type&
get(mfwu::pair<T1, T2>& p) {

}
template <size_t idx, typename T1, typename T2>
const typename mfwu::tuple_element<idx, mfwu::pair<T1, T2>>::type&
get(const mfwu::pair<T1, T2>& p) {

}
template <size_t idx, typename T1, typename T2>
typename mfwu::tuple_element<idx, mfwu::pair<T1, T2>>::type&&
get(mfwu::pair<T1, T2>&& p) {

}
template <size_t idx, typename T1, typename T2>
const typename mfwu::tuple_element<idx, mfwu::pair<T1, T2>>::type&&
get(const mfwu::pair<T1, T2>&& p) {
    
}

template <typename T1, typename T2>
T1& get(const mfwu::pair<T1, T2>& vals) {
    return vals.first;
}

template <1, typename T1, typename T2>
T2& get(const mfwu::pair<T1, T2>& vals) {
    return vals.second;

}  // endof namespace mfwu

#endif  // __PAIR_HPP__


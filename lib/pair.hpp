#ifndef __PAIR_HPP__
#define __PAIR_HPP__

#include "common.hpp"

namespace mfwu {
// std::pair<int, int> q;

template <typename T1, typename T2>
struct pair {
    // TODO: stl has soooo many enable_if restrictions here
    pair() : first(), second() {}
    pair(const T1& f, const T2& s) : first(f), second(s) {}
    // pair(const T1& f, T2&& s) 
    //     : first(f), second(mfwu::move(s)) {}
    // pair(T1&& f, const T2& s) 
    //     : first(mfwu::move(f)), second(s) {}
    // you cannot overload like this 10.10/24
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
        // strictly, you should detect this == &p
        // but i mean, user should ensure the rightfulness
        // 24.10.25
        first = mfwu::move(p.first);
        second = mfwu::move(p.second);
        return *this;
    }

    T1 first;
    T2 second;
};  // endof struct pair

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

template <size_t Index, typename T1, typename T2>
struct pair_element;

template <typename T1, typename T2>
struct pair_element<0, T1, T2> {
    using type = T1;

    static T1& get(mfwu::pair<T1, T2>& p) {
        return p.first;
    }
    static const T1& get(const mfwu::pair<T1, T2>& p) {
        return p.first;
    }
    static T1&& get(mfwu::pair<T1, T2>&& p) {
        return mfwu::move(p.first);
    }
    static const T1&& get(const mfwu::pair<T1, T2>&& p) {
        return mfwu::move(p.first);
    }
};  // endof struct pair_element<0>

template <typename T1, typename T2>
struct pair_element<1, T1, T2> {
    using type = T2;

    static T2& get(mfwu::pair<T1, T2>& p) {
        return p.second;
    }
    static const T2& get(const mfwu::pair<T1, T2>& p) {
        return p.second;
    }
    static T2&& get(mfwu::pair<T1, T2>&& p) {
        return mfwu::move(p.second);
    }
    static const T2&& get(const mfwu::pair<T1, T2>&& p) {
        return mfwu::move(p.second);
    }
};  // endof struct pair_element<1>

template <size_t idx, typename T1, typename T2>
inline typename mfwu::pair_element<idx, T1, T2>::type&
get(mfwu::pair<T1, T2>& p) {
    return mfwu::pair_element<idx, T1, T2>::get(p);
}
template <size_t idx, typename T1, typename T2>
inline const typename mfwu::pair_element<idx, T1, T2>::type&
get(const mfwu::pair<T1, T2>& p) {
    return mfwu::pair_element<idx, T1, T2>::get(p);
}
template <size_t idx, typename T1, typename T2>
inline typename mfwu::pair_element<idx, T1, T2>::type&&
get(mfwu::pair<T1, T2>&& p) {
    return mfwu::pair_element<idx, T1, T2>::get(mfwu::move(p));
}
template <size_t idx, typename T1, typename T2>
inline const typename mfwu::pair_element<idx, T1, T2>::type&&
get(const mfwu::pair<T1, T2>&& p) {
    return mfwu::pair_element<idx, T1, T2>::get(mfwu::move(p));
}

template <typename T1, typename T2>
inline T1& get(mfwu::pair<T1, T2>& p) {
    return p.first;
}
template <typename T1, typename T2>
inline const T1& get(const mfwu::pair<T1, T2>& p) {
    return p.first;
}
template <typename T1, typename T2>
inline T1&& get(mfwu::pair<T1, T2>&& p) {
    return mfwu::move(p.first);
}
template <typename T1, typename T2>
inline const T1&& get(const mfwu::pair<T1, T2>&& p) {
    return mfwu::move(p.first);
}
template <typename T1, typename T2>
inline T1& get(mfwu::pair<T2, T1>& p) {
    return p.second;
}
template <typename T1, typename T2>
inline const T1& get(const mfwu::pair<T2, T1>& p) {
    return p.second;;
}
template <typename T1, typename T2>
inline T1&& get(mfwu::pair<T2, T1>&& p) {
    return mfwu::move(p.second);
}
template <typename T1, typename T2>
inline const T1&& get(const mfwu::pair<T2, T1>&& p) {
    return mfwu::move(p.second);
}
}  // endof namespace mfwu

// oops, done!
// hope it function well 10.26/24

#endif  // __PAIR_HPP__


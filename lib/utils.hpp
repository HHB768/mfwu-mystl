#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "common.hpp"

namespace mfwu {

// iterator_traits
struct input_iterator_tag;
struct output_iterator_tag;
struct forward_iterator_tag;
struct bidirectional_iterator_tag;
struct random_access_iterator_tag;

template <typename It>
class iterator_traits {
public:
    using value_type = typename It::value_type;
    using iterator_category = typename It::iterator_category;
    using pointer = typename It::pointer;
    using reference = typename It::reference;
    using difference_type = typename It::difference_type;
};  // endof class iterator_traits

template <typename T>
class iterator_traits<T*> {
public:
    using value_type = T;
    using iterator_category = mfwu::random_access_iterator_tag;  // ei?
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;
};

template <typename T>
class iterator_traits<const T*> {
public:
    using value_type = T;
    using iterator_category = mfwu::random_access_iterator_tag;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;
};

// memory operations 
// commit or rollback
// 1. construct
// 2. destroy
// 3. uninitialized_copy 
// 4. uninitialized_fill
// 5. uninitialized_fill_n;

// 1 construct
template <typename T1, typename... Args>
inline void construct(T1* p, Args&&... args) {
    new (p) T1{mfwu::forward<Args>(args)...};  // TODO: std::forward
}

/*
// 我是傻逼的珍贵记录，我就是为了在这片未初始化的空间上构造对象
// 那 *first 是在做什么，它并没有指向对象，又谈何赋值呢？
// 我真的太幽默了  X-Q3(psd) 240612/23:19

template <typename ForwardIterator, typename T, typename Ty>
inline void construct_aux(ForwardIterator first, ForwardIterator last, const T& value, const Ty&) {
    for ( ; first != last; first++) {
        new(&*first) Ty(value);
    }
}

template <typename ForwardIterator, typename T>
inline void construct_aux(ForwardIterator first, ForwardIterator last, const T& value, const T&) {
    for ( ; first != last; first++) {
        *first = value;  // check: absolutely wrong!
    }
}

template <typename ForwardIterator, typename T>
inline void construct(ForwardIterator first, ForwardIterator last, const T& value) {
    using Ty = typename mfwu::iterator_traits<ForwardIterator>::value_type;
    construct_aux(first, last, value, Ty{});
}
*/

// 1 construct from-to
template <typename ForwardIterator, typename... Args>
inline void construct(ForwardIterator first, ForwardIterator last, Args&&... args) {
    using Ty = typename mfwu::iterator_traits<ForwardIterator>::value_type;
    for (; first != last; first++) {
        new(&*first) Ty{mfwu::forward<Args>(args)...};
    }
}

// 2 destroy *p
template <typename T>
inline void destroy(T* pointer) {
    pointer->~T();  // destroy v1: destroy(p)
}

template <typename ForwardIterator>
inline void destroy_aux(ForwardIterator, ForwardIterator, std::true_type) {}

template <typename ForwardIterator>
inline void destroy_aux(ForwardIterator first, ForwardIterator last, std::false_type) {
    for (; first != last; ++first) {
        mfwu::destroy(&*first);
    }
}

// 2 destroy from-to
template <typename ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    mfwu::destroy_aux(first, last, std::is_trivially_destructible<
        typename mfwu::iterator_traits<ForwardIterator>::value_type>{});
}

// TODO: 怎么没有把这几个用上，肯定哪里出了问题
// 3 uninitialized_copy
template <typename InputIteraor, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_aux(
    InputIteraor first, InputIteraor last, ForwardIterator res, mfwu::true_type) {
    return std::copy(first, last, res);
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_aux(
    InputIterator first, InputIterator last, ForwardIterator res, mfwu::false_type) {
    for (; first != last; ++first, ++res) {
        mfwu::construct(&*res, *first);
    }
    return res;
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator res) {
    return uninitialized_copy_aux(first, last, res,
        std::is_pod<typename mfwu::iterator_traits<ForwardIterator>::value_type>{});
}

inline char* uninitialized_copy(const char* first, const char* last, char* res) {
    memmove(res, first, last - first);
    return res + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* res) {
    memmove(res, first, sizeof(wchar_t) * (last - first));
    return res + (last - first);
}

// 4 uninitialized_fill
template <typename ForwardIterator, typename T>
inline void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, mfwu::true_type) {
    std::fill(first, last, x);
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, mfwu::false_type) {
    for (ForwardIterator cur = first; cur != last; ++cur) {
        mfwu::construct(&*cur, x);
    }
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x) {
    uninitialized_fill_aux(first, last, x, 
        std::is_pod<typename mfwu::iterator_traits<ForwardIterator>::value_type>{});
}

// 5 fill_n
template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_aux(ForwardIterator first, Size n, const T& x, mfwu::true_type) {
    return std::fill_n(first, n, x);
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_aux(ForwardIterator first, Size n, const T& x, mfwu::false_type) {
    for (; n > 0; n--, ++first) {
        mfwu::construct(&*first, x);
    }
    return first;
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const T& x) {
    return uninitialized_fill_n_aux(first, n, x, 
        std::is_pod<typename mfwu::iterator_traits<ForwardIterator>::value_type>{});
}

// iterator operations
// 1. advance
// 2. distance
template <typename It>
void advance_aux(It& it, typename mfwu::iterator_traits<It>::difference_type n,
    mfwu::input_iterator_tag) {
    while (n > 0) {
        ++it;
        --n;
    }
}

template <typename It>
void advance_aux(It& it, typename mfwu::iterator_traits<It>::difference_type n,
    mfwu::bidirectional_iterator_tag) {
        while (n > 0) {
            ++it;
            --n;
        }
        while (n < 0) {
            --it;
            ++n;
        }
    }

template <typename It>
void advance_aux(It & it, typename mfwu::iterator_traits<It>::difference_type n,
    mfwu::random_access_iterator_tag) {
    it += n;
}

template <typename It, typename Distance>
void advance(It& it, Distance n) {
    advance_aux(it,
        typename mfwu::iterator_traits<It>::difference_type(n),
        typename mfwu::iterator_traits<It>::iterator_category());
}


template <typename It>
typename mfwu::iterator_traits<It>::difference_type
    distance(It first, It last, mfwu::random_access_iterator_tag) {
    return last - first;
}

template <typename It>
typename mfwu::iterator_traits<It>::difference_type
    distance(It first, It last, mfwu::input_iterator_tag) {
    typename mfwu::iterator_traits<It>::difference_type res{0};
    while (first != last) {
        res++;
        first++;
    }
    return res;
}

template <typename It>
typename mfwu::iterator_traits<It>::difference_type
    distance(It first, It last) {
    return distance_aux(first, last, typename mfwu::iterator_traits<It>::iterator_category{});
}

}  // endof namesapce mfwu

#endif  // __UTILS_HPP__

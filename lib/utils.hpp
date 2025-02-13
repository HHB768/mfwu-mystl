#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include "iterator.hpp"

namespace mfwu {

// iterator_traits
struct input_iterator_tag;
struct output_iterator_tag;
struct forward_iterator_tag;
struct bidirectional_iterator_tag;
struct random_access_iterator_tag;

// void_t definition
template <typename...>
using void_t = void;

template<typename _Iterator, typename = mfwu::void_t<>>
struct __iterator_traits { };

template<typename _Iterator>
struct __iterator_traits<_Iterator,
                mfwu::void_t<typename _Iterator::iterator_category,
                    typename _Iterator::value_type,
                    typename _Iterator::difference_type,
                    typename _Iterator::pointer,
                    typename _Iterator::reference>>
{
    typedef typename _Iterator::iterator_category iterator_category;
    typedef typename _Iterator::value_type        value_type;
    typedef typename _Iterator::difference_type   difference_type;
    typedef typename _Iterator::pointer           pointer;
    typedef typename _Iterator::reference         reference;
};

template<typename _Iterator>
struct iterator_traits
: public __iterator_traits<_Iterator> {};

// in <type_traits>:
/* 
    #if __cplusplus >= 201103L
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    ////////////////////////////////////////////////////////////////////////
    // 2408. SFINAE-friendly common_type/iterator_traits is missing in C++14 
    (! 24.10.31, so we need to add this __iterator_traits)
    ////////////////////////////////////////////////////////////////////////
    template<typename _Iterator, typename = __void_t<>>
        struct __iterator_traits { };

    #if ! __cpp_lib_concepts

    template<typename _Iterator>
        struct __iterator_traits<_Iterator,
                    __void_t<typename _Iterator::iterator_category,
                        typename _Iterator::value_type,
                        typename _Iterator::difference_type,
                        typename _Iterator::pointer,
                        typename _Iterator::reference>>
        {
        typedef typename _Iterator::iterator_category iterator_category;
        typedef typename _Iterator::value_type        value_type;
        typedef typename _Iterator::difference_type   difference_type;
        typedef typename _Iterator::pointer           pointer;
        typedef typename _Iterator::reference         reference;
        };
    #endif // ! concepts

    template<typename _Iterator>
        struct iterator_traits
        : public __iterator_traits<_Iterator> { };

    #else // ! C++11
    template<typename _Iterator>
        struct iterator_traits
        {
        typedef typename _Iterator::iterator_category iterator_category;
        typedef typename _Iterator::value_type        value_type;
        typedef typename _Iterator::difference_type   difference_type;
        typedef typename _Iterator::pointer           pointer;
        typedef typename _Iterator::reference         reference;
        };
    #endif // C++11
*/
// instead of directly use:
// template <typename It>
// class iterator_traits {
// public:
//     using value_type = typename It::value_type;
//     using iterator_category = typename It::iterator_category;
//     using pointer = typename It::pointer;
//     using reference = typename It::reference;
//     using difference_type = typename It::difference_type;
// };  // endof class iterator_traits

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
// X-H-Q2 24.07.15 [0809] 

// maybe i can try this: 240924
// template <typename T>
// using is_input_iterator 
//     = is_base_of_template<mfwu::input_iterator_tag,
//                           mfwu::iterator_traits<T>::iterator_category>;
// no you cannot 24.10.19
// GPTANS, try this:

// Primary template - defaults to false
template <typename Base, typename Derived, typename = void>
struct is_base_of : std::false_type {};

// Specialization for valid Base-Derived relationship
/*
    When you see something like is_base_of<Base, Derived, void_t<...>>,
    it checks if the expressions inside void_t are valid. If they are, 
    void_t<...> results in void, and the template specialization is enabled.
    If not, it fails gracefully without a hard error, due to SFINAE rules.
*/ 
template <typename Base, typename Derived>
struct is_base_of<Base, Derived, 
    mfwu::void_t<decltype(static_cast<Base*>(std::declval<Derived*>()))>>
    : std::true_type {};

// Helper variable template
template <typename Base, typename Derived>
constexpr bool is_base_of_v = is_base_of<Base, Derived>::value;

template<typename, typename = std::void_t<>>
struct is_input_iterator : std::false_type {};

template<typename T>
struct is_input_iterator<T, mfwu::void_t<typename mfwu::iterator_traits<T>::iterator_category>>
    :  mfwu::is_base_of<mfwu::input_iterator_tag, typename mfwu::iterator_traits<T>::iterator_category> {};

template<typename T>
constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

// many other trials here 10.29
// template <typename T>
// using is_input_iterator = 
//     mfwu::is_base_of<mfwu::input_iterator_tag,
//                      typename mfwu::iterator_traits<T>::iterator_category>;

// template <typename T, typename = void>
// struct is_input_iterator : std::false_type {};

// template <typename T>
// struct is_input_iterator<T, mfwu::void_t<typename mfwu::iterator_traits<T>::iterator_category>>
//     : std::true_type {};

// template <typename T> struct is_input_iterator { 
//     using iterator_category = 
//         typename mfwu::iterator_traits<T>::iterator_category; 
//     static const bool value = 
//         std::is_base_of<mfwu::input_iterator_tag, iterator_category>::value; 
// };

// template <typename T, typename = void>
// struct is_input_iterator : std::false_type {};

// template <typename T>
// struct is_input_iterator<T, std::void_t<typename mfwu::iterator_traits<T>::iterator_category>> 
//     : std::is_base_of<mfwu::input_iterator_tag, typename std::iterator_traits<T>::iterator_category> {};


//////////////////////////

// memory operations 
// commit or rollback
// 1. construct
// 2. destroy
// 3. uninitialized_copy 
// 4. uninitialized_fill
// 5. uninitialized_fill_n;
/*
frankly speaking, there are at least two points i havent done well:
1. the commit or rollback semeanteme is not implemented, we can use
    try { 
        // what we have done  
    } catch (...) { 
        for ( ; dst_first != current; ++dst_first) {
            dst_first->~T(); 
        }
    }
2. we use ForwardIterator everywhere however, we can only use
   InputIterator and OutputIterator just like what we have done in
   mfwu::copy(...)

TODO: X-H2-Z 24.06.21
*/

// 1 construct
template <typename T1, typename... Args>
inline void construct(T1* p, Args&&... args) {
    new (p) T1{mfwu::forward<Args>(args)...};
}

/*
// 我是傻逼的珍贵记录，我就是为了在这片未初始化的空间上构造对象
// 那 *first 是在做什么，它并没有指向对象，又谈何赋值呢？
// 我真的太幽默了  X-Q3(psd) 240612/23:19
// 但是后面 fill 或者 copy 的时候，是有可能不用 construct 的
// *first 就意味着按照 standard layout 直接填充空间 6.20/24

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
// warning: you wont wanna pass a Ty&& as Args
//          bcz the first iter will steal the content and
//          the rest iter will place this ub object
// 24.10.11 X 
template <typename ForwardIterator, typename... Args>
inline void construct(ForwardIterator first, ForwardIterator last, Args&&... args) {
    using Ty = typename mfwu::iterator_traits<ForwardIterator>::value_type;
    for (; first != last; first++) {
        std::cout << "1\n";
        new(&*first) Ty{mfwu::forward<Args>(args)...};
    }
}

// // 1 construct but try not to construct
// deprecated bcz vector has its own reverse copy
// template <typename T>
// inline void construct_or_not_aux(T* p, const T& val, std::true_type) {
//     *p = val;
// }

// template <typename T>
// inline void construct_or_not_aux(T* p, const T& val, std::false_type) {
//     construct(p, val);
// }

// template <typename T>
// inline void construct_or_not(T* p, const T& val) {
//     construct_or_not_aux(p, val, std::is_pod<T>{});
// }

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

// 3 uninitialized_copy
template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy_aux(InputIterator first, InputIterator last,
                               OutputIterator res, mfwu::random_access_iterator_tag) {
    for ( ; first < last; ++first, ++res) {
        *res = *first;
    }
    return res;
}  
// what is the meaning of this implementation?  241220
// oh, i found it after i comment it...  241220

template <typename InputIterator, typename OutputIterator, typename other_iterator_tag>
inline OutputIterator copy_aux(InputIterator first, InputIterator last,
                               OutputIterator res, other_iterator_tag) {
    for ( ; first != last; ++first, ++res) {
        *res = *first;
    }
    return res;
}

template <typename InputIterator, typename OutputIterator>
inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator res);

template <typename InputIterator, typename Size, typename OutputIterator>
inline OutputIterator copy_n(InputIterator first, Size n, OutputIterator res);

template <typename InputIteraor, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_aux(
    InputIteraor first, InputIteraor last, ForwardIterator res, std::true_type) {
    return mfwu::copy(first, last, res);  
    // cmp to non-pod, it can use *res = *first, instead of construct(res, *first)
}

template <typename InputIterator, typename ForwardIterator>
inline ForwardIterator uninitialized_copy_aux(
    InputIterator first, InputIterator last, ForwardIterator res, std::false_type) {
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

// from cppreference 24.12.20
template<class InputIt, class Size, class NoThrowForwardIt>
inline NoThrowForwardIt uninitialized_copy_n(InputIt first, Size count, NoThrowForwardIt d_first)
{
    using T = typename std::iterator_traits<NoThrowForwardIt>::value_type;
    NoThrowForwardIt current = d_first;
    try
    {
        for (; count > 0; ++first, (void) ++current, --count)
            ::new (static_cast<void*>(std::addressof(*current))) T(*first);
    }
    catch (...)
    {
        for (; d_first != current; ++d_first)
            d_first->~T();
        throw;
    }
    return current;
}

// 4 uninitialized_fill
template <typename ForwardIterator, 
          typename T=typename mfwu::iterator_traits<ForwardIterator>::value_type>  // how about this ?
inline void fill(ForwardIterator first, ForwardIterator last , const T& value);

template <typename ForwardIterator, typename T>
inline void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, std::true_type) {
#ifdef __UNIT_TEST_UTILS__
#ifndef __UNIT_TEST_UTILS_BRIEF__
    std::cout << "calling std::fill (or mfwu::fill)\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__
#endif  // __UNIT_TEST_UTILS__
    mfwu::fill(first, last, x);
}

template <typename ForwardIterator, typename T>
inline void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const T& x, std::false_type) {
#ifdef __UNIT_TEST_UTILS__
#ifndef __UNIT_TEST_UTILS_BRIEF__
    std::cout << "calling construct in loop\n";
#endif  // __UNIT_TEST_UTILS_BRIEF__
#endif  // __UNIT_TEST_UTILS__
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
template <typename ForwardIterator, typename Size,
          typename T=typename mfwu::iterator_traits<ForwardIterator>::value_type>
inline ForwardIterator fill_n(ForwardIterator first, Size count, const T& value);

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, std::true_type) {
    return mfwu::fill_n(first, n, x);
}

template <typename ForwardIterator, typename Size, typename T>
inline ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, std::false_type) {
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
inline void advance_aux(It& it, typename mfwu::iterator_traits<It>::difference_type n,
    mfwu::input_iterator_tag) {
    while (n > 0) {
        ++it;
        --n;
    }
}

template <typename It>
inline void advance_aux(It& it, typename mfwu::iterator_traits<It>::difference_type n,
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
inline void advance_aux(It & it, typename mfwu::iterator_traits<It>::difference_type n,
    mfwu::random_access_iterator_tag) {
    it += n;
}

template <typename It, typename Distance>
inline void advance(It& it, Distance n) {
    advance_aux(it,
        typename mfwu::iterator_traits<It>::difference_type(n),
        typename mfwu::iterator_traits<It>::iterator_category());
}


template <typename It>
inline typename mfwu::iterator_traits<It>::difference_type
distance_aux(It first, It last, mfwu::random_access_iterator_tag) {
    return last - first;
}

template <typename It>
inline typename mfwu::iterator_traits<It>::difference_type
distance_aux(It first, It last, mfwu::input_iterator_tag) {
    typename mfwu::iterator_traits<It>::difference_type res{0};
    while (first != last) {
        res++;
        first++;
    }
    return res;
}

template <typename It>
inline typename mfwu::iterator_traits<It>::difference_type
distance(It first, It last) {
    return distance_aux(first, last, typename mfwu::iterator_traits<It>::iterator_category{});
}

}  // endof namesapce mfwu

#endif  // __UTILS_HPP__

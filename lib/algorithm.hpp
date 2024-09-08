#ifndef __ALGORITHM_HPP__
#define __ALGORITHM_HPP__

#include "common.hpp"
#include "iterator.hpp"
#include "utils.hpp"
#include "pair.hpp"

namespace mfwu {

template <typename RandomAccessIterator>
inline void copy_backward(RandomAccessIterator first, 
                          RandomAccessIterator last,
                          RandomAccessIterator res) {
    --last;
    for (RandomAccessIterator pos = res + (last - first);
            pos >= res; --last, --pos) {
        *pos = *last;
    }
}

template <typename RandomAccessIterator, typename CmpFunctor>
inline RandomAccessIterator partition(RandomAccessIterator first,
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
inline void sort(RandomAccessIterator first, 
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
inline void sort(RandomAccessIterator first, 
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
inline const T& max(const T& a, const T& b) {
    return a < b ? b : a;
}

template <typename T>
inline const T& min(const T& a, const T& b) {
    return a < b ? a : b;
}

inline void print_space(int n) { 
    for (int i=0; i<n; i++) {
        std::cout << " "; 
    } 
}
template <typename Sequence>
inline void print_heap_struct(const Sequence& seq) {
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
inline void print_rbtree_heap_struct(const Sequence& seq) {
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


template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,  
          typename Compare>
inline ForwardIt lower_bound(ForwardIt first, ForwardIt last,
                             const T& val, Compare comp) {
    // using value_type = 
    //     typename mfwu::iterator_traits<ForwardIt>::value_type;
    using difference_type = 
        typename mfwu::iterator_traits<ForwardIt>::difference_type;
    ForwardIt it;
    difference_type count, step;
    count = mfwu::distance(first, last);

    while (count > 0) {
        it = first;
        step = count / 2;
        mfwu::advance(it, step);

        if (comp(*it, value)) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& val) {
    return mfwu::lower_bound(first, last, val, mfwu::less<T>{});
}

template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,  
          typename Compare>
inline ForwardIt upper_bound(ForwardIt first, ForwardIt last,
                             const T& val, Compare comp) {
    // using value_type = 
    //     typename mfwu::iterator_traits<ForwardIt>::value_type;
    using difference_type = 
        typename mfwu::iterator_traits<ForwardIt>::difference_type;
    ForwardIt it;
    difference_type count, step;
    count = mfwu::distance(first, last);

    while (count > 0) {
        it = first;
        step = count / 2;
        mfwu::advance(it, step);

        if (!comp(value, *it)) {
            first = ++it;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& val) {
    return mfwu::upper_bound(first, last, val, mfwu::less<T>{});
}

inline size_t get_next_primer(size_t size) {
    auto it = mfwu::lower_bound(primer_list.begin(), primer_list.end(), size);
}


// other stl algos
/**
 * 01. fill fill_n generate generate_n
 * 02. for_each transform
 * 03. max min max_element min_element
 * 04. sort partition stable_sort stable_partition 
 *     partial_sort partial sort_copy nth_element
 * 05. reverse reverse_copy rotate rotate_copy
 * 06. random_shuffle
 * 07. count count_if find find_if find_first_of find_end adjacent_find
 * 08. search search_n binary_search
 * 09. lower_bound upper_bound equal_range
 * 10. copy copy_backward remove remove_if remove_copy remove_copy_if
 *     replace replace_copy replace_if replace_copy_if
 * 11. unique unique_copy
 * 12. swap swap_range iter_swap
 * 13. accumulate partial_sum adjacent_difference inner_product
 * 14. equal includes lexicographical_compare mismatch
 * 15. merge inplace_merge set_union set_intersection 
 *     set_difference set_symmetric_difference
 * 16. next_permutation prev_permutation
 * 17. make_heap push_heap pop_heap sort_heap
 */
// TODO: check books

// 01. fill fill_n generate generate_n
// defined in utils
template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type> 
void fill(ForwardIt first, ForwardIt last , const T& value);

template <typename ForwardIt, typename Size,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
ForwardIt fill_n(ForwardIt first, Size count, const T& value);

template <typename ForwardIt, typename Generator>
inline void generate(ForwardIt first, ForwardIt last, Generator g) {
    for ( ; first != last; ++first) {
        *first = g();
    }
}

template <typename ForwardIt, typename Size, typename Generator>
inline ForwardIt generate(ForwardIt first, Size n, Generator g) {
    for (Size i = 0; i < n; ++i, ++first) {
        *first = g();
    }
    return first;
}


// 02. for_each transform
template <typename InputIt, typename UnaryFunc>
inline UnaryFunc for_each(InputIt first, InputIt last, UnaryFunc f) {
    for ( ; first != last; ++first) {
        f(*first);  // UB: if UnaryFunc is not MoveConstructible
    }
    return f;
}

template <typename InputIt, typename OutputIt, typename UnaryOp>
inline OutputIt transform(InputIt first, InputIt last,
                          OutputIt res, UnaryOp unary_op) {
    for ( ; first != last; ++res, ++first) {
        *res = unary_op(*first);
    }
    return res;
}

// 03. max min max_element min_element
template <typename ForwardIt>
inline ForwardIt max_element(ForwardIt first,
                             ForwardIt last) {
    if (first == last) { return last; }
    ForwardIt largest = first;
    while (++first != last) {
        if (*largest < *first) {
            largest = first;
        }
    }
    return largest;
}

template <typename ForwardIt, typename Compare>
inline ForwardIt max_element(ForwardIt first,
                             ForwardIt last,
                             Compare cmp) {
    if (first == last) { return last; }
    ForwardIt largest = first;
    while (++first != last) {
        if (cmp(*largest, *first)) {
            largest = first;
        }
    }
    return largest;
}

template <typename T>
inline const T& max(const T& a, const T& b);

template <typename T, typename Compare>
inline const T& max(const T& a, const T& b, Compare cmp) {
    return (cmp(a, b)) ? b : a;
}

template <typename T>
inline T max(const std::initializer_list<T>& vals) {
    return *mfwu::max_element(vals.begin(), vals.end());
}

template <typename T, typename Compare>
inline T max(const std::initializer_list<T>& vals, Compare cmp) {
    return *mfwu::max_element(vals.begin(), vals.end(), cmp);
}

template <typename ForwardIt>
inline ForwardIt min_element(ForwardIt first,
                             ForwardIt last) {
    if (first == last) { return last; }
    ForwardIt largest = first;  // largest ? :D  090124
    while (++first != last) {
        if (*largest > *first) {
            largest = first;
        }
    }
    return largest;
}

template <typename ForwardIt, typename Compare>
inline ForwardIt min_element(ForwardIt first,
                             ForwardIt last,
                             Compare cmp) {
    if (first == last) { return last; }
    ForwardIt largest = first;
    while (++first != last) {
        if (cmp(*first, *largest)) {
            largest = first;
        }
    }
    return largest;
}

template <typename T>
inline const T& min(const T& a, const T& b);

template <typename T, typename Compare>
inline const T& min(const T& a, const T& b, Compare cmp) {
    return (cmp(a, b)) ? a : b;
}

template <typename T>
inline T min(const std::initializer_list<T>& vals) {
    return *mfwu::min_element(vals.begin(), vals.end());
}

template <typename T, typename Compare>
inline T min(const std::initializer_list<T>& vals, Compare cmp) {
    return *mfwu::min_element(vals.begin(), vals.end(), cmp);
}

// 04. sort partition stable_sort stable_partition 
//     partial_sort partial sort_copy nth_element

//     gcc libstdc++: https://github.com/gcc-mirror/gcc/
//                     blob/d9375e490072d1aae73a93949aa158fcd2a27018/
//                     libstdc%2B%2B-v3/include/bits/stl_algo.h#L1950
//     llvm libc++:   https://github.com/llvm/llvm-project/
//                     blob/e7fc254875ca9e82b899d5354fae9b5b779ff485/
//                     libcxx/include/__algorithm/sort.h#L264

// sorting speed ref:
// https://www.cnblogs.com/stormli/p/sort.html
// https://cloud.tencent.com/developer/ask/sof/102569819
// https://killtimer0.github.io/2021/09/20/test-sort/

template <typename RandomAccessIterator, typename CmpFunctor>
RandomAccessIterator partition(RandomAccessIterator first,
                               RandomAccessIterator last,
                               CmpFunctor cmp);

template <typename RandomAccessIterator, typename CmpFunctor>
void sort(RandomAccessIterator first, 
          RandomAccessIterator last, 
          CmpFunctor cmp);
// the order of equal elements is not guaranteed to be preserved
template <typename RandomAccessIterator>
void sort(RandomAccessIterator first, 
          RandomAccessIterator last);

// TODO: insert_sort merge_sort (stable_sort) 
// std::sort partial_sort (heap_sort) etc.
// check books

// 05. reverse reverse_copy rotate rotate_copy
template <typename Iterator1, typename Iterator2>
inline void iter_swap(Iterator1 it1, Iterator2 it2) {
    mfwu::swap(*it1, *it2);
}

template <typename BidirectionalIterator>
inline void reverse(BidirectionalIterator first, BidirectionalIterator last) {
    using iter_cat = typename mfwu::iterator_traits<
        BidirectionalIterator>::iterator_category;
    if (std::is_base_of_v<mfwu::random_access_iterator_tag, iter_cat>) {
        if (first == last) { return ; }
        for (--last; first < last; (void)++first, --last) {
            mfwu::iter_swap(first, last);
        }
    } else {
        while (first != last && first != --last) {
            mfwu::iter_swap(first++, last);
        }
    }
}

template <typename BidirectionalIterator, typename OutputIt>
inline OutputIt reverse_copy(BidirectionalIterator first,
                             BidirectionalIterator last,
                             OutputIt res) {
    for (; first != last; ++res) {
        *res = *(--last);
    }
    return res;
}

template <typename ForwardIt>
inline ForwardIt rotate(ForwardIt first,
                        ForwardIt middle,
                        ForwardIt last) {
    if (first == middle) return last;
    if (middle == last) return first;

    ForwardIt write = first;
    ForwardIt next_read = first;
    for (ForwardIt read = middle; read != last; ++write, ++read) {
        if (write == next_read) {
            next_read = read;  // track the "first"
            // TODO: thick if we can break here
        }
        mfwu::iter_swap(write, read);
    }
    rotate(write, next_read, last);  // remaining part
    return write;
}

template <typename ForwardIt, typename OutputIt>
inline OutputIt rotate_copy(ForwardIt first, ForwardIt middle,
                            ForwardIt last, OutputIt res) {
    res = mfwu::copy(middle, last, res);
    return mfwu::copy(first, middle, res);
}

// 06. random_shuffle
template <typename RandomAccessIterator>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last) {
    for (typename mfwu::iterator_traits<RandomAccessIterator>::difference_type
         i = last - first - 1; i > 0; --i) {
        mfwu::swap(first[i], first[std::rand() % (i + 1)]);
        // TODO: use std::uniform_int_distribution
    }
}

// RandomFunc may be complex, so it is passed by reference
template <typename RandomAccessIterator, typename RandomFunc>
inline void random_shuffle(RandomAccessIterator first, RandomAccessIterator last,
                           RandomFunc&& r) {
    for (typename mfwu::iterator_traits<RandomAccessIterator>::difference_type
         i = last - first - 1; i > 0; --i) {
        mfwu::swap(first[i], first[r(i + 1)]);
        // TODO: use std::uniform_int_distribution
    }
}

template <typename RandomAccessIterator, typename UniformRandomBitGenerator>
inline void shuffle(RandomAccessIterator first, RandomAccessIterator last,
                    UniformRandomBitGenerator&& g) {
    using diff_t = typename mfwu::iterator_traits<
                   RandomAccessIterator>::difference_type;
    using distr_t = std::uniform_int_distribution<diff_t>;
    using param_t = typename distr_t::param_type;

    distr_t D;
    for (diff_t i = last - first - 1; i > 0; --i) {
        mfwu::swap(first[i], first[D(g, param_t(0, i))]);
    }
}

// 07. count count_if find find_if find_first_of find_end adjacent_find
template <typename InputIt, 
          typename T = typename mfwu::iterator_traits<InputIt>::value_type>
inline typename mfwu::iterator_traits<InputIt>::difference_type
count(InputIt first, InputIt last, const T& val) {
    typename mfwu::iterator_traits<InputIt>::difference_type ret = 0;
    for ( ; first != last; ++first) {
        ret += (*first == val);
    }
    return ret;
}

// Copilot 24.09.08
// UnaryPred is usually small, so straightforward passing is recommended
template <typename InputIt, typename UnaryPred>
inline typename mfwu::iterator_traits<InputIt>::difference_type
count_if(InputIt first, InputIt last, UnaryPred p) {
    typename mfwu::iterator_traits<InputIt>::difference_type ret = 0;
    for ( ; first != last; ++first) {
        ret += (!!p(*first));
    }
    return ret;
}

template <typename InputIt, 
          typename T = typename mfwu::iterator_traits<InputIt>::value_type>
inline InputIt find(InputIt first, InputIt last, const T& val) {
    for (; first != last; ++first) {
        if (*first == val) return first;
    }
    return last;
}

template <typename InputIt, typename UnaryPred>
inline InputIt find_if(InputIt first, InputIt last, UnaryPred p) {
    for ( ; first != last; ++first) {
        if (p(*first)) return first;
    }
    return last;
}

template <typename InputIt, typename UnaryPred>
inline InputIt find_if_not(InputIt first, 
                           InputIt last, UnaryPred p) {
    for ( ; first != last; ++first) {
        if (!p(*first)) return first;
    }
    return last;

    // < C++11
    // use thhis:
    // return std::find_if(first, last, std::not1(q));
    // not1(q) returns !p(x) (logical complement of pred(x)) 
}

template <typename ForwardIt1, typename ForwardIt2>
inline ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last) {
    while (true) {
        ForwardIt1 it = first;
        for (ForwardIt2 s_it = s_first;  ; ++it, ++s_it) {
            if (s_it == s_last) return first;
            if (it == last) return last;
            if (!(*it == *s_it)) break;
        }
        ++first;
    }
}  // TODO: why do so ?

template <typename ForwardIt1, typename ForwardIt2,
          typename BinaryPred>
inline ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last,
                         BinaryPred p) {
    while (true) {
        ForwardIt1 it = first;
        for (ForwardIt2 s_it = s_first;  ; ++it, ++s_it) {
            if (s_it == s_last) return first;
            if (it == last) return last;
            if (!p(*it,*s_it)) break;
        }
        ++first;
    }
}

template <typename ForwardIt1, typename ForwardIt2>
inline ForwardIt1 find_end(ForwardIt1 first, ForwardIt1 last,
                           ForwardIt2 s_first, ForwardIt2 s_last) {
    if (s_first == s_last) return last;
    ForwardIt1 res = last;
    while (true) {
        ForwardIt1 new_res = mfwu::search(first, last, s_first, s_last);
        if (new_res == last) break;
        else {
            res = new_res;
            first = res;
            ++first;
        }
    }
    return res;
}

template <typename ForwardIt>
inline ForwardIt adjacent_find(ForwardIt first, ForwardIt last) {
    if (first == last) return last;
    ForwardIt next = first; ++next;
    for (; next != last; ++next, ++first) {
        if (*first == *next) { return first; }
    }
    return last;
}

template <typename ForwardIt, typename BinaryPred>
inline ForwardIt adjacent_find(ForwardIt first, ForwardIt last,
                               BinaryPred p) {
    if (first == last) return last;
    ForwardIt next = first; ++next;
    for (; next != last; ++next, ++first) {
        if (p(*first, *next)) { return first; }
    }
    return last;
}

// CPP x 24.09.07
// BinaryPredicate is a set of requirements expected by some of the standard
// library facilities from the user-provided arguments.
// Given a BinaryPredicate bin_pred and a pair of iterators iter1 and iter2
// or an iterator iter and a (possibly const)(since C++20) value value, 
// the expression bin_pred(*iter1, *iter2) or, respectively,
// bin_pred(*iter, value), must be contextually convertible to bool.
// In addition, evaluation of that expression is not allowed to call
// non-const member functions of the dereferenced iterators ; syntactically,
// the predicate must accept const object arguments, with the same behavior
// regardless of whether its arguments are const or non-const(since C++20).

// 08. search search_n binary_search
template <typename ForwardIt1, typename ForwardIt2>
inline ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last);

template <typename ForwardIt1, typename ForwardIt2,
          typename BinaryPred>
inline ForwardIt1 search(ForwardIt1 first, ForwardIt1 last,
                         ForwardIt2 s_first, ForwardIt2 s_last,
                         BinaryPred p);

template <typename ForwardIt, typename Size, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline ForwardIt search_n(ForwardIt first, ForwardIt last,
                          Size count, const T& val) {
    if (count <= 0) return first;
    for ( ; first != last; ++first) {
        if (!(*first == val)) continue;
        ForwardIt candidate = first;
        for (Size cur_count = 1; true; ++cur_count) {
            if (cur_count >= count) return candidate;
            ++first;
            if (first == last) return last;
            if (!(*first == val)) break;
        }
    }
    return last;
}

template <typename ForwardIt, typename Size, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,
          typename BinaryPred>
inline ForwardIt search_n(ForwardIt first, ForwardIt last,
                          Size count, const T& val, BinaryPred p) {
    if (count <= 0) return first;
    for ( ; first != last; ++first) {
        if (!(p(*first, val))) continue;
        ForwardIt candidate = first;
        for (Size cur_count = 1; true; ++cur_count) {
            if (cur_count >+ count) retunr candidate;
            ++first;
            if (first == last) return last;
            if (!p(*first, val)) break;
        }
    }
    return last;
}

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,
          typename Compare>
inline bool binary_search(ForwardIt first, ForwardIt last,
                          const T& val, Compare comp) {
    first = mfwu::lower_bound(first, last, val, comp);
    return (!(first == last) and !(comp(value, *first)));
}

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline bool binary_search(ForwardIt first, ForwardIt last, const T& val) {
    return mfwu::binary_search(first, last, val, mfwu::less<T>{});
}

// 09. lower_bound upper_bound equal_range
template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
ForwardIt lower_bound(ForwardIt first, ForwardIt last, const T& val);

template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,  
          typename Compare>
ForwardIt upper_bound(ForwardIt first, ForwardIt last,
                      const T& val, Compare comp);

template <typename ForwardIt, 
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& val);

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,
          typename Compare>
inline mfwu::pair<ForwardIt, ForwardIt>
equal_range(ForwardIt first, ForwardIt last, const T& val, Compare cmp) {
    return mfwu::make_pair(mfwu::lower_bound(first, last, val, cmp),
                           mfwu::upper_bound(first, last, val, cmp));
}

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline mfwu::pair<ForwardIt, ForwardIt>
equal_range(ForwardIt first, ForwardIt last, const T& val) {
    return mfwu::equal_range(first, last, val, cmp);
}

// 10. copy copy_backward remove remove_if remove_copy remove_copy_if
//     replace replace_copy replace_if replace_copy_if
template <typename InputIt, typename OutputIt>
OutputIt copy(InputIt first, InputIt last, OutputIt res);

template <typename RandomAccessIterator>
inline void copy_backward(RandomAccessIterator first, 
                          RandomAccessIterator last,
                          RandomAccessIterator res);

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline ForwardIt remove(ForwardIt first, ForwardIt last, const T& val) {
    first = mfwu::find(first, last, val);
    if (first != last) {
        for (ForwardIt i = first; ++i != last; ) {
            if (!(*i == val)) {
                *first++ = mfwu::move(*i);
            }
        }
    }
    return first;
}

template <typename ForwardIt, typename UnaryPred>
inline ForwardIt remove_if(ForwardIt first, ForwardIt last, UnaryPred p) {
    first = mfwu::find_if(first, last, p);
    if (first != last) {
        for (ForwardIt i = first; ++i != last; ) {
            if (!p(*i)) {
                *first++ = mfwu::move(*i);
            }
        }
    }
    return first;
}

template <typename InputIt, typename OutputIt,
          typename T = typename mfwu::iterator_traits<InputIt>::value_type>
inline OutputIt remove_copy(InputIt first, InputIt last,
                            OutputIt res, const T& val) {
    for ( ; first != last; ++first) {
        if (!(*first == val)) {
            *res++ = *first;
        }
    }
    return res;
}

template <typename InputIt, typename OutputIt, typename UnaryPred>
inline OutputIt remove_copy_if(InputIt first, InputIt last,
                               OutputIt res, UnaryPred p) {
    for ( ; first != last; ++first) {
        if (!p(*first)) {
            *res++ = *first;
        }
    }
    return res;
}

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type>
inline void replace(ForwardIt first, ForwardIt last,
                    const T& oldv, const T& newv) {
    for ( ; first != last; ++first) {
        if (*first == oldv) {
            *first == newv;
        }
    }
}

template <typename ForwardIt,
          typename T = typename mfwu::iterator_traits<ForwardIt>::value_type,
          typename UnaryPred>  // TODO: can i ?
inline void replace_if(ForwardIt first, ForwardIt last,
                       UnaryPred p, const T& newv) {
    for ( ; first != last; ++first) {
        if (p(*first)) {
            *first = newv;
        }
    }
}

template <typename InputIt, typename OutputIt, 
          typename T = typename mfwu::iterator_traits<InputIt>::value_type>
inline OutputIt replace_copy(InputIt first, InputIt last, OutputIt res,
                             const T& oldv, const T& newv) {
    for ( ; first != last; ++first) {
        *res++ = (*first == oldv) ? newv : *first;
    }
    return res;
}

template <typename InputIt, typename OutputIt, typename UnaryPred,
          typename T = typename mfwu::iterator_traits<InputIt>::value_type>
inline OutputIt replace_copy_if(InputIt first, InputIt last, OutputIt res,
                                UnaryPred p, const T& newv) {
    for ( ; first != last; ++first) {
        *res++ = p(*first) ? newv : *first;
    }
    return res;
}

// 11. unique unique_copy
template <typename ForwardIt>
inline ForwardIt unique(ForwardIt first, ForwardIt last) {
    if (first == last) return last;
    ForwardIt res = first;
    while (++first != last) {
        if (!(*res == *first) && ++res != first) {
            *res = mfwu::move(*first);
        }
    }
    return ++res;
}

template <typename ForwardIt, typename BinaryPredicate>
inline ForwardIt unique(ForwardIt first, ForwardIt last, BinaryPredicate p) {
    if (first == last) return last;
    ForwardIt res = first;
    while (++first != last) {
        if (!p(*res, *first) && ++res != first) {
            *res = mfwu::move(*first);
        }
    }
    return ++res;
}

// know more in:
// https://github.com/gcc-mirror/gcc
// /blob/d9375e490072d1aae73a93949aa158fcd2a27018
// /libstdc%2B%2B-v3/include/bits/stl_algo.h#L1046
// template <typename InputIt, typename OutputIt, typename BinaryPred>
// inline OutputIt unique_copy(InputIt first, InputIt last,
//                             OutputIt res, BinaryPred p);
// inline OutputIt unique_copy(ForwardIt first, ForwardIt last,
//                             OutputIt res, BinaryPred p);

template <typename InputIt, typename ForwardIt>
inline ForwardIt unique_copy(InputIt first, InputIt last, ForwardIt res) {
    *res = *first;
    while (++first != last) {
        if (!(*res == *first)) {
            *++res = *first;
        }
    }
    return ++res;
}

template <typename InputIt, typename ForwardIt, typename BinaryPred>
inline ForwardIt unique_copy(InputIt first, InputIt last,
                            ForwardIt res, BinaryPred p) {
    *res = *first;
    while (++first != last) {
        if (!p(*res, *first)) {
            *++res = *first;
        }
    }
    return ++res;
}

// 12. swap swap_range iter_swap
// swap defined in common.hpp
// TODO: plz offer specialization for every mfwu-std object
template <typename ForwardIt1, typename ForwardIt2>
inline ForwardIt2 swap_ranges(ForwardIt1 first, ForwardIt1 last,
                              ForwardIt2 res) {
    for ( ; first != last; ++first, ++res) {
        mfwu::iter_swap(first, res);
    }
    return res;
}

template <typename Iterator1, typename Iterator2>
inline void iter_swap(Iterator1 it1, Iterator2 it2);
// TODO: we should gather definitions here and leave
//       declaration in the original positions

}  // endof namespace mfwu

#endif  // __ALGORITHM_HPP__
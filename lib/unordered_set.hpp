#ifndef __UNORDERED_SET_HPP__
#define __UNORDERED_SET_HPP__

#include "hashtable.hpp"
#include "hash_algorithm_pool.hpp"

namespace mfwu {

class unit_test_unordered_set;

template <typename Key,
          typename Hash=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::sbucket<Key>, mfwu::malloc_alloc>>
class unordered_set {
public:
    friend class unit_test_unordered_set;
    using key_type = Key;
    using value_type = Key;  // TODO: check
    using size_type = size_t;

    using hashtable = mfwu::shashtable<Key, Hash, Alloc>;
    using iterator = hashtable::iterator;

    unordered_set() : htbl_() {}

    iterator begin() const { return htbl_.begin(); }
    iterator end() const { return htbl_.end(); }

private:
    hashtable htbl_;
};  // endof class unordered_set

}  // endof namespace mfwu

#endif  // __UNORDERED_SET_HPP__
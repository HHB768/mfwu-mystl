#ifndef __UNORDERED_MAP_HPP__
#define __UNORDERED_MAP_HPP__

#include "hashtable.hpp"
#include "hash_algorithm_pool.hpp"

namespace mfwu {

template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket<Key, Value>, mfwu::malloc_alloc>>
class unordered_map {
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;

    using hashtable = mfwu::hashtable<Key, Value, Hash, Alloc>;
    using iterator = hashtable::iterator;

    unordered_map() : htbl_() {}

    iterator begin() const { return htbl_.begin(); }
    iterator end() const { return htbl_.end(); }
    
private:
    hashtable htbl_;
};  // endof class unordered_map

}  // endof namespace mfwu


#endif  // __UNORDERED_MAP_HPP__
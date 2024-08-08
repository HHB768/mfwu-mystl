#ifndef __HASHTABLE_HPP__
#define __HASHTABLE_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "list.hpp"
#include "pair.hpp"
#include <initializer_list>

namespace mfwu {

template <typename Key, 
          typename Value, 
          typename Alloc=mfwu::DefaultAllocator<
            mfwu::list<mfwu::pair<Key, Value>>, mfwu::malloc_alloc>>
class _hashtable_base {
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;
    
    _hashtable_base() : primer_idx(0), 
        buckets(Alloc::allocate(mfwu::primer_list[primer_idx])) {}
    // _hashtable_base(const std::initialzied_list<mfwu::>& hashed_keys) ?
    void insert(const key_type& key, const value_type& val) {
        size_type hashed_key = hash(key);
        buckets[hashed_key].push_front(
            mfwu:make_pair<key_type, value_type>(key, val));
    }
    void insert(const mfwu::pair<key_type, value_type>& key_val) {
        insert(key_val.first, key_val.second);
    }
    void erase(const key_type& key) {
        
    }

private:
    size_type hash(const key_type& key) {
        return mfwu::hash_algorithm_pool<key_type>::hash(key)
               % mfwu::primer_list[primer_idx];
    }
    size_type primer_idx;
    mfwu::list<value_type>* buckets;
};  // endof class _hashtable_base

template <typename T>
class hashtable {
public:
    using value_type = T;
    using size_type = size_t;
    
    hashtable() : arr() {}
    hashtable(const std::initialzied_list<value_type>& vals) {
        for (const value_type& val : vals) { arr.insert(hash(vals)); }
    }
};  // endof class hashtable


}  // endof namespace mfwu


#endif  // __HASHTABLE_HPP__
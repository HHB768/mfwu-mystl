#ifndef __UNORDERED_MAP_HPP__
#define __UNORDERED_MAP_HPP__

#include "hashtable.hpp"
#include "hash_algorithm_pool.hpp"

namespace mfwu {

template <typename Key,
          typename Value,
          typename Hasher=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket<Key, Value>, mfwu::malloc_alloc>>
class unordered_map {
public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = mfwu::pair<const Key, Value>

    using size_type = size_t;

    using hashtable = mfwu::hashtable<Key, Value, Hasher, Alloc>;
    using iterator = hashtable::iterator;

    // in std::unordered_map constructor uses
    // bucket_count instead of capacity as the param
    // but forget this ... 24.10.18
    // man, what can i say, i just recognize that
    // capacity is exactly bucket_count...
    // 24.10.18 night XD
    unordered_map() : htbl() {}    
    unordered_map(size_type capacity) : htbl_(capacity) {}
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  is_input_iterator<InputIterator>::value>
              >
    unordered_map(InputIterator first, InputIterator last) 
        : htbl_(first, last) {}
    unordered_map(const std::initializer_list<value_type>& vals)
        : htbl_(vals) {}
    unordered_map(const unordered_map& m) : htbl_(m.htbl_) {}
    unordered_map(unordered_map&& m) : htbl_(mfwu::move(m.htbl_)) {}
    ~unordered_map() { ~htbl_(); }

    unordered_map& operator=(const unordered_map& m) {
        htbl_ = m.htbl_;
        return *this;
    }
    unordered_map& operator(unordered_map&& m) {
        htbl_ = mfwu::move(m.htbl_);
        return *this;
    }
    Alloc get_allocator() const { return allocator_; }
    
    iterator begin() const { return htbl_.begin(); }
    iterator end() const { return htbl_.end(); }
    // we dont define cbegin and cend here

    bool empty() const { return htbl_.empty(); }
    size_type size() const { return htbl_.size(); }
    size_type max_size() const { return mfwu::primer_list.back(); }

    void clear() {
        hashtable empty_htbl(htbl_.capacity());
        mfwu::swap(empty_htbl, htbl_);
    }
    // mfwu::pair<iterator, bool> insert(const value_type& value) {}
    // mfwu::pair<iterator, bool> insert(value_type&& value) {}
    // void insert(InputIterator first, InputIterator last) {}
    // void insert(std::initializer_list<value_type> ilist) {}
    // mfwu::pair<iterator, bool> emplace(Args&&... args) {}
    // erase(iterator it) {}
    // erase(const_iterator it) {}
    // size_type erase(const Key& key) {}
    // void swap(unordered_map& other) {}
    
    // T& at(const Key& key) {}
    // const T& at(const Key& key) const {}
    // T& operator[](const Key& key) {}
    // size_type count(const Key& key) const {}
    // iterator find(const Key& key) {}
    // bool contains(const Key& key) const {}
    
    // float load_factor() const {}
    // float max_load_factor() const {}
    // void rehash(size_type capacity) {}
    // void reserve(size_type size) {}  // == rehash(std::ceil(count / max_load_factor()))

    // hash_function
    // key_eq

    
private:
    hashtable htbl_;
    Alloc allocator_;
    Hasher hash_;
};  // endof class unordered_map

}  // endof namespace mfwu


#endif  // __UNORDERED_MAP_HPP__
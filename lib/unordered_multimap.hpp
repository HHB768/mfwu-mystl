#ifndef __UNORDERED_MULTIMAP_HPP__
#define __UNORDERED_MULTIMAP_HPP__


#include "hashtable_exp.hpp"
#include "hash_algorithm_pool.hpp"

namespace mfwu {

class unit_test_unordered_multimap;

template <typename Key,
          typename Value,
          typename Hasher=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket_exp<Key, Value>, mfwu::malloc_alloc>>
class unordered_multimap {
public:
    friend class unit_test_unordered_multimap;
    using key_type = Key;
    using mapped_type = Value;
    using value_type = mfwu::pair<const Key, Value>;

    using size_type = size_t;

    using hashtable = mfwu::hashtable_exp<Key, Value, Hasher, Alloc>;
    using iterator = typename hashtable::iterator;

    unordered_multimap() : htbl_() {}    
    unordered_multimap(size_type capacity) : htbl_(capacity) {}
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    unordered_multimap(InputIterator first, InputIterator last) 
        : htbl_(first, last) {}
    unordered_multimap(const std::initializer_list<value_type>& vals)
        : htbl_(vals) {}
    unordered_multimap(const unordered_multimap& m) : htbl_(m.htbl_) {}
    unordered_multimap(unordered_multimap&& m) : htbl_(mfwu::move(m.htbl_)) {}
    ~unordered_multimap() {}

    unordered_multimap& operator=(const unordered_multimap& m) {
        htbl_ = m.htbl_;
        return *this;
    }
    unordered_multimap& operator=(unordered_multimap&& m) {
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
        // TODO: REALLY? std: O(n) 12.7/24
    }
    mfwu::pair<iterator, bool> insert(const value_type& value) {
        return htbl_.insert(value);
    }
    // we havent implemented rvalue push
    // mfwu::pair<iterator, bool> insert(value_type&& value);
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
              >
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first) {
            htbl_.insert(*first);
        }
    }
    void insert(std::initializer_list<value_type> ilist) {
        for (auto&& val : ilist) {
            htbl_.insert(val);
        }
    }
    template <typename... Args>
    mfwu::pair<iterator, bool> emplace(Args&&... args) {
        return htbl_.insert(value_type{mfwu::forward<Args>(args)...});
    }
    // try_emplace
    iterator erase(iterator it) {
        return htbl_.erase(it);
    }
    // we havent implemented const_iterator
    // erase(const_iterator it);
    size_type erase(const Key& key) {
        return htbl_.erase(key);
    }
    void swap(unordered_multimap& other) {
        mfwu::swap(htbl_, other.htbl_);
    }
    // mapped_type& at(const Key& key);
    // const mapped_type& at(const Key& key);
    mapped_type& operator[](const Key& key) {
        return htbl_[key];
    }
    size_type count(const Key& key) const {
        return htbl_.count(key);
    }
    iterator find(const Key& key) const {
        return htbl_.find(key);
    }
    bool contains(const Key& key) const {
        return htbl_.count(key);
    }
    
    float load_factor() const {
        return htbl_.load_factor();
    }
    float max_load_factor() const {
        return htbl_.max_load_factor();
    }
    void rehash(size_type capacity) {
        htbl_.rehash(capacity);
    }
    void reserve(size_type size) {
        htbl_.rehash(std::ceil(size / max_load_factor()));
    }

    Hasher hash_function() const {
        return hash_;
    }
    // we havent implemented this
    // key_eq

private:
    hashtable htbl_;
    static Alloc allocator_;
    static Hasher hash_;
};  // endof class unordered_multimap

template <typename Key, typename Value, typename Hash, typename Alloc>
Alloc unordered_multimap<Key, Value, Hash, Alloc>::allocator_ = {};
template <typename Key, typename Value, typename Hash, typename Alloc>
Hash unordered_multimap<Key, Value, Hash, Alloc>::hash_ = {};

}  // endof namespace mfwu


#endif  // __UNORDERED_MULTIMAP_HPP__
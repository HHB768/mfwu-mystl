#ifndef __UNORDERED_MULTISET_HPP__
#define __UNORDERED_MULTISET_HPP__

#include "hashtable_exp.hpp"
#include "hash_algorithm_pool.hpp"

namespace mfwu {

class unit_test_unordered_multiset;

template <typename Key,
          typename Hasher=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::sbucket_exp<Key>, mfwu::malloc_alloc>>
class unordered_multiset {
public:
    friend class unit_test_unordered_multiset;
    using key_type = Key;
    using value_type = Key;  // TODO: check
    using size_type = size_t;

    using hashtable = mfwu::shashtable_exp<Key, Hasher, Alloc>;
    using iterator = typename hashtable::iterator;

    unordered_multiset() : htbl_() {}
    unordered_multiset(size_type capacity) : htbl_(capacity) {}
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    unordered_multiset(InputIterator first, InputIterator last) 
        : htbl_(first, last) {}
    unordered_multiset(const std::initializer_list<value_type>& vals)
        : htbl_(vals) {}
    unordered_multiset(const unordered_multiset& s) : htbl_(s.htbl_) {}
    unordered_multiset(unordered_multiset&& s) : htbl_(mfwu::move(s.htbl_)) {}
    ~unordered_multiset() {}

    unordered_multiset& operator=(const unordered_multiset& s) {
        htbl_ = s.htbl_;
        return *this;
    }
    unordered_multiset& operator=(unordered_multiset&& s) {
        htbl_ = mfwu::move(s.htbl_);
        return *this;
    }
    Alloc get_allocator() const { return allocator_; }

    iterator begin() const { return htbl_.begin(); }
    iterator end() const { return htbl_.end(); }

    bool empty() const { return htbl_.empty(); }
    size_type size() const { return htbl_.size(); }
    size_type max_size() const { return mfwu::primer_list.back(); }

    void clear() {
        hashtable empty_htbl(htbl_.capacity());
        mfwu::swap(empty_htbl, htbl_);
    }
    mfwu::pair<iterator, bool> insert(const value_type& value) {
        return htbl_.insert(value);
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
              >
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first) {
            htbl_.insert(*first);
        }
    }
    void insert(const std::initializer_list<value_type>& ilist) {
        for (auto&& val : ilist) {
            htbl_.insert(val);
        }
    }
    template <typename... Args>
    mfwu::pair<iterator, bool> emplace(Args&&... args) {
        return htbl_.insert(value_type{mfwu::forward<Args>(args)...});
    }
    iterator erase(iterator it) {
        return htbl_.erase(it);
    }
    size_type erase(const Key& key) {
        return htbl_.erase(key);
    }
    void swap(unordered_multiset& other) {
        mfwu::swap(htbl_, other.htbl_);
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

private:
    hashtable htbl_;
    static Alloc allocator_;
    static Hasher hash_;
};  // endof class unordered_multiset

template <typename Key, typename Hash, typename Alloc>
Alloc unordered_multiset<Key, Hash, Alloc>::allocator_ = {};
template <typename Key, typename Hash, typename Alloc>
Hash unordered_multiset<Key, Hash, Alloc>::hash_ = {};

}  // endof namespace mfwu

#endif  // __UNORDERED_MULTISET_HPP__

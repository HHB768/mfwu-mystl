#ifndef __HASHTABLE_WTOT_HPP__
#define __HASHTABLE_WTOT_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "hash_algorithm_pool.hpp"
#include "map.hpp"
#include <initializer_list>

namespace mfwu {

// we will not offer bucket_exp_wtot
// X-Qx 250209

template <typename key_type, typename value_type>
class bucket_with_rbtree {
public:
    using rbtr = mfwu::map<key_type, value_type>;
    using node = typename rbtr::node;
    using iterator = typename rbtr::iterator;

    bucket_with_rbtree() : rbt_() {}
    bucket_with_rbtree(const bucket_with_rbtree& bkt) : rbt_(bkt.rbt_) {}
    bucket_with_rbtree(bucket_with_rbtree&& bkt) : rbt_(mfwu::move(bkt.rbt_)) {}
    ~bucket_with_rbtree() {}

    bucket_with_rbtree& operator=(const bucket_with_rbtree& bkt) {
        rbt_ = bkt.rbt_;
        return *this;
    }
    bucket_with_rbtree& operator=(bucket_with_rbtree&& bkt) {
        rbt_ = mfwu::move(bkt.rbt_);
        return *this;
    }
    
    bool empty() { return rbt_.empty(); }
    size_t size() {
        return rbt_.size();
    }
    node* front() { return rbt_.begin().get_cur(); }
    mfwu::pair<node*, bool> push(const key_type& key, 
                                 const value_type& value) {
        mfwu::pair<iterator, bool> res = rbt_.insert(key, value);
        return mfwu::make_pair<node*, bool>(res.first.get_cur(), res.second);
        // 颇有大炮打蚊子的美感 xQx 25.02.09
    }
    // DONT USE THIS
    void pop() {
        rbt_.erase(rbt_.begin());
    }
    bool pop(const key_type& key) {
        return rbt_.erase(key);
    }
    bool pop(node* cur) {
        return pop(cur->val.first);
    }
    mfwu::pair<value_type&, bool> get(const key_type& key) {
        node* ret = search(key);
        bool is_new_node = false;
        if (ret == nullptr) {
            ret = push(key, value_type{}).first;
            is_new_node = true;
        }
        return {ret->val.second, is_new_node};
    }
    bool count(const key_type& key) const {
        return search(key) != nullptr;
    }
    node* find(const key_type& key) const {
        return search(key);
    }
private:
    node* search(const key_type& key) const {
        return rbt_.find(key).get_cur();
    }

    rbtr rbt_;
};  // endof class bucket

class unit_test_hashtable_wtot;

template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket_with_rbtree<Key, Value>, mfwu::malloc_alloc>>
class hashtable_wtot {
public:
    friend class mfwu::unit_test_hashtable_wtot;
    using size_type = size_t;

    using bucket = mfwu::bucket_with_rbtree<Key, Value>;
    using node = bucket::node;
    class hashtable_iterator {
    public:
        using value_type = mfwu::pair<const Key, Value>;
        using iterator_category = mfwu::forward_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;

        hashtable_iterator() : cur_(nullptr), buckets_(nullptr) {}
        hashtable_iterator(node* cur, bucket* buckets)
            : cur_(cur), buckets_(buckets) {}
        hashtable_iterator(const hashtable_iterator& it)
            : cur_(it.cur_), buckets_(it.buckets_) {}
        ~hashtable_iterator() {}
        hashtable_iterator& operator=(const hashtable_iterator& it) {
            cur_ = it.cur_; buckets_ = it.buckets_;
            return *this;
        }
        bool operator==(const hashtable_iterator& it) const {
            return buckets_ == it.buckets_ && cur_ == it.cur_;
        }
        bool operator!=(const hashtable_iterator& it) const {
            return !(*this == it);
        }
        value_type& operator*() const {
            return cur_->val;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        hashtable_iterator& operator++() {
            cur_ = cur_->get_inorder_next();  // here
            while (cur_ == nullptr) {
                ++buckets_;
                cur_ = buckets_->front();
            }
            return *this;
        }
        hashtable_iterator operator++(int) {
            hashtable_iterator tmp = *this;
            this->operator++();
            return mfwu::move(tmp);
        }
        node* get_cur() const {
            return cur_;
        }
        bucket* get_bucket() const {
            return buckets_;
        }
    private:
        node* cur_;
        bucket* buckets_;
    };  // endof class hashtable_iterator
    using iterator = hashtable_iterator;

    using key_type = Key;
    using value_type = Value;

    hashtable_wtot(): capacity_(mfwu::get_next_primer(0)), size_(0),
        buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_wtot(size_type capacity) 
        : capacity_(mfwu::get_next_primer(capacity)),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_wtot(const std::initializer_list<
              mfwu::pair<const key_type, value_type>>& vals)
    // GPT DEBUG LOG 10.21/24:
    /*
        std::initializer_list elements are immutable, meaning they need to be const. 
        When you initialize with std::pair<B, C>, the compiler expects the first element 
        of the pair to be const B. So to avoid compilation errors, you must use 
        const std::initializer_list<std::pair<const B, C>>. 
        Keeps things aligned with how initializer_list handles immutability.
    */
        : capacity_(mfwu::get_next_primer(
                    std::ceil((float)vals.size() / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (auto&& [k, v] : vals) {
            this->insert(k, v);
        }
        init_dummy_node();
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  is_input_iterator<InputIterator>::value>
             >
    hashtable_wtot(InputIterator first, InputIterator last)
        : capacity_(mfwu::get_next_primer(std::ceil(
                    (float)(mfwu::distance(first, last)) / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (; first != last; ++first) {
            this->insert(*first);
        }
        init_dummy_node();
    }
    hashtable_wtot(const hashtable_wtot& tbl) 
        : capacity_(tbl.capacity_), size_(tbl.size_),
          buckets_(Alloc::allocate(capacity_ + 1)) {
        mfwu::uninitialized_copy(tbl.buckets_,
            tbl.buckets_ + capacity_ + 1, buckets_);
    }
    hashtable_wtot(hashtable_wtot&& tbl) : capacity_(tbl.capacity_),
        size_(tbl.size_), buckets_(tbl.buckets_) {
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  
        // found it ! 
        // not in 10.11 but 241022
    }
    ~hashtable_wtot() {
        reset();
    }
    hashtable_wtot& operator=(const hashtable_wtot& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::uninitialized_copy(tbl.buckets_, 
            tbl.buckets_ + capacity_ + 1, buckets_);
        return *this;
    }
    hashtable_wtot& operator=(hashtable_wtot&& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = tbl.buckets_;
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  // nt!
        // let deallocate() deallocate nothing
        // 24.10.11
        return *this;
    }
    mfwu::pair<iterator, bool> insert(const key_type& key, const value_type& val) {
        mfwu::pair<node*, bool> ret = buckets_[hash(key)].push(key, val);
        add_cnt(ret.second);
        return {find(key), ret.second};
        // note: you must search again bcz add_cnt may
        //       rehash the buckets, but found on 24.10.23 T^T
    }
    mfwu::pair<iterator, bool> insert(const mfwu::pair<const key_type, value_type>& key_val) {
        return insert(key_val.first, key_val.second);
    } 
    bool erase(const key_type& key) {
        size_type hashed_key = hash(key);
        bool ret = buckets_[hashed_key].pop(key);
        size_ -= ret;
        return ret;
    }
    iterator erase(iterator it) {
        node* cur = it.get_cur();
        bucket* bkt = it.get_bucket();
        ++it;
        bkt->pop(cur);
        return it;
    }
    // TODO: erase by iterator !
    value_type& operator[](const key_type& key) {
        // assert
        add_cnt(buckets_[hash(key)].get(key).second);
        // note: you must search again bcz add_cnt may
        //       rehash the buckets 24.10.11 
        return buckets_[hash(key)].get(key).first;
    }
    bool count(const key_type& key) const {
        return buckets_[hash(key)].count(key);
    }
    iterator find(const key_type& key) const {
        bucket* bkt = buckets_ + hash(key);
        node* cur = bkt->find(key);
        if (cur == nullptr) {
            return end();
        }
        return iterator(cur, bkt);
    }
    float load_factor() const {
        return (float)size_ / capacity_;
    }
    float max_load_factor() const {
        return alpha;
    }
    void rehash(size_type capacity) {
        capacity = mfwu::get_next_primer(capacity);
        while ((float)size_ / capacity >= max_load_factor()) {
            capacity = mfwu::get_next_primer(capacity + 1);
        }
        rehash_hard(capacity);  // TODO: check
    }
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type capacity() const { return capacity_; }
    iterator begin() const { return iterator(get_first_node(), &get_first_bucket()); }
    iterator end() const { return iterator(get_dummy_node(), &get_dummy_bucket()); }
private:
    void construct() {
        bucket bkt{};  // avoid move construct
        mfwu::construct(buckets_, buckets_ + capacity_ + 1, bkt);
    }
    // TODO: let me think, construct dummy bucket first, then construct others with
    //       enable = dummy.enable (which is new bool(f) swhere)
    //       when req_mem, keep the same bool*
    void destroy() { mfwu::destroy(buckets_, buckets_ + capacity_ + 1); }
    void deallocate() { Alloc::deallocate(buckets_, capacity_ + 1); }
    void reset() { destroy(); deallocate(); }
    void init_dummy_node() {
        buckets_[capacity_].push(key_type{}, value_type{});
    }
    size_type hash(const key_type& key) const {
        return hashfunc_(key) % capacity_;
    }
    void rehash_hard(size_type capacity) {
        hashtable_wtot newtable = hashtable_wtot(capacity);
        for (size_type idx = 0; idx < capacity_; ++idx) {
            bucket cur = buckets_[idx];
            while (!cur.empty()) {
                node* nd = cur.front();
                newtable.insert(nd->val.first, nd->val.second);
                cur.pop();
            }
        }
        newtable.size_ = this->size_;
        *this = mfwu::move(newtable);
    }
    void req_mem() {
        rehash(capacity_ + 1);
    }
    void add_cnt(size_type num) {
        if (!num) return ;
        size_ += num;
        while ((float)size_ / capacity_ > alpha) {
            req_mem();
        }
    }

    bucket& get_first_bucket() const {
        for (size_type idx = 0; idx < capacity_; ++idx) {
            if (!buckets_[idx].empty()) return buckets_[idx];
        }
        return get_dummy_bucket();
    }
    node* get_first_node() const {
        return get_first_bucket().front();
    }
    bucket& get_dummy_bucket() const {
        return buckets_[capacity_];
    }
    node* get_dummy_node() const {
        return get_dummy_bucket().front();
    }

    size_type capacity_;
    size_type size_;
    bucket* buckets_;
    static Hash hashfunc_;
    constexpr static float alpha = 0.7F;

};  // endof class hashtable_wtot

template <typename Key, typename Value, typename Hash, typename Alloc>
Hash hashtable_wtot<Key, Value, Hash, Alloc>::hashfunc_ = {};

}  // endof namespace mfwu


#endif  // __HASHTABLE_WTOT_HPP__

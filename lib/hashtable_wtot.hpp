#ifndef __HASHTABLE_WTOT_HPP__
#define __HASHTABLE_WTOT_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "hash_algorithm_pool.hpp"
#include "multimap.hpp"
#include <initializer_list>

namespace mfwu {

// we will not offer bucket_wtot for non-exp node
// there is further explanation below
// X-Qx 250209

template <typename key_type, typename value_type, typename Compare=mfwu::less<key_type>>
class bucket_with_rbtree {
public:
    // mfwu::map uses node with value_type "pair<key_t, val_t>"
    // instead of "pair<const key_t, val_t>" bcz its rbtree may
    // change the node value to rebalance itself
    // so we will no offer bucket_wtot for non-exp node
    // X 250210
    using rbtr = mfwu::multimap<key_type, value_type, Compare>;
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

    rbtr rbt_;  // multimap now
};  // endof class bucket

class unit_test_hashtable_wtot;

template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Compare=mfwu::less<Key>,  // these hashtble needs comparability
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket_with_rbtree<Key, Value, Compare>, mfwu::malloc_alloc>>
class hashtable_with_tree {
public:
    friend class mfwu::unit_test_hashtable_wtot;
    using size_type = size_t;

    using bucket = mfwu::bucket_with_rbtree<Key, Value, Compare>;
    using node = typename bucket::node;
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
        const value_type& operator*() const {
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

    hashtable_with_tree(): capacity_(mfwu::get_next_primer(0)), size_(0),
        buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_with_tree(size_type capacity) 
        : capacity_(mfwu::get_next_primer(capacity)),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_with_tree(const std::initializer_list<
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
    hashtable_with_tree(InputIterator first, InputIterator last)
        : capacity_(mfwu::get_next_primer(std::ceil(
                    (float)(mfwu::distance(first, last)) / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (; first != last; ++first) {
            this->insert(*first);
        }
        init_dummy_node();
    }
    hashtable_with_tree(const hashtable_with_tree& tbl) 
        : capacity_(tbl.capacity_), size_(tbl.size_),
          buckets_(Alloc::allocate(capacity_ + 1)) {
        mfwu::uninitialized_copy(tbl.buckets_,
            tbl.buckets_ + capacity_ + 1, buckets_);
    }
    hashtable_with_tree(hashtable_with_tree&& tbl) : capacity_(tbl.capacity_),
        size_(tbl.size_), buckets_(tbl.buckets_) {
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  
        // found it ! 
        // not in 10.11 but 241022
    }
    ~hashtable_with_tree() {
        reset();
    }
    hashtable_with_tree& operator=(const hashtable_with_tree& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::uninitialized_copy(tbl.buckets_, 
            tbl.buckets_ + capacity_ + 1, buckets_);
        return *this;
    }
    hashtable_with_tree& operator=(hashtable_with_tree&& tbl) {
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
        hashtable_with_tree newtable = hashtable_with_tree(capacity);
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

};  // endof class hashtable_with_tree

template <typename Key, typename Value, typename Hash, typename Compare, typename Alloc>
Hash hashtable_with_tree<Key, Value, Hash, Compare, Alloc>::hashfunc_ = {};

template <typename Key, typename Value, typename Hash, typename Alloc>
hashtable_with_htbl<Key, Value, Hash, Alloc>;

template <typename key_type, typename value_type, typename Hash, typename Alloc>
class bucket_with_htbl {
public:
    struct bucket_node {
        mfwu::pair<const key_type, value_type> value;
        // not a good idea 24.12.08
        bucket_node* next;

        // TODO: i found many class lacks one or more
        // rvalue constructor
        bucket_node() : value(), next(nullptr) {}
        bucket_node(const key_type& k, const value_type& v)
            : value(k, v), next(nullptr) {}
        bucket_node(const key_type& k, const value_type& v, bucket_node* n)
            : value(k, v), next(n) {}
        bucket_node(const bucket_node& nd)
            : value(nd.value), next(nd.next) {}
        bucket_node(bucket_node&& nd) 
            : value(mfwu::move(nd.value)), next(nd.next) {
            nd.next = nullptr;
        }
    };  // endof struct bucket_node
    using node = bucket_node;

    bucket_with_htbl() :  {}
    bucket_with_htbl(const bucket_with_htbl& bkt) {
        bucket_copy(bkt);
    }
    bucket_with_htbl(bucket_with_htbl&& bkt) : head_(bkt.head_) {
        bkt.head_ = nullptr;
    }
    ~bucket_with_htbl() {
        bucket_destroy();
    }

    bucket_with_htbl& operator=(const bucket_with_htbl& bkt) {
        clear();
        bucket_copy(bkt);
        return *this;
    }
    bucket_with_htbl& operator=(bucket_with_htbl&& bkt) {
        clear();
        head_ = bkt.head_;
        bkt.head_ = nullptr;
    }
    
    bool empty() { return head_->next == nullptr; }
    size_t size() {
        node* cur = head_->next;
        size_t cnt = 0;
        while (cur) {
            cnt++;
            cur = cur->next;
        }
        return cnt;
    }
    node* front() { return head_->next; }
    mfwu::pair<node*, bool> push(const key_type& key, 
                                 const value_type& value) {
        // node* ret = search(key);
        // if (ret != nullptr) {
        //     ret->value.second = value;
        //     return {ret, false};
        // }
        node* next = head_->next;
        node* cur = new node(key, value, next);
        head_->next = cur;
        return {cur, true};
    }
    void pop() {
        node* next = head_->next;
        head_->next = next->next;
        delete next;
    }
    bool pop(const mfwu::pair<const key_type, value_type>& val) {
        node* prev = head_;
        while (prev->next) {
            if (prev->next->value == val) {
                node* next = prev->next;
                prev->next = next->next;
                delete next;
                return true;
            }
            prev = prev->next;
        }
        return false;
    }
    bool pop(const key_type& key) {
        node* prev = head_;
        while (prev->next) {
            if (prev->next->value.first == key) {
                node* next = prev->next;
                prev->next = next->next;
                delete next;
                return true;
            }
            prev = prev->next;
        }
        return false;
    }  // TODO: remove all elements with .key == key
    bool pop(node* cur) {
        node* prev = head_;
        while (prev->next) {
            if (prev->next == cur) {
                prev->next = cur->next;
                delete cur;
                return true;
            }
            prev = prev->next;
        }
        return false;
    }
    mfwu::pair<value_type&, bool> get(const key_type& key) {
        node* ret = search(key);
        bool is_new_node = false;
        if (ret == nullptr) {
            ret = push(key, value_type{}).first;
            is_new_node = true;
        }
        return {ret->value.second, is_new_node};
    }
    bool count(const key_type& key) const {
        return search(key) != nullptr;
    }
    node* find(const key_type& key) const {
        return search(key);
    }
private:
    void bucket_copy(const bucket_with_htbl& bkt) {
        head_ = new node(*bkt.head_);
        node* src = bkt.head_;
        node* dst = head_;
        while (src->next != nullptr) {
            dst->next = new node(*src->next);
            src = src->next;
            dst = dst->next;
        }  // CHECK: dst->next === nullptr
    }
    void bucket_destroy() {
        clear();
        delete head_;
    }
    void clear() { while (head_->next) { pop(); } }
    node* search(const key_type& key) const {
        node* cur = head_->next;
        while (cur) {
            if (cur->value.first == key) {
                return cur;
            }
            cur = cur->next;
        }
        return nullptr;
    }

    mfwu::pair<const key_type, value_type> val_;
    hashtable_with_htbl<key_type, value_type, Hash, Alloc>* htbl_;

};  // endof class bucket_with_htbl

class unit_test_hashtable_wtot;

template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket_with_htbl<Key, Value, Hash, Alloc>, mfwu::malloc_alloc>>
class hashtable_with_htbl {
public:
    friend class mfwu::unit_test_hashtable_exp;
    friend class mfwu::unordered_map<Key, Value, Hash, Alloc>;
    // using key_type = Key;
    // using value_type = Value;
    using size_type = size_t;
    // actual size should be within max_primer

    
    using bucket = mfwu::bucket_with_htbl<Key, Value>;
    using node = typename bucket::node;

    class hashtable_iterator {
    public:
        // TODO: should i use value_type = 
        //       mfwu::pair<Key, Value> ?
        //       how to adjust when Key is Value
        // 10.15: yes you should
        // TODO: plz try this
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
            return cur_->value;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        hashtable_iterator& operator++() {
            cur_ = cur_->next;
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

    hashtable_with_htbl(): capacity_(mfwu::get_next_primer(0)), size_(0),
        buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_with_htbl(size_type capacity) 
        : capacity_(mfwu::get_next_primer(capacity)),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        // GPT DEBUG HISTORY 24.10.11
        /*
            If the objects constructed in the loop are being destructed immediately
            after the loop, ensure their destructors are not causing the crash. 
            This can happen if the destructor tries to access invalid memory.
        */
        init_dummy_node();
    }
    hashtable_with_htbl(const std::initializer_list<
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
    hashtable_with_htbl(InputIterator first, InputIterator last)
        : capacity_(mfwu::get_next_primer(std::ceil(
                    (float)(mfwu::distance(first, last)) / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (; first != last; ++first) {
            this->insert(*first);
        }
        init_dummy_node();
    }
    hashtable_with_htbl(const hashtable_with_htbl& tbl) 
        : capacity_(tbl.capacity_), size_(tbl.size_),
          buckets_(Alloc::allocate(capacity_ + 1)) {
        mfwu::uninitialized_copy(tbl.buckets_,
            tbl.buckets_ + capacity_ + 1, buckets_);
    }
    hashtable_with_htbl(hashtable_with_htbl&& tbl) : capacity_(tbl.capacity_),
        size_(tbl.size_), buckets_(tbl.buckets_) {
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  
        // found it ! 
        // not in 10.11 but 241022
    }
    ~hashtable_with_htbl() {
        reset();
    }
    hashtable_with_htbl& operator=(const hashtable_with_htbl& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::uninitialized_copy(tbl.buckets_, 
            tbl.buckets_ + capacity_ + 1, buckets_);
        return *this;
    }
    hashtable_with_htbl& operator=(hashtable_with_htbl&& tbl) {
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
        hashtable_with_htbl newtable = hashtable_with_htbl(capacity);
        for (size_type idx = 0; idx < capacity_; ++idx) {
            bucket cur = buckets_[idx];
            while (!cur.empty()) {
                node* nd = cur.front();
                newtable.insert(nd->value);
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

};  // endof class hashtable_with_htbl

template <typename Key, typename Value, typename Hash, typename Alloc>
Hash hashtable_with_htbl<Key, Value, Hash, Alloc>::hashfunc_ = {};

}  // endof namespace mfwu


#endif  // __HASHTABLE_WTOT_HPP__

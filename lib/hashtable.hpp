#ifndef __HASHTABLE_HPP__
#define __HASHTABLE_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "hash_algorithm_pool.hpp"
#include <initializer_list>

namespace mfwu {

template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         void*, mfwu::malloc_alloc>>
class hashtable {
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;
    // actual size should be within max_primer

    // consider to use mfwu::list
    // or mfwu::rbtree, or even
    // mfwu::hash_table* here
    // here we offer a simple list
    // to explain api
    // just usable in hashtable  X-H2 24.08.08
    // tree/list/hashtable
    class bucket {
    public:
        struct bucket_node {
            key_type key;
            value_type value;
            node* next;

            // TODO: i found many class lacks one or more
            // rvalue constructor
            node() : key(), value(), next(nullptr) {}
            node(const key_type& k, const value_type& v)
                : key(k), value(v), next(nullptr) {}
            node(const key_type& k, const value_type& v, node* n)
                : key(k), value(v), next(n) {}
            node(const node& nd)
                : key(nd.key), value(nd.value), next(nd.next) {}
            node(node&& nd) : key(mfwu::move(nd.key)),
                value(mfwu::move(nd.value)), next(nd.next) {
                nd.next = nullptr;
            }
        };  // endof struct bucket_node
        using node = bucket_node;

        bucket() : head_(new node(主)),
            next(nullptr), enable_next_(nullptr) {}
        bucket(const bucket& bkt) {
            bucket_copy(bkt);
        }
        bucket(bucket&& bkt) : head_(bkt.head_),
            next_(bkt.next_), enable_next_(bkt.enable_next_) {
            bkt.head_ = nullptr;
        }
        ~bucket() {
            bucket_destroy();
        }

        bucket& operator=(const bucket& bkt) {
            clear();
            bucket_copy(bkt);
            return *this;
        }
        bucket& operator=(bucket&& bkt) {
            clear();
            head_ = bkt.head_;
            bkt.head_ = nullptr;
        }
        
        bool empty() { return head_->next == nullptr; }
        size_type size() {
            node* cur = head_->next;
            size_type cnt = 0;
            while (cur) {
                cnt++;
                cur = cur->next;
            }
            return cnt;
        }
        node* front() { return head_->next; }
        mfwu::pair<node*, bool> push(const key_type& key, 
                                     const value_type& value) {
            node* ret = search(key);
            if (ret != nullptr) {
                ret->value = value;
                return {ret, false};
            }
            node* next = head_->next;
            node* cur = new node(key, value, next);
            head_->next = cur;
            return {cur, true};
        }
        void pop() {
            node* next - head_->next;
            head_->next = next->next;
            delete next;
        }
        bool pop(const key_type& key) {
            node* prev = head_;
            while (prev->next) {
                if (prev->next->key == key) {
                    node* next = prev->next;
                    prev->next = next->next;
                    delete next;
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
            return {ret->value, is_new_node};
        }
    private:
        void bucket_copy(const bucket& bkt) {
            head_ = new node(bkt.head_);
            node* src = bkt.head_;
            node* dst = head_;
            while (src != nullptr) {
                dst->next = new node(src->next);
                src = src->next;
                dst = dst->next;
            }
            next_ = bkt.next_;
            enable_next_ = bkt.enable_next_;
        }
        void bucket_destroy() {
            clear();
            delete head_;
        }
        void clear() { while (head_->next) { pop(); } }
        node* search(const key_type& key) {
            node* cur = head_->next;
            while (cur) {
                if (cur->key == key) {
                    return cur;
                }
                cur = cur->next;
            }
            return nullptr;
        }

        node* head_;
        bucket* next_;  // TODO: faster ++iterator? 
                       // but where to set the 'enable' attribute
        bool* enable_next_;
    };  // endof class bucket
    using bucket = bucket;
    using node = bucket::node;

    class hashtable_iterator {
    public:
        // TODO: should i use value_type = 
        //       mfwu::pair<Key, Value> ?
        //       how to adjust when Key is Value
        using value_type = Value;
        using iterator_category = mfwu::forward_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;

        hashtable_iterator() : cur_(nullptr), buckets_(nullptr) {}
        hashtable_iterator(node* cur, bucket* buckets)
            : cur_(cur), buckets_(bucket) {}
        hashtable_iterator(const hashtable_iterator& it)
            : cur_(it.cur), buckets_(it.buckets_) {}
        ~hashtable() {}
        hashtable_iterator& operator=(const hashtable_iterator& it) {
            cur_ = it.cur; buckets_ = it.buckets_;
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

        // TODO: BAD DESIGN, you should make node->next
        // can across buckets (LegacyForwardIterator ?)
        // or add a machenism that across the valid buckests 
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
    private:
        node* cur_;
        bucket* buckets_;
    };  // endof class hashtable_iterator
    using iterator = hashtable_iterator;

    hashtable(): capacity_(mfwu::get_next_primer(0)), size_(0),
        buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable(size_type capacity) 
        : capacity_(mfwu::get_next_primer(capacity)),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable(const std::initializer_list<
              mfwu::pair<key_type, value_type>>& vals)
        : capacity_(mfwu::get_next_primer(
                    std::ceil((float)vals.size() / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (auto&& [k, v] : vals) {
            this->insert(k, v);
        }
        init_dummy_node();
    }
    hashtable(const hashtable& tbl) 
        : capacity_(tbl.capacity_), size_(tbl.size_),
          buckets_(Alloc::allocate(capacity_ + 1)) {
        mfwu::uninitialized_copy(tbl.buckets_,
            tbl.buckets_ + capacity_ + 1, buckets_);
    }
    hashtable(hashtable&& tbl) : capacity_(tbl.capacity_),
        size_(tbl.size_), bucket_(tbl.buckets_) {
        tbl.buckets_ = nullptr;
    }
    ~hashtable() {
        destroy();
        Alloc::deallocate(buckets_, capacity_ + 1);
    }
    hashtable& operator=(const hashtable& tbl) {
        destroy();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::copy(tbl.buckets_, 
            tbl.buckets_ + capacity_ + 1, buckets_);
        return *this;
    }
    hashtable& operator=(hashtable&& tbl) {
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = tbl.buckets_;
        return *this;
    }
    void insert(const key_type& key, const value_type& val) {
        size_type hashed_key = hash(key);
        add_cnt(buckets_[hashed_key].push(key, val).second);
    }
    void insert(const mfwu::pair<key_type, value_type>& key_val) {
        insert(key_val.first, key_val.second);
    }
    void erase(const key_type& key) {
        size_type hashed_key = hash(key);
        size_ -= buckets_[hashed_key].pop(key);
    }
    value_type& operator[](const key_type& key) {
        // assert
        size_type hashed_key = hash(key);
        auto& [val, is_new_node] = buckets_[hashed_key].get(key);
        add_cnt(is_new_node);
        return val;
    }
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    iterator begin() { iterator(get_first_node(), get_first_bucket()); }
    iterator end() { iterator(get_dummy_node(), get_dummy_bucket()); }
private:
    void construct() { mfwu::construct(buckets_, buckets_ + capacity_ + 1, {}); }
    // TODO: let me think, construct dummy bucket first, then construct others with
    //       enable = dummy.enable (which is new bool(f) swhere)
    //       when req_mem, keep the same bool*
    void destroy() { mfwu::destroy(buckets_, buckets_ + capacity_ + 1); }
    void init_dummy_node() {
        buckets_[capacity_].insert(key_type{}, value_type{});
    }
    size_type hash(const key_type& key) const {
        return hashfunc_(key) % capacity_;
    }
    void enable(bool command) {
        buckets_[0].enbale_next_ = command;
    }
    void req_mem() {
        hashtable newtable = hashtable(mfwu::get_next_primer(capacity_ + 1));
        for (size_type idx = 0; idx < capacity_; ++idx) {
            bucket cur = buckets_[idx];
            while (!cur.empty()) {
                bucket::node& node = cur.front();
                newtable.insert(cur->key, cur->value);
                cur.pop();
            }
        }
        newtable.size_ = this->size_;
        *this = mfwu::move(newtable);
    }
    void add_cnt(int num) {
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
        return get_dummy_bucket;
    }
    node* get_first_node() const {
        return get_first_bucket.front();
    }
    bucket& get_dummy_bucket() const {
        return buckets_[capacity_];
    }
    node* get_dummy_node() const {
        return get_dummy_bucket.front();
    }

    size_type capacity_;
    size_type size_;
    bucket* buckets_;
    Hash hashfunc_;
    constexpr static float alpha = 0.7F;

};  // endof class hashtable


}  // endof namespace mfwu


#endif  // __HASHTABLE_HPP__

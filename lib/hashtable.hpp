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
                         void*, mfwu::malloc_alloc>>
class _hashtable_base {
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;

    // we dont consider implement a complete list
    // just usable in hashtable  X-H2 24.08.08
    class _list {
    public:
        struct node {
            key_type key;
            value_type value;
            node* next;
            
            // TODO: i found many class lacks one or more
            // rvalue constructor
            node() : key(), value(), next(nullptr) {}
            node(const key_type& k) : key(k), value(), next(nullptr) {}
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
        };  // endof struct node

        _list() : head_(new node(6)) {}
        _list(const _list& lst) : head_(new node(6)) {
            copy(lst);
        }
        _list(_list&& lst) : head_(lst.head_) {
            lst.head_ = nulltpr;
        }
        ~_list() {
            while (!empty()) { pop(); }
            delete head_;
        }

        _list& operator=(const _list& lst) {
            while (!empty()) { pop(); }
            copy(lst);
            return *this;
        }
        _list& operator=(_list&& lst) {
            head_ = lst.head_;
            lst.head_ = nullptr;
        }

        bool empty() {
            return head_->next == nullptr;
        }
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

        node* push(const key_type& key, const value_type& value) {
            node* ret = search(key);
            if (ret != nullptr) {
                ret->node = value;
                return ret;
            }
            node* next = head_->next;
            node* cur = new node(key, value, next);
            head_->next = cur;
            return cur;
        }
        void pop() {
            node* next = head_->next;
            head_->next = next->next;
            delete next;
        }
        void pop(const key_value& key) {
            node* prev = head_;
            while (prev->next) {
                if (prev->next->key == key) {
                    node* next = prev->next;
                    prev->next = next->next;
                    delete next;
                    return ;
                }
                prev = prev->next;
            }
        }
        value_type& get(const key_type& key) {
            node* ret = search(key);
            if (ret == nullptr) {
                ret = push(key, value_type{});
            }
            return ret->value;
        }
    private:
        void copy(const _list& lst) {
            node* write = head_->next;
            node* read = lst.head_->next;
            while (read) {
                write->next = new node(read);
                write = write->next;
                read = read->next;
            }
        }
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
    };  // endof class _list
    
    
    _hashtable_base() : primer_idx(0), 
        buckets(Alloc::allocate(mfwu::primer_list[primer_idx])) {}
    // _hashtable_base(const std::initialzied_list<mfwu::>& hashed_keys) ?
    _hashtable_base(const _hashtable_base& table) 
        : primer_idx(table.primer_idx), 
          buckets(Alloc::allocate(mfwu::primer_list[primer_idx])) {
        for (size_type idx = 0; idx < mfwu::primer_list[primer_idx]; ++idx) {
            buckets[idx] = table.buckets[idx];
        } 
    }
    _hashtable_base(_hashtable_base&& table)
        : primer_idx(table.primer_idx), buckets(table.buckets) {
            table.buckets = nullptr;
    }

    void insert(const key_type& key, const value_type& val) {
        size_type hashed_key = hash(key);
        buckets[hashed_key].push(key, val);
    }
    void insert(const mfwu::pair<key_type, value_type>& key_val) {
        insert(key_val.first, key_val.second);
    }
    void erase(const key_type& key) {
        size_type hashed_key = hash(key);
        buckets[hashed_key].pop(key);
    }
    value_type& operator[](const key_type& key) {
        size_type hashed_key = hash(key);
        return buckets[hashed_key].get(key);
    }

private:
    size_type hash(const key_type& key) {
        return mfwu::hash_algorithm_pool<key_type>::hash(key)
               % mfwu::primer_list[primer_idx];
    }
    size_type primer_idx;
    mfwu::list<value_type>* buckets;
};  // endof class _hashtable_base

template <typename Key, 
          typename Value, 
          typename Alloc=mfwu::DefaultAllocator<
                         void*, mfwu::malloc_alloc>>
class hashtable {
public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;
    
    hashtable() : table_() {}
    hashtable(const std::initialzied_list<
              mfwu::pair<key_type, value_type>>& kvs) {
        for (auto kv : kvs) { table_.insert(kv); }
    }
    hashtable(const hashtable& table) : table_(table.table_) {}
    hashtable(hashtable&& table) : table_(mfwu::move(table.table_)) {}
    ~hashtable() {}

    // TODO

private:
    _hashtable_base table_;
};  // endof class hashtable


}  // endof namespace mfwu


#endif  // __HASHTABLE_HPP__
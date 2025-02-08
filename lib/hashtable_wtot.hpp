#ifndef __HASHTABLE_WTOT_HPP__
#define __HASHTABLE_WTOT_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "hash_algorithm_pool.hpp"
#include <initializer_list>

namespace mfwu {

template <typename key_type, typename value_type>
class bucket_with_tree {
public:
    struct bucket_node {
        mfwu::pair<const key_type, value_type> value;
        bucket_node* next;

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

    bucket() : head_(new node(/*key_type{主}, 6*/)) {}
    bucket(const bucket& bkt) {
        bucket_copy(bkt);
    }
    bucket(bucket&& bkt) : head_(bkt.head_) {
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
        node* ret = search(key);
        if (ret != nullptr) {
            ret->value.second = value;
            return {ret, false};
        }
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
    }
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
    void bucket_copy(const bucket& bkt) {
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

    node* head_;

};  // endof class bucket

}  // endof namespace mfwu

#endif  // __HASHTABLE_WTOT_HPP__

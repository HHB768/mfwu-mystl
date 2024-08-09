#ifndef __HASHTABLE_HPP__
#define __HASHTABLE_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "pair.hpp"
#include "iterator.hpp"
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
    // TODO: you may try a tree or another hashtable
    // to replace this list
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

        mfwu::pair<node*, bool> push(const key_type& key, const value_type& value) {
            node* ret = search(key);
            if (ret != nullptr) {
                ret->node = value;
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
        bool pop(const key_value& key) {
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
        mfwu::pair<value_type*, bool> get(const key_type& key) {
            node* ret = search(key);
            bool is_new_node = false;
            if (ret == nullptr) {
                ret = push(key, value_type{}).first;
                is_new_node = true;
            }
            return {&(ret->value), is_new_node};
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
    
    _hashtable_base() : primer_idx(0), element_cnt(0),
        buckets(Alloc::allocate(mfwu::primer_list[primer_idx])) {}
    // _hashtable_base(const std::initialzied_list<mfwu::>& hashed_keys) ?
    _hashtable_base(size_type primer_index) : primer_idx(0), element_cnt(0),
        buckets(Alloc::allocate(mfwu::primer_list[primer_idx])) {
        assert(primer_index < 29);
    }
    _hashtable_base(const _hashtable_base& table) 
        : primer_idx(table.primer_idx), element_cnt(0),
          buckets(Alloc::allocate(mfwu::primer_list[primer_idx])) {
        for (size_type idx = 0; idx < mfwu::primer_list[primer_idx]; ++idx) {
            buckets[idx] = table.buckets[idx];
        } 
    }
    _hashtable_base(_hashtable_base&& table)
        : primer_idx(table.primer_idx), element_cnt(table.element_cnt), 
          buckets(table.buckets) {
            table.buckets = nullptr;
    }

    void insert(const key_type& key, const value_type& val) {
        size_type hashed_key = hash(key);
        add_element_cnt(buckets[hashed_key].push(key, val).second);
    }
    void insert(const mfwu::pair<key_type, value_type>& key_val) {
        insert(key_val.first, key_val.second);
    }
    void erase(const key_type& key) {
        size_type hashed_key = hash(key);
        element_cnt -= buckets[hashed_key].pop(key);
    }
    value_type* operator[](const key_type& key) {
        size_type hashed_key = hash(key);
        auto [valptr, is_new_node] = buckets[hashed_key].get(key);
        add_element_cnt(is_new_node);
        return valptr;
    }


private:
    size_type hash(const key_type& key) {
        return mfwu::hash_algorithm_pool<key_type>::hash(key)
               % mfwu::primer_list[primer_idx];
    }
    void req_mem(int incr=1) {
        _hashtable_base newtable = _hashtable_base(primer_idx + incr);
        for (size_type idx = 0; idx < mfwu::primer_list[primer_idx]; ++idx) {
            _list cur_list = buckets[idx];
            while (!cur_list.empty()) {
                _list::node& cur = cur_list.front();
                newtable.insert(cur->key, cur->value);
                cur_list.pop();
            }
        } 
        *this = mfwu::move(newtable);
    }  
    void add_element_cnt(int num) {
        element_cnt += num;
        if ((float)element_cnt / primer_list[primer_idx] > alpha) {  // constexpr
            int incr = 1;
            while ((float)element_cnt / primer_list[primer_idx + incr] > alpha) {
                incr++;
            }
            req_mem(incr);
        }
    }  
    size_type primer_idx;
    size_type element_cnt;
    constexpr static float alpha = 0.7F;
    _list* buckets;
};  // endof class _hashtable_base

template <typename Key, 
          typename Value=mfwu::empty_type, 
          typename Alloc=mfwu::DefaultAllocator<
                         void*, mfwu::malloc_alloc>>
class hashtable {
public:
    using key_type = Key;  // stl: consider Key and const Key!
    using value_type = Value;  // so complicated! cannot bear that...
    using size_type = size_t;

    using list = mfwu::_hashtable_base::_list;
    using node = mfwu::_hashtable_base::_list::node;

    class hashtable_iterator {
    public:
        using value_type = Value;  // or Key in the uset?
        using iterator_category = mfwu::forward_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;

        hashtable_iterator() : buckets_(nullptr), cur_(nullptr) {}
        hashtable_iterator(list* buckets, node* cur)
            : buckets(buckets), cur_(cur) {}
        
        bool operaotr==(const hashtable_iterator& it) const {
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

        // TODO: BAD DESIGN, you shold make node->next
        // can across buckets (legacy)
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
        list* buckets_;
        node* cur_;
    }
    using iterator = hashtable_iterator;
    
    hashtable() : table_() {}
    hashtable(const std::initialzied_list<
              mfwu::pair<key_type, value_type>>& kvs) {
        for (auto kv : kvs) { table_.insert(kv); }
    }
    hashtable(const hashtable& table) : table_(table.table_) {}
    hashtable(hashtable&& table) : table_(mfwu::move(table.table_)) {}
    ~hashtable() {}

    hashtable& operator=(const hashtable& table) {
        table_ = table.table_;
        return *this;
    }
    hashtable& operator=(hashtable&& table) {
        table_ = mfwu::move(table.table_);
        return *this;
    }

    bool empty() const { return table_.empty(); }
    size_type size() const { return table_.size(); }
    iterator begin() const { 
        // i wonder how to define the begin, and more importantly,
        // the end of hashtable, but after i have seen the source
        // code of stl, i am reminded that i need to keep 2 nodes
        // that called "node_before_begin" and "node_after_end"
        return before_first_->next;
    }
    iterator end() const {
        return after_last_;
    }

    void insert(const key_type& key, const value_type& value) {
        table_.insert(key, val);
    }
    void erase(const key_type& key) {
        table_.erase(key);
    }
    value_type& operator[](const key_type& key) {
        return *(table_.operator[](key));
    }  // TODO: what if we have a empty value_type?
private:
    node* before_first_;  // TODO: i think we need to combime hash and its base
                          // at least in an inheritance manner, these 2 nodes
                          // should involve in list building
    node* after_last_;
    _hashtable_base table_;
};  // endof class hashtable


}  // endof namespace mfwu


#endif  // __HASHTABLE_HPP__
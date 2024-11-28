#ifndef __MAP_HPP__
#define __MAP_HPP__

#include "common.hpp"
#include "drbtree.hpp"

namespace mfwu {

class unit_test_map;

template <typename Key, typename Value,
          typename Compare=mfwu::less<Key>>
class map {
public:
    friend class unit_test_map;

    using key_type = Key;
    using mapped_type = Value;
    using value_type = mfwu::pair<Key, Value>;
    using size_type = size_t;

    using rbtr = mfwu::drbtree<key_type, mapped_type, Compare>;
    using node = typename rbtr::rb_node;

    class map_iterator {
    public:
        using iterator_category = mfwu::forward_iterator_tag;
        using value_type = mfwu::pair<Key, Value>;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;
        
        map_iterator() : cur_(nullptr) {}
        map_iterator(node* cur) : cur_(cur) {}
        map_iterator(const map_iterator& it)
            : cur_(it.cur_) {}
        map_iterator(map_iterator&& it)
            : cur_(it.cur_) { it.cur_ = nullptr; }
        ~map_iterator() {}

        map_iterator& operator=(const map_iterator& it) {
            cur_ = it.cur_;
            return *this;
        }
        map_iterator& operator=(map_iterator&& it) {
            cur_ = it.cur_; it.cur_ = nullptr;
            return *this;
        }
        map_iterator& operator++() {
            cur_ = cur_->get_inorder_next();
            return *this;
        }
        map_iterator operator++(int) {
            map_iterator temp = *this;
            this->operator++();
            return temp;
        }

        bool operator==(const map_iterator& it) const {
            return cur_ == it.cur_;
        }
        bool operator!=(const map_iterator& it) const {
            return !(*this == it);
        }

        const value_type& operator*() const {
            return cur_->val;
        }
        const value_type* operator->() const {
            return & this->operator*();
        }

        node* get_cur() const {
            return cur_;
        }

    private:
        node* cur_; 
    };  // endof class map_iterator
    using iterator = map_iterator;

    map() : rbt_() {}
    map(const std::initializer_list<value_type>& vals)
        : rbt_() {
        for (auto&& val : vals) { 
            insert(val); 
        }
    }
    map(const map& m) : rbt_(m.rbt_) {}
    map(map&& m) : rbt_(mfwu::move(m.rbt_)) {}
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    map(InputIterator first, InputIterator last) : rbt_() {
        for ( ; first != last; ++first) {
            insert((*first).first, (*first).second);  // lol
        }
    }
    ~map() {}

    map& operator=(const map& m) {
        rbt_ = m.rbt_;
        return *this;
    }
    map& operator=(map&& m) {
        rbt_ = mfwu::move(m.rbt_);
        return *this;
    }

    /*
    get_allocator
    */
    iterator begin() const {
        if (rbt_.get_tree() == nullptr) {
            return iterator(nullptr);
        }
        node* cur = rbt_.get_tree();
        while (cur->left) {
            cur = cur->left;
        }
        return iterator(cur);
    }
    iterator end() const {
        return iterator(nullptr);
    }

    bool empty() const { return rbt_.empty(); }
    size_type size() const { return rbt_.size(); }
    size_type height() const { return  rbt_.height(); }
    // value_type& root() const { return rbt_.root(); }

    size_type max_size() const { 
        return std::numeric_limits<mfwu::ptrdiff_t>::max();
    }
    void clear() { rbt_.clear(); }  // TODO: O(n) -> ?

    mfwu::pair<iterator, bool> insert(const key_type& key, 
                                      const mapped_type& val) {
        return insert(mfwu::make_pair<key_type, mapped_type>(key, val));
    }
    mfwu::pair<iterator, bool> insert(const value_type& val) {
        node* ret = rbt_.search(val.first);
        if (ret != nullptr) {
            ret->val.second = val.second;
            return {iterator(ret), false};
        }
        ret = rbt_.insert(val);
        return {iterator(ret), true};
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    void insert(InputIterator first, InputIterator last) {
        for (; first != last; ++first) {
            insert(*first);
        }
    }
    template <typename... Args>
    mfwu::pair<iterator, bool> emplace(Args&&... args) {
        return insert(value_type{mfwu::forward<Args>(args)...});
    }
    mapped_type& operator[](const key_type& key) {
        node* ret = search(key);
        if (ret == nullptr) {
            insert(key, value_type{});
            return search(key)->val.second;
        }
        return ret->val.second;
    }
    size_type erase(const key_type& key) {
        node* ret = rbt_.search(key);
        if (ret != nullptr) {
            ret = rbt_.erase(key);
            return 1;
        }
        return 0;
    }
    size_type erase(const value_type& val) {
        return erase(val.first);
    }
    iterator erase(iterator it) {
        node* cur = it.get_cur();
        // node* ret = cur_.get_next();  // may not be right
                                         // bcz ret can be changed
        ++it;
        if (it.get_cur() == nullptr) {
            rbt_.erase(cur);  
            return iterator(nullptr);
        }
        value_type next_val = *it;  // is it a little stupid?
        rbt_.erase(cur);            // but node may change...
        return iterator(rbt_.search(next_val.first));
    }
    // iterator erase(/*range*/)
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    iterator erase(InputIterator first, InputIterator last) {
        typename mfwu::iterator_traits<InputIterator>::difference_type
        num = mfwu::distance(first, last);
        for (int i = 0; i < num; ++i) {
            first = erase(first);
        }
        return first;
    }
    void swap(map& other) {
        mfwu::swap(rbt_, other.rbt_);
    }
    node* extract() const {
        return rbt_.get_tree();
    }
    template <typename Cmp>
    void merge(map<key_type, mapped_type, Cmp>& m) {
        for (auto it = m.begin(); it != m.end(); ) {
            auto [_, suc] = insert(*it);  
            // TODO: for these ordered & tree-like type
            //       maybe we have some faster way to merge? 241108
            if (suc) { it = m.erase(it); 
            continue; }
            ++it;
        }
    }
    size_type count(const key_type& key) const {
        return rbt_.count(key);
    }
    iterator find(const key_type& key) const {
        return iterator(rbt_.search(key));
    }
    bool contains(const key_type& key) const {
        return rbt_.search(key) != nullptr;
    }
    iterator lower_bound(const key_type& key) const {
        return iterator(rbt_.lower_bound(key));
    }
    iterator upper_bound(const key_type& key) const {
        return iterator(rbt_.upper_bound(key));
    }
    mfwu::pair<iterator, iterator> equal_range(const key_type& key) const {
        return {lower_bound(key), upper_bound(key)};
    }
private:
    rbtr rbt_;
};  // endof class map

}  // endof namespace mfwu

#endif  // __MAP_HPP__
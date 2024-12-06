#ifndef __MULTISET_HPP__
#define __MULTISET_HPP__

#include "rbtree_exp.hpp"

namespace mfwu {

class unit_test_multiset;

template <typename T, typename CmpFunctor=mfwu::less<T>>
class multiset {
public:
    friend class unit_test_multiset;
    
    using key_type = T;
    using value_type = T;
    using size_type = size_t;

    // constexpr static bool red = false;
    // constexpr static bool black = true;

    using rbtree = mfwu::rbtree_exp<value_type, CmpFunctor>;
    using node = typename rbtree::rb_node;

    class set_iterator {
    public:
        using iterator_category = mfwu::forward_iterator_tag;
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;

        set_iterator() : cur_(nullptr) {}
        set_iterator(node* cur) : cur_(cur) {}
        set_iterator(const set_iterator& it)
            : cur_(it.cur_) {}
        set_iterator(set_iterator&& it)
            : cur_(it.cur_) { it.cur_ = nullptr; }
        ~set_iterator() {}

        set_iterator& operator=(const set_iterator& it) {
            cur_ = it.cur_;
            return *this;
        }
        set_iterator& operator=(set_iterator&& it) {
            cur_ = it.cur_; it.cur_ = nullptr;
            return *this;
        }
        set_iterator& operator++() {
            cur_ = cur_->get_inorder_next();
            return *this;
        }
        set_iterator operator++(int) {
            set_iterator temp = *this;
            this->operator++();
            return temp;
        }

        bool operator==(const set_iterator& it) const {
            return cur_ == it.cur_;
        }
        bool operator!=(const set_iterator& it) const {
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
    };  // endof class set_iterator
    using iterator = set_iterator;

    multiset() : rbt_() {}
    multiset(const std::initializer_list<value_type>& vals)
        : rbt_() {
        for (auto&& val : vals) { 
            insert(val); 
        }
    }
    multiset(const multiset& s) : rbt_(s.rbt_) {}
    multiset(multiset&& s) : rbt_(mfwu::move(s.rbt_)) {}
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    multiset(InputIterator first, InputIterator last) : rbt_() {
        for ( ; first != last; ++first) {
            insert(*first);
        }
    }
    ~multiset() {}

    multiset& operator=(const multiset& s) {
        rbt_ = s.rbt_;
        return *this;
    }
    multiset& operator=(multiset&& s) {
        rbt_ = mfwu::move(s.rbt_);
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
    const value_type& root() const { return rbt_.root(); }

    size_type max_size() const { 
        return std::numeric_limits<mfwu::ptrdiff_t>::max();
    }
    void clear() { rbt_.clear(); }  // TODO: O(n) -> ?

    mfwu::pair<iterator, bool> insert(const value_type& val) {
        // node* ret = rbt_.search(val);
        // if (ret != nullptr) {
        //     // ret->val = val;  // meaningless in set
        //     return {iterator(ret), false};
        // }
        node* ret = rbt_.insert(val);
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
    // template <class K>
    // mfwu::pair<iterator, bool> insert(K&& x) {
    //     return insert(value_type{mfwu::forward<K>(x)});
    // }
    /*
        i cannot get it, using this function leads to core dump
        and i wonder if it is useful?  24.11.15
    */
    template <typename... Args>
    mfwu::pair<iterator, bool> emplace(Args&&... args) {
        return insert(value_type{mfwu::forward<Args>(args)...});
    }
    size_type erase(const value_type& val) {
        node* ret = rbt_.search(val);
        if (ret != nullptr) {
            rbt_.erase(val);  
            return 1;
        }
        return 0;
    }
    iterator erase(iterator it) {
        node* cur = it.get_cur();
        // node* ret = cur_.get_next();  // may not be right
                                         // bcz ret can be changed
                                         // but now with rbtree_exp
                                         // we can do this  12.4/24
        // ++it;
        // if (it.get_cur() == nullptr) {
        //     rbt_.erase(cur);  
        //     return iterator(nullptr);
        // }
        // value_type next_val = *it;  // is it a little stupid?
        // rbt_.erase(cur);            // but node may change...
        // return iterator(rbt_.search(next_val));
        node* ret = cur->get_inorder_next();
        rbt_.erase(cur);
        return iterator(ret);
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
            // first = erase(*first);  // TODO: how to do this in multiset?
            first = erase(first);      // ANS : like this?  1108/24
        }
        return first;
    }
    void swap(multiset& other) {
        mfwu::swap(rbt_, other.rbt_);
    }
    node* extract() const {
        return rbt_.get_tree();
    }
    template <typename Cmp>
    void merge(multiset<value_type, Cmp>& s) {
        for (auto it = s.begin(); it != s.end(); /* can i place ++it here? */) {
            auto [_, suc] = insert(*it);  
            // TODO: for these ordered & tree-like type
            //       maybe we have some faster way to merge? 241108
            if (suc) { it = s.erase(it); 
            continue; }
            ++it;
        }
    }
    size_type count(const value_type& val) const {
        return rbt_.count(val);
    }
    iterator find(const value_type& val) const {
        return iterator(rbt_.search(val));
    }
    bool contains(const value_type& val) const {
        return rbt_.search(val) != nullptr;
    }
    iterator lower_bound(const value_type& val) const {
        return iterator(rbt_.lower_bound(val));
    }
    iterator upper_bound(const value_type& val) const {
        return iterator(rbt_.upper_bound(val));
    }
    mfwu::pair<iterator, iterator> equal_range(const value_type& val) const {
        return {lower_bound(val), upper_bound(val)};
    }
    // TODO: use key_type explicitly 11.12/24
private:
    rbtree rbt_;
};  // endof class multiset

}  // endof namespace mfwu

#endif  // __MULTISET_HPP__

// TODO: update rbtree til it wnot have value-assgin, but node adjustment only
// ANS: has been finished 241204
#ifndef __SET_HPP__
#define __SET_HPP__

#include "rbtree.hpp"

namespace mfwu {

template <typename T, typename CmpFunctor=mfwu::less<T>>
class set {
public:
    using value_type = T;
    using size_type = size_t;

    constexpr static bool red = false;
    constexpr static bool black = true;

    using rbtree = mfwu::rbtree<T, CmpFunctor>;
    using node = rbtree::rb_node;

    class set_iterator {
    public:
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
            // TODO
        }

    private:
        node* cur_;
    };  // endof class set_iterator
    using iterator = set_iterator;

    set() : rbt_() {}
    set(const std::initializer_list<value_type>& vals)
        : rbt_(vals) {}
    set(const set& s) : rbt_(s.rbt_) {}
    set(set&& s) : rbt_(mfwu::move(s.rbt_)) {}
    ~set() {}

    set& operator=(const set& s) {
        rbt_ = s.rbt_;
        return *this;
    }
    set& operator=(set&& s) {
        rbt_ = mfwu::move(s.rbt_);
        return *this;
    }

    /*
    get_allocator
    begin
    end
    */
    bool empty() const { return rbt_.empty(); }
    size_type size() const { return rbt_.size(); }
    size_type height() const { return  rbt_.height(); }
    value_type& root() const { return rbt_.root(); }
    /*
    max_size
    clear
    */ 
    

    mfwu::pair<iterator, bool> insert(const value_type& val) {
        node* ret = rbt_.search(val);
        if (ret != nullptr) {
            ret->val = val;
            return {iterator(ret), false};
        }
        ret = rbt_.insert(val);
        return {iterator(ret), true};
    }
    // void insert(range)
    // insert(k)
    // emplace
    size_type erase(const value_type& val) {
        node* ret = rbt_.search(val);
        if (ret != nullptr) {
            ret = rbt_.erase(val);
            return 1;
        }
        return 0;
    }
    iterator erase(iterator it) {
        node* cur = it.get_cur();
        // node* ret = rbt_.get_next();  // may not be right
                                      // bcz ret can be changed
        value_type val = rbt_.get_next()->val;
        rbt_.erase(cur);
        return iterator(rbt_.search(val));
    }
    // iterator erase(/*range*/)
    /*
    swap
    extract
    merge
    count
    find
    contains
    equal_range
    lower_bound
    upper_bound
    */
    
private:
    rbtree rbt_;
};  // endof class set

}  // endof namespace mfwu

#endif  // __SET_HPP__
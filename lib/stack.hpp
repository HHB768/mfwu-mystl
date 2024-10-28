#ifndef __STACK_HPP__
#define __STACK_HPP__

#include "deque.hpp"

namespace mfwu {

template <typename T, typename Container=deque<T, 512UL, 
    mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>>
// try other container
class stack {
public:
    using value_type = T;
    using container_type = Container;
    using size_type = size_t;

    stack() : stk_() {}
    stack(size_type n, const value_type& val) : stk_(n, val) {}
    template <typename InputIterator>  // TODO
    stack(InputIterator first, InputIterator last) : stk_(first, last) {}
    stack(const stack& stk) : stk_(stk.stk_) {}
    stack(stack&& stk) : stk(mfwu::move(stk.stk_)) {}
    ~stk() { stk.~deque(); }

    stack& operator=(const stack& stk) {
        stk_ = stk.stk_;
    }
    stack& operator=(stack&& stk) {
        stk_ = mfwu::move(stk.stk_);
    }

    bool empty() {
        return stk_.empty();
    }
    size_type size() {
        return stk_.size();
    }
    value_type& top() {
        return stk_.back();
    }
    void push(const value_type& val) {
        stk_.push_back(val);
    }
    void push(value_type&& val) {
        stk_.push_back(val);
    }
    void pop() {
        stk_.pop_back();
    }
    template <class... Args>
    void emplace(Args&&... args) {
        stk_.emplace_back(args);
    }

    // lexicographically compare the values
private:
    container_type stk_;
};  // endof class stack

}  // endof namespace mfwu

#endif  // __STACK_HPP__
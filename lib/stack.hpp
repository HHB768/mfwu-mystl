#ifndef __STACK_HPP__
#define __STACK_HPP__

#include "deque.hpp"

namespace mfwu {

template <typename T, typename Container=deque<T, 512UL, 
    mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>>
// try other container
class stack {
public:
    friend class unit_test_stack;

    using value_type = T;
    using container_type = Container;
    using size_type = size_t;

    stack() : stk_() {}
    stack(size_type n, const value_type& val) : stk_(n, val) {}
    // template <typename InputIterator,
    //           typename = typename std::enable_if_t<
    //               mfwu::is_input_iterator<InputIterator>::value>
    //          >
    // stack(InputIterator first, InputIterator last) : stk_(first, last) {}
    stack(const std::initializer_list<value_type>& vals) : stk_(vals) {}
    stack(const stack& stk) : stk_(stk.stk_) {}
    stack(stack&& stk) : stk_(mfwu::move(stk.stk_)) {}
    ~stack() {}

    stack& operator=(const stack& stk) {
        stk_ = stk.stk_;
        return *this;
    }
    stack& operator=(stack&& stk) {
        stk_ = mfwu::move(stk.stk_);
        return *this;
    }

    bool empty() const {
        return stk_.empty();
    }
    size_type size() const {
        return stk_.size();
    }
    value_type& top() const {
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
        // stk_.emplace_back(mfwu::forward<Args>(args)...);
        // deque hasnt implemented emplace yet 10.29/24
        push(value_type{mfwu::forward<Args>(args)...});
    }

    // lexicographically compare the values
private:
    container_type stk_;
};  // endof class stack

}  // endof namespace mfwu

#endif  // __STACK_HPP__
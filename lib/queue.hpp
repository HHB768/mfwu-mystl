#ifndef __QUEUE_HPP__
#define __QUEUE_HPP__

#include "deque.hpp"

namespace mfwu {

template <typename T, typename Container=deque<T, 512UL, 
    mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>>
class queue {
public:
    friend class unit_test_queue;
    using value_type = T;
    using container_type = Container;
    using size_type = size_t;

    queue() : q_() {}
    queue(size_type n, const value_type& val) : q_(n, val) {}
    template <typename InputIterator>
    queue(InputIterator first, InputIterator last) : q_(first, last) {}
    queue(const std::initializer_list<value_type>& vals) : q_(vals) {}
    queue(const queue& q) : q_(q) {}
    queue(queue&& q) : q_(mfwu::move(q)) {}
    ~queue() { q_.~container_type(); }

    queue& operator=(const queue& q) {
        q_ = q;
    }
    queue& operator=(queue&& q) {
        q_ = mfwu::move(q);
    }

    bool empty() {
        return q_.empty();
    }
    size_type size() {
        return q_.size();
    }
    value_type& front() {
        return q_.front();
    }
    void push(const value_type& val) {
        q_.push_back(val);
    }
    void push(value_type&& val) {
        q_.push_back(val);
    }
    template <typename... Args>
    void emplace(Args&&... args) {
        q_.emplace_back(args);
    }
    void pop() {
        q_.pop_front();
    }

    // operator<>
    
private:
    container_type q_;
};  // endof class queue

}  // endof namespace mfwu

#endif  // __QUEUE_HPP__
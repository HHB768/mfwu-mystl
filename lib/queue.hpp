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
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    queue(InputIterator first, InputIterator last) : q_(first, last) {}
    queue(const std::initializer_list<value_type>& vals) : q_(vals) {}
    queue(const queue& q) : q_(q.q_) {}
    queue(queue&& q) : q_(mfwu::move(q.q_)) {}
    ~queue() {}

    queue& operator=(const queue& q) {
        q_ = q.q_;
        return *this;
    }
    queue& operator=(queue&& q) {
        q_ = mfwu::move(q.q_);
        return *this;
    }

    bool empty() const {
        return q_.empty();
    }
    size_type size() const {
        return q_.size();
    }
    value_type& front() const {
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
        // q_.emplace_back(mfwu::forward<Args>(args)...);
        // deque hasnt implemented emplace yet 10.29/24
        push(value_type{mfwu::forward<Args>(args)...});
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
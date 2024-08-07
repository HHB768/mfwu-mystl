#ifndef __HEAP_HPP__
#define __HEAP_HPP__

#include "common.hpp"
#include "vector.hpp"
// #include "queue.hpp"

namespace mfwu {
// TODO: without test
// 一个比heap更简单但更低效的计时器
template <typename T, typename CmpFunctor=mfwu::less<T>>
class list_heap {
    list_heap() : lst_() {}
    list_heap(const std::initializer_list<T>& vals) {
        mfwu::vector<T> vec = vals;
        vec.sort();
        lst_ = mfwu::list<T>(vec.begin(), vec.end());
    }
    list_heap(const list_heap& h) : lst_(h.lst_) {}
    list_heap(list_heap&& h) : lst_(mfwu::move(h.lst_)) {}
    ~list_heap() {}

    list_heap& operator=(const list_heap& h) {
        lst_ = h.lst_;
    }
    list_heap& operator=(list_heap&& h) {
        lst_ = mfwu::move(h.lst_);
    }

    bool empty() const {
        return lst_.empty();
    }
    size_t size() const {
        return lst_.size();
    }
    void push(const T& val) {
        auto it = lst_.begin();
        for ( ; it != lst_.end(); it++) {
            if (*it > val) { break; }
        }
        lst_.insert(it, val);
    }
    void pop() { lst_.pop_front(); }
    // TODO
private:
    mfwu::list<T> lst_;
};  // endof class list_heap

class /*::mfwu::*/unit_test_heap;  
// you cannot declare it with path if it is already in the same namespace

template <typename T, typename Container=mfwu::vector<T>, typename CmpFunctor=mfwu::less<T>>
class heap {
public:
    friend class mfwu::unit_test_heap;

    using value_type = typename Container::value_type;
    using size_type = typename Container::size_type;
    using container_type = Container;
    // using iterator = typename Container::iterator;

    heap() : arr_() {}
    // heap(int n, const value_type& val) : arr_(n, val) {}
    heap(const heap& h) : arr_(h.arr_) {}
    heap(heap&& h) : arr_(mfwu::move(h.arr_)) {}
    heap(const std::initializer_list<value_type>& vals) 
        : arr_(vals) { make_heap(); }
    ~heap() {}

    heap& operator=(const heap& h) {
        arr_ = h.arr_;  // reset and copy
        return *this;
    }
    heap& operator=(heap&& h) {
        arr_ = mfwu::move(h.arr_);
        return *this;
    }

    size_type size() const { return arr_.size(); }
    bool empty() const { return arr_.empty(); }
    value_type& top() const { return arr_.front(); }

    void push(const value_type& val) {
        arr_.push_back(val);
        percolate_up(size() - 1);
    }
    void pop() {
        mfwu::swap(arr_.front(), arr_.back());
        arr_.pop_back();
        percolate_down(0);
    }
    
    // heap sort (smaller ---> larger)
    void sort() {
        for (int i = size() - 1; i >= 1; i--) {
            mfwu::swap(arr_[0], arr_[i]);
            adjust(0, i);
        }
    }

    // TODO: C++23, without test
    template <typename InputIterator>
    void push_range(InputIterator first, InputIterator last) {
        arr_.insert(arr_.end(), first, last);
        make_heap();  // TODO
    }

private:
    void make_heap() {
        for (int i = size() / 2 - 1; i >= 0; i--) { adjust(i, size()); }
    }
    // adjust recursively
    void adjust(size_type parent, size_type len) {
        size_type child = parent * 2 + 1;
        if (child >= len) return ;
        if (child + 1 < len && cmp_(arr_[child], arr_[child + 1])) { ++child; }
        if (!cmp_(arr_[child], arr_[parent])) {
            mfwu::swap(arr_[child], arr_[parent]);
            adjust(child, len);
        }
    }
    // default: larger ---> smaller
    void percolate_down(size_type parent) {
        size_type child = parent * 2 + 1;
        if (child >= size()) return ;
        if (child + 1 < size() && cmp_(arr_[child], arr_[child + 1])) { ++child; }
        while (!cmp_(arr_[child], arr_[parent])) {
            mfwu::swap(arr_[child], arr_[parent]);
            parent = child;
            child = parent * 2 + 1;
            if (child >= size()) { break; } 
            if (child + 1 < size() && arr_[child] < arr_[child + 1]) { ++child; }
        }
    }
    void percolate_up(size_type child) {
        if (child == 0) return ;
        size_type parent = (child - 1) / 2;
        while (!cmp_(arr_[child], arr_[parent])) {
            mfwu::swap(arr_[child], arr_[parent]);
            // print_arr_();
            if (parent == 0) return ;
            child = parent;
            parent = (child - 1) / 2;            
        }
    }
    void print_arr_() {
        for (int i=0; i < size(); i++) {
            std::cout << arr_[i] << " ";
        }
        std::cout << "\n";
    }
    Container arr_;
    CmpFunctor cmp_;
};  // endof class heap

}  // endof namespace mfwu


#endif  // __HEAP_HPP__
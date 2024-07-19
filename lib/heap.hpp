#ifndef __HEAP_HPP__
#define __HEAP_HPP__

#include "common.hpp"
#include "vector.hpp"

namespace mfwu {
template <typename T>
struct less {
    bool operator()(const T& a, const T& b) const {
        return a < b;
    }
};  // endof struct less

// TODO: list_heap

template <typename T, typename Container=mfwu::vector<T>, typename ComparingFunc>
class heap {
public:
    using value_type = T;
    using size_type = size_t;
    using iterator = typename Container::iterator;

    heap() : arr() {}
    heap(int n, const value_type& val) : arr(n, val) {}
    heap(const heap& h) : arr(h.arr) {}
    heap(heap&& h) : arr(mfwu::move(h.arr)) {}
    heap(const std::initializer_list<value_type>& vals) : arr(val) {}
    ~heap() {}

    heap& operator=(const heap& h) {
        arr = h.arr;
        return *this;
    }
    heap& operator=(heap&& h) {
        arr = mfwu::move(h.arr);
        return *this;
    }

    size_type size() const { return arr.size(); }
    bool empty() const { return arr.empty(); }
    value_type& top() const { return arr.front(); }

    void push(const value_type& val) {
        arr.push_back(val);
        percolate_up(size() - 1);
    }
    void pop() {
        mfwu::swap(arr.front(), arr.back());
        arr.pop_back();
        percolate_down(0);
    }

private:
    // default: larger ---> smaller
    void percolate_down(size_typen parent) {
        size_type child = parent * 2;
        if (arr[child] < arr[child + 1]) { ++child; }
        while (child > parent) {
            swap(arr[child], arr[parent]);
            parent = child;
            child = parent * 2;
            if (arr[child] < arr[child + 1]) { ++child; }
        }
    }
    void percolate_up(size_type child) {
        size_type parent = child / 2;
        while (child > parent) {
            mfwu::swap(arr[child], arr[parent]);
            child = parent;
            parent = child / 2;
        }
    }
    Container arr;
};  // endof class heap

}  // endof namespace mfwu


#endif  // __HEAP_HPP__
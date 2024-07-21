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

class /*::mfwu::*/unit_test_heap;  
// you cannot declare it with path if it is already in the same namespace

template <typename T, typename Container=mfwu::vector<T>, typename CmpFunctor=mfwu::less<T>>
class heap {
public:
    friend class mfwu::unit_test_heap;

    using value_type = T;
    using size_type = size_t;
    using iterator = typename Container::iterator;

    heap() : arr() {}
    heap(int n, const value_type& val) : arr(n, val) {}
    heap(const heap& h) : arr(h.arr) {}
    heap(heap&& h) : arr(mfwu::move(h.arr)) {}
    heap(const std::initializer_list<value_type>& vals) : arr(vals) {
        for (int i = size() / 2 - 1; i >= 0; i--) {
            adjust(size(), i);
        }
        sort();
    }
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
    // !!!
    void sort() {
        for (int i = size() - 1; i >= 1; i--) {
            mfwu::swap(arr[0], arr[i]);
            adjust(i, 0);
        }
    }

private:
    // !!1
    void adjust(size_type len, size_type parent) {
        size_type child = parent * 2 + 1;
        if (child >= len) return ;
        if (child + 1 < len && arr[child] < arr[child + 1]) { ++child; }
        if (arr[child] > arr[parent]) {
            mfwu::swap(arr[child], arr[parent]);
            adjust(len, child);
        }
    }
    // default: larger ---> smaller
    void percolate_down(size_type parent) {
        size_type child = parent * 2 + 1;
        if (child >= size()) return ;
        if (child + 1 < size() && arr[child] < arr[child + 1]) { ++child; }
        while (arr[child] > arr[parent]) {
            mfwu::swap(arr[child], arr[parent]);
            parent = child;
            child = parent * 2 + 1;
            if (child >= size()) { break; } 
            if (child + 1 < size() && arr[child] < arr[child + 1]) { ++child; }
        }
    }
    void percolate_up(size_type child) {
        if (child == 0) return ;
        size_type parent = (child - 1) / 2;
        while (arr[child] > arr[parent]) {
            mfwu::swap(arr[child], arr[parent]);
            print_arr();
            if (parent == 0) return ;
            child = parent;
            parent = (child - 1) / 2;            
        }
    }
    void print_arr() {
        for (int i=0; i < size(); i++) {
            std::cout << arr[i] << " ";
        }
        std::cout << "\n";
    }
    Container arr;
};  // endof class heap

}  // endof namespace mfwu


#endif  // __HEAP_HPP__
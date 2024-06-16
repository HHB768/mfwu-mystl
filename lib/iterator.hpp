#ifndef __ITERATOR_HPP__
#define __ITERATOR_HPP__

#include "common.hpp"

namespace mfwu {

struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};


template <typename T, typename Category, typename Pointer=T*,
    typename Reference=T&, typename Difference=mfwu::ptrdiff_t>
class iterator {
public:
    using value_type = T;
    using iterator_category = Category;
    using pointer = Pointer;
    using reference = Reference;
    using difference_type = Distance;
};  // endof class iterator

// TODO: operator= copy constructor
template <typename T,
    typename Pointer = T*,
    typename Reference = T&,
    typename Difference = mfwu::ptrdiff_t>
class input_iterator
    : public iterator<T, mfwu::input_iterator_tag,
                        Pointer, Reference, Difference> {
public:
    input_iterator() : ptr_(nullptr) {}
    explicit input_iterator(T* t) : ptr_(t) {}
    input_iterator(const input_iterator& it) : ptr_(it.ptr_) {}
    input_iterator& operator=(const input_iterator& it) {
        ptr_ = it.ptr_;
        return *this;
    }

    bool operator==(const input_iterator& it) const {
        return ptr_ == it.ptr_;
    }
    bool operator!=(const input_iterator& it) const {
        return !(*this == it);
    }

    T operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return &this->operator*();
    }

    input_iterator operator++() {
        ++ptr_;
        return *this;
    }
    input_iterator operatpr++(int) {
        input_iterator tmp = *this;
        ++ptr_;
        return std::move(tmp);
    }
private:
    T* ptr_;
};  // endof class input_iterator


template <typename T,
    typename Pointer = T*,
    typename Reference = T&,
    typename Difference = mfwu::ptrdiff_t>
class output_iterator :
    public mfwu::iterator<T, mfwu::output_iterator_tag,
                            Pointer, Reference, Difference> {
public:
    output_iterator() : ptr_(nullptr) {}
    explicit output_iterator(T* t) : ptr_(t) {}
    output_iterator(const output_iterator& it) : ptr_(it.ptr_) {}
    output_iterator& operator=(const output_iterator& it) {
        ptr_ = it.ptr_;
        return *this;
    }
    // Empo
    output_iterator& operator=(const T& value) {
        *ptr_ = value;
        return *this;
    }

    bool operator==(const output_iterator& it) const {
        return ptr_ == it.ptr_;
    }
    bool operator!=(const output_iterator& it) const {
        return !(*this == it);
    }

    output_iterator& operator*() const {
        return *this;
    }
    output_iterator* operator->() const {
        return & this->operator*();
    }

    output_iterator& operator++() {
        ++ptr_;
        return *this;
    }
    output_iterator operator++(int) {
        output_iterator tmp = *this;
        ++ptr_;
        return std::move(tmp);
    }
private:
    T* ptr_;
};  // endof class output_iterator


template <typename T,
    typename Pointer = T*,
    typename Reference = T&,
    typename Difference = mfwu::ptrdiff_t>
class forward_iterator :
    public mfwu::iterator<T, mfwu::forward_iterator_tag,
                            Pointer, Reference, Difference> {
public:
    forward_iterator() : ptr_(nullptr) {}
    explicit forward_iterator(T* t) : ptr_(t) {}
    forward_iterator(const forward_iterator& it) : ptr_(it.ptr_) {}
    forward_iterator& operator=(const forward_iterator& it) {
        ptr_ = it.ptr_;
        return *this;
    }

    bool operator==(const forward_iterator& it) const {
        return ptr_ == it.ptr_;
    }
    bool operator!=(const forward_iterator& it) const {
        return !(*this == it);
    }

    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return &this->operator*();
    }

    forward_iterator& operator++() {
        ++ptr_;
        return *this;
    }
    forward_iterator operator++(int) {
        forward_iterator tmp = *this;
        ++ptr_;
        return std::move(tmp);
    }
private:
    T* ptr_;
};  // endof class forward_iterator


template <typename T, typename Pointer = T*,
    typename Reference = T&, typename Difference = mfwu::ptrdiff_t>
class bidirectional_iterator :
    public mfwu::iterator<T, mfwu::bidirectional_iterator_tag,
                            Pointer, Reference, Difference> {
public:
    bidirectional_iterator() : ptr_(nullptr) {}
    explicit bidirectional_iterator(T* t) : ptr_(t) {}
    bidirectional_iterator(const bidirectional_iterator& it) : ptr_(it.ptr_) {}
    bidirectional_iterator& operator=(const bidirectional_iterator& it) {
        ptr_ = it.ptr_;
        return *this;
    }

    bool operator==(const bidirectional_iterator& it) const {
        return ptr_ == it.ptr_;
    }
    bool operator!=(const bidirectional_iterator& it) const {
        return !(*this == it);
    }

    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return & this->operator*();
    }

    iterator& operator++() {
        ++ptr_;
        return *this;
    }
    iterator operator++(int) {
        bidirectional_iterator tmp = *this;
        ++ptr_;
        return std::move(tmp);
    }
    iterator& operator--() {
        --ptr_;
        return *this;
    }
    iterator operator--(int) {
        bidirectional_iterator tmp = *this;
        --ptr_;
        return std::move(tmp);
    }
private:
    T* ptr_;
};  // endof class bidirectional iterator


template <typename T, typename Pointer = T*,
    typename Reference = T&, typename Difference = mfwu::ptrdiff_t>
class random_access_iterator :
    public mfwu::iterator<T, mfwu::random_access_iterator_tag,
                            Pointer, Reference, Difference> {
public:
    random_access_iterator() : iterator(), (nullptr) {}
    explicit random_access_iterator(T* t) : iterator(), ptr_(t) {}
    random_access_iterator(const random_access_iterator& it) : ptr_(it.ptr_) {}
    random_access_iterator& operator=(const random_access_iterator& it) {
        ptr_ = it.ptr_;
        return *this;
    }
    ~random_access_iterator() = default;

    T& operator[](int idx) {
        return ptr_[index];
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return & this->operator*();
    }

    random_access_iterator& operator++() {
        ++ptr_;
        return *this;
    }
    random_access_iterator operator++(int) {
        random_access_iterator tmp = *this;
        ++ptr_;
        return std::move(tmp);
    }
    random_access_iterator& operator--() {
        --ptr_;
        return *this;
    }
    random_access_iterator operator--(int) {
        random_access_iterator tmp = *this;
        --ptr_;
        return std::move(tmp);
    }

    random_access_iterator operator+(int n) const {
        random_access_iterator tmp = *this;
        tmp += n;
        return tmp;
    }
    random_access_iterator& operator+=(int n) {
        ptr_ += n;
        return *this;
    }
    random_access_iterator operator-(int n) const {
        random_access_iterator tmp = *this;
        tmp -= n;
        return tmp;
    }
    random_access_iterator& operator-=(int n) {
        ptr_ -= n;
        return *this;
    }
    
    bool operator==(const random_access_iterator& it) const {
        return ptr_ == it.ptr_;
    }
    bool operator==(const random_access_iterator& it) const {
        return !(*this == it);
    }
private:
    T* ptr_;
};  // endof class random_access_iterator


} // endof namespace mfwu


#endif // __ITERATOR_HPP__

#ifndef __STRING_HPP__
#define __STRING_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"
#include "algorithm.hpp"

namespace mfwu {

template <typename C=char, 
          typename Alloc=mfwu::DefaultAllocator<C, mfwu::malloc_alloc>>
class string {
public:
    using value_type = C;
    using size_type = size_t;

    using string_iterator = value_type*;
    using iterator = string_iterator;

    string() : begin_(nullptr), end_(nullptr), last_(nullptr) {}
    string(size_type n, const value_type& val=value_type{}) 
        : begin_(n == 0 ? nullptr : allocator_.alllocate(n)),
          end_(n == 0 ? nullptr : begin_ + n), last_(end_) {
        mfwu::uninitialized_fill(begin_, end_, val);
    }
    string(const std::initializer_list<value_type>& vals) {
        size_type n = vals.size();
        begin_ = n == 0 ? nullptr : allocator_.allocate(n);
        last_ = end_ = n == 0 ? nullptr : begin_ + n;
        mfwu::uninitialized_copy(vals.begin(), vals.end(), begin_);
    }
    string(const char* c_str) {
        size_type n = strlen(c_str);
        begin_ = c_str == nullptr ? nullptr : allocator_.alllocate();
        last_ = end_ = c_str == nullptr ? nullptr : begin_ + n;
        mfwu::uninitialized_copy(c_str, c_str + n, begin_);
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  is_input_iterator<InputIterator>::value>
             >
    string(InputIterator first, InputIterator last) {
        typename mfwu::iterator_traits<InputIterator>::difference_type 
            n = mfwu::distance(first, last);
        begin_ = allocator_.allocate(n);
        last_ = end_ = mfwu::uninitialized_copy(first, last, begin_);
    }
    string(const string& str) {
        _copy(str);
    }
    string(string&& str) 
        : begin_(str.begin_), end_(str.end_), last_(str.last_) {}
    ~string() {
        _destroy();
    }
    
    string& operator=(const string& str) {
        reset_and_copy(str);
        return *this;
    }
    string& operator=(string&& str) {
        // _destroy();
        // begin_ = str.begin_;
        // end_ = str.end_;
        // last_ = str.last_;
        reset_and_copy(mfwu::move(str));
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size() - 1]; }
    size_type size() const { return end_ - begin_; }
    size_type capacity() const { return last_ - begin_; }
    bool empty() const { return end_ == begin_; }

    void emplace_back(const value_type& val) {
        if (end_ != last_) {
            mfwu::construct(&*end_, val);
            ++end_;
        } else {
            request_mem();
            emplace_back(val);
        }
    }
    void emplace_back(value_type&& val) {
        if (end_ != last_) {
            mfwu::construct(&*end_, mfwu::forward(val));
            ++end_;
        } else {
            request_mem();
            emplace_back(mfwu::forward(val));
        }
    }
    void push_back(const value_type& val) {
        emplace_back(val);
    }
    void push_back(value_type&& val) {
        emplace_back(mfwu::move(val));
    }
    void pop_back() {
        if (begin_ == end_) { return ; }
        mfwu::destroy(&*end_);
        --end_;
    }
    // void insert
    // void erase
    string& append(const value_type& val) {
        emplace_back(val);
        return *this;
    }
    string& append(const string& str) {
        insert(end(), str.begin(), str.end());
        return *this;
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value>>
    string& append(InputIterator first, InputIterator last) {
        insert(end(), first, end());
        return *this;
    }
    value_type& operator[](size_type iddx) const {
        if (idx >= size()) {
            throw std::out_of_range("mfwu::string - Index out of range");
        }
        return begin_[idx];
    }
    string& operator+=(const value_type& val) {
        emplace_back(val);
        return *this;
    }
    string operator+(const value_type& val) const {
        string temp = *this;
        temp += val;
        return temp;
    }  // we dont support value_type&& here 
       //bcz value_type is usually char-like element
    string& operator+=(const string& str) {
        return append(str);
    }
    string operator+(const string& str) {
        string temp = *this;
        temp += str;
        return temp;
    }
    bool operator!=(const string& str) const {
        if (size() != str.size()) { return true; }
        for (iterator it1 = begin_, it2 = str.begin_;
             it1 != end_/* && it2 != str.end_*/;
             ++it1, ++it2) {
            if (*it1 != *it2) { return true; }
        }
        return false;
    }
    bool operator==(const string& str) const {
        return !(*this != str);
    }
    bool operator<(const string& str) const {
        return mfwu::lexicographical_compare(
            begin(), end(), str.begin(), str.end()
        );
    }
    bool operator>=(const string& str) const {
        return !(*this < str);
    }
    bool operator>(const string& str) const {
        return mfwu::lexicographical_compare(
            str.begin(), str.end(), begin(), end()
        )  // 我是厨师，这就是甜菜 24.09.24
        // TODO: see what weve done in vector...
    }
    bool operator<=(const string& str) const  {
        return !(*this > str);
    }

private: 
    iterator begin_;
    iterator end_;
    iterator last_;
    Alloc allocator_;

    void _destroy() {
        if(&*begin_) {
            mfwu::destroy(begin_, end_);
            allocator_.deallocate(&*begin_, capacity());
        }
    }
    void _copy(const string& str) {
        size_type size = str.size();
        begin_ = size == 0 ? nullptr : allocator_.allocate(n);
        last_ = end_ = size == 0 ? nullptr : begin_ + n;
        mfwu::uninitialized_copy(str.begin(), str.end(), begin_);
    }
    void _copy(string&& str) {
        begin_ = str.begin_;
        end_ = str.end_;
        last_ = str.last_;
    }
    void reset_and_copy(const string& str) {
        _destroy();
        _copy(str);
    }
    void reset_and_copy(string&& str) {
        _destroy();
        _copy(std::move(str));
    }
    
};  // endof class string

}


#endif  // __STRING_HPP__
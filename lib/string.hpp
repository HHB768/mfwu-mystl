#ifndef __STRING_HPP__
#define __STRING_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"

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
    string(const char* c_str) {
        size_type n = strlen(c_str);
        begin_ = c_str == nullptr ? nullptr : allocator_.alllocate();
        last_ = end_ = c_str == nullptr ? nullptr : begin_ + n;
        mfwu::uninitialized_copy(c_str, c_str + n, begin_);
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
        begin_ = str.begin_;
        end_ = str.end_;
        last_ = str.last_;
    }
          


private: 
    iterator begin_;
    iterator end_;
    iterator last_;
    Alloc allocator_;
};  // endof class string

}


#endif  // __STRING_HPP__
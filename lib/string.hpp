#ifndef __STRING_HPP__
#define __STRING_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"
#include "algorithm.hpp"

namespace mfwu {

// TODO: how about a string implementation
//       based on deque/vector? 0927

// try to implement a basic_string

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
    string(const string& str, size_type start_idx=0) {
        _copy(str, start_idx);
    }
    string(const string& str, size_type start_idx, size_type len) {
        _copy(str, start_idx, len);
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
        return *this;
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size() - 1]; }
    size_type size() const { return end_ - begin_; }
    size_type length() const { return size(); }
    size_type capacity() const { return last_ - begin_; }
    bool empty() const { return end_ == begin_; }

    // we dont support value_type(Args...) here
    // bcz value_type is usually char
    void emplace_back(const value_type& val) {
        if (end_ != last_) {
            mfwu::construct(&*end_, val);
            ++end_;
        } else {
            req_mem();
            emplace_back(val);
        }
    }
    void emplace_back(value_type&& val) {
        if (end_ != last_) {
            mfwu::construct(&*end_, mfwu::forward(val));
            ++end_;
        } else {
            req_mem();
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

    void insert(size_type idx, const value_type& val) {
        insert(begin_ + idx, val);
    }
    void insert(iterator it, const value_type& val) {
        if (end_ < last_) {
            mfwu::construct(&*end_, back());
            mfwu::reverse_copy(it, end_, it + 1);
            ++end_;
            *it = val;
        } else {
            typename mfwu::iterator_traits<iterator>::difference_type 
            idx = it - begin_;
            req_mem();
            insert(begin_ + idx, val);
        }
    }
    template <typename InputIterator>
    void insert(size_type idx, InputIterator first, InputIterator last) {
        insert(begin_ + idx, first, last);
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        typename mfwu::iterator_traits<InputIterator>::difference_type 
        // n = last - first;
        n = mfwu::distance(first, last);
        if (end_ + n <= last_) {
            if (it + n > end_) {
                mfwu::uninitialized_copy(it, end_, it + n);
                mfwu::copy(first, first + (end_ - it), it);
                mfwu::uninitialized_copy(first + (end_ - it), last, end_);
            } else {
                mfwu::uninitialized_copy(end_ - n, end_, end_);
                mfwu::reverse_copy(it, end_ - n, it + n);
                mfwu::copy(first, last, it);
            }
            end_ += n;  // TODO: check
        } else {
            typename mfwu::iterator_traits<InputIterator>::difference_type 
            idx = it - begin_;
            req_mem();
            insert(begin_ + idx, first, last);
        }
    }
    // void erase
    void erase(size_type idx) {
        erase(begin_ + idx);
    }
    void erase(iterator it) {
        mfwu::copy(it + 1, end_, it);
        --end_;
        mfwu::destroy(&*end_);
    }
    void erase(iterator first, iterator last) {
        typename mfwu::iterator_traits<iterator>::difference_type
        n = last - first;
        mfwu::copy(last, end_, first);
        mfwu::destroy(first + (end_ - last), end_);
        end_ -= n;
    }
    void erase(size_type start_idx, size_type end_idx) {
        erase(begin_ + start_idx, begin_ + end_idx);
    }
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
        insert(end(), first, last);
        return *this;
    }

    void clear() {
        mfwu::destroy(begin_, end_);
        end_ = begin_;
    }
    void replace(const value_type& val, const value_type& new_val) {
        mfwu::replace(begin_, end_, val, new_val);
    }
    void reserve(size_type cap) {
        if (cap <= capacity()) return ;
        value_type* start = allocator_.reallocate(&*begin_, capacity(), cap);
        if (start == nullptr) return ;
        if (start == &*begin_) { last_ = begin_ + cap; return ; }
        mfwu::uninitialized_copy(begin_, end_, start);
        size_type sz = size();
        begin_ = start; end_ = begin_ + sz; last_ = begin_ + cap;
    }
    string copy() const {
        return string(*this);
    }
    value_type* c_str() const {
        value_type* cstr = new value_type(size() + 1);
        value_type* cstr_end = uninitialized_copy(begin_, end_, cstr);
        mfwu::construct(cstr_end, '\0');
        return cstr;
    }
    // is it const ?
    // value_type[] data() const {
    //     return this->c_str();
    // }
    string substr(size_type start_idx, size_type len) const {
        return string(*this, start_idx, len);
    }

    value_type& at(size_type idx) const {
        if (idx >= size()) {
            throw std::out_of_range("mfwu::string - Index out of range");
        }
        return begin_[idx];
    }  // TODO: do this in vector.hpp 0930
    value_type& operator[](size_type idx) const {
        return begin_[idx];
    }
    string& operator+=(const value_type& val) {
        append(val);
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
        // TODO: use mfwu::equal
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
        );
        // 我是厨师，这就是甜菜 24.09.24
        // TODO: see what weve done in vector...
        // lol 1219/24
    }
    bool operator<=(const string& str) const  {
        return !(*this > str);
    }

    // operator<<>>

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
    void _copy(const string& str, size_type start_idx=0) {
        // TODO: what if start_idx > str.size()?
        size_type size = str.size() - start_idx;
        begin_ = size == 0 ? nullptr : allocator_.allocate(size);
        last_ = end_ = size == 0 ? nullptr : begin_ + size;
        mfwu::uninitialized_copy(str.begin() + start_idx, str.end(), begin_);
    }
    void _copy(const string& str, size_type start_idx, size_type len) {
        size_type size = mfwu::min(str.size() - start_idx, len);
        begin_ = size == 0 ? nullptr : allocator_.allocate(size);
        last_ = end_ = size == 0 ? nullptr : begin_ + size;
        mfwu::uninitialized_copy_n(str.begin() + start_idx, size, begin_);
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
    void req_mem() {
        size_type capacity = this->capacity();
        size_type new_capacity = capacity ? 2 * capacity : 1;
        
        iterator new_begin = allocator_.allocate(new_capacity);
        end_ = mfwu::uninitialized_copy(begin_, end_, new_begin);
        allocator_.deallocate(begin_, capacity);
        begin_ = new_begin;
        last_ = new_begin + new_capacity;
    }
    // TODO: private funcs with commonly-used name
    //       should start with '_'
    
};  // endof class string


template <typename C=char, size_t BuffSize=8, 
          typename Alloc=mfwu::DefaultAllocator<C, mfwu::malloc_alloc>>
class tiny_string {
public:
    using value_type = C;
    using size_type = size_t;
    using iterator = value_type*;

    tiny_string() : begin_(&buf_), end_(&buf_), 
                    last_(&buf_ + BuffSize), is_tiny_(true) {}  
                    // = empty_init()
    tiny_string(size_type n, const value_type& val=value_type{}) {
        init_iterator(n);
        if (is_tiny_) {
            mfwu::fill(begin_, end_, val);
        } else {
            mfwu::uninitialized_fill(begin_, end_, val);
        }
    }
    tiny_string(const std::initializer_list<value_type>& vals) {
        init_iterator(vals.size());
        copy_init(vals.begin(), vals.end());
    }
    tiny_string(const char* c_str) {
        size_type n = strlen(c_str);
        init_iterator(n);
        copy_init(c_str, c_str + n);
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  is_input_iterator<InputIterator>::value>
             >
    tiny_string(InputIterator first, InputIterator last) {
        init_iterator(mfwu::distance(first, last));
        copy_init(first, last);
    }
    tiny_string(const tiny_string& str, size_type start_idx=0) {
        if (str.size() < start_idx) {
            empty_init(); return ;
        }
        init_iterator(str.size() - start_idx);
        copy_init(str.begin_ + start_idx, str.end_);
    }
    tiny_string(const tiny_string& str, size_type start_idx, size_type len) {
        if (str.size() < start_idx) {
            empty_init(); return ;
        }
        size_type sz = mfwu::min(str.size() - start_idx, len);
        init_iterator(sz);
        copy_init_n(str.begin_ + start_idx, sz);
    }
    tiny_string(tiny_string&& str) 
        : begin_(str.begin_), end_(str.end_),
          last_(str.last_), is_tiny_(str.is_tiny_) {}
    // TODO: implement tiny_string(tiny_string&&, size_type, size_type)
    // TODO: implement tiny_string(string) and string(tiny_string)
    ~tiny_string() {
        _destroy();
    }

    tiny_string& operator=(const tiny_string& str) {
        reset_and_copy(str);
        return *this;
    }
    tiny_string& operator=(tiny_string&& str) {
        reset_and_copy(mfwu::move(str));
        return *this;
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size() - 1]; }
    size_type size() const { return end_ - begin_; }
    size_type length() const { return size(); }
    size_type capacity() const { return last_ - begin_; }
    bool empty() const { return end_ == begin_; }

    void emplace_back(const value_type& val) {
        if (end_ != last_) {
            if (is_tiny_) {
                *end_ = val;
            } else {
                mfwu::construct(&*end_, val);
            }
            ++end_;
        } else {
            req_mem();
            emplace_back(val);
        } 
    }
    void emplace_back(value_type&& val) {
        if (end_ != last_) {
            if (is_tiny_) {
                *end_ = mfwu::move(val);
            } else {
                mfwu::construct(&*end_, mfwu::move(val));
            }
            ++end_;
        } else {
            req_mem();
            emplace_back(val);
        } 
    }
    void push_back(const value_type& val) {
        emplace_back(val);\
    }
    void push_back(value_type&& val) {
        emplace_back(mfwu::move(val));
    }
    void pop_back() {
        if (begin_ == end_) { return ; }
        if (is_tiny_ == false) {
            mfwu::destroy(&*end_);
        }
        --end_;
    }

    void insert(size_type idx, const value_type& val) {
        insert(begin_ + idx, val);
    }
    void insert(iterator it, const value_type& val) {
        if (end_ < last_) {
            if (is_tiny_ == false) {
                mfwu::construct(&*end_, back());
            }
            mfwu::reverse_copy(it, end_, it + 1);
            *it = val;
            ++end_;
        } else {
            typename mfwu::iterator_traits<iterator>::difference_type
            idx = it - begin_;
            req_mem();
            insert(begin_ + idx, val);
        }
    }
    template <typename InputIterator>
    void insert(size_type idx, InputIterator first, InputIterator last) {
        insert(begin_ + idx, first, last);
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        typename mfwu::iterator_traits<InputIterator>::difference_type
        n = mfwu::distance(first, last);
        if (end_ + n <= last_) {
            if (is_tiny_) {
                mfwu::reverse_copy(it, end_, it + n);
                mfwu::copy(first, last, it);
            } else {
                if (it + n > end_) {
                    mfwu::uninitialized_copy(it, end_, it + n);
                    mfwu::copy(first, first + (end_ - it), it);
                    mfwu::uninitialized_copy(first + (end_ - it), last, end_);
                } else {
                    mfwu::uninitialized_copy(end_ - n, end_, end_);
                    mfwu::reverse_copy(it, end_ - n, it + n);
                    mfwu::copy(first, last, it);
                }
            }
            end_ += n;
        } else {
            typename mfwu::iterator_traits<InputIterator>::difference_type
            idx = it - begin_;
            req_mem();
            insert(begin_ + idx, first, last);
        }
    }

    void erase(size_type idx) {
        erase(begin_ + idx);
    }
    void erase(iterator it) {
        mfwu::copy(it + 1, end_, it);
        --end_;
        if (is_tiny_ == false) {
            mfwu::destroy(&*end_);
        }
    }
    void erase(iterator first, iterator last) {
        typename mfwu::iterator_traits<iterator>::difference_type
        n = last - first;
        mfwu::copy(last, end_, first);
        if (is_tiny_ == false) {
            mfwu::destroy(first + (end_ - last), end_);
        }
        end_ -= n;
    }
    void erase(size_type start_idx, size_type end_idx) {
        erase(begin_ + start_idx, begin_ + end_idx);
    }

    tiny_string& append(const value_type& val) {
        emplace_back(val);
        return *this;
    }
    template <size_type Bufsz>
    tiny_string& append(const tiny_string<C, Bufsz, Alloc>& str) {
        insert(end_, str.begin_, str.end_);
        return *this;
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                   mfwu::is_input_iterator<InputIterator>::value>>
    tiny_string& append(InputIterator first, InputIterator last) {
        insert(end_, first, last);
        return *this;
    }

    void clear() {
        if (is_tiny_ == false) {
            mfwu::destroy(begin_, end_);
        }
        end_ = begin_;
    }
    void replace(const value_type& val, const value_type& new_val) {
        mfwu::replace(begin_, end_, val, new_val);
    }
    void reserve(size_type cap) {
        if (cap < capacity()) return ;
        if (is_tiny_) {
            iterator ori_begin = begin_;
            iterator ori_end = end_;
            init_iterator(cap);
            copy_init(ori_begin, ori_end);
        } else {
            iterator start = Alloc::reallocate(&*begin_, capacity(), cap);
            if (start == nullptr) return ;
            if (start == &*begin_) { last_ = begin_ + cap; return ; }
            mfwu::uninitialized_copy(begin_, end_, start);
            size_type sz = size();
            begin_ = start; end_ = begin_ + sz; last_ = begin_ + cap;
        }
    }
    tiny_string copy() const {
        return tiny_string(*this);
    }
    const value_type* c_str() const {
        const value_type* cstr = new value_type(size() + 1);
        const value_type* cstr_end = uninitialized_copy(begin_, end_, cstr);
        mfwu::construct(cstr_end, '\0');
        return cstr;
    }
    value_type* data() const {
        return begin_;
    }
    tiny_string substr(size_type start_idx, size_type len) const {
        return tiny_string(*this, start_idx, len);
    }

    value_type& at(size_type idx) const {
        if (idx >= size()) {
            throw std::out_of_range(
                "mfwu::tiny_string - Index out of range");
        }
        return begin_[idx];
    }
    value_type& operator[](size_type idx) const {
        return begin_[idx];
    }

    tiny_string& operator+=(const value_type& val) {
        append(val);
    }
    tiny_string operator+(const value_type& val) const {
        string temp = *this;
        temp += val;
        return temp;
    }
    template <size_t Bufsz>
    tiny_string& operator+=(const tiny_string<C, Bufsz, Alloc>& str) {
        return append(str);
    }
    template <size_t Bufsz>
    tiny_string& operator+(const tiny_string<C, Bufsz, Alloc>& str) {
        tiny_string temp = *this;
        temp += str;
        return temp;
    }

    bool operator==(const tiny_string& str) const {
        return mfwu::equal(begin_, end_, str.begin_);
    }
    bool operator!=(const tiny_string& str) const {
        return !(*this == str);
    }
    bool operator<(const tiny_string& str) const {
        return mfwu::lexicographical_compare(
            begin_, end_, str.begin_, str.end_
        );
    }
    bool operator>=(const tiny_string& str) const {
        return !(*this < str);
    }
    bool operator>(const tiny_string& str) const {
        return mfwu::lexicographical_compare(
            str.begin_, str.end_, begin_, end_
        );
    }
    bool operator<=(const tiny_string& str) const {
        return !(*this > str);
    }

private:
    bool init_iterator(size_type n) {
        if (n > BuffSize) {
            is_tiny_ = false;
            begin_ = Alloc::allocate(n);
            last_ = begin_ + n;
        } else {
            is_tiny_ = true;
            begin_ = &buf_;
            last_ = &buf_ + BuffSize;
        }
        end_ = begin_ + n;
        return is_tiny_;
    }
    void empty_init() {
        begin_ = &buf_;
        end_ = &buf_;
        last_ = &buf_ + BuffSize;
        is_tiny_ = true;
    }
    template <typename InputIterator>
    void copy_init(InputIterator first, InputIterator last) {
        if (is_tiny_) {
            mfwu::copy(first, last, begin_);
        } else {
            mfwu::uninitialized_copy(first, last, begin_);
        }
    }
    template <typename InputIterator>
    void copy_init_n(InputIterator first, size_type n) {
        if (is_tiny_) {
            mfwu::copy_n(first, n, begin_);
        } else {
            mfwu::uninitialized_copy_n(first, n, begin_);
        }
    }
    void _destroy() {
        if (is_tiny_ == false) {
            mfwu::destroy(begin_, end_);
            Alloc::deallocate(mfwu::distance(begin_, last_));
        }
    }
    void req_mem() {
        size_type capacity = this->capacity();
        size_type new_capacity = capacity ? 2 * capacity : 1;
        
        iterator new_begin = Alloc::allocate(new_capacity);
        end_ = mfwu::uninitialized_copy(begin_, end_, new_begin);
        if (is_tiny_ == false) {
            Alloc::deallocate(begin_, capacity);
        }
        begin_ = new_begin;
        last_ = new_begin + new_capacity;
        is_tiny_ = true;  // Check
    }
    void _switch(bool toHeapMem=true, size_type n=2*BuffSize) {
        if (toHeapMem) {
            is_tiny_ = false;
            begin_ = Alloc::allocate(n);
            end_ = last_ = begin_ + n;
            end_ = mfwu::copy(buf_, end_, begin_);
        } else {
            if (is_tiny_) return ;
            if (size() > BuffSize) return ;
            mfwu::copy(begin_, end_, buf_);
            begin_ = buf_;
            end_ = buf_ + size();
            last_ = begin_ + BuffSize;
            Alloc::deallocate(buf_, size());
        }
        // check
    }

    iterator begin_, end_, last_;
    bool is_tiny_;
    value_type buf_[BuffSize] = {};
};  // endof class tiny_string

}


#endif  // __STRING_HPP__
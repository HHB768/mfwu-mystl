#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {

    class unit_test_vector;

    template <typename T, typename Alloc = mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
    class vector {
    public:
        friend class mfwu::unit_test_vector;

        class vector_iterator
            : public mfwu::iterator<T, mfwu::random_access_iterator_tag> {
        public:
            using value_type = T;
            using iterator_category = mfwu::random_access_iterator_tag;
            using pointer = T*;
            using reference = T&;
            using difference_type = mfwu::ptrdiff_t;  // already in mfwu::iterator
                                                      // how to use them ? TODO
            using size_type = size_t;

            // vector_iterators don't new and free the pointer
            vector_iterator() : ptr_(nullptr) {}
            /*explicit*/ vector_iterator(pointer p) : ptr_(p) {}
            /*explicit*/ vector_iterator(const vector_iterator& it) : ptr_(it.ptr_) {}
            // you should not use explicit qualifier before copy constructor
            // bcz it will force you to use static_cast<vector_iterator>(it) everywhere
            // even it is already a instance of vector_iterator T^T
            // explicit vector_iterator(const vector_iterator&& it) : ptr_(it.ptr_) { it.ptr_ = nullptr; }
            vector_iterator& operator=(const vector_iterator& it) {
                ptr_ = it.ptr_;
                return *this;
            }
            /* iterators dont manage the memory, the ptrs do */ 
            ~vector_iterator() = default; 

            reference operator[](int idx) = delete;
            /*
            crazy! i have used this one:
            reference operator[](int idx) = delete {  // delete
                return ptr_[idx];
            }
            but it is illegal, even a redundant scope {} can make
            the following declarations inaccessible, try:
            */
            // {}
            vector_iterator& operator++() {
                ++ptr_;
                return *this;
            }
            vector_iterator operator++(int) {
                vector_iterator tmp = *this;
                ++ptr_;
                return tmp;
            }
            vector_iterator& operator--() {
                --ptr_;
                return *this;
            }
            vector_iterator operator--(int) {
                vector_iterator tmp = *this;
                --ptr_;
                return tmp;
            }
            vector_iterator& operator+=(int n) {
                ptr_ += n;
                return *this;
            }
            vector_iterator operator+(int n) {
                vector_iterator tmp = *this;
                tmp += n;
                return tmp;
            }
            vector_iterator& operator-=(int n) {
                ptr_ -= n;
                return *this;
            }
            vector_iterator operator-(int n) {
                vector_iterator tmp = *this;
                tmp -= n;
                return *this;
            }
            difference_type operator-(const vector_iterator& it) const {
                return mfwu::distance(it, *this);
            }
            bool operator==(const vector_iterator& it) const {
                return ptr_ == it.ptr_;
            }
            bool operator!=(const vector_iterator& it) const {
                return !(*this == it);
            }
            reference operator*() const {
                return *ptr_;
            }
            pointer operator->() const {
                return &this->operator*();
            }
            bool operator<(const vector_iterator& it) const {
                return ptr_ < it.ptr_;
            }
            bool operator>=(const vector_iterator& it) const {
                return !(*this < it);
            }
            bool operator>(const vector_iterator& it) const {
                return ptr_ > it.ptr_;
            }
            bool operator<=(const vector_iterator& it) const {
                return !(*this > it);
            }
        private:
            pointer ptr_;
        };  // endof class vector_iterator

        using value_type = T;  // TODO: 2d support
        using size_type = size_t;
        using iterator = vector_iterator;

        vector() = default;
        vector(size_type n) {
            value_type* start = allocator_.allocate(n);
            init_iterator(start, n);
            mfwu::construct(begin_, end_, value_type());
        }
        vector(size_type n, const value_type& value) {
            value_type* start = allocator_.allocate(n);
            init_iterator(start, n);
            mfwu::construct(begin_, end_, value);
        }
        vector(const std::initializer_list<value_type>& values) {
            size_type n = values.size();
            value_type* start = allocator_.allocate(n);
            init_iterator(start, n);

            for (auto&& value : values) {
                mfwu::construct(start, value);
                start++;
            }
        }
        vector(const vector& vec) {
            copy(vec, *this);  // TODO: check
        }
        vector(const vector&& vec) {
            init_iterator(&*vec.begin_, vec.capacity());
            vec.reinit();
        }
        // TODO: 2d vector init
        ~vector() {
            reinit();
            // TODO: ensure iterator(?) and allocator free their memory
        }

        iterator begin() const {
            return begin_;
        }
        iterator end() const {
            return end_;
        }

        value_type& back() {
            return *end_;
        }

        size_type size() const {
            return end_ - begin_; 
        }
        size_type capacity() const {
            return last_ - begin_;  // type conversion? long -> unsigned long
        }
        bool empty() const {
            return end_ == begin_;
        }

        void resize(size_type n, const value_type& value=value_type()) {
            // what if n < size() ?
            vector tmp;
            value_type* start = tmp.allocator_.allocate(n);
            // what if n < c
            tmp.init_iterator(start, n);
            fill(*this, tmp);  // TODO: keep capacity
            *this = std::move(tmp);
        }
        void reserve(size_type c) {
            // similar but just allocate
            if (c < size()) {
                // error 
            } else {
                vector tmp = vector();
                value_type* start = tmp.allocator_.allocate(c);
                tmp.init_iterator(start, 0, c);
                for (iterator& it = begin_; it != end_; ++it) {
                    mfwu::construct(start, *it);
                    ++start;
                }
                *this = std::move(tmp);
            }
        }
        void clear() {
            // destroy the objects but not deallocate the memory
            mfwu::destroy(begin_, end_);
            end_ = begin_;
            // last_ unchanged
        }

        void emplace_back(const value_type& value) {
            if (end_ != last_) {
                mfwu::construct(&*end_, value);
                ++end_;
            } else {
                request_mem();
                emplace_back(value);
            }
        }
        template <typename... Args>
        void emplace_back(Args&&... args) {  // 不太懂这块... TODO
            if (end_ != last_) {
                mfwu::construct(&*end_, std::forward<Args>(args)...);
                ++end_;
            } else {
                request_mem();
                emplace_back(std::forward<Args>(args)...);
            }
        }
        void push_back(const value_type& value) {
            emplace_back(value);
        }
        void pop_back() {
            mfwu::destroy(&*end_);
            --end_;
        }

        void insert(int idx, const value_type& value) {
            insert(&begin_[idx], value);
        }
        void insert(int idx, const value_type& value, size_type n) {
            insert(&begin_[idx], value, n);
        }
        void insert(iterator it, const value_type& value) {
            if (end_ != last_) {
                mfwu::construct(&*end_, back());
                ++end_;
                for (iterator pos = end_; pos != it; --pos) {
                    *pos = *(pos - 1);  // check TODO
                }
                *it = value;
            } else {
                request_mem();
                insert(it, value);
            }
        }
        void insert(iterator it, const value_type& value, size_type n) {
            if (end_ + n <= last_) {
                mfwu::construct(end_, end_ + n - 1, value_type());
                end_ += n;
                for (iterator pos = end_; pos >= it; --pos) {
                    *(pos + n) = *pos;
                }
                mfwu::fill(it, it + n - 1, value);
            } else {
                request_mem();
                insert(it, value, n);
            }
        }

        void insert(iterator first, iterator last, iterator it) {
            // TODO: insert(other.first ~ other.last) to position it
        }
        void erase(int idx) {
            erase(&begin_[idx]);
        }
        void erase(iterator it) {
            --end_;
            for (iterator pos = it; pos != end_; pos++) {
                *pos = *(pos + 1);
            }
            mfwu::destroy(&*end_);
        }
        void shrink(const size_type& ref_size) {
            // size_type sz = max(ref_size, size());
            allocator_.deallocate(&*end_, &*last_);
            last_ = end_;
            // TODO
        }

        value_type& operator[](size_type idx) {
            if (idx >= size()) {
                throw std::out_of_range("mfwu::vector - Index out of range");
            }
            return begin_[idx];
        }
        void operator=(const vector& vec) {
            copy(vec, *this);
        }
        void operator=(const vector&& vec) {
            move(vec, *this);
        }
        bool operator!=(const vector& vec) const {
            // cmp value_type first
            // static_assert(this::value_type, vec::value_type)
            if (size() != vec.size()) {
                return false;
            }
            for (iterator it1 = begin_, it2 = vec.begin_;
                it1 != end_ && it2 != vec.end_;
                it1++, it2++) {
                if (*it1 != *it2) {
                    return false;
                }
            }
            return true;
        }
        bool operator==(const vector& vec) const {
            return !(this != vec);
        }
        bool operator<(const vector& vec) const {
            return aux_cmp(vec, false);
        }
        bool operator>=(const vector& vec) const {
            return !(this < vec);
        }
        bool operator>(const vector& vec) const {
            return aux_cmp(vec, true);
        }
        bool operator<=(const vector& vec) const {
            return !(this > vec);
        }

    private:
        iterator begin_;
        iterator end_;
        iterator last_;
        Alloc allocator_;

        void init_iterator(value_type* start, size_type n) {
            begin_ = start;  // explicit
            end_ = last_ = start + n;
        }
        void init_iterator(value_type* start, size_type n, size_type c) {
            begin_ = start;  //
            end_ = start + n;
            last_ = start + c;
        }

        static void copy(const vector& src, vector& dst) {
            size_type n = src.size();
            size_type c = src.capacity();
            dst.reinit();
            value_type* dstart = dst.allocator_.allocate(c);  // just control your own memory
            value_type* sstart = &*src.begin_;
            for (int i = 0; i < n; i++) {
                mfwu::construct(dstart, *sstart);  // TODO: check  // unless you complete "realloc"
                sstart++; dstart++;
            }
            dst.init_iterator(dstart, n, c);
        }
        static void move(vector& src, vector& dst) {
            size_type c = src.capacity();
            dst.reinit();
            dst.init_iterator(&*src.begin_, c);
            src.reinit();
        }
        static void fill(const vector& src, vector& dst) {
            for (iterator& its = src->begin_, itd = dst->begin_;
                its != src->end_ && itd != dst.end_;
                its++, itd++) {
                *itd = *its;
            }
        }

        void reinit() {
            mfwu::destroy(begin_, end_);
            allocator_.deallocate(&*begin_, capacity());
            begin_ = end_ = last_ = iterator();
        }

        void request_mem() {
            vector tmp;
            value_type* start = tmp.allocator_.allocate(2 * size());
            tmp.init_iterator(start, size(), capacity());
            fill(*this, tmp);
        }

        bool aux_cmp(const vector& vec, bool is_larger = true) const {
            for (iterator it1 = begin_, it2 = vec.begin_;
                 it1 != end_ || it2 != vec.end_;
                 it1++, it2++) {
                if (it1 == end_) {
                    return !is_larger;
                } else if (it2 == vec.end_) {
                    return is_larger;
                } else if (*it1 < *it2) {
                    return !is_larger;
                } else if (*it1 > *it2) {
                    return is_larger;
                } else {
                    // cmp next it
                }
            }
            return false;
        }
    };  // endof class vector


}  // endof namespace mfwu


#endif  // __VECTOR__HPP

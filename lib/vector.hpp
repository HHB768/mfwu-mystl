#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"
#include "algorithm.hpp"

namespace mfwu {

class unit_test_vector;

// TODO: test another allocator
// template <typename T, typename Alloc = mfwu::DefaultAllocator<T, mfwu::default_alloc>>
template <typename T, typename Alloc = mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class vector {
public:
    friend class mfwu::unit_test_vector;

    // actually the behaviors of raw pointer
    class vector_iterator
        : public mfwu::iterator<T, mfwu::random_access_iterator_tag> {
    public:
        using Iter = mfwu::iterator<T, mfwu::random_access_iterator_tag>;
        using value_type = typename Iter::value_type;
        using iterator_category = typename Iter::iterator_category;
        using pointer = typename Iter::pointer;
        using reference = typename Iter::reference;
        using difference_type = typename Iter::difference_type;  
        // really confusing lol, and the std::iterator is deprecated since C++17
        // using size_type = size_t;

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
        // vector_iterator& operator=(value_type* const p) {
        //     ptr_ = p;
        //     return *this;
        // }  // still cannot understand the implicit type conversion here

        /* iterators dont manage the memory, the ptrs do */ 
        ~vector_iterator() = default; 

        // reference operator[](int idx) = delete;
        // TODO: without unit test
        reference operator[](int idx) const {
            return ptr_[idx];
        }
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
        vector_iterator operator+(int n) const {
            vector_iterator tmp = *this;
            tmp += n;
            return tmp;
        }
        vector_iterator& operator-=(int n) {
            ptr_ -= n;
            return *this;
        }
        vector_iterator operator-(int n) const {
            vector_iterator tmp = *this;
            tmp -= n;
            return *this;
        }
        difference_type operator-(const vector_iterator& it) const {
            return ptr_ - it.ptr_;
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
    // TODO: you can cmp to using iterator = T*
    // to check your vector_iterator definition

    // vector() = default;
    vector() : begin_(), end_(), last_() {}
    // For POD types, there’s no default initialization (members remain uninitialized).
    // For non-POD types, members are either zero-initialized or default-constructed 
    //     based on whether the class has a user-provided constructor12. 😊
    // [unarchived-07.03] [0809]
    vector(size_type n) {
        value_type* start = allocator_.allocate(n);
        init_iterator(start, n);
        mfwu::uninitialized_fill(begin_, end_, value_type());
        // mfwu::construct(begin_, end_, value_type());
    }
    vector(size_type n, const value_type& value) {
        value_type* start = allocator_.allocate(n);
        init_iterator(start, n);
        mfwu::uninitialized_fill(begin_, end_, value);
        // mfwu::construct(begin_, end_, value);
    }
    vector(const std::initializer_list<value_type>& values) {
        size_type n = values.size();
        value_type* start = allocator_.allocate(n);
        init_iterator(start, n);

        mfwu::uninitialized_copy(values.begin(), values.end(), begin_);
    }
    vector(const vector& vec) {
        reset_and_copy(vec, *this);
    }
    vector(vector&& vec) {
        reset_and_move(vec, *this);
    }
    template <typename ForwardIterator>
    vector(ForwardIterator first, ForwardIterator last) {
        // TODO: check this, and add template base class
        typename mfwu::iterator_traits<ForwardIterator>::difference_type 
             n = mfwu::distance(first, last);
        value_type* start = allocator_.allocate(n);
        init_iterator(start, n);
        mfwu::uninitialized_copy(first, last, begin_);
    }
    ~vector() {
        reinit();
        // TODO: how to ensure allocators free their memory
        // is it necessary to reset_iterator? no? lets try:
        // destroy();
        // deallocate();
    }

    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size()-1]; }
    size_type size() const { return end_ - begin_; }
    size_type capacity() const { return last_ - begin_; }  // type conversion? long -> unsigned long
    bool empty() const { return end_ == begin_; }

    void resize(size_type n, const value_type& value=value_type()) {
        if (n < 0) { return ; }
        if (n <= capacity()) {
            if (n <= size()) {
                mfwu::destroy(begin_ + n, end_);
            } else {  // if size < n < capacity
                mfwu::uninitialized_fill(end_, begin_ + n, value);
            }
            end_ = begin_ + n;
            return ;
        }
        vector tmp;
        value_type* start = tmp.allocator_.allocate(n);
        tmp.init_iterator(start, n);
        iterator finish = uninitialized_copy(begin_, end_, start);
        mfwu::uninitialized_fill(finish, tmp.end_, value);
        reset_and_move(tmp, *this);
    }
    void reserve(size_type c) {
        // similar but just allocate
        if (c <= capacity()) { return ; }
        vector tmp = vector();
        // TODO: try to realloc begin_
        value_type* start = tmp.allocator_.allocate(c);
        tmp.init_iterator(start, size(), c);
        mfwu::uninitialized_copy(begin_, end_, start);
        reset_and_move(tmp, *this);
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
    void emplace_back(Args&&... args) {
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
        // i write this myself and i am not quite sure
        // but later i find stl_vector.h use same way
    }
    void push_back(value_type&& value) {
        emplace_back(mfwu::move(value));
    }
    void pop_back() {
        if (begin_ == end_) { return ; }
        mfwu::destroy(&*end_);  // even with trivial destructor
        --end_;
    }

    void insert(int idx, const value_type& value) {
        insert(begin_ + idx, value);
    }
    void insert(int idx, const value_type& value, size_type n) {
        insert(begin_ + idx, value, n);  // std 接口好像是 (it, n, val)
    }
    void insert(const iterator it, const value_type& value) {
        assert(begin_ <= it && it <= end_);
        if (end_ != last_) {
            mfwu::construct(&*end_, back());
            reverse_copy(it, end_ - 1, it + 1);
            ++end_;
            *it = value;
        } else {
            typename iterator::difference_type idx = it - begin_;
            request_mem();
            insert(begin_ + idx, value);
        }
    }
    void insert(const iterator it, const value_type& value, size_type n) {
        assert(begin_ <= it && it <= end_);
        if (0 == n) return ;
        if (end_ + n <= last_) {
            // mfwu::uninitialized_copy(it, end_, it + n); 
            // unsafe, senario: it + n < end_
            if (it + n > end_) {
                mfwu::construct(end_, it + n, value);
                mfwu::uninitialized_copy(it, end_, it + n);
                mfwu::fill(it, end_, value);
            } else {
                mfwu::uninitialized_copy(end_ - n, end_, end_);
                reverse_copy(it, end_ - n, it + n);
                mfwu::fill(it, it + n, value);
            }
            end_ += n;
        } else {
            typename iterator::difference_type idx = it - begin_;
            request_mem();
            insert(begin_ + idx, value, n);
        }
        // TODO: refer to std impl _M_range_insert()
        // X-Q3: tldr
    }
    void insert(iterator it, std::initializer_list<value_type>& values) {
        assert(begin_ <= it && it <= end_);
        int n = values.size();
        if (end_ + n <= last_) {
            if (it + n > end_) {
                mfwu::uninitialized_copy(it, end_, it + n);
                mfwu::copy(values.begin(), values.begin() + (end_ - it), it);
                mfwu::uninitialized_copy(values.begin() + (end_ - it), values.end(), end_);
            } else {
                mfwu::uninitialized_copy(end_ - n, end_, end_);
                reverse_copy(it, end_ - n, it + n);
                mfwu::copy(values.begin(), values.end(), it);
            }
            end_ += n;
        } else {
            typename iterator::difference_type idx = it - begin_;
            request_mem();
            insert(begin_ + idx, values);
        }
    }
    void insert(iterator it, iterator first, iterator last) {
        // TODO: BUG RD TO FIX
        // you cannot assume inputiterator is vector::iterator
        assert(begin_ <= it && it <= end_);
        int n = last - first;
        if (end_ + n <= last_) {
            if (it + n > end_) {
                mfwu::uninitialized_copy(it, end_, it + n);
                mfwu::copy(first, first + (end_ - it), it);
                mfwu::uninitialized_copy(first + (end_ - it), last, end_);
            } else {
                mfwu::uninitialized_copy(end_ - n, end_, end_);
                reverse_copy(it, end_ - n, it + n);
                mfwu::copy(first, last, it);
            }
            end_ += n;
        } else {
            typename iterator::difference_type idx = it - begin_;
            // it wont work, bcz the logic in the copy side
            // if (begin_ <= first && first <= last_
            //     && begin_ <= last && last <= last_) {
            //     typename iterator::difference_type fcidx = first - begin_,
            //                                        lcidx = last - begin_;
            //     request_mem();
            //     insert(begin_ + idx, begin_ + fcidx, end_ + lcidx);
            //     return ;
            // }
            request_mem();
            insert(begin_ + idx, first, last);
        }
        
    }
    // TODO: what if these its are beyond the begin_ ~ end_ ?
    void erase(int idx) {
        erase(begin_ + idx);
    }
    void erase(iterator it) {
        assert(begin_ <= it && it <= end_);
        mfwu::copy(it + 1, end_, it);
        --end_;
        mfwu::destroy(&*end_);
    }
    void erase(iterator first, iterator last) {
        assert(begin_ <= first && first <= end_
            && begin_ <= last  && last  <= end_);
        int n = last - first;
        mfwu::copy(last, end_, first);
        mfwu::destroy(first + (end_ - last), end_);
        end_ -= n;
    }

    void shrink(const size_type& ref_size) {
        if (ref_size < size() || ref_size >= capacity()) { return ; }
        value_type* start = allocator_.reallocate(&*begin_, 
                                                    capacity(), ref_size);
        if (nullptr == start) {
            // realloc failed, dont shrink
            return ;
        }
        assert(start == &*begin_);
        last_ = begin_ + ref_size;
        
        // the mem allocated from mem pool is hard to be released
        // bcz they are really small and we have to copy them from
        // one node to another, which is quite tiresome...
        // and if we do that, we should return some size type to
        // indicate the new_size, which is conflict against the
        // definition of DefaultAllocator::reallocate
        // we can only provide:
        // 1. a highly coupled shrink-reallocate composition...
        // 2. a shrink that ignore trivial mem realloc from pool
        // 3. a real_last_ to let last_ pretend to shrink...
        // now we implement #2
        // in the default_alloc, realloc will return nullptr if 
        // the old_size or new_size is less than __MAX_BYTES
        // and realloc just exactly like that in the malloc_alloc
        // 24.06.30-07.01 [0809]
    }

//         // no, we cannot assume that the user use the macro to
//         // adjust the allocator, it is actually a template arg
//         void assert_check(value_type* newstart) {
// #ifdef __USE_MALLOC__
//             assert(newstart == &*begin_);  // bcz ref_size < capacity
// #else  // !__USE_MALLOC__
//             assert(newstart == &*begin_);
//             // if (newstart != &*begin_) {
//             //     begin_ = newstart;
//             //     end_ = begin_ + size();
//             // }
// #endif  // __USE_MALLOC__
//         }
    void sort() {
        mfwu::sort(begin(), end());  // TODO: without test
    }

    value_type& operator[](size_type idx) const {
        if (idx >= size()) {
            throw std::out_of_range("mfwu::vector - Index out of range");
        }
        return begin_[idx];
    }
    vector& operator=(const vector& vec) {
        reset_and_copy(vec, *this);
        return *this;
    }
    vector& operator=(vector&& vec) {
        reset_and_move(vec, *this);
        return *this;
    }
    bool operator!=(const vector& vec) const {
        // TODO: check vec is valid (begin_ != nullptr)
        // check other func if valid
        // cmp value_type first
        // static_assert(this::value_type, vec::value_type)
        if (size() != vec.size()) {
            return true;
        }
        for (iterator it1 = begin_, it2 = vec.begin_;
            it1 != end_ && it2 != vec.end_;
            it1++, it2++) {
            if (*it1 != *it2) {
                return true;
            }
        }
        return false;
    }
    bool operator==(const vector& vec) const {
        return !(*this != vec);
    }
    bool operator<(const vector& vec) const {
        return cmp_aux(vec, false);
    }
    bool operator>=(const vector& vec) const {
        return !(*this < vec);
    }
    bool operator>(const vector& vec) const {
        return cmp_aux(vec, true);
    }
    bool operator<=(const vector& vec) const {
        return !(*this > vec);
    }

private:
    iterator begin_;
    iterator end_;
    iterator last_;
    Alloc allocator_;

    void init_iterator(value_type* start, size_type n) {
        begin_ = start;  // explicit?
        end_ = last_ = start + n;
    }
    void init_iterator(value_type* start, size_type n, size_type c) {
        begin_ = start;  //
        end_ = start + n;
        last_ = start + c;
    }

    static void reset_and_copy(const vector& src, vector& dst) {
        dst.reinit();
        value_type* dstart = dst.allocator_.allocate(src.capacity());
        mfwu::uninitialized_copy(src.begin(), src.end(), dstart);
        dst.init_iterator(dstart, src.size(), src.capacity());
    }
    static void reset_and_move(vector& src, vector& dst) {
        dst.reinit();
        dst.init_iterator(&*src.begin_, src.size(), src.capacity());
        src.reset_iterator();
    }
    // static void fill(const vector& src, vector& dst) {
    //     for (iterator& its = src->begin_, itd = dst->begin_;
    //         its != src->end_ && itd != dst.end_;
    //         its++, itd++) {
    //         *itd = *its;
    //     }
    // }
    void reverse_copy(iterator first, iterator last, iterator res) {
        // 听说指针移动和判等没有整数快
        // TODO: 以后试试把 first != last; first++
        //       改成 i < n; i++
        // 搜 insert 的实现时候看到的
        // 24.06.25 [0809]
        --last;
        for (iterator pos = res + (last - first) /*- 1*/;  // del 0818
                pos >= res; --last, --pos) {
            *pos = *last;
            // as i found in the std::vector test
            // value_type must have copy constructor and operator=
        }
    }
    void reverse_uninitialized_copy_aux(iterator first, iterator last, 
                                    iterator res, std::true_type) {
        reverse_copy(first, last, res);
    }
    void reverse_uninitialized_copy_aux(iterator first, iterator last, 
                                    iterator res, std::false_type) {
        for (iterator pos = res + (last - first) - 1;
                pos >= res; first--, pos--) {
            destroy(&*pos);
            construct(&*pos, *first);
        }
    }
    void reverse_uninitialized_copy(iterator first, iterator last, iterator res) {
        reverse_uninitialized_copy_aux(first, last, res,
            std::is_pod<typename iterator_traits<iterator>::value_type>{});
    }

    void reinit() {
        destroy();
        deallocate();
        reset_iterator();
    }

    void destroy() { if(&*begin_) mfwu::destroy(begin_, end_); }
    void deallocate() { if(&*begin_) allocator_.deallocate(&*begin_, capacity()); }
    void reset_iterator() { begin_ = end_ = last_ = iterator(); }

    void request_mem() {
        // TODO: refer to string::req_mem, is it too complicated? 0927
        vector tmp;
        size_type capacity = (this->capacity() ? 2 * this->capacity() : 1);
        value_type* start = tmp.allocator_.allocate(capacity);
        tmp.init_iterator(start, size(), capacity);
        // std::cout << "capacity: " << capacity << "\n";
        mfwu::uninitialized_copy(begin_, end_, start);
        reset_and_move(tmp, *this);
    }

    bool cmp_aux(const vector& vec, bool is_larger = true) const {
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

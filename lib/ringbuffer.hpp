#ifndef __ringbuffer_HPP__
#define __ringbuffer_HPP__

#include "allocator.hpp"
#include "algorithm.hpp"

namespace mfwu {

template <typename T,
          size_t blksz=128,
          typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class ringbuffer {
public:
    using value_type = T;
    using size_type = size_t;

    struct block {
        static constexpr size_type get_padding(size_type vtsz) {
            return blksz * ((vtsz - 1) / blksz + 1) - vtsz;
        }  // check: will it work? or we should deal with vtsz == 0? 24.10.5
        value_type val;
        char[get_padding(sizeof(value_type))] patch;
    };  // endof struct block with size of 128n

    using iterator = block*;

    // class iterator {
    // public:
    //     iterator() : ptr_(nullptr) {}
    //     iterator(T* ptr) ptr_(ptr) {}
    //     iterator(const iterator& it) : ptr_(it.ptr_) {}
    //     iterator(iterator&& it) : ptr_(it.ptr_) {
    //         it.ptr_ = nullptr;
    //     }
    //     ~iterator() {}
    //     iterator& operator=(const iterator& it) {
    //         ptr_ = it.ptr_;
    //         return *this;
    //     }
    //     iterator& operator=(iterator&& it) {
    //         ptr_ = it.ptr_;
    //         it.ptr_ = nullptr;
    //         return *this;
    //     }
    //     iterator& operator++

    // private:
    //     T* ptr_;
    // }
    ringbuffer() : start_(nullptr), last_(nullptr), 
                   write_pos_(), read_pos_(), size_(0) {}
    ringbuffer(size_type n) 
        : start_(allocator_.allocate(n)), last_(start_ + n),
          write_pos_(), read_pos_(), size(n) {
        // to ensure non-pod data, init for all the mem
        mfwu::construct(start_, last_, {});
    }
    ringbuffer(const ringbuffer& rbf)
        : start_(allocator_.allocate(rbf.size_)), last_(start_ + n),
          write_pos_(start_ + rbf.get_write_pos_idx()),
          read_pos_(start_ + rbf.get_read_pos_idx()), size(rbf.size_) {
        mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
    }
    ringbuffer(ringbuffer&& rbf) 
        : start_(rbf.start_), last_(rbf.last_),
          write_pos_(rbf.write_pos_), read_pos_(rbf.read_pos_),
          size_(rbf.size_) {
        rbf.start_ = rbf.last_ = nullptr;
        // we can ignore reinit iterator bcz they should not manage the memory
    }
    ~ringbuffer() {
        _destroy();
    }

    ringbuffer& operator=(const ringbuffer& rbf) {
        reset_and_copy(rbf);
        return *this;
    }
    ringbuffer& operator=(ringbuffer&& rbf) {
        reset_and_copy(rbf);
        return *this;
    }
    value_type& read() {
        iterator ret = read_pos_;
        read_advance();
        return *ret;
    }
    void write(const value_type& val) {
        block blk{val};
        *write_pos_ = mfwu::move(blk);
        write_advance();
    }
    void write(value_type&& val) {
        block blk(mfwu::move(val));
        *write_pos_ = mfwu::move(blk);
        write_advance();
    }
    void read_advance() {
        ++read_pos_;
        if (&*read_pos_ == last_) {
            read_pos_ = iterator(start_);
        }
    }
    void write_advance() {
        ++write_pos_;
        if (&*write_pos_ == last_) {
            write_pos_ = iterator(start_);
        }
    }
private:
    void _destroy() {
        mfwu::destroy(start_, last_);
        allocator_.deallocate(size_);
    }
    void reset_and_copy(const ringbuffer& rbf) {
        _destroy();
        _copy(rbf);
    }
    void _copy(const ringbuffer& rbf) {
        start_ = allocator_.allocate(rbf.size_);
        mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
        last_ = start_ + rbf.size_;
        write_pos_ = start_ + rbf.get_write_pos_idx();
        read_pos_ = start_ + rbf.get_read_pos_idx();
        size_ = rbf.size_;
    }
    void reset_and_copy(ringbuffer&& rbf) {
        _destroy();
        _copy(mfwu::move(rbf));
    }
    void _copy(ringbuffer&& rbf) {
        start_ = rbf.start_; rbf.start_ = nullptr;
        last_ = rbf.last_; rbf.last_ = nullptr;
        write_pos_ = rbf.write_pos_; rbf.write_pos_ = {};
        read_pos_ = rbf.read_pos_; rbf.read_pos_ = {};
        size_ = rbf.size_;  // we wont del this
    }
    size_type get_write_pos_idx() const {
        return &*(write_pos_ - start_);
    }
    size_type get_read_pos_idx() const {
        return &*(read_pos_ - start_);
    }

    block* start_;
    block* last_;
    iterator write_pos_;
    iterator read_pos_;
    size_type size_;
    // iterator end_;
    Alloc allocator_;
};  // endof class ringbuffer

// TODO: NOW GO AND GET UT 10.5/24

}  // endof namespace mfwu

#endif  // __ringbuffer_HPP__
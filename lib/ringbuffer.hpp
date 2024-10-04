#ifndef __RINGBUFFER_HPP__
#define __RINGBUFFER_HPP__

#include "allocator.hpp"
#include "algorithm.hpp"

namespace mfwu {

template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class RingBuffer {
public:
    using value_type = T;
    using size_type = size_t;

    class block {
        constexpr patch_size = mfwu::cal_patch_128(sizeof(value_type));
        value_type val;
        char[patch_size] patch;
    };  // endof class block with size of 128n

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
    RingBuffer() : start_(nullptr), last_(nullptr), 
                   write_pos_(), read_pos_(), size_(0) {}
    RingBuffer(size_type n) 
        : start_(allocator_.allocate(n)), last_(start_ + n),
          write_pos_(), read_pos_(), size(n) {
        // to ensure non-pod data, init for all the mem
        mfwu::construct(start_, last_, {});
    }
    RingBuffer(const RingBuffer& rbf)
        : start_(allocator_.allocate(rbf.size_)), last_(start_ + n),
          write_pos_(start_ + rbf.get_write_pos_idx()),
          read_pos_(start_ + rbf.get_read_pos_idx()), size(rbf.size_) {
        mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
    }
    RingBuffer(RingBuffer&& rbf) 
        : start_(rbf.start_), last_(rbf.last_),
          write_pos_(rbf.write_pos_), read_pos_(rbf.read_pos_),
          size_(rbf.size_) {
        rbf.start_ = rbf.last_ = nullptr;
        // we can ignore reinit iterator bcz they should not manage the memory
    }
    ~RingBuffer() {
        _destroy();
    }

    RingBuffer& operator=(const RingBuffer& rbf) {
        reset_and_copy(rbf);
        return *this;
    }
    RingBuffer& operator=(RingBuffer&& rbf) {
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
    void reset_and_copy(const RingBuffer& rbf) {
        _destroy();
        _copy(rbf);
    }
    void _copy(const RingBuffer& rbf) {
        start_ = allocator_.allocate(rbf.size_);
        mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
        last_ = start_ + rbf.size_;
        write_pos_ = start_ + rbf.get_write_pos_idx();
        read_pos_ = start_ + rbf.get_read_pos_idx();
        size_ = rbf.size_;
    }
    void reset_and_copy(RingBuffer&& rbf) {
        _destroy();
        _copy(mfwu::move(rbf));
    }
    void _copy(RingBuffer&& rbf) {
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
};  // endof class RingBuffer

}  // endof namespace mfwu

#endif  // __RINGBUFFER_HPP__
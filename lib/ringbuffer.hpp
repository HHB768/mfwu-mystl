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
    RingBuffer() : start_(nullptr), last_(nullptr), pos_(), size_(0) {}
    RingBuffer(size_type n) : start_(allocator_.allocate(n)),
                              last_(start_ + n), pos_(), size_(n) {}
    RingBuffer(size_type n, size_type sft) 
        : start_(allocator_.allocate(n)),
          last_(start_ + n), pos_(start_ + sft), size_(n) {}
    RingBuffer(const RingBuffer& rbf)
        : start_(allocator_.allocate(rbf.size_)),
          last_(start_ + n), pos_(start_ + rbf.get_pos_idx()) {
        mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
    }
    RingBuffer(RingBuffer&& rbf) 
        : start_(rbf.start_), last_(rbf.last_), pos_(rbf.pos_), size_(0) {
        rbf.start_ = rbf.last_ = nullptr;
        rbf.pos_ = iterator();
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
    value_type& read() const {
        return *pos_;
    }
    void write(const value_type& val) {
        block blk{val};
        *pos_ = mfwu::move(blk);
    }
    void write(value_type&& val) {
        block blk(mfwu::move(val));
        *pos_ = mfwu::move(blk);
    }
    void advance() {
        ++pos_;
    }
private:
    void _destroy() {
        mfwu::destroy(start_, last_);
        allocator_.deallocate(size_);
    }
    block* start_;
    block* last_;
    iterator pos_;
    const size_type size_;
    // iterator end_;
    Alloc allocator_;
};  // endof class RingBuffer

}  // endof namespace mfwu

#endif  // __RINGBUFFER_HPP__
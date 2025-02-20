#ifndef __ringbuffer_HPP__
#define __ringbuffer_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"

namespace mfwu {

template <typename value_type,
          size_t blksz=128>
struct rbf_block {
    static constexpr size_t get_padding(size_t vtsz) {
        return blksz * ((vtsz - 1) / blksz + 1) - vtsz;
        //  != vtsz - 1 + blksz - vtsz == blksz - 1  -_|||
    }  // check: will it work? or we should deal with vtsz == 0? 24.10.5
    value_type val;
    char padding[get_padding(sizeof(value_type))];

    rbf_block() : val() {}
    rbf_block(const value_type& value) : val(value) {}
    rbf_block(value_type&& value) : val(mfwu::move(value)) {}

    std::string full_string(char sep=0x20) {
        std::stringstream ss;
        ss << val;
        std::string str(get_padding(sizeof(value_type)), sep);
        //              sizeof(padding) / sizeof(char)
        ss << str;
        return ss.str();
    }
};  // endof struct block with size of 128n(?)
template <typename T, size_t sz>
std::ostream& operator<<(std::ostream& os, 
                         const mfwu::rbf_block<T, sz>& blk) {
    os << blk.val;
    return os;
}
template <typename T,
          size_t blksz=128,
          typename Alloc=mfwu::DefaultAllocator<
              mfwu::rbf_block<T, blksz>, mfwu::malloc_alloc>>
class ringbuffer {
public:
    using value_type = T;
    using size_type = size_t;
    using block = mfwu::rbf_block<value_type, blksz>;

    class iterator {
    public:
        using value_type = T;
        using iterator_category = mfwu::random_access_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;
        iterator() : ptr_(nullptr) {}
        iterator(block* ptr) : ptr_(ptr) {}
        iterator(const iterator& it) : ptr_(it.ptr_) {}
        iterator(iterator&& it) : ptr_(it.ptr_) {
            it.ptr_ = nullptr;
        }
        ~iterator() {}

        block* get_ptr() const {
            return ptr_;
        }

        iterator& operator=(const iterator& it) {
            ptr_ = it.ptr_;
            return *this;
        }
        iterator& operator=(iterator&& it) {
            ptr_ = it.ptr_;
            it.ptr_ = nullptr;
            return *this;
        }
        iterator& operator++() {
            ++ptr_;
            return *this;
        }
        iterator operator++(int) {
            iterator temp = *this;
            ++ptr_;
            return temp;
        }
        iterator& operator--() {
            --ptr_;
            return *this;
        }
        iterator operator--(int) {
            iterator temp = *this;
            --ptr_;
            return temp;
        }
        iterator operator+(int n) {
            iterator temp = *this;
            temp.ptr_ += n;  // no such operator+= lol 25.02.20
                             // ops, it says 'temp.ptr' lol
            return temp;
        }
        iterator operator-(int n) {
            iterator temp = *this;
            temp.ptr_ -= n;
            return temp;
        }
        difference_type operator-(const iterator& it) const {
            return ptr_ - it.ptr_;
        }

        value_type& operator*() const {
            return ptr_->val;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        bool operator==(const iterator& it) const {
            return ptr_ == it.ptr_;
        }
        bool operator!=(const iterator& it) const {
            return !(*this == it);
        }
        bool operator<(const iterator& it) const {
            return ptr_ != it.ptr_;  // TODO: ! TEMP
        }
        // TODO: other op
    private:
        block* ptr_;
    };  // endof class iterator

    ringbuffer() : start_(nullptr), last_(nullptr), 
                   write_pos_(), read_pos_(), size_(0) {}
    ringbuffer(size_type n) 
        : start_(allocator_.allocate(n)), last_(start_ + n),
          write_pos_(start_), read_pos_(start_), size_(n) {
        // to ensure non-pod data, init for all the mem
        // mfwu::construct(start_, last_, value_type{});
        // TODO: YOU CANNOT DO THIS
    }
    ringbuffer(const ringbuffer& rbf)
        : start_(allocator_.allocate(rbf.size_)), last_(start_ + rbf.size_),
          write_pos_(start_ + rbf.get_write_pos_idx()),
          read_pos_(start_ + rbf.get_read_pos_idx()), size_(rbf.size_) {
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

    iterator begin() const { return read_pos_; }
    iterator end() const { return write_pos_; }
    size_type size() const { 
        assert(last_ - start_ == size_);
        return size_; 
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
        while (read_pos_ == write_pos_) {}
        iterator ret = read_pos_;
        read_advance();
        return *ret;
    }
    void write(const value_type& val) {

        *write_pos_ = val;
        write_advance();
    }
    void write(value_type&& val) {
        iterator ori = write_pos_;
        write_advance();
        while (write_pos_ == read_pos_) {}
        *ori = mfwu::move(val);
    }
    void read_advance() {
        ++read_pos_;
        if (read_pos_ == last_) {
            read_pos_ = iterator(start_);
        }
    }
    void write_advance() {
        ++write_pos_;
        if (write_pos_ == last_) {
            write_pos_ = iterator(start_);
        }
    }
private:
    void _destroy() {
        mfwu::destroy(start_, last_);
        allocator_.deallocate(start_.get_ptr(), size_);
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
        return write_pos_ - start_;
    }
    size_type get_read_pos_idx() const {
        return read_pos_ - start_;
    }

    iterator start_;
    iterator last_;
    iterator write_pos_;
    iterator read_pos_;
    size_type size_;
    // iterator end_;
    Alloc allocator_;
};  // endof class ringbuffer

// TODO: NOW GO AND GET UT 10.5/24

}  // endof namespace mfwu

#endif  // __ringbuffer_HPP__
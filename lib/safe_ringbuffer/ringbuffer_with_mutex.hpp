#ifndef __RBF_WITH_MTX__
#define __RBF_WITH_MTX__

#include "ringbuffer.hpp"

namespace mfwu_rbf {

template <typename T,
          size_t BlockSize=128>
struct rbf_block {
    using value_type = T;

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
    static constexpr size_t get_padding(size_t vtsz) {
        return mfwu::rbf_block<T, BlockSize>::get_padding(vtsz);
    }
    value_type val;
    char padding[get_padding(sizeof(value_type))];
};  // endof struct rbf_block
template <typename T, size_t sz>
std::ostream& operator<<(std::ostream& os, 
                         const mfwu_rbf::rbf_block<T, sz>& blk) {
    os << blk.val;
    return os;
}
template <typename T,
          size_t BlockSize=128,
          typename Alloc=mfwu::DefaultAllocator<
              mfwu_rbf::rbf_block<T, BlockSize>, mfwu::malloc_alloc>>
class ringbuffer_with_mutex {
public:
    using value_type = T;
    using size_type = size_t;
    using block = mfwu_rbf::rbf_block<value_type, BlockSize>;
    class iterator {
    public:
        using value_type = T;
        using iterator_category = mfwu::forward_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;
        iterator() : ptr_(nullptr), start_(nullptr), last_(nullptr) {}
        iterator(block* ptr, block* start, block* last)
            : ptr_(ptr), start_(start), last_(last) {}
        iterator(const iterator& it) : ptr_(it.ptr_),
            start_(it.start_), last_(it.last_) {}
        iterator(iterator&& it) : ptr_(it.ptr_),
            start_(it.start_), last_(it.last_) {
            it.ptr_ = it.start_ = it.last_ = nullptr;
        }
        ~iterator() {}
        
        block* get_ptr() const {
            return ptr_;
        }

        iterator& operator=(const iterator& it) {
            ptr_ = it.ptr_;
            start_ = it.start_;
            last_ = it.last_;
            return *this;
        }
        iterator& operator=(iterator&& it) {
            ptr_ = it.ptr_;
            start_ = it.start_;
            last_ = it.last_;
            it.ptr_ = it.start_ = it.last_ = nullptr;
            return *this;
        }

        iterator& operator++() {
            ++ptr_;
            if (ptr_ >= last_) {
                ptr_ = start_ + (ptr_ - last_);
            }
            return *this;
        }
        iterator operator++(int) {
            iterator temp = *this;
            ++(*this);
            return temp;
        }
        iterator operator+(int n) {
            iterator temp = *this;
            temp += n;
            return temp;
        }
        iterator& operator+=(int n) {
            ptr_ += n;
            while (ptr_ >= last_) {
                ptr_ = start_ + (ptr_ - last_);
            }
            return *this;
        }

        value_type& operator*() const {
            return ptr_->val;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        bool operator==(const iterator& it) const {
            return ptr_ == it.ptr_ && start_ == it.start_ && last_ == it.last_;
        }
        bool operator!=(const iterator& it) const {
            return !(*this == it);
        }
    private:
        block* ptr_;
        block* start_;
        block* last_;
    };  // endof class iterator

    ringbuffer_with_mutex() = delete;
    ringbuffer_with_mutex(size_type n)
        : start_(Alloc::allocate(n)), last_(start_ + n),
          write_pos_(start_, start_, last_), 
          read_pos_(start_, start_, last_), size_(n), mtx_() {
        // for non-pod data, init them
        mfwu::construct(start_, last_, value_type{});
    }
    ringbuffer_with_mutex(const ringbuffer_with_mutex& rbf)
        : start_(Alloc::allocate(rbf.size_)), last_(start_ + rbf.size_),
          write_pos_(start_ + rbf.get_write_pos_idx(), start_, last_), 
          read_pos_(start_ + rbf.get_read_pos_idx(), start_, last_),
          mtx_(),size_(rbf.size_) {
        mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
        // TODO: open after copy?
    }
    ringbuffer_with_mutex(ringbuffer_with_mutex&& rbf)
        : start_(rbf.start_), last_(rbf.last_),
          write_pos_(rbf.write_pos_), read_pos_(rbf.read_pos_),
          size_(rbf.size_), mtx_() {
        rbf.start_ = rbf.last_ = nullptr;
    }
    ~ringbuffer_with_mutex() { _destroy(); }

    ringbuffer_with_mutex& operator=(const ringbuffer_with_mutex& rbf) = delete;
    // {
    //     std::lock_guard<std::mutex>(mtx_);
    //     _destroy();
    //     start_ = Alloc::allocate(rbf.size_);
    //     mfwu::uninitialized_copy(rbf.start_, rbf.last_, start_);
    //     last_ = start_ + rbf.size_;
    //     write_pos_ = start_ + rbf.get_write_pos_idx();
    //     read_pos_ = start_ + rbf.get_read_pos_idx();
    //     size_ = rbf.size_;
    //     return *this;
    // }
    ringbuffer_with_mutex& operator=(ringbuffer_with_mutex&& rbf) = delete;
    // {
    //     std::lock_guard<std::mutex>(mtx_);
    //     start_ = rbf.start_; rbf.start_ = {};
    //     last_ = rbf.last_; rbf.last_ = {};
    //     write_pos_ = rbf.write_pos_; rbf.write_pos_ = {};
    //     read_pos_ = rbf.read_pos_; rbf.read_pos_ = {};
    //     size_ = rbf.size_;
    //     return *this;
    // }

    iterator begin() const { return read_pos_; }
    iterator end() const { return write_pos_; }
    bool empty() {  // we cannot add const here?
        std::lock_guard<std::mutex> locker(mtx_);
        return write_pos_ == read_pos_;
    }
    bool full() {
        std::lock_guard<std::mutex> locker(mtx_);
        return write_pos_ + 1 == read_pos_;
    }
    size_type size() const {
        assert(last_ - start_ == size_);
        return size_;
    }
    void clear() {
        std::lock_guard<std::mutex> locker(mtx_);
        write_pos_ = read_pos_ = iterator(start_, start_, last_);
    }
    bool /*iterator*/ read(value_type* res) {
        std::unique_lock<std::mutex> locker(mtx_);
        while (read_pos_ == write_pos_) {
            consumer_.wait(locker);
            if (is_close_) {
                return false;
            }
        }
        *res = *read_pos_;
        ++read_pos_;
        producer_.notify_one();

        return true;
    }
    bool write(const value_type& val) {
        std::unique_lock<std::mutex> locker(mtx_);
        while ((write_pos_ + 1) == read_pos_) {
            producer_.wait(locker);
            if (is_close_) {
                return false;
            }
        }
        *write_pos_ = val;
        ++write_pos_;
        consumer_.notify_one();

        return true;
    }
    bool write(value_type&& val) {
        std::unique_lock<std::mutex> locker(mtx_);
        while ((write_pos_ + 1) == read_pos_) {
            producer_.wait(locker);
            if (is_close_) {
                return false;
            }
        }
        *write_pos_ = mfwu::move(val);
        ++write_pos_;
        consumer_.notify_one();

        return true;
    }

    void _close() {
        {
            // std::cout << "close\n";
            std::unique_lock<std::mutex> locker(mtx_);
            is_close_ = true;
        }
        producer_.notify_all();
        consumer_.notify_all();
    }
    bool is_close() {
        std::unique_lock<std::mutex> locker(mtx_);
        return is_close_;
    }
    void flush() {
        // std::cout << "to flush one\n";
        consumer_.notify_one();
        // sleep(1);
    }
private:
    void _destroy() {
        mfwu::destroy(start_, last_);
        Alloc::deallocate(start_, size_);
    }
    size_type get_write_pos_idx() const {
        return write_pos_ - start_;
    }
    size_type get_read_pos_idx() const {
        return read_pos_ - start_;
    }

    block* start_;
    block* last_;
    
    iterator write_pos_;
    iterator read_pos_;
    const size_type size_;
    std::mutex mtx_;

    std::condition_variable consumer_;
    std::condition_variable producer_;
    bool is_close_ = false;
};  // endof class ringbuffer_with_mutex

}  // endof namespace mfwu_rbf

#endif  // __RBF_WITH_MTX__
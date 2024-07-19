#ifndef __DEQUE_HPP__
#define __DEQUE_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {

// const size_t BLK_SIZE = 16;  // 512
// TODO: should be a template arg

template <typename T, size_t BLK_SIZE=16,
          typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
struct block {
public:
    // friend class mfwu::deque<T, Alloc>;  // TODO

    using value_type = T;
    using iterator = value_type*;
    using size_type = size_t;
    // allocate but no elements
    block() {
        blk_ = allocator_.allocate(BLK_SIZE);
        begin_ = end_ = blk_;
    }
    block(int n, const value_type& val=value_type(), bool reverse=false) {
        assert(n <= BLK_SIZE);
        blk_ = allocator_.allocate(BLK_SIZE);
        if (reverse) {
            end_ = blk_ + BLK_SIZE;
            begin_ = end_ - n;
        } else {
            begin_ = blk_;
            end_ = blk_ + n;
        }
        mfwu::construct(begin_, end_, val);
    }
    // destroy and deallocate
    ~block() {
        mfwu::destroy(begin_, end_);
        allocator_.deallocate(blk_, BLK_SIZE);
    }
    
    iterator push_front(const value_type& val) {
        if (begin_ == blk_) {
            return nullptr;
        }
        mfwu::construct(--begin_, val);
        return begin_;
    }
    iterator push_back(const value_type& val) {
        if (end_ - blk_ >= BLK_SIZE) {
            return nullptr;
        }
        mfwu::construct(end_++, val);
        return end_ - 1;
    }
    iterator pop_front() {
        if (begin_ >= end_) {
            return nullptr;
        }
        mfwu::destroy(begin_++);
        return begin_;
    }
    iterator pop_back() {
        if (begin_ >= end_) {
            return nullptr;
        }
        mfwu::destroy(--end_);
        if (end_ == begin_) {
            return end_;
        }
        return end_ - 1;
    }

    size_type size() const { return end_ - begin_; }
    bool empty() const { return end_ == begin_; }
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type* start() const { return blk_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size() - 1]; }
// private:
    value_type* blk_;
    iterator begin_, end_;
    Alloc allocator_;
};  // endof class block

template <typename T, size_t BLK_SIZE=16,
          typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class deque {
public:
    using value_type = T;
    using size_type = size_t;
    using block = mfwu::block<T, BLK_SIZE, Alloc>;
    using pblock = block*;

    class deque_iterator 
        : public mfwu::iterator<T, mfwu::random_access_iterator_tag> {  // TODO: really?
    public:
        // template <typename U, typename Al>
        // friend class deque<U, Al>;  // TODO: check

        using Iter = mfwu::iterator<T, mfwu::random_access_iterator_tag>;
        using value_type = typename Iter::value_type;
        using iterator_category = typename Iter::iterator_category;
        using pointer = typename Iter::pointer;
        using reference = typename Iter::reference;
        using difference_type = typename Iter::difference_type;

        deque_iterator() : cur_(nullptr), begin_(nullptr), end_(nullptr),
                           pos_(nullptr) {}
        deque_iterator(pointer cur, pblock* pos) : cur_(cur),
            begin_((*pos)->begin()), end_((*pos)->end()), pos_(pos) {}
        deque_iterator(pointer cur, pointer begin, pointer end, pblock* pos) 
            : cur_(cur), begin_(begin), end_(end), pos_(pos) {}
        deque_iterator(const deque_iterator& it)
            : cur_(it.cur_), begin_(it.begin_), end_(it.end_), pos_(it.pos_) {}
        deque_iterator(deque_iterator&& it)
            : cur_(it.cur_), begin_(it.begin_), end_(it.end_), pos_(it.pos_) {
            it.cur_ = it.begin_ = it.end_ = nullptr;
            it.pos_ = nullptr;
        }
        ~deque_iterator() {}
        deque_iterator& operator=(const deque_iterator& it) {
            cur_ = it.cur_;
            begin_ = it.begin_;
            end_ = it.end_;
            pos_ = it.pos_;
            return *this;
        }
        deque_iterator& operator=(deque_iterator&& it) {
            cur_ = it.cur_; it.cur_ = nullptr;
            begin_ = it.begin_; it.begin_ = nullptr;
            end_ = it.end_; it.end_ = nullptr;
            pos_ = it.pos_; it.pos_ = nullptr;
            return *this;
        }

        value_type& operator*() const {
            return *cur_;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        deque_iterator& operator++() {
            if (cur_ + 1 < end_) {
                ++cur_;
            } else {
                to_next_block();
            }
            return *this;
        }
        deque_iterator operator++(int) {
            deque_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        deque_iterator& operator--() {
            if (cur_ -1 >= begin_) {
                --cur_;
            } else {
                to_prev_block();
            }
            return *this;
        } 
        deque_iterator operator--(int) {
            deque_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        deque_iterator& operator+=(int n) {
            if (cur_ + n < end_ && cur_ + n >= begin_) {
                cur_ += n;
            } else if (n > 0) {
                int res = end_ - cur_;
                int blk_num = (n - res) / BLK_SIZE;
                to_next_block(blk_num + 1);
                cur_ += n - res - blk_num * BLK_SIZE;  // TODO
            } else {  // n < 0
                *this -= -n;
            }
            return *this;
        }
        deque_iterator& operator-=(int n) {
            if (cur_ - n >= begin_ && cur_ - n < end_) {
                cur_ -= n;
            } else if (n > 0) {
                int res = cur_ - begin_ + 1;
                int blk_num = (n - res) / BLK_SIZE;
                to_prev_block(blk_num + 1);
                cur_ -= n - res - blk_num * BLK_SIZE;  // TODO
            } else {
                *this += -n;
            }
            return *this;
        }
        deque_iterator operator+(int n) {
            deque_iterator tmp = *this;
            tmp += n;
            return tmp;
        }
        deque_iterator operator-(int n) {
            deque_iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        bool operator==(const deque_iterator& it) const {
            return cur_ == it.cur_ && begin_ == it.begin_
                && end_ == it.end_ && pos_ == it.pos_;
        }
        bool operator!=(const deque_iterator& it) const {
            return !(*this==it);
        }
        bool operator<(const deque_iterator& it) const {
            if (pos_ == it.pos_) {
                return cur_ < it.cur_;
            } else {
                return pos_ < it.pos_;
            }
        }
        bool operator>(const deque_iterator& it) const {
            if (pos_ == it.pos_) {
                return cur_ > it.cur;
            } else {
                return pos_ > it.pos_;
            }
        }
        bool operator<=(const deque_iterator& it) const {
            return !(*this > it);
        }
        bool operator>=(const deque_iterator& it) const {
            return !(*this < it);
        }
        pblock* get_pos() const {
            return pos_;
        }
        pointer get_cur() const {
            return cur_;
        }

    private:
        void to_prev_block(int n=1) {
            pos_ -= n;
            begin_ = &* (*pos_)->begin();
            end_ = &* (*pos_)->end();
            cur_ = end_ - 1;
        }
        void to_next_block(int n=1) {
            pos_ += n;
            cur_ = begin_ = &* (*pos_)->begin();
            end_ = &* (*pos_)->end();
        }
        pointer cur_, begin_, end_;
        pblock* pos_;

    };  // endof class deque_iterator
    using iterator = deque_iterator;

    deque() : ctrl_(nullptr), last_(nullptr), begin_(nullptr), end_(nullptr) {}
    deque(int n, const value_type& val=value_type()) {
        size_type blk_num = init(n);
        pblock* blk = ctrl_;
        for ( ; blk < last_ - 1; blk++) {
            *blk = new block(BLK_SIZE, val);
        }
        *blk = new block(n - BLK_SIZE * (blk_num - 1), val);
    }
    deque(const std::initializer_list<value_type>& vals) {
        init(n);
        pblock* blk = ctrl_;
        for ( ; blk < last_; blk++) {
            *blk = new block();  // the block is aware of the ctrl
        }
        auto it = vals.begin();
        for (blk = ctrl_; blk < last_; blk++) {
            mfwu::uninitialized_copy(it, it + BLK_SIZE, (*blk)->begin());
            it += BLK_SIZE;  // TODO: check
        }
    }
    deque(const deque& dq) {
        copy(dq);
    }
    deque(deque&& dq) : ctrl_(dq.ctrl_), last_(dq.last_) {
        dq.ctrl_ = dq.last_ = nullptr;
    }
    ~deque() {
        // mfwu::destroy(begin(), end());
        for (pblock* blk = ctrl_; blk < last_; blk++) {
            (*blk)->~block();
        }
        free(ctrl_);
    }
    deque& operator=(const deque& dq) {
        reinit();
        copy(dq);
        return *this;
    }
    deque& operator=(deque&& dq) {
        ctrl_ = dq.ctrl_;
        last_ = dq.last_;
        dq.ctrl_ = dq.last_ = nullptr;
    }
    
    value_type& front() const {
        return (*begin_)->front();
    }
    value_type& back() const {
        return (*(end_ - 1))->back();
    }
    iterator begin() const {
        return iterator((*begin_)->begin(), begin_);
    }
    iterator end() const {
        return iterator(nullptr, end_);
    }
    value_type& operator[](int idx) {
        // TODO: out of range
        return *(begin() + idx);  // trytry
    }
    void push_front(const value_type& val) {
        if (begin_ == end_) { add_front_block(); }  // no block
        typename block::iterator it = (*begin_)->push_front(val);
        // block no space
        if (it == nullptr) {
            add_front_block();
            it = (*begin_)->push_front(val);
            // TODO: check: should success
        }
    }
    void push_back(const value_type& val) {
        if (begin_ == end_) { add_back_block(); }
        typename block::iterator it = (*(end_ - 1))->push_back(val);
        if (it == nullptr) {
            add_back_block();
            it = (*begin_)->push_back(val);
        }
    }
    void pop_front() {
        if (begin_ == end_) { return ; }
        typename block::iterator it = (*begin_)->pop_front();
        if (it == nullptr) {
            std::cout << "block empty!\n";  // TODO: check
        } else if (it == (*begin_)->end()) {
            // block empty
            del_front_block();
        }
    } 
    void pop_back() {
        if (begin_ == end_) { return ; }
        typename block::iterator it = (*(end_ - 1))->pop_back();
        if (it == nullptr) {
            std::cout << "block empty!\n";  // TODO
        } else if (it == (*begin_)->end()) {
            del_back_block();
        }
    }

    void insert(int idx, const value_type& val) {
        insert(begin() + idx, val);
    }
    void insert(iterator it, const value_type& val) {
        pblock* blk = it.get_pos();
        if ((*blk)->size() == BLK_SIZE) {
            if (it.get_cur() - (*blk)->begin() < BLK_SIZE / 2) {
                blk = insert_before_block(blk, (*blk)->begin(), it.get_cur());
                blk++;
                mfwu::destroy((*blk)->begin(), (*blk)->begin() + size_before);
                (*blk)->set_begin(size_before);  // TODO: IN THE BLOCK
            } else {
                blk = insert_before_block(blk + 1, it.get_cur(), (*blk)->end());
                blk--;
                mfwu::destroy((*blk)->begin() + size_before, (*blk)->end());
                (*blk)->set_end(size_before);  // check
            }
        } else {
            bool is_head_free = (*blk)->begin() - (*blk)->start();
            bool is_tail_free = (*blk)->start() + BLK_SIZE - (*blk)->end();
            if (is_head_free && is_tail_free) {
                if (it.get_cur() - (*blk)->begin() < (*blk)->end() - it.get_cur()) {
                    (*blk)->insert(it.get_cur(), true);
                } else {
                    (*blk)->insert(it.get_cur(), false);
                }
            } else if (is_head_free) {
                (*blk)->insert(it.get_cur(), true);
            } else if (is_tail_free) {
                (*blk)->insert(it.get_cur(), false);
            } else {
                std::cout << "impossible to come here\n";  // TODO
            }
        }
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {

    }
    

private:
    size_type init(size_type n=0) {
        if (0 == n) {
            ctrl_ = last_ = begin_ = end_ = nullptr;
            return 0;
        }

        int blk_num = (n + BLK_SIZE - 1) / BLK_SIZE;
        begin_ = ctrl_ = (pblock*)malloc(sizeof(pblock) * blk_num);
        end_ = last_ = ctrl_ + blk_num;
        return blk_num;
    }
    void copy(const deque& dq) {
        init(dq.size());
        pblock* blk = ctrl_;
        for ( ; blk < last_; blk++) {
            *blk = new block();
        }
        auto it = dq.begin();
        for (blk = ctrl_; blk < last_; blk++) {
            mfwu::uninitialized_copy(it, it + BLK_SIZE, (*blk)->begin());
            it += BLK_SIZE;
        }
    }
    void add_front_block() {
        if (begin_ > ctrl_) {
            --begin_;
            *begin = new block();
        } else {
            req_mem_front();
            add_front_block();
        }
    }
    void add_back_block() {
        if (end_ < last_) {
            *end_ = new block();
            ++end_;
        } else {
            req_mem_back();
            add_back_block();
        }
    }
    void del_front_block() {
        if (begin_ < end_) {
            (*begin_)->~block();  // or delete?
            ++begin_;
        }
    }
    void del_back_block() {
        if (begin_ < end_) {
            --end_;
            (*end_)->~block();
        }
    }
    pblock* insert_before_block(pblock* blk,
        block::iterator first, block::iterator last) {
        if (begin_ > ctrl_ && end_ < last_) {
            if (blk - begin_ < end_ - blk) {
                pblock* pp = move_forward(blk);
                (*pp)->construct(first, last);
            } else {
                pblock* pp = move_backward(blk);
                (*pp)->construct(first, last);
            }
        } else if (begin_ > ctrl_) {
            pblock* pp = move_forward(blk);
            (*pp)->construct(first, last);
        } else if (end_ < last_) {
            pblock* pp = move_backward(blk);
            (*pp)->construct(first, last);
        } else {
            int idx = blk - ctrl_;
            req_mem_back();
            return insert_before_block(ctrl_ + idx, first, last);
        }
    }
    pblock* move_forward(pblock* blk) {
        pblock* pp = begin_;
        for ( ; pp != blk; ++pp) {
            *(pp - 1) = *pp;
        }
        (*(pp -1))->destroy();
        --begin_;
        return pp - 1;
    }
    pblock* move_backward(pblock* blk) {
        pblock* pp = end_;
        for ( ; pp != blk; --pp) {
            *pp = *(pp - 1);
        }
        (*pp)->destroy();
        ++end_;
        return pp;
    }
    pblock* ctrl_;
    pblock* last_;
    pblock* begin_;
    pblock* end_;

    // TODO: how about a list ctrl?
    // no, you should support random access
    // list + mapping to node ?
    
    // block* pos_;
};
}  // endof namespace mfwu


#endif  // __DEQUE_HPP__
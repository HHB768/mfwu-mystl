#ifndef __DEQUE_HPP__
#define __DEQUE_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {


template <typename T, size_t BLK_SIZE=512,
          typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
struct block {
public:
    // friend class mfwu::deque<T, Alloc>;  // TODO

    using value_type = T;
    using iterator = value_type*;
    using size_type = size_t;
    // allocate but no elements
    block(int n=BLK_SIZE) : blk_(allocator_.allocate(n))
              begin_(blk_), end_(blk_) {}
    // if n == 0, blk_ may be a nullptr, then this block is dummy block
    // malloc(0) might return NULL or a non-NULL pointer, 
    // depending on your system’s implementation. 
    // Regardless of what it returns, you can safely pass the pointer to free().
    // GPT 0812
    block(int n, const value_type& val, bool reverse=false) {
    // we disable val default value to support dummy block init
    // maybe we should place block inside the deque
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
    block(const block& blk): blk_(allocator_.allocate(BLK_SIZE))
                             begin_(blk.begin_), end_(blk_.end_) {
        mfwu::uninitialized_copy(blk.begin_, blk.end_, begin_);
    }
    block(block&& blk) : blk_(blk.blk_), begin_(blk.begin_), end_(blk.end_) {
        blk.blk_ = nullptr;
        blk.begin_ = blk.end_ = nullptr;
    }
    // destroy and deallocate
    ~block() {
        mfwu::destroy(begin_, end_);
        allocator_.deallocate(blk_, BLK_SIZE);
    }
    
    // iterator push_front(const value_type& val) {
    //     if (begin_ == blk_) {
    //         return nullptr;
    //     }
    //     mfwu::construct(--begin_, val);
    //     return begin_;
    // }
    // iterator push_back(const value_type& val) {
    //     if (end_ - blk_ >= BLK_SIZE) {
    //         return nullptr;
    //     }
    //     mfwu::construct(end_++, val);
    //     return end_ - 1;
    // }
    // iterator pop_front() {
    //     if (begin_ >= end_) {
    //         return nullptr;
    //     }
    //     mfwu::destroy(begin_++);
    //     return begin_;
    // }
    // iterator pop_back() {
    //     if (begin_ >= end_) {
    //         return nullptr;
    //     }
    //     mfwu::destroy(--end_);
    //     if (end_ == begin_) {
    //         return end_;
    //     }
    //     return end_ - 1;
    // }
    // 不惯着用户了！应该自行判断是否成功 0810
    // 好吧用户就是我自己，怪不得这么硬气 :D

    void push_front(const value_type& val) {
        mfwu::construct(--begin_, val);
    }
    void push_back(const value_type& val) {
        mfwu::construct(end_++, val);
    }
    void pop_front() {
        mfwu::destroy(begin_++);
    }
    void pop_back() {
        mfwu::destroy(--end_);
    }
    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last, iterator res) {
        iterator res_end = mfwu::uninitialized_copy(first, last, res);
        assert(res >= blk_ && res_end < blk_ + BLK_SIZE);
        begin_ = min(begin_, res);
        end_ = max(end_, res_end);
    }

    size_type size() const { return end_ - begin_; }
    constexpr size_type capacity() const { return BLK_SIZE; }
    bool empty() const { return end_ == begin_; }
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type* start() const { return blk_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size() - 1]; }

    size_type front_space() const { return begin_ - blk_; }
    size_type back_space() const { return blk_ + BLK_SIZE - end_; }
    size_type has_front_space() const { return begin_ > blk_; }
    size_type has_back_space() const { return blk_ + BLK_SIZE > end_; }
// private:
    value_type* blk_;
    iterator begin_, end_;
    Alloc allocator_;
};  // endof struct block

// TODO: test 16 first
template <typename T, size_t BLK_SIZE=512,
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
            if (++cur_ >= end_) {
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
            if (--cur_ < begin_) {
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
                *this -= (-n);
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
                *this += (-n);
            }
            return *this;
        }
        deque_iterator operator+(int n) const {
            deque_iterator tmp = *this;
            tmp += n;
            return tmp;
        }
        deque_iterator operator-(int n) const {
            deque_iterator tmp = *this;
            tmp -= n;
            return tmp;
        }

        bool operator==(const deque_iterator& it) const {
            return (cur_ == it.cur_ && begin_ == it.begin_
                && end_ == it.end_ && pos_ == it.pos_) ;
                // || (cur_ == begin_ && it.cur_ == it.end_ && pos_ - 1 == it.pos_)
                // || (cur_ == end_ && it.cur_ == it.begin_ && pos_ + 1 == it.pos_);
                // TODO: CHECK begin_ iterator == end_ iterator in last buffer
                // no, you should prevent this case in operator++/+=
                // we decide that add a dummy pblock as the position after the last
        }
        bool operator!=(const deque_iterator& it) const {
            return !(*this == it);
        }
        bool operator<(const deque_iterator& it) const {
            if (pos_ == it.pos_) {
                return cur_ < it.cur_;
            }
            return pos_ < it.pos_;
        }
        bool operator>(const deque_iterator& it) const {
            if (pos_ == it.pos_) {
                return cur_ > it.cur;
            }
            return pos_ > it.pos_;
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
            begin_ = /*&**/ (*pos_)->begin();
            end_ = (*pos_)->end();
            cur_ = end_ - 1;
        }
        void to_next_block(int n=1) {
            pos_ += n;
            cur_ = begin_ = (*pos_)->begin();
            end_ = (*pos_)->end();
        }
        pointer cur_, begin_, end_;
        pblock* pos_;

    };  // endof class deque_iterator
    using iterator = deque_iterator;

    // add a dummy pblock in last_
    // when last_ change, remember to create a new dummy pblock
    deque() : ctrl_((pblock*)malloc(sizeof(pblock))),
              last_(ctrl_), begin_(ctrl_), end_(ctrl_) {
        init_dummy_block();
    }
    deque(int n, const value_type& val=value_type()) {
        size_type blk_num = init_ctrl(n);
        pblock* blk = ctrl_;
        for ( ; blk < last_ - 1; blk++) {
            *blk = new block(BLK_SIZE, val);
        }
        *blk = new block(n - BLK_SIZE * (blk_num - 1), val);
        init_dummy_block();
        // previous design:
        // we can use these mem when push_back
        // so we reserve full mem for dummy pblock
        // but when BLK_SIZE too large, it is a waste
    }
    deque(const std::initializer_list<value_type>& vals) {
        copy_init(vals.begin(), vals.end());
    }
    deque(const deque& dq) {
        copy_init(dq.begin(), dq.end());
    }
    deque(deque&& dq) 
        : ctrl_(dq.ctrl_), last_(dq.last_),
          begin_(dq.begin_), end_(dq.end_) {
        dq.ctrl_ = dq.last_ = nullptr;
        dq.begin_ = dq.end_ = nullptr;
    }
    ~deque() {
        mfwu::destroy(begin(), end() + 1);
        // for (pblock* blk = begin_; blk < end_; blk++) {
            // (*blk)->~block();
        // }
        free(ctrl_);
    }
    deque& operator=(const deque& dq) {
        reinit();
        copy_init(dq);
        return *this;
    }
    deque& operator=(deque&& dq) {
        ctrl_ = dq.ctrl_; last_ = dq.last_;
        begin_ = dq.begin_; end_ = dq.end_;
        dq.ctrl_ = dq.last_ = nullptr;
        dq.begin_ = dq.end_ = nullptr;
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
        return iterator((*end_)->begin(), end_);
        // nearly equivalent to iterator(nullptr, end_)
        // end_ has dummy begin_, cur_, end_
    }
    value_type& operator[](int idx) {
        // TODO: out of range
        assert(0 <= idx && idx < size());
        // if (idx < 0 || idx >= size()) { 
        //     throw std::out_of_range("Index out of range"); 
        // }
        return *(begin() + idx);  // GPT: trytry  8.11
    }
    void push_front(const value_type& val) {
        if (begin_ == end_) { push_front_block(); }  // no block
                                                     // except dummy
        if (!(*begin_)->has_front_space()) {
            push_front_block();
        }
        (*begin_)->push_front(val);
    }
    void push_back(const value_type& val) {
        if (begin_ == end_) { push_back_block(); }
        if (!(*(end_ - 1))->has_back_space()) {
            push_back_block();
        }
        (*(end_ - 1))->push_back(val);
    }
    void pop_front() {
        // if (begin_ == end_) { return ; }
        (*begin_)->pop_front();
        if ((*begin_)->empty()) {
            pop_front_block();
        }
    } 
    void pop_back() {
        // if (begin_ == end_) { return ; }
        (*(end_ - 1))->pop_back();
        if ((*(end_ - 1))->empty()) {
            pop_back_block();
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
    void init_dummy_block() {
        *end_ = new block(0);
        // TODO: give a init value to test
        // *end = new block(1);
        // (*end_)->push_back(value_type{});
    }
    // init ctrl region for n elements
    size_type init_ctrl(size_type n=0) {
        if (0 == n) {
            ctrl_ = last_ = begin_ = end_ = nullptr;
            return 0;
        }
        int blk_num = (n + BLK_SIZE - 1) / BLK_SIZE;
        begin_ = ctrl_ = (pblock*)malloc(sizeof(pblock) * blk_num + 1); // 1 dummy
        end_ = last_ = ctrl_ + blk_num;
        return blk_num;
    }
    template <typename RandomAccessIterator>
    void copy_init(RandomAccessIterator first, RandomAccessIterator last) {
        ctrl_init(last - first);
        pblock* blk = ctrl_;
        auto it = first;
        for ( ; blk < last_ - 1; blk++) {
            *blk = new block();  // the block is aware of the ctrl
            (*blk)->assign(it, it + BLK_SIZE, (*blk)->begin());
            it += BLK_SIZE;  // TODO: check
        }
        *blk = new block();
        (*blk)->assign(it, last, (*blk)->begin());
        init_dummy_block();
    }
    void push_front_block() {
        if (begin_ > ctrl_) {
            --begin_;
            *begin = new block();
        } else {
            req_mem_front();
            push_front_block();
        }
    }
    void push_back_block() {
        if (end_ < last_) {
            mfwu::destroy(*end);
            *end_ = new block();
            ++end_;
            init_dummy_block();
        } else {
            req_mem_back();
            push_front_block();
        }
    }
    void pop_front_block() {
        if (begin_ < end_) {
            // ~block() // or delete? // or destroy?
            mfwu::destroy(*begin_);
            ++begin_;
        }
    }
    void pop_front_block() {
        if (begin_ < end_) {
            mfwu::destroy(*end_);
            --end_;
            mfwu::destroy(*end_);
            init_dummy_block();
        }
    }
    void req_mem_front() {
        size_type original_capacity = last_ - ctrl_;
        size_type begin_idx = begin_ - ctrl_;
        size_type end_idx = last_ - end_;
        size_type new_capacity = original_capacity * 2 + 1;
        pblock* new_ctrl = (pblock*)malloc(sizeof(pblock) * new_capacity);
        mfwu::uninitialized_copy(  // copy all these pblocks to new ctrl
            begin_, end_ + 1, new_ctrl + original_capacity + begin_idx);
        free(ctrl_);
        ctrl_ = new_ctrl;
        last_ = new_ctrl + new_capacity;
        begin_ = ctrl_ + original_capacity + begin_idx;
        end_ = ctrl_ + original_capacity + end_idx;
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
    // 24.07.17 [0809]
    
    // block* pos_;
};
}  // endof namespace mfwu


#endif  // __DEQUE_HPP__
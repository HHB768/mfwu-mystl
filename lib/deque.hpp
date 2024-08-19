#ifndef __DEQUE_HPP__
#define __DEQUE_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"
#include "algorithm.hpp"

namespace mfwu {

class unit_test_deque;

// TODO: test 16 first
template <typename T, size_t BLK_SIZE=512,
          typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class deque {
public:
    friend class mfwu::unit_test_deque;

    using value_type = T;
    using size_type = size_t;
    struct block;
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
        mfwu::ptrdiff_t operator-(const deque_iterator& it) const {
            mfwu::ptrdiff_t ret = 0;
            if (pos_ - it.pos_ > 0) {
                for (auto blk = it.pos_; blk != pos_; ++blk) {
                    ret += (*blk)->size();
                }
            } else {
                for (auto blk = pos_; blk != it.pos_; ++blk) {
                    ret -= (*blk)->size();
                }
            }  // TODO: CHECK
            return ret + (cur_ - begin_) - (it.cur_ - it.begin_);
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
        pointer get_begin() const {
            return begin_;
        }
        pointer get_end() const {
            return end_;
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
        destroy();
        deallocate();
    }
    deque& operator=(const deque& dq) {
        this->reinit();  
        // compiler: you should explicitly note that
        // reinit is a member of a dependent type bcz
        // it use some template args  
        // error: there are no arguments to ‘reinit’ 
        // that depend on a template parameter, 
        // so a declaration of ‘reinit’ must be available
        // 0813 0046 X
        copy_init(dq.begin(), dq.end());
        return *this;
    }
    deque& operator=(deque&& dq) {
        this->reinit();
        ctrl_ = dq.ctrl_; last_ = dq.last_;
        begin_ = dq.begin_; end_ = dq.end_;
        dq.ctrl_ = dq.last_ = nullptr;
        dq.begin_ = dq.end_ = nullptr;
        return *this;
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
    size_type size() const {
        return end() - begin();
    }
    value_type& operator[](int idx) {
        // TODO: out of range
        assert(0 <= idx && idx < this->size());
        // if (idx < 0 || idx >= size()) { 
        //     throw std::out_of_range("Index out of range"); 
        // }
        return *(begin() + idx);  // GPT: trytry  8.11
    }
    void push_front(const value_type& val) {
        if (begin_ == end_) { push_front_block(); }  // no block
                                                     // except dummy
        std::cout << begin_ - ctrl_ << ", \n";
        if (!(*begin_)->has_front_space()) {
            push_front_block();
        }
        std::cout << begin_ - ctrl_ << ", \n";
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
        value_type* cur = it.get_cur();
        value_type* begin = (*blk)->begin();
        value_type* end = (*blk)->end();
        value_type* start = (*blk)->start();
        size_type cur_idx = cur - start;
        size_type begin_idx = begin - start;
        size_type end_idx = end - start;
        
        if ((*blk)->size() == BLK_SIZE) {  // full
            if (cur - begin < BLK_SIZE / 2) {
                // move front part to prev block
                if (blk > this->begin_ && (*(blk - 1))->has_back_space()) {
                    (*(blk - 1))->push_back(*begin);
                    mfwu::copy(begin + 1, cur, begin);   
                    *cur = val;
                } else {  // previous block has no back space
                    pblock* newblk = insert_before_block(blk);
                    // insert a new blk
                    // warning: blk, cur, begin, end may be invalid
                    if (blk != newblk + 1) {
                        blk = newblk + 1;
                        start = (*blk)->start();
                        cur = start + cur_idx;
                        begin = start + begin_idx;
                        end = start + end_idx;
                    }
                    *newblk = new block();
                    (*newblk)->assign(begin, cur, (*newblk)->begin());
                    mfwu::destroy(begin, cur - 1);
                    *(cur - 1) = val;
                    (*blk)->begin_ = cur - 1;  // TODO: offer op IN THE BLOCK
                }
            } else { // lets move back part to next block
                if (blk < this->end_ - 1 && (*(blk + 1))->has_front_space()) {
                    (*(blk + 1))->push_front(*(end - 1));
                    if (cur != end - 1) {
                        // TODO: vector has its own backward copy, can we place them in algo?
                        mfwu::copy_backward(cur, end - 2, cur + 1);
                    }
                    *cur = val;
                } else {
                    // if blk + 1 == end should work
                    pblock* newblk = insert_before_block(blk + 1);
                    if (blk != newblk - 1) {
                        blk = newblk - 1;
                        start = (*blk)->start();
                        cur = start + cur_idx;
                        begin = start + begin_idx;
                        end = start + end_idx;
                    }
                    *newblk = new block();
                    (*newblk)->assign(cur, end, (*newblk)->begin());
                    mfwu::destroy(cur + 1, end);
                    *cur = val;
                    (*blk)->end_ = cur + 1;  // TODO: check
                }
            }
        } else {
            value_type* cur = it.get_cur();
            if ((*blk)->has_front_space() && (*blk)->has_back_space()) {
                if (cur - (*blk)->begin() < (*blk)->size() / 2) {
                    (*blk)->insert(cur, val, true);
                } else {
                    (*blk)->insert(cur, val, false);
                }
            } else if ((*blk)->has_front_space()) {
                (*blk)->insert(cur, val, true);
            } else if ((*blk)->has_back_space()) {
                (*blk)->insert(cur, val, false);
            } else {
                std::cout << "impossible to come here\n";  // TODO
            }
        }
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        if (last - first > BLK_SIZE) {
            // split 
            // insert_block
        } else if (last - first > 
            (*it.get_pos())->front_space() + (*it.get_pos())->back_space()) {
            // split
            // block::insert
        } else {
            // block::insert
        }
    }

    void erase(iterator it) {
        pblock* blk = it.get_pos();
        (*blk)->erase(it.get_cur());
        if ((*blk)->empty()) {
            erase_block(blk);
        }
    }
    void erase(iterator first, iterator last) {
        pblock* first_blk = first.get_pos();
        pblock* last_blk = last.get_pos();
        if (first_blk == last_blk) {
            (*first_blk)->erase(first.get_cur(), last.get_cur());
            if ((*first_blk)->empty()) { erase_block(first_blk); }
        } else {
            (*first_blk)->erase(first.get_cur(), (*first_blk)->end());
            (*last_blk)->erase((*last_blk)->begin(), last.get_cur());
            erase_block(first_blk + !(*first_blk)->empty(),
                        last_blk - !(*last_blk)->empty() + 1);
        }
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
        init_ctrl(last - first);
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
    void reinit() {
        destroy();
        deallocate();
        // reset_pblock();
    }
    void destroy() {
        // mfwu::destroy(begin_, end_ + 1);
        if (end_ == nullptr) return ;
        for (pblock* blk = begin_; blk <= end_; ++blk) {
            mfwu::destroy(*blk);
        }
    }
    void deallocate() {
        free(ctrl_);
    }
    void reset_pblock() {
        ctrl_ = last_ = begin_ = end_ = nullptr;
    }

    void push_front_block() {
        if (begin_ > ctrl_) {
            --begin_;
            *begin_ = new block(BLK_SIZE, true);
        } else {
            req_mem_front();
            push_front_block();
        }
    }
    void push_back_block() {
        if (end_ < last_) {
            mfwu::destroy(*end_);
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
    void pop_back_block() {
        if (begin_ < end_) {
            mfwu::destroy(*end_);
            --end_;
            mfwu::destroy(*end_);
            init_dummy_block();
        }
    }
    void erase_block(pblock* blk) {
        mfwu::destroy(*blk);
        if (blk - begin_ < end_ - blk) {
            copy_backward(begin_, blk, begin_ + 1);
            *begin_ = nullptr;
            ++begin_;
        } else {
            copy(blk + 1, end_, blk);
            mfwu::destroy(*end_);
            --end_;
            init_dummy_block();
        }
    }
    void erase_block(pblock* first, pblock* last) {
        // TODO: try to abstract a new func 
        // with move_forward/backward && block ranged erase
        assert(first >= begin_ && end_ >= last);
        for (pblock* blk = first; blk < last; ++blk) {
            mfwu::destroy(*blk);
        }
        //  |-----|----|------|
        //       |-----|------|
        if (first - begin_ < end_ - last) {
            pblock* new_begin = begin_ + (last - first);
            mfwu::copy_backward(begin_, first, new_begin);
            mfwu::fill(begin_, new_begin, nullptr);
            begin_ = new_begin;
        } else {
        //  |------|----|-----|
        //  |------|-----|
            pblock* new_end = end_ - (last - first);
            mfwu::copy(last, end_, first);
            mfwu::fill(new_end, end_, nullptr);
            end_ = new_end;
        }
    }

    void req_mem_front() {
        size_type original_capacity = last_ - ctrl_;
        size_type begin_idx = begin_ - ctrl_;
        size_type end_idx = end_ - ctrl_;
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
    void req_mem_back() {
        size_type original_capacity = last_ - ctrl_;
        size_type begin_idx = begin_ - ctrl_;
        size_type end_idx = end_ - ctrl_;
        size_type new_capacity = original_capacity * 2 + 1;
        pblock* new_ctrl = (pblock*)malloc(sizeof(pblock) * new_capacity);
        mfwu::uninitialized_copy(  // copy all these pblocks to new ctrl
            begin_, end_ + 1, new_ctrl + begin_idx);
        free(ctrl_);
        ctrl_ = new_ctrl;
        last_ = new_ctrl + new_capacity;
        begin_ = ctrl_ + begin_idx;
        end_ = ctrl_ + end_idx;
    }
    // dont put the iterators here
    // bcz the blk insert may invalidate them
    // 0812-X
    // pblock* insert_before_block(pblock* blk,
    //     block::iterator first, block::iterator last) {
    //     if (begin_ > ctrl_ && end_ < last_) {
    //         if (blk - begin_ < end_ - blk) {
    //             pblock* pp = move_forward(blk);
    //             (*pp)->construct(first, last);
    //         } else {
    //             pblock* pp = move_backward(blk);
    //             (*pp)->construct(first, last);
    //         }
    //     } else if (begin_ > ctrl_) {
    //         pblock* pp = move_forward(blk);
    //         (*pp)->construct(first, last);
    //     } else if (end_ < last_) {
    //         pblock* pp = move_backward(blk);
    //         (*pp)->construct(first, last);
    //     } else {
    //         int idx = blk - ctrl_;
    //         req_mem_back();
    //         return insert_before_block(ctrl_ + idx, first, last);
    //     }
    // }

    pblock* split(iterator it) {
        // assert(it.get_cur() != it.get_begin()
        //        && it.get_cur() != it.get_end());
        if (it.get_cur() - it.get_begin() < BLK_SIZE / 2) {
            // move front part to prev block
            pblock* newblk = insert_before_block(it.get_pos());  // insert a new blk
            it = update_iter(it, newblk + 1);
            *newblk = new block();
            (*newblk)->assign(it.get_begin(), it.get_cur(), (*newblk)->begin());
            mfwu::destroy(it.get_begin(), it.get_cur());
            (*it.get_pos())->set_begin(it.get_cur());
        } else { // move back part to next block
            // blk + 1 == end should work
            pblock* newblk = insert_before_block(it.get_pos() + 1);
            it = update_iter(it, newblk - 1);
            *newblk = new block();
            (*newblk)->assign(it.get_cur(), it.get_end(), (*newblk)->begin());
            mfwu::destroy(it.get_cur(), it.get_end());
            (*it.get_pos())->set_end(it.get_cur());  // TODO: check
        }
    }
    iterator update_iter(iterator it, pblock* realloc_blk) {
        pblock* original_blk = it.get_pos();
        value_type* original_start = (*original_blk)->start();
        value_type* realloc_start = (*realloc_blk)->start();
        if (original_blk == realloc_blk) return ;  // not changed
        return iterator(realloc_start + (it.get_cur() - original_start),
                        realloc_start + (it.get_begin() - original_start),
                        realloc_start + (it.get_end() - original_start),
                        realloc_blk);
    }

    // just insert a pblock before that blk
    // return the pblock* that point to new pblock
    pblock* insert_before_block(pblock* blk) {
        if (begin_ > ctrl_ && end_ < last_) {
            if (blk - begin_ < end_ - blk) {
                return move_forward(blk);  // *ret = nullptr
            } else { 
                return move_backward(blk);
            }
        } else if (begin_ > ctrl_) {
            return move_forward(blk);
        } else if (end_ < last_) {
            return move_backward(blk);
        } else {
            int idx = blk - ctrl_;
            req_mem_front();
            return insert_before_block(ctrl_ + idx);
        }
    }
    pblock* move_forward(pblock* blk) {
        pblock* pp = begin_;
        for ( ; pp != blk; ++pp) {
            *(pp - 1) = *pp;
            // bcz *pp is of type pblock
            // it is a raw ptr
            // need no constructor
        }
        *(pp - 1) = nullptr;
        // stop manage any block
        --begin_;
        return pp - 1;
    }
    pblock* move_backward(pblock* blk) {
        pblock* pp = end_;
        for ( ; pp != blk; --pp) {
            *pp = *(pp - 1);
        }
        *pp = nullptr;
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
};  // endof class deque


// NOTE: block is a member of deque
// should not use it everywhere outside
// and its mem is fixed
// user should ensure the operation is valid
// by themselves  X 8.15
template <typename T, size_t BLK_SIZE, typename Alloc>
struct deque<T, BLK_SIZE, Alloc>::block {
public:
    // friend class mfwu::deque<T, Alloc>;  // TODO

    using value_type = T;
    using iterator = value_type*;
    using size_type = size_t;
    // allocate but no elements
    // if specify n != 0, we provide BLK_SIZE
    //            n == 0, dummy node and no mem is alloc
    block(int n=BLK_SIZE, bool reverse=false) 
        : blk_(allocator_.allocate(n ? BLK_SIZE : 0)),
          last_(n ? blk_ + BLK_SIZE : blk_), 
          begin_(reverse ? last_ : blk_), end_(reverse ? last_ : blk_) {}
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
        last_ = blk_ + BLK_SIZE;
        if (reverse) {
            end_ = last_;
            begin_ = end_ - n;
        } else {
            begin_ = blk_;
            end_ = blk_ + n;
        }
        mfwu::construct(begin_, end_, val);
    }
    block(const block& blk): blk_(allocator_.allocate(BLK_SIZE)),
                             last_(blk_ + BLK_SIZE), 
                             begin_(blk_ + (blk.begin_ - blk.blk_)), 
                             end_(blk_ + (blk.end_ - blk.blk_)) {
        mfwu::uninitialized_copy(blk.begin_, blk.end_, begin_);
    }
    block(block&& blk) : blk_(blk.blk_), last_(blk.last_), 
                         begin_(blk.begin_), end_(blk.end_) {
        blk.blk_ = blk.last_ = nullptr;
        blk.begin_ = blk.end_ = nullptr;
    }
    // destroy and deallocate
    ~block() {
        // std::cout << "last - blk = " << last_ - blk_ << "\n";
        // std::cout << last_ << "  " << blk_ << "\n";
        // std::cout << begin_ << "  " << end_ << "\n";
        mfwu::destroy(begin_, end_);
        allocator_.deallocate(blk_, last_ - blk_);
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
        // std::cout << begin_ << " " << last_ << "\n";
        // std::cout << last_ - begin_ << "\n";
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
    // only for new block
    template <typename InputIterator>
    void assign(InputIterator first, InputIterator last, iterator res) {
        iterator res_end = mfwu::uninitialized_copy(first, last, res);
        assert(res >= blk_ && res_end <= last_);
        begin_ = res;  // dangerous
        end_ = res_end;
    }
    // TODO: iterator may be invalidated, plz return a new iterator
    void insert(iterator it, const value_type& val, bool is_move_forward) {
        // TODO: we should add is_move_forward back
        // and return the real inserted it pos
        // or in deque, if you want to move_forward
        // we should insert in the it + 1 pos
        if (is_move_forward) {
            assert(has_front_space());
            mfwu::construct(begin_ - 1, *begin_);
            // mfwu::copy(begin_ + 1, it, begin_);
            // if we use mfwu::copy, we should make sure begin_ + 1 <= it
            // or we can write a copy specially for <
            mfwu::copy(begin_ + 1, it, begin_);
            --begin_;
            *(it - 1) = val;
        } else {
            assert(has_back_space());
            mfwu::construct(end_, *(end_ - 1));
            mfwu::copy_backward(it, end_ - 1, it + 1);
            ++end_;
            *it = val;
        }
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        
    }
    void erase(iterator it) {
        try {
            assert(begin_ < end_);
        } catch(...) {
            std::cout << "erase it in empty block\n \
                check empty blocks or dummy block\n";
        }  // TODO
        
        if (it - begin_ < end_ - it) {
            copy_backward(begin_, it, begin_ + 1);
            mfwu::destroy(begin_++);
        } else {
            copy(it + 1, end_, it);
            mfwu::destroy(--end_);
        }
    }
    void erase(iterator first, iterator last) {
        assert(first >= begin_ && end_ >= last);
        for (pblock* it = first; it < last; ++bit) {
            mfwu::destroy(it);
        }
        //  |-----|----|------|
        //       |-----|------|
        if (first - begin_ < end_ - last) {
            pblock* new_begin = begin_ + (last - first);
            mfwu::copy_backward(begin_, first, new_begin);
            mfwu::destroy(begin_, new_begin);
            begin_ = new_begin;
        } else {
        //  |------|----|-----|
        //  |------|-----|
            pblock* new_end = end_ - (last - first);
            mfwu::copy(last, end_, first);
            mfwu::destroy(new_end, end_);
            end_ = new_end;
        }
    }

    void set_begin(iterator b) { begin_ = b; }
    void set_end(iterator e) { end_ = e; }

    size_type size() const { return end_ - begin_; }
    size_type capacity() const { return last_ - blk_; }
    bool empty() const { return end_ == begin_; }
    iterator begin() const { return begin_; }
    iterator end() const { return end_; }
    value_type* start() const { return blk_; }
    value_type* finish() const { return last_; }
    value_type& front() const { return begin_[0]; }
    value_type& back() const { return begin_[size() - 1]; }

    size_type front_space() const { return begin_ - blk_; }
    size_type back_space() const { return last_ - end_; }
    bool has_front_space() const { return begin_ > blk_; }
    bool has_back_space() const { return last_ > end_; }
// private:
    value_type* blk_;
    value_type* last_;
    iterator begin_, end_;
    Alloc allocator_;
};  // endof struct block

}  // endof namespace mfwu




#endif  // __DEQUE_HPP__
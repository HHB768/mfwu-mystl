#ifndef __LIST_HPP__
#define __LIST_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {

template <typename T>
struct forward_linked_node {
    using value_type = T;
    using node = forward_linked_node;
    value_type val;
    node* next = nullptr;
    forward_linked_node() : val(), next(nullptr) {}
    forward_linked_node(const value_type& value) 
        : val(value), next(nullptr) {}
    forward_linked_node(const value_type& value, 
                        node* nextptr) 
        : val(value), next(nextptr) {}
};  // endof struct forward_linked_node

template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class ForwardLinkedList {
public:
    class list_iterator 
        : public mfwu::iterator<T, mfwu::forward_iterator_tag> {
    public:
        using Iter = mfwu::iterator<T, mfwu::forward_iterator_tag>;
        using value_type = typename Iter::value_type;
        using iterator_category = typename Iter::iterator_category;
        using pointer = typename Iter::pointer;
        using reference = typename Iter::reference;
        using difference_type = typename Iter::difference_type; 

        list_iterator() : ptr_(nullptr) {}
        list_iterator(pointer p) : ptr_(p) {}
        list_iterator(const list_iterator& it) : ptr_(it.ptr_) {}
        list_iterator(list_iterator&& it) : ptr_(std::move(it.ptr_)) {
            it.ptr_ = nullptr;
        }
        ~list_iterator() {}
        list_iterator& operator=(const list_iterator& it) {
            ptr_ = it.ptr_;
        }
        list_iterator& operator=(list_iterator&& it) {
            ptr_ = mfwu::move(it.ptr_);
            it.ptr_ = nullptr;
        }

        typename value_type::value_type operator*() const {
            return ptr_->val;
        }
        typename value_type::value_type* operator->() const {
            return & this->operator*();
        }

        list_iterator& operator++() {
            ptr_ = ptr_->next;
            return *this;
        }
        list_iterator operator++(int) {
            list_iterator tmp = *this;
            ptr_ = ptr_->next;
            return mfwu::move(tmp);
        }

        bool operator==(const list_iterator& it) const {
            return ptr_ == it.ptr_;
        }
        bool operator!=(const list_iterator& it) const {
            return !(*this == it);
        }
        value_type* get_ptr() {
            return ptr_;
        }
    private:
        pointer ptr_;
    };  // endof class list_iterator
    using node = forward_linked_node<T>;
    using iterator = list_iterator<node>;
    using value_type = T;
    using size_type = size_t;

    // TODO: what is the diff between new and alloc & construct
    ForwardLinkedList() : head_(new node(42)), tail_(new node(6)) /*allocator_()*/ {
        connect(head_, tail_);
    }
    ForwardLinkedList(size_type n, const value_type& val=value_type()) 
        : head_(new node(42)), tail_(new node(6)) {
        node* prev = head_;
        for (int i = 0; i < n; i++) {
            node* newnode = new node(val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    template <typename InputIterator>
    ForwardLinkedList(InputIterator first, InputIterator last) 
        : head_(new node(42)), tail_(new node(6)) {
        node* prev = head_;
        for ( ; first != last; first++) {
            node* newnode = new node(*first, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    ForwardLinkedList(const std::initializer_list<value_type>& values)
        : head_(new node(42)), tail_(new node(6)) {
        node* prev = head_;
        for (const value_type& val : values) {
            node* newnode = new node(val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    ForwardLinkedList(const ForwardLinkedList& lst) 
        : head_(new node(*lst.head_)), tail_(new node(*lst.tail_)) {
        node* prev = head_;
        for (node* lstnode = lst.head_->next;
             lstnode != lst.tail_; 
             lstnode = lstnode->next) {
            node* newnode = new node(*lstnode, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    ForwardLinkedList(ForwardLinkedList&& lst) 
        : head_(lst.head_), tail_(lst.tail_) {
        lst.reset_ends();
    }
    ~ForwardLinkedList() {
        destroy();
    }
    ForwardLinkedList& operator=(const ForwardLinkedList& lst) {
        reset_and_copy(lst, *this);
    }
    ForwardLinkedList& operator=(ForwardLinkedList&& lst) {
        reset_and_move(lst, *this);
    }

    value_type& front() {
        return head_->next->val;
    }
    value_type back() {
        return tail_->prev->val;
    }
    iterator begin() {
        return head_->next;
    }
    iterator end() {
        return tail_;
    }
    
    bool empty() {
        return head_->next == tail_;
    }
    size_type size() {
        return mfwu::distance(begin_, end_);  // TODO
    }

    void resize(size_type ref_size) {
        node* cur = head_;
        for (int i = 0; i < ref_size; i++, cur = cur->next) {
            if (cur->next == tail_) {
                int n = ref_size - i;
                cur = add_n_nodes(cur, n, value_type());
                connect(cur, tail_);
                break;
            }
        }
        node* prev = cur->prev;
        destroy(cur, tail_);
        connect(prev, tail_)
    }

    void push_front(const value_type& val) {
        node* newnode = new node(val, head_, head_->next);
        head_->next->prev = newnode;
        head_->next = newnode;
    }
    void push_back(const value_type& val) {
        node* newnode = new node(val, tail_->prev, tail);
        tail_->prev->next = newnode;
        tail_->prev = newnode;
    }
    void pop_front() {
        if (empty()) return ;
        node* next = head_->next->next;
        delete head_->next;
        connect(head_, next);
    }
    void pop_back() {
        if (empty()) return ;
        node* prev = tail_->prev->prev;
        delete tail_->prev;
        connect(prev, tail_);
    }
    void insert(iterator it, const value_type& val) {
        node* cur = it.get_ptr();
        node* newnode = new node(val, cur->prev, cur);
        cur->prev->next = newnode;
        cur->prev = newnode;
    }
    void insert(iterator it, const value_type& val, size_type n) {
        node* prev = it.get_ptr()->prev;
        node* next = prev->next;
        prev = add_n_nodes(prev, n, val);
        connect(prev, next);
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        node* next = it.get_ptr();
        node* prev = next->prev;
        for ( ; first != last; first++) {
            node* newnode = new node(*first, prev, nullptr);
            prev = newnode;
        }
        connect(prev, next);
    }
    void erase(iterator it) {
        node* cur = it.get_ptr();
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        delete cur;
    }
    void erase(iterator first, iterator last) {
        node* prev = first.get_ptr()->prev;
        node* next = last.get_ptr();
        for (node* cur = prev->next; cur != next; ) {
            cur = cur->next;
            delete cur->prev;
        }
        connect(prev, next);
    }


private:
    void connect(node* former, node* latter) {
        former->next = latter;
        latter->prev = former;
    }
    void reset_ends() {
        head_ = nullptr;
        tail_ = nullptr;
    }
    void destroy() {
        for ( ; head_ != tail_; ) {
            node* next = head_->next;
            delete head_;
            head_ = next;
        }
        delete tail_;
    }
    void reinit() {
        destroy();
        reset_ends();
    }
    static void reset_and_copy(const DoubleLinkedList& src, DoubleLinkedList& dst) {
        dst.destroy();
        dst.head_ = new node(*src.head_);
        dst.tail_ = new node(*src.tail_);
        node* prev = dst.head_;
        for (node* srcnode = src.head_->next;
             srcnode != src.tail_; 
             srcnode = srcnode->next) {
            node* dstnode = new node(*srcnode, prev, nullptr);
            prev->next = dstnode;
            prev = dstnode;
        }
        connect(prev, dst.tail_);
    }
    static void reset_and_move(DoubleLinkedList& src, DoubleLinkedList& dst) {
        dst.destroy();
        dst.head_ = src.head_;
        dst.tail_ = src.tail_;
        src.reset_ends();
    }
    node* head_;
    node* tail_;
    Alloc allocator_;

};  // endof class ForwardLinkedList

template <typename T>
struct double_linked_node {
    using value_type = T;
    using node = double_linked_node;
    value_type val;
    node* prev = nullptr;
    node* next = nullptr;
    double_linked_node() : val(), prev(nullptr), next(nullptr) {}
    double_linked_node(const value_type& value) 
        : val(value), prev(nullptr), next(nullptr) {}
    double_linked_node(const value_type& value, 
                       node* prevptr, node* nextptr)
        : val(value), prev(prevptr), next(nextptr) {} 
};  // endof struct double_linked_node

// class mfwu::unit_test_list;

template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class DoubleLinkedList {
public:
    class list_iterator 
        : public mfwu::iterator<T, mfwu::bidirectional_iterator_tag> {
    public:
        using Iter = mfwu::iterator<T, mfwu::bidirectional_iterator_tag>;
        using value_type = typename Iter::value_type;
        using iterator_category = typename Iter::iterator_category;
        using pointer = typename Iter::pointer;
        using reference = typename Iter::reference;
        using difference_type = typename Iter::difference_type; 

        list_iterator() : ptr_(nullptr) {}
        list_iterator(pointer p) : ptr_(p) {}
        list_iterator(const list_iterator& it) : ptr_(it.ptr_) {}
        list_iterator(list_iterator&& it) : ptr_(std::move(it.ptr_)) {
            it.ptr_ = nullptr;
        }
        ~list_iterator() {}
        list_iterator& operator=(const list_iterator& it) {
            ptr_ = it.ptr_;
        }
        list_iterator& operator=(list_iterator&& it) {
            ptr_ = mfwu::move(it.ptr_);
            it.ptr_ = nullptr;
        }

        typename value_type::value_type operator*() const {
            return ptr_->val;
        }
        typename value_type::value_type* operator->() const {
            return & this->operator*();
        }

        list_iterator& operator++() {
            ptr_ = ptr_->next;
            return *this;
        }
        list_iterator& operator--() {
            ptr_ = ptr_->prev;
            return *this;
        }
        list_iterator operator++(int) {
            list_iterator tmp = *this;
            ptr_ = ptr_->next;
            return mfwu::move(tmp);
        }
        list_iterator operator--(int) {
            list_iterator tmp = *this;
            ptr_ = ptr_->prev;
            return mfwu::move(tmp);
        }

        bool operator==(const list_iterator& it) const {
            return ptr_ == it.ptr_;
        }
        bool operator!=(const list_iterator& it) const {
            return !(*this == it);
        }
        value_type* get_ptr() {
            return ptr_;
        }
    private:
        pointer ptr_;
    };  // endof class list_iterator
    using node = double_linked_node<T>;
    using iterator = list_iterator<node>;
    using value_type = T;
    using size_type = size_t;

    // TODO: what is the diff between new and alloc & construct
    DoubleLinkedList() : head_(new node(42)), tail_(new node(6)) /*allocator_()*/ {
        connect(head_, tail_);
    }
    DoubleLinkedList(size_type n, const value_type& val=value_type()) 
        : head_(new node(42)), tail_(new node(6)) {
        node* prev = head_;
        for (int i = 0; i < n; i++) {
            node* newnode = new node(val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    template <typename InputIterator>
    DoubleLinkedList(InputIterator first, InputIterator last) 
        : head_(new node(42)), tail_(new node(6)) {
        node* prev = head_;
        for ( ; first != last; first++) {
            node* newnode = new node(*first, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    DoubleLinkedList(const std::initializer_list<value_type>& values)
        : head_(new node(42)), tail_(new node(6)) {
        node* prev = head_;
        for (const value_type& val : values) {
            node* newnode = new node(val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    DoubleLinkedList(const DoubleLinkedList& lst) 
        : head_(new node(*lst.head_)), tail_(new node(*lst.tail_)) {
        node* prev = head_;
        for (node* lstnode = lst.head_->next;
             lstnode != lst.tail_; 
             lstnode = lstnode->next) {
            node* newnode = new node(*lstnode, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, tail_);
    }
    DoubleLinkedList(DoubleLinkedList&& lst) 
        : head_(lst.head_), tail_(lst.tail_) {
        lst.reset_ends();
    }
    ~DoubleLinkedList() {
        destroy();
    }
    DoubleLinkedList& operator=(const DoubleLinkedList& lst) {
        reset_and_copy(lst, *this);
    }
    DoubleLinkedList& operator=(DoubleLinkedList&& lst) {
        reset_and_move(lst, *this);
    }

    value_type& front() {
        return head_->next->val;
    }
    value_type back() {
        return tail_->prev->val;
    }
    iterator begin() {
        return head_->next;
    }
    iterator end() {
        return tail_;
    }
    
    bool empty() {
        return head_->next == tail_;
    }
    size_type size() {
        return mfwu::distance(begin_, end_);
    }

    void resize(size_type ref_size) {
        node* cur = head_;
        for (int i = 0; i < ref_size; i++, cur = cur->next) {
            if (cur->next == tail_) {
                int n = ref_size - i;
                cur = add_n_nodes(cur, n, value_type());
                connect(cur, tail_);
                break;
            }
        }
        node* prev = cur->prev;
        destroy(cur, tail_);
        connect(prev, tail_)
    }

    void push_front(const value_type& val) {
        node* newnode = new node(val, head_, head_->next);
        head_->next->prev = newnode;
        head_->next = newnode;
    }
    void push_back(const value_type& val) {
        node* newnode = new node(val, tail_->prev, tail);
        tail_->prev->next = newnode;
        tail_->prev = newnode;
    }
    void pop_front() {
        if (empty()) return ;
        node* next = head_->next->next;
        delete head_->next;
        connect(head_, next);
    }
    void pop_back() {
        if (empty()) return ;
        node* prev = tail_->prev->prev;
        delete tail_->prev;
        connect(prev, tail_);
    }
    void insert(iterator it, const value_type& val) {
        node* cur = it.get_ptr();
        node* newnode = new node(val, cur->prev, cur);
        cur->prev->next = newnode;
        cur->prev = newnode;
    }
    void insert(iterator it, const value_type& val, size_type n) {
        node* prev = it.get_ptr()->prev;
        node* next = prev->next;
        prev = add_n_nodes(prev, n, val);
        connect(prev, next);
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        node* next = it.get_ptr();
        node* prev = next->prev;
        for ( ; first != last; first++) {
            node* newnode = new node(*first, prev, nullptr);
            prev = newnode;
        }
        connect(prev, next);
    }
    void erase(iterator it) {
        node* cur = it.get_ptr();
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        delete cur;
    }
    void erase(iterator first, iterator last) {
        node* prev = first.get_ptr()->prev;
        node* next = last.get_ptr();
        for (node* cur = prev->next; cur != next; ) {
            cur = cur->next;
            delete cur->prev;
        }
        connect(prev, next);
    }


private:
    void connect(node* former, node* latter) {
        former->next = latter;
        latter->prev = former;
    }
    void reset_ends() {
        head_ = nullptr;
        tail_ = nullptr;
    }
    void destroy() {
        for ( ; head_ != tail_; ) {
            node* next = head_->next;
            delete head_;
            head_ = next;
        }
        delete tail_;
    }
    void reinit() {
        destroy();
        reset_ends();
    }
    static void reset_and_copy(const DoubleLinkedList& src, DoubleLinkedList& dst) {
        dst.destroy();
        dst.head_ = new node(*src.head_);
        dst.tail_ = new node(*src.tail_);
        node* prev = dst.head_;
        for (node* srcnode = src.head_->next;
             srcnode != src.tail_; 
             srcnode = srcnode->next) {
            node* dstnode = new node(*srcnode, prev, nullptr);
            prev->next = dstnode;
            prev = dstnode;
        }
        connect(prev, dst.tail_);
    }
    static void reset_and_move(DoubleLinkedList& src, DoubleLinkedList& dst) {
        dst.destroy();
        dst.head_ = src.head_;
        dst.tail_ = src.tail_;
        src.reset_ends();
    }
    node* head_;
    node* tail_;
    Alloc allocator_;

};  // endof class DoubleLinkedList
template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
using forward_list = mfwu::ForwardLinkedList<T, Alloc>;
template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
using list = mfwu::DoubleLinkedList<T, Alloc>;

}  // endof namespace mfwu

#endif  // __LIST_HPP__
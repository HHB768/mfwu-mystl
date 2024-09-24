#ifndef __LIST_HPP__
#define __LIST_HPP__

#include "allocator.hpp"
#include "iterator.hpp"
#include "utils.hpp"

namespace mfwu {
// TODO: code changed, re-check

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
    template <typename Node>
    class list_iterator 
        : public mfwu::iterator<Node, mfwu::forward_iterator_tag> {
    public:
        using Iter = mfwu::iterator<Node, mfwu::forward_iterator_tag>;
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
            return *this;
        }
        list_iterator& operator=(list_iterator&& it) {
            ptr_ = mfwu::move(it.ptr_);
            it.ptr_ = nullptr;
            return *this;
        }

        typename value_type::value_type& operator*() const {
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
        value_type* get_ptr() const {
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
    ForwardLinkedList() : head_(new node(42)) /*allocator_()*/ {}
    ForwardLinkedList(size_type n, const value_type& val=value_type()) 
        : head_(new node(42)) {
        node* prev = head_;
        for (int i = 0; i < n; i++) {
            node* newnode = new node(val);
            prev->next = newnode;
            prev = newnode;
        }
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<is_base_of_template<
                mfwu::iterator, InputIterator>::value>
             >
    ForwardLinkedList(InputIterator first, InputIterator last) 
        : head_(new node(42)) {
        node* prev = head_;
        for ( ; first != last; first++) {
            node* newnode = new node(*first);
            prev->next = newnode;
            prev = newnode;
        }
    }
    ForwardLinkedList(const std::initializer_list<value_type>& values)
        : head_(new node(42)) {
        node* prev = head_;
        for (const value_type& val : values) {
            node* newnode = new node(val);
            prev->next = newnode;
            prev = newnode;
        }
    }
    ForwardLinkedList(const ForwardLinkedList& lst) 
        : head_(new node(*lst.head_)) {
        node* prev = head_;
        for (node* lstnode = lst.head_->next;
             lstnode != nullptr; 
             lstnode = lstnode->next) {
            node* newnode = new node(*lstnode);
            prev->next = newnode;
            prev = newnode;
        }
    }
    ForwardLinkedList(ForwardLinkedList&& lst) 
        : head_(lst.head_) {
        lst.head_ = nullptr;
    }
    ~ForwardLinkedList() {
        destroy();
    }
    ForwardLinkedList& operator=(const ForwardLinkedList& lst) {
        reset_and_copy(lst, *this);
        return *this;
    }
    ForwardLinkedList& operator=(ForwardLinkedList&& lst) {
        reset_and_move(lst, *this);
        return *this;
    }

    value_type& front() const {
        return head_->next->val;
    }
    value_type back() const {
        node* cur = head_->next;
        for ( ; cur->next != nullptr; cur = cur->next) {}
        return cur->val;
    }
    iterator begin() const {
        return head_->next;
    }
    iterator end() const {
        return nullptr;
    }
    
    bool empty() const {
        return head_->next == nullptr;
    }
    size_type size() const {
        size_type n = 0;
        for (node* cur = head_->next; 
             cur != nullptr;
             cur = cur->next, ++n) {}
        return n;
    }

    void resize(size_type ref_size) {
        node* cur = head_;
        for (int i = 0; i < ref_size; i++, cur = cur->next) {
            if (cur->next == nullptr) {
                int n = ref_size - i;
                cur = add_n_nodes(cur, n, value_type());
                return ;
            }
        }
        destroy(cur);
    }

    void push_front(const value_type& val) {
        node* newnode = new node(val, head_->next);
        head_->next = newnode;
    }
    void push_back(const value_type& val) {
        node* cur = head_;
        for ( ; cur->next != nullptr; cur = cur->next) {}
        node* newnode = new node(val);
        cur->next = newnode;
    }
    void pop_front() {
        if (empty()) return ;
        node* next = head_->next->next;
        delete head_->next;
        head_->next = next;
    }
    void pop_back() {
        if (empty()) return ;
        node* prev = head_;
        for ( ; prev->next->next != nullptr; prev = prev->next) {}
        delete prev->next;
        prev->next = nullptr;
    }
    void insert(iterator it, const value_type& val) {
        node* prev = head_;
        for ( ; prev->next != it.get_ptr(); prev = prev->next) {}
        node* newnode = new node(val, prev->next);
        prev->next = newnode;
    }
    void insert(iterator it, const value_type& val, size_type n) {
        node* prev = head_;
        for ( ; prev->next != it.get_ptr(); prev = prev->next) {}
        node* next = prev->next;
        prev = add_n_nodes(prev, n, val);
        prev->next = next;
    }
    template <typename InputIterator>
    void insert(iterator it, InputIterator first, InputIterator last) {
        node* prev = head_;
        for ( ; prev->next != it.get_ptr(); prev = prev->next) {}
        node* next = prev->next;
        for ( ; first != last; first++) {
            node* newnode = new node(*first, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        prev->next = next;
    }
    void erase(iterator it) {
        node* prev = head_;
        for ( ; prev->next != it.get_ptr(); prev = prev->next) {}
        node* next = prev->next->next;
        delete prev->next;
        prev->next = next;
    }
    void erase(iterator first, iterator last) {
        node* prev = head_;
        for ( ; prev->next != first.get_ptr(); prev = prev->next) {}
        for (node* cur = prev->next; cur != last.get_ptr(); ) {
            node* next = cur->next;
            delete cur;
            cur = next;
        }
        prev->next = last.get_ptr();
    }
private:
    void destroy() {
        for ( ; head_ != nullptr; ) {
            node* next = head_->next;
            delete head_;
            head_ = next;
        }
    }
    void destroy(node* prev) {
        for (node* cur = prev->next; cur != nullptr; ) {
            node* next = cur->next;
            delete cur;
            cur = next;
        }
        prev->next = nullptr;
    }
     static void reset_and_copy(const ForwardLinkedList& src, ForwardLinkedList& dst) {
        dst.destroy();
        dst.head_ = new node(*src.head_);
        node* prev = dst.head_;
        for (node* srcnode = src.head_->next;
             srcnode != nullptr; 
             srcnode = srcnode->next) {
            node* dstnode = new node(*srcnode);
            prev->next = dstnode;
            prev = dstnode;
        }
    }
    static void reset_and_move(ForwardLinkedList& src, ForwardLinkedList& dst) {
        dst.destroy();
        dst.head_ = src.head_;
        src.head_ = nullptr;
    }
    node* add_n_nodes(node* prev, int n, const typename node::value_type& val) {
        for (int i = 0; i < n; i++) {
            prev->next = new node(val);
            prev = prev->next;
        }
        return prev;
    }
    node* head_;
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

// TODO: list is circular，which means that head_ and tail_ are actually same
template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
// TODO: you can allocate and deallocate mem with alloc, but i dont think it is necessary
class DoubleLinkedList {
public:
    template <typename Node>
    class list_iterator   // TODO: value_type can be T?
        : public mfwu::iterator<Node, mfwu::bidirectional_iterator_tag> {
    public:
        using Iter = mfwu::iterator<Node, mfwu::bidirectional_iterator_tag>;
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
            return *this;
        }
        list_iterator& operator=(list_iterator&& it) {
            ptr_ = mfwu::move(it.ptr_);
            it.ptr_ = nullptr;
            return *this;
        }

        typename value_type::value_type& operator*() const {
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
        value_type* get_ptr() const {
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
    template <typename InputIterator,
              typename = typename std::enable_if_t<is_base_of_template<
                    mfwu::iterator, InputIterator>::value>
             >
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
            node* newnode = new node(lstnode->val, prev, nullptr);
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
        return *this;
    }
    DoubleLinkedList& operator=(DoubleLinkedList&& lst) {
        reset_and_move(lst, *this);
        return *this;
    }

    value_type& front() const {
        return head_->next->val;
    }
    value_type back() const {
        return tail_->prev->val;
    }
    iterator begin() const {
        return head_->next;
    }
    iterator end() const {
        return tail_;
    }
    
    bool empty() const {
        return head_->next == tail_;
    }
    size_type size() const {
        return mfwu::distance(begin(), end());
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
        destroy(cur);
        connect(cur, tail_);
    }

    void push_front(const value_type& val) {
        node* newnode = new node(val, head_, head_->next);
        head_->next->prev = newnode;
        head_->next = newnode;
    }
    void push_back(const value_type& val) {
        node* newnode = new node(val, tail_->prev, tail_);
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
            prev->next = newnode;
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

    // TODO:
    // remove(val)
    // unique()
    // clear()
    // transfer(it, first, last)
    // splice(it, list)
    // reverse()
    // sort()
    // merge(list)

private:
    static void connect(node* former, node* latter) {
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
    void destroy(node* prev) {
        for (node* cur = prev->next; cur != tail_; ) {
            node* next = cur->next;
            delete cur;
            cur = next;
        }
        prev->next = tail_;
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
            node* dstnode = new node(srcnode->val, prev, nullptr);
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
    node* add_n_nodes(node* prev, int n, const typename node::value_type& val) {
        for (int i = 0; i < n; i++) {
            prev->next = new node(val, prev, nullptr);
            prev = prev->next;
        }
        return prev;
    }
    node* head_;
    node* tail_;
    Alloc allocator_;

};  // endof class DoubleLinkedList

// TODO: without test
template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
class CircularDoubleLinkedList {
public:
    using node = mfwu::double_linked_node<T>;
    using iterator = typename mfwu::DoubleLinkedList<T, Alloc>::list_iterator<node>;
    using value_type = T;
    using size_type = size_t;

    CircularDoubleLinkedList() : head_(new node(6)) {
        connect(head_, head_);
    }
    CircularDoubleLinkedList(size_type n, const value_type& val=value_type()) 
        : head_(new node(6)) {
        node* prev = head_;
        for (int i = 0; i < n; ++i) {
            node* newnode = new node(val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, head_);
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<is_base_of_template<
              mfwu::iterator, InputIterator>::value>>
    CircularDoubleLinkedList(InputIterator first, InputIterator last) 
        : head_(new node(6)) {
        node* prev = head_;
        for ( ; first != last; ++first) {
            node* newnode = new node(*first, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, head_);
    }
    CircularDoubleLinkedList(const std::initializer_list<value_type>& values)
        : head_(new node(6)) {
        node* prev = head_;
        for (const value_type& val : values) {
            node* newnode = new node(val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, head_);
    }
    CircularDoubleLinkedList(const CircularDoubleLinkedList& lst) 
        : head_(new node(*lst.head_)) {
        node* prev = head_;
        for (node* lstnode = lst.head_->next;
             lstnode != lst.head_; 
             lstnode = lstnode->next) {
            node* newnode = new node(lstnode->val, prev, nullptr);
            prev->next = newnode;
            prev = newnode;
        }
        connect(prev, head_);
    }
    CircularDoubleLinkedList(CircularDoubleLinkedList&& lst) 
        : head_(lst.head_) {
        lst.reset_ends();
    }
    ~CircularDoubleLinkedList() {
        destroy();
    }
    CircularDoubleLinkedList& operator=(const   CircularDoubleLinkedList& lst) {
        reset_and_copy(lst, *this);
        return *this;
    }
    CircularDoubleLinkedList& operator=(CircularDoubleLinkedList&& lst) {
        reset_and_move(lst, *this);
        return *this;
    }

    value_type& front() const {
        return head_->next->val;
    }
    value_type back() const {
        return head_->prev->val;
    }
    iterator begin() const {
        return head_->next;
    }
    iterator end() const {
        return head_;
    }
    
    bool empty() const {
        return head_->next == head_;
    }
    size_type size() const {
        return mfwu::distance(begin(), end());
        // TODO: C++11 asks list implementation should size() within O(1)
    }

    void resize(size_type ref_size) {
        node* cur = head_;
        for (int i = 0; i < ref_size; i++, cur = cur->next) {
            if (cur->next == head_) {
                int n = ref_size - i;
                cur = add_n_nodes(cur, n, value_type());
                connect(cur, head_);
                break;
            }
        }
        destroy(cur);
        connect(cur, head_);
    }

    void push_front(const value_type& val) {
        node* newnode = new node(val, head_, head_->next);
        head_->next->prev = newnode;
        head_->next = newnode;
    }
    void push_back(const value_type& val) {
        node* newnode = new node(val, head_->prev, head_);
        head_->prev->next = newnode;
        head_->prev = newnode;
    }
    void pop_front() {
        if (empty()) return ;
        node* next = head_->next->next;
        delete head_->next;
        connect(head_, next);
    }
    void pop_back() {
        if (empty()) return ;
        node* prev = head_->prev->prev;
        delete head_->prev;
        connect(prev, head_);
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
            prev->next = newnode;
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

    // TODO:
    void remove(const value_type& val) {
        for (node* cur = head_->next; cur != head_; ) {
            if (cur->val == val) {
                node* next = cur->val;
                delete cur;
                cur = next;
            } else {
                cur = cur->next;
            }
        }
    }
    void unique() {
        for (node* cur = head_->next; cur != head_; ) {
            while (cur->next != head_ 
                   && cur->next->val == cur->val) {
                node* next = cur->next->next;
                delete cur->next;
                cur->next = next;
            }
            cur = cur->next;
        }
    }
    void clear() {
        for (node* cur = head_->next ; cur != head_; ) {
            node* next = cur->next;
            delete next;
            cur = next;
        }
    }
    void transfer(iterator it, iterator first, iterator last) {
        node* cur = it.get_ptr();
        node* prev = cur->prev;
        node* first_prev = first.get_ptr()->prev;
        node* last_prev = last.get_ptr()->prev;
        connect(prev, first.get_ptr());
        connect(last_prev, cur);
        connect(first_prev, last.get_ptr());
    }
    // splice(it, list)
    void reverse() {
        for (node* cur = head_; cur != head_; ) {
            node* next = cur->next;
            cur->next = cur->prev;
            cur->prev = next;
            cur = next;
        }
    }
    // sort()
    // merge(list)

private:
    static void connect(node* former, node* latter) {
        former->next = latter;
        latter->prev = former;
    }
    void reset_ends() {
        head_ = nullptr;
    }
    void destroy() {
        for (node* cur = head_->next ; cur != head_; ) {
            node* next = cur->next;
            delete next;
            cur = next;
        }
        delete head_;
    }
    void destroy(node* prev) {
        for (node* cur = prev->next; cur != head_; ) {
            node* next = cur->next;
            delete cur;
            cur = next;
        }
        prev->next = head_;
    }
    void reinit() {
        destroy();
        reset_ends();
    }
    static void reset_and_copy(const CircularDoubleLinkedList& src, CircularDoubleLinkedList& dst) {
        dst.destroy();
        dst.head_ = new node(*src.head_);
        node* prev = dst.head_;
        for (node* srcnode = src.head_->next;
             srcnode != src.head_; 
             srcnode = srcnode->next) {
            node* dstnode = new node(srcnode->val, prev, nullptr);
            prev->next = dstnode;
            prev = dstnode;
        }
        connect(prev, dst.head_);
    }
    static void reset_and_move(CircularDoubleLinkedList& src, CircularDoubleLinkedList& dst) {
        dst.destroy();
        dst.head_ = src.head_;
        src.reset_ends();
    }
    node* add_n_nodes(node* prev, int n, const typename node::value_type& val) {
        for (int i = 0; i < n; i++) {
            prev->next = new node(val, prev, nullptr);
            prev = prev->next;
        }
        return prev;
    }
    node* head_;
    Alloc allocator_;

};  // endof class CircularDoubleLinkedList
template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
using forward_list = mfwu::ForwardLinkedList<T, Alloc>;
template <typename T, typename Alloc=mfwu::DefaultAllocator<T, mfwu::malloc_alloc>>
using list = mfwu::DoubleLinkedList<T, Alloc>;
// using list = mfwu::CircularDoubleLinkedList<T, Alloc>;  // without test

// TODO: more functions
// TODO: check memory leakage

}  // endof namespace mfwu

#endif  // __LIST_HPP__
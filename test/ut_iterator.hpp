#include "iterator.hpp"

#define __UNIT_TEST_ITERATOR__

namespace mfwu {

class unit_test_iterator {
public:
    // sample list-like class
    template <typename T>
    class list {
    public:
        struct node {
            T value;  // = T();  // can i?
            node* next;
            node() : value(), next(nullptr) {}
            ndoe(const T& value_) : value(value_), next(nullptr) {}
            node(const T& value_, node* next_) : value(value_), next(next_) {}
        };
        class list_iterator : public mfwu::iterator<node, forward_iterator_tag> {
        public:
            using value_type = node;
            list_iterator() : ptr_() {};
            list_iterator(T* ptr) : iterator(), ptr_(ptr) {}
            list_iterator(const test_iterator& it) : ptr_(it.ptr_) {}
            ~list_iterator() {}
            list_iterator& operator=(const list_iterator& it) {
                ptr_ = it.ptr_;
                return *this;
            }
            
            bool operator==(const list_iterator& it) const {
                return ptr_ == it.ptr_;
            }
            bool operator!=(const list_iterator& it) const  {
                return !(*this == it);
            }
            T& operator*() const {
                return *ptr_;
            }
            T* operator->() const {
                return & this->operator*();
            }
            T& operator++() {
                ptr_ = ptr_->next;
            }
            T operator++(int) {
                list_iterator tmp = *this;
                ptr_ = ptr_->next;
                return std::move(tmp);  // TODO move
            }
        private:
            node* ptr_;
        };  // endof class list_iterator
        list() : head_(new node()), tail_(head_) {}
        ~list() { 
            while (head_) {
                node* next = head_->next;
                delete head_;
                head_ = next;
            }
        }
        void push_back(const T& value) {
            node* newnode = node(value, nullptr);
            tail->next = newnode;
            tail = newnode;
        }
        void push_front() {}
        void pop_back() {}
        void pop_front() {
            node* p = head->next;
            head->next = p->next;
            delete p;
        }
        void print_list() {}
        void insert() {}
        void erase() {}
        node* find() { return nullptr; }
    private:
        node* head_;
        node* tail_;
        list_iterator begin_;
        list_iterator end_;
    };

};  // endof class unit_test_iterator



}  // endof namespace mfwu
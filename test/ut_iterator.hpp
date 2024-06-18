#include "iterator.hpp"

#define __UNIT_TEST_ITERATOR__

namespace mfwu {

class unit_test_iterator {
public:
    // list-like self-defined class
    template <typename T>
    class list {
    public:
        struct node {
            T value;  // = T();  // can i?
            node* next;
            node() : value(), next(nullptr) {}
            node(const T& value_) : value(value_), next(nullptr) {}
            node(const T& value_, node* next_) : value(value_), next(next_) {}
        };  // endof struct node

        class list_iterator : public mfwu::iterator<node, forward_iterator_tag> {
        public:
            list_iterator() : ptr_() {};
            list_iterator(node* ptr) : ptr_(ptr) {}
            list_iterator(const list_iterator& it) : ptr_(it.ptr_) {}
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
                return ptr_->value;
            }
            T* operator->() const {
                return & this->operator*();
            }
            list_iterator& operator++() {
                ptr_ = ptr_->next;
            }
            list_iterator operator++(int) {
                list_iterator tmp = *this;
                ptr_ = ptr_->next;
                return std::move(tmp);  // TODO move
            }
        private:
            node* ptr_;
        };  // endof class list_iterator
        list() : head_(new node()), tail_(new node()) { head_->next = tail_; }
        ~list() { 
            while (head_) {
                node* next = head_->next;
                delete head_;
                head_ = next;
            }
        }
        void push_back(const T& value) {
            node* newnode = new node(value, nullptr);
            node* p = head_;
            for ( ; p->next != tail_; p = p->next) {}
            p->next = newnode;
            newnode->next = tail_;
        }
        void push_front(const T& value) {
            node* newnode = new node(value, nullptr);
            newnode->next = head_->next->next;
            head_->next = newnode;
        }
        void pop_back() {
            node* p = head_;
            if (!p->next->next) return ;  // no content
            for ( ; p->next->next != tail_; p = p->next) {}
            delete p->next;
            p->next = tail_;
        }
        void pop_front() {
            node* p = head_->next;
            if (p == tail_) return ;
            head_->next = p->next;
            delete p;
        }
        void print_list() {
            std::cout << "list: ";
            for (node* p = head_->next; p != tail_; p = p->next) {
                std::cout << *p << " -> ";
            }
            std::cout << "nullptr\n";
        }
        void insert(const int& idx, const T& value) {
            node* p = head_;
            for (int i=0; i<idx; i++) {
                p = p->next;
                if (!p) return ;
            }
            node* newnode = new node(value, p->next->next);
            p->next = newnode;
        }
        void insert(const list_iterator& it, const T& value) {
            node* p = head_;
            for ( ; p->next != it; p = p->next) {
                if (p->next == nullptr) return ;
            }
            node* newnode = new node(value, p->next);
            p->next = newnode;
        }
        void erase(const int& idx) {
            node* p = head_;
            for (int i=0; i<idx; i++) {
                p = p->next;
                if (!p) return ;
            }
            node* tmp = p->next;
            p->next = tmp->next;
            delete tmp;
        }
        void erase(const list_iterator& it) {
            node* p = head_;
            for ( ; p->next != it; p = p->next) {
                if (p->next == nullptr) return ;
            }
            node* tmp = p->next;
            p->next = tmp->next;
            delete tmp;
        }
        list_iterator find(const T& value) { 
            node* p = head_;
            for ( ; p != tail_; p = p->next) {
                if (p->value == value) {
                    break;
                }
            }
            return iterator(p);
        }
        list_iterator begin() { return list_iterator(head_); }
        list_iterator end() { return list_iterator(tail_); }
    private:
        node* head_;
        node* tail_;
    };  // endof class list
    
    struct data {
        int value;
    };

    bool use_list_iterator();
    bool use_input_iterator();
    bool use_output_iterator();
    bool use_forward_iterator();
    bool use_bidirectional_iterator();
    bool use_random_access_iterator();
private:
    void set_mem_zero();
    list<data> list_;
    list<data>::list_iterator it_;
    char linear_mem[8];
};  // endof class unit_test_iterator

// operator<< must outside the class
template <typename T>
std::ostream& operator<<(std::ostream& os, const 
    typename unit_test_iterator::list<T>::node& nd) {    
    os << nd.value;
    return os;
}
std::ostream& operator<<(std::iostream& os, const
    typename unit_test_iterator::data& data) {
    os << data.value;
    return os;
}

bool unit_test_iterator::use_list_iterator() {
    for (int i=0; i<10; i++) {
        list_.push_back(data{i});
    }
    list_.print_list();
    list_.push_front(data{-1});
    list_.pop_front();
    list_.pop_back();
    for (list<data>::list_iterator it = list_.begin();
         it != list_.end(); it++) {
        (*it).value++;
        std::cout << --it->value << " -> ";  // change nothing
    }
    std::cout << "nullptr\n";


    return 0;
}


}  // endof namespace mfwu
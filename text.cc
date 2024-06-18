#include <iostream>

class unit_test {
public:
    template <typename T>
    class list {
    public:
        template <typename U>
        friend void print_list(const list<U>&);

        struct node {
            T value;
            node* next;
        };

        // some ignored list operations

    private:
        node* head_ = nullptr;
        node* tail_ = nullptr;
    };

    struct data {
        int value;
    };

    void do_test();

private:
    list<data> list_;
};

// Operator << for data
std::ostream& operator<<(std::ostream& os, const unit_test::data& dt) {
    os << dt.value;
    return os;
}

// Operator << for node
template <typename T>
std::ostream& operator<<(std::ostream& os, const typename unit_test::list<T>::node& nd) {    
    os << nd.value;
    return os;
}

// Print list function
template <typename T>
void print_list(const typename unit_test::list<T>& lst) {
    auto p = lst.head_;
    while (p) {
        std::cout << *p << " ";
        p = p->next;
    }
    std::cout << std::endl;
}

// do_test function
void unit_test::do_test() {
    print_list(list_);
}

int main() {
    unit_test ut;
    ut.do_test();
    return 0;
}

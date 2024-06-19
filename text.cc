// #include <iostream>

// class unit_test {
// public:
//     template <typename T>
//     class list {
//     public:
//         template <typename U>
//         friend void print_list(const list<U>&);

//         struct node {
//             T value;
//             node* next;
//         };

//         // some ignored list operations

//     private:
//         node* head_ = nullptr;
//         node* tail_ = nullptr;
//     };

//     struct data {
//         int value;
//     };

//     void do_test();

// private:
//     list<data> list_;
// };

// // Operator << for data
// std::ostream& operator<<(std::ostream& os, const unit_test::data& dt) {
//     os << dt.value;
//     return os;
// }

// // Operator << for node
// template <typename T>
// std::ostream& operator<<(std::ostream& os, const typename unit_test::list<T>::node& nd) {    
//     os << nd.value;
//     return os;
// }

// // Print list function
// template <typename T>
// void print_list(const typename unit_test::list<T>& lst) {
//     auto p = lst.head_;
//     while (p) {
//         std::cout << *p << " ";
//         p = p->next;
//     }
//     std::cout << std::endl;
// }

// // do_test function
// void unit_test::do_test() {
//     print_list(list_);
// }

// int main() {
//     unit_test ut;
//     ut.do_test();
//     return 0;
// }


#include <iostream>
#include <utility> // for std::forward

struct data {
    int a;
    char b, c, d, e;

    // data(int a, char b, char c, char d, char e) : a(a), b(b), c(c), d(d), e(e) {}
};

// Template function to forward arguments and construct the object in place
template <typename T1, typename... Args>
inline void construct(T1* p, Args&&... args) {
    new (p) T1(std::forward<Args>(args)...);
}

int main() {
    // Allocate memory for a data object
    void* p = operator new(sizeof(data));

    // Construct the data object in the allocated memory
    construct(static_cast<data*>(p), 1, 'a', 'b', 'c', 'd');

    // Access the constructed object
    data* dp = static_cast<data*>(p);
    std::cout << "data: " << dp->a << ", " << dp->b << ", " << dp->c << ", " << dp->d << ", " << dp->e << std::endl;

    // Destruct the object
    dp->~data();

    // Free the allocated memory
    operator delete(p);

    return 0;
}

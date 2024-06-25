#include <iostream>
#include <vector>

struct A {
    int a;
    A(int a_) : a(a_) { std::cout << "construct: " << a << "\n"; }
    A(const A& other) : a(other.a) { std::cout << "copy: " << a << "\n"; } 
    ~A() { std::cout << "destroy: " << a << "\n"; }
    A& operator=(const A& other) {
        a = other.a;
        std::cout << "=copy: " << a << "\n";
        return *this;
    }
};

void print_vec(const std::vector<A>& vec) {
    for (const A& a:vec) {
        std::cout << a.a << " ";
    }
    std::cout << "\n";
}

int main() {
    A a1(4), a2(2), a3(3), a4(4);
    std::vector<A> vvec = {1, 2, 5, 3, 4};
    std::cout << "-------------------\n";
    std::vector<A> vec = {};
    print_vec(vec);
    vec.reserve(10);
    print_vec(vec);
    std::cout << "------------------\n";
    vec.insert(vec.begin(), a1);
    print_vec(vec);
    vec.insert(vec.begin(), a2);
    print_vec(vec);
    vec.insert(vec.begin(), a3);
    print_vec(vec);
    vec.insert(vec.begin(), a4);
    print_vec(vec);
    std::cout << "------------------\n";
    vec.insert(vec.begin(), vvec.begin(), vvec.end());
    print_vec(vec);
    return 0;
}
    

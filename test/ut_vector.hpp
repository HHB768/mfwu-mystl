#define __UNIT_TEST_VECTOR__

#include "vector.hpp"
#include <iostream>

namespace mfwu {

class unit_test_vector {
public:
    bool use_vector_iterator();
    bool use_mfwu_vector();
    bool use_vector_algo();
private:
    struct pod_data {
        int a;
        char b, c, d, e;
    };
    struct npod_data {
        int a;
        npod_data() : a(0) {}
        npod_data(int a_) : a(a_) {}
        ~npod_data() { a = 0x2E; }
        npod_data& operator++() {
            ++a;
            return *this;
        }
        npod_data operator--(int) {
            --a;
            return *this;
        }
    };
    friend std::ostream& operator<<(std::ostream&, const pod_data&);
    friend std::ostream& operator<<(std::ostream&, const npod_data&);
};  // endof class unit_test_vector

std::ostream& operator<<(std::ostream& os, 
                         const unit_test_vector::pod_data& data) {
    os << "{" << data.a << ", " 
              << (int)data.b << ", "
              << (int)data.c << ", "
              << (int)data.d << ", "
              << (int)data.e << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, 
                         const unit_test_vector::npod_data& data) {
    os << data.a;
    return os;
}

bool unit_test_vector::use_vector_iterator() {
    std::cout << "\n------- Test: use vector_iterator -------\n";
    std::cout << "moving and derefering iterator\n";
    mfwu::vector<npod_data> vec = {1, 2, 3, 4, 5, 6};
    for (mfwu::vector<npod_data>::iterator it = vec.begin(); 
         it < vec.end(); it+=2, --it, it++, --it, ++it, it = &*it) {
        std::cout << mfwu::distance(vec.begin(), it) 
                  << " : " << (++(*it))-- << "  ";
    }
    std::cout << "\n";
    return 0;
}

template <typename T>
void print_basic_info(const mfwu::vector<T>& vec) {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
    std::cout << "&vec.begin() = " << &*vec.begin() 
              << ", vec.size() = " << vec.size() 
              << ", vec.capacity() = " << vec.capacity() << "\n";
#endif  // __UNIT_TEST_VECTOR_BRIEF__
}

template <typename T>
void print_detailed_info(const mfwu::vector<T>& vec) {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
    print_basic_info(vec);
    for (auto&& val:vec) {
        std::cout << val << " ";
    }
    std::cout << "\n";
#endif  // __UNIT_TEST_VECTOR_BRIEF__
}

template <typename T>
void try_print_range(const mfwu::vector<T>& vec, int start, int end) {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
    print_basic_info(vec);
    for (int i = start; i <= end; i++) {
        try {
            std::cout << vec[i] << " ";
        } catch(std::out_of_range) {
            std::cout << "__idx_out_of_range__ ";
        }
    }
    std::cout << "\n";
#endif // __UNIT_TEST_VECTOR_BRIEF__
}

bool unit_test_vector::use_mfwu_vector() {
    std::cout << "\n------- Test: use mfwu::vector -------\n";
    std::cout << "constructing pod data\n";
    mfwu::vector<pod_data> vec1;
    print_basic_info(vec1);
    mfwu::vector<pod_data> vec2(1);
    print_basic_info(vec2);
    mfwu::vector<pod_data> vec3(1, pod_data{});
    print_basic_info(vec3);
    mfwu::vector<pod_data> vec4(mfwu::move(vec3));
    print_basic_info(vec4);
    mfwu::vector<pod_data> vec5 = {{1, 'a', 'b', 'c', 'd'}, {2, 'e', 'f', 'g', 'h'}};
    print_basic_info(vec5);
    mfwu::vector<pod_data> vec6 = vec2;
    print_basic_info(vec6);

    std::cout << "constructing non-pod data\n";
    mfwu::vector<npod_data> nvec1;
    print_basic_info(nvec1);
    mfwu::vector<npod_data> nvec2(1);
    print_basic_info(nvec2);
    mfwu::vector<npod_data> nvec3(1, npod_data());
    print_basic_info(nvec3);
    mfwu::vector<npod_data> nvec4(mfwu::move(nvec4));
    print_basic_info(nvec4);
    mfwu::vector<npod_data> nvec5 = {1, 2};
    print_basic_info(nvec5);
    mfwu::vector<npod_data> nvec6 = nvec2;
    print_basic_info(nvec6);

    std::cout << "reserving\n";
    vec5.reserve(1);
    vec5.reserve(5);
    print_detailed_info(vec5);
    vec5.reserve(1);
    print_detailed_info(vec5);

    nvec5.reserve(1);
    nvec5.reserve(5);
    print_detailed_info(nvec5);
    nvec5.reserve(1);
    print_detailed_info(nvec5);

    std::cout << "resizing\n";
    vec5.resize(1);
    vec5.resize(5);
    print_detailed_info(vec5);
    vec5.resize(1);
    print_detailed_info(vec5);

    nvec5.resize(1);
    nvec5.resize(5);
    print_detailed_info(nvec5);
    nvec5.resize(1);
    print_detailed_info(nvec5);

    std::cout << "clearing\n";
    vec5.clear();
    print_basic_info(vec5);
    nvec5.clear();
    print_basic_info(nvec5);

    std::cout << "emplacing/pushing back\n";
    vec6.emplace_back(2, 'a', 'c', 'e', 's');
    print_detailed_info(vec6);
    vec6.emplace_back(pod_data{});
    print_detailed_info(vec6);
    vec6.push_back({3, 'h', 'a', 'c', 'e'});
    print_detailed_info(vec6);

    npod_data npdata = 4;
    nvec6.emplace_back(2);
    print_detailed_info(nvec6);
    nvec6.emplace_back(npdata);
    print_detailed_info(nvec6);
    nvec6.push_back({3});
    print_detailed_info(nvec6);

    std::cout << "poping back\n";
    vec6.pop_back();
    print_detailed_info(vec6);
    vec6.pop_back();
    print_detailed_info(vec6);

    nvec6.pop_back();
    print_detailed_info(nvec6);
    nvec6.pop_back();
    print_detailed_info(nvec6);
    nvec6.pop_back();
    print_detailed_info(nvec6);
    nvec6.pop_back();
    print_detailed_info(nvec6);
    nvec6.pop_back();
    print_detailed_info(nvec6);

    std::cout << "inserting\n";
    vec5.insert(0, pod_data{1, 'q', 'w', 'e', 'r'});
    print_detailed_info(vec5);
    vec5.insert(1, {2, 't', 'y', 't', 'y'}, 5);
    print_detailed_info(vec5);
    vec5.insert(1, {}, 0);
    print_detailed_info(vec5);
    auto it1 = vec5.begin();
    it1 += 2;
    vec5.insert(it1, {}, 0);
    print_detailed_info(vec5);
    vec5.insert(it1, vec6.begin(), vec6.end());
    print_detailed_info(vec5);
    
    nvec5.insert(0, npod_data{1});
    print_detailed_info(nvec5);
    nvec5.insert(1, {2}, 5);
    print_detailed_info(nvec5);
    nvec5.insert(1, {}, 0);
    print_detailed_info(nvec5);
    auto it2 = nvec5.begin();
    it2 += 2;
    nvec5.insert(it2, {}, 0);
    print_detailed_info(nvec5);
    mfwu::vector<npod_data> tmp = {3, 4, 4, 7};
    nvec5.insert(it2, tmp.begin(), tmp.end());
    print_detailed_info(nvec5);
    
    std::cout << "erasing\n";
    vec5.erase(5);
    print_detailed_info(vec5);
    vec5.erase(it1);
    print_detailed_info(vec5);
    vec5.erase(vec5.begin(), it1);
    print_detailed_info(vec5);
    
    nvec5.erase(5);
    print_detailed_info(nvec5);
    nvec5.erase(it2);
    print_detailed_info(nvec5);
    nvec5.erase(nvec5.begin(), it2);
    print_detailed_info(nvec5);

    std::cout << "shrinking\n";
    // TODO: we need to test shrink based on default_malloc later
    vec5.shrink(8);
    print_basic_info(vec5);
    vec5.shrink(4);
    print_basic_info(vec5);
    nvec5.shrink(8);
    print_basic_info(nvec5);
    nvec5.shrink(4);
    print_basic_info(nvec5);
    
    std::cout << "testing operators\n";
    try_print_range(vec5, -1, 5);
    try_print_range(nvec5, -1, 5);



    // TODO: check memory leakage
    return 0;
}

bool unit_test_vector::use_vector_algo() {
    return 0;
}


}  // endof namespace mfwu

#define __UNIT_TEST_LIST__

#include "list.hpp"

namespace mfwu {

class unit_test_list {
public:
    bool use_forward_list_iterator();
    bool use_mfwu_forward_list();
    bool use_list_iterator();
    bool use_mfwu_list();
private:
    class data {
    public:
        data(int a) : a_(a) {}
        data() : a_(0) {}
        data(const data& d) : a_(d.a_) {}
        data& operator=(const data& d) {
            a_ = d.a_;
            return *this;
        }
        ~data() {}
        operator int() {
            return a_;
        }
    private:
        int a_;
    };  // endof class data
};  // endof class unit_test_list

bool unit_test_list::use_forward_list_iterator() {
    std::cout << "\n------- Test: use forward_list_iterator -------\n";
    std::cout << "moving and derefering iterator\n";
    mfwu::forward_list<data>::iterator it;
    mfwu::forward_list<data> lst = {1, 2, 3, 4, 5, 6};
    for (it = lst.begin(); it != lst.end(); it++) {
#ifdef __UNIT_TEST_LIST__
#ifndef __UNIT_TEST_LIST_BRIEF__
        std::cout << mfwu::distance(lst.begin(), it) << ":" << *it << " ";
#else  // __UNIT_TEST_LIST_BRIEF__
        mfwu::distance(lst.begin(), it);
        *it = 1;
#endif  // __UNIT_TEST_LIST_BRIEF__
#endif  // __UNIT_TEST_LIST__
    }
    std::cout << "\n";
    return 0;
}

template <typename T>
void print_basic_info(const mfwu::forward_list<T>& lst) {
#ifdef __UNIT_TEST_LIST__
#ifndef __UNIT_TEST_LIST_BRIEF__
    std::cout << "front = " << lst.front() << ", ";
    std::cout << "back = " << lst.back() << ", ";
    std::cout << "size = " << lst.size() << "\n";
#endif  // __UNIT_TEST_LIST_BRIEF__
#endif  // __UNIT_TEST_LIST__
}

template <typename T>
void print_detailed_info(const mfwu::forward_list<T>& lst) {
#ifdef __UNIT_TEST_LIST__
#ifndef __UNIT_TEST_LIST_BRIEF__
    std::cout << "z=";
    for (auto it = lst.begin(); it != lst.end(); it++) {
        std::cout <<" -> " << *it;
    }
    std::cout << " -> =6\n";
#endif  // __UNIT_TEST_LIST_BRIEF__
#endif  // __UNIT_TEST_LIST__
}

bool unit_test_list::use_mfwu_forward_list() {
    std::cout << "\n------- Test: use mfwu_forward_list -------\n";
    std::cout << "constructing & copying & moving\n";
    mfwu::forward_list<data> lst1;
    print_detailed_info(lst1);
    mfwu::forward_list<data> lst2(2);
    print_detailed_info(lst2);
    mfwu::forward_list<data> lst3(3, 3);
    print_detailed_info(lst3);
    mfwu::forward_list<data> lst4(lst3.begin(), lst3.end());
    print_detailed_info(lst4);
    mfwu::forward_list<data> lst5 = {1, 2, 3, 4, 5};
    print_detailed_info(lst5);
    mfwu::forward_list<data> lst6 = lst5;
    print_detailed_info(lst6);
    mfwu::forward_list<data> lst7 = mfwu::move(lst3);
    print_detailed_info(lst7);
    lst4 = lst5;
    print_detailed_info(lst4);
    lst3 = mfwu::move(lst5);
    print_detailed_info(lst3);

    std::cout << "\ngeting basic/detailed infos\n";
    print_basic_info(lst3);
    print_detailed_info(lst3);

    std::cout << "\nresizing\n";
    lst3.resize(7);
    print_detailed_info(lst3);
    lst3.resize(3);
    print_detailed_info(lst3);

    std::cout << "\npushing & poping\n";
    print_detailed_info(lst4);
    lst4.push_front(6);
    print_detailed_info(lst4);
    lst4.pop_back();
    print_detailed_info(lst4);
    lst4.push_back(6);
    print_detailed_info(lst4);
    lst4.pop_front();
    print_detailed_info(lst4);

    std::cout << "\ninserting\n";
    lst4.insert(lst4.end(), data{1});
    print_detailed_info(lst4);
    lst4.insert(0, data{1});
    print_detailed_info(lst4);
    lst4.insert(lst4.begin(), data{42}, 2);
    print_detailed_info(lst4);
    lst4.insert(++lst4.begin(), lst3.begin(), ++++lst3.begin());
    print_detailed_info(lst4);

    std::cout << "\nerasing\n";
    lst4.erase(lst4.begin());
    print_detailed_info(lst4);
    lst4.erase(++lst4.begin(), ++++++lst4.begin());
    print_detailed_info(lst4);

    return 0;
}

bool unit_test_list::use_list_iterator() {
    std::cout << "\n------- Test: use list_iterator -------\n";
    std::cout << "moving and derefering iterator\n";
    mfwu::list<data>::iterator it;
    mfwu::list<data> lst = {1, 2, 3, 4, 5, 6};
    for (it = lst.begin(); it != lst.end(); it++) {
#ifdef __UNIT_TEST_LIST__
#ifndef __UNIT_TEST_LIST_BRIEF__
        std::cout << mfwu::distance(lst.begin(), --++it) 
                  << ":" << *(it++--) << " ";
                  // actually "--" is invalid bcz
                  // it++ is a rvalue
                  // warning! dont use ++/-- in << stream
                  // the computing order may be right2left
#else  // __UNIT_TEST_LIST_BRIEF__
        mfwu::distance(lst.begin(), --++it--++);
        *it = 1;
#endif  // __UNIT_TEST_LIST_BRIEF__
#endif  // __UNIT_TEST_LIST__
    }
    std::cout << "\n";
    return 0;
}

template <typename T>
void print_basic_info(const mfwu::list<T>& lst) {
#ifdef __UNIT_TEST_LIST__
#ifndef __UNIT_TEST_LIST_BRIEF__
    std::cout << "front = " << lst.front() << ", ";
    std::cout << "back = " << lst.back() << ", ";
    std::cout << "size = " << lst.size() << "\n";
#endif  // __UNIT_TEST_LIST_BRIEF__
#endif  // __UNIT_TEST_LIST__
}

template <typename T>
void print_detailed_info(const mfwu::list<T>& lst) {
#ifdef __UNIT_TEST_LIST__
#ifndef __UNIT_TEST_LIST_BRIEF__
    std::cout << "z=";
    for (auto it = lst.begin(); it != lst.end(); it++) {
        std::cout <<" <-> " << *it;
    }
    std::cout << " <-> =6\n";
#endif  // __UNIT_TEST_LIST_BRIEF__
#endif  // __UNIT_TEST_LIST__
}

bool unit_test_list::use_mfwu_list() {
    std::cout << "\n------- Test: use mfwu_list -------\n";
    std::cout << "constructing & copying & moving\n";
    mfwu::list<data> lst1;
    print_detailed_info(lst1);
    mfwu::list<data> lst2(2);
    print_detailed_info(lst2);
    mfwu::list<data> lst3(3, 3);
    print_detailed_info(lst3);
    mfwu::list<data> lst4(lst3.begin(), lst3.end());
    print_detailed_info(lst4);
    mfwu::list<data> lst5 = {1, 2, 3, 4, 5};
    print_detailed_info(lst5);
    mfwu::list<data> lst6 = lst5;
    print_detailed_info(lst6);
    mfwu::list<data> lst7 = mfwu::move(lst3);
    print_detailed_info(lst7);
    lst4 = lst5;
    print_detailed_info(lst4);
    lst3 = mfwu::move(lst5);
    print_detailed_info(lst3);

    std::cout << "\ngeting basic/detailed infos\n";
    print_basic_info(lst3);
    print_detailed_info(lst3);

    std::cout << "\nresizing\n";
    lst3.resize(7);
    print_detailed_info(lst3);
    lst3.resize(3);
    print_detailed_info(lst3);

    std::cout << "\npushing & poping\n";
    print_detailed_info(lst4);
    lst4.push_front(6);
    print_detailed_info(lst4);
    lst4.pop_back();
    print_detailed_info(lst4);
    lst4.push_back(6);
    print_detailed_info(lst4);
    lst4.pop_front();
    print_detailed_info(lst4);

    std::cout << "\ninserting\n";
    lst4.insert(lst4.end(), data{1});
    print_detailed_info(lst4);
    // lst4.insert(0, data{1});
    // print_detailed_info(lst4);
    lst4.insert(lst4.begin(), data{42}, 2);
    print_detailed_info(lst4);
    lst4.insert(++lst4.begin(), lst3.begin(), ++++lst3.begin());
    print_detailed_info(lst4);

    std::cout << "\nerasing\n";
    lst4.erase(lst4.begin());
    print_detailed_info(lst4);
    lst4.erase(++lst4.begin(), ++++++lst4.begin());
    print_detailed_info(lst4);

    return 0;
}

}  // endof namespace mfwu
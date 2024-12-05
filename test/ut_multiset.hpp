#define __UNIT_TEST_MULTISET__

#include "multiset.hpp"
#include "ut_rbtree_exp.hpp"

namespace mfwu {

class unit_test_multiset {
public:
    bool use_mfwu_multiset();
    bool use_modifier_and_lookup();
    bool use_other_interface();
private:
    template <typename T, typename Compare>
    void print_multiset(const multiset<T, Compare>& s) {
#ifdef __UNIT_TEST_MULTISET__
#ifndef __UNIT_TEST_MULTISET_BRIEF__
        unit_test_rbtree_exp::print_and_check(s.rbt_);
#endif  // __UNIT_TEST_MULTISET_BRIEF__
#endif  // __UNIT_TEST_MULTISET__
    }

};  // endof class unit_test_set

bool unit_test_multiset::use_mfwu_multiset() {
    std::cout << "\n------- Test: use mfwu::multiset -------\n";
    std::cout << "constructing & copying & moving & destroying\n";
    mfwu::multiset<int> s1;
    print_multiset(s1);
    mfwu::multiset<int> s2 = {0, 0, 5, 6, 1, 8, 5, 3, 4,
                              1, 3, 5, 7, 9, 2, 4, 6, 8};
    print_multiset(s2);
    mfwu::multiset<int> s3 = s2;
    print_multiset(s3);
    mfwu::multiset<int> s4 = mfwu::move(s2);
    print_multiset(s4);
    mfwu::multiset<int> s5;
    s5 = s3;
    s5 = mfwu::move(s4);
    print_multiset(s5);
    mfwu::multiset<int> s6(++s5.begin(), s5.end());
    print_multiset(s6);
    mfwu::multiset<int, mfwu::greater<int>> 
    s7(++s6.begin(), ++++++++++++s6.begin());
    // TODO: test greater set with all interfaces 11.16/24
    // if you have problems within inserting, try to complete 
    // the ut_rebtree::use_cmp_functor()
    // other parts may have problems too...
    print_multiset(s7);

    return 0;
}

bool unit_test_multiset::use_modifier_and_lookup() {
    std::cout << "\n------- Test: use modifiers and lookups -------\n";
    std::cout << "inserting & erasing\n";
    mfwu::multiset<int> s1 = {1, 9, 8, 3};
    print_multiset(s1);
    s1.insert(4);
    print_multiset(s1);
    s1.insert(4);
    print_multiset(s1);
    s1.insert(7);
    print_multiset(s1);
    s1.erase(3);
    print_multiset(s1);
    s1.erase(6);
    print_multiset(s1);
    s1.emplace(6);
    print_multiset(s1);
    s1.erase(++s1.begin());
    print_multiset(s1);

    std::cout << "clearing & ranged_inserting & ranged_erasing "
              << "& swapping & merging\n";
    mfwu::multiset<int> s2 = s1;
    s2.clear();
    print_multiset(s2);
    s2.insert(s1.begin(), ++s1.begin());
    print_multiset(s2);
    s1.swap(s2);
    print_multiset(s2);
    s2.insert({2});
    s1.insert(6);
    print_multiset(s1);
    print_multiset(s2);
    s1.merge(s2);
    print_multiset(s1);
    print_multiset(s2);
    s1.erase(s1.begin(), s1.end());
    print_multiset(s1);

    std::cout << "finding & counting & containing & calcing equal_range\n";
#ifdef __UNIT_TEST_MULTISET__
#ifndef __UNIT_TEST_MULTISET_BRIEF__    
    std::cout << mfwu::distance(s1.find(2), s1.end()) << "\n";
    std::cout << s1.count(2) << "\n";
    std::cout << mfwu::distance(s2.find(1), s2.end()) << "\n";
    std::cout << s2.contains(1) << "\n";
#else  // __UNIT_TEST_MULTISET_BRIEF__
    mfwu::distance(s1.find(2), s1.end());
    s1.count(2);
#endif  // __UNIT_TEST_MULTISET_BRIEF__
#endif  // __UNIT_TEST_MULTISET__
    auto [it1, it2] = s2.equal_range(1);
    auto [it3, it4] = s2.equal_range(2);
#ifdef __UNIT_TEST_MULTISET__
#ifndef __UNIT_TEST_MULTISET_BRIEF__   
    std::cout << mfwu::distance(s2.begin(), it1) << ", "
              << mfwu::distance(s2.begin(), it2) << ", "
              << mfwu::distance(s2.begin(), it3) << ", "
              << mfwu::distance(s2.begin(), it4) << "\n";
#endif  // __UNIT_TEST_MULTISET_BRIEF__
#endif  // __UNIT_TEST_MULTISET__

    return 0;
}

bool unit_test_multiset::use_other_interface() {
    std::cout << "\n------- Test: use other interfaces -------\n";
    mfwu::multiset<char> s1 = {1, 2, 6, 9, 8, 7, 3, 4};
#ifdef __UNIT_TEST_MULTISET__
#ifndef __UNIT_TEST_MULTISET_BRIEF__   
    std::cout << "empty(): " << s1.empty() << "\n";
    std::cout << "size(): " << s1.size() << "\n";
    std::cout << "height(): " << s1.height() << "\n";
    std::cout << "root(): " << (int)s1.root() << "\n";
    std::cout << "max_size(): " << s1.max_size() << "\n";
#endif  // __UNIT_TEST_MULTISET_BRIEF__
#endif  // __UNIT_TEST_MULTISET__

    return 0;
}

}  // endof namespace mfwu

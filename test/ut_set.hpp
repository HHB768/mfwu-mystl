#define __UNIT_TEST_SET__

#include "set.hpp"
#include "ut_rbtree.hpp"

namespace mfwu {

class unit_test_set {
public:
    bool use_mfwu_set();
    bool use_modifier_and_lookup();
    bool use_other_interface();
private:
    template <typename T, typename Compare>
    void print_set(const set<T, Compare>& s) {
        ut_rbtree.print_detailed_info(s.rbt_);
    }

    unit_test_rbtree ut_rbtree;

};  // endof class unit_test_set

bool unit_test_set::use_mfwu_set() {
    std::cout << "\n------- Test: use mfwu::set -------\n";
    std::cout << "constructing & copying & moving & destroying\n";
    mfwu::set<int> s1;
    print_set(s1);
    mfwu::set<int> s2 = {0, 0, 5, 6, 1, 8, 5, 3, 4,
                         1, 3, 5, 7, 9, 2, 4, 6, 8};
    print_set(s2);
    mfwu::set<int> s3 = s2;
    print_set(s3);
    mfwu::set<int> s4 = mfwu::move(s2);
    print_set(s4);
    mfwu::set<int> s5;
    s5 = s3;
    s5 = mfwu::move(s4);
    print_set(s5);
    mfwu::set<int> s6(++s5.begin(), s5.end());
    print_set(s6);
    mfwu::set<int, mfwu::greater<int>> 
    s7(++s6.begin(), ++++++++++++s6.begin());
    // TODO: test greater set with all interfaces 11.16/24
    // if you have problems within inserting, try to complete 
    // the ut_rebtree::use_cmp_functor()
    // other parts may have problems too...
    print_set(s7);

    return 0;
}

bool unit_test_set::use_modifier_and_lookup() {
    std::cout << "\n------- Test: use modifiers and lookups -------\n";
    std::cout << "inserting & erasing\n";
    mfwu::set<int> s1 = {1, 9, 8, 3};
    print_set(s1);
    s1.insert(4);
    print_set(s1);
    s1.insert(4);
    print_set(s1);
    s1.insert(7);
    print_set(s1);
    s1.erase(3);
    print_set(s1);
    s1.erase(6);
    print_set(s1);
    s1.emplace(6);
    print_set(s1);
    s1.erase(++s1.begin());
    print_set(s1);

    std::cout << "clearing & ranged_inserting & ranged_erasing "
              << "& swapping & merging\n";
    mfwu::set<int> s2 = s1;
    s2.clear();
    print_set(s2);
    s2.insert(s1.begin(), ++s1.begin());
    print_set(s2);
    s1.swap(s2);
    print_set(s2);
    s2.insert({2});
    s1.insert(6);
    print_set(s1);
    print_set(s2);
    s1.merge(s2);
    print_set(s1);
    print_set(s2);
    s1.erase(s1.begin(), s1.end());
    print_set(s1);

    std::cout << "finding & counting & containing & calcing equal_range\n";
    std::cout << mfwu::distance(s1.find(2), s1.end()) << "\n";
    std::cout << s1.count(2) << "\n";
    std::cout << mfwu::distance(s2.find(1), s2.end()) << "\n";
    std::cout << s2.contains(1) << "\n";

    auto [it1, it2] = s2.equal_range(1);
    auto [it3, it4] = s2.equal_range(2);
    std::cout << mfwu::distance(s2.begin(), it1) << ", "
              << mfwu::distance(s2.begin(), it2) << ", "
              << mfwu::distance(s2.begin(), it3) << ", "
              << mfwu::distance(s2.begin(), it4) << "\n";

    return 0;
}

bool unit_test_set::use_other_interface() {
    std::cout << "\n------- Test: use other interfaces -------\n";
    mfwu::set<char> s1 = {1, 2, 6, 9, 8, 7, 3, 4};
    std::cout << "empty(): " << s1.empty() << "\n";
    std::cout << "size(): " << s1.size() << "\n";
    std::cout << "height(): " << s1.height() << "\n";
    std::cout << "root(): " << (int)s1.root() << "\n";
    std::cout << "max_size(): " << s1.max_size() << "\n";

    return 0;
}

}  // endof namespace mfwu

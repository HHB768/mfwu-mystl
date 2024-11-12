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
    print_set(s7);

    return 0;
}

bool unit_test_set::use_modifier_and_lookup() {

    return 0;
}

bool unit_test_set::use_other_interface() {

    return 0;
}

}  // endof namespace mfwu

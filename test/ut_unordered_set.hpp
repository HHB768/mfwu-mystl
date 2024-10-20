#define __UNIT_TEST_UNORDERED_SET__

#include "unordered_set.hpp"
#include "ut_hashtable.hpp"

namespace mfwu {

class unit_test_unordered_set {
public:
    using data = mfwu::unit_test_hashtable::data;
    using data_hash = mfwu::unit_test_hashtable::data_hash;
    using int_hash = mfwu::unit_test_hashtable::hash_func;
    using test_set = mfwu::unordered_set<int>;

    bool use_mfwu_unordered_set();
    bool use_modifier_and_lookup();
    bool use_other_interface();
private:
    unit_test_hashtable ut_hashtable = {};
    void print_unordered_set(const test_set& set) {
        ut_hashtable.print_shashtable(set.htbl_);
    }
};  // endof class unit_test_unordered_set

bool unit_test_unordered_set::use_mfwu_unordered_set() {
    std::cout << "\n------- Test: use mfwu::unordered_set -------\n";
    std::cout << "constructing & copying & moving\n";
    
    return 0;
}

bool unit_test_unordered_set::use_modifier_and_lookup() {
    return 0;
}

bool unit_test_unordered_set::use_other_interface() {
    return 0;
}

}  // endof namespace mfwu
#define __UNIT_TEST_UNORDERED_MAP__

#include "unordered_map.hpp"
#include "ut_hashtable.hpp"

namespace mfwu {

class unit_test_unordered_map {
public:
    using data = mfwu::unit_test_hashtable::data;
    using data_hash = mfwu::unit_test_hashtable::data_hash;
    using int_hash = mfwu::unit_test_hashtable::hash_func;
    using test_map = mfwu::unordered_map<data, int, data_hash>;

    bool use_mfwu_unordered_map();
    bool use_modifier_and_lookup();
    bool use_other_interface();
private:
    unit_test_hashtable ut_hashtable = {};
    void print_unordered_map(const test_map& map) {
        ut_hashtable.print_hashtable(map.htbl_);
    }
};  // endof class unit_test_unordered_map

bool unit_test_unordered_map::use_mfwu_unordered_map() {
    std::cout << "\n------- Test: use mfwu::unordered_map -------\n";
    std::cout << "constructing & copying & moving\n";
    using test_map = mfwu::unordered_map<data, int, data_hash>;
    test_map map1;
    print_unordered_map(map1);
    mfwu::unordered_map<data, bool, data_hash> map2(10);
    // print_unordered_map(map2);
    test_map map3 = {{data{1}, 2}, 
                     {data{2}, 3}, 
                     {data{3}, 4}, 
                     {data{5}, 6}};
    print_unordered_map(map3);
    test_map map4(++map3.begin(), map3.end());
    print_unordered_map(map4);
    test_map map5 = map4;
    print_unordered_map(map5);
    test_map map6 = mfwu::move(map5);
    print_unordered_map(map6);
    mfwu::unordered_map<int, int> map7 = {{1, 2}, {3, 4}};
    // print_unordered_map(map7);
    mfwu::unordered_map<int, int, int_hash> map8(map7);
    // print_unordered_map(map8);
    // map4 = map6;
    // map4 = mfwu::move(map6);
    print_unordered_map(map4);

    return 0;
}

bool unit_test_unordered_map::use_modifier_and_lookup() {
    return 0;
}

bool unit_test_unordered_map::use_other_interface() {
    return 0;
}

}  // endof namespace mfwu
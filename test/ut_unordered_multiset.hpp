#define __UNIT_TEST_UNORDERED_MULTISET__

#include "unordered_multiset.hpp"
#include "ut_hashtable_exp.hpp"

namespace mfwu {

class unit_test_unordered_multiset {
public:
    using data = mfwu::unit_test_hashtable_exp::data;
    using data_hash = mfwu::unit_test_hashtable_exp::data_hash;
    using int_hash = mfwu::unit_test_hashtable_exp::hash_func;
    using test_set = mfwu::unordered_multiset<int, int_hash>;

    bool use_mfwu_unordered_multiset();
    bool use_modifier_and_lookup();
    bool use_other_interface();
private:
    unit_test_hashtable_exp ut_hashtable = {};
    template <typename Key, typename Hash, typename Alloc>
    void print_unordered_multiset(
        const unordered_multiset<Key, Hash, Alloc>& set) {
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
        ut_hashtable.print_shashtable(set.htbl_);
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
    }
    template <typename Key, typename Hash, typename Alloc>
    void print_ret_info(
        typename unordered_multiset<Key, Hash, Alloc>::iterator it,
        bool is_new) {
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
        std::cout << "*it = " << *it << "\n";
        std::cout << "is new: " << (is_new ? "true" : "false") << "\n"; 
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
    }
    template <typename Key, typename Hash, typename Alloc>
    void print_it_info(
        typename unordered_multiset<Key, Hash, Alloc>::iterator it) {
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
        std::cout << "*it = " << *it << "\n";
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
    }
    template <typename Key, typename Hash, typename Alloc>
    void print_suc_info(bool is_new) {
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
        std::cout << "is successful: " << (is_new ? "true" : "false") << "\n"; 
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
    }
    template <typename Key, typename Hash, typename Alloc>
    void print_unordered_multiset_with_iterator(
        const unordered_multiset<Key, Hash, Alloc>& set) {
        for (auto it = set.begin(); it != set.end(); ++it) {
        const auto& key = *it;
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
        std::cout << "key: " << key << "\n";
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
        }
    }
    template <typename Key, typename Hash, typename Alloc>
    void print_unordered_multiset_other_info(
        const unordered_multiset<Key, Hash, Alloc>& set) {
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
        std::cout << "empty: " << (set.empty() ? "true" : "false") << "\n";
        std::cout << "size: " << set.size() << "\n";
        std::cout << "load_factor: " << set.load_factor() << "\n";
        std::cout << "max_size: " << set.max_size() << "\n";
        std::cout << "max_load_factor: " << set.max_load_factor() << "\n";
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
    }
};  // endof class unit_test_unordered_multiset

bool unit_test_unordered_multiset::use_mfwu_unordered_multiset() {
    std::cout << "\n------- Test: use mfwu::unordered_multiset -------\n";
    std::cout << "constructing & copying & moving & destroying\n";
    // using test_set = mfwu::unordered_multiset<data, data_hash>;
    test_set set1;
    print_unordered_multiset(set1);
    mfwu::unordered_multiset<bool> set2(10);
    // print_unordered_multiset(set2);
    test_set set3 = {1, 2, 3, 5};
    print_unordered_multiset(set3);
    test_set set4(++set3.begin(), set3.end());
    print_unordered_multiset(set4);
    test_set set5 = set4;
    print_unordered_multiset(set5);
    test_set set6 = mfwu::move(set5);
    print_unordered_multiset(set6);
    mfwu::unordered_multiset<data, data_hash> set7 = {data{1}, data{3}};
    // print_unordered_multiset(set7);
    mfwu::unordered_multiset<data, data_hash> set8(set7);
    // print_unordered_multiset(set8);
    set4 = set6;
    set4 = mfwu::move(set6);
    print_unordered_multiset(set4);
    return 0;
}

bool unit_test_unordered_multiset::use_modifier_and_lookup() {
    std::cout << "\n------- Test: use modifiers and lookups -------\n";
    test_set set1 = {2, 2, 8, 6};
    auto [it, res] = set1.insert(7);
    print_ret_info<int, int_hash, mfwu::DefaultAllocator<
        mfwu::sbucket_exp<int>, mfwu::malloc_alloc>>(it, res);
    print_unordered_multiset(set1);
    set1.insert({11, 2});
    print_unordered_multiset(set1);
    test_set set2 = {5, 22, 5};
    set1.insert(set2.begin(), set2.end());
    print_unordered_multiset(set1);
    auto [it2, res2] = set1.emplace(7);
    print_ret_info<int, int_hash, mfwu::DefaultAllocator<
        mfwu::sbucket_exp<int>, mfwu::malloc_alloc>>(it2, res2);
    print_unordered_multiset(set1);
    it = set1.erase(it2);
    if (it != set1.end()) {
        print_it_info<int, int_hash, mfwu::DefaultAllocator<
        mfwu::sbucket_exp<int>, mfwu::malloc_alloc>>(it);
    }
    res = set1.erase(5);
    print_suc_info<int, int_hash, mfwu::DefaultAllocator<
        mfwu::sbucket_exp<int>, mfwu::malloc_alloc>>(res);
    res = set1.erase(10);
    print_suc_info<int, int_hash, mfwu::DefaultAllocator<
        mfwu::sbucket_exp<int>, mfwu::malloc_alloc>>(res);
    set1.swap(set2);
    print_unordered_multiset(set1);
    set1.swap(set2);
    print_unordered_multiset(set1);

#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
    std::cout << (!!set1.count(6) 
                    == set1.contains(6) ? "true" : "false") << "\n";
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__
    it = set1.find(6);
    print_it_info<int, int_hash, mfwu::DefaultAllocator<
        mfwu::sbucket_exp<int>, mfwu::malloc_alloc>>(it);
    return 0;
}

bool unit_test_unordered_multiset::use_other_interface() {
    std::cout << "\n------- Test: use other interfaces -------\n";
    test_set set1 = {2, 53, 12, 23, 1, 60};
    std::cout << "allocating & deallocating\n";
    auto alc = set1.get_allocator();
    auto ptr = alc.allocate(2);
    alc.deallocate(ptr, 2);
    std::cout << "hashing\n";
    auto hasher = set1.hash_function();
#ifdef __UNIT_TEST_UNORDERED_MULTISET__
#ifndef __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
    std::cout << hasher(-主) << "\n";
#endif  // __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MULTISET__

    std::cout << "testing interfaces\n";
    print_unordered_multiset_with_iterator(set1);
    print_unordered_multiset_other_info(set1);
    std::cout << "rehashing capacity = 50...\n";
    set1.rehash(50);
    print_unordered_multiset_with_iterator(set1);
    print_unordered_multiset_other_info(set1);
    std::cout << "reserving size = 50...\n";
    set1.reserve(50);
    print_unordered_multiset_with_iterator(set1);
    print_unordered_multiset_other_info(set1);
    std::cout << "clearing...\n";
    set1.clear();
    print_unordered_multiset_with_iterator(set1);
    print_unordered_multiset_other_info(set1);
    test_set set2 = {};
    set2.load_factor();
    return 0;
}

}  // endof namespace mfwu
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
    template <typename Key, typename Value, typename Hash, typename Alloc>
    void print_unordered_map(
        const unordered_map<Key, Value, Hash, Alloc>& map) {
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
        ut_hashtable.print_hashtable(map.htbl_);
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    }
    template <typename Key, typename Value, typename Hash, typename Alloc>
    void print_ret_info(
        typename unordered_map<Key, Value, Hash, Alloc>::iterator it,
        bool is_new) {
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
        std::cout << "*it = [" << it->first << ", " << it->second << "]\n";
        std::cout << "is new: " << (is_new ? "true" : "false") << "\n"; 
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    }
    template <typename Key, typename Value, typename Hash, typename Alloc>
    void print_it_info(
        typename unordered_map<Key, Value, Hash, Alloc>::iterator it) {
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
        std::cout << "*it = [" << it->first << ", " << it->second << "]\n";
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    }
    template <typename Key, typename Value, typename Hash, typename Alloc>
    void print_suc_info(bool is_new) {
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
        std::cout << "is successful: " << (is_new ? "true" : "false") << "\n"; 
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    }
    template <typename Key, typename Value, typename Hash, typename Alloc>
    void print_unordered_map_with_iterator(
        const unordered_map<Key, Value, Hash, Alloc>& map) {
        for (auto it = map.begin(); it != map.end(); ++it) {
        const auto& [key, val] = *it;
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
        std::cout << "key: " << key << ", val: " << val << "\n";
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
        }
    }
    template <typename Key, typename Value, typename Hash, typename Alloc>
    void print_unordered_map_other_info(
        const unordered_map<Key, Value, Hash, Alloc>& map) {
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
        std::cout << "empty: " << (map.empty() ? "true" : "false") << "\n";
        std::cout << "size: " << map.size() << "\n";
        std::cout << "load_factor: " << map.load_factor() << "\n";
        std::cout << "max_size: " << map.max_size() << "\n";
        std::cout << "max_load_factor: " << map.max_load_factor() << "\n";
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    }
};  // endof class unit_test_unordered_map

bool unit_test_unordered_map::use_mfwu_unordered_map() {
    std::cout << "\n------- Test: use mfwu::unordered_map -------\n";
    std::cout << "constructing & copying & moving & destroying\n";
    // using test_map = mfwu::unordered_map<data, int, data_hash>;
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
    print_unordered_map(map7);
    mfwu::unordered_map<int, int, int_hash> map8(map7);
    print_unordered_map(map8);
    map4 = map6;
    map4 = mfwu::move(map6);
    print_unordered_map(map4);

    return 0;
}

bool unit_test_unordered_map::use_modifier_and_lookup() {
    std::cout << "\n------- Test: use modifiers and lookups -------\n";
    test_map map1 = {{data{2}, 2}, {data{2}, 3}, 
                     {data{20}, 6}, {data{66}, 7}};
    auto [it, res] = map1.insert({data{10}, 5});
    print_ret_info<data, int, data_hash, mfwu::DefaultAllocator<
        mfwu::bucket<data, int>, mfwu::malloc_alloc>>(it, res);
    print_unordered_map(map1);
    map1.insert({{data{1}, 1}, {data{2}, 2}});
    print_unordered_map(map1);
    test_map map2 = {{data{10}, 3}, {data{13}, 5}, {data{5}, 8}};
    map1.insert(map2.begin(), map2.end());
    print_unordered_map(map1);
    auto [it2, res2] = map1.emplace(data{10}, 9);
    print_ret_info<data, int, data_hash, mfwu::DefaultAllocator<
        mfwu::bucket<data, int>, mfwu::malloc_alloc>>(it2, res2);
    print_unordered_map(map1);
    it = map1.erase(it2);
    if (it != map1.end()) {
        print_it_info<data, int, data_hash, mfwu::DefaultAllocator<
            mfwu::bucket<data, int>, mfwu::malloc_alloc>>(it);
    }
    res = map1.erase(data{5});
    print_suc_info<data, int, data_hash, mfwu::DefaultAllocator<
        mfwu::bucket<data, int>, mfwu::malloc_alloc>>(res);
    res = map1.erase(data{10});
    print_suc_info<data, int, data_hash, mfwu::DefaultAllocator<
        mfwu::bucket<data, int>, mfwu::malloc_alloc>>(res);
    map1.swap(map2);
    print_unordered_map(map1);
    map1.swap(map2);
    print_unordered_map(map1);

#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
    std::cout << (!!map1.count(data{20}) 
                    == map1.contains(data{20}) ? "true" : "false") << "\n";
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    it = map1.find(data{20});
    print_it_info<data, int, data_hash, mfwu::DefaultAllocator<
        mfwu::bucket<data, int>, mfwu::malloc_alloc>>(it);
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
    std::cout << map1[data{20}] << "\n";
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__
    return 0;
}

bool unit_test_unordered_map::use_other_interface() {
    std::cout << "\n------- Test: use other interfaces -------\n";
    test_map map1 = {{data{2}, 2}, {data{2}, 3}, 
                     {data{12}, 4}, {data{20}, 6},
                     {data{51}, 4}, {data{66}, 7}};
    std::cout << "allocating & deallocating\n";
    auto alc = map1.get_allocator();
    auto ptr = alc.allocate(2);
    alc.deallocate(ptr, 2);
    std::cout << "hashing\n";
    auto hasher = map1.hash_function();
#ifdef __UNIT_TEST_UNORDERED_MAP__
#ifndef __UNIT_TEST_UNORDERED_MAP_BRIEF__
    std::cout << hasher(data{-主}) << "\n";
#endif  // __UNIT_TEST_UNORDERED_MAP_BRIEF__
#endif  // __UNIT_TEST_UNORDERED_MAP__

    std::cout << "testing interfaces\n";
    print_unordered_map_with_iterator(map1);
    print_unordered_map_other_info(map1);
    std::cout << "rehashing capacity = 50...\n";
    map1.rehash(50);
    print_unordered_map_with_iterator(map1);
    print_unordered_map_other_info(map1);
    std::cout << "reserving size = 50...\n";
    map1.reserve(50);
    print_unordered_map_with_iterator(map1);
    print_unordered_map_other_info(map1);
    std::cout << "clearing...\n";
    map1.clear();
    print_unordered_map_with_iterator(map1);
    print_unordered_map_other_info(map1);
    test_map map2 = {};
    map2.load_factor();  // capacity_ will never be zero :P 10.23
    return 0;
}

}  // endof namespace mfwu
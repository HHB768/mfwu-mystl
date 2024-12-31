#define __UNIT_TEST_MULTIMAP_

#include "multimap.hpp"
#include "ut_rbtree_exp.hpp"

namespace mfwu {

class unit_test_multimap {
public:
    bool use_mfwu_multimap();
    bool use_modifier_and_lookup();
    bool use_other_interface();
private:
    template <typename Key, typename Value, typename Compare>
    static mfwu::pair<typename mfwu::rbtree_exp<Key, Compare>::node*,
                      typename mfwu::rbtree_exp<Value, Compare>::node*>
    split_multimap(const multimap<Key, Value, Compare>& m) {
        using node1 = typename mfwu::rbtree_exp<Key, Compare>::node;
        using node2 = typename mfwu::rbtree_exp<Value, Compare>::node;
        typename mfwu::drbtree_exp<Key, Value, Compare>::node* root = m.rbt_.get_tree();
        node1* root1 = nullptr;
        node2* root2 = nullptr;

        if (root) {
            root1 = new node1(root->val.first, 
                              mfwu::drbtree_exp<Key, Value, Compare>::black);
            root2 = new node2(root->val.second, 
                              mfwu::drbtree_exp<Key, Value, Compare>::black);
            split_multimap_aux<Key, Value, Compare>(root, root1, root2);
        }
        
        return {root1, root2};
    }  // TODO: do this everywhere 241120

    template <typename Key, typename Value, typename Compare>
    static void split_multimap_aux(typename mfwu::drbtree_exp<Key, Value, Compare>::node* root,
                       typename mfwu::rbtree_exp<Key, Compare>::node* root1,
                       typename mfwu::rbtree_exp<Value, Compare>::node* root2) {
        using node1 = typename mfwu::rbtree_exp<Key, Compare>::node;
        using node2 = typename mfwu::rbtree_exp<Value, Compare>::node;
        
        if (root->left) {
            root1->left = new node1(root->left->val.first,
                                    root->left->color,
                                    nullptr, nullptr, root1);
            root2->left = new node2(root->left->val.second,
                                    root->left->color,
                                    nullptr, nullptr, root2);
            split_multimap_aux<Key, Value, Compare>(root->left, root1->left, root2->left);
        }
        if (root->right) {
            root1->right = new node1(root->right->val.first,
                                    root->right->color,
                                    nullptr, nullptr, root1);
            root2->right = new node2(root->right->val.second,
                                    root->right->color,
                                    nullptr, nullptr, root2);
            split_multimap_aux<Key, Value, Compare>(root->right, root1->right, root2->right);
        }
    }

    template <typename Key, typename Value, typename Compare>
    static void print_multimap(const mfwu::multimap<Key, Value, Compare>& m) {
        // auto [root1, root2] = split_multimap(m);  // can i?
        mfwu::pair<typename mfwu::rbtree_exp<Key, Compare>::node*,
                   typename mfwu::rbtree_exp<Value, Compare>::node*>
        res = split_multimap(m);
#ifdef __UNIT_TEST_MULTIMAP_
#ifndef __UNIT_TEST_MULTIMAP_BRIEF__  
        unit_test_rbtree::print_colored_struct(res.first);
        unit_test_rbtree::is_valid_rbtree(res.first, m.rbt_.get_cmp());
        unit_test_rbtree::print_colored_struct(res.second);
#endif  // __UNIT_TEST_MULTIMAP_BRIEF__
#endif  // __UNIT_TEST_MULTIMAP_
    }

};  // endof class unit_test_multimap

bool unit_test_multimap::use_mfwu_multimap() {
    std::cout << "\n------- Test: use mfwu::multimap -------\n";
    std::cout << "constructing & copying & moving & destroying\n";
    mfwu::multimap<int, char> m1;
    print_multimap(m1);
    mfwu::multimap<int, long> m2 = {{0, 0}, {0, 0}, {5, 5}, {6, 7}, {1, 1},
                                    {8, 8}, {5, 5}, {3, 3}, {4, 4}, {1, 1},
                                    {3, 3}, {5, 5}, {7, 7}, {9, 9}, {2, 2},
                                    {4, 4}, {6, 6}, {8, 8}};
    print_multimap(m2);
    mfwu::multimap<int, long> m3 = m2;
    print_multimap(m3);
    mfwu::multimap<int, long> m4 = mfwu::move(m2);
    print_multimap(m4);
    mfwu::multimap<int, long> m5;
    m5 = m3;
    m5 = mfwu::move(m4);
    print_multimap(m5);
    mfwu::multimap<char, char> m6(++m5.begin(), m5.end());
    // less node but wider tree [1206]
    print_multimap(m6);
    mfwu::multimap<long, int, mfwu::greater<int>> 
    s7(++m6.begin(), ++++++++++++m6.begin());
    // TODO: test greater multimap too 241121
    print_multimap(s7);

    return 0;
}

bool unit_test_multimap::use_modifier_and_lookup() {
    std::cout << "\n------- Test: use modifiers and lookups -------\n";
    std::cout << "inserting & erasing\n";
    mfwu::multimap<int, int> m1 = {{1, 1}, {9, 9}, {8, 8}, {3, 3}};
    print_multimap(m1);
    m1.insert(4, 4);
    print_multimap(m1);
    m1.insert(4, 4);
    print_multimap(m1);
    m1.insert(7, 7);
    print_multimap(m1);
    m1.erase({3, 42});
    print_multimap(m1);
    m1.erase(6);
    print_multimap(m1);
    m1.emplace(6, 'a');
    print_multimap(m1);
    m1.erase(++m1.begin());
    print_multimap(m1);

    std::cout << "clearing & ranged_inserting & ranged_erasing "
              << "& swapping & merging\n";
    mfwu::multimap<int, int> m2 = m1;
    m2.clear();
    print_multimap(m2);
    m2.insert(m1.begin(), ++m1.begin());
    print_multimap(m2);
    m1.swap(m2);
    print_multimap(m2);
    m2.insert({2, 2});
    m1.insert(6, 6);
    print_multimap(m1);
    print_multimap(m2);
    m1.merge(m2);
    print_multimap(m1);
    print_multimap(m2);
    m1.erase(m1.begin(), m1.end());
    print_multimap(m1);

    std::cout << "finding & counting & containing & calcing equal_range\n";
#ifdef __UNIT_TEST_MULTIMAP_
#ifndef __UNIT_TEST_MULTIMAP_BRIEF__  
    std::cout << mfwu::distance(m1.find(2), m1.end()) << "\n";
    std::cout << m1.count(2) << "\n";
    std::cout << mfwu::distance(m2.find(1), m2.end()) << "\n";
    std::cout << m2.contains(1) << "\n";
#else  // __UNIT_TEST_SET_BRIEF__
    mfwu::distance(m1.find(2), m1.end());
    m1.count(2);
#endif  // __UNIT_TEST_MULTIMAP_BRIEF__
#endif  // __UNIT_TEST_MULTIMAP_

    auto [it1, it2] = m1.equal_range(1);
    auto [it3, it4] = m2.equal_range(2);
#ifdef __UNIT_TEST_MULTIMAP_
#ifndef __UNIT_TEST_MULTIMAP_BRIEF__  
    std::cout << mfwu::distance(m1.begin(), it1) << ", "
              << mfwu::distance(m1.begin(), it2) << ", "
              << mfwu::distance(m2.begin(), it3) << ", "
              << mfwu::distance(m2.begin(), it4) << "\n";
#endif  // __UNIT_TEST_MULTIMAP_BRIEF__
#endif  // __UNIT_TEST_MULTIMAP_

    return 0;
}

bool unit_test_multimap::use_other_interface() {
    std::cout << "\n------- Test: use other interfaces -------\n";
    mfwu::multimap<char, int> m1 = {{1, 1}, {2, 2}, {6, 6}, {9, 9}, 
                                    {8, 8}, {7, 7}, {3, 3}, {4, 4}};
#ifdef __UNIT_TEST_MULTIMAP_
#ifndef __UNIT_TEST_MULTIMAP_BRIEF__  
    std::cout << "empty(): " << m1.empty() << "\n";
    std::cout << "size(): " << m1.size() << "\n";
    std::cout << "height(): " << m1.height() << "\n";
    // std::cout << "root().val: " << m1.root().second << "\n";
    std::cout << "max_size(): " << m1.max_size() << "\n";
#endif  // __UNIT_TEST_MULTIMAP_BRIEF__
#endif  // __UNIT_TEST_MULTIMAP_

    return 0;
}

// TODO: BRIEF

}  // endof namespace mfwu
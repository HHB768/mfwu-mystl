#define __ALL_BRIEF__

#ifdef __ALL_BRIEF__
#   define __UNIT_TEST_ALLOCATOR_BRIEF__
#   define __UNIT_TEST_ITERATOR_BRIEF__
#   define __UNIT_TEST_UTILS_BRIEF__
#   define __UNIT_TEST_VECTOR_BRIEF__
#   define __UNIT_TEST_LIST_BRIEF__
#   define __UNIT_TEST_HEAP_BRIEF__
#   define __UNIT_TEST_BST_BRIEF__
#   define __UNIT_TEST_AVL_TREE_BRIEF__
#   define __UNIT_TEST_RBTREE_BRIEF__
#   define __UNIT_TEST_RBF_BRIEF__
#   define __UNIT_TEST_HASHTABLE_BRIEF__
#   define __UNIT_TEST_UNORDERED_MAP_BRIEF__
#   define __UNIT_TEST_UNORDERED_SET_BRIEF__
#   define __UNIT_TEST_DEQUE_BRIEF__
#   define __UNIT_TEST_STACK_BRIEF__
#   define __UNIT_TEST_QUEUE_BRIEF__
#   define __UNIT_TEST_SET_BRIEF__
#   define __UNIT_TEST_MAP_BRIEF__
#   define __UNIT_TEST_MULTISET_BRIEF__
#   define __UNIT_TEST_MULTIMAP_BRIEF__
#   define __UNIT_TEST_HASHTABLE_EXP_BRIEF__
#   define __UNIT_TEST_UNORDERED_MULTIMAP_BRIEF__
#   define __UNIT_TEST_UNORDERED_MULTISET_BRIEF__
// #   define __UNIT_TEST_STRING_BRIEF__
#   define __UNIT_TEST_TRIE_BRIEF__
#endif  // __ALL_BRIEF__

// #define __USE_MALLOC__

#ifndef __UNIT_TEST_VECTOR_BRIEF__
#   define __VECTOR_MEMORY_CHECK__
#endif  // __UNIT_TEST_VECTOR_BRIEF__

#include "ut_allocator.hpp"
#include "ut_iterator.hpp"
#include "ut_utils.hpp"
#include "ut_vector.hpp"
#include "ut_list.hpp"
#include "ut_heap.hpp"
#include "ut_bst.hpp"
#include "ut_avl_tree.hpp"
#include "ut_rbtree.hpp"
#include "ut_deque.hpp"
#include "ut_rbf.hpp"
#include "ut_hashtable.hpp"
#include "ut_unordered_map.hpp"
#include "ut_unordered_set.hpp"
#include "ut_stack.hpp"
#include "ut_queue.hpp"
#include "ut_set.hpp"
#include "ut_map.hpp"
#include "ut_rbtree_exp.hpp"
#include "ut_multiset.hpp"
#include "ut_multimap.hpp"
#include "ut_hashtable_exp.hpp"
#include "ut_unordered_multimap.hpp"
#include "ut_unordered_multiset.hpp"
#include "ut_string.hpp"
#include "ut_trie.hpp"

static size_t ttotal = 0;
static size_t sscore = 0;

size_t unit_test() {
    return 0;
}

template <typename UtFunc, typename... ResUtFunc>
size_t unit_test(UtFunc ut_func, ResUtFunc... other_func) {
    return !ut_func() + unit_test(other_func...);
}

template <typename... UtFunc>
void unit_test(const char* title, UtFunc... ut_func) {
    size_t total = sizeof...(ut_func);
    size_t score = unit_test(ut_func...);
    std::cout << "\n----------- " << title << " test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";
    ttotal += total; sscore += score;
}

// void update_score(const mfwu::pair<size_t, size_t>& p) {
//     ttotal += p.first;
//     sscore += p.second;
// }

int main() {
    mfwu::unit_test_allocator ut_alloc;
    mfwu::unit_test_iterator ut_iter;
    mfwu::unit_test_utils ut_utils;
    mfwu::unit_test_vector ut_vector;
    mfwu::unit_test_list ut_list;
    mfwu::unit_test_heap ut_heap;
    mfwu::unit_test_bst ut_bst;
    mfwu::unit_test_avl_tree ut_avl_tree;
    mfwu::unit_test_rbtree ut_rbtree;
    // mfwu::unit_test_deque ut_deque;
    mfwu::unit_test_rbf ut_rbf;
    mfwu::unit_test_hashtable ut_htbl;
    mfwu::unit_test_unordered_map ut_uod_map;
    mfwu::unit_test_unordered_set ut_uod_set;
    mfwu::unit_test_deque ut_deque;
    mfwu::unit_test_stack ut_stack;
    mfwu::unit_test_queue ut_queue;
    mfwu::unit_test_set ut_set;
    mfwu::unit_test_map ut_map;
    mfwu::unit_test_rbtree_exp ut_rbte; 
    mfwu::unit_test_multiset ut_mtset;
    mfwu::unit_test_multimap ut_mtmap;
    mfwu::unit_test_hashtable_exp ut_htble;
    mfwu::unit_test_unordered_multimap ut_uod_mmap;
    mfwu::unit_test_unordered_multiset ut_uod_mset;
    mfwu::unit_test_string ut_string;
    mfwu::unit_test_trie ut_trie;

    size_t score = 0;
    size_t total = 0;

    // TODO: use mfwu::tie and mfwu::pair instead

#define ut_func(func, obj) std::bind(&func, &obj)

    unit_test(
        "Alloc",
        ut_func(mfwu::unit_test_allocator::use_malloc_allocator, ut_alloc),
        ut_func(mfwu::unit_test_allocator::use_default_allocator, ut_alloc)
    );
    
    unit_test(
        "Iterator",
        ut_func(mfwu::unit_test_iterator::use_list_iterator, ut_iter),
        ut_func(mfwu::unit_test_iterator::use_input_iterator, ut_iter),
        ut_func(mfwu::unit_test_iterator::use_output_iterator, ut_iter),
        ut_func(mfwu::unit_test_iterator::use_forward_iterator, ut_iter),
        ut_func(mfwu::unit_test_iterator::use_bidirectional_iterator, ut_iter),
        ut_func(mfwu::unit_test_iterator::use_random_access_iterator, ut_iter)
    );

    unit_test(
        "Utils",
        ut_func(mfwu::unit_test_utils::use_construct_destroy, ut_utils),
        ut_func(mfwu::unit_test_utils::use_uninitialized_op, ut_utils),
        ut_func(mfwu::unit_test_utils::use_advance_distance, ut_utils)
    );

    unit_test(
        "Vector",
        ut_func(mfwu::unit_test_vector::use_vector_iterator, ut_vector),
        ut_func(mfwu::unit_test_vector::use_mfwu_vector, ut_vector),
        ut_func(mfwu::unit_test_vector::use_vector_algo, ut_vector)
    );

    unit_test(
        "List",
        ut_func(mfwu::unit_test_list::use_forward_list_iterator, ut_list),
        ut_func(mfwu::unit_test_list::use_mfwu_forward_list, ut_list),
        ut_func(mfwu::unit_test_list::use_list_iterator, ut_list),
        ut_func(mfwu::unit_test_list::use_mfwu_list, ut_list)
    );

    unit_test(
        "Heap",
        ut_func(mfwu::unit_test_heap::use_mfwu_heap, ut_heap),
        ut_func(mfwu::unit_test_heap::use_cmp_functor, ut_heap)
    );

    unit_test(
        "BST",
        ut_func(mfwu::unit_test_bst::use_mfwu_bst, ut_bst),
        ut_func(mfwu::unit_test_bst::use_cmp_functor, ut_bst)
    );

    unit_test(
        "AVL_tree",
        ut_func(mfwu::unit_test_avl_tree::use_avl_tree, ut_avl_tree),
        ut_func(mfwu::unit_test_avl_tree::use_cmp_functor, ut_avl_tree)
    );

    unit_test(
        "Rbtree",
        ut_func(mfwu::unit_test_rbtree::use_rbtree_insert, ut_rbtree),
        ut_func(mfwu::unit_test_rbtree::use_rbtree_erase, ut_rbtree),
        ut_func(mfwu::unit_test_rbtree::use_cmp_functor, ut_rbtree)
    );

    // legacy: [archieved 241129]
    // score = 0;
    // total = 3;
    // ttotal += total;
    // score += !ut_deque.use_deque_block();
    // score += !ut_deque.use_deque_iterator();
    // score += !ut_deque.use_mfwu_deque();
    // std::cout << "\n---------- Deque test result -----------\n";
    // std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    unit_test(
        "Rbf",
        ut_func(mfwu::unit_test_rbf::use_mfwu_rbf, ut_rbf)  
    );

    unit_test(
        "Hashtable",
        ut_func(mfwu::unit_test_hashtable::use_mfwu_hashtable, ut_htbl),
        ut_func(mfwu::unit_test_hashtable::use_mfwu_shashtable, ut_htbl)
    );
    
    unit_test(
        "Unordered_map",
        ut_func(mfwu::unit_test_unordered_map::use_mfwu_unordered_map, ut_uod_map),
        ut_func(mfwu::unit_test_unordered_map::use_modifier_and_lookup, ut_uod_map),
        ut_func(mfwu::unit_test_unordered_map::use_other_interface, ut_uod_map)
    );

    unit_test(
        "Unordered_set",
        ut_func(mfwu::unit_test_unordered_set::use_mfwu_unordered_set, ut_uod_set),
        ut_func(mfwu::unit_test_unordered_set::use_modifier_and_lookup, ut_uod_set),
        ut_func(mfwu::unit_test_unordered_set::use_other_interface, ut_uod_set)
    );

    unit_test(
        "Deque",
        ut_func(mfwu::unit_test_deque::use_deque_block, ut_deque),
        ut_func(mfwu::unit_test_deque::use_deque_iterator, ut_deque),
        ut_func(mfwu::unit_test_deque::use_mfwu_deque, ut_deque),
        ut_func(mfwu::unit_test_deque::use_ranged_insert_erase, ut_deque)
    );

    unit_test(
        "Stack",
        ut_func(mfwu::unit_test_stack::use_mfwu_stack, ut_stack)
    );

    unit_test(
        "Queue",
        ut_func(mfwu::unit_test_queue::use_mfwu_queue, ut_queue)
    );

    unit_test(
        "Set",
        ut_func(mfwu::unit_test_set::use_mfwu_set, ut_set),
        ut_func(mfwu::unit_test_set::use_modifier_and_lookup, ut_set),
        ut_func(mfwu::unit_test_set::use_other_interface, ut_set)
    );

    unit_test(
        "Map",
        ut_func(mfwu::unit_test_map::use_mfwu_map, ut_map),
        ut_func(mfwu::unit_test_map::use_modifier_and_lookup, ut_map),
        ut_func(mfwu::unit_test_map::use_other_interface, ut_map)
    );

    unit_test(
        "Rbtree_exp",
        ut_func(mfwu::unit_test_rbtree_exp::use_rbtree_exp, ut_rbte)
    );

    unit_test(
        "Multiset",
        ut_func(mfwu::unit_test_multiset::use_mfwu_multiset, ut_mtset),
        ut_func(mfwu::unit_test_multiset::use_modifier_and_lookup, ut_mtset),
        ut_func(mfwu::unit_test_multiset::use_other_interface, ut_mtset)
    );

    unit_test(
        "Multimap",
        ut_func(mfwu::unit_test_multimap::use_mfwu_multimap, ut_mtmap),
        ut_func(mfwu::unit_test_multimap::use_modifier_and_lookup, ut_mtmap),
        ut_func(mfwu::unit_test_multimap::use_other_interface, ut_mtmap)
    );

    unit_test(
        "Hashtable_exp",
        ut_func(mfwu::unit_test_hashtable_exp::use_mfwu_hashtable_exp, ut_htble),
        ut_func(mfwu::unit_test_hashtable_exp::use_mfwu_shashtable_exp, ut_htble)
    );

    unit_test(
        "Unordered_multimap",
        ut_func(mfwu::unit_test_unordered_multimap::use_mfwu_unordered_multimap, ut_uod_mmap),
        ut_func(mfwu::unit_test_unordered_multimap::use_modifier_and_lookup, ut_uod_mmap),
        ut_func(mfwu::unit_test_unordered_multimap::use_other_interface, ut_uod_mmap)
    );

    unit_test(
        "Unordered_multiset",
        ut_func(mfwu::unit_test_unordered_multiset::use_mfwu_unordered_multiset, ut_uod_mset),
        ut_func(mfwu::unit_test_unordered_multiset::use_modifier_and_lookup, ut_uod_mset),
        ut_func(mfwu::unit_test_unordered_multiset::use_other_interface, ut_uod_mset)
    );

    unit_test(
        "String",
        ut_func(mfwu::unit_test_string::use_mfwu_string, ut_string),
        ut_func(mfwu::unit_test_string::use_mfwu_tiny_string, ut_string)
    );

    unit_test(
        "Trie",
        ut_func(mfwu::unit_test_trie::use_mfwu_trie, ut_trie)
    );
    
    std::cout << "\n---------- test result -----------\n";
    std::cout << "\nTotal number of unit tests: " << ttotal;
    std::cout << "\nEndof unit tests\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++\n";

    return 0;
}

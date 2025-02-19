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


size_t unit_test() {
    return 1;
}

template <typename UtFunc, typename... ResUtFunc>
size_t unit_test(UtFunc ut_func, ResUtFunc... other_func) {
    return !ut_func() + unit_test(other_func...);
}

template <typename... UtFunc>
std::pair<size_t, size_t> unit_test(const char* title, UtFunc... ut_func) {
    size_t total = sizeof...(ut_func);
    size_t score = unit_test(ut_func...);
    std::cout << "\n----------- " << title << " test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";
    return {score, total};
}


int main() {
    size_t ttotal = 0;
    size_t sscore = 0;

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

    size_t score = 0;
    size_t total = 0;

    // TODO: use mfwu::tie and mfwu::pair instead
    
    std::tie(score, total) = unit_test("Alloc", 
        std::bind(&mfwu::unit_test_allocator::use_malloc_allocator, &ut_alloc),
        std::bind(&mfwu::unit_test_allocator::use_default_allocator, &ut_alloc));
    ttotal += total;
    sscore += score;

    score = 0;
    total = 6;
    ttotal += total;
    score += !ut_iter.use_list_iterator();
    score += !ut_iter.use_input_iterator();
    score += !ut_iter.use_output_iterator();
    score += !ut_iter.use_forward_iterator();
    score += !ut_iter.use_bidirectional_iterator();
    score += !ut_iter.use_random_access_iterator();
    std::cout << "\n---------- Iterator test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_utils.use_construct_destroy();
    score += !ut_utils.use_uninitialized_op();
    score += !ut_utils.use_advance_distance();
    std::cout << "\n---------- Utils test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_vector.use_vector_iterator();
    score += !ut_vector.use_mfwu_vector();
    score += !ut_vector.use_vector_algo();
    std::cout << "\n---------- Vector test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";


    score = 0;
    total = 4;
    ttotal += total;
    score += !ut_list.use_forward_list_iterator();
    score += !ut_list.use_mfwu_forward_list();
    score += !ut_list.use_list_iterator();
    score += !ut_list.use_mfwu_list();
    std::cout << "\n---------- List test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 2;
    ttotal += total;
    score += !ut_heap.use_mfwu_heap();
    score += !ut_heap.use_cmp_functor();
    std::cout << "\n---------- Heap test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 2;
    ttotal += total;
    score += !ut_bst.use_mfwu_bst();
    score += !ut_bst.use_cmp_functor();
    std::cout << "\n---------- BST test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 2;
    ttotal += total;
    score += !ut_avl_tree.use_avl_tree();
    score += !ut_avl_tree.use_cmp_functor();
    std::cout << "\n---------- AVL_tree test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_rbtree.use_rbtree_insert();
    score += !ut_rbtree.use_rbtree_erase();
    score += !ut_rbtree.use_cmp_functor();
    std::cout << "\n---------- Rbtree test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    // score = 0;
    // total = 3;
    // ttotal += total;
    // score += !ut_deque.use_deque_block();
    // score += !ut_deque.use_deque_iterator();
    // score += !ut_deque.use_mfwu_deque();
    // std::cout << "\n---------- Deque test result -----------\n";
    // std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";


    score = 0;
    total = 1;
    ttotal += total;
    score += !ut_rbf.use_mfwu_rbf();
    std::cout << "\n---------- Rbf test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 2;
    ttotal += total;
    score += !ut_htbl.use_mfwu_hashtable();
    score += !ut_htbl.use_mfwu_shashtable();
    std::cout << "\n---------- Hashtable test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_uod_map.use_mfwu_unordered_map();
    score += !ut_uod_map.use_modifier_and_lookup();
    score += !ut_uod_map.use_other_interface();
    std::cout << "\n---------- Unordered_map test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_uod_set.use_mfwu_unordered_set();
    score += !ut_uod_set.use_modifier_and_lookup();
    score += !ut_uod_set.use_other_interface();
    std::cout << "\n---------- Unordered_set test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 4;
    ttotal += total;
    score += !ut_deque.use_deque_block();
    score += !ut_deque.use_deque_iterator();
    score += !ut_deque.use_mfwu_deque();
    score += !ut_deque.use_ranged_insert_erase();
    std::cout << "\n---------- Deque test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 1;
    ttotal += total;
    score += !ut_stack.use_mfwu_stack();
    std::cout << "\n---------- Stack test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 1;
    ttotal += total;
    score += !ut_queue.use_mfwu_queue();
    std::cout << "\n---------- Queue test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_set.use_mfwu_set();
    score += !ut_set.use_modifier_and_lookup();
    score += !ut_set.use_other_interface();
    std::cout << "\n---------- Set test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 3;
    ttotal += total;
    score += !ut_map.use_mfwu_map();
    score += !ut_map.use_modifier_and_lookup();
    score += !ut_map.use_other_interface();
    std::cout << "\n---------- Map test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    // TODO: use function or marco
    
    std::cout << "\n---------- test result -----------\n";
    std::cout << "\nTotal number of unit tests: " << ttotal;
    std::cout << "\nEndof unit tests\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++\n";

    return 0;
}
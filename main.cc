#define __ALL_BRIEF__

#ifdef __ALL_BRIEF__
#   define __UNIT_TEST_ALLOCATOR_BRIEF__
#   define __UNIT_TEST_ITERATOR_BRIEF__
#   define __UNIT_TEST_UTILS_BRIEF__
#   define __UNIT_TEST_VECTOR_BRIEF__
#   define __UNIT_TEST_LIST_BRIEF__
#   define __UNIT_TEST_HEAP_BRIEF__
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

int main() {
    int ttotal = 0;
    mfwu::unit_test_allocator ut_alloc;
    mfwu::unit_test_iterator ut_iter;
    mfwu::unit_test_utils ut_utils;
    mfwu::unit_test_vector ut_vector;
    mfwu::unit_test_list ut_list;
    mfwu::unit_test_heap ut_heap;
    mfwu::unit_test_bst ut_bst;

    int score = 0;
    int total = 2;
    ttotal += total;
    score += !ut_alloc.use_malloc_allocator();
    score += !ut_alloc.use_default_allocator();
    std::cout << "\n----------- Alloc test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

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
    total = 1;
    ttotal += total;
    score += !ut_bst.use_mfwu_bst();
    std::cout << "\n---------- BST test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";


    std::cout << "\n---------- test result -----------\n";
    std::cout << "\nTotal number of unit tests: " << ttotal;
    std::cout << "\nEndof unit tests\n";
    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++\n";

    return 0;
}
#define __UNIT_TEST_ALLOCATOR_BRIEF__
#define __UNIT_TEST_ITERATOR_BRIEF__

#ifdef __ALL_BRIEF__
#   define __UNIT_TEST_ALLOCATOR_BRIEF__
#   define __UNIT_TEST_ITERATOR_BRIEF__
#endif  // __ALL_BRIEF__

#include "ut_allocator.hpp"
#include "ut_iterator.hpp"

int main() {
    int ttotal = 0;
    mfwu::unit_test_allocator ut_alloc;
    mfwu::unit_test_iterator ut_iter;

    int score = 0;
    int total = 2;
    ttotal += total;
    score += !ut_alloc.use_malloc_allocator();
    score += !ut_alloc.use_default_allocator();
    std::cout << "\n----------- Alloc test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";

    score = 0;
    total = 1;
    ttotal += total;
    score += !ut_iter.use_list_iterator();
    std::cout << "\n---------- Iterator test result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n\n\n";
    return 0;
}
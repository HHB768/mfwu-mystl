#include "ut_allocator.hpp"
#include "ut_iterator.hpp"

int main() {
    int score = 0;
    const int total = 2;
    // mfwu::unit_test_allocator ut_alloc;
    mfwu::unit_test_iterator ut_iter;
    // score += !ut_alloc.use_malloc_allocator();
    // score += !ut_alloc.use_default_allocator();
    score += !ut_iter.use_list_iterator();
    std::cout << "----------- result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n";
    return 0;
}
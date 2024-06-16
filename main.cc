#include "ut_allocator.hpp"

int main() {
    int score = 0;
    const int total = 1;
    mfwu::unit_test_allocator ut_malloc_alloc;
    score += !ut_malloc_alloc.use_malloc_allocator();
    std::cout << score << "/" << total << "\n";
    return 0;
}
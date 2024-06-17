#include "ut_allocator.hpp"

int main() {
    int score = 0;
    const int total = 2;
    mfwu::unit_test_allocator ut_alloc;
    score += !ut_alloc.use_malloc_allocator();
    score += !ut_alloc.use_default_allocator();

    std::cout << "----------- result -----------\n";
    std::cout << "Pass/Total: " << score << "/" << total << "\n";
    return 0;
}
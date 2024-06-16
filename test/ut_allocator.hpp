#include "allocator.hpp"

namespace mfwu {

class unit_test_allocator {
public:
    bool use_malloc_allocator();
    bool use_default_allocator();
// private:
//     mfwu::malloc_alloc mallocator;
//     mfwu::default_alloc dallocator;
};  // endof class unit_test_allocator

void (*oom_handler_test)() {};

std::string malloc_alloc::identify() {
    return "malloc_alloc";
}

std::string default_alloc::identify() {
    return "default_alloc";
}

bool unit_test_allocator::use_malloc_allocator() {
    std::cout << "In " << malloc_alloc::identify() << "\n";

    std::cout << "allocating 0\n";
    void* start = malloc_alloc::allocate(0);
    std::cout << "ptr: " << start << "\n";
    std::cout << "deallocating 0\n";
    malloc_alloc::deallocate(start, 0);
    
    std::cout << "setting dummy oom_handler\n";
    malloc_alloc::set_malloc_handler(oom_handler_test);

    std::cout << "allocating 1024^3\n";
    start = malloc_alloc::allocate(1024*1024*1024);  // GB?
    std::cout << "ptr: " << start << "\n";
    sleep(5);
    std::cout << "reallocating 1.5*1024^3\n";
    start = malloc_alloc::reallocate(start, 42, 1024*1024*1536);
    std::cout << "ptr: " << start << "\n";
    sleep(5);
    std::cout << "deallocating 1.5*1024^3\n";
    malloc_alloc::deallocate(start, 0);

    std::cout << "allocating and deallocating 10 times\n";
    for (int i = 0; i < 10; ++i) {
        int* p = (int*)malloc_alloc::allocate(i * sizeof(long double));
        std::cout << "ptr: " << p << "\n";
        malloc_alloc::deallocate(p, 424242);
    }
    sleep(2);
    std::cout << "allocating 1000 times\n";
    for (int i = 0; i < 1000; ++i) {
        malloc_alloc::allocate(10 * sizeof(int));  // leaky mem
    }
    sleep(5);
    std::cout << "setting invalid oom_handler\n";
    malloc_alloc::set_malloc_handler(nullptr);
    std::cout << "oom malloc failure testing\n";
    malloc_alloc::oom_malloc(25);
    
    return 0;
}

}  // endof namespace mfwu

// 学到了新知识！
/*
    - use "valgrind --tool=massif ./app" to run ./app
    - output file "massif.out.<pid>"
    - malloc(0) may return nullptr or some none-null ptr that can 
      be used in free(), it depends on the implementation
    - realloc a smaller size, return the same ptr (i guess)
      and maybe actully recycle no memory (if it is a large block)
    - oom_malloc has exit, it clear the normal mem first,
      then clear the leaky mem (size = 40000), u can find a snapshot
      in the massif.out.pid file
*/
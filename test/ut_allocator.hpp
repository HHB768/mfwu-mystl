#include "allocator.hpp"
#define __UNIT_TEST_ALLOCATOR__

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

std::string default_alloc::print_status() {
    std::stringstream ss;
    ss << "memory pool: \n";
    ss << "heap size: " << heap_size << " start-end: " 
       << (void*)start_free << " - " << (void*)end_free << "\n";
    ss << "free lists: \n[ ";
    for (int i=0; i<__NFREELISTS; i++) {
        int num = 0;
        default_alloc::obj* node = free_lists[i];
        while(node) {
            node = node->free_list_link;
            num++;
        }
        ss << num << " ";
    }
    ss << "]\n\n";
    return ss.str();
}

void* default_alloc::print_status_alloc(size_t n) {
    void* res = default_alloc::allocate(n);
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << default_alloc::print_status();
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
    return res;
}

void default_alloc::print_status_dealloc(void* p, size_t n) {
    default_alloc::deallocate(p, n);
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << default_alloc::print_status();
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
}

bool unit_test_allocator::use_malloc_allocator() {
    std::cout << "------- Test: use_malloc_allocator -------\n";
    std::cout << "In " << malloc_alloc::identify() << "\n";

    std::cout << "allocating 0\n";

    void* start = malloc_alloc::allocate(0);
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "ptr: " << start << "\n";
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "deallocating 0\n";
    malloc_alloc::deallocate(start, 0);
    
    std::cout << "setting dummy oom_handler\n";
    malloc_alloc::set_malloc_handler(oom_handler_test);

    std::cout << "allocating 1024^3\n";
    start = malloc_alloc::allocate(1024*1024*1024);  // GB?
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "ptr: " << start << "\n";
    sleep(1);
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "reallocating 1.5*1024^3\n";
    start = malloc_alloc::reallocate(start, 42, 1024*1024*1536);
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "ptr: " << start << "\n";
    sleep(1);
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "deallocating 1.5*1024^3\n";
    malloc_alloc::deallocate(start, 0);

    std::cout << "allocating and deallocating 10 times\n";
    for (int i = 0; i < 10; ++i) {
        int* p = (int*)malloc_alloc::allocate(i * sizeof(long double));
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
        std::cout << "ptr: " << p << "\n";
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
        malloc_alloc::deallocate(p, 424242);
    }
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    sleep(1);
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "allocating 1000 times\n";
    for (int i = 0; i < 1000; ++i) {
        malloc_alloc::allocate(10 * sizeof(int));  // leaky mem
    }
#ifndef __UNIT_TEST_ALLOCATOR_BRIEF__
    sleep(1);
#endif  // __UNIT_TEST_ALLOCATOR_BRIEF__
    std::cout << "setting invalid oom_handler\n";
    malloc_alloc::set_malloc_handler(nullptr);
    std::cout << "oom malloc failure testing: \n";
    malloc_alloc::oom_malloc(25);
    std::cout << "\n";
    
    return 0;
}

bool unit_test_allocator::use_default_allocator() {
    std::cout << "------- Test: use_default_allocator -------\n";
    std::cout << "In " << default_alloc::identify() << "\n";
    std::cout << "malloc_allocating large block\n";
    void* start = default_alloc::print_status_alloc(129);
    std::cout << "malloc_deallocating large block\n";
    default_alloc::print_status_dealloc(start, 129);

    std::cout << "allocating and deallocating random units\n";
    void* p1 = default_alloc::print_status_alloc(1);
    void* p2 = default_alloc::print_status_alloc(4);
    void* p3 = default_alloc::print_status_alloc(13);
    void* p4 = default_alloc::print_status_alloc(62);
    void* p5 = default_alloc::print_status_alloc(24);
    void* p6 = default_alloc::print_status_alloc(25);
    default_alloc::print_status_dealloc(p3, 13);
    void* p7 = default_alloc::print_status_alloc(39);
    default_alloc::print_status_dealloc(p4, 63);  // same blocksize
    void* p8 = default_alloc::print_status_alloc(128);
    void* p9 = default_alloc::print_status_alloc(68);
    default_alloc::print_status_dealloc(p1, 1);
    default_alloc::print_status_dealloc(p9, 68);
    default_alloc::print_status_dealloc(p8, 128);
    default_alloc::print_status_dealloc(p5, 24);
    void* p10 = default_alloc::print_status_alloc(80);
    default_alloc::print_status_dealloc(p6, 25);
    default_alloc::print_status_dealloc(p2, 4);
    default_alloc::print_status_dealloc(p7, 39);
    default_alloc::print_status_dealloc(p10, 80);
    std::cout << "end of random tests\n";
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
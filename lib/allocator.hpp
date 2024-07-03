#ifndef __ALLOCATOR_HPP__
#define __ALLOCATOR_HPP__

#include "common.hpp"

#define __COUNTER_START 0

namespace mfwu {

template <typename T>
class SimpleAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = mfwu::ptrdiff_t;

    template <typename U>
    struct rebind {
        using other = SimpleAllocator<U>;
    };

    pointer allocate(size_type n) {
        return _allocate((difference_type)n);
    }

    void deallocate(pointer p) { _deallocate(p); }

    void construct(pointer p, const_reference value) {
        _construct(p, value);
    }

    void destroy(pointer p) {
        _destroy(p);
    }

private:
    pointer _allocate(difference_type size) {
        std::set_new_handler(0);
        pointer tmp = (pointer)(::operator new((size_type)(size * sizeof(T))));
        if (tmp == nullptr) {
            std::cerr << "out of memory" << std::endl;
            exit(1);
        }
        return tmp;
    }

    void _deallocate(pointer buffer) {
        ::operator delete(buffer);
    }

    void _construct(pointer p, const_reference value) {
        new(p) value_type(value);  // placement new
    }

    void _destroy(pointer ptr) {
        ptr->~T();
    }
};  // endof class SimpleAllocator


class malloc_alloc;
class default_alloc;

#ifdef __USE_MALLOC__
typedef malloc_alloc alloc;
#else  // !__USE_MALLOC__
typedef default_alloc alloc;
#endif  // __USE_MALLOC__

#if 0
#   define __THROW_BAD_ALLOC throw ::std::bad_alloc
#elif !defined __THROW_BAD_ALLOC
#   define __THROW_BAD_ALLOC ::std::cerr << "out of memory" << ::std::endl/*; exit(1)*/
#endif

// stl alloc interface
template <typename T, typename Alloc=alloc>
class DefaultAllocator {
public:
    static volatile int GLOBAL_COUNTER;
    static T* allocate(size_t n=1) {
#ifdef __VECTOR_MEMORY_CHECK__
        print_alloc_info(n);
#endif  // __VECTOR_MEMORY_CHECK__
        return 0 == n ? 0 : (T*)Alloc::allocate(n * sizeof(T));
    }
    static void deallocate(T* p, size_t n=1) {
#ifdef __VECTOR_MEMORY_CHECK__
        print_dealloc_info(n);
#endif  // __VECTOR_MEMORY_CHECK__
        if (0 != n) {
            Alloc::deallocate(p, n * sizeof(T));
        }
    }
    static T* reallocate(T* p, size_t old_n, size_t n=1) {
        T* ret = (T*)Alloc::reallocate(p, old_n * sizeof(T), n * sizeof(T));
        if (ret != nullptr) {
#ifdef __VECTOR_MEMORY_CHECK__
        print_realloc_success_info(n - old_n);
#endif  // __VECTOR_MEMORY_CHECK__
        } else {
#ifdef __VECTOR_MEMORY_CHECK__
        print_realloc_failure_info();
#endif  // __VECTOR_MEMORY_CHECK__
       }
       return ret;
    }
    static void print_alloc_info(const int n) {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
        std::cout << "+++++++++++++++++++++++++\n";
        std::cout << "identity: " << T::identity() << "\n";
        std::cout << "allocate, size = " << n << "\n";
        GLOBAL_COUNTER += n;
        std::cout << "global alloc count = " << GLOBAL_COUNTER << "\n";
        std::cout << "+++++++++++++++++++++++++\n";
#endif  // __UNIT_TEST_VECTOR_BRIEF__ 
    }
    static void print_dealloc_info(const int n) {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
        std::cout << "+++++++++++++++++++++++++\n";
        std::cout << "identity: " << T::identity() << "\n";
        std::cout << "deallocate, size = " << n << "\n";
        GLOBAL_COUNTER -= n;
        std::cout << "global alloc count = " << GLOBAL_COUNTER << "\n";
        std::cout << "+++++++++++++++++++++++++\n";
#endif  // __UNIT_TEST_VECTOR_BRIEF__ 
    }
    static void print_realloc_success_info(const int n) {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
        std::cout << "+++++++++++++++++++++++++\n";
        std::cout << "identity: " << T::identity() << "\n";
        std::cout << "reallocate, size = " << n << "\n";
        GLOBAL_COUNTER += n;
        std::cout << "global alloc count = " << GLOBAL_COUNTER << "\n";
        std::cout << "+++++++++++++++++++++++++\n";
#endif  // __UNIT_TEST_VECTOR_BRIEF__ 
    }
    static void print_realloc_failure_info() {
#ifndef __UNIT_TEST_VECTOR_BRIEF__
        std::cout << "+++++++++++++++++++++++++\n";
        std::cout << "identity: " << T::identity() << "\n";
        std::cout << "reallocate failed\n";
        std::cout << "+++++++++++++++++++++++++\n";
#endif  // __UNIT_TEST_VECTOR_BRIEF__ 
    }
};  // endof class DefaultAllocator
template <typename T, typename Alloc>
volatile int mfwu::DefaultAllocator<T, Alloc>::GLOBAL_COUNTER = __COUNTER_START;

// real alloc, don't keep construct / destroy here
// just allocate and deallocate the memory

// 1st implementation: use malloc and free
class malloc_alloc {
public:
    friend class unit_test_allocator;
    // QA: why we can declare friendship 
    // without forward declaring the class
    // ChatGPT answer: When all classes are in the same namespace and 
    // the namespace block is processed by the compiler in its entirety,
    // the compiler can resolve dependencies between classes without
    // the need for forward declarations within that namespace.
    
    static void* allocate(size_t n) {
        void* res = malloc(n);
        if (0 == res) res = oom_malloc(n);
        return res;
    }

    static void deallocate(void* p, size_t) {
        free(p);
    }

    static void* reallocate(void* p, size_t, size_t new_size) {
        void* res = realloc(p, new_size);
        if (0 == res) res = oom_realloc(p, new_size);
        return res;
    }

    static void (*set_malloc_handler(void (*f)())) () {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return old;
    }
private:
#ifdef __UNIT_TEST_ALLOCATOR__
    static std::string identify();  // test interface
#endif  // __UNIT_TEST_ALLOCATOR__
    static void* oom_malloc(size_t n) {
        void (*my_malloc_handler)();
        void* res;
        for (;;) {
            my_malloc_handler = __malloc_alloc_oom_handler;
            if (0 == my_malloc_handler) { __THROW_BAD_ALLOC; return nullptr; }
            (*my_malloc_handler)();
            res = malloc(n);
            if (res) return res;
        }
    }

    static void* oom_realloc(void*, size_t) {  /*TODO*/ return nullptr; }

    static void (*__malloc_alloc_oom_handler)();
};
void (*malloc_alloc::__malloc_alloc_oom_handler)() = 0;

// similar funcions but focusing on smaller memory
// 2nd implementation: keep a memory pool
enum { __ALIGN = 8 };
enum { __MAX_BYTES = 128 };
enum { __NFREELISTS = __MAX_BYTES / __ALIGN };
enum { __DEFAULT_REFILLING_SIZE = 20 };

class default_alloc {
public:
    friend class unit_test_allocator;
    static void* allocate(size_t n) {
        obj* volatile* my_free_list;
        obj* res;
        if (n > (size_t)__MAX_BYTES) {
            return malloc_alloc::allocate(n);
        }
        my_free_list = free_lists + FREELIST_INDEX(n);
        res = *my_free_list;  // res = free_lists[index]
        if (res == 0) {
            void* r = refill(ROUND_UP(n));
            return r;
        }
        *my_free_list = res->free_list_link;  // free_lists[index] = free_list[index]->next
        // detach the head node of the free_list[index]
        return res;
    }
    static void deallocate(void* p, size_t n) {
        obj* q = (obj*)p;
        obj* volatile* my_free_list;
        if (n > (size_t)__MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
            return;
        }
        my_free_list = free_lists + FREELIST_INDEX(n);
        q->free_list_link = *my_free_list;  // q->next = head;
        *my_free_list = q;  // head = q;
    }
    static void* reallocate(void* p, size_t old_size, size_t new_size) {
        if (old_size <= __MAX_BYTES/* || new_size <= __MAX_BYTES*/) { return nullptr; }
        // in fact, only the mem allocated by malloc_alloc
        // should be realloc to a smaller size
        return malloc_alloc::reallocate(p, old_size, new_size);
    }

    static void* oom_realloc(void*, size_t) {  /*TODO*/ return nullptr; }

private:
#ifdef __UNIT_TEST_ALLOCATOR__  // test interface
    static std::string identify();  
    static std::string print_status();
    static void* print_status_alloc(size_t n);
    static void print_status_dealloc(void* p, size_t n);
#endif // __UNIT_TEST_ALLOCATOR__

    static size_t ROUND_UP(size_t bytes) {
        return (bytes + __ALIGN - 1) & ~(__ALIGN - 1);
    }

    union obj {
        union obj* free_list_link;
        char client_data[1];
    };

    static obj* volatile free_lists[__NFREELISTS];  // free_lists[index] is a pointer to the head node of a free list
    static size_t FREELIST_INDEX(size_t bytes) {
        return (bytes + __ALIGN - 1) / __ALIGN - 1;
    }

    static void* refill(size_t n) {  // we assume n has been ROUNDUPed
        int nobjs = __DEFAULT_REFILLING_SIZE;  // default refilling size
        char* chunk = chunk_alloc(n, nobjs);  // get mem from pool

        if (1 == nobjs) {
            return chunk;
        }
        obj* volatile* my_free_lists = free_lists + FREELIST_INDEX(n);
        obj* res = (obj*)chunk;  // return the first obj*
        obj* current_obj, * next_obj;
        *my_free_lists = next_obj = (obj*)(chunk + n);  // link the others on the free_lists[n]
        for (int i = 1; ; i++) {
            current_obj = next_obj;
            next_obj = (obj*)((char*)next_obj + n);
            if (nobjs - 1 == i) {
                current_obj->free_list_link = 0;
                break;
            } else {
                current_obj->free_list_link = next_obj;
            }
        }
        return res;
    }
    // get mem from pool
    static char* chunk_alloc(size_t size, int& nobjs) {
        char* res;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        if (bytes_left >= total_bytes) {
            res = start_free;
            start_free += total_bytes;
            return res;
        } else if (bytes_left >= size) {
            nobjs = bytes_left / size;
            total_bytes = size * nobjs;
            res = start_free;
            start_free += total_bytes;
            return res;
        } else {
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size / 16);  // 2 * needed + round(poolsize / 16)
            if (bytes_left > 0) {
                obj* volatile* my_free_list = free_lists + FREELIST_INDEX(bytes_left);  // why can we do that?
                ((obj*)start_free)->free_list_link = *my_free_list;  // bcz bytes_left % 8 === 0 !
                *my_free_list = (obj*)start_free;
            }
            start_free = (char*)malloc(bytes_to_get);
            if (0 == start_free) {
                obj* volatile* my_free_list;
                obj* p;
                for (int i = size; i <= __MAX_BYTES; i += __ALIGN) {  // get mem from free_list[>n]
                    my_free_list = free_lists + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if (0 != p) {
                        *my_free_list = p->free_list_link;
                        start_free = (char*)p;
                        end_free = start_free + i;
                        return chunk_alloc(size, nobjs);
                    }
                }
                end_free = 0;
                start_free = (char*)malloc_alloc::allocate(bytes_to_get);
            }
            // successful malloc
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return chunk_alloc(size, nobjs);
        }
    }

    static char* start_free;  // start &
    static char* end_free;    // end of the memory pool
    static size_t heap_size;
};

char* default_alloc::start_free = 0;

char* default_alloc::end_free = 0;

size_t default_alloc::heap_size = 0;

default_alloc::obj* volatile
    default_alloc::free_lists[__NFREELISTS] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


}  // endof namespace mfwu

#endif  // __ALLOCATOR_HPP__    

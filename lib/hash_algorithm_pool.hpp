#ifndef __HASH_ALGORITHM_POOL__
#define __HASH_ALGORITHM_POOL__

#include "common.hpp"

namespace mfwu {

template <typename T>
struct hash_functor {};  
// char
template <>
struct hash_functor<char> {
    size_t operator()(const char key) {
        return static_cast<size_t>(key);
    }
};
template <>
struct hash_functor<unsigned char> {
    size_t operator()(const unsigned char key) {
        return static_cast<size_t>(key);
    }
};
template <>
struct hash_functor<signed char> {
    size_t operator()(const signed char key) {
        return static_cast<size_t>(key);
    }
};
// short
template <>
struct hash_functor<short> {
    size_t operator()(const short key) {
        return static_cast<size_t>(key);
    }
};
template <>
struct hash_functor<unsigned short> {
    size_t operator()(const unsigned short key) {
        return static_cast<size_t>(key);
    }
};
// int
template <>
struct hash_functor<int> {
    size_t operator()(const int key) {
        return static_cast<size_t>(key);
    }
};
template <>
struct hash_functor<unsigned int> {
    size_t operator()(const unsigned int key) {
        return static_cast<size_t>(key);
    }
};
// long
template <>
struct hash_functor<long> {
    size_t operator()(const long key) {
        return static_cast<size_t>(key);
    }
};
template <>
struct hash_functor<long long> {
    size_t operator()(const long long key) {
        return static_cast<size_t>(key);
    }
};
template <>
struct hash_functor<unsigned long> {
    size_t operator()(const unsigned long key) {
        return static_cast<size_t>(key);
    }
};
// float
template <>
struct hash_functor<float> {
    size_t operator()(const float key) {
        return static_cast<size_t>(reinterpret_cast<const int&>(key));
    }
};
// double   emmm... 24.08.09
template <>
struct hash_functor<double> {
    size_t operator()(const double key) {
        return static_cast<size_t>(reinterpret_cast<const long long&>(key));
    }
};

// string  __stl_hash_string
inline size_t _hash_string(const char* s) {
    size_t h = 0;
    for ( ; *s; ++s) {
        h = 5 * h + *s;
    }
    return size_t(h);
}
template <>
struct hash_functor<char*> {
    size_t operator()(const char* s) const {
        return _hash_string(s);
    }
};
template <>
struct hash_functor<const char*> {
    size_t operator()(const char* s) const {
        return _hash_string(s);
    }
};
template <typename T>
struct hash_functor<T*> {
    size_t operator()(T* ptr) {
        return static_cast<size_t>(ptr);
    }
};


// usage: mfwu::hash_algorithm_pool<key_type>::hash(key) % size
template <typename Key, typename HashFunctor=mfwu::hash_functor<Key>>
class hash_algorithm_pool {
    using key_type = Key;
    using size_type = size_t;

    static size_type hash(const Key& key) {
        return hashfunc(key);
    }

private:
    static HashFunctor hashfunc;
};  // endof class hash_algorithm_pool<Key, HashFunctor>


}  // endof namespace mfwu

#endif  // __HASH_ALGORITHM_POOL__
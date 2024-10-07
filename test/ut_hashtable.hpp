#define __UNIT_TEST_HASHTABLE__

#include "hashtable.hpp"

namespace mfwu {

class unit_test_hashtable {
public:
    bool use_mfwu_hashtable();
    
    struct data {
        int val;
    };  // endof class data
    static size_t data_hash(const data& d) {
        return hash_(d.val);
    }
private:
    using hash_func = mfwu::hash_functor<int>;
    static hash_func hash_;
    void print_hashtable(hashtable<data, int, hash_func>) {
        
    }
};  // endof class unit_test_hashtable

bool unit_test_hashtable::use_mfwu_hashtable() {
    std::cout << "\n------- Test: use mfwu::hashtable -------\n";


    return 0;
}

}  // endof namespace mfwu


#define __UNIT_TEST_HASHTABLE__

#include "hashtable.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_hashtable {
public:
    bool use_mfwu_hashtable();
    
    struct data {
        int val;
        bool operator==(const data& d) const {
            return d.val == val;
        }
    };  // endof class data
    struct data_hash {
        size_t operator()(const data& d) const {
            return hash_(d.val);
        }
    };  // endof struct data_hash
    using hash_func = mfwu::hash_functor<int>;
private:
    static hash_func hash_;
    void print_hashtable(const hashtable<data, int, data_hash>& htbl) {
        mfwu::vector<mfwu::vector<int>> 
            cache(htbl.size_, mfwu::vector<int>(htbl.capacity_, -主));
        std::cout << "capacity: " << htbl.capacity_ << "\t"
                  << "size: " << htbl.size_ << "\n";
        for (size_t idx = 0; idx < htbl.capacity_; ++idx) {
            if (!htbl.buckets_[idx].empty()) {
                size_t depth = 0;
                auto cur = htbl.buckets_[idx].front();
                while (cur) {
                    cache[depth][idx] = cur->value;
                    ++depth; cur = cur->next;
                }
            }
        }
        mfwu::print_rect(cache);
    }
};  // endof class unit_test_hashtable

unit_test_hashtable::hash_func unit_test_hashtable::hash_ = {};
/*
    The error message you’re seeing, 
    undefined reference to 'mfwu::unit_test_hashtable::hash_',
    typically occurs during the linking phase of compilation. 
    This means that the compiler knows about the function or variable
    (it has been declared), but it cannot find the actual implementation
    (definition) during the linking process. If hash_ is a static functor, 
    you need to ensure that it is defined and initialized properly
    GPT ANS 24.10.10 02:18 a.m.
*/

std::ostream& operator<<(std::ostream& os,
                         const unit_test_hashtable::data& d) {
        os << d.val;
        return os;
}

bool unit_test_hashtable::use_mfwu_hashtable() {
    std::cout << "\n------- Test: use mfwu::hashtable -------\n";
    mfwu::hashtable<data, int, data_hash> htbl1(5);
    htbl1[data{1}] = 1;
    htbl1.insert(data{2}, 2);
    htbl1.insert(mfwu::make_pair<data, int>(data{3}, 3));
    htbl1[data{6}] = 6;
    htbl1[data{5}] = 5;
    htbl1[data{26}] = 11;
    print_hashtable(htbl1);
    // TODO: DEBUG REQ_MEM

    return 0;
}

}  // endof namespace mfwu


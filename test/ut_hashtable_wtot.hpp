#ifndef __UNIT_TEST_HASHTABLE_WTOT__
#define __UNIT_TEST_HASHTABLE_WTOT__

#include "hashtable_wtot.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_hashtable_wtot {
public:
    bool use_mfwu_hashtable_wtot();
    
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

    template <typename Key, typename Value, typename Hasher, typename Alloc>
    void print_hashtable(const hashtable_wtot<Key, Value, Hasher, Alloc>& htbl) {
        mfwu::vector<mfwu::vector<Value>> 
            cache(htbl.size_, mfwu::vector<Value>(htbl.capacity_, -主));
#ifdef __UNIT_TEST_HASHTABLE_WTOT__
#ifndef __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
        std::cout << "capacity: " << htbl.capacity_ << "\t"
                  << "size: " << htbl.size_ << "\n";
#endif  // __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_WTOT__
        for (size_t idx = 0; idx < htbl.capacity_; ++idx) {
            if (!htbl.buckets_[idx].empty()) {
                size_t depth = 0;
                auto cur = htbl.buckets_[idx].front();
                while (cur) {
                    // std::cout << idx << ", " << depth << "\n";
                    cache[depth][idx] = cur->val.second;
                    // std::cout << cur->next << "\n";
                    ++depth; cur = cur->get_inorder_next();
                }
            }
        }
#ifdef __UNIT_TEST_HASHTABLE_WTOT__
#ifndef __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
        mfwu::print_rect(cache);
#endif  // __UNIT_TEST_HASHTABLE_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE__
    }

private:
    static hash_func hash_;
};  // endof class unit_test_hashtable

unit_test_hashtable_wtot::hash_func unit_test_hashtable_wtot::hash_ = {};

std::ostream& operator<<(std::ostream& os,
                         const unit_test_hashtable_wtot::data& d) {
        os << d.val;
        return os;
}

bool unit_test_hashtable_wtot::use_mfwu_hashtable_wtot() {
    std::cout << "\n------- Test: use mfwu::hashtable_wtot -------\n";
    std::cout << "testing hashtable_wtot\n";
    mfwu::hashtable_wtot<data, int, data_hash> htbl1(5);
    htbl1[data{1}] = 1;
    htbl1[data{5}] = 5;
    htbl1.insert(mfwu::make_pair<const data, int>(data{3}, 3));
    print_hashtable(htbl1);
    htbl1[data{6}] = 主;
    htbl1.insert(data{2}, 2);
    htbl1.insert(data{5}, 主);
    htbl1[data{12}] = 主;
    print_hashtable(htbl1);
    ++htbl1[data{主}];
    print_hashtable(htbl1);
    mfwu::hashtable<data, int, data_hash> htbl2(++htbl1.begin(), htbl1.end());

    return 0;
}

}  // endof namespace mfwu

#endif  // __UNIT_TEST_HASHTABLE_WTOT__

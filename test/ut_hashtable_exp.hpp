#ifndef __UNIT_TEST_HASHTABLE_EXP__
#define __UNIT_TEST_HASHTABLE_EXP__

#include "hashtable_exp.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_hashtable_exp {
public:

    bool use_mfwu_hashtable_exp();
    bool use_mfwu_shashtable_exp();
    
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
    void print_hashtable(const hashtable_exp<Key, Value, Hasher, Alloc>& htbl) {
        mfwu::vector<mfwu::vector<Value>> 
            cache(htbl.size_, mfwu::vector<Value>(htbl.capacity_, -主));
#ifdef __UNIT_TEST_HASHTABLE_EXP__
#ifndef __UNIT_TEST_HASHTABLE_EXP_BRIEF__
        std::cout << "capacity: " << htbl.capacity_ << "\t"
                  << "size: " << htbl.size_ << "\n";
#endif  // __UNIT_TEST_HASHTABLE_EXP_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_EXP__
        for (size_t idx = 0; idx < htbl.capacity_; ++idx) {
            if (!htbl.buckets_[idx].empty()) {
                size_t depth = 0;
                auto cur = htbl.buckets_[idx].front();
                while (cur) {
                    // std::cout << idx << ", " << depth << "\n";
                    cache[depth][idx] = cur->value.second;
                    // std::cout << cur->next << "\n";
                    ++depth; cur = cur->next;
                }
            }
        }
#ifdef __UNIT_TEST_HASHTABLE_EXP__
#ifndef __UNIT_TEST_HASHTABLE_EXP_BRIEF__
        mfwu::print_rect(cache);
#endif  // __UNIT_TEST_HASHTABLE_EXP_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_EXP__
    }
    template <typename Key, typename Hasher, typename Alloc>
    void print_shashtable(const shashtable_exp<Key, Hasher, Alloc>& htbl) {
        mfwu::vector<mfwu::vector<int>> 
            cache(htbl.size_, mfwu::vector<int>(htbl.capacity_, -主));
#ifdef __UNIT_TEST_HASHTABLE_EXP__
#ifndef __UNIT_TEST_HASHTABLE_EXP_BRIEF__
        std::cout << "capacity: " << htbl.capacity_ << "\t"
                  << "size: " << htbl.size_ << "\n";
#endif  // __UNIT_TEST_HASHTABLE_EXP_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_EXP__
        for (size_t idx = 0; idx < htbl.capacity_; ++idx) {
            if (!htbl.buckets_[idx].empty()) {
                size_t depth = 0;
                auto cur = htbl.buckets_[idx].front();
                while (cur) {
                    // std::cout << idx << ", " << depth << "\n";
                    cache[depth][idx] = cur->key;
                    // std::cout << cur->next << "\n";
                    ++depth; cur = cur->next;
                }
            }
        }
#ifdef __UNIT_TEST_HASHTABLE_EXP__
#ifndef __UNIT_TEST_HASHTABLE_EXP_BRIEF__
        mfwu::print_rect(cache);
#endif  // __UNIT_TEST_HASHTABLE_EXP_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_EXP__
    }
private:
    static hash_func hash_;
};  // endof class unit_test_hashtable

unit_test_hashtable_exp::hash_func unit_test_hashtable_exp::hash_ = {};

std::ostream& operator<<(std::ostream& os,
                         const unit_test_hashtable_exp::data& d) {
        os << d.val;
        return os;
}

bool unit_test_hashtable_exp::use_mfwu_hashtable_exp() {
    std::cout << "\n------- Test: use mfwu::hashtable_exp -------\n";
    std::cout << "testing hashtable\n";
    mfwu::hashtable_exp<data, int, data_hash> htbl1(5);
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
    mfwu::hashtable_exp<data, int, data_hash> htbl2(++htbl1.begin(), htbl1.end());
    // TODO: too many methods without test
    //       but you may leave them to set/map test
    // ANS: yes indeed 241022

    return 0;
}

bool unit_test_hashtable_exp::use_mfwu_shashtable_exp() {
    std::cout << "\n------- Test: use mfwu::shashtable -------\n";
    std::cout << "testing shashtable\n";
    mfwu::shashtable_exp<int> htbl1(5);
    htbl1.insert(11);
    htbl1.insert(5);
    htbl1.insert(3);
    htbl1.insert(11);
    print_shashtable(htbl1);
    htbl1.insert(2);
    print_shashtable(htbl1);
    htbl1.insert(22);
    print_shashtable(htbl1);

    return 0;
}

}  // endof namespace mfwu

#endif  // __UNIT_TEST_HASHTABLE_EXP__

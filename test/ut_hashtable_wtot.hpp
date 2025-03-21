#ifndef __UNIT_TEST_HASHTABLE_WTOT__
#define __UNIT_TEST_HASHTABLE_WTOT__

#include "hashtable_wtot.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_hashtable_wtot {
public:
    bool use_mfwu_hashtable_with_tree();
    bool use_mfwu_hashtable_with_htbl();
    
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

    template <typename Key, typename Value, typename Hasher, typename Compare, typename Alloc>
    void print_hashtable(const hashtable_with_tree<Key, Value, Hasher, Compare, Alloc>& htbl) {
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
#endif  // __UNIT_TEST_HASHTABLE_WTOTBRIEF__
#endif  // __UNIT_TEST_HASHTABLE_WTOT__
    }


    template <typename Key, typename Value, typename Hasher, typename Alloc>
    void print_hashtable(const hashtable_with_htbl<Key, Value, Hasher, Alloc>& htbl) {
        mfwu::vector<mfwu::vector<Value>> 
            cache(htbl.size_, mfwu::vector<Value>(htbl.capacity_, -主));
#ifdef __UNIT_TEST_HASHTABLE_WTOT__
#ifndef __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
        std::cout << "capacity: " << htbl.capacity_ << "\t"
                  << "size: " << htbl.size_ << "\n";
#endif  // __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_WTOT__
        for (size_t idx = 0; idx < htbl.capacity_; ++idx) {
            auto bkt = htbl.buckets_ + idx;
            if (!bkt->empty()) {
                size_t depth = 0;
                auto cur = bkt->front();
                // std::cout << "front pos: " << cur->val_ << "\n";
                size_t sz = bkt->size();
                if (cur == nullptr) continue;
                auto it = typename htbl_base<Key, Value>::iterator(cur, bkt);
                for (size_t i = 0; i < sz; i++) {
                    // std::cout << "iterator_cur pos: " << it.get_cur()->val_ << "\n";
                    cache[depth][idx] = it.get_cur()->val_->second;
                    ++depth; ++it;
                }
            }
        }
#ifdef __UNIT_TEST_HASHTABLE_WTOT__
#ifndef __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
        mfwu::print_rect(cache);
#endif  // __UNIT_TEST_HASHTABLE_WTOT_BRIEF__
#endif  // __UNIT_TEST_HASHTABLE_WTOT__
    }

private:
    struct data_compare {
        bool operator()(const data& a, const data& b) const {
            return a.val < b.val;
        }
    };

    static hash_func hash_;
};  // endof class unit_test_hashtable

unit_test_hashtable_wtot::hash_func unit_test_hashtable_wtot::hash_ = {};

std::ostream& operator<<(std::ostream& os,
                         const unit_test_hashtable_wtot::data& d) {
        os << d.val;
        return os;
}

bool unit_test_hashtable_wtot::use_mfwu_hashtable_with_tree() {
    std::cout << "\n------- Test: use mfwu::hashtable_with_tree -------\n";
    std::cout << "testing hashtable_with_tree\n";
    mfwu::hashtable_with_tree<data, int, data_hash, data_compare> htbl1(5);
    htbl1[data{5}] = 5;
    htbl1[data{1}] = 1;
    htbl1.insert(mfwu::make_pair<const data, int>(data{3}, 3));
    print_hashtable(htbl1);
    htbl1[data{6}] = 主;
    htbl1.insert(data{2}, 2);
    htbl1.insert(data{5}, 主);  // with underlying multimap
    htbl1[data{12}] = 主;
    print_hashtable(htbl1);
    ++htbl1[data{主}];
    print_hashtable(htbl1);
    htbl1.erase(data{5});
    htbl1.erase(data{4});
    htbl1.erase(data{3});
    htbl1.insert(data{34}, 🐻);
    htbl1.insert(data{23}, 🐻 + 主);
    print_hashtable(htbl1);
    mfwu::hashtable_with_tree<data, int, data_hash, data_compare> htbl2(++htbl1.begin(), htbl1.end());
    print_hashtable(htbl2);
    return 0;
}

bool unit_test_hashtable_wtot::use_mfwu_hashtable_with_htbl() {
    std::cout << "\n------- Test: use mfwu::hashtable_with_htbl -------\n";
    std::cout << "testing hashtable_with_htbl\n";
    mfwu::hashtable_with_htbl<data, int, data_hash> htbl1(5);
    htbl1[data{5}] = 5;
    htbl1[data{1}] = 1;
    htbl1.insert(mfwu::make_pair<const data, int>(data{3}, 3));
    print_hashtable(htbl1);
    htbl1[data{6}] = 主;
    htbl1.insert(data{2}, 2);
    htbl1.insert(data{5}, 主);  // change the val:5 of key:data{5}
    htbl1[data{12}] = 主 + 1;
    print_hashtable(htbl1);
    ++htbl1[data{主}];
    print_hashtable(htbl1);
    htbl1.erase(data{5});
    htbl1.erase(data{4});
    htbl1.erase(data{3});
    htbl1.insert(data{34}, 🐻);
    htbl1.insert(data{23}, 🐻 + 主);
    print_hashtable(htbl1);
    mfwu::hashtable_with_htbl<data, int, data_hash> htbl2(++htbl1.begin(), htbl1.end());
    print_hashtable(htbl2);
    for (int i = 0; i < 1000; i++) {
        int key = rand() % 60;
        int val = rand() % 10;
        // std::cout << "key_val: " << key << " : " << val << "\n";
        htbl2[data{key}] = val;
        // print_hashtable(htbl2);
    }
    print_hashtable(htbl2);
    // std::cout << "-----------------------------------------\n";
    mfwu::hashtable_with_htbl<data, int, data_hash> htbl3;
    htbl3[data{0}] = 1;
    // htbl3[data{60}] = 1;  // 炸了
    print_hashtable(htbl3);
    return 0;
}

}  // endof namespace mfwu

#endif  // __UNIT_TEST_HASHTABLE_WTOT__

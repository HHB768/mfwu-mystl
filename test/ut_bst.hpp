#define __UNIT_TEST_BST__

#include "binary_search_tree.hpp"

namespace mfwu {

class unit_test_bst {
public:
    bool use_mfwu_bst();

private:
    template <typename T, typename CmpFunctor>
    void print_basic_info(const mfwu::binary_search_tree<T, CmpFunctor>& bst) {
        std::cout << "size(): " << bst.size() 
                  << "height(): " << bst.height()
                  << "root(): " << bst.root() << "\n";
        std::cout << "maximum(): " << bst.maximum()
                  << "minimum(): " << bst.minimum() << "\n";
    }
    template <typename T, typename CmpFunctor>
    void print_detailed_info(
        const mfwu::binary_search_tree<T, CmpFunctor>& bst) {
        mfwu::vector<int> bst_seq = bst.sequentialize();
        mfwu::print_seq_tree(bst_seq);
    }
    template <typename T>
    void (*test_func(const T& val)) {
        std::cout << val << " ";
    }
};  // endof class unit_test_bst



bool unit_test_bst::use_mfwu_bst() {
    mfwu::binary_search_tree<int> bst1;
    print_detailed_info(bst1);
    mfwu::binary_search_tree<int> bst2 = {1, 2, 4, 1, 5, 6, 3, 1};
    print_detailed_info(bst2);
    mfwu::binary_search_tree<int> bst3 = bst1;
    print_detailed_info(bst3);
    mfwu::binary_search_tree<int> bst4 = bst2;
    print_detailed_info(bst4);
    mfwu::binary_search_tree<int> bst5 = {9, 4, 5, 7, 1, 6, 7, 3, 2, 9};
    print_detailed_info(bst5);
    mfwu::binary_search_tree<int> bst6 = mfwu::move(bst5);
    print_detailed_info(bst6);
    mfwu::binary_search_tree<int> bst7;
    bst7 = bst2;
    print_detailed_info(bst7);
    bst7 = mfwu::move(bst3);
    print_detailed_info(bst7);

    bst6.push(1);
    print_detailed_info(bst6);
    bst6.push(9);
    bst6.push(2);
    bst6.push(8);
    print_detailed_info(bst6);
    bst6.push(3);
    bst6.push(7);
    bst6.push(8);
    print_detailed_info(bst6);
    bst6.pop();
    print_detailed_info(bst6);
    bst6.pop(4);
    print_detailed_info(bst6);
    bst6.pop(4);
    print_detailed_info(bst6);
    bst6.pop(bst6.root_->left);
    print_detailed_info(bst6);

    bst7.push(1);
    print_detailed_info(bst7);
    bst7.push(9);
    bst7.push(2);
    bst7.push(8);
    print_detailed_info(bst7);
    bst7.push(3);
    bst7.push(7);
    bst7.push(8);
    print_detailed_info(bst7);
    bst7.pop();
    print_detailed_info(bst7);
    bst7.pop(4);
    print_detailed_info(bst7);
    bst7.pop(4);
    print_detailed_info(bst7);
    bst7.pop(bst7.root_->left);
    print_detailed_info(bst7);

    mfwu::binary_search_tree<int>::node* ans = bst7.search(3);
    std::cout << "ans->val: " << ans->val
              << "\nans->left->val: " << ans->left->val
              << "\nans->right->val: " << ans->right->val << "\n";

    bst7.pre_order(test_func);
    return 0;
}

}  // endof namespace mfwu
#define __UNIT_TEST_AVL_TREE__

#include "adelson_velskii_landis_tree.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {
class unit_test_avl_tree {
public:
    bool use_avl_tree();

private:
    template <typename T, typename CmpFunctor>
    void print_basic_info(
        const mfwu::avl_tree<T, CmpFunctor>& av) {
        std::cout << "size(): " << av.size() << " "
                  << "height(): " << av.height() << "\n";
    }
    template <typename T, typename CmpFunctor>
    void print_detailed_info(
        const mfwu::avl_tree<T, CmpFunctor>& av) {
        print_basic_info(av);
        mfwu::vector<int> av_seq = av.sequentialize();
        for (int i : av_seq) { std::cout << i << " "; }
        std::cout << "\n";
        mfwu::print_tree_struct(av.root_);
    }
};  // endof class unit_test_avl_tree

bool unit_test_avl_tree::use_avl_tree() {
    mfwu::avl_tree<int> av1;
    print_detailed_info(av1);
    mfwu::avl_tree<int> av2 = {1, 2, 3, 4, 5, 6, 7, 8, 9,
                               9, 8, 7, 6, 5, 4, 3, 2, 1,
                               1, 2, 3, 4, 5, 6, 7, 8, 9,
                               1, 2, 3};
    print_detailed_info(av2);
    // mfwu::avl_tree<int> av3 = av1;
    // print_detailed_info(av3);
    // mfwu::avl_tree<int> av4 = mfwu::move(av2);
    // print_detailed_info(av4);
    // mfwu::avl_tree<int> av5 = {3, 4, 1, 1, 1, 5, 5, 5, 9, 4};
    // print_detailed_info(av5);
    // mfwu::avl_tree<int> av6;
    // av6 = av5;
    // print_detailed_info(av6);
    // av6 = mfwu::move(av3);
    // print_detailed_info(av6);

    // av2.rotate_ll(av2.root_->left->right);
    av2.push(4);
    print_detailed_info(av2);

    return 0;
}
}  // endof namespace mfwu

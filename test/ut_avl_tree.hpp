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
                               2, 5, 1, 6, 9, 1, 2, 3, 5};
    print_detailed_info(av2);
    mfwu::avl_tree<int> av3 = av1;
    print_detailed_info(av3);
    mfwu::avl_tree<int> av4 = mfwu::move(av2);
    print_detailed_info(av4);
    mfwu::avl_tree<int> av5 = {3, 4, 1, 1, 1, 5, 5, 5, 9, 4};
    print_detailed_info(av5);
    mfwu::avl_tree<int> av6;
    av6 = av5;
    print_detailed_info(av6);
    av6 = mfwu::move(av3);
    print_detailed_info(av6);

    av5.push(4);
    av5.push(1);
    av5.push(0);
    av5.push(2);
    av5.push(9);
    print_detailed_info(av5);
    for (int i = 0; i < 20; i++) { av5.push(0); }
    print_detailed_info(av5);
    
    print_detailed_info(av4);
    av4.pop();
    print_detailed_info(av4);
    av4.pop(4);
    print_detailed_info(av4);
    av4.pop(4);
    print_detailed_info(av4);
    av4.pop(4);
    print_detailed_info(av4);
    mfwu::avl_tree<int>::node* cur = av4.search(6);
    av4.pop(cur);
    print_detailed_info(av4);

    return 0;
}
}  // endof namespace mfwu

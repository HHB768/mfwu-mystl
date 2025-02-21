#ifndef __UNIT_TEST_RBTREE_EXP__
#define __UNIT_TEST_RBTREE_EXP__

#include "ut_rbtree.hpp"
#include "rbtree_exp.hpp"

namespace mfwu {

class unit_test_rbtree_exp {
public:
    bool use_rbtree_exp();

    template <typename T, typename Compare>
    static void print_and_check(const rbtree_exp<T, Compare>& rbt) {
        unit_test_rbtree::print_colored_struct(rbt.get_tree());
        unit_test_rbtree::is_valid_rbtree(rbt.get_tree(), rbt.cmp);
    }

private:
    template <typename T, typename Compare>
    static bool equal_(const mfwu::rbtree<T, Compare>& rbt,
                      const mfwu::rbtree_exp<T, Compare>& rbte) {
        return equal_(rbt.get_tree(), rbte.get_tree());
    }
    template <typename Node1, typename Node2>
    static bool equal_(Node1* root1, Node2* root2) {
        if (root1 == nullptr && root2 == nullptr) {
            return true;
        }
        if (root1 == nullptr || root2 == nullptr) {
            return false;
        }
        return (root1->val == root2->val) 
               && (root1->color == root2->color)
               && equal_(root1->left, root2->left)
               && equal_(root1->right, root2->right);
    }
};  // endof class unit_test_rbtree_exp


bool unit_test_rbtree_exp::use_rbtree_exp() {
    std::cout << "\n------- Test: use mfwu::rbtree_exp -------\n";
    mfwu::rbtree<int> rbt = {1, 3, 5, 4, 2, 6, 9, 7, 8};
    mfwu::rbtree_exp<int> rbte = {1, 3, 5, 4, 2, 6, 9, 7, 8};

    srand(time(0));
    // srand(42);

    std::cout << "testing random push/pop\n";
    for (int test_num = 0; test_num < 20; test_num++) {
        auto rbt1 = rbt;
        auto rbt2 = rbte;
        for (int i = 0; i < 10; i++) {
            int num = rand() % 20;
            if (num >= 10) {
                num = rand() % 10;
                rbt1.erase(num);
                rbt2.erase(num);
            } else {
                rbt1.insert(num);
                rbt2.insert(num);
            }
            // unit_test_rbtree::print_colored_struct(rbt1.get_tree());
            // unit_test_rbtree::print_colored_struct(rbt2.get_tree());
        }
        if (equal_(rbt1, rbt2)) {
            // std::cout << "validated, test num: " << test_num + 1 << "\n";
            // unit_test_rbtree::print_colored_struct(rbt1.get_tree());
            // unit_test_rbtree::print_colored_struct(rbt2.get_tree());
        } else {
            std::cout << "validation failed\n";
            unit_test_rbtree::print_colored_struct(rbt1.get_tree());
            unit_test_rbtree::print_colored_struct(rbt2.get_tree());
            return 1;
        }
    }
    std::cout << "insane random push/pop test!\n";
    int iter_num = 1000;
    for (int test_num = 0; test_num < iter_num; test_num++) {
        auto rbt1 = rbt;
        auto rbt2 = rbte;
        for (int i = 0; i < 10; i++) {
            int num = rand() % 100;
            if (num >= 50) {
                // num = rand() % 10;
                rbt1.erase(num);
                rbt2.erase(num);
            } else {
                rbt1.insert(num);
                rbt2.insert(num);
            }
            // unit_test_rbtree::print_colored_struct(rbt1.get_tree());
            // unit_test_rbtree::print_colored_struct(rbt2.get_tree());
        }
        if (equal_(rbt1, rbt2)) {
            // std::cout << "validated, test num: " << test_num + 1 << "\n";
            // unit_test_rbtree::print_colored_struct(rbt1.get_tree());
            // unit_test_rbtree::print_colored_struct(rbt2.get_tree());
        } else {
            std::cout << "validation failed\n";
            unit_test_rbtree::print_colored_struct(rbt1.get_tree());
            unit_test_rbtree::print_colored_struct(rbt2.get_tree());
            return 1;
        }
    }
    std::cout << "validated, iter_num: " << iter_num << "\n";
    return 0;
}


}

#endif  // __UNIT_TEST_RBTREE_EXP__
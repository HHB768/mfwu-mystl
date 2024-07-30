#define __UNIT_TEST_BST__

#include "binary_search_tree.hpp"
#include "vector.hpp"

namespace mfwu {


template <typename Node>
mfwu::vector<std::string> tree2vec(Node* root) {
    using node = Node;
    mfwu::vector<std::string> ret;
    if (root == nullptr) return {};
    std::queue<node*> q;
    q.emplace(root);
    while (!q.empty()) {
        size_t size = q.size();
        bool flag = false;
        for (int i = 0; i < size; i++) {
            node* cur = q.front();
            if (cur == nullptr) {
                q.emplace(nullptr);
                q.emplace(nullptr);
                ret.emplace_back(" ");
            } else {
                q.emplace(cur->left);
                q.emplace(cur->right);
                if (cur->left || cur->right) {
                    flag = true;
                }
                ret.emplace_back(std::to_string(cur->val));
            }
            q.pop();
        }
        if (!flag) break;
    }
    return ret;
} 

template <typename Node>
void print_tree_struct(Node* root) {
    print_heap_struct(tree2vec(root));
}

class unit_test_bst {
public:
    bool use_mfwu_bst();

private:
    template <typename T, typename CmpFunctor>
    void print_basic_info(
        const mfwu::binary_search_tree<T, CmpFunctor>& bst) {
        std::cout << "size(): " << bst.size() << " "
                  << "height(): " << bst.height() << "\n";
                  //<< "root(): " << bst.root() << "\n";
        // std::cout << "maximum(): " << bst.maximum() << " "
        //           << "minimum(): " << bst.minimum() << "\n";
    }
    template <typename T, typename CmpFunctor>
    void print_detailed_info(
        const mfwu::binary_search_tree<T, CmpFunctor>& bst) {
        print_basic_info(bst);
        mfwu::vector<int> bst_seq = bst.sequentialize();
        for (int i : bst_seq) { std::cout << i << " "; }
        std::cout << "\n";
        mfwu::print_tree_struct(bst.root_);
    }
    // you cannot place test_func here
    // bcz 'this' obj will make this func ptr is of
    // void(unit_test_bst::*)(const int&) instead of
    // void(*) (const int&), understand?
    // endof class unit_test_bst
};  


template <typename T>
void test_func(const T& val) {
    std::cout << val << " ";
}

bool unit_test_bst::use_mfwu_bst() {
    std::cout << "\n------- Test: use mfwu bst -------\n";
    std::cout << "building binary search tree\n";
    mfwu::binary_search_tree<int> bst1;
    print_detailed_info(bst1);
    mfwu::binary_search_tree<int> bst2 = {1, 2, 4, 1, 5, 6, 3, 1};
    print_detailed_info(bst2);
    mfwu::binary_search_tree<int> bst3 = bst1;
    print_detailed_info(bst3);
    mfwu::binary_search_tree<int> bst4 = bst2;
    print_detailed_info(bst4);
    mfwu::binary_search_tree<int> bst5 = {8, 4, 5, 7, 1, 6, 7, 3, 2, 9};
    print_detailed_info(bst5);
    mfwu::binary_search_tree<int> bst6 = mfwu::move(bst5);
    print_detailed_info(bst6);
    mfwu::binary_search_tree<int> bst7;
    bst7 = bst2;
    print_detailed_info(bst7);
    bst7 = mfwu::move(bst3);
    print_detailed_info(bst7);

    std::cout << "random push/pop test: part 1\n";
    bst6.push(1);
    print_detailed_info(bst6);
    bst6.push(4);
    bst6.push(2);
    bst6.push(6);
    print_detailed_info(bst6);
    bst6.push(3);
    bst6.push(7);
    bst6.push(7);
    print_detailed_info(bst6);
    bst6.pop();
    print_detailed_info(bst6);
    bst6.pop(4);
    print_detailed_info(bst6);
    bst6.pop(4);
    print_detailed_info(bst6);
    bst6.pop(bst6.root_->left);
    print_detailed_info(bst6);

    std::cout << "random push/pop test: part 2\n";
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
    bst7.pop(2);
    print_detailed_info(bst7);
    bst7.pop(2);
    print_detailed_info(bst7);
    bst7.pop(bst7.root_->left);
    print_detailed_info(bst7);

    std::cout << "searching\n";
    mfwu::binary_search_tree<int>::node* ans = bst7.search(3);
    std::cout << "ans->val: " << ans->val
              << "\nans->left->val: " 
              << (ans->left ? std::to_string(ans->left->val) : "null")
              << "\nans->right->val: " 
              << (ans->right ? std::to_string(ans->right->val) : "null") << "\n";

    bst6.pre_order(&test_func<int>);
    std::cout << "\n";
    bst6.in_order(&test_func<int>);
    std::cout << "\n";
    bst6.post_order(&test_func<int>);
    std::cout << "\n";
    return 0;
}

}  // endof namespace mfwu
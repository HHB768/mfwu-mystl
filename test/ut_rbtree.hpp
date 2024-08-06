#define __UNIT_TEST_RBTREE__

#include "rbtree.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_rbtree {
public:
    bool use_rbtree_push();
    bool use_rbtree_pop();
    
private:
    template <typename T, typename CmpFunctor>
    void print_basic_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
        std::cout << "size() : " << rbt.size() << " "
                  << " height(): " << rbt.height() << "\n";
    }
    template <typename T, typename CmpFunctor>
    void print_color_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
        print_color_struct(rbt.root_);
    }
    template <typename T, typename CmpFunctor>
    void print_detailed_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
        print_basic_info(rbt);
        print_color_info(rbt);
        print_tree_struct(rbt.root_);
    }
    template <typename Node>
    mfwu::vector<std::string> tree2color(Node* root) {
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
                    ret.emplace_back(".");
                } else {
                    q.emplace(cur->left);
                    q.emplace(cur->right);
                    if (cur->left || cur->right) { flag = true; }
                    ret.emplace_back(cur->color ? "b" : "r");
                }
                q.pop();
            }
            if (!flag) break;
        }
        return ret;
    } 
    template <typename Node>
    void print_color_struct(Node* root) {
        print_heap_struct(tree2color(root));
    }

};  // endof class unit_test_ rbtree

bool unit_test_rbtree::use_rbtree_push() {
    mfwu::rbtree<int> rbt1;
    print_detailed_info(rbt1);
    mfwu::rbtree<int> rbt2 = {1, 2, 3, 4, 5, 6, 7, 8, 9,
                              9, 8, 7, 6, 5, 4, 3, 2, 1,
                              1, 2, 3, 4, 5, 6, 7, 8, 9,
                              0, 8, 1, 5, 0, 7, 3, 4, 7};
    print_detailed_info(rbt2);
    return 0;
}

bool unit_test_rbtree::use_rbtree_pop() {
    mfwu::rbtree<int> rbt1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print_detailed_info(rbt1);
    rbt1.pop();
    print_detailed_info(rbt1);
    rbt1.pop(2);
    print_detailed_info(rbt1);
    rbt1.pop(5);
    print_detailed_info(rbt1);
    rbt1.pop();
    print_detailed_info(rbt1);

    return 0;
}


}  // endof class mfwu
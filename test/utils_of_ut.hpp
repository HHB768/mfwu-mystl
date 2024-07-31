#ifndef __UTILS_OF_UT__
#define __UTILS_OF_UT__

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
                ret.emplace_back(".");
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

}  // endof namespace mfwu

#endif  // __UTILS_OF_UT__
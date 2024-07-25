#ifndef __AVL_TREE__
#define __AVL_TREE__

#include "common.hpp"

namespace mfwu {

template <typename T, typename CmpFunctor>
class avl_tree {
public:
    using value_type = T;
    using size_type = size_t;
    using node = tree_node<T>;

    avl_tree() : root_() {}
    avl_tree(const std::initializer_list<value_type>& vals) {
        for (const value_type& val : vals) {
            push(val);
        }
    }
    avl_tree(const avl_tree& av) {
        copy(av);
    }
    avl_tree(avl_tree&& av) : root_(av.root_) {
        av.root_ = nullptr;
    }
    ~avl_tree() {
        destroy_tree(root_);
    }

    push(const value_type& val) {
        
    }
    
private:
    void destroy_tree(node* root) {
        if (root == nullptr) return ;
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
    void rotate_ll(node* root) {
        node* parent = root->parent;
        node* left = root->left;
        node* leftright = root->left->right;

        left->parent = parent;
        if (parent) {
            if (parent->left == root) {
                parent->left = left;
            } else {
                parent->right = left;
            }
        }

        root->parent = left;
        left->right = root;

        root->left = leftright;
        if (leftright) {
            leftright->parent = root;
        }
    }
    void rotate_rr(node* root) {
        node* parent = root->parent;
        node* right = root->right;
        node* rightleft = root->right->left;

        right->parent = parent;
        if (parent) {
            if (parent->left == root) {
                parent->left = right;
            } else {
                parent->right = right;
            }
        }

        root->parent = right;
        right->left = root;

        root->right = rightleft;
        if (rightleft) {
            rightleft->parent = root;
        }
    }
    void rotate_lr(node* root) {
        rotate_ll(root->left);
        rotate_rr(root);
    }
    void rotate_rl(node* root) {
        rotate_rr(root->right);
        rotate_ll(root);
    }
    node* root_;
};  // endof class mfwu

}  // endof namespace mfwu


#endif  // __AVL_TREE__
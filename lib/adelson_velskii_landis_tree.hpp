#ifndef __AVL_TREE__
#define __AVL_TREE__

#include "common.hpp"

namespace mfwu {

template <typename T, typename CmpFunctor>
class avl_tree {
public:
    using value_type = T;
    using size_type = size_t;

    struct avl_node {
        value_type val;   
        size_type height;  // TODO: change heights when rotate
        avl_node* parent;
        avl_node* left;
        avl_node* right;
        avl_node() : val(), height(1), parent(nullptr),
                    left(nullptr), right(nullptr) {}
        avl_node(const value_type& v) : val(v), height(1), parent(nullptr),
                                    left(nullptr), right(nullptr) {}
        avl_node(value_type&& v) : val(mfwu::move(v)), parent(nullptr),
                                left(nullptr), right(nullptr) {}
        avl_node(const value_type& v, size_type h, node* p, node* l, node* r) 
            : val(v), height(h), parent(p), left(l), right(r) {}
        avl_node(value_type&& v, node* p, node* l, node* r)
            : val(mfwu::move(v)), height(h), parent(p), left(l), right(r) {}
    };  // endof struct avl_node
    using node = avl_node;
    
    struct ienb {
        node* newnode;
        bool lastside;
    };  // endof struct ienb

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

    void push(const value_type& val) {
        push(root_, val);
    }
    ienb push(node* root, const value_type& val) {
        ienb ie{nullptr, false};
        bool thisside = false;
        if (root == nullptr) {
            ie.newnode = new node(val, 1, nullptr, nullptr, nullptr);
            return ie;
        }
        if (val > root->val) {
            ie = push(root->right, val);
            if (ie.newnode) {
                ie.newnode->parent = root;
                root->right = ie.newnode;
            }
            thisside = true;
        } else {
            ie = push(root->left, val);
            if (ie.newnode) {
                ie.newnode->parent = root;
                root->left = ie.newnode;
            }
            thisside = false;
        }
        root->height++;
        if (root->right->height - root->left->height > 1
            || root->left->height - root->right->height > 1) {
            root->height--;
            if (thisside && ie.lastside) {
                root->right->height++;
                rotate_rr(root);
            } else if (thisside && !ie.lastside) {
                root->right->left->height += 2;
                rotate_rl(root);
            } else if (!thisside && ie.lastside) {
                root->left->right->height += 2;
                rotate_lr(root);
            } else {
                root->left->height++;
                rotate_ll(root);
            }
        } 
        return {nullptr, thisside};
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
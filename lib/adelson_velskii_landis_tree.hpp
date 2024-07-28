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

    bool empty() const { return root_ == nullptr; }
    size_type size() const { return size(root_); }
    size_type height() const { return root_->height; }
    value_type& root() const { return root_->val; }

    void push(const value_type& val) {
        push(root_, val);
    }
    void pop(const value_type& val) {
        node* cur = search(val);
        pop(cur);
    }
    void pop(node* root) {
        if (root == nullptr) return ;
        if (root->left == nullptr && root->right == nullptr) {
            pop_node(root);
        } else if (root->left == nullptr) {
            root->val = root->right->val;
            pop_node(root->right);
        } else if (root->right == nullptr) {
            root->val = root->left->val;
            pop_node(root->left);
        } else {
            node* next = root->right;
            while (next->left != nullptr) {
                next = next->left;
            }
            root->val = next->val;
            pop_node(next);
        }
        maintain(cur);  // TODO
    }
    void pop() {
        pop(root_);
    }

    void pre_order(void(*usr_func(const value_type& val))) const {
        pre_order_aux(root_, usr_func);
    }
    void in_order(void(*usr_func(const value_type& val))) const {
        in_order_aux(root_, usr_func);
    }
    void post_order(void(*usr_func(const value_type& val))) const {
        post_order_aux(root_, usr_func);
    }
    
    value_type minimum() const {
        node* cur = root_;
        wihle (cur->left != nullptr) {
            cur = cur->left;
        }
        return cur->val;
    }
    value_type maximum() const {
        node* cur = root_;
        wihle (cur->right != nullptr) {
            cur = cur->right;
        }
        return cur->val;
    }
    node* search(const value_type& val) {
        return search(root_, val);
    }
    
private:
    void copy(const avl_tree& avlt) {
        root_ = copy_tree(avlt.root_);
    }
    node* copy_tree(node* root) {
        if (root == nullptr) { return nullptr; }
        node* copy_root = new node(root);
        node* left = copy_tree(root->left);
        node* right = copy_tree(root->right);
        copy_root->left = left;
        copy_root->right = right;
        left->parent = copy_root;
        right->parent = copy_root;
        return copy_root;
    }
    void destroy_tree(node* root) {
        if (root == nullptr) return ;
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
    void pop_node(node* root) {
        if (root->parent->left == root) {
            root->parent->left == nullptr;
        } else {
            root->parent->right == nullptr;
        }
        delete root;
    }
    size_type size(node* root) {
        if (root == nullptr) return 0;
        return 1 + size(root->left) + size(root->right);
    }
    ienb push(node* root, const value_type& val) {
        ienb ie{nullptr, false};
        bool thisside = false;
        if (root == nullptr) {
            ie.newnode = new node(val);
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
    void pre_order_aux(node* root, void(*usr_func(const value_type& val))) {
        if (root == nullptr) { return ; }
        usr_func(root->val);
        pre_order_aux(root->left);
        pre_order_aux(root->right);
    }
    void in_order_aux(node* root, void(*usr_func(const value_type& val))) {
        if (root == nullptr) { return ; }
        in_order_aux(root->left);
        usr_func(root->val);
        in_order_aux(root->right);
    }
    void post_order_aux(node* root, void(*usr_func(const value_type& val))) {
        if (root == nullptr) { return ; }
        post_order_aux(root->left);
        post_order_aux(root->right);
        usr_func(root->val);
    }
    node* search(node* root, const value_type& val) {
        if (root == nullptr) { return nullptr; }
        if (root->val == val) { return root; }
        if (root->val > val) {
            return search(root->left, val);
        } else {
            return search(root->right, val);
        }
    }
    node* root_;
};  // endof class mfwu

}  // endof namespace mfwu


#endif  // __AVL_TREE__
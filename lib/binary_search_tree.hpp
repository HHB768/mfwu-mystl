#ifndef __BINARY_SEARCH_TREE_HPP__
#define __BINARY_SEARCH_TREE_HPP__

#include "common.hpp"

namespace mfwu {

template <typename U>
struct bst_node {
    using value_type = U;
    value_type val;    
    bst_node* parent;
    bst_node* left;
    bst_node* right;
    bst_node() : val(), parent(nullptr),
                left(nullptr), right(nullptr) {}
    bst_node(const value_type& v) : val(v), parent(nullptr),
                                left(nullptr), right(nullptr) {}
    bst_node(value_type&& v) : val(mfwu::move(v)), parent(nullptr),
                            left(nullptr), right(nullptr) {}
    bst_node(const value_type& v, node* p, node* l, node* r) 
        : val(v), parent(p), left(l), right(r) {}
    bst_node(value_type&& v, node* p, node* l, node* r)
        : val(mfwu::move(v)), parent(p), left(l), right(r) {}
};  // endof struct node

// TODO: test
template <typename T, typename CmpFunctor>
class binary_search_tree {
public:
    using value_type = T;
    using size_type = size_t;
    using node = mfwu::bst_node<T>;
    
    binary_search_tree() : root_(nullptr) {}
    binary_search_tree(const std::initializer_list<value_type>& vals) {
        for (value_type& val : vals) {
            push(val);
        }
    }
    binary_search_tree(const binary_search_tree& bst) {
        copy(bst);
    } 
    binary_search_tree(binary_search_tree&& bst) 
        : root_(bst.root_) {
        bst.root_ = nullptr;
    }
    ~binary_search_tree() {
        this->destroy_tree(root_);
    }

    binary_search_tree& operator=(const binary_search_tree& bst) {
        this->copy(bst);
        return *this;
    }
    binary_search_tree& operator(binary_search_tree&& bst) {
        root_ = bst.root_;
        bst.root_ = nullptr;
    }

    bool empty() const { return root_ == nullptr; }
    size_type size() const { size(root_); }
    size_type height() const { height(root_); }

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
    }
    void pop() {
        pop(root_);
    }

    void pre_order(void(*usr_func(const value_type& val))) const {
        pre_order_aux(root, usr_func);
    }
    void in_order() const {

    }
    void post_order() const {

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
    void copy(const binary_search_tree& bst) {
        root_ = copy_tree(bst.root_);
    }
    node* copy_tree(node* root) {
        if (root == nullptr) return nullptr;
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
        if (!root) return ;
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
    node* push(node* root, const value_type& val) {
        if (root == nullptr) {
            root = new node(val);  // node def
            return root;
        }
        if (val >= root->val) {
            node* ret = push(root->right, val);
            if (ret) {
                root->right = ret;
                ret->parent = root;
            }
        } else {
            node* ret = push(root->left, val);
            if (ret) {
                root->left = ret;
                ret->parent = root;
            }
        }
        return nullptr;
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
};  // endof class binary_search_tree

}  // endof namespace mfwu

#endif  // __BINARY_SEARCH_TREE__
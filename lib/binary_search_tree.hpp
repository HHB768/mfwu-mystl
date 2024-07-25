#ifndef __BINARY_SEARCH_TREE_HPP__
#define __BINARY_SEARCH_TREE_HPP__

#include "common.hpp"

namespace mfwu {

// TODO: test
template <typename T, typename CmpFunctor>
class binary_search_tree {
public:
    using value_type = T;
    using size_type = size_t;

    template <typename U>
    struct tree_node {
        using value_type = U;
        value_type val;    
        tree_node* parent;
        tree_node* left;
        tree_node* right;
        tree_node() : val(), parent(nullptr),
                 left(nullptr), right(nullptr) {}
        tree_node(const value_type& v) : val(v), parent(nullptr),
                                    left(nullptr), right(nullptr) {}
        tree_node(value_type&& v) : val(mfwu::move(v)), parent(nullptr),
                               left(nullptr), right(nullptr) {}
        tree_node(const value_type& v, node* p, node* l, node* r) 
            : val(v), parent(p), left(l), right(r) {}
        tree_node(value_type&& v, node* p, node* l, node* r)
            : val(mfwu::move(v)), parent(p), left(l), right(r) {}
    };  // endof struct node
    using node = tree_node<T>;
    
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

    value_type& head() const { return root_->val; }

    void push(const value_type& val) {
        push(root_, val);
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

private:
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
            node* ret = push(root->left);
            if (ret) {
                root->left = ret;
                ret->parent = root;
            }
        }
        return nullptr;
    }

    node* root_;
};  // endof class binary_search_tree

}  // endof namespace mfwu

#endif  // __BINARY_SEARCH_TREE__
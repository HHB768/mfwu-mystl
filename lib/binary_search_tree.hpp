#ifndef __BINARY_SEARCH_TREE_HPP__
#define __BINARY_SEARCH_TREE_HPP__

#include "common.hpp"

namespace mfwu {

template <typename T, typename CmpFunctor>
class binary_search_tree {
public:
    using value_type = T;
    using size_type = size_t;

    struct node {
        value_type val;    
        node* parent;
        node* left;
        node* right;
    };  // endof struct node
    
    binary_search_tree() : head_(nullptr) {}
    binary_search_tree(const std::initializer_list<value_type>& vals) {
        for (value_type& val : vals) {
            push(val);
        }
    }
    binary_search_tree(const binary_search_tree& bst) {
        copy(bst);
    } 
    binary_search_tree(binary_search_tree&& bst) 
        : head_(bst.head_) {
        bst.head_ = nullptr;
    }
    ~binary_search_tree() {
        destroy_tree(head_);
    }

    binary_search_tree& operator=(const binary_search_tree& bst) {
        copy(bst);
        return *this;
    }
    binary_search_tree& operator(binary_search_tree&& bst) {
        head_ = bst.head_;
        bst.head_ = nullptr;
    }

    bool empty() const { return head_ == nullptr; }
    size_type size() const { size(head_); }

    void push(const value_type& val) {
        
    }



private:
    void destroy_tree(node* root) {
        if (!root) return ;
        destroy_tree(root->left);
        destroy_tree(root->right);
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
            push(root->left);
        }
        return nullptr;
    }
    node* head_;
};  // endof class binary_search_tree

}  // endof namespace mfwu

#endif  // __BINARY_SEARCH_TREE__
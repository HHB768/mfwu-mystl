#ifndef __BINARY_SEARCH_TREE_HPP__
#define __BINARY_SEARCH_TREE_HPP__

#include "common.hpp"
#include "vector.hpp"

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
    bst_node(const value_type& v, bst_node* p, bst_node* l, bst_node* r) 
        : val(v), parent(p), left(l), right(r) {}
    bst_node(value_type&& v, bst_node* p, bst_node* l, bst_node* r)
        : val(mfwu::move(v)), parent(p), left(l), right(r) {}
    bst_node(const bst_node& node) : val(node.val), parent(node.parent),
                                    left(node.left), right(node.right) {}
    bst_node(bst_node&& node) : val(mfwu::move(node.val)),
        parent(node.parent), left(node.left), right(node.right) {
        node.parent = node.left = node.right = nullptr;
    }
    bst_node& operator=(const bst_node& node) {
        val = node.val; parent = node.parent;
        left = node.left; right = node.right;
        return *this;
    }
    bst_node& operator=(bst_node&& node) {
        val = mfwu::move(node.val);
        parent = node.parent;
        left = node.left; right = node.right;
        node.parent = node.left = node.right = nullptr;
        return *this;
    }
};  // endof struct node

class unit_test_bst;
// TODO: test
template <typename T, typename CmpFunctor=mfwu::less<T>>
class binary_search_tree {
public:
    friend class mfwu::unit_test_bst;
    using value_type = T;
    using size_type = size_t;
    using node = mfwu::bst_node<T>;
    
    binary_search_tree() : root_(nullptr) {}
    binary_search_tree(const std::initializer_list<value_type>& vals)
        : root_(nullptr) {
        for (const value_type& val : vals) {
            push(val);
        }
    }
    binary_search_tree(const binary_search_tree& bst) {
        this->copy(bst);
    } 
    binary_search_tree(binary_search_tree&& bst) 
        : root_(bst.root_) {
        bst.root_ = nullptr;
    }
    ~binary_search_tree() {
        this->destroy_tree(root_);
    }

    binary_search_tree& operator=(const binary_search_tree& bst) {
        this->destroy_tree(root_);  // TODO
        this->copy(bst);
        return *this;
    }
    binary_search_tree& operator=(binary_search_tree&& bst) {
        root_ = bst.root_;
        bst.root_ = nullptr;
        return *this;
    }

    bool empty() const { return root_ == nullptr; }
    size_type size() const { return size(root_); }
    size_type height() const { return height(root_); }

    value_type& root() const { return root_->val; }

    void push(const value_type& val) {
        if (root_ == nullptr) {
            root_ = new node(val);
            return ;
        }
        push(root_, val);
    }
    void pop(const value_type& val) {
        node* cur = search(val);
        pop(cur);
    }
    void pop(node* root) {
        if (root == nullptr) return ;
        if (root->left == nullptr && root->right == nullptr) {
            pop_node(root, nullptr);
        } else if (root->left == nullptr) {
            // std::cout << root->right->val << "\n";
            pop_node(root, root->right);
            // std::cout << "?\n";
        } else if (root->right == nullptr) {
            pop_node(root, root->left);
        } else {
            node* next = root->right;
            while (next->left != nullptr) {
                next = next->left;
            }
            root->val = next->val;
            pop(next);
        }
    }
    void pop() {
        pop(root_);
    }

    void pre_order(void(*usr_func)(const value_type& val)) {
        pre_order_aux(root_, usr_func);
    }
    void in_order(void(*usr_func)(const value_type& val)) {
        in_order_aux(root_, usr_func);
    }
    void post_order(void(*usr_func)(const value_type& val)) {
        post_order_aux(root_, usr_func);
    }
    mfwu::vector<value_type> sequentialize() const  {
        mfwu::vector<value_type> res;
        std::queue<node*> q;  // TODO: mfwu::queue
        if (root_ == nullptr) return res;
        q.emplace(root_);
        while (!q.empty()) {
            size_type size = q.size();
            for (int i = 0; i < size; i++) {
                node* cur = q.front();
                res.emplace_back(cur->val);
                if (cur->left != nullptr) {
                    q.emplace(cur->left);
                }
                if (cur->right != nullptr) {
                    q.emplace(cur->right);
                }
                q.pop();
            }
        }
        return res;
    }

    value_type minimum() const {
        if (root_ == nullptr) return {};
        node* cur = root_;
        while (cur->left != nullptr) {
            cur = cur->left;
        }
        return cur->val;
    }
    value_type maximum() const {
        if (root_ == nullptr) return {};
        node* cur = root_;
        while (cur->right != nullptr) {
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
        if (root == nullptr) { return nullptr; }
        node* copy_root = new node(*root);
        node* left = copy_tree(root->left);
        node* right = copy_tree(root->right);
        copy_root->left = left;
        copy_root->right = right;
        if (left)  left->parent = copy_root;
        if (right) right->parent = copy_root;
        return copy_root;
    }
    void destroy_tree(node* root) {
        if (!root) return ;
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
    void pop_node(node* root, node* next) {
        if (root->parent == nullptr) {
            root_ = next;
            root_->parent = nullptr;
        } else if (root->parent->left == root) {
            root->parent->left = next;
        } else {
            root->parent->right = next;
        }
        delete root;
    }
    size_type size(node* root) const {
        if (root == nullptr) return 0;
        return 1 + size(root->left) + size(root->right);
    }
    size_type height(node* root) const {
        if (root == nullptr) { return 0; }
        return max(height(root->left), height(root->right)) + 1;
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
    void pre_order_aux(node* root, void(*usr_func)(const value_type& val)) {
        if (root == nullptr) { return ; }
        usr_func(root->val);
        pre_order_aux(root->left, usr_func);
        pre_order_aux(root->right, usr_func);
    }
    void in_order_aux(node* root, void(*usr_func)(const value_type& val)) {
        if (root == nullptr) { return ; }
        in_order_aux(root->left, usr_func);
        usr_func(root->val);
        in_order_aux(root->right, usr_func);
    }
    void post_order_aux(node* root, void(*usr_func)(const value_type& val)) {
        if (root == nullptr) { return ; }
        post_order_aux(root->left, usr_func);
        post_order_aux(root->right, usr_func);
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
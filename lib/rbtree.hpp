#ifndef __RBTREE_HPP__
#define __RBTREE_HPP__

#include "common.hpp"
#include "algorithm.hpp"
#include "adelson_velskii_landis_tree.hpp"

namespace mfwu {

class unit_test_rbtree;

template <typename T, typename CmpFunctor=mfwu::less<T>>
class rbtree {
public:
    friend class mfwu::unit_test_rbtree;
    using value_type = T;
    using size_type = size_t;

    struct rb_node {
        value_type val;
        size_type height;
        bool isblack;
        node* left;
        node* right;
        node* parent;
        rb_node() : val(), height(1), isblack(false), 
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v) : val(v), height(1), isblack(false),
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v, size_type h, bool ib,
                node* l, node* r, node* p)
            : val(v), height(h), isblack(ib), left(l), right(r), parent(p) {}
        rb_node(const rb_node& node) 
            : val(node.val), height(node.height), isblack(node.isblack), 
              left(node.left), right(node.right), parent(node.parent) {}
        rb_node(rb_node&& node) : val(mfwu::move(node.val)), 
            height(node.height), isblack(node.isblack), 
            left(node.left), right(node.right), parent(node.parent) {}
    };  // endof struct rb_node
    using node = rb_node;

    rbtree() : root_(nullptr) {}
    rbtree(const std::initializer_list<value_type>& vals) : root_(nullptr) {
        for (auto&& val : vals) { push(val); }
    }
    rbtree(const rbtree& rbt) {
        this->copy(rbt);
    }
    rbtree(rbtree&& rbt) : root_(rbt.root_) {
        rbt.root_ = nullptr;
    }
    ~rbtree() {
        this->destroy_tree(root_);
    }

    rbtree& operator=(const rbtree& rbt) {
        this->destroy_tree(root_);
        this->copy(rbt);
        return *this;
    }
    rbtree& operator=(rbtree&& rbt) {
        root_ = rbt.root_;
        rbt.root_ = nullptr;
        return *this;
    }

    bool empty() const { return root_ == nullptr; }
    size_type size() const { return size(root_); }
    size_type height() const { return height(root_); }
    value_type& root() const { return root_->val; }
    
    void push(const value_type& val) {
        if (root_ == nullptr) {
            rootz_ = new node(val);
            return ;
        }
        push(root_, val);
    }
    void pop() {
        pop(root_);
    }
    void pop(const value_type& val) {
        node* cur = search(val);
        pop(cur);
    }
    void pop(node* root) {
        if (root == nullptr) return ;
        // TODO
    }
    
    void pre_order(void(*usr_func)(node*)) {
        pre_order_aux(root_, usr_func);
    }
    void in_order(void(*usr_func)(node*)) {
        in_order_aux(root_, usr_func);
    }
    void post_order(void(*usr_func)(node*)) {
        post_order_aux(root_, usr_func);
    }
    mfwu::vector<value_type> sequentialize() const {
        mfwu::vector<value_type> res;
        std::queue<node*> q;  // TODO
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
    void copy(const rbtree& rbt) {
        root_ = copy_tree(rbt.root_);
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
    node* push(node* root, const value_type& val) {
        // TODO
    }
    void llr(node* root) {

    }
    void rrr(node* root) {

    }
    void lrr(node* root) {

    }
    void rlr(node* root) {

    }
    void llb(node* root) {

    }
    void rrb(node* root) {

    }
    void lrb(node* root) {

    }
    void rlb(node* root) {
        
    }

    node* 

};  // endof class rbtree

}  // endof namespace mfwu

#endif  // __RBTREE_HPP__
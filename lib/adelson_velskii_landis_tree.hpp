#ifndef __AVL_TREE__
#define __AVL_TREE__

#include "common.hpp"

namespace mfwu {

class unit_test_avl_tree;

template <typename T, typename CmpFunctor=mfwu::less<T>>
class avl_tree {
public:
    friend class mfwu::unit_test_avl_tree;
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
        avl_node(const value_type& v, size_type h, avl_node* p, avl_node* l, avl_node* r) 
            : val(v), height(h), parent(p), left(l), right(r) {}
        avl_node(value_type&& v, size_type h, avl_node* p, avl_node* l, avl_node* r)
            : val(mfwu::move(v)), height(h), parent(p), left(l), right(r) {}
        avl_node(const avl_node& node) : val(node.val), height(node.height),
            parent(node.parent), left(node.left), right(node.right) {}
        avl_node(avl_node&& node) : val(mfwu::move(node.val)), height(node.val),
            parent(node.parent), left(node.left), right(node.right) {}
        avl_node& operator=(const avl_node& node) {
            val = node.val; height = node.height;
            parent = node.parent; left = node.left; right = node.right;
            return *this;
        }
        avl_node& operator=(avl_node&& node) {
            val = mfwu::move(node.val), height = node.height;
            parent = node.parent; left = node.left; right = node.right;
            node.parent = node.left = node.right = nullptr;
        }
        
    };  // endof struct avl_node
    using node = avl_node;
    
    // struct ienb {
    //     node* newnode;
    //     bool lastside;
    // };  // endof struct ienb

    avl_tree() : root_(nullptr) {}
    avl_tree(const std::initializer_list<value_type>& vals) 
        : root_(nullptr) {
        for (const value_type& val : vals) {
            push(val);
        }
    }
    avl_tree(const avl_tree& av) {
        this->copy(av);
    }
    avl_tree(avl_tree&& av) : root_(av.root_) {
        av.root_ = nullptr;
    }
    ~avl_tree() {
        this->destroy_tree(root_);
    }

    avl_tree& operator=(const avl_tree& av) {
        this->destroy_tree(root_);
        this->copy(av);
        return *this;
    }
    avl_tree& operator=(avl_tree&& av) {
        root_ = av.root_;
        av.root_ = nullptr;
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
    void pop() {
        pop(root_);
    }
    void pop(const value_type& val) {
        node* cur = search(val);
        pop(cur);
        // this approach is hard to maintain the height
        
        // 一开始觉得一边找一边记录height和llrr会快一点
        // 但是这样实现会将很多无用的东西上溯
        // TODO: 后面再试试吧
        
    }
    void pop(node* root) {
        if (root == nullptr) return ;
        node* parent = root->parent;
        if (root->left == nullptr && root->right == nullptr) {
            pop_node(root, nullptr);
        } else if (root->left == nullptr) {
            pop_node(root, root->right);
        } else if (root->right == nullptr) {
            pop_node(root, root->left);
        } else {
            node* next = root->right;
            while (next->left != nullptr) {
                next = next->left;
            }
            root->val = next->val;
            pop(next);
            return ;  // pop(next) has maintained_up
        }
        maintain_up(parent);
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
    void copy(const avl_tree& bst) {
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
    node* push(node* root, const value_type& val) {
        if (root == nullptr) {
            node* cur = new node(val);
            return cur;
        }
        node* parent = root->parent;
        if (val >= root->val) {
            node* ret = push(root->right, val);
            if (ret) {
                ret->parent = root;
                root->right = ret;
            }
            root->height = height(root);
            if ((ssize_t)height(root->right) - 
                (ssize_t)height(root->left) > 1) {
                maintain_r(root);
            }
        } else {
            node* ret = push(root->left, val);
            if (ret) {
                ret->parent = root;
                root->left = ret;
            }
            root->height = height(root);
            if ((ssize_t)height(root->left) - 
                (ssize_t)height(root->right) > 1) {
                maintain_l(root);
            }
        }
        return nullptr;
    }
    void maintain_l(node* root) {
        if (height(root->left->left) > 
            height(root->left->right)) {
            rotate_ll(root);
        } else {
            rotate_lr(root);
        }
    }
    void maintain_r(node* root) {
        if (height(root->right->left) >
            height(root->right->right)) {
            rotate_rl(root);
        } else {
            rotate_rr(root);
        }
    }
    void destroy_tree(node* root) {
        if (root == nullptr) return ;
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
    // ll and rotate_right
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
        } else {
            root_ = left;
        }

        root->parent = left;
        left->right = root;

        root->left = leftright;
        if (leftright) {
            leftright->parent = root;
        }
        root->height = height(root);
        left->height = height(left);
    }
    // rr and rotate_left
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
        } else {
            root_ = right;
        }

        root->parent = right;
        right->left = root;

        root->right = rightleft;
        if (rightleft) {
            rightleft->parent = root;
        }
        root->height = height(root);
        right->height = height(right);
    }
    void rotate_lr(node* root) {
        rotate_rr(root->left);
        rotate_ll(root);
    }
    void rotate_rl(node* root) {
        rotate_ll(root->right);
        rotate_rr(root);
    }

    void pop_node(node* root, node* next) {
        if (root == nullptr) return ;
        if (root->parent == nullptr) {
            root_ = next;
            root_->parent = nullptr;
        } else if (root->parent->left == root) {
            root->parent->left = next;
            if (next) next->parent = root->parent;
        } else {
            root->parent->right = next;
            if (next) next->parent = root->parent;
        }
        delete root;
    }
    void maintain_up(node* parent) {
        if (parent == nullptr) return ;
        while (parent) {
            parent->height = height(parent);
            if ((ssize_t)height(parent->left) - 
                (ssize_t)height(parent->right) > 1) {
                maintain_l(parent);
            } else if ((ssize_t)height(parent->right) - 
                       (ssize_t)height(parent->left) > 1) {
                maintain_r(parent);
            }
            parent = parent->parent;
        }
    }

    size_type size(node* root) const {
        if (root == nullptr) return 0;
        return 1 + size(root->left) + size(root->right);
    }
    size_type height(node* root) const {
        if (root == nullptr) { return 0; }
        size_type h = 0;
        if (root->left) h = max(h, root->left->height);
        if (root->right) h = max(h, root->right->height);
        return h + 1;
    }
    void pre_order_aux(node* root, void(*usr_func)(node*)) {
        if (root == nullptr) { return ; }
        usr_func(root);
        pre_order_aux(root->left, usr_func);
        pre_order_aux(root->right, usr_func);
    }
    void in_order_aux(node* root, void(*usr_func)(node*)) {
        if (root == nullptr) { return ; }
        in_order_aux(root->left, usr_func);
        usr_func(root);
        in_order_aux(root->right, usr_func);
    }
    void post_order_aux(node* root, void(*usr_func)(node*)) {
        if (root == nullptr) { return ; }
        post_order_aux(root->left, usr_func);
        post_order_aux(root->right, usr_func);
        usr_func(root);
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
};  // endof class avl_tree

}  // endof namespace mfwu


#endif  // __AVL_TREE__
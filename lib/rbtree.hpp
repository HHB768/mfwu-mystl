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
    
    // TODO: check
    constexpr static bool red = false;
    constexpr static bool black = true;

    struct rb_node {
        value_type val;
        bool color;
        rb_node* left;
        rb_node* right;
        rb_node* parent;
        // TODO: ready key - value pair in mfwu::map
        // not implement here, just cmp the key
        rb_node() : val(), color(red), 
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v) : val(v), color(red),
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v, bool ib) : val(v), color(ib),
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v, bool ib,
                rb_node* l, rb_node* r, rb_node* p)
            : val(v), color(ib), left(l), right(r), parent(p) {}
        rb_node(const rb_node& node) 
            : val(node.val), color(node.color), 
              left(node.left), right(node.right), parent(node.parent) {}
        rb_node(rb_node&& node) 
            : val(mfwu::move(node.val)), color(node.color), 
              left(node.left), right(node.right), parent(node.parent) {}
        // void set_color(bool c) {
        //     this->color = c;
        // }
        // void invert_color() {
        //     this->color = !this->color;
        // }
    };  // endof struct rb_node
    using node = rb_node;

    rbtree() : root_(nullptr) {}
    rbtree(const std::initializer_list<value_type>& vals) : root_(nullptr) {
        for (auto&& val : vals) { 
            std::cout << "push value: " << val << "\n";
            push(val); 
        }
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
            root_ = new node(val, black);  // set_root
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
        if (root->color == red) {
            if (root->left == nullptr && root->right == nullptr) {
                pop_node(root, nullptr);
            } else if (root->left == nullptr) {
                pop_node(root, root->right);
            } else if (root->right == nullptr) {
                pop_node(root, root->left);
            } else {  // has both children
                node* next = get_successor(root);
                root->val = next->val;
                pop(next);
            }
        } else {  // root is black 
            // TODO: can we conclude these into uxy?
            if (root->left == nullptr && root->right == nullptr) {
                pop_node(root, nullptr);
                // TODO: maintain
            } else if (root->left == nullptr) {
                // 必是右边单走一个红
                root->val = root->right->val;
                delete root->right;
                root->right = nullptr;
            } else if (root->right == nullptr) {
                root->val = root->left->val;
                delete root->left;
                root->left = nullptr;
            } else {

            }
        }
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
    node* search(const value_type& val) const {
        return search(root_, val);
    }

private:
    void copy(const rbtree& rbt) {
        root_ = copy_tree(rbt.root_);
    }
    static node* copy_tree(node* root) {
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
    static void destroy_tree(node* root) {
        if (root == nullptr) return ;
        destroy_tree(root->left);
        destroy_tree(root->right);
        delete root;
    }
    void set_root(node* root) {
        root_ = root;
        root_->parent = nullptr;
        root_->color = black;
    }
    node* push(node* root, const value_type& val) {
        if (root == nullptr) {
            node* cur = new node(val);
            return cur;
        }
        node* parent = root->parent;
        if (val >= root->val) {
            node* ret = push(root->right, val);
            if (ret) {  // xrx
                ret->parent = root;
                root->right = ret;
                std::cout << "ret color : " << ret->color << "    "
                          << "root color : " << root->color << "\n"; 
                if (root->color == red) {
                    // root->parent must exist
                    // bcz root is red then it is not root_
                    if (parent->left == root) {  // lrx
                        if (color(parent->right) == red) {
                            lrr(parent);
                        } else {  // black
                            lrb(parent);
                        }
                    } else {  // rrx
                        if (color(parent->left) == red) {
                            rrr(parent);
                        } else { // black
                            std::cout << "here\n";
                            rrb(parent);
                        }
                    }
                }
            }
        } else {  // xlx
            node* ret = push(root->left, val);
            if (ret) {
                ret->parent = root;
                root->left = ret;
                if (root->color == red) {
                    if (parent->left == root) {  // llx
                        if (color(parent->right) == red) {
                            llr(parent);
                        } else {  // black
                            llb(parent);
                        }
                    } else {
                        if (color(parent->left) == red) {
                            rlr(parent);
                        } else {  // black
                            rlb(parent);
                        }
                    }
                }
            }
        }
        return nullptr;
    }
    void llr(node* root) {
        // TODO: detect nullptr
        root->color = red;
        root->left->color = root->right->color = black;
        maintain(root);
    }
    void rrr(node* root) {
        llr(root);
    }
    void lrr(node* root) {
        llr(root);
    }
    void rlr(node* root) {
        llr(root);
    }
    void llb(node* root) {
        root->color = red;
        root->left->color = black;
        rotate_right(root);
    }
    void rrb(node* root) {
        root->color = red;
        root->right->color = black;
        rotate_left(root);
    }
    void lrb(node* root) {
        rotate_left(root->left);
        llb(root);
    }
    void rlb(node* root) {
        rotate_right(root->right);
        rrb(root);
    }
    void maintain(node* root) {
        if (root == nullptr) { 
            std::cout << "啊？\n"; 
        } else if (root->parent == nullptr) { 
            root->color = black;
            return ;
        } else if (root->parent->color == black) { 
            return ; 
        } else if (root->parent->parent == nullptr) {
            root->parent->color = black;
            return ;
        } else {  // red parent and black grandparent
            node* parent = root->parent;
            node* gparent = parent->parent;
            if (gparent->left == parent) {
                if (parent->left == root) {
                    if (color(gparent->right) == red) {
                        llr(gparent);
                    } else {  // black
                        llb(gparent);
                    }
                } else {  // lrx
                    if (color(gparent->left) == red) {
                        lrr(gparent);
                    } else {  // black
                        lrb(gparent);
                    }
                }
            } else {  // rxx
                if (parent->left == root) {
                    if (color(gparent->right) == red) {
                        rlr(gparent);
                    } else {  // black
                        rlb(gparent);
                    }
                } else {  // rrx
                    if (color(gparent->left) == red) {
                        rrr(gparent);
                    } else {  // black
                        rrb(gparent);
                    }
                }
            }
        }
    }
    
    void rotate_left(node* root) {
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
            set_root(right);
        }

        root->parent = right;
        right->left = root;

        root->right = rightleft;
        if (rightleft) {
            rightleft->parent = root;
        }
    }
    void rotate_right(node* root) {
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
            set_root(left);
        }

        root->parent = left;
        left->right = root;

        root->left = leftright;
        if (leftright) {
            leftright->parent = root;
        }
    }

    void pop_node(node* root, node* next) {
        if (root == nullptr) return ;
        if (root->parent == nullptr) {
            set_root(next);
        } else if (root->parent->left == root) {
            root->parent->left = next;
            if (next) { next->parent = root->parent; }
        } else {
            root->parent->right = next;
            if (next) { next->parent = root->parent; }
        }
        delete root;
    }
    static node* get_successor(node* root) {
        if (root == nullptr || root->right == nullptr) {
            return nullptr;
        }
        node* next = root->right;
        while (next->left != nullptr) {
            next = next->left;
        }
        return next;
    }
    void brr(node* root) {
        // impossible
    }
    void brb(node* root) {
        // 'r' must be a leaf
        // last 'b' must be nullptr
        node* next = get_successor(root);  // TODO: 上层找过了，别再找了，直接传进来
        root->val = next->val;
        pop_node(next, nullptr);  // or pop(next), whatever
    }
    void bbr(node* root) {
        // 'r' must be a leaf
        node* next = get_successor(root);
        node* nnext = get_successor(next);
        root->val = next->val;
        next->val = nnext->val;
        next->color = black;
        pop_node(nnext, nullptr);
    }
    void bbb(node* root) {
        // last 'b' must be nullptr
        node* next = get_successor(root);
        if (next == nullptr) {
            // root is leaf
            node* parent = root->parent;
            node* brother = nullptr;
            if (parent->left == root) {
                brother = parent->right;
                pop_node(root, nullptr);
                if (color(brother) == red) {
                    Lr();
                } else {
                    Lb();
                }
            } else {
                brother = parent->left;
                pop_node(root, nullptr);
                if (color(brother) == red) {
                    Rr();
                } else {
                    Rb();
                }
            }
        }
        root->val = next->val;
    }
    void Lr() {}
    void Lb() {}
    void Rr() {

    }
    void Rb(node* brother) {
        if (color(brother->left) == black && color(brother->right) == black) {
            Rb0();
        } else if (color(brother->left) == black && color(brother->right) == red) {
            Rb2();
        } else {
            Rb1();
        }
    }
    
    static bool color(node* root) {
        if (root == nullptr) return black;
        return root->color;
    }
    static size_type size(node* root) {
        if (root == nullptr) return 0;
        return 1  + size(root->left) + size(root->right);
    }
    static size_type height(node* root) {
        if (root == nullptr) { return 0; }
        return 1 + height(root->left) + height(root->right);
    }

    static void pre_order_aux(node* root, void(*usr_func)(node*)) {
        if (root == nullptr) { return ; }
        usr_func(root);
        pre_order_aux(root->left, usr_func);
        pre_order_aux(root->right, usr_func);
    }
    static void in_order_aux(node* root, void(*usr_func)(node*)) {
        if (root == nullptr) { return ; }
        in_order_aux(root->left, usr_func);
        usr_func(root);
        in_order_aux(root->right, usr_func);
    }
    static void post_order_aux(node* root, void(*usr_func)(node*)) {
        if (root == nullptr) { return ; }
        post_order_aux(root->left, usr_func);
        post_order_aux(root->right, usr_func);
        usr_func(root);
    }
    static node* search(node* root, const value_type& val) {
        if (root == nullptr) { return nullptr; }
        if (root->val == val) { return root; }
        if (root->val > val) {
            return search(root->left, val);
        } else {
            return search(root->right, val);
        }
    }

    node* root_;

};  // endof class rbtree

}  // endof namespace mfwu

#endif  // __RBTREE_HPP__
#ifndef __RBTREE_EXP_HPP__
#define __RBTREE_EXP_HPP__

#ifdef __GLOBAL_DEBUG__
#   define __RBTREE_EXP_DEBUG__
#else
#   ifdef __UNIT_TEST_RBTREE_BRIEF__
#       undef __RBTREE_EXP_DEBUG__
#   endif  // __UNIT_TEST_RBTREE_BRIEF__
#endif // __GLOBAL_DEBUG__

// debug rbtree
// #define __RBTREE_EXP_DEBUG__

#include "common.hpp"
#include "algorithm.hpp"
#include "adelson_velskii_landis_tree.hpp"

namespace mfwu {

template <typename T, typename CmpFunctor=mfwu::less<T>>
class rbtree_exp {
public:
    
    using value_type = T;
    using size_type = size_t;
    
    constexpr static bool red = false;
    constexpr static bool black = true;

    struct rb_node {
        const value_type val;  // can i? 12.3/24
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
        rb_node* get_inorder_next() const {
            rb_node* ret = nullptr;
            if (right) {
                ret = right;
                while (ret->left) { ret = ret->left; }
                return ret;
            } else if (parent) {
                const rb_node* ret = this;
                while (ret->parent) {
                    if (ret->parent->left == ret) {
                        return ret->parent;
                    }
                    ret = ret->parent;
                }
            }
            return nullptr;
        }
    };  // endof struct rb_node
    using node = rb_node;

    rbtree_exp() : root_(nullptr) {}
    rbtree_exp(const std::initializer_list<value_type>& vals) : root_(nullptr) {
        for (auto&& val : vals) { 
            insert(val); 
        }
    }
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    rbtree_exp(InputIterator first, InputIterator last) : root_(nullptr) {
        for ( ; first != last; ++first) {
            insert(*first);
        }
    }
    rbtree_exp(const rbtree_exp& rbt) {
        this->copy(rbt);
    }
    rbtree_exp(rbtree_exp&& rbt) : root_(rbt.root_) {
        rbt.root_ = nullptr;
    }
    ~rbtree_exp() {
        this->destroy_tree(root_);
    }

    rbtree_exp& operator=(const rbtree_exp& rbt) {
        this->destroy_tree(root_);
        this->copy(rbt);
        return *this;
    }
    rbtree_exp& operator=(rbtree_exp&& rbt) {
        root_ = rbt.root_;
        rbt.root_ = nullptr;
        return *this;
    }

    bool empty() const { return root_ == nullptr; }
    size_type size() const { return size(root_); }
    size_type height() const { return height(root_); }
    value_type& root() const { return root_->val; }
    node* get_tree() const { return root_; }
    
    node* insert(const value_type& val) {
        if (root_ == nullptr) {
            root_ = new node(val, black);  // set_root
            return root_;
        }
        return insert(root_, val);
    }
    node* erase() {
        if (root_ == nullptr) return nullptr;
        node* ret = root_->get_inorder_next();
        erase(root_);
        return ret;
    }
    node* erase(const value_type& val) {
        node* cur = search(val);
        if (cur == nullptr) return nullptr;
        node* ret = cur->get_inorder_next();  // IS IT RIGHT? 24.12.03
        erase(cur);
        return ret;
    }
    void erase(node* root) {
        if (root == nullptr) return ;
        if (root->color == red) {
#ifdef __RBTREE_EXP_DEBUG__
            std::cout << "erase red: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
            if (root->left == nullptr && root->right == nullptr) {
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "erase red leaf\n";
#endif  // __RBTREE_EXP_DEBUG__
                erase_node(root, nullptr);
            } else if (root->left == nullptr) {
                // erase_node(root, root->right);  // is it possible?
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "a?\n";
#endif  // __RBTREE_EXP_DEBUG__
            } else if (root->right == nullptr) {
                // erase_node(root, root->left);  // is it possible?
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "b?\n";
#endif  // __RBTREE_EXP_DEBUG__
            } else {  // has both children
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "erase red inner node\n";
#endif  // __RBTREE_EXP_DEBUG__
                node* next = get_successor(root);
                // root->val = next->val;  // HERE: 
                swap_pos(root, next);
                erase(root);  // remains the structure and erase successor
            }
        } else {  // root is black 
            // TODO: can we conclude these into uxy?
#ifdef __RBTREE_EXP_DEBUG__
            std::cout << "erase black: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
            if (root->left == nullptr && root->right == nullptr) {
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "erase black leaf\n";
#endif  // __RBTREE_EXP_DEBUG__
                bbb(root, nullptr, nullptr);
            } else if (root->left == nullptr) {
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "erase black node 1r\n";
#endif  // __RBTREE_EXP_DEBUG__
                // 必是右边单走一个红
                // root->val = root->right->val;  // HERE
                // = erase_node(root->right, nullptr)
                swap_pos(root, root->right);
                // delete root->right;
                // root->right = nullptr;
                root->parent->right = nullptr;
                delete root;
            } else if (root->right == nullptr) {
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "erase black node 1l\n";
#endif  // __RBTREE_EXP_DEBUG__
                // root->val = root->left->val;  // HERE
                swap_pos(root, root->left);
                // delete root->left;
                // root->left = nullptr;
                root->parent->left = nullptr;
                delete root;
            } else {
#ifdef __RBTREE_EXP_DEBUG__
                std::cout << "erase black node 2\n";
#endif  // __RBTREE_EXP_DEBUG__
                node* next = get_successor(root);
                node* nnext = get_successor(next);
                if (color(next) == black && color(nnext) == black) {
                    bbb(root, next, nnext);
                } else if (color(next) == red) {
                    brb(root, next, nnext);  // nnext must be nullptr
                } else {
                    bbr(root, next, nnext);
                }
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
                q.erase();
            }
        }
        return res;
    }

    value_type minimum() const {
        if (root_ == nullptr) return {};  // TODO: we dont need this
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

    void clear() {
        destroy_tree(root_);
        root_ = nullptr;
    }
    size_type count(const value_type& val) const {
        node* cur = search(val);
        size_type cnt = 0;
        while (cur && cur->val == val) {
            ++cnt;
            cur = cur->get_inorder_next();
        }
        return cnt;
    }

    node* lower_bound(const value_type& val) const {
        return lower_bound(root_, val);
    }

    node* upper_bound(const value_type& val) const {
        return upper_bound(root_, val);
    }

private:
    void copy(const rbtree_exp& rbt) {
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
        if (root_) {  // also found in 24.11.15 02:48
            root_->parent = nullptr;
            root_->color = black;
        }
    }
    // void copy_node(node* src, node* dst) {
    //     src->parent = dst->parent;
    //     src->left = dst->left;
    //     src->right = dst->right;
    //     if (dst->parent->left == dst) {
    //         dst->parent->left = src;
    //     } else {
    //         dst->parent->right = src;
    //     }
    //     dst->left->parent = src;
    //     dst->right->parent = src;
    // }
    static void connect(node* node1, node* node2, bool is_left) {
        if (node1 == nullptr && node2 == nullptr) {
            return ;
        }
        if (node1 == nullptr) {
            node2->parent = nullptr;
            return ;
        } else if (is_left) {
            node1->left = node2;
        } else {
            node1->right = node2;
        }
        if (node2) node2->parent = node1;
    }
    static void swap_prs(node* p, node* s) {
        node* pp = p->parent;
        node* pl = p->left;
        node* sl = s->left;
        node* sr = s->right;
        if (pp) {
            connect(pp, s, pp->left == p);
        } else {
            s->parent = nullptr;
        }
        connect(s, pl, true);
        connect(s, p, false);
        connect(p, sl, true);
        connect(p, sr, false);
    }
    static void swap_pls(node* p, node* s) {
        node* pp = p->parent;
        node* pr = p->right;
        node* sl = s->left;
        node* sr = s->right;
        if (pp) {
            connect(pp, s, pp->left == p);
        } else {
            s->parent = nullptr;
        }
        connect(s, pr, false);
        connect(s, p, true);
        connect(p, sl, true);
        connect(p, sr, false);
    }
    // note: to replace the src->val = dst->val
    //       we ensure src & dst are not null
    void swap_pos(node* src, node* dst) {
        // std::cout << src->val << " " << dst->val << "\n";
        if (src == root_) root_ = dst;  // CHECK
        mfwu::swap(src->color, dst->color);
        if (src->right == dst) {
            return swap_prs(src, dst);
        } else if (src->left == dst) {
            return swap_pls(src, dst);
        } else if (dst->right == src) {
            return swap_prs(dst, src);
        } else if (dst->left == src) {
            return swap_pls(dst, src);
        }
        // std::cout << "?\n";
        node* sp = src->parent;
        node* sl = src->left;
        node* sr = src->right;
        node* dp = dst->parent;
        node* dl = dst->left;
        node* dr = dst->right;

        if (sp) {
            connect(sp, dst, sp->left == src);
        } else {
            dst->parent = nullptr;
        }
        connect(dst, sl, true);
        connect(dst, sr, false);
        if (dp) {
            connect(dp, src, dp->left == dst);
        } else {
            src->parent = nullptr;
        }
        connect(src, dl, true);
        connect(src, dr, false);
        // return src;  // may be deleted later
    }
    node* insert(node* root, const value_type& val) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "insert: " << val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        if (root == nullptr) {
            node* cur = new node(val);
#ifdef __RBTREE_EXP_DEBUG__
            std::cout << "create new node: " << val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
            return cur;
        }
        node* parent = root->parent;
        if (!cmp(val, root->val)) {
            node* ret = insert(root->right, val);
            if (ret) {  // xrx
                ret->parent = root;
                root->right = ret;
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
                            rrb(parent);
                        }
                    }
                }
            }
        } else {  // xlx
            node* ret = insert(root->left, val);
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
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "llr: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        root->color = red;
        root->left->color = root->right->color = black;
        maintain(root);
    }
    void rrr(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "rrr: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        llr(root);
    }
    void lrr(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "lrr: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        llr(root);
    }
    void rlr(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "rlr: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        llr(root);
    }
    void llb(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "llb: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        root->color = red;
        root->left->color = black;
        rotate_right(root);
    }
    void rrb(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "rrb: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        root->color = red;
        root->right->color = black;
        rotate_left(root);
    }
    void lrb(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "lrb: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        rotate_left(root->left);
        llb(root);
    }
    void rlb(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "rlb: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        rotate_right(root->right);
        rrb(root);
    }
    void maintain(node* root) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "maintain: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        if (root == nullptr) { 
#ifdef __RBTREE_EXP_DEBUG__
            std::cout << "啊？\n"; 
#endif  // __RBTREE_EXP_DEBUG__
        } else if (root->parent == nullptr) { 
            root->color = black;  // ROOT
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
                    if (color(gparent->right) == red) {
                        lrr(gparent);
                    } else {  // black
                        lrb(gparent);
                    }
                }
            } else {  // rxx
                if (parent->left == root) {
                    if (color(gparent->left) == red) {
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

    void erase_node(node* root, node* next) {
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
    void brb(node* root, node* next, node* nnext) {
        // 'r' must be a leaf
        // last 'b' must be nullptr
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "brb: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        // root->val = next->val;  // HERE
        swap_pos(root, next);
        erase_node(root, nullptr);
    }
    void bbr(node* root, node* next, node* nnext) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "bbr: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        // 'r' must be a leaf
        // root->val = next->val;  // HERE
        // next->val = nnext->val;
        swap_pos(root, next);
        swap_pos(root, nnext);
        nnext->color = black;
        erase_node(root, nullptr);
    }
    void bbb(node* root, node* next, node* nnext) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "bbb: " << root->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        // last 'b' must be nullptr
        if (next == nullptr) {
            // root is leaf
            node* parent = root->parent;
            if (parent == nullptr) {
                // root == root_ ! fixed on 24.11.15 02:41
                erase_node(root, nullptr);
                return ;
            }
            node* brother = nullptr;
            if (parent->left == root) {
                brother = parent->right;
                erase_node(root, nullptr);
                if (color(brother) == red) {
                    Lr(brother);
                } else {
                    Lb(brother);
                }
            } else {
                brother = parent->left;
                erase_node(root, nullptr);
                if (color(brother) == red) {
                    Rr(brother);
                } else {
                    Rb(brother);
                }
            }
        } else {
            // root->val = next->val;  // HERE
            swap_pos(root, next);
            // next is leaf
            bbb(root, nullptr, nullptr);  // CHECK
            // may be right
            // bcz in bbb, root->val will always be useless
        }
        
    }
    void Lr(node* brother) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "Lr: " << brother->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        node* parent = brother->parent;
        brother->color = black;
        parent->color = red;
        rotate_left(parent);
        Lb(parent->right);
    }  // sym
    void Lb(node* brother) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "Lb: " << brother->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        node* parent = brother->parent;
        if (color(brother->left) == black && color(brother->right) == black) {
            // in this senario, brother should be a leaf
            // Lb0
            bool flag = parent->color == black;
            parent->color = black;
            brother->color = red;
            // maintain(brother(parent));
            if (flag) maintain_up(parent);
        } else if (color(brother->left) == red && color(brother->right) == black) {
            // Lb2
            brother->color = red;
            brother->left->color = black;
            rotate_right(brother);
            // Lb1
            parent->right->color = parent->color;
            parent->color = black;
            parent->right->right->color = black;
            rotate_left(parent);
        } else {  // color(brother->right) == red
            brother->color = parent->color;
            parent->color = black;
            brother->right->color = black;
            rotate_left(parent);
        }
    }  // sym
    void Rr(node* brother) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "Rr: " << brother->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        node* parent = brother->parent;  // TODO: root_
        brother->color = black;
        parent->color = red;
        rotate_right(parent);
        Rb(parent->left);  // !
    }
    void Rb(node* brother) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "Rb: " << brother->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        node* parent = brother->parent;  // TODO: root_
        if (color(brother->left) == black && color(brother->right) == black) {
            // Rb0
            bool flag = parent->color == black;
            parent->color = black;
            brother->color = red;
            // maintain(brother(parent));
            if (flag) maintain_up(parent);
        } else if (color(brother->left) == black && color(brother->right) == red) {
            // Rb2
            brother->color = red;
            brother->right->color = black;
            rotate_left(brother);
            // change into Rb1
            parent->left->color = parent->color;
            parent->color = black;
            parent->left->left->color = black;
            rotate_right(parent);
        } else {  // color(brother->left) == red
            // Rb1
            brother->color = parent->color;
            parent->color = black;
            brother->left->color = black;
            rotate_right(parent);
        }
    }
    void maintain_up(node* parent) {
#ifdef __RBTREE_EXP_DEBUG__
        std::cout << "maintain_up: " << parent->val << "\n";
#endif  // __RBTREE_EXP_DEBUG__
        node* gparent = parent->parent;
        node* brother = nullptr;
        if (gparent != nullptr) {
            if (gparent->left == parent) {
                brother = gparent->right;
                if (color(brother) == red) {
                    Lr(brother);
                } else {
                    Lb(brother);
                }
            } else {
                brother = gparent->left;
                if (color(brother) == red) {
                    Rr(brother);
                } else {
                    Rb(brother);
                }
            }
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
        return 1 + max(height(root->left), height(root->right));
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
        if (cmp(val, root->val)) {
            return search(root->left, val);
        } else {
            return search(root->right, val);
        }
    }

    static node* lower_bound(node* root, const value_type& val) {
        if (root == nullptr) {
            return nullptr;
        }
        node* ret = nullptr;
        if (cmp(val, root->val)) {  // TODO: CHECK if we can do this cmp
            ret = lower_bound(root->left, val);
            if (ret) { return ret; }
            return root;
        }
        if (root->val == val) { return root; }
        return lower_bound(root->right, val);
    }

    static node* upper_bound(node* root, const value_type& val) {
        if (root == nullptr) {
            return nullptr;
        }
        node* ret = nullptr;
        if (cmp(val, root->val)) {
            ret = upper_bound(root->left, val);
            if (ret) { return ret; }
            return root;
        }
        return upper_bound(root->right, val);
    }

    node* root_;
    constexpr static CmpFunctor cmp = {};  
    // constexpr  1204/24

};  // endof class rbtree_exp

// template <typename T, typename CmpFunctor>
// CmpFunctor rbtree_exp<T, CmpFunctor>::cmp = {};

}  // endof namespace mfwu

#endif  // __RBTREE_HPP__
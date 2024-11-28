#ifndef __DRBTREE_HPP__
#define __DRBTREE_HPP__

#ifdef __GLOBAL_DEBUG__
#   define __DRBTREE_DEBUG__
#else
#   ifdef __UNIT_TEST_DRBTREE_BRIEF__
#       undef __DRBTREE_DEBUG__
#   endif  // __UNIT_TEST_DRBTREE_BRIEF__
#endif // __GLOBAL_DEBUG__

// debug drbtree
// #define __DRBTREE_DEBUG__

#include "common.hpp"
#include "algorithm.hpp"
#include "adelson_velskii_landis_tree.hpp"
#include "rbtree.hpp"

namespace mfwu {

class unit_test_map;

template <typename Key, typename Value,
          typename Compare=mfwu::less<Key>>
class drbtree {
public:
    friend class unit_test_map;

    using key_type = Key;
    using mapped_type = Value;
    using value_type = mfwu::pair<Key, Value>;
    using size_type = size_t;

    using rbtree = mfwu::rbtree<Key, Compare>;

    constexpr static bool red = rbtree::red;
    constexpr static bool black = rbtree::black;

    struct rb_node {
        value_type val;
        bool color;
        rb_node* left;
        rb_node* right;
        rb_node* parent;

        rb_node() : val(), color(red),
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v) : val(v), color(red),
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v, bool ib) : val(v), color(ib),
            left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const value_type& v, bool ib, 
                rb_node* l, rb_node* r, rb_node* p)
            : val(v), color(ib), left(l), right(r), parent(p) {}
        rb_node(const key_type& k, const mapped_type& m) 
            : val(k, m), color(red),
              left(nullptr), right(nullptr), parent(nullptr) {}  // can i?
        rb_node(const key_type& k, const mapped_type& m, bool ib) 
            : val(k, m), color(ib), 
              left(nullptr), right(nullptr), parent(nullptr) {}
        rb_node(const key_type& k, const mapped_type& m, bool ib, 
                rb_node* l, rb_node* r, rb_node* p)
            : val(k, m), color(ib), left(l), right(r), parent(p) {}
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
                while (ret->parent){
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

    drbtree() : root_(nullptr) {}
    drbtree(const std::initializer_list<value_type>& vals)
        : root_(nullptr) {
        for (auto&& val : vals) {
            insert(val);
        }
    }
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value>
             >
    drbtree(InputIterator first, InputIterator last) : root_(nullptr) {
        for ( ; first != last; ++first) {
            insert(*first);
        }
    }
    drbtree(const drbtree& rbt) {
        this->copy(rbt);
    }
    drbtree(drbtree&& rbt) : root_(rbt.root_) {
        rbt.root_ = nullptr;
    }
    ~drbtree() {
        this->destroy_tree(root_);
    }

    drbtree& operator=(const drbtree& rbt) {
        this->destroy_tree(root_);
        this->copy(rbt);
        return *this;
    }
    drbtree& operator=(drbtree&& rbt) {
        root_ = rbt.root_;
        rbt.root_ = nullptr;
        return *this;
    }

    bool empty() const { return root_ == nullptr; }
    size_type size() const { return size(root_); }
    size_type height() const { return height(root_); }
    // value_type& root() const { return root_->val; }
    node* get_tree() const { return root_; }

    node* insert(const key_type& key, const mapped_type& mapped) {
        return insert(mfwu::make_pair<key_type, mapped_type>(key, mapped));
    }
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
    node* erase(const key_type& key) {
        node* cur = search(key);
        if (cur == nullptr) return nullptr;
        node* ret = cur->get_inorder_next();
        erase(cur);
        return ret;
    }
    node* erase(const value_type& val) {
        return erase(val.first);
    }
    void erase(node* root) {
        if (root == nullptr) return ;
        if (root->color == red) {
#ifdef __DRBTREE_DEBUG__
            std::cout << "erase red: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
            if (root->left == nullptr && root->right == nullptr) {
#ifdef __DRBTREE_DEBUG__
                std::cout << "erase red leaf\n";
#endif  // __DRBTREE_DEBUG__
                erase_node(root, nullptr);
            } else if (root->left == nullptr) {
                // erase_node(root, root->right);  // is it possible?
#ifdef __DRBTREE_DEBUG__
                std::cout << "a?\n";
#endif  // __DRBTREE_DEBUG__
            } else if (root->right == nullptr) {
                // erase_node(root, root->left);  // is it possible?
#ifdef __DRBTREE_DEBUG__
                std::cout << "b?\n";
#endif  // __DRBTREE_DEBUG__
            } else {  // has both children
#ifdef __DRBTREE_DEBUG__
                std::cout << "erase red inner node\n";
#endif  // __DRBTREE_DEBUG__
                node* next = get_successor(root);
                root->val = next->val;
                erase(next);  // remains the structure and erase successor
            }
        } else {  // root is black 
            // TODO: can we conclude these into uxy?
#ifdef __DRBTREE_DEBUG__
            std::cout << "erase black: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
            if (root->left == nullptr && root->right == nullptr) {
#ifdef __DRBTREE_DEBUG__
                std::cout << "erase black leaf\n";
#endif  // __DRBTREE_DEBUG__
                bbb(root, nullptr, nullptr);
            } else if (root->left == nullptr) {
#ifdef __DRBTREE_DEBUG__
                std::cout << "erase black node 1r\n";
#endif  // __DRBTREE_DEBUG__
                // 必是右边单走一个红
                root->val = root->right->val;
                // = erase_node(root->right, nullptr)
                delete root->right;
                root->right = nullptr;
            } else if (root->right == nullptr) {
#ifdef __DRBTREE_DEBUG__
                std::cout << "erase black node 1l\n";
#endif  // __DRBTREE_DEBUG__
                root->val = root->left->val;
                delete root->left;
                root->left = nullptr;
            } else {
#ifdef __DRBTREE_DEBUG__
                std::cout << "erase black node 2\n";
#endif  // __DRBTREE_DEBUG__
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
    node* search(const key_type& key) const {
        return search(root_, key);
    }
    node* search(const value_type& val) const {
        return search(root_, val.first);
    }

    void clear() {
        destroy_tree(root_);
        root_ = nullptr;
    }
    size_type count(const key_type& key) const {
        node* cur = search(key);
        size_type cnt = 0;
        while (cur && cur->val.first == key) {
            ++cnt;
            cur = cur->get_inorder_next();
        }
        return cnt;
    }
    size_type count(const value_type& val) const {
        return count(val.first);
    }

    node* lower_bound(const key_type& key) const {
        return lower_bound(root_, key);
    }

    node* upper_bound(const key_type& key) const {
        return upper_bound(root_, key);
    }

private:
    void copy(const drbtree& rbt) {
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
    node* insert(node* root, const value_type& val) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "insert: " << val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        if (root == nullptr) {
            node* cur = new node(val);
#ifdef __DRBTREE_DEBUG__
            std::cout << "create new node: " << val.first << "\n";
#endif  // __DRBTREE_DEBUG__
            return cur;
        }
        node* parent = root->parent;
        if (!cmp(val.first, root->val.first)) {
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
#ifdef __DRBTREE_DEBUG__
        std::cout << "llr: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        root->color = red;
        root->left->color = root->right->color = black;
        maintain(root);
    }
    void rrr(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "rrr: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        llr(root);
    }
    void lrr(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "lrr: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        llr(root);
    }
    void rlr(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "rlr: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        llr(root);
    }
    void llb(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "llb: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        root->color = red;
        root->left->color = black;
        rotate_right(root);
    }
    void rrb(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "rrb: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        root->color = red;
        root->right->color = black;
        rotate_left(root);
    }
    void lrb(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "lrb: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        rotate_left(root->left);
        llb(root);
    }
    void rlb(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "rlb: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        rotate_right(root->right);
        rrb(root);
    }
    void maintain(node* root) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "maintain: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        if (root == nullptr) { 
#ifdef __DRBTREE_DEBUG__
            std::cout << "啊？\n"; 
#endif  // __DRBTREE_DEBUG__
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
#ifdef __DRBTREE_DEBUG__
        std::cout << "brb: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        root->val = next->val;
        erase_node(next, nullptr);  // or erase(next), whatever
    }
    void bbr(node* root, node* next, node* nnext) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "bbr: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        // 'r' must be a leaf
        root->val = next->val;
        next->val = nnext->val;
        next->color = black;
        erase_node(nnext, nullptr);
        // or (TODO)
        // root->val = next->val;
        // erase_node(next, nnext);
        // nnext->color = black;
    }
    void bbb(node* root, node* next, node* nnext) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "bbb: " << root->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
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
            root->val = next->val;
            // next is leaf
            bbb(next, nullptr, nullptr);
        }
        
    }
    void Lr(node* brother) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "Lr: " << brother->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        node* parent = brother->parent;
        brother->color = black;
        parent->color = red;
        rotate_left(parent);
        Lb(parent->right);
    }  // sym
    void Lb(node* brother) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "Lb: " << brother->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
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
#ifdef __DRBTREE_DEBUG__
        std::cout << "Rr: " << brother->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
        node* parent = brother->parent;  // TODO: root_
        brother->color = black;
        parent->color = red;
        rotate_right(parent);
        Rb(parent->left);  // !
    }
    void Rb(node* brother) {
#ifdef __DRBTREE_DEBUG__
        std::cout << "Rb: " << brother->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
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
#ifdef __DRBTREE_DEBUG__
        std::cout << "maintain_up: " << parent->val.first << "\n";
#endif  // __DRBTREE_DEBUG__
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

    static node* search(node* root, const key_type& key) {
        if (root == nullptr) { return nullptr; }
        if (root->val.first == key) { return root; }
        if (cmp(key, root->val.first)) {
            return search(root->left, key);
        } else {
            return search(root->right, key);
        }
    }

    static node* lower_bound(node* root, const key_type& key) {
        if (root == nullptr) {
            return nullptr;
        }
        node* ret = nullptr;
        if (cmp(key, root->val.first)) {  // TODO: CHECK if we can do this cmp
            ret = lower_bound(root->left, key);
            if (ret) { return ret; }
            return root;
        }
        if (root->val.first == key) { return root; }
        return lower_bound(root->right, key);
    }

    static node* upper_bound(node* root, const key_type& key) {
        if (root == nullptr) {
            return nullptr;
        }
        node* ret = nullptr;
        if (cmp(key, root->val.first)) {
            ret = upper_bound(root->left, key);
            if (ret) { return ret; }
            return root;
        }
        return upper_bound(root->right, key);
    }  // TODO: check this

    node* root_;
    static Compare cmp;

};  // endof class drbtree

template <typename Key, typename Value, typename Compare>
Compare drbtree<Key, Value, Compare>::cmp = {};

}  // endof namespace mfwu

#endif  // __DRBTREE_HPP__
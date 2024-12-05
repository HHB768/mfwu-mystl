#ifndef __UNIT_TEST_RBTREE__
#define __UNIT_TEST_RBTREE__

#include "rbtree.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_rbtree {
public:
    bool use_rbtree_insert();
    bool use_rbtree_erase();
    bool use_cmp_functor();


    // TODO: make these static
    template <typename T, typename CmpFunctor>
    static void print_basic_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        std::cout << "size() : " << rbt.size() << " "
                  << " height(): " << rbt.height() << "\n";
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename T, typename CmpFunctor>
    static void print_color_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_color_struct(rbt.root_);
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename T, typename CmpFunctor>
    static void print_detailed_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_basic_info(rbt);
        // print_color_info(rbt);
        // print_tree_struct(rbt.root_);
        print_colored_struct(rbt.root_);
        is_valid_rbtree(rbt.root_, rbt.cmp);
        // std::cout << rbt.cmp(0, 1) << "\n";
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename Node>
    static mfwu::vector<std::string> tree2color(Node* root) {
        using node = Node;
        mfwu::vector<std::string> ret;
        if (root == nullptr) return {};
        std::queue<node*> q;
        q.emplace(root);
        while (!q.empty()) {
            size_t size = q.size();
            bool flag = false;
            for (int i = 0; i < size; i++) {
                node* cur = q.front();
                if (cur == nullptr) {
                    q.emplace(nullptr);
                    q.emplace(nullptr);
                    ret.emplace_back(".");
                } else {
                    q.emplace(cur->left);
                    q.emplace(cur->right);
                    if (cur->left || cur->right) { flag = true; }
                    ret.emplace_back(cur->color ? "b" : "r");
                }
                q.pop();
            }
            if (!flag) break;
        }
        return ret;
    } 
    template <typename Node>
    static void print_color_struct(Node* root) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_heap_struct(tree2color(root));
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename Node>
    static mfwu::vector<Node*> tree2node(Node* root) {
        using node = Node;
        mfwu::vector<node*> ret;
        if (root == nullptr) return {};
        std::queue<node*> q;
        q.emplace(root);
        while (!q.empty()) {
            size_t size = q.size();
            bool flag = false;
            for (int i = 0; i < size; i++) {
                node* cur = q.front();
                if (cur == nullptr) {
                    q.emplace(nullptr);
                    q.emplace(nullptr);
                } else {
                    q.emplace(cur->left);
                    q.emplace(cur->right);
                    if (cur->left || cur->right) { flag = true; }
                }
                ret.emplace_back(cur);
                q.pop();
            }
            if (!flag) break;
        }
        return ret;
    } 
    template <typename Node>
    static void print_colored_struct(Node* root) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_rbtree_heap_struct(tree2node(root));
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }

    template <typename Node, typename Compare>
    static bool is_valid_rbtree(Node* root, Compare cmp) {
        try {
            root->color;
            root->val;
            root->parent;
            root->left;
            root->right;
        } catch(...) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
            std::cout << "invalid rbtree\n";
#endif  // __UNIT_TEST_RBTREE_BRIEF__
            return false;
        }
        if (root == nullptr) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
            std::cout << "empty tree\n";
#endif  // __UNIT_TEST_RBTREE_BRIEF__
            return true; 
        }
        if (root->color == red) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
            std::cout << "invalid rbtree\n";
#endif  // __UNIT_TEST_RBTREE_BRIEF__
            return false; 
        }
        auto mm = is_valid_rbtree_aux(root, cmp);
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        if (mm.is_rbtree) {
            std::cout << "rbtree validated\n";
            std::cout << "info: \n";
            std::cout << "min: " << mm.min << "\n";
            std::cout << "max: " << mm.max << "\n";
            std::cout << "black cnt in each path: " << mm.black_cnt << "\n";
            return true;
        } else {
            std::cout << "invalid rbtree\n";
            return false;
        }
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    
private:
    template <typename value_type>
    struct minmax {
        bool mm_valid = false;
        bool is_rbtree = true;
        value_type min;
        value_type max;
        int black_cnt = 1;
    };  // endof class minmax
    template <typename Node, typename Compare>
    static minmax<typename std::remove_const<decltype(Node::val)>::type> 
    is_valid_rbtree_aux(Node* root, Compare cmp) {
        using return_type = minmax<typename std::remove_const<decltype(Node::val)>::type>;
        
        return_type mm = {};
        if (root == nullptr) { return mm; }
        
        if (root->color == red && 
            (color(root->left) == red || color(root->right) == red)) {
            mm.is_rbtree = false; return mm;
        }
        return_type mm_left = is_valid_rbtree_aux(root->left, cmp);
        return_type mm_right = is_valid_rbtree_aux(root->right, cmp);
        if (mm_left.is_rbtree == false || mm_right.is_rbtree == false
            || mm_left.black_cnt != mm_right.black_cnt) {
            mm.is_rbtree = false; return mm;
        }
        if (!mm_left.mm_valid) { mm_left.min = mm_left.max = root->val; }
        if (!mm_right.mm_valid) { mm_right.min = mm_right.max = root->val; }
        if (cmp(root->val, mm_left.max) || cmp(mm_right.min, root->val)
            || cmp(root->val, mm_left.min) || cmp(mm_right.max, root->val)) {
            std::cout << "root: " << root->val << "\n";
            std::cout << mm_left.min << ", " << mm_left.max << "\n";
            std::cout << mm_right.min << ", " << mm_right.max << "\n";
            std::cout << cmp(root->val, mm_left.max) << "\n";
            mm.is_rbtree = false; return mm;
        }
        mm.mm_valid = true;
        mm.is_rbtree = true;
        mm.min = cmp(mm_left.min, mm_right.max) ? mm_left.min : mm_right.min;
        mm.max = cmp(mm_left.min, mm_right.max) ? mm_right.max : mm_left.max;
        mm.black_cnt = mm_left.black_cnt + (root->color);
        return mm;
    }
    template <typename Node>
    static bool color(Node* root) {
        if (root == nullptr) { return 1; }
        return root->color;
    }

    constexpr static bool red = 0;
    constexpr static bool black = 1;

    mfwu::rbtree<int> rbt_;

};  // endof class unit_test_ rbtree

bool unit_test_rbtree::use_rbtree_insert() {
    std::cout << "\n------- Test: use mfwu::rbtree::insert -------\n";
    std::cout << "constructing rbtree\n";
    mfwu::rbtree<int> rbt1;
    print_detailed_info(rbt1);
    mfwu::rbtree<int> rbt2 = {1, 2, 3, 4, 5, 6, 7, 8, 9,
                              9, 8, 7, 6, 5, 4, 3, 2, 1,
                              1, 2, 3, 4, 5, 6, 7, 8, 9,
                              0, 8, 1, 5, 0, 7, 3, 4, 7};
    print_detailed_info(rbt2);
    mfwu::rbtree<int> rbt3 = rbt2;
    print_detailed_info(rbt3);
    mfwu::rbtree<int> rbt4 = mfwu::move(rbt2);
    print_detailed_info(rbt4);
    mfwu::rbtree<int> rbt5;
    rbt5 = rbt1 = rbt3;
    print_detailed_info(rbt5);
    rbt5 = mfwu::move(rbt1);
    print_detailed_info(rbt5);

    std::cout << "random inserting test: part 1\n";
    mfwu::rbtree<int> rbt6 = {1, 9, 1, 5, 6, 7, 0, 1, 4, 9, 8};
    print_detailed_info(rbt6);
    rbt6.insert(0);
    print_detailed_info(rbt6);
    rbt6.insert(6);
    print_detailed_info(rbt6);
    rbt6.insert(7);
    print_detailed_info(rbt6);
    rbt6.insert(2);
    print_detailed_info(rbt6);
    rbt6.insert(9);
    print_detailed_info(rbt6);

    std::cout << "random inserting test: part 2\n";
    mfwu::vector<int> temp = {2, 8, 8, 6, 1, 4, 7, 8, 3, 4, 1,
            8, 5, 9, 3, 5, 7, 6, 4, 0, 2, 6};
    rbt6 = mfwu::rbtree<int>{};
    for (int& i : temp) {
        rbt6.insert(i);
        print_detailed_info(rbt6);
    }
    print_detailed_info(rbt6);
    rbt6.insert(1);
    print_detailed_info(rbt6);
    rbt6.insert(5);
    print_detailed_info(rbt6);
    rbt6.insert(4);
    print_detailed_info(rbt6);
    rbt6.insert(8);
    print_detailed_info(rbt6);
    rbt6.insert(3);
    print_detailed_info(rbt6);

    rbt_ = rbt5;
    
    return 0;
}

bool unit_test_rbtree::use_rbtree_erase() {
    std::cout << "\n------- Test: use mfwu::rbtree::erase -------\n";
    std::cout << "random inserting/erasing test: part 1\n";
    mfwu::rbtree<int> rbt1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print_detailed_info(rbt1);
    rbt1.insert(6);
    rbt1.erase();
    print_detailed_info(rbt1);
    rbt1.insert(5);
    rbt1.erase(2);
    print_detailed_info(rbt1);
    rbt1.insert(9);
    rbt1.erase(5);
    print_detailed_info(rbt1);
    rbt1.insert(1);
    rbt1.erase();
    print_detailed_info(rbt1);
    rbt1.insert(4);
    rbt1.erase();
    rbt1.erase();
    rbt1.erase();
    rbt1.insert(6);
    rbt1.insert(7);
    rbt1.erase(7);
    rbt1.erase(7);
    for (int i = 0; i < 10; i++) { rbt1.erase(6); }
    print_detailed_info(rbt1);

    std::cout << "random inserting/eraseing test: part 2\n";
    print_detailed_info(rbt_);
    for (int i = 0; i < 12; i++) { rbt_.insert(6); }
    rbt_.erase(6);
    print_detailed_info(rbt_);
    rbt_.insert(4);
    rbt_.erase(4);
    print_detailed_info(rbt_);
    rbt_.insert(7);
    rbt_.erase(9);
    print_detailed_info(rbt_);
    rbt_.insert(0);
    rbt_.erase(1);
    print_detailed_info(rbt_);
    rbt_.insert(9);
    rbt_.erase();
    rbt_.erase(3);
    rbt_.erase();
    rbt_.insert(2);
    rbt_.insert(8);
    rbt_.erase(7);
    rbt_.erase(7);
    for (int i = 0; i < 20; i++) { rbt_.erase(6); }
    print_detailed_info(rbt_);

    return 0;
}

bool unit_test_rbtree::use_cmp_functor() {
    return 0;
}


}  // endof class mfwu

#endif  // __UNIT_TEST_RBTREE__
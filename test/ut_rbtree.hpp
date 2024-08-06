#define __UNIT_TEST_RBTREE__

#include "rbtree.hpp"
#include "vector.hpp"
#include "utils_of_ut.hpp"

namespace mfwu {

class unit_test_rbtree {
public:
    bool use_rbtree_push();
    bool use_rbtree_pop();
    bool use_cmp_functor();
    
private:
    template <typename T, typename CmpFunctor>
    void print_basic_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        std::cout << "size() : " << rbt.size() << " "
                  << " height(): " << rbt.height() << "\n";
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename T, typename CmpFunctor>
    void print_color_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_color_struct(rbt.root_);
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename T, typename CmpFunctor>
    void print_detailed_info(const mfwu::rbtree<T, CmpFunctor>& rbt) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_basic_info(rbt);
        // print_color_info(rbt);
        // print_tree_struct(rbt.root_);
        print_colored_struct(rbt.root_);
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename Node>
    mfwu::vector<std::string> tree2color(Node* root) {
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
    void print_color_struct(Node* root) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_heap_struct(tree2color(root));
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }
    template <typename Node>
    mfwu::vector<Node*> tree2node(Node* root) {
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
    void print_colored_struct(Node* root) {
#ifndef __UNIT_TEST_RBTREE_BRIEF__
        print_rbtree_heap_struct(tree2node(root));
#endif  // __UNIT_TEST_RBTREE_BRIEF__
    }

    mfwu::rbtree<int> rbt_;

};  // endof class unit_test_ rbtree

bool unit_test_rbtree::use_rbtree_push() {
    std::cout << "\n------- Test: use mfwu::rbtree::push -------\n";
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

    std::cout << "random pushing test: part 1\n";
    mfwu::rbtree<int> rbt6 = {1, 9, 1, 5, 6, 7, 0, 1, 4, 9, 8};
    print_detailed_info(rbt6);
    rbt6.push(0);
    print_detailed_info(rbt6);
    rbt6.push(6);
    print_detailed_info(rbt6);
    rbt6.push(7);
    print_detailed_info(rbt6);
    rbt6.push(2);
    print_detailed_info(rbt6);
    rbt6.push(9);
    print_detailed_info(rbt6);

    std::cout << "random pushing test: part 2\n";
    rbt6 = {2, 8, 8, 6, 1, 4, 7, 8, 3, 4, 1,
            8, 5, 9, 3, 5, 7, 6, 4, 0, 2, 6};
    print_detailed_info(rbt6);
    rbt6.push(1);
    print_detailed_info(rbt6);
    rbt6.push(5);
    print_detailed_info(rbt6);
    rbt6.push(4);
    print_detailed_info(rbt6);
    rbt6.push(8);
    print_detailed_info(rbt6);
    rbt6.push(3);
    print_detailed_info(rbt6);

    rbt_ = rbt5;
    
    return 0;
}

bool unit_test_rbtree::use_rbtree_pop() {
    std::cout << "\n------- Test: use mfwu::rbtree::pop -------\n";
    std::cout << "random pushing/poping test: part 1\n";
    mfwu::rbtree<int> rbt1 = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    print_detailed_info(rbt1);
    rbt1.push(6);
    rbt1.pop();
    print_detailed_info(rbt1);
    rbt1.push(5);
    rbt1.pop(2);
    print_detailed_info(rbt1);
    rbt1.push(9);
    rbt1.pop(5);
    print_detailed_info(rbt1);
    rbt1.push(1);
    rbt1.pop();
    print_detailed_info(rbt1);
    rbt1.push(4);
    rbt1.pop();
    rbt1.pop();
    rbt1.pop();
    rbt1.push(6);
    rbt1.push(7);
    rbt1.pop(7);
    rbt1.pop(7);
    for (int i = 0; i < 10; i++) { rbt1.pop(6); }
    print_detailed_info(rbt1);

    std::cout << "random pushing/poping test: part 2\n";
    print_detailed_info(rbt_);
    for (int i = 0; i < 12; i++) { rbt_.push(6); }
    rbt_.pop(6);
    print_detailed_info(rbt_);
    rbt_.push(4);
    rbt_.pop(4);
    print_detailed_info(rbt_);
    rbt_.push(7);
    rbt_.pop(9);
    print_detailed_info(rbt_);
    rbt_.push(0);
    rbt_.pop(1);
    print_detailed_info(rbt_);
    rbt_.push(9);
    rbt_.pop();
    rbt_.pop(3);
    rbt_.pop();
    rbt_.push(2);
    rbt_.push(8);
    rbt_.pop(7);
    rbt_.pop(7);
    for (int i = 0; i < 20; i++) { rbt_.pop(6); }
    print_detailed_info(rbt_);

    return 0;
}

bool unit_test_rbtree::use_cmp_functor() {
    return 0;
}


}  // endof class mfwu
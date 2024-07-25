#ifndef __AVL_TREE__
#define __AVL_TREE__

#include "common.hpp"

namespace mfwu {

template <typename T, typename CmpFunctor>
class avl_tree {
public:
    using value_type = T;
    using size_type = size_t;
    using node = tree_node<T>;

    avl_tree() : root_() {}
    
private:
    node* root_;
};  // endof class mfwu

}  // endof namespace mfwu


#endif  // __AVL_TREE__
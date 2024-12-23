#ifndef __TRIE_HPP__
#define __TRIE_HPP__

#include "unordered_map.hpp"

namespace mfwu {

template <typename T, bool Mode>
struct trie_node;

template <typename T>
struct trie_node<T, 0> {
    T val = {};
    size_t path_count = 0;
    size_t end_count = 0;
    mfwu::unordered_map<T, trie_node*> children = {};

    trie_node() = default;
    trie_node(const T& v) : val(v) {}
};  // endof struct trie_node<T, 0>

template <typename T>
struct trie_node<T, 1> {
    T val = {};
    size_t path_count = 0;
    size_t end_count = 0;
    trie_node* children[128] = {};
};  // endof struct trie_node<T, 1>

class unit_test_trie;

template <typename ContainerType, bool Mode=false>
class trie {
public:
    friend class unit_test_trie;

    using value_type = typename ContainerType::value_type;
    using size_type = size_t;
    using node = trie_node<value_type, Mode>;

    trie() : root_(new node()) {}
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value
              >
             >
            //   typename = typename std::enable_if<
            //       typename std::is_same<
            //           mfwu::iterator_traits<InputIterator>::value_type,
            //           ContainerType
            //       >::value
            //   >
             
    trie(InputIterator first, InputIterator last) 
        : root_(new node()) {
        for ( ; first != last; ++first) {
            insert(*first);
        }
    }
    // template <typename InputIterator,
    //           typename = typename std::enable_if<
    //               mfwu::is_input_iterator<InputIterator>::value
    //           >
    //          >
    //         //   typename = typename std::enable_if<
    //         //       typename std::is_same<
    //         //           mfwu::iterator_traits<InputIterator>::value_type,
    //         //           value_type
    //         //       >::value
    //         //   >

    // trie(InputIterator first, InputIterator last)
    //     : root_(new node({}, 0, 0, {})) {
    //     insert(first, last);
    // }   
    trie(const trie& t) {
        root_ = copy_trie(t.root_);
    } 
    trie(trie&& t) : root_(t.root_) {
        t.root_ = nullptr;
    }
    ~trie() {
        reset();
    }

    trie& operator=(const trie& t) {
        reset();
        root_ = copy_trie(t.root_);
        return *this;
    }
    trie& operator=(trie&& t) {
        root_ = t.root_;
        t.root_ = nullptr;
    }

    int depth() const {
        if (root_ == nullptr) return 0;
        return depth(root_);
    }

    void insert(const ContainerType& container) {
        insert(container.begin(), container.end());
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value
              >
             >
    void insert(InputIterator first, InputIterator last) {
        node* cur = root_;
        for ( ; first != last; ++first) {
            cur->path_count++;
            if (!cur->children.count(*first)) {
                cur->children[*first] = new node(*first);
            }
            cur = cur->children[*first];
        }
        cur->path_count++;
        cur->end_count++;
    }
    void erase(const ContainerType& container) {
        erase(container.begin(), container.end());
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value
              >
             >
    void erase(InputIterator first, InputIterator last) {
        node* cur = root_;
        for ( ; first != last; ++first) {
            cur->path_count--;
            if (!cur->children.count(*first)) {
                // no such res that can be erased
                // TODO: restore path_count
            }
            cur = cur->children[*first];
        }
        cur->path_count--;
        cur->end_count--;
    }
    size_type count(const ContainerType& container) {
        return count(container.begin(), container.end());
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value
              >
             >
    size_type count(InputIterator first, InputIterator last) {
        node* cur = root_;
        for ( ; first != last; ++first) {
            cur->path_count++;
            if (!cur->children.count(*first)) {
                return 0;
            }
            cur = cur->children[*first];
        }
        return cur->end_count; 
    }
    size_type count_pref(const ContainerType& container) {
        return count_pref(container.begin(), container.end());
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  mfwu::is_input_iterator<InputIterator>::value
              >
             >
    size_type count_pref(InputIterator first, InputIterator last) {
        node* cur = root_;
        for ( ; first != last; ++first) {
            cur->path_count++;
            if (!cur->children.count(*first)) {
                return 0;
            }
            cur = cur->children[*first];
        }
        return cur->path_count; 
    }
    mfwu::vector<ContainerType> get_pref_with(const ContainerType& container) {
        node* cur = root_;
        for (auto first = container.begin(); 
             first != container.end(); ++first) {
            cur->path_count++;
            if (!cur->children.count(*first)) {
                return 0;
            }
            cur = cur->children[*first];
        }
        mfwu::vector<ContainerType> ret = {};
        get_suc(cur, ret);
        for (auto&& suc : ret) {
            suc = container + suc;
        }
        return ret;
    }

private:
    node* copy_trie(node* cur) {
        if (cur == nullptr) return nullptr;
        node* ret = new node(*cur);
        for (auto [val, child] : cur->children) {
            ret->children[val] = copy_trie(child);
        }
        return ret;
    }
    void reset() {} // TODO
    void get_suc(node* cur, mfwu::vector<ContainerType>& ret,
                 ContainerType& container) {
        for (auto&& [val, child] : cur->children) {
            container += val;
            if (cur->end_count > 0) {
                ret.emplace_back(container);
            }
            get_suc(cur, ret, container);
            container.pop_back();
        }
    }

    int depth(node* cur) const {
        if (cur == nullptr) return 0;
        int ret = 1;
        for (auto&& [val, child] : cur->children) {
            ret = mfwu::max(depth(child) + 1, ret);
        }
        return ret;
    }

    node* root_;

};  // endof class trie


}  // endof namespace mfwu

#endif  // __TRIE_HPP__
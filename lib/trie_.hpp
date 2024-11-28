#include "unordered_map.hpp"


template <typename T, bool Mode>
struct trie_node;

template <typename T>
struct trie_node<T, 0> {
    T val;
    size_t path_count = 0;
    size_t end_count = 0;
    mfwu::unordered_map<T, trie_node*> children;    
};

template <typename T>
struct trie_node<T, 1> {
    T val;
    size_t path_count = 0;
    size_t end_count = 0;
    trie_node*[128] children;
};

template <typename ContainerType, bool Mode=false>
class trie {
public:
    using value_type = ContainerType::value_type;
    using size_type = size_t;
    using node = trie_node<value_type, Mode>;

    trie() : root_(new node({}, 0, 0, {})) {}
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value
              >
              typename = typename std::enable_if<
                  typename std::is_same<
                      mfwu::iterator_traits<InputIterator>::value_type,
                      ContainerType
                  >::value
              >
             >
    trie(InputIterator first, InputIterator last) 
        : root_(new node({}, 0, 0, {})) {
        for ( ; first != last; ++first) {
            insert(*first);
        }
    }
    template <typename InputIterator,
              typename = typename std::enable_if<
                  mfwu::is_input_iterator<InputIterator>::value
              >
              typename = typename std::enable_if<
                  typename std::is_same<
                      mfwu::iterator_traits<InputIterator>::value_type,
                      value_type
                  >::value
              >
             >
    trie(InputIterator first, InputIterator last)
        : root_(new node({}, 0, 0, {})) {
        insert(first, last);
    }   
    trie(const trie& t) {
        copy_trie(t);
    } 
    trie(trie&& t) : root_(t.root_) {
        t.root_ = nullptr;
    }
    ~trie() {
        reset();
    }

    trie& operator=(const trie& t) {
        reset();
        copy_trie(t);
        return *this;
    }
    trie& operator=(trie&& t) {
        root_ = t.root_;
        t.root_ = nullptr;
    }

    void insert(const ContainerType& container) {
        insert(container.begin(), container.end());
    }
    template <typename InputIterator,
              typename = typename std::enable_if<
                  typename mfwu::is_input_iterator<InputIterator>::value
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
              typename = typename std::enable_if<
                  typename mfwu::is_input_iterator<InputIterator>::value
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
              typename = typename std::enable_if<
                  typename mfwu::is_input_iterator<InputIterator>::value
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
              typename = typename std::enable_if<
                  typename mfwu::is_input_iterator<InputIterator>::value
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
        for ( ; first != last; ++first) {
            cur->path_count++;
            if (!cur->children.count(*first)) {
                return 0;
            }
            cur = cur->children[*first];
        }
        return get_suc();
    }

private:
    void copy_trie() {

    }
    mfwu::vector<ContainerType> get_suc

    node* root_;

};
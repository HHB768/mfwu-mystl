#ifndef __HASHTABLE_WTOT_HPP__
#define __HASHTABLE_WTOT_HPP__

#include "common.hpp"
#include "allocator.hpp"
#include "algorithm.hpp"
#include "pair.hpp"
#include "iterator.hpp"
#include "hash_algorithm_pool.hpp"
#include "multimap.hpp"
#include <initializer_list>

namespace mfwu {

// we will not offer bucket_wtot for non-exp node
// there is further explanation below
// X-Qx 250209

template <typename key_type, typename value_type, typename Compare=mfwu::less<key_type>>
class bucket_with_rbtree {
public:
    // mfwu::map uses node with value_type "pair<key_t, val_t>"
    // instead of "pair<const key_t, val_t>" bcz its rbtree may
    // change the node value to rebalance itself
    // so we will no offer bucket_wtot for non-exp node
    // X 250210
    using rbtr = mfwu::multimap<key_type, value_type, Compare>;
    using node = typename rbtr::node;
    using iterator = typename rbtr::iterator;

    bucket_with_rbtree() : rbt_() {}
    bucket_with_rbtree(const bucket_with_rbtree& bkt) : rbt_(bkt.rbt_) {}
    bucket_with_rbtree(bucket_with_rbtree&& bkt) : rbt_(mfwu::move(bkt.rbt_)) {}
    ~bucket_with_rbtree() {}

    bucket_with_rbtree& operator=(const bucket_with_rbtree& bkt) {
        rbt_ = bkt.rbt_;
        return *this;
    }
    bucket_with_rbtree& operator=(bucket_with_rbtree&& bkt) {
        rbt_ = mfwu::move(bkt.rbt_);
        return *this;
    }
    
    bool empty() { return rbt_.empty(); }
    size_t size() {
        return rbt_.size();
    }
    node* front() { return rbt_.begin().get_cur(); }
    mfwu::pair<node*, bool> push(const key_type& key, 
                                 const value_type& value) {
        mfwu::pair<iterator, bool> res = rbt_.insert(key, value);
        return mfwu::make_pair<node*, bool>(res.first.get_cur(), res.second);
        // 颇有大炮打蚊子的美感 xQx 25.02.09
    }
    // DONT USE THIS
    void pop() {
        rbt_.erase(rbt_.begin());
    }
    bool pop(const key_type& key) {
        return rbt_.erase(key);
    }
    bool pop(node* cur) {
        return pop(cur->val.first);
    }
    mfwu::pair<value_type&, bool> get(const key_type& key) {
        node* ret = search(key);
        bool is_new_node = false;
        if (ret == nullptr) {
            ret = push(key, value_type{}).first;
            is_new_node = true;
        }
        return {ret->val.second, is_new_node};
    }
    bool count(const key_type& key) const {
        return search(key) != nullptr;
    }
    node* find(const key_type& key) const {
        return search(key);
    }
private:
    node* search(const key_type& key) const {
        return rbt_.find(key).get_cur();
    }

    rbtr rbt_;  // multimap now
};  // endof class bucket

class unit_test_hashtable_wtot;

template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Compare=mfwu::less<Key>,  // these hashtble needs comparability
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket_with_rbtree<Key, Value, Compare>, mfwu::malloc_alloc>>
class hashtable_with_tree {
public:
    friend class mfwu::unit_test_hashtable_wtot;
    using size_type = size_t;

    using bucket = mfwu::bucket_with_rbtree<Key, Value, Compare>;
    using node = typename bucket::node;
    class hashtable_iterator {
    public:
        using value_type = mfwu::pair<const Key, Value>;
        using iterator_category = mfwu::forward_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;

        hashtable_iterator() : cur_(nullptr), buckets_(nullptr) {}
        hashtable_iterator(node* cur, bucket* buckets)
            : cur_(cur), buckets_(buckets) {}
        hashtable_iterator(const hashtable_iterator& it)
            : cur_(it.cur_), buckets_(it.buckets_) {}
        ~hashtable_iterator() {}
        hashtable_iterator& operator=(const hashtable_iterator& it) {
            cur_ = it.cur_; buckets_ = it.buckets_;
            return *this;
        }
        bool operator==(const hashtable_iterator& it) const {
            return buckets_ == it.buckets_ && cur_ == it.cur_;
        }
        bool operator!=(const hashtable_iterator& it) const {
            return !(*this == it);
        }
        const value_type& operator*() const {
            return cur_->val;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        hashtable_iterator& operator++() {
            cur_ = cur_->get_inorder_next();  // here
            while (cur_ == nullptr) {
                ++buckets_;
                cur_ = buckets_->front();
            }
            return *this;
        }
        hashtable_iterator operator++(int) {
            hashtable_iterator tmp = *this;
            this->operator++();
            return mfwu::move(tmp);
        }
        node* get_cur() const {
            return cur_;
        }
        bucket* get_bucket() const {
            return buckets_;
        }
    private:
        node* cur_;
        bucket* buckets_;
    };  // endof class hashtable_iterator
    using iterator = hashtable_iterator;

    using key_type = Key;
    using value_type = Value;

    hashtable_with_tree(): capacity_(mfwu::get_next_primer(0)), size_(0),
        buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_with_tree(size_type capacity) 
        : capacity_(mfwu::get_next_primer(capacity)),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        init_dummy_node();
    }
    hashtable_with_tree(const std::initializer_list<
              mfwu::pair<const key_type, value_type>>& vals)
    // GPT DEBUG LOG 10.21/24:
    /*
        std::initializer_list elements are immutable, meaning they need to be const. 
        When you initialize with std::pair<B, C>, the compiler expects the first element 
        of the pair to be const B. So to avoid compilation errors, you must use 
        const std::initializer_list<std::pair<const B, C>>. 
        Keeps things aligned with how initializer_list handles immutability.
    */
        : capacity_(mfwu::get_next_primer(
                    std::ceil((float)vals.size() / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (auto&& [k, v] : vals) {
            this->insert(k, v);
        }
        init_dummy_node();
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  is_input_iterator<InputIterator>::value>
             >
    hashtable_with_tree(InputIterator first, InputIterator last)
        : capacity_(mfwu::get_next_primer(std::ceil(
                    (float)(mfwu::distance(first, last)) / alpha))),
          size_(0), buckets_(Alloc::allocate(capacity_ + 1)) {
        construct();
        for (; first != last; ++first) {
            this->insert(*first);
        }
        init_dummy_node();
    }
    hashtable_with_tree(const hashtable_with_tree& tbl) 
        : capacity_(tbl.capacity_), size_(tbl.size_),
          buckets_(Alloc::allocate(capacity_ + 1)) {
        mfwu::uninitialized_copy(tbl.buckets_,
            tbl.buckets_ + capacity_ + 1, buckets_);
    }
    hashtable_with_tree(hashtable_with_tree&& tbl) : capacity_(tbl.capacity_),
        size_(tbl.size_), buckets_(tbl.buckets_) {
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  
        // found it ! 
        // not in 10.11 but 241022
    }
    ~hashtable_with_tree() {
        reset();
    }
    hashtable_with_tree& operator=(const hashtable_with_tree& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::uninitialized_copy(tbl.buckets_, 
            tbl.buckets_ + capacity_ + 1, buckets_);
        return *this;
    }
    hashtable_with_tree& operator=(hashtable_with_tree&& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        buckets_ = tbl.buckets_;
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  // nt!
        // let deallocate() deallocate nothing
        // 24.10.11
        return *this;
    }
    mfwu::pair<iterator, bool> insert(const key_type& key, const value_type& val) {
        mfwu::pair<node*, bool> ret = buckets_[hash(key)].push(key, val);
        add_cnt(ret.second);
        return {find(key), ret.second};
        // note: you must search again bcz add_cnt may
        //       rehash the buckets, but found on 24.10.23 T^T
    }
    mfwu::pair<iterator, bool> insert(const mfwu::pair<const key_type, value_type>& key_val) {
        return insert(key_val.first, key_val.second);
    } 
    bool erase(const key_type& key) {
        size_type hashed_key = hash(key);
        bool ret = buckets_[hashed_key].pop(key);
        size_ -= ret;
        return ret;
    }
    iterator erase(iterator it) {
        node* cur = it.get_cur();
        bucket* bkt = it.get_bucket();
        ++it;
        bkt->pop(cur);
        return it;
    }
    // TODO: erase by iterator !
    value_type& operator[](const key_type& key) {
        // assert
        add_cnt(buckets_[hash(key)].get(key).second);
        // note: you must search again bcz add_cnt may
        //       rehash the buckets 24.10.11 
        return buckets_[hash(key)].get(key).first;
    }
    bool count(const key_type& key) const {
        return buckets_[hash(key)].count(key);
    }
    iterator find(const key_type& key) const {
        bucket* bkt = buckets_ + hash(key);
        node* cur = bkt->find(key);
        if (cur == nullptr) {
            return end();
        }
        return iterator(cur, bkt);
    }
    float load_factor() const {
        return (float)size_ / capacity_;
    }
    float max_load_factor() const {
        return alpha;
    }
    void rehash(size_type capacity) {
        capacity = mfwu::get_next_primer(capacity);
        while ((float)size_ / capacity >= max_load_factor()) {
            capacity = mfwu::get_next_primer(capacity + 1);
        }
        rehash_hard(capacity);  // TODO: check
    }
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type capacity() const { return capacity_; }
    iterator begin() const { return iterator(get_first_node(), &get_first_bucket()); }
    iterator end() const { return iterator(get_dummy_node(), &get_dummy_bucket()); }
private:
    void construct() {
        bucket bkt{};  // avoid move construct
        mfwu::construct(buckets_, buckets_ + capacity_ + 1, bkt);
    }
    // TODO: let me think, construct dummy bucket first, then construct others with
    //       enable = dummy.enable (which is new bool(f) swhere)
    //       when req_mem, keep the same bool*
    void destroy() { mfwu::destroy(buckets_, buckets_ + capacity_ + 1); }
    void deallocate() { Alloc::deallocate(buckets_, capacity_ + 1); }
    void reset() { destroy(); deallocate(); }
    void init_dummy_node() {
        buckets_[capacity_].push(key_type{}, value_type{});
    }
    size_type hash(const key_type& key) const {
        return hashfunc_(key) % capacity_;
    }
    void rehash_hard(size_type capacity) {
        hashtable_with_tree newtable = hashtable_with_tree(capacity);
        for (size_type idx = 0; idx < capacity_; ++idx) {
            bucket cur = buckets_[idx];
            while (!cur.empty()) {
                node* nd = cur.front();
                newtable.insert(nd->val.first, nd->val.second);
                cur.pop();
            }
        }
        newtable.size_ = this->size_;
        *this = mfwu::move(newtable);
    }
    void req_mem() {
        rehash(capacity_ + 1);
    }
    void add_cnt(size_type num) {
        if (!num) return ;
        size_ += num;
        while ((float)size_ / capacity_ > alpha) {
            req_mem();
        }
    }

    bucket& get_first_bucket() const {
        for (size_type idx = 0; idx < capacity_; ++idx) {
            if (!buckets_[idx].empty()) return buckets_[idx];
        }
        return get_dummy_bucket();
    }
    node* get_first_node() const {
        return get_first_bucket().front();
    }
    bucket& get_dummy_bucket() const {
        return buckets_[capacity_];
    }
    node* get_dummy_node() const {
        return get_dummy_bucket().front();
    }

    size_type capacity_;
    size_type size_;
    bucket* buckets_;
    static Hash hashfunc_;
    constexpr static float alpha = 0.7F;

};  // endof class hashtable_with_tree

template <typename Key, typename Value, typename Hash, typename Compare, typename Alloc>
Hash hashtable_with_tree<Key, Value, Hash, Compare, Alloc>::hashfunc_ = {};


/* 
    i want to implement a hashtable with hashtable-like buckets
    but i fail to do so bcz you can not guarantee that
    multiple hash ops will eventually divide them into different buckets
    so we will finally have a different underlying hashtable (with
    different hasher or using list / tree), it is not graceful enough.
    therefore, i may implement a usable hashtable_with_htbl with 
    limited depth later... just maybe...           XQX 250212  
*/
template <typename Key, typename Value, typename Hash, typename Alloc>
class hashtable_with_htbl;

template <typename key_type, typename value_type>
class bucket_with_htbl;

template <typename Key, typename Value>
class htbl_base {
public:
    friend class mfwu::unit_test_hashtable_wtot;

    using bucket = mfwu::bucket_with_htbl<Key, Value>;
    using node = typename bucket::node;
    using size_type = size_t;

    class hashtable_iterator {
    public:
        // TODO: should i use value_type = 
        //       mfwu::pair<Key, Value> ?
        //       how to adjust when Key is Value
        // 10.15: yes you should
        // TODO: plz try this
        using value_type = mfwu::pair<const Key, Value>;
        using iterator_category = mfwu::forward_iterator_tag;
        using pointer = value_type*;
        using reference = value_type&;
        using difference_type = mfwu::ptrdiff_t;

        hashtable_iterator() : cur_(nullptr), buckets_(nullptr) {}
        hashtable_iterator(node* cur, bucket* buckets)
            : cur_(cur), buckets_(buckets) {}
        hashtable_iterator(const hashtable_iterator& it)
            : cur_(it.cur_), buckets_(it.buckets_) {}
        ~hashtable_iterator() {}
        hashtable_iterator& operator=(const hashtable_iterator& it) {
            cur_ = it.cur_; buckets_ = it.buckets_;
            return *this;
        }
        bool operator==(const hashtable_iterator& it) const {
            return buckets_ == it.buckets_ && cur_ == it.cur_;
        }
        bool operator!=(const hashtable_iterator& it) const {
            return !(*this == it);
        }
        value_type& operator*() const {
            return *cur_->val_;
        }
        value_type* operator->() const {
            return & this->operator*();
        }

        hashtable_iterator& operator++() {
            // inside the bucket to which cur belongs
            bucket* bkt = buckets_;
            if (bkt->get_status() == 0) { std::cerr << "error!\n"; }
            else if (bkt->get_status() == 1) { /* no next node in this bucket */ }
            else {  // == 2
                iterator lower_it = iterator(
                    cur_, bkt->htbl_->buckets_ + bkt->htbl_->hash(cur_->val_->first));
                ++lower_it;
                if (lower_it.get_cur() != bkt->htbl_->get_dummy_node()) {
                    cur_ = lower_it.cur_;
                    return *this;
                }  /* otherwise, no next node in this bucket */
            }

            while (cur_ == nullptr) {
                // the dummy node val should always be valid
                // it is compatible with it == end()?
                ++buckets_;
                cur_ = buckets_->front();
            }
            return *this;
        }
        hashtable_iterator operator++(int) {
            hashtable_iterator tmp = *this;
            this->operator++();
            return mfwu::move(tmp);
        }
        node* get_cur() const {
            return cur_;
        }
        bucket* get_bucket() const {
            return buckets_;
        }
    private:
        node* cur_;
        bucket* buckets_;
    };  // endof class hashtable_iterator
    using iterator = hashtable_iterator;

    htbl_base() : buckets_(nullptr) {}
    htbl_base(bucket* bkt) : buckets_(bkt) { std::cout << buckets_ << "\n";}
    // should we delete copy constructor here? 25.02.13   ANS: no. XQX 0213
    htbl_base(const htbl_base& tbl) : buckets_(tbl.buckets_ ? new bucket(*tbl.buckets_) : nullptr) {}
    virtual ~htbl_base() {}

    using key_type = Key;
    using value_type = Value;

    virtual bool empty() const { return false; }
    virtual iterator find(const key_type& key) const { return iterator(); }
    virtual iterator end() const { return {}; }
    virtual mfwu::pair<iterator, bool> 
    insert(const key_type& key, const value_type& val) { return {}; }
    virtual mfwu::pair<iterator, bool> 
    insert(const mfwu::pair<const key_type, value_type>& key_val) { return {}; } 
    virtual bool erase(const key_type& key) { return 0; }
    virtual iterator erase(iterator it) { return {}; }
    virtual node* get_first_node() const { return nullptr; }
    virtual node* get_dummy_node() const { return nullptr; }
    virtual size_type hash(const key_type& key) const { return 0; }

    bucket* buckets_ = nullptr;
    // dont alloc or dealloc in this class
};  // endof class htbl_base

template <typename key_type, typename value_type>
class bucket_with_htbl {
public:
    friend class mfwu::unit_test_hashtable_wtot;
    friend class mfwu::htbl_base<key_type, value_type>;
    // template <typename Hash, typename Alloc>
    // friend class mfwu::hashtable_with_htbl<key_type, value_type, Hash, Alloc>;
    // error: specialization of ‘template<class Key, class Value, class Hash, 
    // class Alloc> class mfwu::hashtable_with_htbl’ must appear at namespace scope
    // specialization should appear at namespace instead of class scope XQX 25.02.13
    template <typename Key, typename Value, typename Hash, typename Alloc>
    friend class mfwu::hashtable_with_htbl;

    // using node = mfwu::pair<const key_type, value_type>;  // discarded
    struct node {
    public:
        // we dont need the thisBucket_
        // the iterator records the highlevel buckets_
        // if we record lowlevel thisBuckets_ like this
        // we will need thisHashtable in the bucket members
        // X-Q3 25.02.13
        using val_t = mfwu::pair<const key_type, value_type>;
        node() = delete;
        node(val_t* val) 
            : val_(val) {}
        node(const node& nd) 
            : val_(nd.val_ ? new val_t(*nd.val_) : nullptr) {}
        node(node&& nd) : val_(nd.val_) {
            nd.val_ = nullptr;
        } 
        ~node() {
            if (val_) { delete val_; }
        }
        val_t* val_ = nullptr;
        // bucket_with_htbl* thisBucket_ = nullptr;
    };  // endof class node

    bucket_with_htbl()
        : cur_(new node(nullptr)), htbl_(nullptr), status_(0) {std::cout << "b\n";}
    bucket_with_htbl(const bucket_with_htbl& bkt)
        : cur_(bkt.cur_ ? new node(*bkt.cur_) : nullptr),
          htbl_(bkt.htbl_ ? new htbl_base(*bkt.htbl_) : nullptr) , status_(bkt.status_) { std::cout << " ?-- \n"; }
    // i found in many places we dont have nullptr detection before deref
    bucket_with_htbl(bucket_with_htbl&& bkt) 
        : cur_(bkt.cur_), htbl_(bkt.htbl_), status_(bkt.status_) {
        std::cout << "a\n";
        bkt.cur_ = nullptr;
        bkt.htbl_ = nullptr;
    }
    ~bucket_with_htbl() {
        bucket_destroy();
    }

    bucket_with_htbl& operator=(const bucket_with_htbl& bkt) {
        clear();
        bucket_copy(bkt);
        return *this;
    }
    bucket_with_htbl& operator=(bucket_with_htbl&& bkt) {
        clear();
        bucket_copy(mfwu::move(bkt));
        return *this;
    }
    
    bool empty() { 
        return status_ == 0 
               || (status_ == 2 && htbl_->empty()); 
    }
    size_t size() {
        if (status_ == 2) {
            return htbl_.size();
        } 
        return status_;
    }
    node* front() {
        if (status_ == 1) return cur_;
        return htbl_->get_first_node(); 
    }
    mfwu::pair<node*, bool> push(const key_type& key, 
                                 const value_type& value) {
        if (status_ == 0) {
            cur_->val_ = new mfwu::pair<const key_type, value_type>(key, value);
            status_ = 1;
            return {cur_, true};
        } else if (status_ == 1) {
            if (cur_->val_->first == key) {
                cur_->val_->second = value;
                return {cur_, false};
            } else {
                // htbl_ = new hashtable_with_htbl  // you cannot do this bcz you have no Hash and Alloc
                assert(htbl_ != nullptr);
                htbl_->insert(*cur_->val_);
                cur_->val_ = nullptr;
                status_ = 2;
            }
        } 
        auto ret = htbl_->insert(key, value);
        return {ret.first.get_cur(), ret.second};
    }
    // DONT USE THIS FUNC  
    void pop() {}

    bool pop(const mfwu::pair<const key_type, value_type>& val) {
        return pop(val.first);
    }
    bool pop(const key_type& key) {
        if (status_ == 0) {
            return false;
        } else if (status_ == 1) {
            if (key == cur_->val_->first) {
                cur_->val_ = nullptr;
                status_ = 0;
                return true;
            } else {
                return false;
            }
        }
        return htbl_->erase(key);
        // if status_ == 2, it will not come back to 1 / 0
        // even after elements are erased, except clear() being called
        // what you have to concern is how to dealloc its mem
        // X-Qx 25.02.11
    }
    mfwu::pair<value_type&, bool> get(const key_type& key) {
        node* ret = search(key);
        bool is_new_node = false;
        if (ret == nullptr) {
            ret = push(key, value_type{}).first;
            is_new_node = true;
        }
        return {ret->val_->second, is_new_node};
    }
    bool count(const key_type& key) const {
        return search(key) != nullptr;
    }
    node* find(const key_type& key) const {
        return search(key);
    }
    int get_status() const {
        return status_;
    }
private:
    void bucket_copy(const bucket_with_htbl& bkt) {
        cur_ = new node(*bkt.cur_);
        htbl_ = new htbl_base(*bkt.htbl_);
    }
    void bucket_destroy() {
        clear();
    }
    void clear() { 
        if (cur_) {
            delete cur_;
            cur_ = nullptr;
        } 
        if (htbl_) {
            delete htbl_; 
            htbl_ = nullptr;
        }
        status_ = 0;
    }
    node* search(const key_type& key) const {
        if (status_ == 0) { return nullptr; }
        if (status_ == 1) {
            if (cur_->val_->first == key) { return cur_; }
            return nullptr;
        }
        return htbl_->find(key).get_cur();
    }

    node* cur_;
    htbl_base<key_type, value_type>* htbl_;
    int status_ = 0;
    // htbl_base<key_type, value_type>* thisHtbl_;  // it seems we dont need this
};  // endof class bucket_with_htbl

class unit_test_hashtable_wtot;

/*
    c++ 两阶段名字查找，对于非依赖模板名称的都是现场决议的，编译器不查找基类 scope，没找到就报错，
    编译器不查找基类，因为此时基类还是个类模板，到底是什么东西还不定呢。
    然后实例化的时候进行第二阶段名字查找，这时候所有类型都是确定的，才能够查找基类 scope。
    增加 this 或 Base:: 就是把对 function 的查找延迟到第二阶段，这是 c++ 标准规定的                  
    https://blog.csdn.net/sb985/article/details/79670881
*/
template <typename Key,
          typename Value,
          typename Hash=mfwu::hash_functor<Key>,
          typename Alloc=mfwu::DefaultAllocator<
                         mfwu::bucket_with_htbl<Key, Value>, mfwu::malloc_alloc>>
class hashtable_with_htbl : public htbl_base<Key, Value> {
public:
    friend class mfwu::unit_test_hashtable_wtot;
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;

    using bucket = mfwu::bucket_with_htbl<Key, Value>;
    using node = typename bucket::node;
    using base_type = htbl_base<Key, Value>;
    using iterator = typename base_type::hashtable_iterator;

    
    hashtable_with_htbl(): capacity_(mfwu::get_next_primer(0)), size_(0),
        /*base_type(Alloc::allocate(capacity_ + 1)),*/ k_(rand01()) {
        // you cannot do this in initlist, bcz base_type is the first member
        // and capacity_ is the second (maybe), you can just use the code
        // from hashtable_with_tree orz  xqx 25.02.13
        this->buckets_ = Alloc::allocate(capacity_ + 1);
        construct();
        init_dummy_node();
    }
    hashtable_with_htbl(size_type capacity) 
        : capacity_(mfwu::get_next_primer(capacity)),
          size_(0), k_(rand01()) {
        this->buckets_ = Alloc::allocate(capacity_ + 1);
        construct();
        // GPT DEBUG HISTORY 24.10.11
        /*
            If the objects constructed in the loop are being destructed immediately
            after the loop, ensure their destructors are not causing the crash. 
            This can happen if the destructor tries to access invalid memory.
        */
        init_dummy_node();
    }
    hashtable_with_htbl(const std::initializer_list<
              mfwu::pair<const key_type, value_type>>& vals)
    // GPT DEBUG LOG 10.21/24:
    /*
        std::initializer_list elements are immutable, meaning they need to be const. 
        When you initialize with std::pair<B, C>, the compiler expects the first element 
        of the pair to be const B. So to avoid compilation errors, you must use 
        const std::initializer_list<std::pair<const B, C>>. 
        Keeps things aligned with how initializer_list handles immutability.
    */
        : capacity_(mfwu::get_next_primer(
                    std::ceil((float)vals.size() / alpha))),
          size_(0), k_(rand01()) {
        this->buckets_ = Alloc::allocate(capacity_ + 1);
        construct();
        for (auto&& [k, v] : vals) {
            this->insert(k, v);
        }
        init_dummy_node();
    }
    template <typename InputIterator,
              typename = typename std::enable_if_t<
                  is_input_iterator<InputIterator>::value>
             >
    hashtable_with_htbl(InputIterator first, InputIterator last)
        : capacity_(mfwu::get_next_primer(std::ceil(
                    (float)(mfwu::distance(first, last)) / alpha))),
          size_(0), k_(rand01()) {
        this->buckets_ = Alloc::allocate(capacity_ + 1);
        construct();
        for (; first != last; ++first) {
            this->insert(*first);
        }
        init_dummy_node();
    }
    hashtable_with_htbl(const hashtable_with_htbl& tbl) 
        : capacity_(tbl.capacity_), size_(tbl.size_), k_(rand01()) {
        this->buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::uninitialized_copy(tbl.buckets_,
            tbl.buckets_ + capacity_ + 1, this->buckets_);
    }
    hashtable_with_htbl(hashtable_with_htbl&& tbl) : capacity_(tbl.capacity_),
        size_(tbl.size_), base_type(tbl.buckets_), k_(rand01()) {
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  
        // found it ! 
        // not in 10.11 but 241022
    }
    ~hashtable_with_htbl() {
        reset();
    }
    hashtable_with_htbl& operator=(const hashtable_with_htbl& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        this->buckets_ = Alloc::allocate(capacity_ + 1);
        mfwu::uninitialized_copy(tbl.buckets_, 
            tbl.buckets_ + capacity_ + 1, this->buckets_);
        k_ = tbl.k_;
        return *this;
    }
    hashtable_with_htbl& operator=(hashtable_with_htbl&& tbl) {
        reset();
        capacity_ = tbl.capacity_;
        size_ = tbl.size_;
        this->buckets_ = tbl.buckets_;
        tbl.buckets_ = nullptr;
        tbl.capacity_ = -1;  // nt!
        // let deallocate() deallocate nothing
        // 24.10.11
        k_ = tbl.k_;
        return *this;
    }
    mfwu::pair<iterator, bool> insert(const key_type& key, const value_type& val) {
        mfwu::pair<node*, bool> ret = this->buckets_[hash(key)].push(key, val);
        add_cnt(ret.second);
        return {find(key), ret.second};
        // note: you must search again bcz add_cnt may
        //       rehash the buckets, but found on 24.10.23 T^T
    }
    mfwu::pair<iterator, bool> insert(const mfwu::pair<const key_type, value_type>& key_val) {
        return insert(key_val.first, key_val.second);
    } 
    bool erase(const key_type& key) {
        size_type hashed_key = hash(key);
        bool ret = this->buckets_[hashed_key].pop(key);
        size_ -= ret;
        return ret;
    }
    iterator erase(iterator it) {
        node* cur = it.get_cur();
        bucket* bkt = it.get_bucket();
        ++it;
        bkt->pop(cur->val_->first);
        return it;
    }
    // TODO: erase by iterator !
    value_type& operator[](const key_type& key) {
        // assert
        add_cnt(this->buckets_[hash(key)].get(key).second);
        // note: you must search again bcz add_cnt may
        //       rehash the buckets 24.10.11 
        return this->buckets_[hash(key)].get(key).first;
    }
    bool count(const key_type& key) const {
        return this->buckets_[hash(key)].count(key);
    }
    iterator find(const key_type& key) const {
        bucket* bkt = this->buckets_ + hash(key);
        node* cur = bkt->find(key);
        if (cur == nullptr) {
            return end();
        }
        return iterator(cur, bkt);
    }
    float load_factor() const {
        return (float)size_ / capacity_;
    }
    float max_load_factor() const {
        return alpha;
    }
    void rehash(size_type capacity) {
        capacity = mfwu::get_next_primer(capacity);
        while ((float)size_ / capacity >= max_load_factor()) {
            capacity = mfwu::get_next_primer(capacity + 1);
        }
        rehash_hard(capacity);  // TODO: check
    }
    bool empty() const { return size_ == 0; }
    size_type size() const { return size_; }
    size_type capacity() const { return this->capacity_; }
    iterator begin() const { return iterator(get_first_node(), &get_first_bucket()); }
    iterator end() const { return iterator(get_dummy_node(), &get_dummy_bucket()); }
private:
    void construct() {
        bucket bkt{};  // avoid move construct   // ? 250211
        std::cout << "???\n";
        std::cout << this->buckets_ << "\n";
        mfwu::construct(this->buckets_, this->buckets_ + capacity_ + 1, bkt);
        std::cout << "??\n";
        // for (size_type i = 0; i <= capacity_; i++) {
            // this->buckets_[i].htbl_ = new hashtable_with_htbl();
        // }
        // cannot new hashtable_with_htbl here: recursively constructing
    }
    void validate_buckets_() {
        if (!buckets_validation_flag_) {
            for (size_type i = 0; i <= capacity_; i++) {
                this->buckets_[i].htbl_ = new hashtable_with_htbl();
            }
            buckets_validation_flag = true;
        }
    }
    bucket* get_buckets_() {
        validate_buckets_();
        return this->buckets_;
    }
    void destroy() {
        mfwu::destroy(this->buckets_, this->buckets_ + capacity_ + 1);
        // delete htbl_ has already been ~bucket(); 
    }
    void deallocate() { Alloc::deallocate(this->buckets_, capacity_ + 1); }
    void reset() { destroy(); deallocate(); }
    void init_dummy_node() {
        this->buckets_[capacity_].push(key_type{}, value_type{});
    }
    size_type hash(const key_type& key) const {
        return hashfunc_(key) % size_type(capacity_ * k_);
    }
    void rehash_hard(size_type capacity) {
        hashtable_with_htbl newtable = hashtable_with_htbl(capacity);
        for (size_type idx = 0; idx < capacity_; ++idx) {
            bucket cur = this->buckets_[idx];
            while (!cur.empty()) {
                node* nd = cur.front();
                newtable.insert(*nd->val_);
                cur.pop();
            }
        }
        newtable.size_ = this->size_;
        *this = mfwu::move(newtable);
    }
    void req_mem() {
        rehash(capacity_ + 1);
    }
    void add_cnt(size_type num) {
        if (!num) return ;
        size_ += num;
        while ((float)size_ / capacity_ > alpha) {
            req_mem();
        }
    }

    bucket& get_first_bucket() const {
        for (size_type idx = 0; idx < capacity_; ++idx) {
            if (!this->buckets_[idx].empty()) return this->buckets_[idx];
        }
        return get_dummy_bucket();
    }
    node* get_first_node() const {
        return get_first_bucket().front();
    }
    bucket& get_dummy_bucket() const {
        return this->buckets_[capacity_];
    }
    node* get_dummy_node() const {
        return get_dummy_bucket().front();
    }

    size_type capacity_;
    size_type size_;
    static Hash hashfunc_;
    constexpr static float alpha = 0.7F;
    float k_ = 1;
    bool buckets_validation_flag_ = false;

};  // endof class hashtable_with_htbl

template <typename Key, typename Value, typename Hash, typename Alloc>
Hash hashtable_with_htbl<Key, Value, Hash, Alloc>::hashfunc_ = {};

}  // endof namespace mfwu


#endif  // __HASHTABLE_WTOT_HPP__

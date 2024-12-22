#define __UNIT_TEST_TRIE__

#include "trie.hpp"
#include "vector.hpp"
#include "queue.hpp"

namespace mfwu {

class unit_test_trie {
public:
    bool use_mfwu_trie();
private:
    template <typename ContainerType>
    static void print_trie(const trie<ContainerType, 0>& t) {
        using node = typename trie<ContainerType, 0>::node;
        node* cur = t.root_;
        if (cur == nullptr) { return ; }
        size_t depth = t.depth();
        size_t width = get_width(t.root_);
        mfwu::vector<mfwu::vector<node*>> rec(
            depth, mfwu::vector<node*>{}
        );
        mfwu::queue<node*> q;
        q.emplace(cur);
        int count = 0;
        rec[count].emplace_back(cur);
        while (!q.empty()) {
            size_t sz = q.size();
            count++;
            for (int i = 0; i < sz; i++) {
                node* cur = q.front();
                q.pop();
                size_t num_child = cur->children.size();
                for (int j = 0; j < num_child; j++) {
                    q.emplace(cur->children[j]);
                    rec[count].emplace_back(cur->children[j]);
                }
            }
        }
        using value_type = typename ContainerType::value_type;
        mfwu::vector<mfwu::vector<value_type>> cache(
            depth, mfwu::vector<value_type>(width)
        );
        for (int d = depth - 1; d >= 0; d--) {
            int now_w = 0;
            for (int w = 0; w < rec[d].size(); w++) {
                node* cur = rec[d][w];
                size_t ww = get_width(cur);
                cache[d][now_w + ww / 2 + 1] = cur->val;
                now_w += ww;
            }
        }
        for (int d = 0; d < depth; d++) {
            for (int w = 0; w < width; w++) {
                std::cout << cache[d][w];
            }
            std::cout << "\n";
        }
    }
    template <typename Node>
    static int get_width(Node* cur) {
        if (cur == nullptr) return 3;
        int ret = 0;
        bool flag = false;
        for (auto [val, cur]: cur->children) {
            ret += get_width(cur);
            flag = true;
        }
        if (flag) return ret;
        return 3;
    }
};  // endof class unit_test_trie

bool unit_test_trie::use_mfwu_trie() {
    mfwu::vector<std::string> strs = {
        "niganma", "niganma-aiyo", "aiyo", "ninini"
    };
    mfwu::trie<std::string> data(strs.begin(), strs.end());
    print_trie(data);

    return 0;
}

}  // endof namespace mfwu
#define __UNIT_TEST_TRIE__

#include "trie.hpp"
#include "vector.hpp"
#include "queue.hpp"
#include "string.hpp"

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
        // std::cout << "depth: " << depth << "\n";
        mfwu::queue<node*> q;
        q.emplace(cur);
        int count = 0;
        // rec[count].emplace_back(cur);
        // std::cout << "0\n";
        while (!q.empty()) {
            // we dont need this bcz we already know the depth
            // bool flag = false;
            // std::cout << "count: " << count << "\n";
            if (count >= depth) break;
            size_t sz = q.size();
            for (int i = 0; i < sz; i++) {
                node* cur = q.front();
                q.pop();
                if (cur == nullptr) {
                    q.emplace(nullptr);
                    rec[count].emplace_back(nullptr);
                } else {
                    // std::cout << cur->val << ", ";
                    size_t num_child = cur->children.size();
                    for (auto&& [val, child] : cur->children) {
                        q.emplace(child);
                        rec[count].emplace_back(child);
                        // flag = true;
                    }
                    if (num_child == 0) {
                        q.emplace(nullptr);
                        rec[count].emplace_back(nullptr);
                    }
                }
            }
            count++;
            // std::cout << "\n";
            // if (flag == false) break;
        }
        // for (int i = 0; i < rec.size(); i++) {
        //     for (int j = 0; j < rec[i].size(); j++) {
        //         if (rec[i][j] == nullptr) {
        //             std::cout << "   ";
        //         } else {
        //             std::cout << " " << rec[i][j]->val << " ";
        //         }
        //     }
        //     std::cout << "\n";
        // }
        // std::cout << "1\n";
        using value_type = typename ContainerType::value_type;
        mfwu::vector<mfwu::vector<value_type>> cache(
            depth, mfwu::vector<value_type>(width, ' ')
        );
        // we dont show the 'root' layer so d has only depth - 1 layer(s)
        for (int d = depth - 2; d >= 0; d--) {
            int now_w = 0;
            for (int w = 0; w < rec[d].size(); w++) {
                node* cur = rec[d][w];
                size_t ww = get_width(cur);
                if(cur) {
                    cache[d][now_w + ww / 2] = cur->val;
                    cache[d][now_w] = '|';
                    cache[d][now_w + ww - 1] = '|';
                }
                now_w += ww;
            }
        }
        // std::cout << "2\n";
        for (int d = 0; d < depth; d++) {
            for (int w = 0; w < width; w++) {
                std::cout << cache[d][w];
            }
            std::cout << "\n";
        }
        for (int d = depth - 1; d >= 0; d--) {
            for (int w = 0; w < width; w++) {
                std::cout << cache[d][w];
            }
            std::cout << "\n";
        }
    }
    template <typename Node>
    static int get_width(Node* cur) {
        const int display_branch_width = 7;
        if (cur == nullptr) return display_branch_width;
        int ret = 0;
        bool flag = false;
        for (auto [val, cur]: cur->children) {
            ret += get_width(cur);
            flag = true;
        }
        if (flag) return ret;
        return display_branch_width;
    }
};  // endof class unit_test_trie

bool unit_test_trie::use_mfwu_trie() {
    mfwu::vector<mfwu::string<char>> strs = {
        "aaa", "aab", "aabc", "bcd", "acd", "e", "acd", "ace", "aba"
    };
    mfwu::trie<mfwu::string<char>> data(strs.begin(), strs.end());
    print_trie(data);

    return 0;
}

}  // endof namespace mfwu
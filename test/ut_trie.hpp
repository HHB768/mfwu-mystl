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
        // 打印一下q来看看
        q.emplace(cur);
        // q.show_queue();
        int count = 0;
        // rec[count].emplace_back(cur);
        // std::cout << "0\n";
        while (!q.empty()) {
            // we dont need this bcz we already know the depth
            // bool flag = false;
            // std::cout << "count: " << count << "\n";
            if (count >= depth) break;
            size_t sz = q.size();
            // std::cout << "size: " << sz << "\n";
            for (int i = 0; i < sz; i++) {
                node* cur = q.front();
                // q.pop();
                // q.show_queue();
                if (cur == nullptr) {
                    q.emplace(nullptr);
                    rec[count].emplace_back(nullptr);
                    // q.show_queue();
                } else {
                    // std::cout << cur->val << ", ";
                    size_t num_child = cur->children.size();
                    // std::cout << "num child: " << num_child << "\n";
                    for (auto&& [val, child] : cur->children) {
                        // std::cout << "val: " << val << "\n";
                        q.emplace(child);
                        rec[count].emplace_back(child);
                        // q.show_queue();
                        // flag = true;
                    }
                    if (num_child == 0) {
                        // std::cout << "no child\n";
                        q.emplace(nullptr);
                        rec[count].emplace_back(nullptr);
                        // q.show_queue();
                    }
                }
                q.pop();  // genius debug trial! 24.12.30
                // q.show_queue();
            }
            count++;
            // std::cout << "\n";
            // if (flag == false) break;
        }
        // q.show_queue();
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
#ifdef __UNIT_TEST_TRIE__
#ifndef __UNIT_TEST_TRIE_BRIEF__
        for (int w = 0; w < width; w++) {
            std::cout << "-";
        }
        std::cout << "\n";
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
        for (int w = 0; w < width; w++) {
            std::cout << "-";
        }
        std::cout << "\n";
#endif  // __UNIT_TEST_TRIE_BRIEF__
#endif  // __UNIT_TEST_TRIE__
        // rec.clear();
        // cache.clear();
        // rec.~vector();
        // cache.~vector();
        // q.~queue();
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
    std::cout << "\n------- Test: use mfwu::trie -------\n";
    std::cout << "constructing & inserting & erasing\n";
    using data_type = mfwu::vector<mfwu::string<char>>;
    data_type strs = {
        "aaa", "aab", "aabc", "bcd", 
        "acd", "e", "acd", "ace", "aba"
    };
    using trie = mfwu::trie<mfwu::string<char>>;
    trie data1(strs.begin(), strs.end());
    print_trie(data1);

    trie data2;
    print_trie(data2);
    data2.insert("niganma");
    print_trie(data2);
    data2.insert("ninini");
    data2.insert("aiyo");
    data2.insert("yoo~");
    data2.insert("niganshenmo");
    print_trie(data2);

    trie data3 = data1;
    print_trie(data3);
    data3.erase("aaa");
    print_trie(data3);
    data3.erase("zzz");
    data3.insert("aba");
    data3.insert("abdd");
    print_trie(data3);
    std::cout << "counting & getting res with prefix\n";
    std::cout << "count 'abd': " << data3.count("aba") << "\n";
    std::cout << "count 'a' as prefix: " << data3.count_pref("a") << "\n";
    data_type res = data3.get_pref_with("a");
    // for (auto str: res) {
    //     std::cout << str << "\n";
    // }
    trie data4(mfwu::move(data1));
    data4 = trie(res.begin(), res.end());
    print_trie(data4);

    return 0;
}
// TODO: fully ut 1.1/25

}  // endof namespace mfwu
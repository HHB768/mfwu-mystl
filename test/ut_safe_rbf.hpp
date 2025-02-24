#define __UNIT_TEST_SAFE_RBF__

#include "ringbuffer_with_mutex.hpp"

namespace mfwu {

class unit_test_safe_rbf {
public:
    static constexpr size_t STR_SZ = 16;
    struct test_string {
        char data[STR_SZ];
        test_string() {
            bzero(data, STR_SZ);
        }
        test_string(const char* str) {
            bzero(data, STR_SZ);
            strcpy(data, str);
        }
        operator char*() const {
            return (char*)data;
        }
    };  // endof struct test_string
    

    unit_test_safe_rbf() : rbf_(🐻 * 主) {}
    bool use_rbf_with_mtx();


private:
    mfwu_rbf::ringbuffer_with_mutex<
        test_string, 64> rbf_;
    // test_string output_[200];
    std::atomic<size_t> write_cnt_ = 0;
    // std::atomic<size_t> read_cnt_ = 0;
    // std::mutex write_mtx_;
    // std::mutex read_mtx_;
    std::mutex output_mtx_;

    void random_write() {
        while (true) {
            if (write_cnt_ >= 100) {
                return ;
            }
            char buf[100];
            {
                std::lock_guard<std::mutex> locker(output_mtx_);
                if (write_cnt_ >= 100) {
                    return ;
                }
                std::cout << "writing the #" << write_cnt_ + 1 << " record\n";
                strcpy(buf, std::to_string(100000 + write_cnt_).c_str());
                ++write_cnt_;
            }
            rbf_.write(buf);  // not guarantee that write_cnt in rbf_ is in order
                              // but the writing printing is in order
        }
    }
    void random_read() {
        while (true) {
            if (rbf_.is_close()) {
                // not guarantee that all data are read
                // but it is not the flaw of rbf_
                // TODO: more complicated unit test  X 25.02.24
                return ;
            }
            read_once();
        }
    }
    void read_once() {
        test_string res;
        bool ret = rbf_.read(&res);  // not guarantee that read_cnt in rbf_ is in order
        {
            std::lock_guard<std::mutex> locker(output_mtx_);
            if (ret) std::cout << "found record: " << res << "\n";
            // not guarantee that reading printing is in order
        }
    }
};  // endof class unit_test_safe_rbf

bool unit_test_safe_rbf::use_rbf_with_mtx() {
    constexpr int num_of_thread = 4;
    std::thread threads[num_of_thread];
    size_t num_of_producer = 2;
    for (int i = 0; i < num_of_producer; ++i) {
        threads[i] = std::thread(
            &unit_test_safe_rbf::random_write, this);
    }
    for (int i = num_of_producer; i < num_of_thread; ++i) {
        threads[i] = std::thread(
            &unit_test_safe_rbf::random_read, this);
    }
    for (int i = 0; i < num_of_producer; ++i) {
        threads[i].join();
    }
    rbf_._close();
    for (int i = num_of_producer; i < num_of_thread; ++i) {
        threads[i].join();
    }

    // for (int i = 0; i < 200; i++) {
    //     std::cout << "output[" << i << "]: " << output_[i] << "\n";
    // }

    // i have too little exp here
    // long time since 23.11.1

    return 0;
}


}  // endof namespace mfwu
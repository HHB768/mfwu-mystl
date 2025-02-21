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
    test_string output_[100];
    std::atomic<size_t> write_cnt_ = 0;
    std::atomic<size_t> read_cnt_ = 0;
    // std::mutex write_mtx_;
    std::mutex read_mtx_;
    std::mutex output_mtx_;

    void random_write() {
        while (true) {
            if (rbf_.is_close() || write_cnt_ >= 100) {
                rbf_._close();
                std::cout << "wj\n";
                return ;
            }
            char buf[100];
            {
                std::lock_guard<std::mutex> locker(output_mtx_);
                if (rbf_.is_close() || write_cnt_ >= 100) {
                    rbf_._close();
                    std::cout << "wj\n";
                    return ;
                }
                std::cout << "writing the #" << write_cnt_ + 1 << " record\n";
                strcpy(buf, std::to_string(100000 + write_cnt_).c_str());
                ++write_cnt_;
                
            }
            rbf_.write(buf);
        }
    }
    void random_read() {
        while (true) {
            if (rbf_.is_close()) {
                while (!rbf_.empty()) {
                    // rbf_.flush();  // ...
                    read_once();
                }
                return ;
            }
            read_once();
        }
    }
    void read_once() {
        test_string* res;
        {
            std::lock_guard<std::mutex> locker(read_mtx_);
            res = output_ + read_cnt_;
            ++read_cnt_;
            rbf_.read(res);
        }
        
        {
            std::lock_guard<std::mutex> locker(output_mtx_);
            std::cout << "found record: " << *res << "\n";
        }
    }
};  // endof class unit_test_safe_rbf

bool unit_test_safe_rbf::use_rbf_with_mtx() {
    std::thread threads[10];
    for (int i = 0; i < 2; ++i) {
        threads[i] = std::thread(
            &unit_test_safe_rbf::random_write, this);
    }
    for (int i = 2; i < 10; ++i) {
        threads[i] = std::thread(
            &unit_test_safe_rbf::random_read, this);
    }
    for (int i = 0; i < 10; ++i) {
        threads[i].join();
    }

    // i have too little exp here
    // long time since 23.11.1

    return 0;
}


}  // endof namespace mfwu
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include "ringbuffer_with_mutex.hpp"

std::atomic<int> counter(1);  // 共享计数器
std::mutex counter_mtx;       // 计数器互斥锁

void producer(mfwu_rbf::ringbuffer_with_mutex<int>& rbf, int num_items) {
    for (int i = 0; i < num_items; ++i) {
        int value;
        {
            std::lock_guard<std::mutex> lock(counter_mtx);
            value = counter++;
        }
        rbf.write(value);
    }
}

void consumer(mfwu_rbf::ringbuffer_with_mutex<int>& rbf, int num_items) {
    for (int i = 0; i < num_items; ++i) {
        int value;
        rbf.read(&value);
        std::cout << value << std::endl;
    }
}

int main() {
    const int buffer_size = 50;
    const int num_producers = 5;
    const int num_consumers = 5;
    const int items_per_producer = 10;

    mfwu_rbf::ringbuffer_with_mutex<int> rbf(buffer_size);

    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 创建生产者线程
    for (int i = 0; i < num_producers; ++i) {
        producers.emplace_back(producer, std::ref(rbf), items_per_producer);
    }

    // 创建消费者线程
    for (int i = 0; i < num_consumers; ++i) {
        consumers.emplace_back(consumer, std::ref(rbf), items_per_producer);
    }

    // 等待所有生产者线程完成
    for (auto& t : producers) {
        t.join();
    }

    // 关闭环形缓冲区以通知消费者线程停止等待
    rbf._close();

    // 等待所有消费者线程完成
    for (auto& t : consumers) {
        t.join();
    }

    return 0;
}

// TODO: copilot好像搞不懂怎样才能同步，可能脑内模拟运行还是不行吧
// TODO: 回来把他补全  25.02.23
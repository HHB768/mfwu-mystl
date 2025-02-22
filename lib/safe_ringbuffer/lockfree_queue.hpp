#ifndef __LOCK_FREE_QUEUE_HPP__
#define __LOCK_FREE_QUEUE_HPP__

// source: TODO: WRBSITE HERE

#include <atomic>
#include <cstddef>

namespace mfwu_rbf {

template <typename T>
class LockFreeQueue {
public:
    LockFreeQueue() = delete;
    explicit LockFreeQueue(size_t capacity) {
        _mask = capacity - 1;
        for (size_t i = 1; i < sizeof(void*) * 4; i <<= 1) {
            _mask |= _mask >> i;
        }
        _capacity = _mask + 1;
        // == roundup22 in common.hpp

        _queue = (Node*)new char[sizeof(Node) * _capacity];
        for (size_t i = 0; i < _capacity; i++) {
            _queue[i].tail.store(i, std::memory_order_relaxed);
            _queue[i].head.store(-1, std::memory_order_relaxed);
        }

        _tail.store(0, std::memory_order_relaxed);
        _head.store(0, std::memory_order_relaxed);
    }

    ~LockFreeQueue() {
        for (size_t i = _head; i != tail; ++i) {
            (&_queue[i & _mask].data)->~T();
        }
        delete [] (char*)_queue;  // why char* here?
    }

    size_t capacity() const { return _capacity; }

    size_t size() const {
        size_t head = _head.load(std::memory_order_acquire);
        return _tail.load(std::memory_order_relaxed) - head;
    }

    bool push(const T& data) {
        Node* node;
        size_t tail = _tail.load(std::memory_order_relaxed);
        for (;;) {
            node = &_queue[tail & _mask];
            if (node->tail.load(std::memory_order_relaxed) != tail) {
                return false;
            }
            if ((_tail.compare_exchange_weak(
                tail, tail + 1, std::memory_order_relaxed))) {
                break;
            }
            new (&node->data)T(data);
            node->head.store(tail, std::memory_order_release);
            return true;
        }
    }

    bool pop(T& result) {
        Node* node;
        size_t head = _head.load(std::memory_order_relaxed);
        for (;;) {
            node = &_queue[head & _mask];
            if (node->head.load(std::memory_order_relaxed)) {
                return false;
            }
            if (_head.compare_exchange_weak(
                head, head + 1, std::memory_order_relaxed)) {
                break;
            }
            result = node->data;
            (&node->data)->~T();
            node->tail.store(head + _capacity, std::memory_order_release);
            return true;
        }
    }

private:
    struct Node {
        T data;
        std::atomic<size_t> tail;
        std::atomic<size_t> head;
    };  // endof struct Node

    size_t _mask;
    Node* _queue;
    size_t _capacity;
    char cacheLinePad1[64];
    std::atomic<size_t> _tail;
    char cacheLinePad2[64];
    std::atomic<size_t> _head;
    char cacheLinePad3[64];
};  // endof class LockFreeQueue

}  // endof namespace mfwu_rbf




#endif  // __LOCK_FREE_QUEUE_HPP__
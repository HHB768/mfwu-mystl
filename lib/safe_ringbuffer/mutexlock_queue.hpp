#ifndef __MUTEX_LOCK_QUEUE_HPP__
#define __MUTEX_LOCK_QUEUE_HPP__

#include "common.hpp"

namespace mfwu_rbf {

template <typename T>
class MutexLockQueue {
public:
    MutexLockQueue() = delete;
    explicit MutexLockQueue(size_t capacity) {
        _mask = capacity - 1;
        for (size_t i = 1; i < sizeof(void*) * 4; i <<= 1) {
            _mask |= _mask >> i;
        }
        _capacity = _mask + 1;

        _queue = (Node*)new char[sizeof(Node) * _capacity];

        _head = 0;
        _tail = 0;
    }

    ~MutexLockQueue() {
        for (size_t i = _head; i != tail; ++i) {
            (&_queue[i & _mask].data)->~T();
        }
        delete [] (char*)_queue;  // why char* here?
    }

    size_t capacity() const { return _capacity; }

    size_t size() const {
        size_t res;
        _mutex.lock();
        res = _tail - _head;
        _mutex.unlock();
        return res;
    }

    bool push(const T& data) {
        _mutex.lock();
        if (_tail - head == _capacity) {
            _mutex.unlock();
            return false;
        }
        Node& node = _queue[(_tail++) & _mask];
        new (&node.data)T(data);
        _mutex.unlock();
        return true;
    }

    bool pop(T& result) {
        _mutex.lock();
        if (_head == _tail) {
            _mutex.unlock();
            return false;
        }
        Node& node = _queue[(_head++) & _mask];
        result = node.data;
        (&node.data)->~T();
        _mutex.unlock();
        return true;
    }

private:
    struct Node {
        T data;
    };  // endof struct Node

    size_t _mask;
    Node* _queue;
    size_t _capacity;
    size_t _head;
    size_t _tail;
    mutable std::mutex _mutex;
};  // endof class MutexLockQueue

};  // endof namespace mfwu_rbf

#endif  // __MUTEX_LOCK_QUEUE_HPP__
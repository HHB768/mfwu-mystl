#ifndef __SPIN_LOCK_QUEUE_HPP__
#define __SPIN_LOCK_QUEUE_HPP__

namespace mfwu_rbf {

template <typename T>
class SpinLockQueue {
public:
    explicit SpinLockQueue(size_t capacity) {
        _mask = capacity - 1;
        for (size_t i = 1; i < sizeof(void*) * 4; i <<= 1) {
            _mask |= _mask >> i;
        }
        _capacity = _mask + 1;

        _queue = (Node*)new char[sizeof(Node) * _capacity];

        _lock = 0;
        _head = 0;
        _tail = 0;
    }

    ~SpinLockQueue() {
        for (size_t i = _head; i != _tail; ++i) {
            (&_queue[i & _mask].data)->~T();
        }
        delete [] (char*)_queue;  // why char* here?
    }

    size_t capacity() const { return _capacity; }

    size_t size() const {
        size_t res;
        while (__sync_lock_test_and_set(&_lock, 1)) {}
        res = _tail - _head;
        __sync_lock_release(&_lock);
        return res;
    }

    bool push(const T& data) {
        while (__sync_lock_test_and_set(&_lock, 1)) {}
        if (_tail - _head == _capacity) {
            __sync_lock_release(&_lock);
            return false;
        }
        Node& node = _queue[(_tail++) & _mask];
        new (&node.data)T(data);
        __sync_lock_release(&_lock);
        return true;
    }

    bool pop(T& result) {
        while (__sync_lock_test_and_set(&_lock, 1)) {}
        if (_head == _tail) {
            __sync_lock_release(&_lock);
            return false;
        }
        Node& node = _queue[(_head++) & _mask];
        result = node.data;
        (&node.data)->~T();
        __sync_lock_release(&_lock);
        return true;
    }


private:
    struct Node {
        T data;
    };

    size_t _mask;
    size_t _capacity;
    Node* _queue;

    mutable int32_t _lock;
    size_t _head;
    size_t _tail;
};  // endof class SpinLockQueue

}  // endof namespace mfwu_rbf


#endif  // __SPIN_LOCK_QUEUE_HPP__

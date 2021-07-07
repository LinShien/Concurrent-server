#ifndef _SAFEQUEUE_H_
#define _SAFEQUEUE_H_

#include <mutex>
#include <queue>

// Thread safe implementation of a task queue for threadpool

template <typename T>
class SafeQueue {
    public:
        SafeQueue() {}
        ~SafeQueue() {}

        bool empty();
        int size();

        void enqueue(T& t);
        bool dequeue(T& t);

    private:
        std::mutex _mutex;
        std::queue<T> _queue;
};

template <typename T>
bool SafeQueue<T>::empty() {
    std::unique_lock<std::mutex> lock(_mutex);

    return _queue.empty();
}

template <typename T>
int SafeQueue<T>::size() {
    std::unique_lock<std::mutex> lock(_mutex);

    return _queue.size();
}

template <typename T>
void SafeQueue<T>::enqueue(T& t) {
    std::unique_lock<std::mutex> lock(_mutex);   // acquire the mutex lock
    _queue.push(t);
}

template <typename T>
bool SafeQueue<T>::dequeue(T& t) {
    std::unique_lock<std::mutex> lock(_mutex);
            
    if (_queue.empty()) {
         return false;
    }

    t = std::move(_queue.front());
    _queue.pop();

    return true;
}

#endif
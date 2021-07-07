#include "ThreadPool.h"
#include "SafeQueue.h"

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <iostream>

/**
 * the inner bracket inside while loop is to prevent the thread lock the _conditional_mutex again 
 * that would block other threads from waking up by ThreadPool manager
 */
void ThreadPool::ThreadWorker::operator()() {
    std::function<void()> func;
    bool dequeued;

    while (!_pool->_shutdown) {
        {                                                   
            std::unique_lock<std::mutex> lock(_pool->_conditional_mutex);  // crtical section

            if (_pool->_queue.empty()) {
               _pool->_conditional_lock.wait(lock);                 // blocked until threadpool notify
            }

            dequeued = _pool->_queue.dequeue(func);
        }

        if (dequeued) {
            func();
        }
    }
}

void ThreadPool::init() {
    for (unsigned int i = 0; i < _threads.size(); ++i) {
        _threads[i] = std::thread(ThreadWorker(this, i));
        std::cout << "Thread: " << _threads[i].get_id() << " is created in ThreadPool." << std::endl;
    }
}

/**
 *  Waits until threads finish their current task and shutdowns the pool
 */
void ThreadPool::shutdown() {
    _shutdown = true;
    _conditional_lock.notify_all();     // wake up all the thread workers

    for (unsigned int i = 0; i < _threads.size(); ++i) {
        if (_threads[i].joinable()) {        // check whether the thread can be shutdowned
            _threads[i].join();
        }
    }
}

void ThreadPool::print_info() {
    std::cout << "ThreadPool size: " << _threads.size() << std::endl;
}
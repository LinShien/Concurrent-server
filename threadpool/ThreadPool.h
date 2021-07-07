#include "SafeQueue.h"
#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool {
    public:
        ThreadPool(int num_threads): _threads(std::vector<std::thread>(num_threads)), _shutdown(false) {};
        ~ThreadPool() {};

        ThreadPool(const ThreadPool& threadpool) = delete;               // copy constructor not allowed
        ThreadPool(ThreadPool&& threadpool) = delete;                    // move constructor not allowed
        ThreadPool& operator=(const ThreadPool& threadpool) = delete;    // copy assignment is not allowed
        ThreadPool& operator=(ThreadPool&& threadpool) = delete;         // move assignment is not allowed

        void init();
        void shutdown();
        void print_info();

        template<typename F, typename...Args>
        decltype(auto) submit(F&& f, Args&&... args);

    private:
        bool _shutdown;
        SafeQueue<std::function<void()>> _queue;
        std::vector<std::thread> _threads;
        std::mutex _conditional_mutex;
        std::condition_variable _conditional_lock;

        class ThreadWorker {
            public:
                ThreadWorker(ThreadPool* pool, const int id): _pool(pool), _id(id) {};
                void operator()();
                
            private:
                int _id;
                ThreadPool* _pool;
        };
};

template<typename F, typename...Args>
decltype(auto) ThreadPool::submit(F&& f, Args&&... args) {
    std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

    auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

    std::function<void()> wrapper_func = [task_ptr]() {
        (*task_ptr)();
    };

    _queue.enqueue(wrapper_func);
    _conditional_lock.notify_one();

    return task_ptr->get_future();
}

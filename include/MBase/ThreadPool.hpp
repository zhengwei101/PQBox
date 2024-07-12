#pragma once

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <atomic>
#include <queue>
#include <vector>
#include <future>
#include <stdexcept>

namespace MBase {

class ThreadPool
{
public:
    ThreadPool(size_t size = 0)
    {
        int nThread = size;
        if (nThread <= 0) {
            nThread = std::thread::hardware_concurrency();
            nThread = (nThread <= 0 ? 2 : nThread);
        }
        addThread(nThread);
    }

    ~ThreadPool()
    {
        stop_ = true;

        // stop all threads
        task_cv_.notify_all();

        // join them
        for (auto& t : pool_) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

    // Two ways to invoking class member function:
    // use bind, enqueue(std::bind(&Dog::sayHello, &dog));
    // use mem_fn, enqueue(std::mem_fn(&Dog::sayHello), this);

    // 入队任务(传入函数和函数的参数)
    template <class F, class... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using return_type = decltype(f(args...));

        // 将任务函数和其参数绑定，构建一个packaged_task
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        //std::future用来访问异步操作的结果
        std::future<return_type> future = task->get_future();

        { // 添加任务到队列
            std::lock_guard<std::mutex> lock{mutex_};

            // 不允许入队到已经停止的线程池
            // don't allow enqueueing after stopping the pool
            if (stop_) {
                throw std::runtime_error("commit on ThreadPool is stopped.");
            }

            // add the task
            tasks_.emplace([task]() { (*task)(); });
        }

        //发送通知，唤醒某一个工作线程取执行任务
        // wake up one thread
        task_cv_.notify_one();

        return future;
    }

    void addThread(size_t size)
    {
        for (size_t i = 0; i < size; ++i) {
            pool_.emplace_back([this] {
                while (!stop_) {
                    std::function<void()> task; // 获取一个待执行的 task

                    {
                        // acquire lock
                        std::unique_lock<std::mutex> lock{mutex_};

                        // 等待条件成立
                        // 只有在tasks_不为空的情况下才会返回true,防止虚假唤醒
                        task_cv_.wait(
                            lock, [this] { return stop_ || !tasks_.empty(); }); // wait 直到有 task

                        // 如果线程池停止且任务队列为空，结束返回
                        // exit if the pool is stopped
                        if (stop_ && tasks_.empty()) {
                            return;
                        }

                        // 取得任务队首任务(注意此处的std::move)
                        // get the task from the queue
                        task = std::move(tasks_.front());

                        // 从队列移除
                        tasks_.pop();
                    }

                    //执行任务
                    // execute the task
                    task();
                }
            });
        }
    }

private:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    // need to keep track of threads so we can join them
    std::vector<std::thread> pool_;

    // the task queue
    std::queue<std::function<void()>> tasks_;

    // synchronization
    std::mutex mutex_;
    std::condition_variable task_cv_; // 条件变量

    // 停止标志
    std::atomic<bool> stop_{false};
};

} // namespace MBase

#endif
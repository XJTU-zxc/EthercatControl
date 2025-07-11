#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>


/* 线程安全队列 */
template <typename T>
class ThreadSafeQueue {
private:
    std::queue<T> queue;
    mutable  std::mutex mutex;
    std::condition_variable cond;
    std::condition_variable full_cond; // 条件变量，用于队列满时等待
    size_t max_size; // 队列的最大长度

public:
    // 构造函数，初始化最大长度
    ThreadSafeQueue(size_t max_size) : max_size(max_size) {}

    void push(T value) {
        std::unique_lock<std::mutex> lock(mutex);
        // 等待队列非满
        full_cond.wait(lock, [this]{ return queue.size() < max_size; });
        queue.push(value);
        cond.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mutex);
        // 等待队列非空
        cond.wait(lock, [this]{ return !queue.empty(); });
        T value = queue.front();
        queue.pop();
        // 通知可能等待的push操作
        full_cond.notify_one();
        return value;
    }

    // 检查队列是否为空
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

    // 获取队列当前长度
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.size();
    }
};


typedef struct RESPMessage
{
    /* data */
    int code;

    int datalen;

    char data[80];

};


#endif // THREAD_SAFE_QUEUE_H
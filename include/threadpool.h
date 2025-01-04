#pragma once
#include "result.h"
#include "task.h"
#include "thread.h"
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <unordered_map>
#include <vector>
namespace ThreadPool
{

enum class PoolMode
{
    MODE_FIXED,  // 固定线程数量
    MODE_CACHED, // 线程数量可动态增长
};

class ThreadPool
{
  public:
    ThreadPool();
    ~ThreadPool();
    void start(size_t initThreadSize = std::thread::hardware_concurrency());
    void setMode(PoolMode mode);
    void setThreadMaxThreshHold(size_t threshHold);
    void setTaskQueMaxThreshHold(size_t threshHold);
    Result submitTask(std::shared_ptr<Task> sp);
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;

  private:
    // 定义线程执行函数
    void threadFunc(uint16_t threadId);

    // 检查pool的运行状态
    bool checkRunningState();

  private:
    std::unordered_map<uint16_t, std::unique_ptr<Thread>> threads_;
    size_t initThreadSize_;               // 初始线程数量
    std::atomic_uint16_t curThreadSize_;  // 当前线程池的线程总数量
    std::atomic_uint16_t idleThreadSize_; // 空闲线程数量
    size_t threadSizeThreshHold_;         // 线程数量上限

    std::queue<std::shared_ptr<Task>> taskQue_; // 任务队列
    std::atomic_uint taskSize_;                 // 任务数量
    size_t taskQueMaxThreshHold_;               // 任务数量上限

    std::mutex taskQueMtx_;            // 任务队列互斥锁
    std::condition_variable notEmpty_; // 任务队列不空(可以消费)的条件变量
    std::condition_variable notFull_;  // 任务队列不满(可以生产)的条件变量
    std::condition_variable notExit_;  // 线程退出条件变量

    PoolMode poolMode_;          // 线程池工作模式
    std::atomic_bool isRunning_; // 线程池是否运行
};

} // namespace ThreadPool

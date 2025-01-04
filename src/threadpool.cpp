
#include "threadpool.h"

namespace ThreadPool
{

const int THREAD_MAX_THRESHHOLD = 1024;
const int TASK_MAX_THRESHHOLD = 100;
const int THREAD_MAX_IDLE_TIMEOUT = 60;
// 线程池构造函数
ThreadPool::ThreadPool()
    : initThreadSize_(0), taskSize_(0), taskQueMaxThreshHold_(TASK_MAX_THRESHHOLD), poolMode_(PoolMode::MODE_FIXED),
      isRunning_(false), curThreadSize_(0), idleThreadSize_(0), threadSizeThreshHold_(THREAD_MAX_THRESHHOLD)
{
}

// 线程池析构函数
ThreadPool::~ThreadPool()
{
    this->isRunning_ = false;
    std::cout << "线程池析构" << std::endl;
    // 唤醒所有线程，有些线程卡在了线程函数的条件变量上（等待任务队列不为空）
    // notEmpty_.notify_all();

    // 析构的死锁问题
    // 1.析构函数先拿到锁，会提前唤醒notempty，但是线程函数还等在锁外，当等到阻塞的线程函数拿到锁时，会一直收不到notempty信号，导致线程无法退出
    // 此时需要在线程函数拿到锁后，二次判断isRunning_，如果为false，就退出线程
    // 2.线程函数先拿到锁，此时会等待notempty通知，所有notempty的通知要放在析构函数拿到锁后。这样线程函数就能收到通知，退出线程

    std::unique_lock<std::mutex> lock(taskQueMtx_);
    notEmpty_.notify_all(); // 通知应该放在这里，防止线程池析构时，线程还在等待任务队列不为空
    notExit_.wait(lock, [this]() -> bool { return this->threads_.size() == 0; });
}

// 启动线程池
void ThreadPool::start(size_t initThreadSize)
{
    initThreadSize_ = initThreadSize;
    isRunning_ = true;

    // 创建线程对象
    for (size_t i = 0; i < initThreadSize_; ++i)
    {
        auto ptr = std::make_unique<Thread>([this](uint16_t thread_id) { this->threadFunc(thread_id); });
        threads_[ptr->getThreadId()] = std::move(ptr);
    }

    for (size_t i = 0; i < initThreadSize_; ++i)
    {
        // 启动线程
        threads_[i]->start();
        idleThreadSize_++;
        curThreadSize_++;
    }
}

// 设置线程池模式
void ThreadPool::setMode(PoolMode mode)
{
    if (checkRunningState())
    {
        return;
    }
    poolMode_ = mode;
}

void ThreadPool::setThreadMaxThreshHold(size_t threshHold)
{
    if (checkRunningState())
    {
        return;
    }
    if (poolMode_ == PoolMode::MODE_CACHED)
    {
        threadSizeThreshHold_ = threshHold;
    }
}

// 设置任务队列上限
void ThreadPool::setTaskQueMaxThreshHold(size_t threshHold)
{
    if (poolMode_ == PoolMode::MODE_FIXED)
    {
        return;
    }
    else
    {
        taskQueMaxThreshHold_ = threshHold;
    }
}

// 提交任务
Result ThreadPool::submitTask(std::shared_ptr<Task> sp)
{
    // 获取锁
    std::unique_lock<std::mutex> lock(taskQueMtx_);
    // 判断任务队列是否满

    // 任务队列满，notFull_ 需要等待
    using namespace std::chrono_literals;
    if (!notFull_.wait_for(lock, 1s, [this]() -> bool { return this->taskQue_.size() < this->taskQueMaxThreshHold_; }))
    {
        // 条件依然没有满足，超时
        std::cout << "任务队列满了，提交任务失败" << std::endl;
        return Result(sp, false);
    }

    // 任务队列不满，可以放入任务
    taskQue_.emplace(sp);
    taskSize_++;
    // notEmpty_ 唤醒消费
    notEmpty_.notify_all();

    // 需要根据任务数量和空闲线程数量，判断是否要创建新的线程
    // cache: 适合处理时间较短的任务，否则线程会越来越多
    if (poolMode_ == PoolMode::MODE_CACHED && taskSize_ > idleThreadSize_ && curThreadSize_ < threadSizeThreshHold_)
    {
        // 创建新线程
        auto ptr = std::make_unique<Thread>([this](uint16_t thread_id) { this->threadFunc(thread_id); });
        auto threadId = ptr->getThreadId();
        threads_.emplace(threadId, std::move(ptr));
        // 启动线程
        threads_[threadId]->start();
        std::cout << "创建新线程: " << threadId << std::endl;

        // 修改线程个数相关变量
        idleThreadSize_++;
        curThreadSize_++;
    }

    return Result(sp);
}

void ThreadPool::threadFunc(uint16_t threadId)
{
    std::cout << "threadId: " << threadId << " come!" << std::endl;

    auto lastTime = std::chrono::high_resolution_clock::now();
    // 等到所有任务都执行完，线程池才能回收所有线程资源
    while (true)
    {
        // 获取锁
        std::unique_lock<std::mutex> lock(taskQueMtx_);
        using namespace std::chrono_literals;

        // 锁 + 双重判断
        while (taskQue_.empty())
        {
            // 线程池要结束，回收线程资源
            if (!isRunning_)
            {
                std::cout << "threadId: " << threadId << " exit!!!!!!!!!!!!" << std::endl;
                threads_.erase(threadId);
                curThreadSize_--;
                std::cout << "rest thread size: " << threads_.size() << std::endl;
                notExit_.notify_all();
                return;
            }
            // cache模式下，如果空闲事件超过60s，将多余线程退出回收
            if (poolMode_ == PoolMode::MODE_CACHED)
            {
                // 每1s检查一次
                if (notEmpty_.wait_for(lock, 1s) == std::cv_status::timeout)
                {
                    // 超时返回
                    auto now = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
                    if (duration.count() >= THREAD_MAX_IDLE_TIMEOUT && curThreadSize_ > initThreadSize_)
                    {
                        // 线程销毁
                        threads_.erase(threadId);
                        curThreadSize_--;
                        idleThreadSize_--;
                        std::cout << "threadId: " << threadId << " timeout exit!" << std::endl;
                        return;
                    }
                }
                std::cout << "threadId: " << threadId << " waiting..." << std::endl;
            }
            else
            {
                notEmpty_.wait(lock);
            }

            // 线程退出销毁
            // if (isRunning_ == false)
            // {
            //     threads_.erase(threadId);
            //     std::cout << "threadId: " << threadId << " exit!" << std::endl;

            //     notExit_.notify_all();
            //     return;
            // }
        }
        // 消费任务
        idleThreadSize_--;
        // 从任务队列中获取任务
        std::shared_ptr<Task> task = taskQue_.front();
        taskQue_.pop();
        taskSize_--;
        // 如果依然有剩余任务，继续唤醒其他线程进行消费
        if (taskQue_.size() > 0)
        {
            notEmpty_.notify_all();
        }

        // notFull_ 唤醒生产，用户得以提交新任务
        notFull_.notify_all();
        lock.unlock(); // 解锁

        // 处理任务
        if (task != nullptr)
        {
            // task->run(); // 执行任务;把任务的返回值给到result
            task->exec();
            std::cout << "threadId: " << threadId << " exec task!" << std::endl;
        }
        // 任务执行完毕，空闲线程数量加1
        idleThreadSize_++;
        // 线程空闲时间重新计时
        lastTime = std::chrono::high_resolution_clock::now();
        std::cout << "isRunning" << std::boolalpha << isRunning_ << std::endl;
    }
    std::cout << "threadId: " << threadId << " exit!!!!!!!!!!!!" << std::endl;
    threads_.erase(threadId);
    curThreadSize_--;
    std::cout << "rest thread size: " << threads_.size() << std::endl;
    notExit_.notify_all();
}

bool ThreadPool::checkRunningState()
{
    return isRunning_;
}

} // namespace ThreadPool

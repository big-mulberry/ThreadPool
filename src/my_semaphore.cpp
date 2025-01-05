#include "my_semaphore.h"

namespace ThreadPool
{
MySemaphore::MySemaphore(int resource_limit) : resourceLimit_(resource_limit), isRunning_(true)
{
}
MySemaphore::~MySemaphore()
{
    isRunning_ = false;
}
void MySemaphore::wait()
{
    if (!isRunning_)
    {
        // result对象已经析构，不再在意结果
        return;
    }
    std::unique_lock<std::mutex> locker(mtx_);
    cv_.wait(locker, [this]() -> bool { return resourceLimit_ > 0; });
    resourceLimit_--;
}
void MySemaphore::post()
{
    if (!isRunning_)
    {
        return;
    }
    std::unique_lock<std::mutex> locker(mtx_);
    resourceLimit_++;
    cv_.notify_all();
}
} // namespace ThreadPool

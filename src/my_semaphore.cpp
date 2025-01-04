#include "my_semaphore.h"

namespace ThreadPool
{
MySemaphore::MySemaphore(int resource_limit) : resourceLimit_(resource_limit)
{
}
void MySemaphore::wait()
{
    std::unique_lock<std::mutex> locker(mtx_);
    cv_.wait(locker, [this]() -> bool { return resourceLimit_ > 0; });
    resourceLimit_--;
}
void MySemaphore::post()
{
    std::unique_lock<std::mutex> locker(mtx_);
    resourceLimit_++;
    cv_.notify_all();
}
} // namespace ThreadPool

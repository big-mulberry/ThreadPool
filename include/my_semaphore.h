#pragma once
#include <condition_variable>
#include <mutex>
namespace ThreadPool
{
class MySemaphore
{
  public:
    MySemaphore(int resource_limit = 0);
    ~MySemaphore() = default;

    //* 获取信号量资源
    void wait();
    //* 增加信号量资源
    void post();

  private:
    // 资源计数
    int resourceLimit_;
    std::mutex mtx_;
    std::condition_variable cv_;
};
} // namespace ThreadPool

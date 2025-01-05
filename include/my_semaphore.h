#pragma once
#include <condition_variable>
#include <mutex>
namespace ThreadPool
{
class MySemaphore
{
  public:
    MySemaphore(int resource_limit = 0);

    // 注意析构函数
    // result指针会传给task
    // result对象析构时，task对象还在，并在另一线程中调用result的setValue方法
    // 伴随result对象析构的MySemaphore中的条件变量的wait和notify会导致未定义的结果
    ~MySemaphore();

    //* 获取信号量资源
    void wait();
    //* 增加信号量资源
    void post();

  private:
    // 资源计数
    int resourceLimit_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool isRunning_;
};
} // namespace ThreadPool

#pragma once
#include "my_any.h"
#include "my_semaphore.h"
#include "task.h"
#include <atomic>
#include <memory>
namespace ThreadPool
{
class Result
{
  public:
    Result(std::shared_ptr<Task> task, bool isValid = true);
    ~Result() = default;

    // 问题：setValue()方法，result怎么获取task run完之后的返回值 并且设置进any_
    void setValue(MyAny any);
    // 问题：get()方法，用户获取时需要阻塞(信号量)
    MyAny get();

  private:
    MyAny any_;
    MySemaphore sem_;
    std::shared_ptr<Task> task_; // 指向对应获取返回值的任务对象
    std::atomic_bool isValid_;   // 返回值是否有效
};
} // namespace ThreadPool

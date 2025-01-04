
#pragma once
#include "my_any.h"
#include <memory>
namespace ThreadPool
{
class Result;

// 任务抽象基类
class Task
{
  public:
    Task();
    virtual ~Task() = default;
    virtual MyAny run() = 0;
    void exec();
    void setResult(Result *result);

  private:
    Result *result_;
};
} // namespace ThreadPool

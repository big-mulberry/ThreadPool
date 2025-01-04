#pragma once
#include <functional>
#include <iostream>
#include <thread>
namespace ThreadPool
{

class Thread
{
  public:
    using ThreadCallback = std::function<void(uint16_t)>;
    Thread(const ThreadCallback &cb);
    ~Thread();
    void start();
    uint16_t getThreadId() const;

  private:
    ThreadCallback thread_cb_;
    static uint16_t generateThreadId_;
    uint16_t threadId_;
};
} // namespace ThreadPool
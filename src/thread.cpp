#include "thread.h"
namespace ThreadPool
{

uint16_t Thread::generateThreadId_ = 0;
Thread::Thread(const ThreadCallback &cb) : thread_cb_(cb), threadId_(generateThreadId_++)
{
    std::cout << "创建线程: " << threadId_ << std::endl;
}

Thread::~Thread()
{
}
void Thread::start()
{
    std::thread t(thread_cb_, threadId_);
    t.detach();
    // t.join();
}
uint16_t Thread::getThreadId() const
{
    return threadId_;
}
} // namespace ThreadPool

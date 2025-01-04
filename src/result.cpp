#include "result.h"

namespace ThreadPool
{
Result::Result(std::shared_ptr<Task> task, bool isValid) : task_(task), isValid_(isValid)
{
    task->setResult(this);
}

void Result::setValue(MyAny any)
{
    any_ = std::move(any);
    // 已经获取了任务的返回值，增加信号量资源
    sem_.post();
}

MyAny Result::get()
{
    if (!isValid_)
    {
        return MyAny();
    }
    sem_.wait();
    return std::move(any_);
}

} // namespace ThreadPool

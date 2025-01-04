#include "task.h"
#include "result.h"
namespace ThreadPool
{
Task::Task()
{
}

void Task::exec()
{
    if (result_ != nullptr)
    {
        result_->setValue(std::move(this->run())); // 这里发生多态调用
    }
}

void Task::setResult(Result *result)
{
    this->result_ = result;
}

} // namespace ThreadPool

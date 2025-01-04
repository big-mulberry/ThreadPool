/**
 * @file main.cpp
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-12-29
 *
 * @copyright Copyright (c) 2024
 * 1 建立线程池（容器 + 存放初始化好的线程）
 * 2 提交任务（任务队列 + 条件变量）
 * 3 线程执行任务
 * 4 获取任务结果（task run + result set value + 信号量post ）
 * 5 任务返回值获取（信号量wait + result get value）
 */
#include "my_any.h"
#include "result.h"
#include "task.h"
#include "thread.h"
#include "threadpool.h"
#include <unistd.h>
using ULong = unsigned long long;

class MyTask : public ThreadPool::Task
{
  public:
    MyTask() = default;
    MyTask(ULong start, ULong end) : start_(start), end_(end)
    {
    }
    virtual ThreadPool::MyAny run() override
    {
        ULong sum = 0;
        for (ULong i = start_; i <= end_; ++i)
        {
            sum += i;
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
        return sum;
    }

  private:
    ULong start_;
    ULong end_;
};

int main()
{
    {
        ThreadPool::ThreadPool pool;
        pool.setMode(ThreadPool::PoolMode::MODE_CACHED);
        pool.start(4);

        ThreadPool::Result res1 = pool.submitTask(std::make_shared<MyTask>(1, 100000000));
        // ThreadPool::Result res2 = pool.submitTask(std::make_shared<MyTask>(100000001, 200000000));
        // ThreadPool::Result res3 = pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
        // ThreadPool::Result res4 = pool.submitTask(std::make_shared<MyTask>(200000001, 300000000));
        // ThreadPool::Result res5 = pool.submitTask(std::make_shared<MyTask>(400000001, 500000000));
        // ThreadPool::Result res6 = pool.submitTask(std::make_shared<MyTask>(500000001, 600000000));
        // ThreadPool::Result res7 = pool.submitTask(std::make_shared<MyTask>(500000001, 600000000));
        // ThreadPool::Result res8 = pool.submitTask(std::make_shared<MyTask>(500000001, 600000000));
        // ThreadPool::Result res9 = pool.submitTask(std::make_shared<MyTask>(500000001, 600000000));
        // ThreadPool::Result res10 = pool.submitTask(std::make_shared<MyTask>(500000001, 600000000));
        ULong sum1 = res1.get().myany_cast<ULong>();
        // ULong sum2 = res2.get().myany_cast<ULong>();
        // ULong sum3 = res3.get().myany_cast<ULong>();
        // ULong sum4 = res4.get().myany_cast<ULong>();
        // ULong sum5 = res5.get().myany_cast<ULong>();
        // ULong sum6 = res6.get().myany_cast<ULong>();
        // ULong sum7 = res7.get().myany_cast<ULong>();
        // ULong sum8 = res8.get().myany_cast<ULong>();
        // ULong sum9 = res9.get().myany_cast<ULong>();
        // ULong sum10 = res10.get().myany_cast<ULong>();

        // std::cout << "sum: " << ULong(sum1) << std::endl;
    }
    // getchar();
    return 0;
}
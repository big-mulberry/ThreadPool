#pragma once
#include <any>
#include <memory>
namespace ThreadPool
{
class MyAny
{
  public:
    MyAny() = default;
    ~MyAny() = default;
    MyAny(const MyAny &) = delete;
    MyAny &operator=(const MyAny &) = delete;
    MyAny(MyAny &&) = default;
    MyAny &operator=(MyAny &&) = default;
    // 构造函数
    template <typename T> MyAny(const T &data) : base_(std::make_unique<Derived<T>>(data))
    {
    }

    template <typename T> T myany_cast()
    {
        // 从base找到derive对象
        // 基类指针转为unique_ptr<Derived> RTTI类型识别 dynamic_cast
        if (auto p = dynamic_cast<Derived<T> *>(base_.get()))
        {
            return p->data_;
        }
        else
        {
            throw "type is not match!";
        }
    }

  public:
    // 基类类型
    class Base
    {
      public:
        virtual ~Base() = default;
    };

    template <typename T> class Derived : public Base
    {
      public:
        Derived(const T &data) : data_(data)
        {
        }
        T data_;
    };

  private:
    // 定义基类指针
    std::unique_ptr<Base> base_;
};
} // namespace ThreadPool

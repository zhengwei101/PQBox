#pragma once

#ifndef __SINGLETON_H__
#define __SINGLETON_H__

/*
*Examples:
*--------------------------------------------------------
*   class Test final : public MBase::Singleton<Test>
*   {
*   public:
*       Test()  { cout << "Test Ctor" << endl; }
*       ~Test() { cout << "Test Dtor" << endl; }
*
*       void use() const { cout << " call use" << endl; }
*   };
*
*   auto const& t = Test::GetInstance();
*   t.use();
*
*   {
*       Test::GetInstance().use();
*   }
*--------------------------------------------------------
*/

namespace MBase
{
template <typename T>
class Singleton
{
public:
    static T& GetInstance();
    virtual ~Singleton(){};

    //删除函数应该为public
    //这是因为编译器会在删除状态前检查可访问性，这样可以得到更好的错误提示
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

protected:
    Singleton() {}
};

template <typename T>
T& Singleton<T>::GetInstance()
{
    static T ins{};
    return ins;
}

} // namespace MBase

#endif // __SINGLETON_H__
#include "gtest/gtest.h"

#include "sl651.h"

GTEST_TEST(Definition, package)
{
    Head *head = NewInstance(Head);           // 创建空实例
    Head_ctor(head);                          // 调用构造，Head的构造实际上是个空实现
    (*head).addrPair.downAddr.centerAddr = 0; // 调用方法，或者直接访问，必要的功能可以封装起来，简单赋值直接访问
    (*head).direction = Up;
    Package *pkg = NewInstance(Package); // 同样的步骤，创建空实例
    Package_ctor(pkg, head);             // 构造函数，带有参数
    ASSERT_EQ(Package_Direction(pkg), Up);
    // if var is malloced call this, othewise donot call this.
    Head_dtor(pkg);    // 倒序析构，用来释放实例所 管理 的指针，管理：包括实例在其封装方法中创建的内存指针，或接口中约定的传入的代管内存指针
    Package_dtor(pkg); // 析构
    DelInstance(head); // 释放实例，与析构分开调用；如果不是New，则不需要释放实例
    DelInstance(pkg);  // 同上
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
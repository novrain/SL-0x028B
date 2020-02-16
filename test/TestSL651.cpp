#include "gtest/gtest.h"

#include "sl651.h"

GTEST_TEST(Definition, package)
{
    Package pkg;
    int *ptr;
    if (sizeof(ptr) == 8)
    {
        ASSERT_EQ(sizeof(pkg), 35);
    }
    else if (sizeof(ptr) == 4)
    {
        ASSERT_EQ(sizeof(pkg), 31);
    }
    pkg.head.direction = Up;
    pkg.head.addrPair.downAddr.centerAddr = 0;
    ASSERT_EQ(pkg.head.direction, Up);
    ASSERT_EQ(pkg.head.addrPair.downAddr.centerAddr, 0);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
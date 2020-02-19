#include "gtest/gtest.h"

#include "sl651.h"

GTEST_TEST(Definition, package)
{
    Package pkg;
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
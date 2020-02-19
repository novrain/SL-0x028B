#include "gtest/gtest.h"

#include "sl651.h"

GTEST_TEST(Definition, package)
{
    Package pkg;
    Head head;
    head.addrPair.downAddr.centerAddr = 0;
    head.direction = Up;
    Package_ctor(&pkg, head);
    ASSERT_EQ(Package_Direction(&pkg), Up);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
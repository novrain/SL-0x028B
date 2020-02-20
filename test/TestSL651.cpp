#include "gtest/gtest.h"

#include "sl651.h"

GTEST_TEST(Definition, package)
{
    Package *pkg;
    pkg = (Package *)malloc(sizeof(Package));
    Head *head;
    head = (Head *)malloc(sizeof(head));
    Head_ctor(head); // empty
    (*head).addrPair.downAddr.centerAddr = 0;
    (*head).direction = Up;
    Package_ctor(pkg, head);
    ASSERT_EQ(Package_Direction(pkg), Up);
    // if var is malloced call this, othewise donot call this.
    Head_dtor(pkg);    // free(head);
    Package_dtor(pkg); // free(pkg);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
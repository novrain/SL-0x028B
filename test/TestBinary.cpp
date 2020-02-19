#include "gtest/gtest.h"

#include "binary.h"

GTEST_TEST(Binary, hostEndian)
{
    ASSERT_EQ(LittleEndian, hostEndian());
}

GTEST_TEST(Binary, toBeUInt16)
{
    uint8_t bin[] = {1, 2, 3, 4, 0xab, 0xcd};
    uint16_t v = toBeUInt16(bin);
    ASSERT_EQ(v, (1 << 8) + 2);
}

GTEST_TEST(Binary, toLeUInt16)
{
    uint8_t bin[] = {1, 2, 3, 4, 0xab, 0xcd};
    uint16_t v = toLeUInt16(bin);
    ASSERT_EQ(v, (2 << 8) + 1);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
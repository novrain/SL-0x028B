#include "gtest/gtest.h"

#include "hex.h"

GTEST_TEST(Hex, toBin)
{
    const uint8_t hex[] = "01020304ABcd";
    uint8_t *bin = (uint8_t *)malloc(6);
    memset(bin, 0, 6);
    size_t len = hex2bin(hex, 12, bin, 6);
    ASSERT_EQ(len, 6);
    ASSERT_EQ(bin[0], 1);
    ASSERT_EQ(bin[5], 0xCD);
}

GTEST_TEST(Hex, toUInt8)
{
    const uint8_t hex[] = "01020304ABcd";
    uint8_t ui8 = 0;
    hexToUInt8(hex, &ui8);
    ASSERT_EQ(ui8, 1);
    hexToUInt8(hex + 2, &ui8);
    ASSERT_EQ(ui8, 2);
    hexToUInt8(hex + 1, &ui8);
    ASSERT_EQ(ui8, 0x10);
}

GTEST_TEST(Hex, toBeUInt16)
{
    const uint8_t hex[] = "01020304ABcd";
    uint16_t ui16 = 0;
    hexToBeUInt16(hex, &ui16);
    ASSERT_EQ(ui16, 0x102);
    hexToBeUInt16(hex + 2, &ui16);
    ASSERT_EQ(ui16, 0x203);
    hexToBeUInt16(hex + 1, &ui16);
    ASSERT_EQ(ui16, 0x1020);
}

GTEST_TEST(Hex, toLeUInt16)
{
    const uint8_t hex[] = "01020304ABcd";
    uint16_t ui16 = 0;
    hexToLeUInt16(hex, &ui16);
    ASSERT_EQ(ui16, 0x201);
    hexToLeUInt16(hex + 2, &ui16);
    ASSERT_EQ(ui16, 0x302);
    hexToLeUInt16(hex + 1, &ui16);
    ASSERT_EQ(ui16, 0x2010);
}

GTEST_TEST(Bin, toHex)
{
    char *hex = (char *)malloc(13);
    memset(hex, 0, 13);
    uint8_t bin[] = {1, 2, 3, 4, 0xab, 0xcd};
    size_t len = bin2hex(bin, 6, (uint8_t *)hex, 12);
    ASSERT_EQ(len, 12);
    printf("%d, %s", strlen(hex), hex);
    ASSERT_EQ(hex[0], '0');
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
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

GTEST_TEST(Hex, toLeUInt32)
{
    const uint8_t hex[] = "01020304ABcd";
    uint32_t ui32 = 0;
    hexToLeUInt32(hex, &ui32);
    ASSERT_EQ(ui32, 0x4030201);
    hexToLeUInt32(hex + 2, &ui32);
    ASSERT_EQ(ui32, 0xAB040302);
    hexToLeUInt32(hex + 1, &ui32);
    ASSERT_EQ(ui32, 0x4A302010);
}

GTEST_TEST(Hex, toUInt64)
{
    const uint8_t hex[] = "01020304ABcd7890";
    uint64_t ui64 = 0;
    hexToLeUInt64(hex, &ui64);
    ASSERT_EQ(ui64, 0x9078cdab04030201);
    hexToBeUInt64(hex, &ui64);
    ASSERT_EQ(ui64, 0x01020304ABcd7890);
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

GTEST_TEST(Bin, hexPutUInt8)
{
    char *hex = (char *)malloc(5);
    char *head = hex;
    memset(hex, 0, 5);
    uint8_t b = 0xAB;
    head += hexPutUInt8((uint8_t *)head, &b);
    ASSERT_EQ(hex[0], 'A');
    ASSERT_EQ(strlen(hex), 2);
    b = 0xCD;
    head += hexPutUInt8((uint8_t *)head, &b);
    ASSERT_EQ(hex[2], 'C');
    ASSERT_EQ(strlen(hex), 4);
}

GTEST_TEST(Bin, hexPutUInt16)
{
    char *hex = (char *)malloc(9);
    char *head = hex;
    memset(hex, 0, 9);
    uint16_t b = 0xABCD;
    head += hexPutBeUInt16((uint8_t *)head, &b);
    ASSERT_EQ(hex[0], 'A');
    ASSERT_EQ(hex[3], 'D');
    ASSERT_EQ(strlen(hex), 4);
    b = 0xEF01;
    head += hexPutLeUInt16((uint8_t *)head, &b);
    ASSERT_EQ(hex[4], '0');
    ASSERT_EQ(hex[5], '1');
    ASSERT_EQ(hex[6], 'E');
    ASSERT_EQ(hex[7], 'F');
    ASSERT_EQ(strlen(hex), 8);
}

GTEST_TEST(Bin, hexPutUInt)
{
    char *hex = (char *)malloc(61);
    char *head = hex;
    memset(hex, 0, 61);
    uint16_t u16 = 0xABCD;
    head += hexPutBeUInt16((uint8_t *)head, &u16);
    ASSERT_EQ(hex[0], 'A');
    ASSERT_EQ(hex[3], 'D');
    ASSERT_EQ(strlen(hex), 4);
    uint32_t u32 = 0xEF01;
    head += hexPutLeUInt32((uint8_t *)head, &u32);
    ASSERT_EQ(hex[4], '0');
    ASSERT_EQ(hex[5], '1');
    ASSERT_EQ(hex[6], 'E');
    ASSERT_EQ(hex[7], 'F');
    ASSERT_EQ(hex[11], '0');
    ASSERT_EQ(strlen(hex), 12);
    head += hexPutBeUInt32((uint8_t *)head, &u32);
    ASSERT_EQ(hex[16], 'E');
    ASSERT_EQ(hex[17], 'F');
    ASSERT_EQ(strlen(hex), 20);
    uint8_t b = 0xAB;
    head += hexPutUInt8((uint8_t *)head, &b);
    ASSERT_EQ(hex[20], 'A');
    ASSERT_EQ(hex[21], 'B');
    ASSERT_EQ(strlen(hex), 22);
    uint64_t u64 = 0x9078cdab04030201;
    head += hexPutBeUInt64((uint8_t *)head, &u64);
    ASSERT_EQ(hex[26], 'C');
    ASSERT_EQ(strlen(hex), 38);
    head += hexPutLeUInt64((uint8_t *)head, &u64);
    ASSERT_EQ(hex[39], '1');
    ASSERT_EQ(strlen(hex), 54);
    head += hexPutLeUInt16((uint8_t *)head, &u16);
    ASSERT_EQ(hex[57], 'B');
    ASSERT_EQ(strlen(hex), 58);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
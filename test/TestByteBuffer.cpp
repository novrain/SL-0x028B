#include "gtest/gtest.h"

#include "bytebuffer.h"

GTEST_TEST(ByteBuffer, Ctor)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    ByteBuffer_ctor(buf, 100);
    ASSERT_EQ(buf->position, 0);
    ASSERT_EQ(buf->limit, 100);
    ASSERT_EQ(buf->size, 100);
    ByteBuffer_dtor(buf);
    DelInstance(buf);
}

GTEST_TEST(ByteBuffer, Ctor_cpoy)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(buf, (uint8_t *)"a", 1);
    ASSERT_EQ(buf->position, 1);
    ASSERT_EQ(buf->limit, 1);
    ASSERT_EQ(buf->size, 1);

    ByteBuffer_Flip(buf);
    ASSERT_EQ(buf->position, 0);
    ASSERT_EQ(buf->limit, 1);
    ASSERT_EQ(buf->size, 1);

    ByteBuffer_Clear(buf);
    ASSERT_EQ(buf->position, 0);
    ASSERT_EQ(buf->limit, 1);
    ASSERT_EQ(buf->size, 1);

    ByteBuffer_dtor(buf);
    DelInstance(buf);
}

GTEST_TEST(ByteBuffer, Ctor_wrapped)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    ByteBuffer_ctor_wrapped(buf, (uint8_t *)"abc", 3);
    ASSERT_EQ(buf->position, 3);
    ASSERT_EQ(buf->limit, 3);
    ASSERT_EQ(buf->size, 3);
    ByteBuffer_Flip(buf);
    ASSERT_EQ(buf->position, 0);
    ASSERT_EQ(buf->limit, 3);
    ASSERT_EQ(buf->size, 3);
    ByteBuffer_Clear(buf); // cannot clean wrapped
    ASSERT_EQ(buf->position, 0);
    ASSERT_EQ(buf->limit, 3);
    ASSERT_EQ(buf->size, 3);
    ByteBuffer_dtor(buf);
    DelInstance(buf);
}

GTEST_TEST(ByteBuffer, PutGetUInt8)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    ByteBuffer_ctor(buf, 1);
    ASSERT_EQ(buf->position, 0);
    ASSERT_EQ(buf->limit, 1);
    ASSERT_EQ(buf->size, 1);
    ASSERT_EQ(1, ByteBuffer_PutUInt8(buf, 100));
    ASSERT_EQ(0, ByteBuffer_PutUInt8(buf, 100));
    ASSERT_EQ(buf->position, 1);
    ASSERT_EQ(buf->limit, 1);
    ASSERT_EQ(buf->size, 1);
    uint8_t val = 0;
    ASSERT_EQ(0, ByteBuffer_GetUInt8(buf, &val));
    ByteBuffer_Flip(buf);
    ASSERT_EQ(1, ByteBuffer_GetUInt8(buf, &val));
    ASSERT_EQ(0, ByteBuffer_GetUInt8(buf, &val));
    ASSERT_EQ(val, 100);
    ByteBuffer_dtor(buf);
    DelInstance(buf);
}

GTEST_TEST(ByteBuffer, ByteBuffer_GetByteBuffer)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    ByteBuffer_ctor_wrapped(buf, (uint8_t *)"af0", 3);
    ByteBuffer_Flip(buf);

    ByteBuffer *cp = ByteBuffer_GetByteBuffer(buf, 4);
    ASSERT_TRUE(cp == NULL);
    cp = ByteBuffer_GetByteBuffer(buf, 1);
    ByteBuffer_Flip(cp);
    uint8_t u8 = 0;
    ASSERT_EQ(ByteBuffer_GetUInt8(cp, &u8), 1);
    ASSERT_EQ(u8, 'a');
    ByteBuffer_dtor(cp);
    DelInstance(cp);

    cp = ByteBuffer_GetByteBuffer(buf, 2);
    ByteBuffer_Flip(cp);
    ByteBuffer_dtor(cp);
    DelInstance(cp);

    ByteBuffer_dtor(buf);
    DelInstance(buf);
}

GTEST_TEST(ByteBuffer, ByteBuffer_BE_GetUInt)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    uint8_t bin[] = {1, 2, 3, 4, 0xab, 0xcd, 1, 2, 3, 4};
    ByteBuffer_ctor_wrapped(buf, bin, 10);
    ByteBuffer_Flip(buf);
    uint8_t u8 = 0;
    ASSERT_EQ(1, ByteBuffer_BE_GetUInt(buf, &u8, 1));
    ASSERT_EQ(1, u8);

    uint16_t u16 = 0;
    ASSERT_EQ(2, ByteBuffer_BE_GetUInt16(buf, &u16));
    ASSERT_EQ((2 << 8) + 3, u16);

    ByteBuffer_Rewind(buf);

    uint32_t u32 = 0;
    ASSERT_EQ(4, ByteBuffer_BE_GetUInt32(buf, &u32));
    ASSERT_EQ((1 << 24) + (2 << 16) + (3 << 8) + 4, u32);

    ByteBuffer_Rewind(buf);

    uint64_t u64 = 0;
    ASSERT_EQ(8, ByteBuffer_BE_GetUInt64(buf, &u64));
    uint64_t expect = (1ll << 56) |
                      (2ll << 48) |
                      (3ll << 40) |
                      (4ll << 32) |
                      (0xabll << 24) |
                      (0xcdll << 16) |
                      (1ll << 8) |
                      2ll;
    ASSERT_EQ(expect, u64);

    ByteBuffer_Rewind(buf);

    u8 = 0;
    ASSERT_EQ(1, ByteBuffer_LE_GetUInt(buf, &u8, 1));
    ASSERT_EQ(1, u8);

    u16 = 0;
    ASSERT_EQ(2, ByteBuffer_LE_GetUInt16(buf, &u16));
    ASSERT_EQ((3 << 8) + 2, u16);

    ByteBuffer_Rewind(buf);
    u32 = 0;
    ASSERT_EQ(4, ByteBuffer_LE_GetUInt32(buf, &u32));
    ASSERT_EQ((4 << 24) + (3 << 16) + (2 << 8) + 1, u32);

    ByteBuffer_Rewind(buf);
    u64 = 0;
    ASSERT_EQ(8, ByteBuffer_LE_GetUInt64(buf, &u64));
    expect = (1ll) |
             (2ll << 8) |
             (3ll << 16) |
             (4ll << 24) |
             (0xabll << 32) |
             (0xcdll << 40) |
             (1ll << 48) |
             (2ll << 56);
    ASSERT_EQ(expect, u64);

    ByteBuffer_dtor(buf);
    DelInstance(buf);
}

GTEST_TEST(ByteBuffer, ByteBuffer_CRC16)
{
    ByteBuffer *buf = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(buf, "7E7E001234567810123434800802000114061314385304696E", 50);
    ByteBuffer_Flip(buf);

    uint16_t crc = 0;
    ByteBuffer_CRC16(buf, &crc, 0, ByteBuffer_Available(buf) - 2);
    ASSERT_EQ(crc >> 8, 0x69);
    ASSERT_EQ(crc & 0xFF, 0x6E);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
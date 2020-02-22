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

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
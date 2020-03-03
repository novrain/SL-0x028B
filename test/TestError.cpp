#include "gtest/gtest.h"

#include "sl651/sl651.h"

GTEST_TEST(Error, last_error)
{
    ASSERT_EQ(SL651_ERROR_SUCCESS, last_error());
    ASSERT_FALSE(set_error_indicate(SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY));
    ASSERT_EQ(last_error(), SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY);
    const char *err = error_str(SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY);
    ASSERT_STREQ(err, "Empty artificial element.");
    ASSERT_STREQ(error_name(SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY),
                 "SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY");
    ASSERT_STREQ(error_module_name(SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY), "sl651");
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
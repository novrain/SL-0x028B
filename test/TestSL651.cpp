#include "gtest/gtest.h"

#include "sl651.h"

GTEST_TEST(Definition, package)
{
    Head *head = NewInstance(Head);           // 创建空实例
    Head_ctor(head);                          // 调用构造，Head的构造实际上是个空实现
    (*head).addrPair.downAddr.centerAddr = 0; // 调用方法，或者直接访问，必要的功能可以封装起来，简单赋值直接访问
    (*head).direction = Up;                   //
    Package *pkg = NewInstance(Package);      // 同样的步骤，创建空实例
    Package_ctor(pkg, head);                  // 构造函数，带有参数
    ASSERT_EQ(Package_Direction(pkg), Up);    //
    Head_dtor(pkg);                           // 倒序析构，用来释放实例所 管理 的指针，管理：包括实例在其封装方法中创建的内存指针，或接口中约定的传入的代管内存指针
    Package_dtor(pkg);                        // 析构
    DelInstance(head);                        // 释放实例，与析构分开调用；如果不是New，则不需要释放实例
    DelInstance(pkg);                         // 同上
}

GTEST_TEST(DecodeElement, decodeCustomeElement)
{
    Element *el = decodeElementFromHex(NULL);
    ByteBuffer *hexBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"FFFFFFFFFF", 10);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el == NULL);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);
}

GTEST_TEST(DecodeElement, decodeObserveTimeElement)
{
    Element *el = decodeElementFromHex(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *hexBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"a", 2);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el == NULL);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);

    hexBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"F0F0200222222211", 16);
    ByteBuffer_Flip(hexBuff);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, DATETIME);
    ASSERT_EQ(el->dataDef, DATETIME);
    ObserveTimeElement *otel = (ObserveTimeElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(otel->super.identifierLeader, DATETIME);
    ASSERT_EQ(otel->super.dataDef, DATETIME);
    ASSERT_EQ(otel->observeTime.day, 22);

    ObserveTimeElement_dtor(otel);
    DelInstance(otel);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);
}

GTEST_TEST(DecodeElement, decodeRemoteStationAddrElement)
{
    Element *el = decodeElementFromHex(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *hexBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"F1F12002222222", 14); // 其他遥测站  != A5_HYDROLOGICAL_TELEMETRY_STATION
    ByteBuffer_Flip(hexBuff);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ADDRESS);
    ASSERT_EQ(el->dataDef, ADDRESS);
    RemoteStationAddrElement *rsael = (RemoteStationAddrElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(rsael->super.identifierLeader, ADDRESS);
    ASSERT_EQ(rsael->super.dataDef, ADDRESS);
    ASSERT_EQ(rsael->stationAddr.A1, 87); // 0x2222  8738  =>  00 87 38

    RemoteStationAddrElement_dtor(rsael);
    DelInstance(rsael);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);

    hexBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"F1F10002222222", 14); // 水文遥测站 A5_HYDROLOGICAL_TELEMETRY_STATION
    ByteBuffer_Flip(hexBuff);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ADDRESS);
    ASSERT_EQ(el->dataDef, ADDRESS);
    rsael = (RemoteStationAddrElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(rsael->super.identifierLeader, ADDRESS);
    ASSERT_EQ(rsael->super.dataDef, ADDRESS);
    ASSERT_EQ(rsael->stationAddr.A5, A5_HYDROLOGICAL_TELEMETRY_STATION);
    ASSERT_EQ(rsael->stationAddr.A1, 22);

    RemoteStationAddrElement_dtor(rsael);
    DelInstance(rsael);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);
}

GTEST_TEST(DecodeElement, decodePictureElement)
{
    Element *el = decodeElementFromHex(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *hexBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"F3F3200222222211", 16);
    ByteBuffer_Flip(hexBuff);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, PICTURE_IL);
    ASSERT_EQ(el->dataDef, PICTURE_IL);
    PictureElement *pel = (PictureElement *)el;
    ASSERT_EQ(pel->super.identifierLeader, PICTURE_IL);
    ASSERT_EQ(pel->super.dataDef, PICTURE_IL);

    PictureElement_dtor(pel);
    DelInstance(pel);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);
}

GTEST_TEST(DecodeElement, decodeNumberElement)
{
    Element *el = decodeElementFromHex(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *hexBuff = NewInstance(ByteBuffer);
    // 28 23
    // 水位基值1标识符
    // 00 01 11 10
    // 11.110米
    // N(7,3) ，数据单位：米
    ByteBuffer_ctor_copy(hexBuff, (uint8_t *)"282300011110", 12);
    ByteBuffer_Flip(hexBuff);
    el = decodeElementFromHex(hexBuff);
    ASSERT_TRUE(el != NULL);
    NumberElement *nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x28);
    ASSERT_EQ(nel->super.dataDef, 0x23);
    ASSERT_EQ(nel->buff->size, 8);

    uint64_t u64 = 0;
    NumberElement_GetInteger(nel, &u64);

    float f = 0;
    NumberElement_GetFloat(nel, &f);
    ASSERT_EQ(f, 11.11f);

    NumberElement_dtor(nel);
    DelInstance(nel);
    ByteBuffer_dtor(hexBuff);
    DelInstance(hexBuff);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
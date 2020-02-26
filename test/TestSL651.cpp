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
    Element *el = decodeElement(NULL);
    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "FFFFFFFFFF", 10);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el == NULL);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeObserveTimeElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(byteBuff, (uint8_t *)"a", 2);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el == NULL);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);

    byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "F0F0200222222211", 16);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, DATETIME);
    ASSERT_EQ(el->dataDef, DATETIME);
    ObserveTimeElement *otel = (ObserveTimeElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(otel->super.identifierLeader, DATETIME);
    ASSERT_EQ(otel->super.dataDef, DATETIME);
    ASSERT_EQ(otel->observeTime.day, 22);

    ObserveTimeElement_dtor(otel);
    DelInstance(otel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeRemoteStationAddrElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "F1F12002222222", 14); // 其他遥测站  != A5_HYDROLOGICAL_TELEMETRY_STATION
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ADDRESS);
    ASSERT_EQ(el->dataDef, ADDRESS);
    RemoteStationAddrElement *rsael = (RemoteStationAddrElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(rsael->super.identifierLeader, ADDRESS);
    ASSERT_EQ(rsael->super.dataDef, ADDRESS);
    ASSERT_EQ(rsael->stationAddr.A1, 87); // 0x2222  8738  =>  00 87 38

    RemoteStationAddrElement_dtor(rsael);
    DelInstance(rsael);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);

    byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "F1F10002222222", 14); // 水文遥测站 A5_HYDROLOGICAL_TELEMETRY_STATION
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
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
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodePictureElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "F3F3200222222211", 16);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, PICTURE_IL);
    ASSERT_EQ(el->dataDef, PICTURE_IL);
    PictureElement *pel = (PictureElement *)el;
    ASSERT_EQ(pel->super.identifierLeader, PICTURE_IL);
    ASSERT_EQ(pel->super.dataDef, PICTURE_IL);

    PictureElement_dtor(pel);
    DelInstance(pel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeArtificialElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "F2F279616E7975313938383A59435A2D32412D313031212121", 50);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ARTIFICIAL_IL);
    ASSERT_EQ(el->dataDef, ARTIFICIAL_IL);
    ArtificialElement *aiel = (ArtificialElement *)el;
    ASSERT_EQ(aiel->super.identifierLeader, ARTIFICIAL_IL);
    ASSERT_EQ(aiel->super.dataDef, ARTIFICIAL_IL);

    ArtificialElement_dtor(aiel);
    DelInstance(aiel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeDRP5MINElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff,
                               "F460"
                               "000100000000000000000000",
                               28);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, DRP5MIN);
    ASSERT_EQ(el->dataDef, DRP5MIN_DATADEF);
    DRP5MINElement *drp5el = (DRP5MINElement *)el;
    ASSERT_EQ(drp5el->super.identifierLeader, DRP5MIN);
    ASSERT_EQ(drp5el->super.dataDef, DRP5MIN_DATADEF);
    float fv = 1;
    ASSERT_EQ(1, DRP5MINElement_ValueAt(drp5el, 0, &fv));
    ASSERT_TRUE(0 == fv);
    ASSERT_EQ(1, DRP5MINElement_ValueAt(drp5el, 1, &fv));
    ASSERT_EQ(0.1f, fv);

    DRP5MINElement_dtor(drp5el);
    DelInstance(drp5el);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeFlowRateDataElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "FDF6200222222211", 16);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, FLOW_RATE_DATA);
    ASSERT_EQ(el->dataDef, FLOW_RATE_DATA_DATADEF);
    FlowRateDataElement *flel = (FlowRateDataElement *)el;
    ASSERT_EQ(flel->super.identifierLeader, FLOW_RATE_DATA);
    ASSERT_EQ(flel->super.dataDef, FLOW_RATE_DATA_DATADEF);
    FlowRateDataElement_dtor(flel);
    DelInstance(flel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeRelativeWaterLevelElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff,
                               "F5C0"
                               "0AAA0AAAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
                               52);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(el->dataDef, RELATIVE_WATER_LEVEL_5MIN1_DATADEF);
    RelativeWaterLevelElement *rwl5el = (RelativeWaterLevelElement *)el;
    ASSERT_EQ(rwl5el->super.identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(rwl5el->super.dataDef, RELATIVE_WATER_LEVEL_5MIN1_DATADEF);
    float fv = 1;
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwl5el, 0, &fv));
    ASSERT_TRUE(0x0AAA / 100.0f == fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwl5el, 1, &fv));
    ASSERT_EQ(0x0AAA / 100.0f, fv);

    RelativeWaterLevelElement_dtor(rwl5el);
    DelInstance(rwl5el);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeNumberElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    // 28 23
    // 水位基值1标识符
    // 00 01 11 10
    // 11.110米
    // N(7,3) ，数据单位：米
    ByteBuffer_ctor_fromHexStr(byteBuff, "282300011110", 12);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    NumberElement *nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x28);
    ASSERT_EQ(nel->super.dataDef, 0x23);
    ASSERT_EQ(nel->buff->size, 4);

    uint64_t u64 = 0;
    NumberElement_GetInteger(nel, &u64);

    float f = 0;
    NumberElement_GetFloat(nel, &f);
    ASSERT_EQ(f, 11.11f);

    NumberElement_dtor(nel);
    DelInstance(nel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeTimeStepCodeElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "0418200222", 10);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(el->dataDef, TIME_STEP_CODE_DATADEF);
    TimeStepCodeElement *tscel = (TimeStepCodeElement *)el;
    ASSERT_EQ(tscel->super.identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(tscel->super.dataDef, TIME_STEP_CODE_DATADEF);
    ASSERT_EQ(tscel->timeStepCode.hour, 2);

    TimeStepCodeElement_dtor(tscel);
    DelInstance(tscel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeStationStatusElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "452020022211", 12);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, STATION_STATUS);
    ASSERT_EQ(el->dataDef, STATION_STATUS_DATADEF);
    StationStatusElement *ssel = (StationStatusElement *)el;
    ASSERT_EQ(ssel->super.identifierLeader, STATION_STATUS);
    ASSERT_EQ(ssel->super.dataDef, STATION_STATUS_DATADEF);
    ASSERT_EQ(ssel->status, 0x20022211);

    StationStatusElement_dtor(ssel);
    DelInstance(ssel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeDurationElement)
{
    Element *el = decodeElement(NULL);
    ASSERT_TRUE(el == NULL);

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    ByteBuffer_ctor_fromHexStr(byteBuff, "052831312e3031", 14);
    ByteBuffer_Flip(byteBuff);
    el = decodeElement(byteBuff);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, DURATION_OF_XX);
    ASSERT_EQ(el->dataDef, DURATION_OF_XX_DATADEF);
    DurationElement *duel = (DurationElement *)el;
    ASSERT_EQ(duel->super.identifierLeader, DURATION_OF_XX);
    ASSERT_EQ(duel->super.dataDef, DURATION_OF_XX_DATADEF);
    ASSERT_EQ(duel->hour, 11);
    ASSERT_EQ(duel->minute, 1);

    DurationElement_dtor(duel);
    DelInstance(duel);
    ByteBuffer_dtor(byteBuff);
    DelInstance(byteBuff);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
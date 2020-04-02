#include "gtest/gtest.h"

#include "common/class.h"
#include "sl651/sl651.h"

GTEST_TEST(Definition, package)
{
    //
    Package *pkg = NewInstance(Package);   // 同样的步骤，创建空实例
    Package_ctor(pkg);                     // 构造函数，带有参数
    pkg->head.centerAddr = 0;              // 调用方法，或者直接访问，必要的功能可以封装起来，简单赋值直接访问
    pkg->head.direction = Up;              //
    ASSERT_EQ(Package_Direction(pkg), Up); //
    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, PACKAGE_HEAD_STX_LEN);
    Package_EncodeHead(pkg, encoded);
    ASSERT_EQ(BB_Position(encoded), PACKAGE_HEAD_STX_LEN);

    BB_dtor(encoded);
    DelInstance(encoded);
    Package_dtor(pkg); // 析构 or pkg->vtbl->dtor(pkg)?
    DelInstance(pkg);  // 同上
}

GTEST_TEST(DecodeElement, decodeCustomeElement)
{
    Element *el = NULL;
    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "FFFFFFFFFF", 10);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el == NULL);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeObserveTimeElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_copy(byteBuff, (uint8_t *)"a", 2);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el == NULL);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);

    byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "F0F02002222222", 14);
    BB_Flip(byteBuff);
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, OBSERVETIME);
    ASSERT_EQ(el->dataDef, OBSERVETIME);
    ASSERT_EQ(el->vptr->size(el), OBSERVETIME_LEN + ELEMENT_IDENTIFER_LEN);

    ObserveTimeElement *otel = (ObserveTimeElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(otel->super.identifierLeader, OBSERVETIME);
    ASSERT_EQ(otel->super.dataDef, OBSERVETIME);
    ASSERT_EQ(otel->observeTime.day, 22);

    ASSERT_EQ(otel->super.vptr->size(el), 7);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, otel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), otel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    ObserveTimeElement_dtor(el);
    DelInstance(otel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeRemoteStationAddrElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "F1F12002222222", 14); // 其他遥测站  != A5_HYDROLOGICAL_TELEMETRY_STATION
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ADDRESS);
    ASSERT_EQ(el->dataDef, ADDRESS);
    ASSERT_EQ(el->vptr->size(el), REMOTE_STATION_ADDR_LEN + ELEMENT_IDENTIFER_LEN);

    RemoteStationAddrElement *rsael = (RemoteStationAddrElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(rsael->super.identifierLeader, ADDRESS);
    ASSERT_EQ(rsael->super.dataDef, ADDRESS);
    ASSERT_EQ(rsael->stationAddr.A1, 87); // 0x2222  8738  =>  00 87 38

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, rsael->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), rsael->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    RemoteStationAddrElement_dtor(el);
    DelInstance(rsael);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);

    byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "F1F10002222222", 14); // 水文遥测站 A5_HYDROLOGICAL_TELEMETRY_STATION
    BB_Flip(byteBuff);
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ADDRESS);
    ASSERT_EQ(el->dataDef, ADDRESS);
    ASSERT_EQ(el->vptr->size(el), REMOTE_STATION_ADDR_LEN + ELEMENT_IDENTIFER_LEN);

    rsael = (RemoteStationAddrElement *)el; // 类型转换要小心，因为没有严格的类型匹配
    ASSERT_EQ(rsael->super.identifierLeader, ADDRESS);
    ASSERT_EQ(rsael->super.dataDef, ADDRESS);
    ASSERT_EQ(rsael->stationAddr.A5, A5_HYDROLOGICAL_TELEMETRY_STATION);
    ASSERT_EQ(rsael->stationAddr.A1, 22);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, rsael->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), rsael->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    RemoteStationAddrElement_dtor(el);
    DelInstance(rsael);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodePictureElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "F3F3200222222211", 16);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    head.stxFlag = SYN;
    head.sequence.seq = 1;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, PICTURE_IL);
    ASSERT_EQ(el->dataDef, PICTURE_IL);
    PictureElement *pel = (PictureElement *)el;
    ASSERT_EQ(pel->super.identifierLeader, PICTURE_IL);
    ASSERT_EQ(pel->super.dataDef, PICTURE_IL);
    ASSERT_EQ(pel->super.vptr->size(el), ELEMENT_IDENTIFER_LEN + 6);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, pel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    PictureElement_dtor(el);
    DelInstance(pel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeArtificialElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "F2F279616E7975313938383A59435A2D32412D313031212121", 50);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, ARTIFICIAL_IL);
    ASSERT_EQ(el->dataDef, ARTIFICIAL_IL);
    ArtificialElement *aiel = (ArtificialElement *)el;
    ASSERT_EQ(aiel->super.identifierLeader, ARTIFICIAL_IL);
    ASSERT_EQ(aiel->super.dataDef, ARTIFICIAL_IL);
    ASSERT_EQ(aiel->super.vptr->size(el), 25);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, aiel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), aiel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    Element_SetDirection(el, Down);
    ASSERT_EQ(aiel->super.vptr->size(el), 25);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, aiel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), aiel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    ArtificialElement_dtor(el);
    DelInstance(aiel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeDRP5MINElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff,
                       "F460"
                       "000100000000000000000000",
                       28);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, DRP5MIN);
    ASSERT_EQ(el->dataDef, DRP5MIN_DATADEF);
    ASSERT_EQ(el->vptr->size(el), DRP5MIN_LEN + ELEMENT_IDENTIFER_LEN);
    DRP5MINElement *drp5el = (DRP5MINElement *)el;
    ASSERT_EQ(drp5el->super.identifierLeader, DRP5MIN);
    ASSERT_EQ(drp5el->super.dataDef, DRP5MIN_DATADEF);
    float fv = 1;
    ASSERT_EQ(1, DRP5MINElement_ValueAt(drp5el, 0, &fv));
    ASSERT_TRUE(0 == fv);
    ASSERT_EQ(1, DRP5MINElement_ValueAt(drp5el, 1, &fv));
    ASSERT_EQ(0.1f, fv);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, drp5el->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), drp5el->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    Element_SetDirection(el, Down);
    ASSERT_EQ(el->vptr->size(el), ELEMENT_IDENTIFER_LEN);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, drp5el->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), 2);

    BB_dtor(encoded);
    DelInstance(encoded);

    DRP5MINElement_dtor(el);
    DelInstance(drp5el);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeFlowRateDataElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "FDF6200222222211", 16);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, FLOW_RATE_DATA);
    ASSERT_EQ(el->dataDef, FLOW_RATE_DATA_DATADEF);
    ASSERT_EQ(el->vptr->size(el), 8);
    FlowRateDataElement *flel = (FlowRateDataElement *)el;
    ASSERT_EQ(flel->super.identifierLeader, FLOW_RATE_DATA);
    ASSERT_EQ(flel->super.dataDef, FLOW_RATE_DATA_DATADEF);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, flel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), flel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    Element_SetDirection(el, Down);
    ASSERT_EQ(el->vptr->size(el), 2);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, flel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), flel->super.vptr->size(el));

    BB_dtor(encoded);
    DelInstance(encoded);

    FlowRateDataElement_dtor(el);
    DelInstance(flel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeRelativeWaterLevelElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff,
                       "F5C0"
                       "0AAA0AAAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF",
                       52);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(el->dataDef, RELATIVE_WATER_LEVEL_5MIN1_DATADEF);
    ASSERT_EQ(el->vptr->size(el), 26);

    RelativeWaterLevelElement *rwl5el = (RelativeWaterLevelElement *)el;
    ASSERT_EQ(rwl5el->super.identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(rwl5el->super.dataDef, RELATIVE_WATER_LEVEL_5MIN1_DATADEF);
    float fv = 1;
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwl5el, 0, &fv));
    ASSERT_TRUE(0x0AAA / 100.0f == fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwl5el, 1, &fv));
    ASSERT_EQ(0x0AAA / 100.0f, fv);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, rwl5el->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), rwl5el->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    Element_SetDirection(el, Down);
    ASSERT_EQ(el->vptr->size(el), 2);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, rwl5el->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), rwl5el->super.vptr->size(el));

    ASSERT_EQ(encoded->buff[0], 0xF5);
    ASSERT_EQ(encoded->buff[1], 0xC0);
    BB_dtor(encoded);
    DelInstance(encoded);

    RelativeWaterLevelElement_dtor(el);
    DelInstance(rwl5el);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeNumberElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    // 28 23
    // 水位基值1标识符
    // 00 01 11 10
    // 11.110米
    // N(7,3) ，数据单位：米
    BB_ctor_fromHexStr(byteBuff, "282300011110", 12);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->vptr->size(el), 6);
    NumberElement *nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x28);
    ASSERT_EQ(nel->super.dataDef, 0x23);
    ASSERT_EQ(nel->buff->size, 4);

    uint64_t u64 = 0;
    NumberElement_GetInteger(nel, &u64);

    float f = 0;
    NumberElement_GetFloat(nel, &f);
    ASSERT_EQ(f, 11.11f);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, nel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), nel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    Element_SetDirection(el, Down);
    ASSERT_EQ(el->vptr->size(el), 2);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, nel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), nel->super.vptr->size(el));

    ASSERT_EQ(encoded->buff[0], 0x28);
    ASSERT_EQ(encoded->buff[1], 0x23);

    BB_dtor(encoded);
    DelInstance(encoded);

    NumberElement_dtor(el);
    DelInstance(nel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeTimeStepCodeElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "0418"
                                 "000005"
                                 "3923"
                                 "00000122"
                                 "00000122"
                                 "00000285"
                                 "FFFFFFFF"
                                 "00010490"
                                 "00010490"
                                 "FFFFFFFF",
                       70);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(el->dataDef, TIME_STEP_CODE_DATADEF);
    ASSERT_EQ(el->vptr->size(el), 35);

    TimeStepCodeElement *tscel = (TimeStepCodeElement *)el;
    ASSERT_EQ(tscel->super.identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(tscel->super.dataDef, TIME_STEP_CODE_DATADEF);
    ASSERT_EQ(tscel->timeStepCode.hour, 0);
    ASSERT_EQ(tscel->timeStepCode.minute, 5);

    NumberListElement *nle = &tscel->numberListElement;
    ASSERT_EQ(nle->count, 7);
    float fv = 0;
    ASSERT_EQ(4, NumberListElement_GetFloatAt(nle, 0, &fv));
    ASSERT_EQ(0.122f, fv);
    ASSERT_EQ(4, NumberListElement_GetFloatAt(nle, 5, &fv));
    ASSERT_EQ(10.49f, fv);
    ASSERT_EQ(0, NumberListElement_GetFloatAt(nle, 6, &fv));
    ASSERT_EQ(7.20576e+13f, fv);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, tscel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), tscel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));

    BB_dtor(encoded);
    DelInstance(encoded);

    Element_SetDirection(el, Down);
    ASSERT_EQ(el->vptr->size(el), 7);

    encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, tscel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), tscel->super.vptr->size(el));

    ASSERT_EQ(encoded->buff[0], 0x04);
    ASSERT_EQ(encoded->buff[1], 0x18);
    ASSERT_EQ(encoded->buff[5], 0x39);
    ASSERT_EQ(encoded->buff[6], 0x23);
    BB_dtor(encoded);
    DelInstance(encoded);

    TimeStepCodeElement_dtor(el);
    DelInstance(tscel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeStationStatusElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "452020022211", 12);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, STATION_STATUS);
    ASSERT_EQ(el->dataDef, STATION_STATUS_DATADEF);
    StationStatusElement *ssel = (StationStatusElement *)el;
    ASSERT_EQ(ssel->super.identifierLeader, STATION_STATUS);
    ASSERT_EQ(ssel->super.dataDef, STATION_STATUS_DATADEF);
    ASSERT_EQ(ssel->status, 0x20022211);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, ssel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), ssel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    StationStatusElement_dtor(el);
    DelInstance(ssel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(DecodeElement, decodeDurationElement)
{
    Element *el = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "052831312e3031", 14);
    BB_Flip(byteBuff);
    Head head = {0};
    head.direction = Up;
    el = decodeElement(byteBuff, &head);
    ASSERT_TRUE(el != NULL);
    ASSERT_EQ(el->identifierLeader, DURATION_OF_XX);
    ASSERT_EQ(el->dataDef, DURATION_OF_XX_DATADEF);
    DurationElement *duel = (DurationElement *)el;
    ASSERT_EQ(duel->super.identifierLeader, DURATION_OF_XX);
    ASSERT_EQ(duel->super.dataDef, DURATION_OF_XX_DATADEF);
    ASSERT_EQ(duel->hour, 11);
    ASSERT_EQ(duel->minute, 1);

    ByteBuffer *encoded = NewInstance(ByteBuffer);
    BB_ctor(encoded, duel->super.vptr->size(el));

    el->vptr->encode(el, encoded);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), duel->super.vptr->size(el));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    DurationElement_dtor(el);
    DelInstance(duel);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeKeepAliveUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "01"
                                 "0012345678"
                                 "1234"
                                 "2F"
                                 "0008"
                                 "02"
                                 "0003"
                                 "591011155111"
                                 "03"
                                 "6BCA",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x6B);
    ASSERT_EQ(crc & 0xFF, 0xCA);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, 0x2F);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x6BCA);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    BB_dtor(byteBuff);
    DelInstance(byteBuff);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
}

GTEST_TEST(Package, decodeRainfallStationHourlyDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "10"
                                 "1234"
                                 "34"
                                 "8008"
                                 "02"
                                 "0001"
                                 "140613143853"
                                 "04"
                                 "696E",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x69);
    ASSERT_EQ(crc & 0xFF, 0x6E);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, HOUR);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, 04);
    ASSERT_EQ(pkg->tail.crc, 0x696e);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeRainfallStationHourlyUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "10"
                                 "0012345678"
                                 "1234"
                                 "34"
                                 "0038"
                                 "02"
                                 "0001"
                                 "140612020000"
                                 "F1F1"
                                 "0012345678"
                                 "50"
                                 "F0F0"
                                 "1406120200"
                                 "F460"
                                 "000000000000000000000000"
                                 "2619"
                                 "000000"
                                 "2019"
                                 "000000"
                                 "1A19"
                                 "000000"
                                 "3812"
                                 "1290"
                                 "03"
                                 "4383",
                       146);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x43);
    ASSERT_EQ(crc & 0xFF, 0x83);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, HOUR);
    ASSERT_EQ(pkg->head.direction, Up);
    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 5);
    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, DRP5MIN);
    ASSERT_EQ(el->dataDef, 0x60);
    // it is a DRP5MINElement
    DRP5MINElement *drp5el = (DRP5MINElement *)el;
    ASSERT_EQ(drp5el->super.identifierLeader, DRP5MIN);
    ASSERT_EQ(drp5el->super.dataDef, DRP5MIN_DATADEF);
    float fv = 1;
    ASSERT_EQ(1, DRP5MINElement_ValueAt(drp5el, 0, &fv));
    ASSERT_TRUE(0 == fv);
    ASSERT_EQ(1, DRP5MINElement_ValueAt(drp5el, 1, &fv));
    ASSERT_EQ(0, fv);

    NumberElement *nel = NULL;

    el = elements->data[1];
    // it is a NumberElement
    nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x26);
    ASSERT_EQ(nel->super.dataDef, 0x19);
    ASSERT_EQ(3, NumberElement_GetFloat(nel, &fv));
    ASSERT_EQ(0, fv);

    el = elements->data[2];
    // it is a NumberElement
    nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x20);
    ASSERT_EQ(nel->super.dataDef, 0x19);
    ASSERT_EQ(3, NumberElement_GetFloat(nel, &fv));
    ASSERT_EQ(0, fv);

    el = elements->data[3];
    // it is a NumberElement
    nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x1A);
    ASSERT_EQ(nel->super.dataDef, 0x19);
    ASSERT_EQ(3, NumberElement_GetFloat(nel, &fv));
    ASSERT_EQ(0, fv);

    el = elements->data[4];
    // it is a NumberElement
    nel = (NumberElement *)el;
    ASSERT_EQ(nel->super.identifierLeader, 0x38);
    ASSERT_EQ(nel->super.dataDef, 0x12);
    ASSERT_EQ(2, NumberElement_GetFloat(nel, &fv));
    ASSERT_EQ(12.9f, fv);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x4383);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);

    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeWaterRainStationHourlyDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "10"
                                 "1234"
                                 "34"
                                 "8008"
                                 "02"
                                 "0003"
                                 "140613144304"
                                 "04"
                                 "99A1",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x99);
    ASSERT_EQ(crc & 0xFF, 0xA1);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, HOUR);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, 04);
    ASSERT_EQ(pkg->tail.crc, 0x99A1);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeWaterRainStationHourlyUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "10"
                                 "0012345678"
                                 "1234"
                                 "34"
                                 "0057"
                                 "02"
                                 "0003"
                                 "140612020000"
                                 "F1F1"
                                 "0012345678"
                                 "48"
                                 "F0F0"
                                 "1406120200"
                                 "F460"
                                 "000000000000000000000000"
                                 "F5C0"
                                 "0AAA0AAAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
                                 "2619"
                                 "000000"
                                 "2019"
                                 "000000"
                                 "1A19"
                                 "000000"
                                 "391A"
                                 "002730"
                                 "3812"
                                 "1290"
                                 "03"
                                 "DD4e",
                       208);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xdd);
    ASSERT_EQ(crc & 0xFF, 0x4e);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, HOUR);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0xdd4e);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 7);

    Element *el = elements->data[1];
    ASSERT_EQ(el->identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(el->dataDef, 0xc0);
    // it is a RelativeWaterLevelElement
    RelativeWaterLevelElement *rwlel = (RelativeWaterLevelElement *)el;
    ASSERT_EQ(rwlel->super.identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(rwlel->super.dataDef, 0xc0);
    float fv = 0;
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwlel, 0, &fv));
    ASSERT_EQ(27.3f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwlel, 1, &fv));
    ASSERT_EQ(27.3f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwlel, 11, &fv));
    ASSERT_EQ(0xffff, fv);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeRainStationAddRPTDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "10"
                                 "1234"
                                 "33"
                                 "8008"
                                 "02"
                                 "0002"
                                 "140612021525"
                                 "04"
                                 "DAAA",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xDA);
    ASSERT_EQ(crc & 0xFF, 0xAA);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, ADDED);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, 04);
    ASSERT_EQ(pkg->tail.crc, 0xDAAA);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeRainStationAddRPTUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "10"
                                 "0012345678"
                                 "1234"
                                 "33"
                                 "0038"
                                 "02"
                                 "0002"
                                 "140612021500"
                                 "F1F1"
                                 "0012345678"
                                 "50"
                                 "F0F0"
                                 "1406120215"
                                 "F460"
                                 "000000000000000000000000"
                                 "2619"
                                 "000020"
                                 "2019"
                                 "000010"
                                 "1A19"
                                 "000010"
                                 "3812"
                                 "1209"
                                 "03"
                                 "7c62",
                       146);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x7c);
    ASSERT_EQ(crc & 0xFF, 0x62);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, ADDED);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x7c62);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 5);

    Element *el = elements->data[4];
    ASSERT_EQ(el->identifierLeader, 0x38);
    ASSERT_EQ(el->dataDef, 0x12);

    NumberElement *nel = (NumberElement *)elements->data[4];
    float fv = 0;
    ASSERT_EQ(2, NumberElement_GetFloat(nel, &fv));
    ASSERT_EQ(12.09f, fv);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeTestDownlinkMessage1)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "10"
                                 "1234"
                                 "30"
                                 "8008"
                                 "02"
                                 "0002"
                                 "140612021525"
                                 "04"
                                 "19af",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x19);
    ASSERT_EQ(crc & 0xFF, 0xAf);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, 0x30); // enum TEST CONFLICT WITH GTEST
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, 04);
    ASSERT_EQ(pkg->tail.crc, 0x19AF);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeTestDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "01"
                                 "1234"
                                 "30"
                                 "8008"
                                 "02"
                                 "0003"
                                 "591011154947"
                                 "1B"
                                 "75D4",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x75);
    ASSERT_EQ(crc & 0xFF, 0xD4);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, 0x30); // enum TEST CONFLICT WITH GTEST
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ESC);
    ASSERT_EQ(pkg->tail.crc, 0x75D4);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeTestUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "01"
                                 "0012345678"
                                 "1234"
                                 "30"
                                 "002B"
                                 "02"
                                 "0003"
                                 "591011154947"
                                 "F1F1"
                                 "0012345678"
                                 "48"
                                 "F0F0"
                                 "5910111549"
                                 "2019"
                                 "000005"
                                 "2619"
                                 "000005"
                                 "3923"
                                 "00000127"
                                 "3812"
                                 "1115"
                                 "03"
                                 "20FA",
                       120);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x20);
    ASSERT_EQ(crc & 0xFF, 0xFA);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, 0x30); // enum TEST CONFLICT WITH GTEST
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x20FA);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 4);

    Element *el = elements->data[2];
    ASSERT_EQ(el->identifierLeader, 0x39);
    ASSERT_EQ(el->dataDef, 0x23);

    NumberElement *nel = (NumberElement *)el;
    float fv = 0;
    ASSERT_EQ(4, NumberElement_GetFloat(nel, &fv));
    ASSERT_EQ(0.127f, fv);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeEvenTimeDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0011223344"
                                 "05"
                                 "03E8"
                                 "31"
                                 "8008"
                                 "02"
                                 "0031"
                                 "170718110005"
                                 "1B"
                                 "291C",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x29);
    ASSERT_EQ(crc & 0xFF, 0x1C);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, EVEN_TIME);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ESC);
    ASSERT_EQ(pkg->tail.crc, 0x291C);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeEvenTimeUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "05"
                                 "0011223344"
                                 "03E8"
                                 "31"
                                 "004E"
                                 "02"
                                 "0031"
                                 "170718110005"
                                 "F1F1"
                                 "001122334448"
                                 "F0F0"
                                 "17071810"
                                 "0504"
                                 "18000005"
                                 "3923"
                                 "00000122"
                                 "00000122"
                                 "00000285"
                                 "00010490"
                                 "00010490"
                                 "FFFFFFFF"
                                 "FFFFFFFF"
                                 "FFFFFFFF"
                                 "FFFFFFFF"
                                 "FFFFFFFF"
                                 "00010490"
                                 "00010490"
                                 "03"
                                 "7AC7",
                       190);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x7a);
    ASSERT_EQ(crc & 0xFF, 0xC7);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, EVEN_TIME);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x7AC7);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(el->dataDef, TIME_STEP_CODE_DATADEF);

    TimeStepCodeElement *tsel = (TimeStepCodeElement *)el;
    ASSERT_EQ(tsel->super.identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(tsel->super.dataDef, TIME_STEP_CODE_DATADEF);

    ASSERT_EQ(0x39, tsel->numberListElement.super.identifierLeader);
    ASSERT_EQ(0x23, tsel->numberListElement.super.dataDef);
    ASSERT_EQ(12, tsel->numberListElement.count);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeIntervalDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0011223344"
                                 "05"
                                 "03E8"
                                 "32"
                                 "8008"
                                 "02"
                                 "0034"
                                 "170718110016"
                                 "1B"
                                 "E5D4",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xe5);
    ASSERT_EQ(crc & 0xFF, 0xd4);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, INTERVAL);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ESC);
    ASSERT_EQ(pkg->tail.crc, 0xE5D4);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeIntervalUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "05"
                                 "0011223344"
                                 "03E8"
                                 "32"
                                 "002B"
                                 "02"
                                 "0034"
                                 "170718110016"
                                 "F1F1"
                                 "001122334448"
                                 "F0F0"
                                 "1707181100"
                                 "2019"
                                 "000040"
                                 "2619"
                                 "000040"
                                 "3923"
                                 "00010490"
                                 "3812"
                                 "1099"
                                 "03"
                                 "A421",
                       120);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xA4);
    ASSERT_EQ(crc & 0xFF, 0x21);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, INTERVAL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0xA421);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 4);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0x20);
    ASSERT_EQ(el->dataDef, 0x19);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeArtificalDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "01"
                                 "1234"
                                 "35"
                                 "8008"
                                 "02"
                                 "0004"
                                 "591011155145"
                                 "1B"
                                 "F118",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xf1);
    ASSERT_EQ(crc & 0xFF, 0x18);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, ARTIFICIAL);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ESC);
    ASSERT_EQ(pkg->tail.crc, 0xf118);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeArtificalUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "01"
                                 "0012345678"
                                 "1234"
                                 "35"
                                 "0011"
                                 "02"
                                 "0004"
                                 "591011155145"
                                 "F2F2"
                                 "5120312E323334"
                                 "03"
                                 "4602",
                       68);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x46);
    ASSERT_EQ(crc & 0xFF, 0x02);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, ARTIFICIAL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x4602);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeArtificalUplinkMessage1)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "10"
                                 "0012345678"
                                 "1234"
                                 "35"
                                 "0021"
                                 "02"
                                 "000F"
                                 "130325095804"
                                 "F2F2"
                                 "79616E7975313938383A59435A2D32412D313031212121"
                                 "03"
                                 "B698",
                       100);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xB6);
    ASSERT_EQ(crc & 0xFF, 0x98);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, ARTIFICIAL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0xB698);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodePictureDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "01"
                                 "1234"
                                 "36"
                                 "8008"
                                 "02"
                                 "0000"
                                 "170718095255"
                                 "05"
                                 "bb17",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xbb);
    ASSERT_EQ(crc & 0xFF, 0x17);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, PICTURE);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xBB17);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodePictureUplinkMessagePkg1)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "01"
                                 "0012345678"
                                 "1234"
                                 "36"
                                 "011C"
                                 "16"
                                 "00D001"
                                 "0005"
                                 "591011161118"
                                 "F1F1"
                                 "0012345678"
                                 "48"
                                 "F0F0"
                                 "5910111611"
                                 "F3F3"
                                 "FFD8FFE000104A46494600010100000100010000FFDB004300090606070606090707070909090A0C150D0C0C0C0C1912130F151E191F1E1C191C1C21242E2721222B221C1C2837282B30313434341F27393D39333C2E333431FFDB0043010909090C0A0C180D0D1831211C213131313131313131313131313131313131313131313131313131313131313131313131313131313131313131313131313131FFC4001F0000010501010101010100000000000000000102030405060708090A0BFFC400B5100002010303020403050504040000017D01020300041105122131410613516107227114328191A1082342B1C11552D1F02433627282090A161718191A"
                                 "17"
                                 "96CB",
                       602);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x96);
    ASSERT_EQ(crc & 0xFF, 0xCB);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, PICTURE);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETB);
    ASSERT_EQ(pkg->tail.crc, 0x96CB);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));
    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodePictureUplinkMessagePkg2)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E01001234567812343601031600D00225262728292A3435363738393A434445464748494A535455565758595A636465666768696A737475767778797A838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE1E2E3E4E5E6E7E8E9EAF1F2F3F4F5F6F7F8F9FAFFC4001F0100030101010101010101010000000000000102030405060708090A0BFFC400B51100020102040403040705040400010277000102031104052131061241510761711322328108144291A1B1C109233352F0156272D10A162434E125F11718191A262728292A35363738393A434445464748494A535455565758595A636465666768696A73741759D8",
                       552);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x59);
    ASSERT_EQ(crc & 0xFF, 0xD8);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, PICTURE);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETB);
    ASSERT_EQ(pkg->tail.crc, 0x59D8);
    ASSERT_EQ(pkg->head.len - PACKAGE_HEAD_SEQUENCE_LEN, BB_Available(((LinkMessage *)pkg)->rawBuff));

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodePictureUplinkMessagePkg3)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E01001234567812343601031600D00375767778797A82838485868788898A92939495969798999AA2A3A4A5A6A7A8A9AAB2B3B4B5B6B7B8B9BAC2C3C4C5C6C7C8C9CAD2D3D4D5D6D7D8D9DAE2E3E4E5E6E7E8E9EAF2F3F4F5F6F7F8F9FAFFC00011080078008003012100021101031101FFDD00040002FFDA000C03010002110311003F00CE5F0D693C7FA20FFBEDBFC6A55F0D692707EC9D3FE9A37F8D5FB79F4667ECE26B410AC3124483088A154649C003039A942E6B26DC9DD9692D8FFFD0D10BF95417FF0025B31F751FA8159CB61A4796EB103DBEAD74B29E4CECE3E8C770FD08AEE3C12A174881474C487FF221A5D110B5933FFFD1D32B4D2B53D02D61856A32B4D09A3FFFD299D7009A8996176456",
                       552);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x64);
    ASSERT_EQ(crc & 0xFF, 0x56);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, PICTURE);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETB);
    ASSERT_EQ(pkg->tail.crc, 0x6456);
    ASSERT_EQ(pkg->head.len - PACKAGE_HEAD_SEQUENCE_LEN, BB_Available(((LinkMessage *)pkg)->rawBuff));

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodePictureUplinkMessagePkg13)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E01001234567812343600F41600D00D471F9D49A5EB66783C8548A268DB01635C2840063033F5FD28E633EA692DE4C41064FC860FE7522CEE461A4623D09AAB81FFD5A1E66697CCF7AA331AD27151349C52B81FFFD6C4792AB4AFC1A0CCA4B22ABAE72BB5B231D3A63FAF6ABF0CA0E39EB4D324FFD7E5655DF01C7A83FAD615B5BDAD9CD2290A240C5031392D5E96395ECFCCE3A2FA162E59552197F78019002147DEC838CE6AB6A23F7A8C3FBBFE35E63DCDCFFFD0F2F0DDB356F4694A6A3807865C7E87FF00AD48C8E995F1D2A457A2E33FFED1C70F4A64C77A0CC6993DE98589A407FFD2E7C8623A544D193458C8AD25A67914C43243F4FD0D023FFFD900B703679B",
                       522);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x67);
    ASSERT_EQ(crc & 0xFF, 0x9B);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, PICTURE);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x679B);
    ASSERT_EQ(pkg->head.len - PACKAGE_HEAD_SEQUENCE_LEN, BB_Available(((LinkMessage *)pkg)->rawBuff));

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryRealtimeDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "01"
                                 "1234"
                                 "37"
                                 "8008"
                                 "02"
                                 "0000"
                                 "170718095404"
                                 "05"
                                 "ABC8",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xAB);
    ASSERT_EQ(crc & 0xFF, 0xC8);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_REALTIME);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xABC8);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryRealtimeUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "01"
                                 "0012345678"
                                 "1234"
                                 "37"
                                 "002B"
                                 "02"
                                 "0009"
                                 "591011161403"
                                 "F1F1"
                                 "0012345678"
                                 "48"
                                 "F0F0"
                                 "5910111614"
                                 "2019"
                                 "000015"
                                 "2619"
                                 "000015"
                                 "3923"
                                 "00000122"
                                 "3812"
                                 "1108"
                                 "03"
                                 "38FE",
                       120);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x38);
    ASSERT_EQ(crc & 0xFF, 0xFE);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_REALTIME);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x38FE);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 4);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0x20);
    ASSERT_EQ(el->dataDef, 0x19);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryTimeRangeDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "10"
                                 "1234"
                                 "38"
                                 "8017"
                                 "02"
                                 "0000"
                                 "130325110533"
                                 "13032410"
                                 "13032510"
                                 "0418"
                                 "000000"
                                 "F460"
                                 "05"
                                 "F800",
                       80);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xf8);
    ASSERT_EQ(crc & 0xFF, 0x00);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_TIMERANGE);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xf800);
    ASSERT_EQ(((DownlinkMessage *)pkg)->messageHead.timeRange.start.year, 13);
    ASSERT_EQ(((DownlinkMessage *)pkg)->messageHead.timeRange.end.year, 13);

    ElementPtrVector *elements = &((DownlinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(el->dataDef, TIME_STEP_CODE_DATADEF);

    TimeStepCodeElement *tscel = (TimeStepCodeElement *)el;
    ASSERT_EQ(tscel->super.identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(tscel->super.dataDef, TIME_STEP_CODE_DATADEF);
    ASSERT_EQ(tscel->timeStepCode.hour, 0);
    ASSERT_EQ(tscel->timeStepCode.minute, 0);
    ASSERT_EQ(tscel->numberListElement.super.identifierLeader, 0xf4);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryTimeRangeUplinkMessage1)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "10"
                                 "0012345678"
                                 "1234"
                                 "38"
                                 "00DE"
                                 "02"
                                 "0031"
                                 "130325110533"
                                 "F1F1"
                                 "0012345678"
                                 "48"
                                 "F0F0"
                                 "1303241000"
                                 "0418"
                                 "000000"
                                 "F460000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                                 "17"
                                 "16F5",
                       478);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x16);
    ASSERT_EQ(crc & 0xFF, 0xF5);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_TIMERANGE);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETB);
    ASSERT_EQ(pkg->tail.crc, 0x16f5);

    ElementPtrVector *elements = &((DownlinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(el->dataDef, TIME_STEP_CODE_DATADEF);

    TimeStepCodeElement *tscel = (TimeStepCodeElement *)el;
    ASSERT_EQ(tscel->super.identifierLeader, TIME_STEP_CODE);
    ASSERT_EQ(tscel->super.dataDef, TIME_STEP_CODE_DATADEF);
    ASSERT_EQ(tscel->timeStepCode.hour, 0);
    ASSERT_EQ(tscel->timeStepCode.minute, 0);
    ASSERT_EQ(tscel->numberListElement.super.identifierLeader, 0xf4);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryArtificalDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E0012345678011234398008020000170718100720056832",
                       50);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x68);
    ASSERT_EQ(crc & 0xFF, 0x32);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ARTIFICIAL);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0x6832);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryArtificalUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E0100123456781234390011020014170718100630F2F25120312E3233340346AA",
                       68);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x46);
    ASSERT_EQ(crc & 0xFF, 0xAA);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ARTIFICIAL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x46AA);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryElementDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0011223344"
                                 "05"
                                 "03E8"
                                 "3A"
                                 "800A"
                                 "02"
                                 "000017"
                                 "0718110230"
                                 "F460"
                                 "05"
                                 "9B7B",
                       54);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x9B);
    ASSERT_EQ(crc & 0xFF, 0x7B);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ELEMENT);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0x9B7B);

    ElementPtrVector *elements = &((DownlinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0xF4);
    ASSERT_EQ(el->dataDef, 0x60);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryElementUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "05"
                                 "0011223344"
                                 "03E8"
                                 "3A"
                                 "0025"
                                 "02"
                                 "003717"
                                 "0718110226"
                                 "F1F1"
                                 "0011223344"
                                 "48"
                                 "F0F0"
                                 "1707181005"
                                 "F460"
                                 "0500000014FFFFFFFFFF0000"
                                 "03"
                                 "074E",
                       108);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x07);
    ASSERT_EQ(crc & 0xFF, 0x4E);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ELEMENT);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x074E);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0xF4);
    ASSERT_EQ(el->dataDef, 0x60);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryElementDownlinkMessage1)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E00112233440503E83A800A020000170718110239F5C005C751",
                       54);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xC7);
    ASSERT_EQ(crc & 0xFF, 0x51);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ELEMENT);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xC751);

    ElementPtrVector *elements = &((DownlinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0xF5);
    ASSERT_EQ(el->dataDef, 0xC0);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryElementUplinkMessage1)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "05"
                                 "0011223344"
                                 "03E8"
                                 "3A"
                                 "0031"
                                 "02"
                                 "003817"
                                 "0718110235"
                                 "F1F1"
                                 "0011223344"
                                 "48"
                                 "F0F0"
                                 "1707181005"
                                 "F5C0"
                                 "000C000C001C00310031FFFFFFFFFFFFFFFFFFFF00310031"
                                 "03"
                                 "3CAF",
                       132);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x3c);
    ASSERT_EQ(crc & 0xFF, 0xaf);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ELEMENT);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x3caf);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 1);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0xF5);
    ASSERT_EQ(el->dataDef, 0xC0);

    // it is a RelativeWaterLevelElement
    RelativeWaterLevelElement *rwlel = (RelativeWaterLevelElement *)el;
    ASSERT_EQ(rwlel->super.identifierLeader, RELATIVE_WATER_LEVEL_5MIN1);
    ASSERT_EQ(rwlel->super.dataDef, 0xc0);
    float fv = 0;
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwlel, 0, &fv));
    ASSERT_EQ(0.12f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwlel, 1, &fv));
    ASSERT_EQ(0.12f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_ValueAt(rwlel, 11, &fv));
    ASSERT_EQ(0.49f, fv);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryElementDownlinkMessage2)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0011223344"
                                 "05"
                                 "03E8"
                                 "3A"
                                 "8012"
                                 "02"
                                 "000017"
                                 "07181102"
                                 "58"
                                 "1F19"
                                 "2019"
                                 "2619"
                                 "3812"
                                 "3923"
                                 "05"
                                 "84B9",
                       70);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x84);
    ASSERT_EQ(crc & 0xFF, 0xB9);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ELEMENT);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0x84B9);

    ElementPtrVector *elements = &((DownlinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 5);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0x1F);
    ASSERT_EQ(el->dataDef, 0x19);

    el = elements->data[4];
    ASSERT_EQ(el->identifierLeader, 0x39);
    ASSERT_EQ(el->dataDef, 0x23);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeQueryElementUplinkMessage2)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "05"
                                 "0011223344"
                                 "03E8"
                                 "3A"
                                 "0030"
                                 "02"
                                 "003917"
                                 "0718110254"
                                 "F1F1"
                                 "0011223344"
                                 "48"
                                 "F0F0"
                                 "1707181102"
                                 "1F19"
                                 "000000"
                                 "2019"
                                 "000040"
                                 "2619"
                                 "000040"
                                 "3812"
                                 "1099"
                                 "3923"
                                 "00010490"
                                 "03"
                                 "EDD0",
                       130);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xED);
    ASSERT_EQ(crc & 0xFF, 0xD0);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_ELEMENT);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0xEDD0);

    ElementPtrVector *elements = &((UplinkMessage *)pkg)->super.elements;
    ASSERT_EQ(elements->length, 5);

    Element *el = elements->data[0];
    ASSERT_EQ(el->identifierLeader, 0x1f);
    ASSERT_EQ(el->dataDef, 0x19);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeModifyBasicDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E"
                                 "0012345678"
                                 "01"
                                 "1234"
                                 "40"
                                 "8046"
                                 "020000"
                                 "1707181015"
                                 "57012005000000022800112233440450022180941531720097080538010130123456780C08020D4080010000000000000F6831303133313132333435363738"
                                 "05"
                                 "E4B9",
                       174);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xE4);
    ASSERT_EQ(crc & 0xFF, 0xB9);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, MODIFY_BASIC_CONFIG);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xE4B9);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeModifyBasicUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E010012345678123440004D02001B170718101458F1F1001234567801200500000002280011223344045002218094153172009708053801A130123456780C08020D4080010000000000000F6831303133313132333435363738032B4C",
                       188);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x2B);
    ASSERT_EQ(crc & 0xFF, 0x4C);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, MODIFY_BASIC_CONFIG);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x2B4C);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadConfigDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E0011223344051234418018020000170718101912012002280310040005000C080D400F0005F2E3",
                       82);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0xF2);
    ASSERT_EQ(crc & 0xFF, 0xE3);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, BASIC_CONFIG);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xF2E3);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadConfigUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E050011223344123441005102001E170718101813F1F100112233440120050000000228001122334403101234045002218094153172009708053801A130123456780C08020D4080010000000000000F6831303133313132333435363738036F60",
                       196);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x6f);
    ASSERT_EQ(crc & 0xFF, 0x60);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, BASIC_CONFIG);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x6f60);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeModifyRuntimeDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E001122334405123442803802000017071810201220080121080522080823100300240805250910260910270802282300010000301B00020038120500401200034112002005348A",
                       146);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x34);
    ASSERT_EQ(crc & 0xFF, 0x8A);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, MODIFY_RUNTIME_CONFIG);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0x348A);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeModifyRuntimeUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E050011223344123442003F02001F170718101913F1F1001122334420080121080522080823100300240805250910260910270802282300010000301B0002003812050040120003411200200342A6",
                       160);
    BB_Flip(byteBuff);

    uint16_t crc = 0;
    BB_CRC16(byteBuff, &crc, 0, BB_Available(byteBuff) - 2);
    ASSERT_EQ(crc >> 8, 0x42);
    ASSERT_EQ(crc & 0xFF, 0xA6);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, MODIFY_RUNTIME_CONFIG);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x42A6);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadRuntimeDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E0011223344051234438022020000170718102034200821082208231024082509260927082823301B381240124112056564",
                       102);
    BB_Flip(byteBuff);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, RUNTIME_CONFIG);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0x6564);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadRuntimeUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E050011223344123443003F020020170718101935F1F1001122334420080121080522080823100300240805250910260910270802282300010000301B00020038120500401200034112002003AF1B",
                       160);
    BB_Flip(byteBuff);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, RUNTIME_CONFIG);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0xAF1B);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadPumpingDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E001122334405123444800802000017071810204805BAD4",
                       50);
    BB_Flip(byteBuff);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_PUMPING_REALTIME);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0xBAD4);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadPumpingUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E050011223344123444002F020021170718101949F1F1001122334448F0F017071810197011FFFF7111FFFF7211FFFF7311FFFF7411FFFF7511FFFF033703",
                       128);
    BB_Flip(byteBuff);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_PUMPING_REALTIME);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0x3703);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadSoftDownlinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E0011223344051234458008020000170718102055056BDF",
                       50);
    BB_Flip(byteBuff);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_SOFTWARE_VERSION);
    ASSERT_EQ(pkg->head.direction, Down);
    ASSERT_EQ(pkg->tail.etxFlag, ENQ);
    ASSERT_EQ(pkg->tail.crc, 0x6bdf);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

GTEST_TEST(Package, decodeReadSoftUplinkMessage)
{
    Package *pkg = NULL;

    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor_fromHexStr(byteBuff, "7E7E0500112233441234450021020022170718101955F1F10011223344114441544138362D53572D56312E3228482903C476",
                       100);
    BB_Flip(byteBuff);

    pkg = decodePackage(byteBuff);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.funcCode, QUERY_SOFTWARE_VERSION);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->tail.etxFlag, ETX);
    ASSERT_EQ(pkg->tail.crc, 0xc476);

    ByteBuffer *encoded = pkg->vptr->encode(pkg);
    BB_Flip(encoded);
    ASSERT_EQ(BB_Available(encoded), pkg->vptr->size(pkg));

    ASSERT_TRUE(BB_Equal(encoded, byteBuff));
    BB_dtor(encoded);
    DelInstance(encoded);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    BB_dtor(byteBuff);
    DelInstance(byteBuff);
}

/***
 * @Todo
            case QUERY_STATUS:
            case INIT_STORAGE:
            case RESET:
            case CHANGE_PASSWORD:
            case SET_CLOCK:
            case SET_IC:
            case PUMPING_SWITCH:
            case VALVE_SWITCH:
            case GATE_SWITCH:
            case WATER_VOLUME_SETTING:
            case QUERY_LOG:
            case QUERY_CLOCK:

7E7E001122334405123446800802000017071810210805F8B3
7E7E0500112233441234460015020023170718102008F1F100112233444520000004000315C1
7E7E00112233440503E847800A020000170718110351970005983C
7E7E05001122334403E847000F02003A170718110347F1F100112233440305BD
7E7E00112233440503E848800A020000170718110437980005D7E8
7E7E05001122334403E848000F0200313170718110432F1F1001122334403B3C3
7E7E001122334405123449801002000017071810212303101234031003E80566B9
7E7E0500112233441234490013020024170718102024F1F10011223344031003E803CEC8
7E7E00112233440503E84A800802000017071810214905749E
7E7E05001122334403E84A000F020025170718102149F1F10011223344032D27
7E7E00112233440503E84B800E0200001707181022374520000002000517C7
7E7E05001122334403E84130015020028170718102235F1F100112233444520000006000352A1
7E7E00112233440503E84C800A020000170718102247010105BD2F
7E7E05001122334403E84C0011020029170718102248F1F10011223344010103D1B3
7E7E00112233440503E84D800A02000017071810225801010569E9
7E7E05001122334403E84D001102002A170718102257F1F1001122334401010339F7
7E7E00112233440503E84E800C020000170718102311010100020513736
7E7E05001122334403E84E001302002B170718102309F1F100112233440101000203869B
7E7E00112233440503E84F8009020000170718102321FF05A90F
7E7E05001122334403E84F001002002C170718102324F1F10011223344FF0332F5
7E7E00112233440503E850800802000017071810234405BE1D
7E7E05001122334403E850004F02002D170718102342F1F1001122334400000002000000000001000000000000000000000000000000000000000000710044000500000000000000000000000000000000000000000000000000000000036B0A 
7E7E00112233440503E851800802000017071810571005E561
7E7E05001122334403E851000F020030170718105706F1F1001122334403CC07

 */

GTEST_TEST(Package, putItTogether)
{
    UplinkMessage *msg = NewInstance(UplinkMessage); // 选择是上行还是下行
    UplinkMessage_ctor(msg, 1);                      // 调用构造函数,如果有要素，需要指定要素数量
    Package *pkg = (Package *)msg;                   // 获取父结构Package
    Head *head = &pkg->head;                         // 获取Head结构
    head->centerAddr = 1;                            // 填写Head结构
    head->funcCode = INTERVAL;                       // 功能码，后续需要根据功能码填写相应的要素或自定包
    //head->......
    msg->messageHead.seq = 1; // 根据功能码填写报文头
    // msg->messageHead.

    NumberElement *nel = NewInstance(NumberElement);  // 根据场景创建Element 或 自定数据
    NumberElement_ctor(nel, 0x20, 0x19);              // 封装了协议里 数值型的 要素结构，标识符 和 数据类型（字节数、精度，此处为“当前降水量”），需要自行填写，没有严格限制 >= 0x01 &&  <= 0x75 && != TIME_STEP_CODE && != STATION_STATUS && != DURATION_OF_XX
    NumberElement_SetFloat(nel, 11.1);                //
    LinkMessage *linkMsg = (LinkMessage *)msg;        // 获取父结构LinkMessage
    LinkMessage_PushElement(linkMsg, (Element *)nel); // 添加LinkMessage

    ByteBuffer *byteOut = pkg->vptr->encode(pkg); // 编码

    // 释放
    UplinkMessage_dtor((Package *)msg);
    DelInstance(msg);

    BB_dtor(byteOut);
    DelInstance(byteOut);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
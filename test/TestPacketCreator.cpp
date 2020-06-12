#include "gtest/gtest.h"

#include "common/class.h"
#include "cJSON/cJSON_Helper.h"
#include "packet_creator.h"

GTEST_TEST(Packet_creator, timingReport)
{
    cJSON *data = cJSON_FromFile("./jsontopkg/timing_report.json");
    Package *pkg = createPackage(data);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->head.funcCode, 0x32);
    LinkMessage *linkMsg = (LinkMessage *)pkg;
    ASSERT_EQ(12, LinkMessage_ElementsSize(linkMsg));
    NumberElement *el = (NumberElement *)LinkMessage_ElementAt(linkMsg, 0);
    ASSERT_EQ(el->super.identifierLeader, 0x26);
    ASSERT_EQ(el->super.dataDef, 0x11);
    float fv = 0;
    ASSERT_FLOAT_EQ(2, NumberElement_GetFloat(el, &fv));
    ASSERT_FLOAT_EQ(4.0f, fv);
    el = (NumberElement *)LinkMessage_ElementAt(linkMsg, 1);
    ASSERT_EQ(el->super.identifierLeader, 0x39);
    ASSERT_EQ(el->super.dataDef, 0x12);
    ASSERT_FLOAT_EQ(2, NumberElement_GetFloat(el, &fv));
    ASSERT_FLOAT_EQ(13.2f, fv);
    el = (NumberElement *)LinkMessage_ElementAt(linkMsg, 2);
    ASSERT_EQ(el->super.identifierLeader, 0x38);
    ASSERT_EQ(el->super.dataDef, 0x12);
    ASSERT_FLOAT_EQ(2, NumberElement_GetFloat(el, &fv));
    ASSERT_FLOAT_EQ(0.06f, fv);
    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    cJSON_Delete(data);
}

GTEST_TEST(Packet_creator, evenTimeReport)
{
    cJSON *data = cJSON_FromFile("./jsontopkg/even_time_report.json");
    Package *pkg = createPackage(data);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->head.funcCode, 0x32);
    LinkMessage *linkMsg = (LinkMessage *)pkg;
    // ASSERT_EQ(12, LinkMessage_ElementsSize(linkMsg));
    TimeStepCodeElement *el = (TimeStepCodeElement *)LinkMessage_ElementAt(linkMsg, 0);
    ASSERT_EQ(el->numberListElement.numbers.length, 13);
    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    cJSON_Delete(data);
}

GTEST_TEST(Packet_creator, rain5Min)
{
    cJSON *data = cJSON_FromFile("./jsontopkg/rain_5_min.json");
    Package *pkg = createPackage(data);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->head.funcCode, 0x34);
    LinkMessage *linkMsg = (LinkMessage *)pkg;
    // ASSERT_EQ(12, LinkMessage_ElementsSize(linkMsg));
    DRP5MINElement *el = (DRP5MINElement *)LinkMessage_ElementAt(linkMsg, 0);
    ASSERT_EQ(BB_Size(el->buff), 12);
    float fv = 0;
    ASSERT_EQ(1, DRP5MINElement_GetValueAt(el, 0, &fv));
    ASSERT_EQ(1.5, fv);
    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    cJSON_Delete(data);
}

GTEST_TEST(Packet_creator, water5Min)
{
    cJSON *data = cJSON_FromFile("./jsontopkg/water_5_min.json");
    Package *pkg = createPackage(data);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->head.funcCode, 0x34);
    LinkMessage *linkMsg = (LinkMessage *)pkg;
    // ASSERT_EQ(12, LinkMessage_ElementsSize(linkMsg));
    RelativeWaterLevelElement *el = (RelativeWaterLevelElement *)LinkMessage_ElementAt(linkMsg, 0);
    ASSERT_EQ(BB_Size(el->buff), 24);
    float fv = 0;
    ASSERT_EQ(2, RelativeWaterLevelElement_GetValueAt(el, 0, &fv));
    ASSERT_EQ(1.15f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_GetValueAt(el, 11, &fv));
    ASSERT_EQ(1.5f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_GetValueAt(el, 9, &fv));
    ASSERT_EQ(1.25f, fv);
    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    cJSON_Delete(data);
}

GTEST_TEST(Packet_creator, hourReport)
{
    cJSON *data = cJSON_FromFile("./jsontopkg/hour_report.json");
    Package *pkg = createPackage(data);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
    ASSERT_EQ(pkg->head.funcCode, 0x34);
    LinkMessage *linkMsg = (LinkMessage *)pkg;
    // Element 0
    RelativeWaterLevelElement *el = (RelativeWaterLevelElement *)LinkMessage_ElementAt(linkMsg, 0);
    ASSERT_EQ(BB_Size(el->buff), 24);
    float fv = 0;
    ASSERT_EQ(2, RelativeWaterLevelElement_GetValueAt(el, 0, &fv));
    ASSERT_EQ(1.15f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_GetValueAt(el, 11, &fv));
    ASSERT_EQ(1.5f, fv);
    ASSERT_EQ(2, RelativeWaterLevelElement_GetValueAt(el, 9, &fv));
    ASSERT_EQ(1.25f, fv);

    // Element 1
    DRP5MINElement *drpel = (DRP5MINElement *)LinkMessage_ElementAt(linkMsg, 1);
    ASSERT_EQ(BB_Size(drpel->buff), 12);
    ASSERT_EQ(1, DRP5MINElement_GetValueAt(drpel, 0, &fv));
    ASSERT_EQ(1.5f, fv);
    ASSERT_EQ(1, DRP5MINElement_GetValueAt(drpel, 11, &fv));
    ASSERT_EQ(1.3f, fv);
    ASSERT_EQ(1, DRP5MINElement_GetValueAt(drpel, 4, &fv));
    ASSERT_EQ(1.1f, fv);

    // Element 2
    ObserveTimeElement *obel = (ObserveTimeElement *)LinkMessage_ElementAt(linkMsg, 2);
    ASSERT_EQ(obel->observeTime.year, 20);
    ASSERT_EQ(obel->observeTime.month, 06);
    ASSERT_EQ(obel->observeTime.day, 10);
    ASSERT_EQ(obel->observeTime.hour, 17);
    ASSERT_EQ(obel->observeTime.minute, 00);

    //NumberElements
    NumberElement *nel = (NumberElement *)LinkMessage_ElementAt(linkMsg, 3);
    ASSERT_EQ(nel->super.identifierLeader, 0x26);
    ASSERT_EQ(nel->super.dataDef, 0x11);
    ASSERT_FLOAT_EQ(2, NumberElement_GetFloat(nel, &fv));
    ASSERT_FLOAT_EQ(4.0f, fv);
    nel = (NumberElement *)LinkMessage_ElementAt(linkMsg, 4);
    ASSERT_EQ(nel->super.identifierLeader, 0x39);
    ASSERT_EQ(nel->super.dataDef, 0x12);
    ASSERT_FLOAT_EQ(2, NumberElement_GetFloat(nel, &fv));
    ASSERT_FLOAT_EQ(13.2f, fv);
    nel = (NumberElement *)LinkMessage_ElementAt(linkMsg, 5);
    ASSERT_EQ(nel->super.identifierLeader, 0x38);
    ASSERT_EQ(nel->super.dataDef, 0x12);
    ASSERT_FLOAT_EQ(2, NumberElement_GetFloat(nel, &fv));
    ASSERT_FLOAT_EQ(0.06f, fv);

    ByteBuffer *buff = pkg->vptr->encode(pkg);
    BB_dtor(buff);
    DelInstance(buff);

    pkg->vptr->dtor(pkg);
    DelInstance(pkg);
    cJSON_Delete(data);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
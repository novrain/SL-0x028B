#include "gtest/gtest.h"

#include "common/class.h"
#include "packet_creator.h"

GTEST_TEST(Packet_creator, timingReport)
{
    cJSON *data = cJSON_Parse("{                            "
                              "    \"fcode\":\"32\",        "
                              "    \"observetime\":\"2006101700\",        "
                              "    \"direction\":0,         "
                              "    \"elements\":[           "
                              "        {                    "
                              "            \"t\":\"number\","
                              "            \"id\":\"26\",   "
                              "            \"vt\":\"11\",   "
                              "            \"v\":4          "
                              "        },                   "
                              "        {                    "
                              "            \"t\":\"number\","
                              "            \"id\":\"39\",   "
                              "            \"vt\":\"12\",   "
                              "            \"v\":13.2       "
                              "        },                   "
                              "        {                    "
                              "            \"t\":\"number\","
                              "            \"id\":\"38\",   "
                              "            \"vt\":\"12\",   "
                              "            \"v\":0.06       "
                              "        }                    "
                              "    ]                        "
                              "}                            ");
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
}

GTEST_TEST(Packet_creator, evenTimeReport)
{
    cJSON *data = cJSON_Parse("{                                         "
                              "    \"fcode\":\"32\",                     "
                              "    \"observetime\":\"2006101700\",       "
                              "    \"direction\":0,                      "
                              "    \"elements\":[                        "
                              "        {                                 "
                              "            \"t\":\"time_step_code\",     "
                              "            \"step\":\"000005\",          "
                              "            \"id\":\"39\",                "
                              "            \"vt\":\"12\",                "
                              "            \"v\":[                       "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                0                         "
                              "            ]                             "
                              "        }                                 "
                              "    ]                                     "
                              "}                                         ");
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
}

GTEST_TEST(Packet_creator, rain5Min)
{
    cJSON *data = cJSON_Parse("{                                         "
                              "    \"fcode\":\"34\",                     "
                              "    \"observetime\":\"2006101700\",       "
                              "    \"direction\":0,                      "
                              "    \"elements\":[                        "
                              "        {                                 "
                              "            \"t\":\"rain_hour_5min\",     "
                              "            \"v\":[                       "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5,                      "
                              "                1.5                       "
                              "            ]                             "
                              "        }                                 "
                              "    ]                                     "
                              "}                                         ");
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
}

GTEST_TEST(Packet_creator, water5Min)
{
    cJSON *data = cJSON_Parse("{                                         "
                              "    \"fcode\":\"34\",                     "
                              "    \"observetime\":\"2006101700\",       "
                              "    \"direction\":0,                      "
                              "    \"elements\":[                        "
                              "        {                                 "
                              "            \"t\":\"water_hour_5min\",    "
                              "            \"id\":\"F5\",                "
                              "            \"v\":[                       "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.25,                     "
                              "                1.5,                      "
                              "                1.5                       "
                              "            ]                             "
                              "        }                                 "
                              "    ]                                     "
                              "}                                         ");
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
}

GTEST_TEST(Packet_creator, water5Min)
{
    cJSON *data = cJSON_Parse("{                                         "
                              "    \"fcode\":\"34\",                     "
                              "    \"observetime\":\"2006101700\",       "
                              "    \"direction\":0,                      "
                              "    \"elements\":[                        "
                              "        {                                 "
                              "            \"t\":\"water_hour_5min\",    "
                              "            \"id\":\"F5\",                "
                              "            \"v\":[                       "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.15,                     "
                              "                1.25,                     "
                              "                1.5,                      "
                              "                1.5                       "
                              "            ]                             "
                              "        }                                 "
                              "    ]                                     "
                              "}                                         ");
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
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
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

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
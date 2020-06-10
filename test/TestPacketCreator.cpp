#include "gtest/gtest.h"

#include "common/class.h"
#include "packet_creator.h"

GTEST_TEST(Packet_creator, factoryLoad)
{
    PacketCreatorFactory *factory = NewInstance(PacketCreatorFactory);
    PacketCreatorFactory_ctor(factory, 10);
    PacketCreatorFactory_loadDirectory(factory, "./schemas/factory");
    ASSERT_TRUE(PacketCreatorFactory_count(factory) == 1);
    PacketCreator *c = PacketCreatorFactory_getPacketCreator(factory, "test_schema");
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(strcmp(PacketCreator_schemaName(c), "test_schema") == 0);
    c = PacketCreatorFactory_loadFile(factory, "./schemas/factory/test_schema.json");
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(strcmp(PacketCreator_schemaName(c), "test_schema") == 0);
    ASSERT_TRUE(PacketCreatorFactory_count(factory) == 1);
    PacketCreatorFactory_dtor(factory);
    DelInstance(factory);
}

GTEST_TEST(Packet_creator, simpleNumberCreator)
{
    PacketCreatorFactory *factory = NewInstance(PacketCreatorFactory);
    PacketCreatorFactory_ctor(factory, 10);
    PacketCreator *c = PacketCreatorFactory_loadFile(factory, "./schemas/simple_number.json");
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(strcmp(PacketCreator_schemaName(c), "simple_number") == 0);
    ASSERT_TRUE(PacketCreatorFactory_count(factory) == 1);
    Package *pkg = PacketCreator_createPacket(c, NULL);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
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
    PacketCreatorFactory_dtor(factory);
    DelInstance(factory);
}

GTEST_TEST(Packet_creator, timingReport)
{
    PacketCreatorFactory *factory = NewInstance(PacketCreatorFactory);
    PacketCreatorFactory_ctor(factory, 10);
    PacketCreator *c = PacketCreatorFactory_loadFile(factory, "./schemas/timing_report.json");
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(strcmp(PacketCreator_schemaName(c), "timing_report") == 0);
    ASSERT_TRUE(PacketCreatorFactory_count(factory) == 1);
    cJSON *data = cJSON_Parse("{                                  "
                              "    \"observeTime\":\"2006101200\","
                              "    \"elements\":{                 "
                              "        \"26\":4,                  "
                              "        \"38\":0.06,               "
                              "        \"39\":13.2                "
                              "    }                              "
                              "}                                  ");
    Package *pkg = PacketCreator_createPacket(c, data);
    ASSERT_TRUE(pkg != NULL);
    ASSERT_EQ(pkg->head.direction, Up);
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
    PacketCreatorFactory_dtor(factory);
    DelInstance(factory);
}

// GTEST_TEST(Packet_creator, evenTimeReport)
// {
//     PacketCreatorFactory *factory = NewInstance(PacketCreatorFactory);
//     PacketCreatorFactory_ctor(factory, 10);
//     PacketCreator *c = PacketCreatorFactory_loadFile(factory, "./schemas/even_time_report.json");
//     ASSERT_TRUE(c != NULL);
//     ASSERT_TRUE(strcmp(PacketCreator_schemaName(c), "even_time_report") == 0);
//     ASSERT_TRUE(PacketCreatorFactory_count(factory) == 1);
//     cJSON *data = cJSON_Parse("{                                       "
//                               "    \"observeTime\":\"2006101199\",     "
//                               "    \"elements\":{                      "
//                               "        \"timeStepCode\":{              "
//                               "            \"step\":\"000005\",        "
//                               "            \"identifierLeader\":\"39\","
//                               "            \"dataDef\":\"12\",         "
//                               "            \"values\":[                "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                1.15,                   "
//                               "                0                       "
//                               "            ]                           "
//                               "        }                               "
//                               "    }                                   "
//                               "}                                       ");
//     Package *pkg = PacketCreator_createPacket(c, data);
//     ASSERT_TRUE(pkg != NULL);
//     ASSERT_EQ(pkg->head.direction, Up);
//     LinkMessage *linkMsg = (LinkMessage *)pkg;
//     ASSERT_EQ(1, LinkMessage_ElementsSize(linkMsg));
//     TimeStepCodeElement *el = (TimeStepCodeElement *)LinkMessage_ElementAt(linkMsg, 0);
//     ASSERT_EQ(el->super.identifierLeader, 0x04);
//     ASSERT_EQ(el->super.dataDef, 0x18);
//     pkg->vptr->dtor(pkg);
//     DelInstance(pkg);
//     PacketCreatorFactory_dtor(factory);
//     DelInstance(factory);
// }

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#include "gtest/gtest.h"

#include "common/class.h"
#include "packet_creator.h"

// a mock storage implement for test.
typedef struct
{
    Storage super;
} MockStorage;

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
    // mock storage
    Storage *storage = (Storage *)NewInstance(MockStorage);
    Package *pkg = PacketCreator_createPacket(c, storage);
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

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
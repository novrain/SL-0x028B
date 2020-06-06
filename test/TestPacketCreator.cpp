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
    ASSERT_TRUE(strcmp(PacketCreator_schemaName(c), "test_schema") == 0);
    ASSERT_TRUE(c != NULL);
    ASSERT_TRUE(PacketCreatorFactory_count(factory) == 1);
    PacketCreatorFactory_dtor(factory);
    DelInstance(factory);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
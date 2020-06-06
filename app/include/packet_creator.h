#ifndef H_PACKET_CREATOR
#define H_PACKET_CREATOR

#ifdef __cplusplus
extern "C"
{
#endif

#include "cJSON/cJSON.h"
#include "vec/vec.h"
#include "tinydir/tinydir.h"
#include "sl651/sl651.h"

    // 伪存储接口，待与采集层对接
    typedef struct Storage
    {
    } Storage;

    // Element Creator，对应一个Element
    typedef struct
    {
        struct ElementCreatorVtbl const *vptr; // 虚函数，由子类实现
        cJSON *schema;
    } ElementCreator;
    typedef vec_t(ElementCreator *) ElementCreatorPtrVector;

    // 虚函数表
    typedef struct ElementCreatorVtbl
    {
        Element *(*createElement)(ElementCreator *const me, Storage *const storage);
    } ElementCreatorVtbl;

    // 包创建器，包头包尾，管理调用ElementCreator
    typedef struct
    {
        cJSON *schema;
        ElementCreatorPtrVector elementCreators;
    } PacketCreator;
    void PacketCreator_ctor(PacketCreator *const me, cJSON *schema);
    void PacketCreator_dtor(PacketCreator *const me);
    const char *PacketCreator_schemaName(PacketCreator *const me);
    Package *PacketCreator_createPacket(PacketCreator *const me, Storage *const storage);
    typedef vec_t(PacketCreator *) PacketCreatorPtrVector;

    typedef struct
    {
        PacketCreatorPtrVector packetCreators;
    } PacketCreatorFactory;
    void PacketCreatorFactory_ctor(PacketCreatorFactory *const me, uint16_t initCreatorCount);
    void PacketCreatorFactory_dtor(PacketCreatorFactory *const me);
    PacketCreator *PacketCreatorFactory_getPacketCreator(PacketCreatorFactory *const me, const char *schemaName);
    void PacketCreatorFactory_loadDirectory(PacketCreatorFactory *const me, const char *directory);
    PacketCreator *PacketCreatorFactory_loadFile(PacketCreatorFactory *const me, const char *directory);
#define PacketCreatorFactory_count(ptr_) (ptr_)->packetCreators.length

#ifdef __cplusplus
}
#endif
#endif
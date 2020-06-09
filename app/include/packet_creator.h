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

    // Element Creator，对应一个Element
    typedef struct
    {
        struct ElementCreatorVtbl const *vptr; // 虚函数，由子类实现
        cJSON *schema;
    } ElementCreator;
    void ElementCreator_ctor(ElementCreator *const me, cJSON *const schema);
    void ElementCreator_dtor(ElementCreator *const me);
    typedef vec_t(ElementCreator *) ElementCreatorPtrVector;

    // 虚函数表
    typedef struct ElementCreatorVtbl
    {

        // 暂时不支持分包
        Element *(*createElement)(ElementCreator *const me, cJSON *const data);
        void (*dtor)(ElementCreator *const me);
    } ElementCreatorVtbl;

    typedef struct
    {
        ElementCreator super;
    } NumberElementCreator;
    void NumberElementCreator_ctor(NumberElementCreator *const me, cJSON *schema);

    // 包创建器，包头包尾，管理调用ElementCreator
    typedef struct
    {
        cJSON *schema;
        // ElementCreatorPtrVector elementCreators;
    } PacketCreator;
    void PacketCreator_ctor(PacketCreator *const me, cJSON *schema);
    void PacketCreator_dtor(PacketCreator *const me);
    const char *PacketCreator_schemaName(PacketCreator *const me);
    Package *PacketCreator_createPacket(PacketCreator *const me, cJSON *const data);
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
    Package *PacketCreatorFactory_createPacket(PacketCreatorFactory *const me, char *const schemaName, cJSON *const data);
#define PacketCreatorFactory_count(ptr_) (ptr_)->packetCreators.length

#ifdef __cplusplus
}
#endif
#endif
#include <assert.h>
#include "cJSON/cJSON_Helper.h"
#include "common/class.h"
#include "packet_creator.h"

// PacketCreator
void PacketCreator_ctor(PacketCreator *const me, cJSON *schema)
{
    assert(me);
    assert(schema);
    me->schema = schema;
}

void PacketCreator_dtor(PacketCreator *const me)
{
    cJSON_Delete(me->schema);
}

Package *PacketCreator_createPacket(PacketCreator *const me, Storage *const storage)
{
    return NULL;
}

const char *PacketCreator_schemaName(PacketCreator *const me)
{
    assert(me);
    assert(me->schema);
    cJSON *schemaName = cJSON_GetObjectItemCaseSensitive(me->schema, "schemaName");
    return schemaName ? schemaName->valuestring : NULL;
}

bool PacketCreator_isSameSchemaName(PacketCreator *const me, PacketCreator *const target)
{
    assert(me);
    assert(target);
    return strcmp(PacketCreator_schemaName(me), PacketCreator_schemaName(target)) == 0;
}
// PacketCreator END

//  PacketCreatorFactory
void PacketCreatorFactory_ctor(PacketCreatorFactory *const me, uint16_t initCreatorCount)
{
    vec_init(&me->packetCreators);
    vec_reserve(&me->packetCreators, initCreatorCount);
}

void PacketCreatorFactory_dtor(PacketCreatorFactory *const me)
{
    size_t i;
    PacketCreator *c = NULL;
    vec_foreach(&me->packetCreators, c, i)
    {
        if (c != NULL)
        {
            PacketCreator_dtor(c);
        }
    }
}

PacketCreator *PacketCreatorFactory_getPacketCreator(PacketCreatorFactory *const me, const char *schemaName)
{
    assert(me);
    assert(schemaName);
    size_t i;
    PacketCreator *c = NULL;
    bool isExist = false;
    vec_foreach(&me->packetCreators, c, i)
    {
        if (c != NULL && strcmp(PacketCreator_schemaName(c), schemaName) == 0)
        {
            isExist = true;
            break;
        }
    }
    return isExist ? c : NULL;
}

void PacketCreatorFactory_pushPacketCreator(PacketCreatorFactory *const me, PacketCreator *const packetCreator)
{
    assert(me);
    assert(packetCreator);
    size_t i;
    PacketCreator *c = NULL;
    bool isExist = false;
    vec_foreach(&me->packetCreators, c, i)
    {
        if (c != NULL && PacketCreator_isSameSchemaName(c, packetCreator)) // 相同的更新
        {
            PacketCreator_dtor(c);
            c = packetCreator;
            isExist = true;
            break;
        }
    }
    if (!isExist)
    {
        vec_push(&me->packetCreators, packetCreator);
    }
}

void PacketCreatorFactory_loadDirectory(PacketCreatorFactory *const me, const char *directory)
{
    assert(me);
    assert(directory);
    tinydir_dir dir;
    int i;
    tinydir_open_sorted(&dir, directory);
    for (i = 0; i < dir.n_files; i++)
    {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);
        if (!file.is_dir && strcmp(file.extension, "json") == 0)
        {
            PacketCreatorFactory_loadFile(me, file.path);
        }
    }
    tinydir_close(&dir);
}

PacketCreator *PacketCreatorFactory_loadFile(PacketCreatorFactory *const me, const char *schemaFile)
{
    assert(me);
    assert(schemaFile);
    PacketCreator *packetCreator = NULL;
    cJSON *schema = cJSON_FromFile(schemaFile);
    if (schema != NULL)
    {
        cJSON *functionCode = cJSON_GetObjectItemCaseSensitive(schema, "functionCode");
        cJSON *schemaName = cJSON_GetObjectItemCaseSensitive(schema, "schemaName");
        if ((functionCode != NULL &&
             (functionCode->type == cJSON_String || functionCode->type == cJSON_Number)) &&
            (schemaName != NULL && schemaName->type == cJSON_String))
        {
            packetCreator = NewInstance(PacketCreator);
            PacketCreator_ctor(packetCreator, schema);
            PacketCreatorFactory_pushPacketCreator(me, packetCreator);
        }
    }
    return packetCreator;
}
//  PacketCreatorFactory END
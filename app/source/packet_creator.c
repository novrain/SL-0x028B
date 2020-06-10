#include <assert.h>
#include <stdio.h>
#include "cJSON/cJSON_Helper.h"
#include "common/class.h"
#include "packet_creator.h"

// Element Creators
// ElementCreator Interface
void ElementCreator_ctor(ElementCreator *const me, cJSON *const schema)
{
    assert(me);
    assert(schema);
    me->schema = schema;
}
void ElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    // do nothing
}
// ElementCreator Interface END

// NumberElementCreator
Element *NumberElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    assert(me->schema);
    // assert(data);
    cJSON *schema = me->schema;
    cJSON_GET_NUMBER(identifierLeader, uint8_t, schema, 0, 16);
    cJSON_GET_NUMBER(dataDef, uint8_t, schema, 0, 16);
    cJSON_GET_NUMBER(supportSignedFlag, bool, schema, 0, 16);
    cJSON_GET_VALUE(dataSource, char *, schema, valuestring, NULL);
    if (identifierLeader == 0 || dataDef == 0)
    {
        return NULL;
    }
    double v = 0;
    if (dataSource != NULL && strcmp(dataSource, "valueField") == 0)
    {
        cJSON_COPY_VALUE(v, value, schema, valuedouble);
    }
    else // Get data from data
    {
        if (data != NULL)
        {
            char strIdentifier[5] = {0};
            snprintf(strIdentifier, 20, "%02X", identifierLeader);
            cJSON *valJSON = cJSON_GetObjectItem(data, strIdentifier);
            if (valJSON != NULL)
            {
                if (valJSON->type == cJSON_Object)
                {
                    cJSON_COPY_VALUE(v, value, valJSON, valuedouble);
                }
                else if (valJSON->type == cJSON_Number)
                {
                    v = valJSON->valuedouble;
                }
                else
                {
                    return NULL;
                }
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }
    Element *el = (Element *)NewInstance(NumberElement);
    NumberElement_ctor((NumberElement *)el, identifierLeader, dataDef, supportSignedFlag);
    if ((dataDef & NUMBER_ELEMENT_PRECISION_MASK) == 0)
    {
        NumberElement_SetInteger((NumberElement *)el, v);
    }
    else
    {
        NumberElement_SetDouble((NumberElement *)el, v);
    }
    return el;
}

void NumberElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void NumberElementCreator_ctor(NumberElementCreator *const me, cJSON *schema)
{
    assert(me);
    assert(schema);
    static ElementCreatorVtbl const vtbl = {
        &NumberElementCreator_createElement,
        &NumberElementCreator_dtor};
    ElementCreator_ctor(&me->super, schema);
    me->super.vptr = &vtbl;
}
// NumberElementCreator END

// TimeStepCodeElementCreator
Element *TimeStepCodeElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    assert(me->schema);
    assert(data);
    cJSON *schema = me->schema;
    cJSON *tscData = cJSON_GetObjectItem(data, "");
}

void TimeStepCodeElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void TimeStepCodeElementCreator_ctor(TimeStepCodeElementCreator *const me, cJSON *schema)
{
    assert(me);
    assert(schema);
    static ElementCreatorVtbl const vtbl = {
        &TimeStepCodeElementCreator_createElement,
        &TimeStepCodeElementCreator_dtor};
    ElementCreator_ctor(&me->super, schema);
    me->super.vptr = &vtbl;
}
// TimeStepCodeElementCreator END
// Element Creators END

// PacketCreator
void PacketCreator_ctor(PacketCreator *const me, cJSON *schema)
{
    assert(me);
    assert(schema);
    me->schema = schema;
}

void PacketCreator_dtor(PacketCreator *const me)
{
    assert(me);
    cJSON_Delete(me->schema);
}

Package *PacketCreator_createPacket(PacketCreator *const me, cJSON *const data)
{
    assert(me);
    assert(me->schema);
    // assert(data);
    Package *pkg = NULL;
    LinkMessage *linkMsg = NULL;
    cJSON *schema = me->schema;
    // functionCode 不做有效性判断
    cJSON_GET_NUMBER(functionCode, uint8_t, schema, 0, 16);
    cJSON_GET_VALUE(direction, Direction, schema, valuedouble, Up);
    if (direction == Down)
    {
        DownlinkMessage *msg = NewInstance(DownlinkMessage);
        DownlinkMessage_ctor((DownlinkMessage *)msg, DEFAULT_ELEMENT_NUMBER);
        pkg = (Package *)msg;
        linkMsg = (LinkMessage *)msg;
    }
    else
    {
        UplinkMessage *msg = NewInstance(UplinkMessage);
        UplinkMessage_ctor((UplinkMessage *)msg, DEFAULT_ELEMENT_NUMBER);
        pkg = (Package *)msg;
        linkMsg = (LinkMessage *)msg;
        cJSON_GET_VALUE(observeTime, char *, data, valuestring, NULL);
        if (observeTime != NULL && strlen(observeTime) == 10)
        {
            ByteBuffer buff = {0};
            BB_ctor_fromHexStr(&buff, observeTime, 10);
            BB_Flip(&buff);
            ObserveTime_Decode(&msg->messageHead.observeTimeElement.observeTime, &buff);
        }
        else
        {
            ObserveTime_now(&msg->messageHead.observeTimeElement.observeTime);
        }
    }
    bool validSchema = true;
    cJSON *elements = cJSON_GetObjectItemCaseSensitive(schema, "elements");
    cJSON *elementsData = cJSON_GetObjectItemCaseSensitive(data, "elements");
    if (elements != NULL && elements->type == cJSON_Array && elementsData != NULL)
    {
        cJSON *elementSchema;
        cJSON_ArrayForEach(elementSchema, elements)
        {
            cJSON_GET_VALUE(type, char *, elementSchema, valuestring, NULL);
            if (type == NULL)
            {
                validSchema = false;
                break;
            }
            ElementCreator *ec = NULL;
            // switch case ... 每种类型，创建对应的ElementCreator
            if (strcmp(type, "number") == 0)
            {
                ec = (ElementCreator *)(NewInstance(NumberElementCreator)); // 创建指针，需要转为Element*
                NumberElementCreator_ctor((NumberElementCreator *)ec, elementSchema);
            }
            if (strcmp(type, "time_step_code") == 0)
            {
                ec = (ElementCreator *)(NewInstance(TimeStepCodeElementCreator)); // 创建指针，需要转为Element*
                TimeStepCodeElementCreator_ctor((TimeStepCodeElementCreator *)ec, elementSchema);
            }
            // 无效的Element配置
            if (ec == NULL)
            {
                validSchema = false;
                break;
            }
            Element *el = ec->vptr->createElement(ec, elementsData);
            // 无法创建Element
            if (el == NULL)
            {
                validSchema = false;
                ec->vptr->dtor(ec);
                DelInstance(ec);
                break;
            }
            el->direction = direction;
            LinkMessage_PushElement(linkMsg, el);
        }
    }
    if (validSchema)
    {
        pkg->head.funcCode = functionCode;
        // 其他字段由Channel填写 @see Channel_FillUplinkMessageHead
        //
    }
    else
    {
        pkg->vptr->dtor(pkg);
        DelInstance(pkg);
    }
    return pkg;
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
            DelInstance(c);
        }
    }
    vec_deinit(&me->packetCreators);
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
            DelInstance(c);
            // 修改数据，直接访问方式
            me->packetCreators.data[i] = packetCreator;
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

Package *PacketCreatorFactory_createPacket(PacketCreatorFactory *const me, char *const schemaName, cJSON *const data)
{
    assert(me);
    assert(schemaName);
    assert(data);
    PacketCreator *creator = PacketCreatorFactory_getPacketCreator(me, schemaName);
    if (creator == NULL)
    {
        return NULL;
    }
    return PacketCreator_createPacket(creator, data);
}
//  PacketCreatorFactory END
#include <assert.h>

#include "sl651.h"

// "AbstractClass" Package
/* purely-virtual */
static bool Package_Virtual_Encode2Hex(Package const *const me, ByteBuffer *hexBuff)
{
    assert(0);
    return true;
}
static bool Package_Virtual_DecodeFromHex(Package const *const me, ByteBuffer *hexBuff)
{
    assert(0);
    return true;
}
static size_t Package_Virtual_Size()
{
    assert(0);
    return 0;
}

/* Package Constructor */
void Package_ctor(Package *me, Head *head)
{
    assert(me);
    static PackageVtbl const vtbl = {
        &Package_Virtual_Encode2Hex,
        &Package_Virtual_DecodeFromHex,
        &Package_Virtual_Size};
    me->vptr = &vtbl;
    me->head = head;
}

/* Methods  & Destrucor */
bool Package_Head2Hex(Package const *const me, ByteBuffer *hexBuff)
{
    return true;
}

bool Package_Tail2Hex(Package const *const me, ByteBuffer *hexBuff)
{
    return true;
}

bool Package_Hex2Head(Package const *me, ByteBuffer *hexBuff)
{
    return true;
}

bool Package_Hex2Tail(Package const *me, ByteBuffer *hexBuff)
{
    return true;
}

// "AbstractClass" Package END

/* LinkMessage Construtor & Destrucor */
void LinkMessage_ctor(LinkMessage *const me, Head *head, uint16_t elementCount)
{
}
void LinkMessage_dtor(LinkMessage *const me)
{
}

/* Public methods */
bool LinkMessage_Elements2Hex(LinkMessage const *const me, ByteBuffer *hexBuff)
{
    return true;
}

bool LinkMessage_Hex2Elements(LinkMessage const *me, ByteBuffer *hexBuff)
{
    return true;
}

bool LinkMessage_putElement(LinkMessage const *me, uint16_t index, Element *element)
{
    return true;
}

bool LinkMessage_setElement(LinkMessage const *me, uint16_t index, Element *element)
{
    return true;
}
Element *LinkMessage_getElement(LinkMessage const *me, uint16_t index)
{
    return NULL;
}
// "Basic" LinkMessage END

// "AbstractUpClass" UplinkMessage
/* UplinkMessage Construtor & Destrucor */
void UplinkMessage_ctor(UplinkMessage *const me, Head *head, UplinkMessageHead *upLinkHead, uint16_t elementCount)
{
}
void UplinkMessage_dtor(UplinkMessage *const me)
{
}
/* Public methods */
bool UplinkMessage_Head2Hex(UplinkMessage const *const me, ByteBuffer *hexBuff)
{
    return true;
}
bool UplinkMessage_Hex2Head(UplinkMessage const *me, ByteBuffer *hexBuff)
{
    return true;
}
// "AbstractUpClass" UplinkMessage END

// "AbstractUpClass" DownlinkMessage

/* DownlinkMessage Construtor  & Destrucor */
void DownlinkMessage_ctor(DownlinkMessage *const me, Head *head, DownlinkMessageHead *downLinkHead, uint16_t elementCount)
{
}
void DownlinkMessage_dtor(DownlinkMessage *const me)
{
}
/* Public methods */
bool DownlinkMessage_Head2Hex(DownlinkMessage const *const me, ByteBuffer *hexBuff)
{
    return true;
}
bool DownlinkMessage_Hex2Head(DownlinkMessage const *me, ByteBuffer *hexBuff)
{
    return true;
}
// "AbstractUpClass" DownlinkMessage END

// Elements
// "AbstractorClass" Element
static bool Element_Virtual_Encode2Hex(Element const *const me, ByteBuffer *hexBuff)
{
    assert(0);
}
static bool Element_Virtual_DcodeFromHex(Element const *const me, ByteBuffer *hexBuff)
{
    assert(0);
}
size_t Element_Virtual_SizeInHex(Element const *const me)
{
    assert(0);
    return 0;
}
void Element_ctor(Element *me)
{
    assert(me);
    static ElementVtbl const vtbl = {
        &Element_Virtual_Encode2Hex,
        &Element_Virtual_DcodeFromHex,
        &Element_Virtual_SizeInHex};
}
// "AbstractorClass" Element END

// ObserveTimeElement
static bool ObserveTimeElement_Encode2Hex(Element const *const me, ByteBuffer *hexBuff)
{
    assert(me);
}

static bool ObserveTimeElement_DcodeFromHex(Element const *me, ByteBuffer *hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < DATETIME_LEN * 2)
    {
        return false;
    }
    ObserveTimeElement *self = (ObserveTimeElement *)me;
    uint8_t usedLen = 0;
    usedLen += ByteBuffer_HexGetUInt8(hexBuff, &self->observeTime.year);
    usedLen += ByteBuffer_HexGetUInt8(hexBuff, &self->observeTime.month);
    usedLen += ByteBuffer_HexGetUInt8(hexBuff, &self->observeTime.day);
    usedLen += ByteBuffer_HexGetUInt8(hexBuff, &self->observeTime.hour);
    usedLen += ByteBuffer_HexGetUInt8(hexBuff, &self->observeTime.minute);
    usedLen += ByteBuffer_HexGetUInt8(hexBuff, &self->observeTime.second);
    return usedLen == 12;
}

static size_t ObserveTimeElement_SizeInHex(Element const *const me)
{
    assert(me);
    return 14; // IDNENTIFIER 2 + ADDRESS 5 in hex
}

void ObserveTimeElement_ctor(ObserveTimeElement *me)
{
    // override
    static ElementVtbl const vtbl = {
        &ObserveTimeElement_Encode2Hex,
        &ObserveTimeElement_DcodeFromHex,
        &ObserveTimeElement_SizeInHex};
    Element_ctor(&me->super);
    me->super.vptr = &vtbl;
    me->super.identifierLeader = DATETIME;
    me->super.dataDef = DATETIME;
}
// ObserveTimeElement END

// NumberElement
void NumberElement_ctor(NumberElement const *me)
{
}
// NumberElement END

// ByteBuffer should be in read mode
Element *decodeElementFromHex(ByteBuffer *hexBuff)
{
    if (hexBuff == NULL || ByteBuffer_Available(hexBuff) < ELEMENT_IDENTIFER_LEN * 2)
    {
        return NULL;
    }                                                      // 至少包含标识符
    uint8_t identifierLeader = 0;                          //
    ByteBuffer_HexGetUInt8(hexBuff, &identifierLeader);    // 解析一个字节的 标识符引导符 ， 同时位移
    uint8_t dataDef = 0;                                   //
    ByteBuffer_HexGetUInt8(hexBuff, &dataDef);             // 解析一个字节的 数据定义符，同时位移
    Element *el = NULL;                                    // 根据标识符引导符，开始解析 Element
    bool decoded = false;                                  //
    switch (identifierLeader)                              //
    {                                                      //
    case CUSTOM_IDENTIFIER:                                // unsupport
        return NULL;                                       // 返回NULL
    case DATETIME:                                         // 观测时间 Element
        el = (Element *)(NewInstance(ObserveTimeElement)); // 创建指针，需要转为Element*
        ObserveTimeElement_ctor((ObserveTimeElement *)el); // 构造函数
        decoded = el->vptr->decodeFromHex(el,              // 解析观测时间Element，调用 “重载” 的解码方法
                                          hexBuff);        // 长度需要缩短
        if (!decoded)                                      // 解析失败，需要手动删除指针
        {                                                  //
            ObserveTimeElement_dtor(el);                   // 调用析构，规范步骤
            DelInstance(el);                               // 删除指针
            return NULL;                                   //
        }                                                  //
        return el;
    case ADDRESS:
        break;
    case ARTIFICIAL_IL:
        break;
    case PICTURE_IL:
        break;
    case DPR:
        break;
    case DRZ1:
    case DRZ2:
    case DRZ3:
    case DRZ4:
    case DRZ5:
    case DRZ6:
    case DRZ7:
    case DRZ8:
        break;
    case FLOW_RATE_DATA:
        break;
    default:
        // 按照数据类型解析
        if (identifierLeader >= 0x01 && identifierLeader <= 0x75 &&
            identifierLeader != DRXNN &&
            identifierLeader != STAION_STATUS &&
            identifierLeader != DURATION_OF_XX)
        {
            // 数据区
            uint8_t lenRequired = dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
            if (lenRequired * 2 + ELEMENT_IDENTIFER_LEN * 2 > ByteBuffer_Available(hexBuff))
            {
                return NULL;
            }
            el = (Element *)NewInstance(NumberElement);
            NumberElement_ctor((NumberElement *)el);
            el->identifierLeader = identifierLeader;
            el->dataDef = dataDef;
            decoded = el->vptr->decodeFromHex(el, hexBuff);
            if (!decoded)
            {
                NumberElement_dtor(el);
                DelInstance(el);
                return NULL;
            }
            return el;
        }
        else
        {
            return NULL;
        }
    }
}

// Elements END
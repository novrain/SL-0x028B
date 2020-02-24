#include <assert.h>
#include <math.h>

#include "sl651.h"

// "AbstractClass" Package
/* purely-virtual */
static bool Package_Virtual_Encode2Hex(Package const *const me, ByteBuffer *const hexBuff)
{
    assert(0);
    return true;
}
static bool Package_Virtual_DecodeFromHex(Package *const me, ByteBuffer *const hexBuff)
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
bool Package_Head2Hex(Package const *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool Package_Tail2Hex(Package const *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool Package_Hex2Head(Package *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool Package_Hex2Tail(Package *const me, ByteBuffer *const hexBuff)
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
bool LinkMessage_Elements2Hex(LinkMessage const *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool LinkMessage_Hex2Elements(LinkMessage *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool LinkMessage_putElement(LinkMessage *const me, uint16_t index, Element *element)
{
    return true;
}

bool LinkMessage_setElement(LinkMessage *const me, uint16_t index, Element *element)
{
    return true;
}
Element *LinkMessage_getElement(LinkMessage const *const me, uint16_t index)
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
bool UplinkMessage_Head2Hex(UplinkMessage const *const me, ByteBuffer *const hexBuff)
{
    return true;
}
bool UplinkMessage_Hex2Head(UplinkMessage const *me, ByteBuffer *const hexBuff)
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
bool DownlinkMessage_Head2Hex(DownlinkMessage const *const me, ByteBuffer *const hexBuff)
{
    return true;
}
bool DownlinkMessage_Hex2Head(DownlinkMessage const *me, ByteBuffer *const hexBuff)
{
    return true;
}
// "AbstractUpClass" DownlinkMessage END

// Elements
// "AbstractorClass" Element
static bool Element_Virtual_Encode2Hex(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(0);
}
static bool Element_Virtual_DecodeFromHex(Element *const me, ByteBuffer *const hexBuff)
{
    assert(0);
}
size_t Element_Virtual_SizeInHex(Element const *const me)
{
    assert(0);
    return 0;
}
void Element_ctor(Element *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    assert(me);
    static ElementVtbl const vtbl = {
        &Element_Virtual_Encode2Hex,
        &Element_Virtual_DecodeFromHex,
        &Element_Virtual_SizeInHex};
    me->vptr = &vtbl;
    me->identifierLeader = identifierLeader;
    me->dataDef = dataDef;
}
// "AbstractorClass" Element END

// ObserveTimeElement
static bool ObserveTimeElement_Encode2Hex(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
}

static bool ObserveTimeElement_DecodeFromHex(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < DATETIME_LEN * 2)
    {
        return false;
    }
    ObserveTimeElement *self = (ObserveTimeElement *)me;
    uint8_t usedLen = 0;
    usedLen += ByteBuffer_HEXGetUInt8(hexBuff, &self->observeTime.year);
    usedLen += ByteBuffer_HEXGetUInt8(hexBuff, &self->observeTime.month);
    usedLen += ByteBuffer_HEXGetUInt8(hexBuff, &self->observeTime.day);
    usedLen += ByteBuffer_HEXGetUInt8(hexBuff, &self->observeTime.hour);
    usedLen += ByteBuffer_HEXGetUInt8(hexBuff, &self->observeTime.minute);
    usedLen += ByteBuffer_HEXGetUInt8(hexBuff, &self->observeTime.second);
    return usedLen == 12;
}

static size_t ObserveTimeElement_SizeInHex(Element const *const me)
{
    assert(me);
    return 14; // IDNENTIFIER 2 + ADDRESS 5 in hex
}

void ObserveTimeElement_ctor(ObserveTimeElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &ObserveTimeElement_Encode2Hex,
        &ObserveTimeElement_DecodeFromHex,
        &ObserveTimeElement_SizeInHex};
    Element_ctor(&me->super, DATETIME, DATETIME);
    me->super.vptr = &vtbl;
}
// ObserveTimeElement END

// PictureElement

static bool PictureElement_Encode2Hex(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
}

static bool PictureElement_DecodeFromHex(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < 0) // 截取所有
    {
        return false;
    }
    PictureElement *self = (PictureElement *)me;
    // @Todo 是否需要直接转为二进制
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, ByteBuffer_Available(hexBuff));
    return true;
}

static size_t PictureElement_SizeInHex(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN * 2 + ((PictureElement *)me)->buff->size * 2;
}

void PictureElement_ctor(PictureElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &PictureElement_Encode2Hex,
        &PictureElement_DecodeFromHex,
        &PictureElement_SizeInHex};
    Element_ctor(&me->super, PICTURE_IL, PICTURE_IL);
    me->super.vptr = &vtbl;
}

void PictureElement_dtor(PictureElement *const me)
{
    if (me == NULL)
    {
        return;
    }
    if (me->buff)
    {
        ByteBuffer_dtor(me->buff);
        DelInstance(me->buff);
    }
}
// PictureElement END

// NumberElement
static bool NumberElement_Encode2Hex(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
}

static bool NumberElement_DecodeFromHex(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL)
    {
        return false;
    }
    uint8_t size = me->dataDef >> 3;
    if (ByteBuffer_Available(hexBuff) < size * 2)
    {
        return false;
    }
    NumberElement *self = (NumberElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, size * 2); //read all bcd
    if (self->buff == NULL)
    {
        return false;
    }
    ByteBuffer_Flip(self->buff); // Flip to read it.
    return true;
}

static size_t NumberElement_SizeInHex(Element const *const me)
{
    return ELEMENT_IDENTIFER_LEN * 2 + (me->dataDef >> NUMBER_ELEMENT_LEN_OFFSET) * 2;
}

void NumberElement_ctor(NumberElement *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    static ElementVtbl const vtbl = {
        &NumberElement_Encode2Hex,
        &NumberElement_DecodeFromHex,
        &NumberElement_SizeInHex};
    Element_ctor(&me->super, identifierLeader, dataDef);
    me->super.vptr = &vtbl;
}

void NumberElement_dtor(NumberElement *const me)
{
    if (me != NULL && me->buff != NULL)
    {
        DelInstance(me->buff);
    }
}

uint8_t NumberElement_GetInteger(NumberElement *const me, uint64_t *val)
{
    if (me == NULL || me->buff == NULL)
    {
        return 0;
    }
    uint8_t signedFlag = 0;
    ByteBuffer_GetUInt8(me->buff, &signedFlag);
    if (signedFlag != 0xFF) // 如果不是负值
    {
        ByteBuffer_Rewind(me->buff);
    }
    uint8_t bitLen = ByteBuffer_Available(me->buff) / 2;
    *val = 0; // 副作用
    uint8_t b = 0;
    while (bitLen > 0)
    {
        ByteBuffer_HEXGetUInt8(me->buff, &b);
        if (b == 0)
        {
            bitLen--;
            continue;
        }
        uint8_t h = b >> 4;
        uint8_t l = b & 0xF;
        if (h > 9 || l > 9) //非法
        {
            return 0;
        }
        if (h != 0)
        {
            *val += h * pow(10, bitLen * 2 - 1);
        }
        if (l != 0)
        {
            *val += l * pow(10, bitLen * 2 - 2);
        }
        bitLen--;
    }
    if (signedFlag == 0xFF)
    {
        *val *= -1;
    }
    return 1;
}

uint8_t NumberElement_GetFloat(NumberElement *const me, float *val)
{
    uint64_t u64 = 0;
    uint8_t res = NumberElement_GetInteger(me, &u64);
    uint8_t precision = me->super.dataDef & NUMBER_ELEMENT_PRECISION_MASK;
    *val = u64;
    if (precision > 0)
    {
        *val = u64 / pow(10, precision);
    }
    return res;
}
// NumberElement END

// ByteBuffer should be in read mode
Element *decodeElementFromHex(ByteBuffer *const hexBuff)
{
    if (hexBuff == NULL || ByteBuffer_Available(hexBuff) < ELEMENT_IDENTIFER_LEN * 2)
    {
        return NULL;
    }                                                      // 至少包含标识符
    uint8_t identifierLeader = 0;                          //
    ByteBuffer_HEXGetUInt8(hexBuff, &identifierLeader);    // 解析一个字节的 标识符引导符 ， 同时位移
    uint8_t dataDef = 0;                                   //
    ByteBuffer_HEXGetUInt8(hexBuff, &dataDef);             // 解析一个字节的 数据定义符，同时位移
    Element *el = NULL;                                    // 根据标识符引导符，开始解析 Element
    bool decoded = false;                                  //
    switch (identifierLeader)                              //
    {                                                      //
    case CUSTOM_IDENTIFIER:                                // unsupport
        return NULL;                                       // 返回NULL
    case DATETIME:                                         // 观测时间 Element
        el = (Element *)(NewInstance(ObserveTimeElement)); // 创建指针，需要转为Element*
        ObserveTimeElement_ctor((ObserveTimeElement *)el); // 构造函数
        decoded = el->vptr->decodeFromHex(el, hexBuff);    // 解析观测时间Element，调用 “重载” 的解码方法
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
        el = (Element *)(NewInstance(PictureElement));  // 创建指针，需要转为Element*
        PictureElement_ctor((PictureElement *)el);      // 构造函数
        decoded = el->vptr->decodeFromHex(el, hexBuff); // 解析观测时间Element，调用 “重载” 的解码方法
        if (!decoded)                                   // 解析失败，需要手动删除指针
        {                                               //
            PictureElement_dtor((PictureElement *)el);  // 调用析构，规范步骤
            DelInstance(el);                            // 删除指针
            return NULL;                                //
        }                                               //
        return el;
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
            el = (Element *)NewInstance(NumberElement);
            NumberElement_ctor((NumberElement *)el, identifierLeader, dataDef);
            el->identifierLeader = identifierLeader;
            el->dataDef = dataDef;
            decoded = el->vptr->decodeFromHex(el, hexBuff);
            if (!decoded)
            {
                NumberElement_dtor((NumberElement *)el);
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
    return el;
}

// Elements END
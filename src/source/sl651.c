#include <assert.h>
#include <math.h>

#include "sl651.h"

// "AbstractClass" Package
/* purely-virtual */
static bool Package_Virtual_Encode(Package const *const me, ByteBuffer *const hexBuff)
{
    assert(0);
    return true;
}
static bool Package_Virtual_Decode(Package *const me, ByteBuffer *const hexBuff)
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
        &Package_Virtual_Encode,
        &Package_Virtual_Decode,
        &Package_Virtual_Size};
    me->vptr = &vtbl;
    me->head = head;
}

/* Methods  & Destrucor */
bool Package_EncodeHead(Package const *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool Package_EncodeTail(Package const *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool Package_DecodeHead(Package *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool Package_DecodeTail(Package *const me, ByteBuffer *const hexBuff)
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
bool LinkMessage_EncodeElements(LinkMessage const *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool LinkMessage_DecodeElements(LinkMessage *const me, ByteBuffer *const hexBuff)
{
    return true;
}

bool LinkMessage_PutElement(LinkMessage *const me, uint16_t index, Element *element)
{
    return true;
}

bool LinkMessage_SetElement(LinkMessage *const me, uint16_t index, Element *element)
{
    return true;
}
Element *LinkMessage_GetElement(LinkMessage const *const me, uint16_t index)
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
bool UplinkMessage_EncodeHead(UplinkMessage const *const me, ByteBuffer *const hexBuff)
{
    return true;
}
bool UplinkMessage_DecodeHead(UplinkMessage const *me, ByteBuffer *const hexBuff)
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
bool DownlinkMessage_EncodeHead(DownlinkMessage const *const me, ByteBuffer *const hexBuff)
{
    return true;
}
bool DownlinkMessage_DecodeHead(DownlinkMessage const *me, ByteBuffer *const hexBuff)
{
    return true;
}
// "AbstractUpClass" DownlinkMessage END

// Elements
// "AbstractorClass" Element
static bool Element_Virtual_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(0);
    return true;
}

static bool Element_Virtual_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    assert(0);
    return true;
}

size_t Element_Virtual_Size(Element const *const me)
{
    assert(0);
    return 0;
}
void Element_ctor(Element *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    assert(me);
    static ElementVtbl const vtbl = {
        &Element_Virtual_Encode,
        &Element_Virtual_Decode,
        &Element_Virtual_Size};
    me->vptr = &vtbl;
    me->identifierLeader = identifierLeader;
    me->dataDef = dataDef;
}
// "AbstractorClass" Element END

// RemoteStationAddrElement
static bool RemoteStationAddrElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool RemoteStationAddrElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < REMOTE_STATION_ADDR_LEN)
    {
        return false;
    }
    RemoteStationAddrElement *self = (RemoteStationAddrElement *)me;
    uint8_t usedLen = 0;
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->stationAddr.A5);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->stationAddr.A4);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->stationAddr.A3);
    if (self->stationAddr.A5 == A5_HYDROLOGICAL_TELEMETRY_STATION)
    {
        usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->stationAddr.A2);
        usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->stationAddr.A1);
    }
    else
    {
        uint16_t u16A2A1 = 0;
        uint8_t len = ByteBuffer_BE_GetUInt16(hexBuff, &u16A2A1);
        if (len == 2) // 2 byte
        {
            self->stationAddr.A2 = u16A2A1 / 10000;
            self->stationAddr.A1 = u16A2A1 / 100 - self->stationAddr.A2 * 10000;
            self->stationAddr.A0 = u16A2A1 - self->stationAddr.A2 * 10000 - self->stationAddr.A1 * 100;
            usedLen += len;
        }
        else
        {
            return false;
        }
    }
    return usedLen == REMOTE_STATION_ADDR_LEN;
}

static size_t RemoteStationAddrElement_Size(Element const *const me)
{
    assert(me);
    return REMOTE_STATION_ADDR_LEN + ELEMENT_IDENTIFER_LEN;
}

void RemoteStationAddrElement_ctor(RemoteStationAddrElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &RemoteStationAddrElement_Encode,
        &RemoteStationAddrElement_Decode,
        &RemoteStationAddrElement_Size};
    Element_ctor(&me->super, ADDRESS, ADDRESS);
    me->super.vptr = &vtbl;
}
// RemoteStationAddrElement END

// ObserveTimeElement
static bool ObserveTimeElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool ObserveTimeElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < DATETIME_LEN)
    {
        return false;
    }
    ObserveTimeElement *self = (ObserveTimeElement *)me;
    uint8_t usedLen = 0;
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->observeTime.year);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->observeTime.month);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->observeTime.day);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->observeTime.hour);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->observeTime.minute);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->observeTime.second);
    return usedLen == DATETIME_LEN;
}

static size_t ObserveTimeElement_Size(Element const *const me)
{
    assert(me);
    return DATETIME_LEN + ELEMENT_IDENTIFER_LEN;
}

void ObserveTimeElement_ctor(ObserveTimeElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &ObserveTimeElement_Encode,
        &ObserveTimeElement_Decode,
        &ObserveTimeElement_Size};
    Element_ctor(&me->super, DATETIME, DATETIME);
    me->super.vptr = &vtbl;
}
// ObserveTimeElement END

// PictureElement
static bool PictureElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool PictureElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < 0) // 截取所有
    {
        return false;
    }
    PictureElement *self = (PictureElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, ByteBuffer_Available(hexBuff));
    ByteBuffer_Flip(self->buff);
    return true;
}

static size_t PictureElement_Size(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN + ((PictureElement *)me)->buff->size;
}

void PictureElement_ctor(PictureElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &PictureElement_Encode,
        &PictureElement_Decode,
        &PictureElement_Size};
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

// ArtificialElement ARTIFICIAL_IL

static bool ArtificialElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool ArtificialElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < 0) // 截取所有
    {
        return false;
    }
    ArtificialElement *self = (ArtificialElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, ByteBuffer_Available(hexBuff));
    ByteBuffer_Flip(self->buff);
    // @Todo decode to ArtificialItem
    // @See SL 330 2011
    return true;
}

static size_t ArtificialElement_Size(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN + ((ArtificialElement *)me)->buff->size;
}

void ArtificialElement_ctor(ArtificialElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &ArtificialElement_Encode,
        &ArtificialElement_Decode,
        &ArtificialElement_Size};
    Element_ctor(&me->super, ARTIFICIAL_IL, ARTIFICIAL_IL);
    me->super.vptr = &vtbl;
}

void ArtificialElement_dtor(ArtificialElement *const me)
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
// ArtificialElement ARTIFICIAL_IL END

// DRP5MINElement

static bool DRP5MINElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool DRP5MINElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < DRP5MIN_LEN)
    {
        return false;
    }
    DRP5MINElement *self = (DRP5MINElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, DRP5MIN_LEN);
    ByteBuffer_Flip(self->buff);
    return true;
}

static size_t DRP5MINElement_Size(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN + DRP5MIN_LEN;
}

void DRP5MINElement_ctor(DRP5MINElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &DRP5MINElement_Encode,
        &DRP5MINElement_Decode,
        &DRP5MINElement_Size};
    Element_ctor(&me->super, DRP5MIN, DRP5MIN_DATADEF);
    me->super.vptr = &vtbl;
}

void DRP5MINElement_dtor(DRP5MINElement *const me)
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

uint8_t DRP5MINElement_ValueAt(DRP5MINElement *const me, uint8_t index, float *val)
{
    if (me == NULL || me->buff == NULL)
    {
        return 0;
    }
    uint8_t u8;
    uint8_t res = ByteBuffer_PeekUInt8At(me->buff, index, &u8);
    if (res == 1)
    {
        *val = u8 != 0xFF ? u8 / 10.0f : u8; // FF 为无效值
    }
    return res;
}
// DRP5MINElement END

// FlowRateDataElement
static bool FlowRateDataElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool FlowRateDataElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < 0) // 截取所有
    {
        return false;
    }
    FlowRateDataElement *self = (FlowRateDataElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, ByteBuffer_Available(hexBuff));
    ByteBuffer_Flip(self->buff);
    return true;
}

static size_t FlowRateDataElement_Size(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN + ((FlowRateDataElement *)me)->buff->size;
}

void FlowRateDataElement_ctor(FlowRateDataElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &FlowRateDataElement_Encode,
        &FlowRateDataElement_Decode,
        &FlowRateDataElement_Size};
    Element_ctor(&me->super, FLOW_RATE_DATA, FLOW_RATE_DATA_DATADEF);
    me->super.vptr = &vtbl;
}

void FlowRateDataElement_dtor(FlowRateDataElement *const me)
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
// FlowRateDataElement END

// RelativeWaterLevelElement
static bool RelativeWaterLevelElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool RelativeWaterLevelElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < RELATIVE_WATER_LEVEL_LEN)
    {
        return false;
    }
    RelativeWaterLevelElement *self = (RelativeWaterLevelElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, RELATIVE_WATER_LEVEL_LEN);
    ByteBuffer_Flip(self->buff);
    return true;
}

static size_t RelativeWaterLevelElement_Size(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN + RELATIVE_WATER_LEVEL_LEN;
}

void RelativeWaterLevelElement_ctor(RelativeWaterLevelElement *const me, uint8_t identifierLeader)
{
    // override
    static ElementVtbl const vtbl = {
        &RelativeWaterLevelElement_Encode,
        &RelativeWaterLevelElement_Decode,
        &RelativeWaterLevelElement_Size};
    Element_ctor(&me->super, identifierLeader, RELATIVE_WATER_LEVEL_5MIN1_DATADEF);
    me->super.vptr = &vtbl;
}

void RelativeWaterLevelElement_dtor(RelativeWaterLevelElement *const me)
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

uint8_t RelativeWaterLevelElement_ValueAt(RelativeWaterLevelElement *const me, uint8_t index, float *val)
{
    if (me == NULL || me->buff == NULL)
    {
        return 0;
    }
    uint16_t u16;
    uint8_t res = ByteBuffer_BE_PeekUInt16At(me->buff, index * 2, &u16);
    if (res == 2)
    {
        *val = u16 != 0xFFFF ? u16 / 100.0f : u16; // FF 为无效值
    }
    return res;
}
// RelativeWaterLevelElement END

// TimeStepCodeElement
static bool TimeStepCodeElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool TimeStepCodeElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < TIME_STEP_CODE_LEN)
    {
        return false;
    }
    TimeStepCodeElement *self = (TimeStepCodeElement *)me;
    uint8_t usedLen = 0;
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->timeStepCode.day);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->timeStepCode.hour);
    usedLen += ByteBuffer_BCDGetUInt8(hexBuff, &self->timeStepCode.minute);
    return usedLen == TIME_STEP_CODE_LEN;
}

static size_t TimeStepCodeElement_Size(Element const *const me)
{
    assert(me);
    return TIME_STEP_CODE_LEN + ELEMENT_IDENTIFER_LEN;
}

void TimeStepCodeElement_ctor(TimeStepCodeElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &TimeStepCodeElement_Encode,
        &TimeStepCodeElement_Decode,
        &TimeStepCodeElement_Size};
    Element_ctor(&me->super, TIME_STEP_CODE, TIME_STEP_CODE_DATADEF);
    me->super.vptr = &vtbl;
}
// TimeStepCodeElement END

// StationStatusElement
static bool StationStatusElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool StationStatusElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < STATION_STATUS_LEN)
    {
        return false;
    }
    StationStatusElement *self = (StationStatusElement *)me;
    uint8_t usedLen = ByteBuffer_BE_GetUInt32(hexBuff, &self->status);
    return usedLen == STATION_STATUS_LEN;
}

static size_t StationStatusElement_Size(Element const *const me)
{
    assert(me);
    return STATION_STATUS_LEN + ELEMENT_IDENTIFER_LEN;
}

void StationStatusElement_ctor(StationStatusElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &StationStatusElement_Encode,
        &StationStatusElement_Decode,
        &StationStatusElement_Size};
    Element_ctor(&me->super, STATION_STATUS, STATION_STATUS_DATADEF);
    me->super.vptr = &vtbl;
}

uint8_t StationStatusElement_StatusAt(StationStatusElement const *const me, uint8_t index)
{
    if (me == NULL || index < 0 || index > 31)
    {
        return 0;
    }
    return me->status >> index & 1;
}
// StationStatusElement END

// DurationElement
static bool DurationElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool DurationElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL || ByteBuffer_Available(hexBuff) < DURATION_OF_XX_LEN)
    {
        return false;
    }
    DurationElement *self = (DurationElement *)me;
    uint8_t byte = 0;
    // @Todo ASCII TO INT atoi with end.
    uint8_t usedLen = ByteBuffer_GetUInt8(hexBuff, &self->hour);
    usedLen += ByteBuffer_GetUInt8(hexBuff, &byte);
    self->hour = (self->hour - 0x30) * 10 + (byte - 0x30);
    usedLen += ByteBuffer_GetUInt8(hexBuff, &byte); // a dot in ascii.
    usedLen += ByteBuffer_GetUInt8(hexBuff, &self->minute);
    byte = 0;
    usedLen += ByteBuffer_GetUInt8(hexBuff, &byte);
    self->minute = (self->minute - 0x30) * 10 + (byte - 0x30);
    return usedLen == DURATION_OF_XX_LEN;
}

static size_t DurationElement_Size(Element const *const me)
{
    assert(me);
    return DURATION_OF_XX_LEN + ELEMENT_IDENTIFER_LEN;
}

void DurationElement_ctor(DurationElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &DurationElement_Encode,
        &DurationElement_Decode,
        &DurationElement_Size};
    Element_ctor(&me->super, DURATION_OF_XX, DURATION_OF_XX_DATADEF);
    me->super.vptr = &vtbl;
}
// DurationElement END

// NumberElement
static bool NumberElement_Encode(Element const *const me, ByteBuffer *const hexBuff)
{
    assert(me);
    return true;
}

static bool NumberElement_Decode(Element *const me, ByteBuffer *const hexBuff)
{
    if (me == NULL || hexBuff == NULL)
    {
        return false;
    }
    uint8_t size = me->dataDef >> 3;
    if (ByteBuffer_Available(hexBuff) < size)
    {
        return false;
    }
    NumberElement *self = (NumberElement *)me;
    self->buff = ByteBuffer_GetByteBuffer(hexBuff, size); //read all bcd
    if (self->buff == NULL)
    {
        return false;
    }
    ByteBuffer_Flip(self->buff); // Flip to read it.
    return true;
}

static size_t NumberElement_Size(Element const *const me)
{
    assert(me);
    return ELEMENT_IDENTIFER_LEN + (me->dataDef >> NUMBER_ELEMENT_LEN_OFFSET);
}

void NumberElement_ctor(NumberElement *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    static ElementVtbl const vtbl = {
        &NumberElement_Encode,
        &NumberElement_Decode,
        &NumberElement_Size};
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
    uint8_t bitLen = ByteBuffer_Available(me->buff);
    *val = 0; // 副作用
    uint8_t res = ByteBuffer_BCDGetUInt(me->buff, val, bitLen);
    if (signedFlag == 0xFF && res != 0)
    {
        *val *= -1;
    }
    return res;
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
Element *decodeElement(ByteBuffer *const hexBuff)
{
    if (hexBuff == NULL || ByteBuffer_Available(hexBuff) < ELEMENT_IDENTIFER_LEN)
    {
        return NULL;
    }                                                      // 至少包含标识符
    uint8_t identifierLeader = 0;                          //
    ByteBuffer_GetUInt8(hexBuff, &identifierLeader);       // 解析一个字节的 标识符引导符 ， 同时位移
    uint8_t dataDef = 0;                                   //
    ByteBuffer_GetUInt8(hexBuff, &dataDef);                // 解析一个字节的 数据定义符，同时位移
    Element *el = NULL;                                    // 根据标识符引导符，开始解析 Element
    bool decoded = false;                                  //
    void (*dtor)(void *) = NULL;                           // 统一的析构函数形式；但具体实现和构造函数无法统一
    switch (identifierLeader)                              //
    {                                                      //
    case CUSTOM_IDENTIFIER:                                // unsupport
        return NULL;                                       // 返回NULL
    case DATETIME:                                         // 观测时间 Element
        el = (Element *)(NewInstance(ObserveTimeElement)); // 创建指针，需要转为Element*
        ObserveTimeElement_ctor((ObserveTimeElement *)el); // 构造函数
        // dtor = &ObserveTimeElement_dtor;
        break;
    case ADDRESS:
        el = (Element *)(NewInstance(RemoteStationAddrElement));       // 创建指针，需要转为Element*
        RemoteStationAddrElement_ctor((RemoteStationAddrElement *)el); // 构造函数
        // dtor = &RemoteStationAddrElement_dtor;
        break;
    case ARTIFICIAL_IL:
        el = (Element *)(NewInstance(ArtificialElement)); // 创建指针，需要转为Element*
        ArtificialElement_ctor((ArtificialElement *)el);  // 构造函数
        dtor = (void (*)(void *))(&ArtificialElement_dtor);
        break;
    case PICTURE_IL:
        el = (Element *)(NewInstance(PictureElement)); // 创建指针，需要转为Element*
        PictureElement_ctor((PictureElement *)el);     // 构造函数
        dtor = (void (*)(void *))(&PictureElement_dtor);
        break;
    case DRP5MIN:
        if (dataDef != DRP5MIN_DATADEF) //固定为 0x60
        {
            return NULL;
        }
        el = (Element *)(NewInstance(DRP5MINElement)); // 创建指针，需要转为Element*
        DRP5MINElement_ctor((DRP5MINElement *)el);     // 构造函数
        dtor = (void (*)(void *))(&DRP5MINElement_dtor);
        break;
    case RELATIVE_WATER_LEVEL_5MIN1:
    case RELATIVE_WATER_LEVEL_5MIN2:
    case RELATIVE_WATER_LEVEL_5MIN3:
    case RELATIVE_WATER_LEVEL_5MIN4:
    case RELATIVE_WATER_LEVEL_5MIN5:
    case RELATIVE_WATER_LEVEL_5MIN6:
    case RELATIVE_WATER_LEVEL_5MIN7:
    case RELATIVE_WATER_LEVEL_5MIN8:
        if (dataDef != RELATIVE_WATER_LEVEL_5MIN1_DATADEF) //固定为 0x60
        {
            return NULL;
        }
        el = (Element *)(NewInstance(RelativeWaterLevelElement));                          // 创建指针，需要转为Element*
        RelativeWaterLevelElement_ctor((RelativeWaterLevelElement *)el, identifierLeader); // 构造函数
        dtor = (void (*)(void *))(&RelativeWaterLevelElement_dtor);
        break;
    case FLOW_RATE_DATA:
        if (dataDef != FLOW_RATE_DATA_DATADEF) //固定为 0xF6
        {
            return NULL;
        }
        el = (Element *)(NewInstance(FlowRateDataElement));  // 创建指针，需要转为Element*
        FlowRateDataElement_ctor((FlowRateDataElement *)el); // 构造函数
        dtor = (void (*)(void *))(&FlowRateDataElement_dtor);
        break;
    case TIME_STEP_CODE:
        el = (Element *)(NewInstance(TimeStepCodeElement));  // 创建指针，需要转为Element*
        TimeStepCodeElement_ctor((TimeStepCodeElement *)el); // 构造函数
        // dtor = &TimeStepCodeElement_dtor;
        break;
    case STATION_STATUS:
        el = (Element *)(NewInstance(StationStatusElement));   // 创建指针，需要转为Element*
        StationStatusElement_ctor((StationStatusElement *)el); // 构造函数
        // dtor = &TimeStepCodeElement_dtor;
        break;
    case DURATION_OF_XX:
        el = (Element *)(NewInstance(DurationElement)); // 创建指针，需要转为Element*
        DurationElement_ctor((DurationElement *)el);    // 构造函数
        // dtor = &TimeStepCodeElement_dtor;
        break;
    default:
        // 按照数据类型解析
        if (isNumberElement(identifierLeader))
        {
            el = (Element *)NewInstance(NumberElement);
            NumberElement_ctor((NumberElement *)el, identifierLeader, dataDef);
            dtor = (void (*)(void *))(&NumberElement_dtor);
        }
        else
        {
            return NULL;
        }
    }
    if (el != NULL)
    {
        decoded = el->vptr->decode(el, hexBuff); // 解析
        if (!decoded)                            // 解析失败，需要手动删除指针
        {                                        //
            if (dtor != NULL)                    // 实现了析构函数
            {                                    //
                dtor((ArtificialElement *)el);   // 调用析构，规范步骤
            }                                    //
            DelInstance(el);                     // 删除指针
            return NULL;                         //
        }
    }
    return el;
}

// Elements END
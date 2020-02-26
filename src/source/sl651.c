#include <assert.h>
#include <math.h>

#include "sl651.h"

// fowarding define
static bool RemoteStationAddrElement_Encode(Element const *const me, ByteBuffer *const byteBuff);
static bool RemoteStationAddrElement_Decode(Element *const me, ByteBuffer *const byteBuff);
static bool ObserveTimeElement_Encode(Element const *const me, ByteBuffer *const byteBuff);
static bool ObserveTimeElement_Decode(Element *const me, ByteBuffer *const byteBuff);

static bool RemoteStationAddr_Decode(RemoteStationAddr *me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t usedLen = 0;
    usedLen += BB_BCDGetUInt8(byteBuff, &me->A5);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->A4);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->A3);
    if (me->A5 == A5_HYDROLOGICAL_TELEMETRY_STATION)
    {
        usedLen += BB_BCDGetUInt8(byteBuff, &me->A2);
        usedLen += BB_BCDGetUInt8(byteBuff, &me->A1);
    }
    else
    {
        uint16_t u16A2A1 = 0;
        uint8_t len = BB_BE_GetUInt16(byteBuff, &u16A2A1);
        if (len == 2) // 2 byte
        {
            me->A2 = u16A2A1 / 10000;
            me->A1 = u16A2A1 / 100 - me->A2 * 10000;
            me->A0 = u16A2A1 - me->A2 * 10000 - me->A1 * 100;
            usedLen += len;
        }
        else
        {
            return false;
        }
    }
    return usedLen == REMOTE_STATION_ADDR_LEN;
}

static DateTime_Decode(DateTime *me, ByteBuffer *byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t usedLen = 0;
    usedLen += BB_BCDGetUInt8(byteBuff, &me->year);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->month);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->day);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->hour);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->minute);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->second);
    return usedLen == DATETIME_LEN;
}

// "AbstractClass" Package
/* purely-virtual */
static bool Package_Virtual_Encode(Package const *const me, ByteBuffer *const byteBuff)
{
    assert(0);
    return true;
}
static bool Package_Virtual_Decode(Package *const me, ByteBuffer *const byteBuff)
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
void Package_ctor(Package *me)
{
    assert(me);
    static PackageVtbl const vtbl = {
        &Package_Virtual_Encode,
        &Package_Virtual_Decode,
        &Package_Virtual_Size};
    me->vptr = &vtbl;
}

/* Methods  & Destrucor */
bool Package_EncodeHead(Package const *const me, ByteBuffer *const byteBuff)
{
    return true;
}

bool Package_EncodeTail(Package const *const me, ByteBuffer *const byteBuff)
{
    return true;
}

bool Package_DecodeHead(Package *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint32_t buffSize = BB_Available(byteBuff);
    if (buffSize < PACKAGE_HEAD_STX_LEN + PACKAGE_TAIL_LEN) // 非ASCII的最小长度
    {
        return false;
    }
    // 基本的数据判断
    // 消息头
    uint8_t usedLen = BB_BE_GetUInt16(byteBuff, &me->head.soh);
    if (me->head.soh != SOH_BINARY)
    {
        // @Todo ASCII 模式
        return false;
    }
    // Direction  do it again :(.
    bool decoded = false;
    BB_PeekUInt8At(byteBuff, PACKAGE_HEAD_STX_DIRECTION_INDEX, &me->head.direction);
    me->head.direction >>= PACKAGE_HEAD_STX_DIRECTION_INDEX_MASK_BIT;
    switch (me->head.direction)
    {
    case Up:
        usedLen += BB_GetUInt8(byteBuff, &me->head.centerAddr);
        decoded = RemoteStationAddr_Decode(&me->head.stationAddr, byteBuff);
        break;
    case Down:
        decoded = RemoteStationAddr_Decode(&me->head.stationAddr, byteBuff);
        usedLen += BB_GetUInt8(byteBuff, &me->head.centerAddr);
        break;
    default:
        break;
    }
    if (!decoded) // invalid address
    {
        return false;
    }
    usedLen += REMOTE_STATION_ADDR_LEN;
    usedLen += BB_BE_GetUInt16(byteBuff, &me->head.password);
    usedLen += BB_GetUInt8(byteBuff, &me->head.funcCode);
    usedLen += BB_BE_GetUInt16(byteBuff, &me->head.len);
    me->head.len &= PACKAGE_HEAD_STX_BODY_LEN_MASK;
    usedLen += BB_GetUInt8(byteBuff, &me->head.stxFlag);
    if (me->head.stxFlag == SYN)
    {
        uint32_t u32 = 0;
        usedLen += BB_BE_GetUInt(byteBuff, &u32, 3);
        me->head.sequence.seq = u32 & PACKAGE_HEAD_SEQUENCE_SEQ_MASK;
        me->head.sequence.count = u32 >> PACKAGE_HEAD_SEQUENCE_COUNT_BIT_MASK_LEN &
                                  PACKAGE_HEAD_SEQUENCE_COUNT_MASK;
    }
    return me->head.stxFlag == SYN ? usedLen == PACKAGE_HEAD_SNY_LEN : usedLen == PACKAGE_HEAD_STX_LEN;
}

bool Package_DecodeTail(Package *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    // assert(BB_Available(byteBuff) == PACKAGE_TAIL_LEN); // very strict
    uint8_t usedLen = BB_GetUInt8(byteBuff, &me->tail.etxFlag);
    usedLen += BB_BE_GetUInt16(byteBuff, &me->tail.crc);
    return usedLen == PACKAGE_TAIL_LEN;
}

// "AbstractClass" Package END

/* LinkMessage Construtor & Destrucor */
void LinkMessage_ctor(LinkMessage *const me, uint16_t initElementCount)
{
    assert(me);
    Package_ctor(&me->super);
    // keep virtual function inherit from Package.
    if (initElementCount < 0 || initElementCount > MAX_ELEMENT_NUMBER)
    {
        initElementCount = DEFAULT_ELEMENT_NUMBER;
    }
    // init Elements array(Vector) and reserve initElementCount or DEFAULT_ELEMENT_NUMBER
    vec_init(&me->elements);
    vec_reserve(&me->elements, initElementCount);
}

void LinkMessage_dtor(LinkMessage *const me)
{
    assert(me);
    Package_dtor(&me->super)
}

/* Public methods */
bool LinkMessage_EncodeElements(LinkMessage const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return true;
}

bool LinkMessage_DecodeElements(LinkMessage *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return true;
}

bool LinkMessage_PutElement(LinkMessage *const me, uint16_t index, Element *element)
{
    assert(me);
    return true;
}

bool LinkMessage_SetElement(LinkMessage *const me, uint16_t index, Element *element)
{
    assert(me);
    return true;
}
Element *LinkMessage_GetElement(LinkMessage const *const me, uint16_t index)
{
    assert(me);
    return NULL;
}
// "Basic" LinkMessage END

// "AbstractUpClass" UplinkMessage
/* UplinkMessage Construtor & Destrucor */
static bool UplinkMessage_Encode(Package const *const me, ByteBuffer *const byteBuff)
{
    assert(0);
    return true;
}
static bool UplinkMessage_Decode(Package *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    bool res = Package_DecodeHead(me, byteBuff) && UplinkMessage_DecodeHead((UplinkMessage *)me, byteBuff);
    if (isMessageCombinedByElements(me->head.funcCode)) // 按照要素节码
    {
    }
    else // 否则交给具体功能码去处理
    {
    }
    // decode tail
    res = Package_DecodeTail(me, byteBuff);
    return res;
}
static size_t UplinkMessage_Size()
{
    assert(0);
    return 0;
}

void UplinkMessage_ctor(UplinkMessage *const me, uint16_t initElementCount)
{
    assert(me);
    static PackageVtbl const vtbl = {
        &UplinkMessage_Encode,
        &UplinkMessage_Decode,
        &UplinkMessage_Size};
    LinkMessage_ctor(&me->super, initElementCount);
    // me->super.super.vptr = &vtbl;  // or -->
    ((Package *)me)->vptr = &vtbl; // single inheritance
}

void UplinkMessage_dtor(UplinkMessage *const me)
{
    assert(me);
    LinkMessage_dtor(&me->super);
}

/* Public methods */
bool UplinkMessage_EncodeHead(UplinkMessage const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return true;
}
bool UplinkMessage_DecodeHead(UplinkMessage *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t usedLen = BB_BE_GetUInt16(byteBuff, &me->messageHead.seq);
    if (!DateTime_Decode(&me->messageHead.sendTime, byteBuff))
    {
        return false;
    }
    if (isContainRemoteStationAddrElement(me->super.super.head.funcCode) &&
        !RemoteStationAddrElement_Decode((Element *)&me->messageHead.stationAddrElement, byteBuff))
    {
        return false;
    }
    bool containCategroyField = isContainStationCategoryField(me->super.super.head.funcCode);
    if (containCategroyField)
    {
        usedLen += BB_GetUInt8(byteBuff, &me->messageHead.stationCategory);
    }
    if (isContainObserveTimeElement(me->super.super.head.funcCode) &&
        !ObserveTimeElement_Decode((Element *)&me->messageHead.ObserveTimeElement, byteBuff))
    {
        return false;
    }
    return usedLen == containCategroyField ? 3 : 2;
}
// "AbstractUpClass" UplinkMessage END

// "AbstractUpClass" DownlinkMessage

/* DownlinkMessage Construtor  & Destrucor */
void DownlinkMessage_ctor(DownlinkMessage *const me, uint16_t initElementCount)
{
    assert(me);
    LinkMessage_ctor(&me->super, initElementCount);
}

void DownlinkMessage_dtor(DownlinkMessage *const me)
{
    assert(me);
}
/* Public methods */
bool DownlinkMessage_EncodeHead(DownlinkMessage const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return true;
}
bool DownlinkMessage_DecodeHead(DownlinkMessage *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return true;
}
// "AbstractUpClass" DownlinkMessage END

// Elements
// "AbstractorClass" Element
static bool Element_Virtual_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(0);
    return true;
}

static bool Element_Virtual_Decode(Element *const me, ByteBuffer *const byteBuff)
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
static bool RemoteStationAddrElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool RemoteStationAddrElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < REMOTE_STATION_ADDR_LEN)
    {
        return false;
    }
    RemoteStationAddrElement *self = (RemoteStationAddrElement *)me;
    return RemoteStationAddr_Decode(&self->stationAddr, byteBuff);
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
static bool ObserveTimeElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool ObserveTimeElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < DATETIME_LEN)
    {
        return false;
    }
    ObserveTimeElement *self = (ObserveTimeElement *)me;
    return DateTime_Decode(&self->observeTime, byteBuff);
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
static bool PictureElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool PictureElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < 0) // 截取所有
    {
        return false;
    }
    PictureElement *self = (PictureElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, BB_Available(byteBuff));
    BB_Flip(self->buff);
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
    assert(me);
    if (me->buff)
    {
        BB_dtor(me->buff);
        DelInstance(me->buff);
    }
}
// PictureElement END

// ArtificialElement ARTIFICIAL_IL

static bool ArtificialElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool ArtificialElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < 0) // 截取所有
    {
        return false;
    }
    ArtificialElement *self = (ArtificialElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, BB_Available(byteBuff));
    BB_Flip(self->buff);
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
    assert(me);
    if (me->buff)
    {
        BB_dtor(me->buff);
        DelInstance(me->buff);
    }
}
// ArtificialElement ARTIFICIAL_IL END

// DRP5MINElement

static bool DRP5MINElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool DRP5MINElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < DRP5MIN_LEN)
    {
        return false;
    }
    DRP5MINElement *self = (DRP5MINElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, DRP5MIN_LEN);
    BB_Flip(self->buff);
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
    assert(me);
    if (me->buff)
    {
        BB_dtor(me->buff);
        DelInstance(me->buff);
    }
}

uint8_t DRP5MINElement_ValueAt(DRP5MINElement *const me, uint8_t index, float *val)
{
    assert(me);
    if (me->buff == NULL)
    {
        return 0;
    }
    uint8_t u8;
    uint8_t res = BB_PeekUInt8At(me->buff, index, &u8);
    if (res == 1)
    {
        *val = u8 != 0xFF ? u8 / 10.0f : u8; // FF 为无效值
    }
    return res;
}
// DRP5MINElement END

// FlowRateDataElement
static bool FlowRateDataElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool FlowRateDataElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < 0) // 截取所有
    {
        return false;
    }
    FlowRateDataElement *self = (FlowRateDataElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, BB_Available(byteBuff));
    BB_Flip(self->buff);
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
    assert(me);
    if (me->buff)
    {
        BB_dtor(me->buff);
        DelInstance(me->buff);
    }
}
// FlowRateDataElement END

// RelativeWaterLevelElement
static bool RelativeWaterLevelElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool RelativeWaterLevelElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < RELATIVE_WATER_LEVEL_LEN)
    {
        return false;
    }
    RelativeWaterLevelElement *self = (RelativeWaterLevelElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, RELATIVE_WATER_LEVEL_LEN);
    BB_Flip(self->buff);
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
    assert(me);
    if (me->buff)
    {
        BB_dtor(me->buff);
        DelInstance(me->buff);
    }
}

uint8_t RelativeWaterLevelElement_ValueAt(RelativeWaterLevelElement *const me, uint8_t index, float *val)
{
    assert(me);
    if (me->buff == NULL)
    {
        return 0;
    }
    uint16_t u16;
    uint8_t res = BB_BE_PeekUInt16At(me->buff, index * 2, &u16);
    if (res == 2)
    {
        *val = u16 != 0xFFFF ? u16 / 100.0f : u16; // FF 为无效值
    }
    return res;
}
// RelativeWaterLevelElement END

// TimeStepCodeElement
static bool TimeStepCodeElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool TimeStepCodeElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < TIME_STEP_CODE_LEN)
    {
        return false;
    }
    TimeStepCodeElement *self = (TimeStepCodeElement *)me;
    uint8_t usedLen = 0;
    usedLen += BB_BCDGetUInt8(byteBuff, &self->timeStepCode.day);
    usedLen += BB_BCDGetUInt8(byteBuff, &self->timeStepCode.hour);
    usedLen += BB_BCDGetUInt8(byteBuff, &self->timeStepCode.minute);
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
static bool StationStatusElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool StationStatusElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < STATION_STATUS_LEN)
    {
        return false;
    }
    StationStatusElement *self = (StationStatusElement *)me;
    uint8_t usedLen = BB_BE_GetUInt32(byteBuff, &self->status);
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
    assert(me);
    if (index < 0 || index > 31)
    {
        return 0;
    }
    return me->status >> index & 1;
}
// StationStatusElement END

// DurationElement
static bool DurationElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool DurationElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < DURATION_OF_XX_LEN)
    {
        return false;
    }
    DurationElement *self = (DurationElement *)me;
    uint8_t byte = 0;
    // @Todo ASCII TO INT atoi with end.
    uint8_t usedLen = BB_GetUInt8(byteBuff, &self->hour);
    usedLen += BB_GetUInt8(byteBuff, &byte);
    self->hour = (self->hour - 0x30) * 10 + (byte - 0x30);
    usedLen += BB_GetUInt8(byteBuff, &byte); // a dot in ascii.
    usedLen += BB_GetUInt8(byteBuff, &self->minute);
    byte = 0;
    usedLen += BB_GetUInt8(byteBuff, &byte);
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
static bool NumberElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    return true;
}

static bool NumberElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    if (byteBuff == NULL)
    {
        return false;
    }
    uint8_t size = me->dataDef >> 3;
    if (BB_Available(byteBuff) < size)
    {
        return false;
    }
    NumberElement *self = (NumberElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, size); //read all bcd
    if (self->buff == NULL)
    {
        return false;
    }
    BB_Flip(self->buff); // Flip to read it.
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
    assert(me);
    if (me->buff == NULL)
    {
        return 0;
    }
    uint8_t signedFlag = 0;
    BB_GetUInt8(me->buff, &signedFlag);
    if (signedFlag != 0xFF) // 如果不是负值
    {
        BB_Rewind(me->buff);
    }
    uint8_t bitLen = BB_Available(me->buff);
    *val = 0; // 副作用
    uint8_t res = BB_BCDGetUInt(me->buff, val, bitLen);
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
// Elements END

// Decode & Encode
// Util Functions
// ByteBuffer should be in read mode
Element *decodeElement(ByteBuffer *const byteBuff)
{
    assert(byteBuff);
    if (BB_Available(byteBuff) < ELEMENT_IDENTIFER_LEN)
    {
        return NULL;
    }                                                      // 至少包含标识符
    uint8_t identifierLeader = 0;                          //
    BB_GetUInt8(byteBuff, &identifierLeader);              // 解析一个字节的 标识符引导符 ， 同时位移
    uint8_t dataDef = 0;                                   //
    BB_GetUInt8(byteBuff, &dataDef);                       // 解析一个字节的 数据定义符，同时位移
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
        decoded = el->vptr->decode(el, byteBuff); // 解析
        if (!decoded)                             // 解析失败，需要手动删除指针
        {                                         //
            if (dtor != NULL)                     // 实现了析构函数
            {                                     //
                dtor(el);                         // 调用析构，规范步骤
            }                                     //
            DelInstance(el);                      // 删除指针
            return NULL;                          //
        }
    }
    return el;
}

// ByteBuffer should be in read mode
Package *decodePackage(ByteBuffer *const byteBuff)
{
    assert(byteBuff);
    uint32_t buffSize = BB_Available(byteBuff);
    if (buffSize < PACKAGE_HEAD_STX_LEN + PACKAGE_TAIL_LEN) // 非ASCII的最小长度
    {
        return NULL;
    }
    // crc
    uint16_t crcCalc = 0;
    uint16_t crcInBuf = 0;
    uint8_t dataEnd = buffSize - (PACKAGE_TAIL_LEN - 1);
    BB_BE_PeekUInt16At(byteBuff, dataEnd, &crcInBuf);
    BB_CRC16(byteBuff, &crcCalc, 0, dataEnd);
    if (crcCalc != crcInBuf)
    {
        return NULL;
    }
    // Direction
    uint8_t direction = Down;
    BB_PeekUInt8At(byteBuff, PACKAGE_HEAD_STX_DIRECTION_INDEX, &direction);
    direction >>= PACKAGE_HEAD_STX_DIRECTION_INDEX_MASK_BIT;
    // @Todo ASCII 模式
    // DataTransMode transMode = TRANS_IN_BINARY;
    Package *pkg = NULL;
    bool decoded = false;        //
    void (*dtor)(void *) = NULL; // 统一的析构函数形式；但具体实现和构造函数无法统一
    switch (direction)
    {
    case Up:
        pkg = (Package *)NewInstance(UplinkMessage);
        UplinkMessage_ctor((UplinkMessage *)pkg, DEFAULT_ELEMENT_NUMBER);
        dtor = (void (*)(void *))(&UplinkMessage_dtor);
        break;
    case Down:
        pkg = (Package *)NewInstance(DownlinkMessage);
        DownlinkMessage_ctor((DownlinkMessage *)pkg, DEFAULT_ELEMENT_NUMBER);
        dtor = (void (*)(void *))(&DownlinkMessage_dtor);
        break;
    default:
        return NULL;
    }
    if (pkg != NULL)
    {
        decoded = pkg->vptr->decode(pkg, byteBuff); // 解析
        if (!decoded)                               // 解析失败，需要手动删除指针
        {                                           //
            if (dtor != NULL)                       // 实现了析构函数
            {                                       //
                dtor(pkg);                          // 调用析构，规范步骤
            }                                       //
            DelInstance(pkg);                       // 删除指针
            return NULL;                            //
        }
    }
    return pkg;
}
// Decode & Encode END
// #pragma pack()
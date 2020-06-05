#include <assert.h>
#include <memory.h>
#include <math.h>
#include <time.h>
#include <stdio.h>

#include "common/class.h"
#include "sl651/sl651.h"

/* init and register error info before main. */
#define SL651_DEFINE_ERROR_INFO_COMMON(C, ES) [(C)-0x0000] = DEFINE_ERROR_INFO(C, ES, "sl651")

static struct error_info errors[] = {
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_SUCCESS,
        "Success."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_SOH,
        "Invalid SOH."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_DIRECTION,
        "Invalid direction."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INSUFFICIENT_PACKAGE_LEN,
        "Insufficient package length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_STATION_ADDR,
        "Invalid station address(eg: invalid BCD)."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_STATION_ELEMENT,
        "Invalid station element(invalid dentifier(0xF1F1) or station address(eg: invalid BCD))."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_DATATIME,
        "Invalid report time(eg: invalid BCD)."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_OBSERVETIME,
        "Invalid observe time(eg: invalid BCD)."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_OBSERVETIME_ELEMENT,
        "Invalid observe time element(invalid dentifier(0xF1F1) or observe time(eg: invalid BCD))."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INVALID_TIMERANGE,
        "Invalid time range(eg: invalid BCD)."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_INSUFFICIENT_TIMERANGE_LEN,
        "Insufficient time range length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_INSUFFICIENT_HEAD_LEN,
        "Insufficient head length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_INVALID_HEAD,
        "Invalid head data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_INVALID_UPLINKMESSAGE_HEAD,
        "Invalid uplink message head data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_INVALID_DOWNLINKMESSAGE_HEAD,
        "Invalid downlink message head data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_INVALID_CRC,
        "CRC not match."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_INSUFFICIENT_LEN,
        "Insufficient element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_UNKOWN_INDENTIFIERLEADER,
        "Unkown element indentifier leader."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_UNSUPPORTCUSTOM,
        "Does not support custom element."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_OBSERVETIME_INSUFFICIENT_LEN,
        "Insufficient observe time element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_REMOTEADDR_INSUFFICIENT_LEN,
        "Insufficient remote address element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY,
        "Empty artificial element."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_PICTURE_EMPTY,
        "Empty picture element."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_DRP5MIN_DATADEF_ERROR,
        "DRP5MIN element's data definition field error."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_DRP5MIN_INSUFFICIENT_LEN,
        "Insufficient DRP5MIN element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_RELATIVEWATERLEVEL_DATADEF_ERROR,
        "Relative water level element's data definition field error."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_RELATIVEWATERLEVEL_INSUFFICIENT_LEN,
        "Insufficient relative water level element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_FLOWRATE_DATADEF_ERROR,
        "Flow rate element's data definition field error."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_FLOWRATE_INSUFFICIENT_LEN,
        "Insufficient flow rate element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_TIMESTEPCODE_DATADEF_ERROR,
        "Timestep code element's data definition field error."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_TIMESTEPCODE_INSUFFICIENT_LEN,
        "Insufficient timestep code element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_STATIONSTATUS_DATADEF_ERROR,
        "Station status element's data definition field error."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_STATIONSTATUS_INSUFFICIENT_LEN,
        "Insufficient station status element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_DURATION_DATADEF_ERROR,
        "Duration element's data definition field error."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_DURATION_INSUFFICIENT_LEN,
        "Insufficient duration element length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_NUMBERLIST_ODD_SIZE,
        "NumberList element should not be an odd length."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_DECODE_ELEMENT_NUMBER_SIZE_NOT_MATCH_DATADEF,
        "Number element data length does not match the data definition field."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_INVALID_HEAD,
        "Invalid head data to encode."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_INVALID_UPLINKMESSAGE_HEAD,
        "Invalid uplink message head data to encode."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_INVALID_DOWNLINKMESSAGE_HEAD,
        "Invalid downlink message head data to encode."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_RAWBUFF,
        "Cannot encode raw buffer of link message."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_ELEMENTS,
        "Cannot encode elements of link message."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_ELEMENT_INDENTIFIER,
        "Cannot encode element's indentifier."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_PICTURE_DATA,
        "Cannot encode picture element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_ARTIFICIAL_DATA,
        "Cannot encode artificial element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_DRP5MIN_DATA,
        "Cannot encode drp5min element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_FLOWRATE_DATA,
        "Cannot encode flow rate element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_RELATIVEWATER_DATA,
        "Cannot encode relative water level element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_STATIONSTATUS_DATA,
        "Cannot encode station status element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_DURATION_DATA,
        "Cannot encode duration element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_NUMBER_DATA,
        "Cannot encode number element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_CANNOT_PROCESS_NUMBERLIST_DATA,
        "Cannot encode number list element's data."),
    SL651_DEFINE_ERROR_INFO_COMMON(
        SL651_ERROR_ENCODE_FAIL_CALC_CRC,
        "Can not calc crc of message."),
};

static struct error_info_list sl651_error_list = {
    .error_list = errors,
    .count = sizeof(errors) / sizeof(errors[0]),
};

__attribute__((constructor)) void register_error()
{
    register_error_info(&sl651_error_list);
}
/* register error end*/

bool RemoteStationAddr_Encode(RemoteStationAddr const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t writeLen = 0;
    writeLen += BB_BCDPutUInt8(byteBuff, me->A5);
    writeLen += BB_BCDPutUInt8(byteBuff, me->A4);
    writeLen += BB_BCDPutUInt8(byteBuff, me->A3);
    if (me->A5 == A5_HYDROLOGICAL_TELEMETRY_STATION)
    {
        writeLen += BB_BCDPutUInt8(byteBuff, me->A2);
        writeLen += BB_BCDPutUInt8(byteBuff, me->A1);
    }
    else
    {
        uint16_t u16A2A1 = me->A2 * 10000 + me->A1 * 100 + me->A0;
        writeLen += BB_BE_PutUInt16(byteBuff, u16A2A1);
    }
    return writeLen == REMOTE_STATION_ADDR_LEN || set_error_indicate(SL651_ERROR_INVALID_STATION_ADDR);
}

bool RemoteStationAddr_Decode(RemoteStationAddr *const me, ByteBuffer *const byteBuff)
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
    return usedLen == REMOTE_STATION_ADDR_LEN || set_error_indicate(SL651_ERROR_INVALID_STATION_ADDR);
}

void DateTime_now(DateTime *const me)
{
    time_t nSeconds;
    struct tm *pTM;
    time(&nSeconds);
    pTM = localtime(&nSeconds);
    me->year = pTM->tm_year % 100;
    me->month = pTM->tm_mon + 1;
    me->day = pTM->tm_mday;
    me->hour = pTM->tm_hour;
    me->minute = pTM->tm_min;
    me->second = pTM->tm_sec;
}

static bool DateTime_Encode(DateTime const *const me, ByteBuffer *byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t writeLen = 0;
    writeLen += BB_BCDPutUInt8(byteBuff, me->year);
    writeLen += BB_BCDPutUInt8(byteBuff, me->month);
    writeLen += BB_BCDPutUInt8(byteBuff, me->day);
    writeLen += BB_BCDPutUInt8(byteBuff, me->hour);
    writeLen += BB_BCDPutUInt8(byteBuff, me->minute);
    writeLen += BB_BCDPutUInt8(byteBuff, me->second);
    return writeLen == DATETIME_LEN || set_error_indicate(SL651_ERROR_INVALID_DATATIME);
}

static bool DateTime_Decode(DateTime *const me, ByteBuffer *byteBuff)
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
    return usedLen == DATETIME_LEN || set_error_indicate(SL651_ERROR_INVALID_DATATIME);
}

void ObserveTime_now(ObserveTime *const me)
{
    time_t nSeconds;
    struct tm *pTM;
    time(&nSeconds);
    pTM = localtime(&nSeconds);
    me->year = pTM->tm_year % 100;
    me->month = pTM->tm_mon + 1;
    me->day = pTM->tm_mday;
    me->hour = pTM->tm_hour;
    me->minute = pTM->tm_min;
}

static bool ObserveTime_Encode(ObserveTime const *const me, ByteBuffer *byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t writeLen = 0;
    writeLen += BB_BCDPutUInt8(byteBuff, me->year);
    writeLen += BB_BCDPutUInt8(byteBuff, me->month);
    writeLen += BB_BCDPutUInt8(byteBuff, me->day);
    writeLen += BB_BCDPutUInt8(byteBuff, me->hour);
    writeLen += BB_BCDPutUInt8(byteBuff, me->minute);
    return writeLen == OBSERVETIME_LEN || set_error_indicate(SL651_ERROR_INVALID_OBSERVETIME);
}

static bool ObserveTime_Decode(ObserveTime *const me, ByteBuffer *byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t usedLen = 0;
    usedLen += BB_BCDGetUInt8(byteBuff, &me->year);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->month);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->day);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->hour);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->minute);
    return usedLen == OBSERVETIME_LEN || set_error_indicate(SL651_ERROR_INVALID_OBSERVETIME);
}

// "AbstractClass" Package
/* purely-virtual */
static ByteBuffer *Package_Virtual_Encode(Package *const me)
{
    assert(0);
    return NULL;
}

static bool Package_Virtual_Decode(Package *const me, ByteBuffer *const byteBuff)
{
    assert(0);
    return true;
}

static size_t Package_Virtual_Size(Package const *const me)
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
        &Package_Virtual_Size,
        &Package_dtor};
    me->vptr = &vtbl;
}

/* Methods & Destrucor */
bool Package_EncodeHead(Package const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t writeLen = BB_BE_PutUInt16(byteBuff, SOH_BINARY);
    if (me->head.direction == Up)
    {
        writeLen += BB_PutUInt8(byteBuff, me->head.centerAddr);
        writeLen += RemoteStationAddr_Encode(&me->head.stationAddr, byteBuff) ? REMOTE_STATION_ADDR_LEN : 0;
    }
    else if (me->head.direction == Down)
    {
        writeLen += RemoteStationAddr_Encode(&me->head.stationAddr, byteBuff) ? REMOTE_STATION_ADDR_LEN : 0;
        writeLen += BB_PutUInt8(byteBuff, me->head.centerAddr);
    }
    else
    {
        return set_error_indicate(SL651_ERROR_INVALID_DIRECTION);
    }
    writeLen += BB_BE_PutUInt16(byteBuff, me->head.password);
    writeLen += BB_PutUInt8(byteBuff, me->head.funcCode);
    writeLen += BB_BE_PutUInt16(byteBuff, (me->head.len & 0xFFF) |
                                              (((uint16_t)me->head.direction)
                                               << (PACKAGE_HEAD_STX_DIRECTION_INDEX_MASK_BIT + 8)));
    writeLen += BB_PutUInt8(byteBuff, me->head.stxFlag);
    if (me->head.stxFlag == SYN)
    {
        uint32_t u32 = 0;
        u32 = (((uint32_t)me->head.sequence.count) << PACKAGE_HEAD_SEQUENCE_COUNT_BIT_MASK_LEN) + me->head.sequence.seq;
        writeLen += BB_PutUInt8(byteBuff, u32 >> 16);
        writeLen += BB_BE_PutUInt16(byteBuff, u32 & 0xFFFF);
    }
    return (me->head.stxFlag == SYN
                ? writeLen == PACKAGE_HEAD_SYN_LEN
                : writeLen == PACKAGE_HEAD_STX_LEN) ||
           set_error_indicate(SL651_ERROR_ENCODE_INVALID_HEAD);
}

bool Package_EncodeTail(Package const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint8_t writeLen = BB_PutUInt8(byteBuff, me->tail.etxFlag);
    uint16_t crc16 = 0;
    if (BB_CRC16(byteBuff, &crc16, 0, BB_Position(byteBuff)))
    {
        writeLen += BB_BE_PutUInt16(byteBuff, crc16);
        return writeLen == PACKAGE_TAIL_LEN || set_error_indicate(SL651_ERROR_ENCODE_FAIL_CALC_CRC);
    }
    else
    {
        return set_error_indicate(SL651_ERROR_ENCODE_FAIL_CALC_CRC);
    }
}

bool Package_DecodeHead(Package *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    uint32_t buffSize = BB_Available(byteBuff);
    if (buffSize < PACKAGE_HEAD_STX_LEN + PACKAGE_TAIL_LEN) // 非ASCII的最小长度
    {
        return set_error_indicate(SL651_ERROR_DECODE_INSUFFICIENT_HEAD_LEN);
    }
    // 基本的数据判断
    // 消息头
    uint8_t usedLen = BB_BE_GetUInt16(byteBuff, &me->head.soh);
    if (me->head.soh != SOH_BINARY)
    {
        // @Todo ASCII 模式
        return set_error_indicate(SL651_ERROR_INVALID_SOH);
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
        return set_error_indicate(SL651_ERROR_INVALID_DIRECTION);
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
    if (me->head.len > (BB_Available(byteBuff) - PACKAGE_TAIL_LEN))
    {
        return set_error_indicate(SL651_ERROR_INSUFFICIENT_PACKAGE_LEN);
    }
    uint16_t crcCalc = 0;
    uint16_t crcInBuf = 0;
    uint32_t dataStart = BB_Position(byteBuff) - usedLen;
    uint32_t dataEnd = BB_Position(byteBuff) + me->head.len + 1;
    BB_BE_PeekUInt16At(byteBuff, dataEnd, &crcInBuf);
    BB_CRC16(byteBuff, &crcCalc, dataStart, dataEnd - dataStart);
    if (crcCalc != crcInBuf)
    {
        printf("sl651 invalid CRC, E[%4x] A[%4X]\r\n", crcCalc, crcInBuf);
        return set_error_indicate(SL651_ERROR_DECODE_INVALID_CRC);
    }
    if (me->head.stxFlag == SYN)
    {
        uint32_t u32 = 0;
        usedLen += BB_BE_GetUInt(byteBuff, &u32, 3);
        me->head.sequence.seq = u32 & PACKAGE_HEAD_SEQUENCE_SEQ_MASK;
        me->head.sequence.count = u32 >> PACKAGE_HEAD_SEQUENCE_COUNT_BIT_MASK_LEN &
                                  PACKAGE_HEAD_SEQUENCE_COUNT_MASK;
    }
    return (me->head.stxFlag == SYN ? usedLen == PACKAGE_HEAD_SYN_LEN : usedLen == PACKAGE_HEAD_STX_LEN) ||
           set_error_indicate(SL651_ERROR_DECODE_INVALID_HEAD);
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

size_t Package_HeadSize(Package const *const me)
{
    assert(me);
    return me->head.stxFlag == SYN ? PACKAGE_HEAD_SYN_LEN : PACKAGE_HEAD_STX_LEN;
}

size_t Package_TailSize(Package const *const me)
{
    assert(me);
    return PACKAGE_TAIL_LEN;
}
// "AbstractClass" Package END

/* LinkMessage Construtor & Destrucor */
void LinkMessage_dtor(Package *const me)
{
    assert(me);
    Package_dtor(me);
    // release elements and elementvector
    LinkMessage *const self = (LinkMessage *const)me;
    int i;
    Element *el;
    vec_foreach(&self->elements, el, i)
    {
        if (el != NULL)
        {
            el->vptr->dtor(el);
            DelInstance(el);
        }
    }
    vec_deinit(&self->elements);
    if (self->rawBuff != NULL)
    {
        BB_dtor(self->rawBuff);
        DelInstance(self->rawBuff);
    }
}

void LinkMessage_ctor(LinkMessage *const me, uint16_t initElementCount)
{
    assert(me);
    Package_ctor(&me->super);
    // keep virtual function inherit from Package.
    // overwrite the dtor
    static PackageVtbl const vtbl = {
        &Package_Virtual_Encode,
        &Package_Virtual_Decode,
        &Package_Virtual_Size,
        &LinkMessage_dtor};
    me->super.vptr = &vtbl;
    if (initElementCount < 0 || initElementCount > MAX_ELEMENT_NUMBER)
    {
        initElementCount = DEFAULT_ELEMENT_NUMBER;
    }
    // init Elements array(Vector) and reserve initElementCount or DEFAULT_ELEMENT_NUMBER
    vec_init(&me->elements);
    vec_reserve(&me->elements, initElementCount);
}

void LinkMessage_PushElement(LinkMessage *const me, Element *const el)
{
    assert(me);
    assert(el);
    vec_push(&me->elements, el);
}

Element *const LinkMessage_ElementAt(LinkMessage *const me, uint8_t index)
{
    assert(me);
    if (index >= 0 && index < me->elements.length)
    {
        return me->elements.data[index];
    }
    return NULL;
}

static bool LinkMessage_EncodeRawBuff(LinkMessage *const me, ByteBuffer *byteBuff)
{
    assert(me);
    return me->rawBuff != NULL
               ? (BB_PutByteBuffer(byteBuff, me->rawBuff) ||
                  set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_RAWBUFF))
               : true;
}

static bool LinkMessage_EncodeElements(LinkMessage *const me, ByteBuffer *byteBuff)
{
    assert(me);
    assert(byteBuff);
    bool res = true;
    Element *el;
    uint8_t i = 0;
    vec_foreach(&me->elements, el, i)
    {
        res = el->vptr->encode(el, byteBuff);
        if (!res)
        {
            // set_error(SL651_ERROR_ENCODE_CANNOT_PROCESS_ELEMENTS);
            break;
        }
    }
    return res;
}

size_t LinkMessage_ElementsSize(LinkMessage const *const me)
{
    assert(me);
    size_t size = 0;
    uint8_t i = 0;
    Element *el;
    vec_foreach(&me->elements, el, i)
    {
        size += el->vptr->size(el);
    }
    return size;
}

size_t LinkMessage_RawByteBuffSize(LinkMessage const *const me)
{
    assert(me);
    return me->rawBuff == NULL ? 0 : BB_Available(me->rawBuff);
}
// "Basic" LinkMessage END

// "AbstractUpClass" UplinkMessage
/* UplinkMessage Construtor & Destrucor */
static size_t UplinkMessage_SelfHeadSize(UplinkMessage const *const me)
{
    assert(me);
    Package const *const pkg = &me->super.super;
    return ((pkg->head.stxFlag == SYN && pkg->head.sequence.seq > 1)
                ? 0
                : (2 + DATETIME_LEN +
                   (isContainObserveTimeElement(pkg->head.funcCode) ? OBSERVETIME_LEN + ELEMENT_IDENTIFER_LEN : 0) +
                   (isContainStationCategoryField(pkg->head.funcCode) ? 1 : 0) +
                   (isContainRemoteStationAddrElement(Up, pkg->head.funcCode) ? REMOTE_STATION_ADDR_LEN + ELEMENT_IDENTIFER_LEN : 0)));
}

static size_t UplinkMessage_HeadSize(UplinkMessage const *const me)
{
    assert(me);
    Package const *const pkg = &me->super.super;
    return Package_HeadSize(pkg) + UplinkMessage_SelfHeadSize(me);
}

static size_t UplinkMessage_Size(Package const *const me)
{
    assert(me);
    return UplinkMessage_HeadSize((UplinkMessage *)me) +
           LinkMessage_RawByteBuffSize((LinkMessage *)me) +
           LinkMessage_ElementsSize((LinkMessage *)me) +
           Package_TailSize(me);
}

static ByteBuffer *UplinkMessage_Encode(Package *const me)
{
    assert(me);
    UplinkMessage *self = (UplinkMessage *)me;
    uint32_t size = UplinkMessage_Size(me);
    if (size <= 0)
    {
        return NULL;
    }
    me->head.len = size - PACKAGE_WRAPPER_LEN;
    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor(byteBuff, size);
    // 编码不按照规则，由调用者按照规范去填写相关字段，有就编码，没有就不编码
    if (UplinkMessage_EncodeHead(self, byteBuff) &&
        LinkMessage_EncodeRawBuff(&self->super, byteBuff) &&
        LinkMessage_EncodeElements(&self->super, byteBuff) &&
        Package_EncodeTail(me, byteBuff))
    {
        return byteBuff;
    }
    else
    {
        BB_dtor(byteBuff);
        DelInstance(byteBuff);
        return NULL;
    }
}

static bool UplinkMessage_Decode(Package *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    bool res = Package_DecodeHead(me, byteBuff) && UplinkMessage_DecodeHead((UplinkMessage *)me, byteBuff);
    if (!res)
    {
        return false;
    }
    // @Todo 分包情况下，后续包是否还需要去解析？
    if (isMessageCombinedByElements(Up, me->head.funcCode) &&
        (me->head.stxFlag == STX || (me->head.stxFlag == SYN && me->head.sequence.seq == 1)) &&
        BB_Available(byteBuff) > ELEMENT_IDENTIFER_LEN + PACKAGE_TAIL_LEN) // 按照要素解码
    {
        ByteBuffer elBuff;
        uint32_t len = me->head.len - UplinkMessage_SelfHeadSize((UplinkMessage *)me);
        if (me->head.stxFlag == SYN)
        {
            len -= 3; // 包总数及序列号
        }
        BB_ctor_wrappedAnother(&elBuff, byteBuff, BB_Position(byteBuff), BB_Position(byteBuff) + len);
        BB_Flip(&elBuff);
        while (BB_Available(&elBuff) > 0)
        {
            Element *el = decodeElement(&elBuff, &(((Package *)me)->head));
            if (el != NULL)
            {
                LinkMessage_PushElement((LinkMessage *const)me, el);
            }
            else
            {
                BB_dtor(&elBuff);
                return false;
            }
        }
        BB_Skip(byteBuff, BB_Position(&elBuff));
        BB_dtor(&elBuff);
    }
    else
    {
        uint16_t bodyLen = me->head.len -
                           UplinkMessage_SelfHeadSize((UplinkMessage *)me) -
                           (me->head.stxFlag == SYN ? PACKAGE_HEAD_SEQUENCE_LEN : 0);
        if (bodyLen > 0 && BB_Available(byteBuff) >= bodyLen + PACKAGE_TAIL_LEN) // 否则交给具体功能码去处理，包括多包的后续包
        {
            ((LinkMessage *)me)->rawBuff = BB_GetByteBuffer(byteBuff, bodyLen);
            BB_Flip(((LinkMessage *)me)->rawBuff);
        }
    }
    // decode tail
    res = BB_Available(byteBuff) >= PACKAGE_TAIL_LEN && Package_DecodeTail(me, byteBuff);
    return res;
}

void UplinkMessage_dtor(Package *const me)
{
    assert(me);
    LinkMessage_dtor(me);
    // UplinkMessage *self = (UplinkMessage *)me;
}

void UplinkMessage_ctor(UplinkMessage *const me, uint16_t initElementCount)
{
    assert(me);
    static PackageVtbl const vtbl = {
        &UplinkMessage_Encode,
        &UplinkMessage_Decode,
        &UplinkMessage_Size,
        &UplinkMessage_dtor};
    LinkMessage_ctor(&me->super, initElementCount);
    // me->super.super.vptr = &vtbl;  // or -->
    ((Package *)me)->vptr = &vtbl; // single inheritance
}

/* Public methods */
bool UplinkMessage_EncodeHead(UplinkMessage const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return (Package_EncodeHead(&me->super.super, byteBuff) &&
            ((me->super.super.head.stxFlag == SYN && me->super.super.head.sequence.seq > 1)
                 ? true
                 : (BB_BE_PutUInt16(byteBuff, me->messageHead.seq) == 2 &&
                    DateTime_Encode(&me->messageHead.sendTime, byteBuff) &&
                    (isContainRemoteStationAddrElement(Up, me->super.super.head.funcCode)
                         ? (BB_PutUInt8(byteBuff, ADDRESS) &&
                            BB_PutUInt8(byteBuff, ADDRESS) &&
                            RemoteStationAddr_Encode(&me->messageHead.stationAddrElement.stationAddr, byteBuff))
                         : true) &&
                    (isContainStationCategoryField(me->super.super.head.funcCode)
                         ? BB_PutUInt8(byteBuff, me->messageHead.stationCategory) == 1
                         : true) &&
                    (isContainObserveTimeElement(me->super.super.head.funcCode)
                         ? (BB_PutUInt8(byteBuff, OBSERVETIME) &&
                            BB_PutUInt8(byteBuff, OBSERVETIME) &&
                            ObserveTime_Encode(&me->messageHead.observeTimeElement.observeTime, byteBuff))
                         : true)))) ||
           set_error_indicate(SL651_ERROR_ENCODE_INVALID_UPLINKMESSAGE_HEAD);
}

bool UplinkMessage_DecodeHead(UplinkMessage *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    // 如果是第二包，则不处理包头
    Package *pkg = (Package *)me;
    if (pkg->head.stxFlag == SYN && pkg->head.sequence.seq > 1)
    {
        return true;
    }
    uint8_t usedLen = BB_BE_GetUInt16(byteBuff, &me->messageHead.seq);
    if (!DateTime_Decode(&me->messageHead.sendTime, byteBuff))
    {
        return false;
    }
    if (isContainRemoteStationAddrElement(Up, me->super.super.head.funcCode))
    {
        BB_GetUInt8(byteBuff, &me->messageHead.stationAddrElement.super.identifierLeader);
        BB_GetUInt8(byteBuff, &me->messageHead.stationAddrElement.super.dataDef);
        if (me->messageHead.stationAddrElement.super.identifierLeader != ADDRESS ||
            (me->messageHead.stationAddrElement.super.dataDef != ADDRESS && me->messageHead.stationAddrElement.super.dataDef != 0x28) || // 支持非标
            !RemoteStationAddr_Decode(&me->messageHead.stationAddrElement.stationAddr, byteBuff))
        {
            return set_error_indicate(SL651_ERROR_INVALID_STATION_ELEMENT);
        }
    }
    bool containCategroyField = isContainStationCategoryField(me->super.super.head.funcCode);
    if (containCategroyField)
    {
        usedLen += BB_GetUInt8(byteBuff, &me->messageHead.stationCategory);
    }
    if (isContainObserveTimeElement(me->super.super.head.funcCode))
    {
        BB_GetUInt8(byteBuff, &me->messageHead.observeTimeElement.super.identifierLeader);
        BB_GetUInt8(byteBuff, &me->messageHead.observeTimeElement.super.dataDef);
        if (me->messageHead.observeTimeElement.super.identifierLeader != OBSERVETIME ||
            (me->messageHead.observeTimeElement.super.dataDef != OBSERVETIME && me->messageHead.observeTimeElement.super.dataDef != 0x28) || // 支持非标
            !ObserveTime_Decode(&me->messageHead.observeTimeElement.observeTime, byteBuff))
        {
            return set_error_indicate(SL651_ERROR_INVALID_OBSERVETIME_ELEMENT);
        }
    }
    return (usedLen == (containCategroyField ? 3 : 2)) || set_error_indicate(SL651_ERROR_DECODE_INVALID_UPLINKMESSAGE_HEAD);
}
// "AbstractUpClass" UplinkMessage END

// "AbstractUpClass" DownlinkMessage

/* DownlinkMessage Construtor  & Destrucor */
static size_t DownlinkMessage_SelfHeadSize(DownlinkMessage *const me)
{
    assert(me);
    Package *pkg = &me->super.super;
    return 2 + // sequence
           DATETIME_LEN +
           (pkg->head.direction == Down &&
                    pkg->head.funcCode == QUERY_TIMERANGE
                ? TIME_STEP_RANGE_LEN
                : 0) +
           (isContainRemoteStationAddrElement(Down, pkg->head.funcCode) ? REMOTE_STATION_ADDR_LEN + ELEMENT_IDENTIFER_LEN : 0);
}

static size_t DownlinkMessage_HeadSize(DownlinkMessage *const me)
{
    assert(me);
    Package *pkg = &me->super.super;
    return Package_HeadSize(pkg) + DownlinkMessage_SelfHeadSize(me);
}

static size_t DownlinkMessage_Size(Package const *const me)
{
    assert(me);
    return DownlinkMessage_HeadSize((DownlinkMessage *)me) +
           LinkMessage_RawByteBuffSize((LinkMessage *)me) +
           LinkMessage_ElementsSize((LinkMessage *)me) +
           Package_TailSize(me);
}

static ByteBuffer *DownlinkMessage_Encode(Package *const me)
{
    assert(me);
    DownlinkMessage *self = (DownlinkMessage *)me;
    uint32_t size = DownlinkMessage_Size(me);
    if (size <= 0)
    {
        return NULL;
    }
    me->head.len = size - PACKAGE_WRAPPER_LEN;
    ByteBuffer *byteBuff = NewInstance(ByteBuffer);
    BB_ctor(byteBuff, size);
    // 编码不按照规则，由调用者按照规范去填写相关字段，有就编码，没有就不编码
    if (DownlinkMessage_EncodeHead(self, byteBuff) &&
        LinkMessage_EncodeRawBuff(&self->super, byteBuff) &&
        LinkMessage_EncodeElements(&self->super, byteBuff) &&
        Package_EncodeTail(me, byteBuff))
    {
        return byteBuff;
    }
    else
    {
        BB_dtor(byteBuff);
        DelInstance(byteBuff);
        return NULL;
    }
}

static bool DownlinkMessage_Decode(Package *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    bool res = Package_DecodeHead(me, byteBuff) && DownlinkMessage_DecodeHead((DownlinkMessage *)me, byteBuff);
    if (!res)
    {
        return false;
    }
    // @Todo 分包情况下，后续包是否还需要去解析？
    if (isMessageCombinedByElements(Down, me->head.funcCode) &&
        (me->head.stxFlag == STX || (me->head.stxFlag == SYN && me->head.sequence.seq == 1)) &&
        BB_Available(byteBuff) >= ELEMENT_IDENTIFER_LEN + PACKAGE_TAIL_LEN) // 按照要素解码
    {
        ByteBuffer elBuff;
        BB_ctor_wrappedAnother(&elBuff, byteBuff, BB_Position(byteBuff), BB_Limit(byteBuff) - PACKAGE_TAIL_LEN);
        BB_Flip(&elBuff);
        while (BB_Available(&elBuff) > 0)
        {
            Element *el = decodeElement(&elBuff, &(((Package *)me)->head));
            if (el != NULL)
            {
                LinkMessage_PushElement((LinkMessage *const)me, el);
            }
            else
            {
                BB_dtor(&elBuff);
                return false;
            }
        }
        BB_Skip(byteBuff, BB_Position(&elBuff));
        BB_dtor(&elBuff);
    }
    else
    {
        uint16_t bodyLen = me->head.len -
                           DownlinkMessage_SelfHeadSize((DownlinkMessage *)me) -
                           (me->head.stxFlag == SYN ? PACKAGE_HEAD_SEQUENCE_LEN : 0);
        if (bodyLen > 0 && BB_Available(byteBuff) >= bodyLen + PACKAGE_TAIL_LEN) // 否则交给具体功能码去处理
        {
            ((LinkMessage *)me)->rawBuff = BB_GetByteBuffer(byteBuff, bodyLen);
            BB_Flip(((LinkMessage *)me)->rawBuff);
        }
    }
    // decode tail
    res = BB_Available(byteBuff) >= 3 && Package_DecodeTail(me, byteBuff);
    return res;
}

void DownlinkMessage_ctor(DownlinkMessage *const me, uint16_t initElementCount)
{
    assert(me);
    static PackageVtbl const vtbl = {
        &DownlinkMessage_Encode,
        &DownlinkMessage_Decode,
        &DownlinkMessage_Size,
        &DownlinkMessage_dtor};
    LinkMessage_ctor(&me->super, initElementCount);
    // me->super.super.vptr = &vtbl;  // or -->
    ((Package *)me)->vptr = &vtbl; // single inheritance
}

void DownlinkMessage_dtor(Package *const me)
{
    assert(me);
    LinkMessage_dtor(me);
    // DownlinkMessage *self = (DownlinkMessage *)me;
}
/* Public methods */
static bool TimeRange_Decode(TimeRange *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < TIME_STEP_RANGE_LEN)
    {
        return set_error_indicate(SL651_ERROR_INSUFFICIENT_TIMERANGE_LEN);
    }
    uint8_t usedLen = BB_BCDGetUInt8(byteBuff, &me->start.year);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->start.month);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->start.day);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->start.hour);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->end.year);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->end.month);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->end.day);
    usedLen += BB_BCDGetUInt8(byteBuff, &me->end.hour);
    return usedLen == TIME_STEP_RANGE_LEN || set_error_indicate(SL651_ERROR_INVALID_TIMERANGE);
}

static bool TimeRange_Encode(TimeRange const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < TIME_STEP_RANGE_LEN)
    {
        return set_error_indicate(SL651_ERROR_INSUFFICIENT_TIMERANGE_LEN);
    }
    uint8_t usedLen = BB_BCDPutUInt8(byteBuff, me->start.year);
    usedLen += BB_BCDPutUInt8(byteBuff, me->start.month);
    usedLen += BB_BCDPutUInt8(byteBuff, me->start.day);
    usedLen += BB_BCDPutUInt8(byteBuff, me->start.hour);
    usedLen += BB_BCDPutUInt8(byteBuff, me->end.year);
    usedLen += BB_BCDPutUInt8(byteBuff, me->end.month);
    usedLen += BB_BCDPutUInt8(byteBuff, me->end.day);
    usedLen += BB_BCDPutUInt8(byteBuff, me->end.hour);
    return usedLen == TIME_STEP_RANGE_LEN || set_error_indicate(SL651_ERROR_INVALID_TIMERANGE);
}

bool DownlinkMessage_EncodeHead(DownlinkMessage const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return (Package_EncodeHead(&me->super.super, byteBuff) &&
            BB_BE_PutUInt16(byteBuff, me->messageHead.seq) == 2 &&
            DateTime_Encode(&me->messageHead.sendTime, byteBuff) &&
            ((me->super.super.head.direction == Down && me->super.super.head.funcCode == QUERY_TIMERANGE)
                 ? TimeRange_Encode(&me->messageHead.timeRange, byteBuff)
                 : true) &&
            (isContainRemoteStationAddrElement(Down, me->super.super.head.funcCode)
                 ? (BB_PutUInt8(byteBuff, ADDRESS) &&
                    BB_PutUInt8(byteBuff, ADDRESS) &&
                    RemoteStationAddr_Encode(&me->messageHead.stationAddrElement.stationAddr, byteBuff))
                 : true)) ||
           set_error_indicate(SL651_ERROR_ENCODE_INVALID_DOWNLINKMESSAGE_HEAD);
    ;
}

bool DownlinkMessage_DecodeHead(DownlinkMessage *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    // 如果是第二包，则不处理包头
    Package *pkg = (Package *)me;
    if (pkg->head.sequence.seq > 1)
    {
        return true;
    }
    uint8_t usedLen = BB_BE_GetUInt16(byteBuff, &me->messageHead.seq);
    if (!DateTime_Decode(&me->messageHead.sendTime, byteBuff))
    {
        return false;
    }
    // 中心站查询遥测站时段数据下行报文 特殊结构
    if ((me->super.super.head.direction == Down &&
         me->super.super.head.funcCode == QUERY_TIMERANGE) &&
        !TimeRange_Decode(&me->messageHead.timeRange, byteBuff))
    {
        return false;
    }
    if (isContainRemoteStationAddrElement(Down, me->super.super.head.funcCode))
    {
        BB_GetUInt8(byteBuff, &me->messageHead.stationAddrElement.super.identifierLeader);
        BB_GetUInt8(byteBuff, &me->messageHead.stationAddrElement.super.dataDef);
        if (me->messageHead.stationAddrElement.super.identifierLeader != ADDRESS ||
            me->messageHead.stationAddrElement.super.dataDef != ADDRESS ||
            !RemoteStationAddr_Decode(&me->messageHead.stationAddrElement.stationAddr, byteBuff))
        {
            return false;
        }
    }
    return usedLen == 2 || set_error_indicate(SL651_ERROR_DECODE_INVALID_DOWNLINKMESSAGE_HEAD);
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
        &Element_Virtual_Size,
        &Element_dtor};
    me->vptr = &vtbl;
    me->identifierLeader = identifierLeader;
    me->dataDef = dataDef;
    me->direction = Up;
}

static bool Element_EncodeIdentifier(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    return (BB_PutUInt8(byteBuff, me->identifierLeader) + BB_PutUInt8(byteBuff, me->dataDef) == 2) ||
           set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_ELEMENT_INDENTIFIER);
}
// "AbstractorClass" Element END

// RemoteStationAddrElement
static bool RemoteStationAddrElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    RemoteStationAddrElement *self = (RemoteStationAddrElement *)me;
    return Element_EncodeIdentifier(me, byteBuff) &&
           RemoteStationAddr_Encode(&self->stationAddr, byteBuff);
}

static bool RemoteStationAddrElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < REMOTE_STATION_ADDR_LEN)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_REMOTEADDR_INSUFFICIENT_LEN);
    }
    RemoteStationAddrElement *self = (RemoteStationAddrElement *)me;
    return RemoteStationAddr_Decode(&self->stationAddr, byteBuff);
}

static size_t RemoteStationAddrElement_Size(Element const *const me)
{
    assert(me);
    return REMOTE_STATION_ADDR_LEN + ELEMENT_IDENTIFER_LEN;
}

void RemoteStationAddrElement_dtor(Element *me)
{
    assert(me);
    Element_dtor(me);
}

void RemoteStationAddrElement_ctor(RemoteStationAddrElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &RemoteStationAddrElement_Encode,
        &RemoteStationAddrElement_Decode,
        &RemoteStationAddrElement_Size,
        &RemoteStationAddrElement_dtor};
    Element_ctor(&me->super, ADDRESS, ADDRESS);
    me->super.vptr = &vtbl;
}
// RemoteStationAddrElement END

// ObserveTimeElement
static bool ObserveTimeElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    ObserveTimeElement *self = (ObserveTimeElement *)me;
    return Element_EncodeIdentifier(me, byteBuff) &&
           ObserveTime_Encode(&self->observeTime, byteBuff);
}

static bool ObserveTimeElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < OBSERVETIME_LEN)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_OBSERVETIME_INSUFFICIENT_LEN);
    }
    ObserveTimeElement *self = (ObserveTimeElement *)me;
    return ObserveTime_Decode(&self->observeTime, byteBuff);
}

static size_t ObserveTimeElement_Size(Element const *const me)
{
    assert(me);
    return OBSERVETIME_LEN + ELEMENT_IDENTIFER_LEN;
}

void ObserveTimeElement_dtor(Element *me)
{
    assert(me);
    Element_dtor(me);
}

void ObserveTimeElement_ctor(ObserveTimeElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &ObserveTimeElement_Encode,
        &ObserveTimeElement_Decode,
        &ObserveTimeElement_Size,
        &Element_dtor};
    Element_ctor(&me->super, OBSERVETIME, OBSERVETIME);
    me->super.vptr = &vtbl;
}
// ObserveTimeElement END

// PictureElement
static bool PictureElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    PictureElement *self = (PictureElement *)me;
    return (self->pkgNo == 1 ? Element_EncodeIdentifier(me, byteBuff) : true) &&
           (BB_PutByteBuffer(byteBuff, self->buff) ||
            set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_PICTURE_DATA));
}

static bool PictureElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < 0) // 截取所有
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_PICTURE_EMPTY);
    }
    PictureElement *self = (PictureElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, BB_Available(byteBuff));
    BB_Flip(self->buff);
    return true;
}

static size_t PictureElement_Size(Element const *const me)
{
    assert(me);
    PictureElement *self = (PictureElement *)me;
    assert(self->buff);
    return (self->pkgNo == 1 ? ELEMENT_IDENTIFER_LEN : 0) + BB_Size(self->buff);
}

void PictureElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    PictureElement *self = (PictureElement *)me;
    if (self->buff)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}

void PictureElement_ctor(PictureElement *const me, uint16_t pkgNo)
{
    // override
    static ElementVtbl const vtbl = {
        &PictureElement_Encode,
        &PictureElement_Decode,
        &PictureElement_Size,
        &PictureElement_dtor};
    Element_ctor(&me->super, PICTURE_IL, PICTURE_IL);
    me->super.vptr = &vtbl;
    me->pkgNo = pkgNo;
}
// PictureElement END

// ArtificialElement ARTIFICIAL_IL

static bool ArtificialElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    ArtificialElement *self = (ArtificialElement *)me;
    assert(self->buff);
    return Element_EncodeIdentifier(me, byteBuff) &&
           (BB_PutByteBuffer(byteBuff, self->buff) ||
            set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_ARTIFICIAL_DATA));
}

static bool ArtificialElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < 0) // 截取所有
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY);
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
    ArtificialElement *self = (ArtificialElement *)me;
    assert(self->buff);
    return ELEMENT_IDENTIFER_LEN + BB_Size(self->buff);
}

void ArtificialElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    ArtificialElement *self = (ArtificialElement *)me;
    if (self->buff)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}

void ArtificialElement_ctor(ArtificialElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &ArtificialElement_Encode,
        &ArtificialElement_Decode,
        &ArtificialElement_Size,
        &ArtificialElement_dtor};
    Element_ctor(&me->super, ARTIFICIAL_IL, ARTIFICIAL_IL);
    me->super.vptr = &vtbl;
}
// ArtificialElement ARTIFICIAL_IL END

// DRP5MINElement

static bool DRP5MINElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    DRP5MINElement *self = (DRP5MINElement *)me;
    if (me->direction == Up && self->buff == NULL)
    {
        return false;
    }
    return me->direction == Up
               ? Element_EncodeIdentifier(me, byteBuff) &&
                     (BB_PutByteBuffer(byteBuff, self->buff) ||
                      set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_DRP5MIN_DATA))
               : Element_EncodeIdentifier(me, byteBuff);
}

static bool DRP5MINElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (me->direction == Down)
    {
        return true;
    }
    if (BB_Available(byteBuff) < DRP5MIN_LEN)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_DRP5MIN_INSUFFICIENT_LEN);
    }
    DRP5MINElement *self = (DRP5MINElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, DRP5MIN_LEN);
    BB_Flip(self->buff);
    return true;
}

static size_t DRP5MINElement_Size(Element const *const me)
{
    assert(me);
    return me->direction == Up
               ? ELEMENT_IDENTIFER_LEN + DRP5MIN_LEN
               : ELEMENT_IDENTIFER_LEN;
}

void DRP5MINElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    DRP5MINElement *self = (DRP5MINElement *)me;
    if (self->buff)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}

void DRP5MINElement_ctor(DRP5MINElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &DRP5MINElement_Encode,
        &DRP5MINElement_Decode,
        &DRP5MINElement_Size,
        &DRP5MINElement_dtor};
    Element_ctor(&me->super, DRP5MIN, DRP5MIN_DATADEF);
    me->super.vptr = &vtbl;
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
    assert(byteBuff);
    FlowRateDataElement *self = (FlowRateDataElement *)me;
    if (me->direction == Up && self->buff == NULL)
    {
        return false;
    }
    return me->direction == Up
               ? Element_EncodeIdentifier(me, byteBuff) &&
                     (BB_PutByteBuffer(byteBuff, self->buff) ||
                      set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_FLOWRATE_DATA))
               : Element_EncodeIdentifier(me, byteBuff);
}

static bool FlowRateDataElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (me->direction == Down)
    {
        return true;
    }
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
    return me->direction == Up
               ? ELEMENT_IDENTIFER_LEN + BB_Size(((FlowRateDataElement *)me)->buff)
               : ELEMENT_IDENTIFER_LEN;
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

void FlowRateDataElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    FlowRateDataElement *self = (FlowRateDataElement *)me;
    if (self->buff)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}
// FlowRateDataElement END

// RelativeWaterLevelElement
static bool RelativeWaterLevelElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    RelativeWaterLevelElement *self = (RelativeWaterLevelElement *)me;
    if (me->direction == Up && self->buff == NULL)
    {
        return false;
    }
    return me->direction == Up
               ? Element_EncodeIdentifier(me, byteBuff) &&
                     (BB_PutByteBuffer(byteBuff, self->buff) ||
                      set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_RELATIVEWATER_DATA))
               : Element_EncodeIdentifier(me, byteBuff);
}

static bool RelativeWaterLevelElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (me->direction == Down)
    {
        return true;
    }
    if (BB_Available(byteBuff) < RELATIVE_WATER_LEVEL_LEN)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_RELATIVEWATERLEVEL_INSUFFICIENT_LEN);
    }
    RelativeWaterLevelElement *self = (RelativeWaterLevelElement *)me;
    self->buff = BB_GetByteBuffer(byteBuff, RELATIVE_WATER_LEVEL_LEN);
    BB_Flip(self->buff);
    return true;
}

static size_t RelativeWaterLevelElement_Size(Element const *const me)
{
    assert(me);
    return me->direction == Up
               ? ELEMENT_IDENTIFER_LEN + RELATIVE_WATER_LEVEL_LEN
               : ELEMENT_IDENTIFER_LEN;
}

void RelativeWaterLevelElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    RelativeWaterLevelElement *self = (RelativeWaterLevelElement *)me;
    if (self->buff)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}

void RelativeWaterLevelElement_ctor(RelativeWaterLevelElement *const me, uint8_t identifierLeader)
{
    // override
    static ElementVtbl const vtbl = {
        &RelativeWaterLevelElement_Encode,
        &RelativeWaterLevelElement_Decode,
        &RelativeWaterLevelElement_Size,
        &RelativeWaterLevelElement_dtor};
    Element_ctor(&me->super, identifierLeader, RELATIVE_WATER_LEVEL_5MIN1_DATADEF);
    me->super.vptr = &vtbl;
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

// StationStatusElement
static bool StationStatusElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    StationStatusElement *self = (StationStatusElement *)me;
    return me->direction == Up
               ? Element_EncodeIdentifier(me, byteBuff) &&
                     (BB_BE_PutUInt32(byteBuff, self->status) == STATION_STATUS_LEN ||
                      set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_STATIONSTATUS_DATA))
               : true;
}

static bool StationStatusElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < STATION_STATUS_LEN)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_STATIONSTATUS_INSUFFICIENT_LEN);
    }
    StationStatusElement *self = (StationStatusElement *)me;
    uint8_t usedLen = BB_BE_GetUInt32(byteBuff, &self->status);
    return usedLen == STATION_STATUS_LEN;
}

static size_t StationStatusElement_Size(Element const *const me)
{
    assert(me);
    return me->direction == Up
               ? STATION_STATUS_LEN + ELEMENT_IDENTIFER_LEN
               : 0;
}

void StationStatusElement_dtor(Element *const me)
{
    Element_dtor(me);
}

void StationStatusElement_ctor(StationStatusElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &StationStatusElement_Encode,
        &StationStatusElement_Decode,
        &StationStatusElement_Size,
        &StationStatusElement_dtor};
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
    assert(byteBuff);
    if (me->direction == Down)
    {
        return true;
    }
    if (!Element_EncodeIdentifier(me, byteBuff))
    {
        return false;
    }
    DurationElement *self = (DurationElement *)me;
    uint8_t writeLen = BB_PutUInt8(byteBuff, self->hour / 10 + '0');
    writeLen = BB_PutUInt8(byteBuff, self->hour % 10 + '0');
    writeLen = BB_PutUInt8(byteBuff, '.');
    writeLen = BB_PutUInt8(byteBuff, self->minute / 10 + '0');
    writeLen = BB_PutUInt8(byteBuff, self->minute % 10 + '0');
    return writeLen == DURATION_OF_XX_LEN || set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_DURATION_DATA);
}

static bool DurationElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < DURATION_OF_XX_LEN)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_DURATION_INSUFFICIENT_LEN);
    }
    DurationElement *self = (DurationElement *)me;
    uint8_t byte = 0;
    // @Todo ASCII TO INT atoi with end.
    uint8_t usedLen = BB_GetUInt8(byteBuff, &self->hour);
    usedLen += BB_GetUInt8(byteBuff, &byte);
    self->hour = (self->hour - '0') * 10 + (byte - '0');
    usedLen += BB_GetUInt8(byteBuff, &byte); // a dot in ascii.
    usedLen += BB_GetUInt8(byteBuff, &self->minute);
    byte = 0;
    usedLen += BB_GetUInt8(byteBuff, &byte);
    self->minute = (self->minute - '0') * 10 + (byte - '0');
    return usedLen == DURATION_OF_XX_LEN;
}

static size_t DurationElement_Size(Element const *const me)
{
    assert(me);
    return me->direction == Up
               ? DURATION_OF_XX_LEN + ELEMENT_IDENTIFER_LEN
               : 0;
}

void DurationElement_dtor(Element *const me)
{
    Element_dtor(me);
}

void DurationElement_ctor(DurationElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &DurationElement_Encode,
        &DurationElement_Decode,
        &DurationElement_Size,
        &DurationElement_dtor};
    Element_ctor(&me->super, DURATION_OF_XX, DURATION_OF_XX_DATADEF);
    me->super.vptr = &vtbl;
}
// DurationElement END

// NumberElement
static bool NumberElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    NumberElement *self = (NumberElement *)me;
    if (me->direction == Up && self->buff == NULL)
    {
        return false;
    }
    if (self->buff != NULL)
    {
        BB_Flip(self->buff);
    }
    return me->direction == Up
               ? Element_EncodeIdentifier(me, byteBuff) &&
                     (BB_PutByteBuffer(byteBuff, self->buff) ||
                      set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_NUMBER_DATA))
               : Element_EncodeIdentifier(me, byteBuff);
}

static bool NumberElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (me->direction == Down)
    {
        return true;
    }
    uint8_t size = me->dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
    if (BB_Available(byteBuff) < size)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_NUMBER_SIZE_NOT_MATCH_DATADEF);
    }
    NumberElement *self = (NumberElement *)me;
    if (self->buff != NULL) // 释放
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
    self->buff = BB_GetByteBuffer(byteBuff, size); //read all bcd
    if (self->buff == NULL)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_NUMBER_SIZE_NOT_MATCH_DATADEF);
    }
    BB_Flip(self->buff); // Flip to read it.
    return true;
}

static size_t NumberElement_Size(Element const *const me)
{
    assert(me);
    return me->direction == Up
               ? ELEMENT_IDENTIFER_LEN + (me->dataDef >> NUMBER_ELEMENT_LEN_OFFSET)
               : ELEMENT_IDENTIFER_LEN;
}

void NumberElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    NumberElement *self = (NumberElement *)me;
    if (self->buff != NULL)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}

void NumberElement_ctor_noBuff(NumberElement *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    assert(me);
    static ElementVtbl const vtbl = {
        &NumberElement_Encode,
        &NumberElement_Decode,
        &NumberElement_Size,
        &NumberElement_dtor};
    Element_ctor(&me->super, identifierLeader, dataDef);
    me->super.vptr = &vtbl;
}

void NumberElement_ctor(NumberElement *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    NumberElement_ctor_noBuff(me, identifierLeader, dataDef);
    if (me->buff == NULL)
    {
        uint8_t size = me->super.dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
        me->buff = NewInstance(ByteBuffer);
        BB_ctor(me->buff, size + 1); // 多一个字节给符号位
    }
}

uint8_t NumberElement_SetInteger(NumberElement *const me, uint64_t val)
{
    assert(me);
    assert(me->buff);
    BB_Clear(me->buff);
    if (val < 0)
    {
        BB_PutUInt8(me->buff, 0xFF);
        val *= -1;
    }
    return BB_BE_BCDPutUInt(me->buff, &val, me->super.dataDef >> NUMBER_ELEMENT_LEN_OFFSET);
}

uint8_t NumberElement_SetFloat(NumberElement *const me, float val)
{
    uint8_t precision = me->super.dataDef & NUMBER_ELEMENT_PRECISION_MASK;
    return NumberElement_SetInteger(me, val * pow(10, precision));
}

uint8_t NumberElement_SetDouble(NumberElement *const me, double val)
{
    uint8_t precision = me->super.dataDef & NUMBER_ELEMENT_PRECISION_MASK;
    return NumberElement_SetInteger(me, val * pow(10, precision));
}

uint8_t NumberElement_GetInteger(NumberElement *const me, uint64_t *val)
{
    assert(me);
    if (me->buff == NULL)
    {
        return 0;
    }
    uint8_t signedFlag = 0;
    uint8_t at = 0;
    BB_PeekUInt8(me->buff, &signedFlag);
    if (signedFlag == 0xFF) // 如果不是负值
    {
        at = 1;
    }
    uint8_t size = me->super.dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
    *val = 0; // 副作用
    uint8_t res = BB_BCDPeekUIntAt(me->buff, at, val, size);
    if (res != size)
    {
        *val = 0xFFFFFFFFFFFFFFFF;
        return res;
    }
    if (signedFlag == 0xFF)
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

uint8_t NumberElement_GetDouble(NumberElement *const me, double *val)
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

// TimeStepCodeElement
// Nest NumberListElement
static bool NumberListElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    NumberListElement *self = (NumberListElement *)me;
    if (me->direction == Up && self->buff == NULL)
    {
        return false;
    }
    if (self->buff != NULL)
    {
        BB_Rewind(self->buff);
    }
    return me->direction == Up
               ? Element_EncodeIdentifier(me, byteBuff) &&
                     (BB_PutByteBuffer(byteBuff, self->buff) ||
                      set_error_indicate(SL651_ERROR_ENCODE_CANNOT_PROCESS_NUMBERLIST_DATA))
               : Element_EncodeIdentifier(me, byteBuff);
}

static bool NumberListElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (me->direction == Down) // 下行消息没有消息体，直接返回
    {
        return true;
    }
    uint8_t size = me->dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
    if (BB_Available(byteBuff) % size != 0)
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_NUMBERLIST_ODD_SIZE);
    }
    NumberListElement *self = (NumberListElement *)me;
    if (self->buff != NULL) // 释放
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
    self->buff = BB_GetByteBuffer(byteBuff, BB_Available(byteBuff)); //read all
    if (self->buff == NULL)
    {
        return false;
    }
    BB_Flip(self->buff); // Flip to read it.
    self->count = BB_Available(self->buff) / size;
    return true;
}

static size_t NumberListElement_Size(Element const *const me)
{
    assert(me);
    NumberListElement *self = (NumberListElement *)me;
    return me->direction == Up
               ? ELEMENT_IDENTIFER_LEN + BB_Size(self->buff)
               : ELEMENT_IDENTIFER_LEN;
}

void NumberListElement_dtor(Element *const me)
{
    assert(me);
    Element_dtor(me);
    NumberListElement *self = (NumberListElement *)me;
    if (self->buff != NULL)
    {
        BB_dtor(self->buff);
        DelInstance(self->buff);
    }
}

void NumberListElement_ctor(NumberListElement *const me, uint8_t identifierLeader, uint8_t dataDef)
{
    static ElementVtbl const vtbl = {
        &NumberListElement_Encode,
        &NumberListElement_Decode,
        &NumberListElement_Size,
        &NumberListElement_dtor};
    Element_ctor(&me->super, identifierLeader, dataDef);
    me->super.vptr = &vtbl;
}

uint8_t NumberListElement_GetIntegerAt(NumberListElement *const me, uint8_t index, uint64_t *val)
{
    assert(me);
    if (index < 0 || me->count <= 0 || index > me->count || me->buff == NULL)
    {
        return 0;
    }
    uint8_t size = me->super.dataDef >> NUMBER_ELEMENT_LEN_OFFSET;
    index = index * size;
    // @Todo 暂时无法支持负值
    // uint8_t signedFlag = 0;
    // BB_PeekUInt8At(me->buff, index, &signedFlag);
    // if (signedFlag == 0xFF) // 如果是负值
    // {
    //     index++;
    // }
    *val = 0; // 副作用
    uint8_t res = BB_BCDPeekUIntAt(me->buff, index, val, size);
    // if (signedFlag == 0xFF && res != 0)
    // {
    //     *val *= -1;
    // }
    if (res != size)
    {
        *val = 0xFFFFFFFFFFFFFF;
    }
    return res;
}

uint8_t NumberListElement_GetFloatAt(NumberListElement *const me, uint8_t index, float *val)
{
    uint64_t u64 = 0;
    uint8_t res = NumberListElement_GetIntegerAt(me, index, &u64);
    uint8_t precision = me->super.dataDef & NUMBER_ELEMENT_PRECISION_MASK;
    *val = u64;
    if (precision > 0)
    {
        *val = u64 / pow(10, precision);
    }
    return res;
}
// Nest NumberListElement END

static bool TimeStepCodeElement_Encode(Element const *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    TimeStepCodeElement *self = (TimeStepCodeElement *)me;
    Element_SetDirection((Element *)&self->numberListElement, self->super.direction);
    return Element_EncodeIdentifier(me, byteBuff) &&
           BB_PutUInt8(byteBuff, self->timeStepCode.day) == 1 &&
           BB_PutUInt8(byteBuff, self->timeStepCode.hour) == 1 &&
           BB_PutUInt8(byteBuff, self->timeStepCode.minute) == 1 &&
           NumberListElement_Encode((Element *)&self->numberListElement, byteBuff);
}

static bool TimeStepCodeElement_Decode(Element *const me, ByteBuffer *const byteBuff)
{
    assert(me);
    assert(byteBuff);
    if (BB_Available(byteBuff) < TIME_STEP_CODE_LEN + ELEMENT_IDENTIFER_LEN) //至少一个时间步长+一个ELEMENT头
    {
        return set_error_indicate(SL651_ERROR_DECODE_ELEMENT_TIMESTEPCODE_INSUFFICIENT_LEN);
    }
    TimeStepCodeElement *self = (TimeStepCodeElement *)me;
    uint8_t usedLen = 0;
    usedLen += BB_BCDGetUInt8(byteBuff, &self->timeStepCode.day);
    usedLen += BB_BCDGetUInt8(byteBuff, &self->timeStepCode.hour);
    usedLen += BB_BCDGetUInt8(byteBuff, &self->timeStepCode.minute);
    // decode NumberListElement
    uint8_t identifierLeader = 0;                        //
    usedLen += BB_GetUInt8(byteBuff, &identifierLeader); // 解析一个字节的 标识符引导符 ， 同时位移
    uint8_t dataDef = 0;                                 //
    usedLen += BB_GetUInt8(byteBuff, &dataDef);          // 解析一个字节的 数据定义符，同时位移
    NumberListElement_ctor(&self->numberListElement, identifierLeader, dataDef);
    Element_SetDirection(&self->numberListElement.super, me->direction);
    if (!NumberListElement_Decode((Element *)&self->numberListElement, byteBuff))
    {
        return false;
    }
    return usedLen == TIME_STEP_CODE_LEN + ELEMENT_IDENTIFER_LEN;
}

static size_t TimeStepCodeElement_Size(Element const *const me)
{
    assert(me);
    TimeStepCodeElement *self = (TimeStepCodeElement *)me;
    // force NumberListElement's direction sync with TimeStepCodeElement
    // here call NumberListElement_Size directly.
    Element_SetDirection((Element *)&self->numberListElement, self->super.direction);
    return TIME_STEP_CODE_LEN + ELEMENT_IDENTIFER_LEN + NumberListElement_Size((Element *)&self->numberListElement);
}

void TimeStepCodeElement_dtor(Element *me)
{
    assert(me);
    Element_dtor(me);
    TimeStepCodeElement *self = (TimeStepCodeElement *)me;
    NumberListElement_dtor((Element *)&self->numberListElement);
}

void TimeStepCodeElement_ctor(TimeStepCodeElement *const me)
{
    // override
    static ElementVtbl const vtbl = {
        &TimeStepCodeElement_Encode,
        &TimeStepCodeElement_Decode,
        &TimeStepCodeElement_Size,
        &TimeStepCodeElement_dtor};
    Element_ctor(&me->super, TIME_STEP_CODE, TIME_STEP_CODE_DATADEF);
    me->super.vptr = &vtbl;
}
// TimeStepCodeElement END
// Elements END

// Decode & Encode
// Util Functions
// ByteBuffer should be in read mode
Element *decodeElement(ByteBuffer *const byteBuff, Head *const head)
{
    assert(byteBuff);
    if (BB_Available(byteBuff) < ELEMENT_IDENTIFER_LEN)
    {
        set_error(SL651_ERROR_DECODE_ELEMENT_INSUFFICIENT_LEN);
        return NULL;
    }                                                          // 至少包含标识符
    uint8_t identifierLeader = 0;                              //
    BB_GetUInt8(byteBuff, &identifierLeader);                  // 解析一个字节的 标识符引导符 ， 同时位移
    uint8_t dataDef = 0;                                       //
    BB_GetUInt8(byteBuff, &dataDef);                           // 解析一个字节的 数据定义符，同时位移
    Element *el = NULL;                                        // 根据标识符引导符，开始解析 Element
    bool decoded = false;                                      //
    switch (identifierLeader)                                  //
    {                                                          //
    case CUSTOM_IDENTIFIER:                                    // unsupport
        set_error(SL651_ERROR_DECODE_ELEMENT_UNSUPPORTCUSTOM); //
        return NULL;                                           // 返回NULL
    case OBSERVETIME:                                          // 观测时间 Element
        el = (Element *)(NewInstance(ObserveTimeElement));     // 创建指针，需要转为Element*
        ObserveTimeElement_ctor((ObserveTimeElement *)el);     // 构造函数
        break;
    case ADDRESS:
        el = (Element *)(NewInstance(RemoteStationAddrElement));       // 创建指针，需要转为Element*
        RemoteStationAddrElement_ctor((RemoteStationAddrElement *)el); // 构造函数
        break;
    case ARTIFICIAL_IL:
        el = (Element *)(NewInstance(ArtificialElement)); // 创建指针，需要转为Element*
        ArtificialElement_ctor((ArtificialElement *)el);  // 构造函数
        break;
    case PICTURE_IL:
        el = (Element *)(NewInstance(PictureElement));                 // 创建指针，需要转为Element*
        PictureElement_ctor((PictureElement *)el, head->sequence.seq); // 构造函数
        break;
    case DRP5MIN:
        if (dataDef != DRP5MIN_DATADEF) //固定为 0x60
        {
            set_error(SL651_ERROR_DECODE_ELEMENT_DRP5MIN_DATADEF_ERROR);
            return NULL;
        }
        el = (Element *)(NewInstance(DRP5MINElement)); // 创建指针，需要转为Element*
        DRP5MINElement_ctor((DRP5MINElement *)el);     // 构造函数
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
            set_error(SL651_ERROR_DECODE_ELEMENT_RELATIVEWATERLEVEL_DATADEF_ERROR);
            return NULL;
        }
        el = (Element *)(NewInstance(RelativeWaterLevelElement));                          // 创建指针，需要转为Element*
        RelativeWaterLevelElement_ctor((RelativeWaterLevelElement *)el, identifierLeader); // 构造函数
        break;
    case FLOW_RATE_DATA:
        if (dataDef != FLOW_RATE_DATA_DATADEF) //固定为 0xF6
        {
            set_error(SL651_ERROR_DECODE_ELEMENT_FLOWRATE_DATADEF_ERROR);
            return NULL;
        }
        el = (Element *)(NewInstance(FlowRateDataElement));  // 创建指针，需要转为Element*
        FlowRateDataElement_ctor((FlowRateDataElement *)el); // 构造函数
        break;
    case TIME_STEP_CODE:
        if (dataDef != TIME_STEP_CODE_DATADEF) //固定为 0x18
        {
            set_error(SL651_ERROR_DECODE_ELEMENT_TIMESTEPCODE_DATADEF_ERROR);
            return NULL;
        }
        el = (Element *)(NewInstance(TimeStepCodeElement));  // 创建指针，需要转为Element*
        TimeStepCodeElement_ctor((TimeStepCodeElement *)el); // 构造函数
        break;
    case STATION_STATUS:
        if (dataDef != STATION_STATUS_DATADEF) //固定为 0x60
        {
            set_error(SL651_ERROR_DECODE_ELEMENT_STATIONSTATUS_DATADEF_ERROR);
            return NULL;
        }
        el = (Element *)(NewInstance(StationStatusElement));   // 创建指针，需要转为Element*
        StationStatusElement_ctor((StationStatusElement *)el); // 构造函数
        break;
    case DURATION_OF_XX:
        if (dataDef != DURATION_OF_XX_DATADEF) //固定为 0x60
        {
            set_error(SL651_ERROR_DECODE_ELEMENT_DURATION_DATADEF_ERROR);
            return NULL;
        }
        el = (Element *)(NewInstance(DurationElement)); // 创建指针，需要转为Element*
        DurationElement_ctor((DurationElement *)el);    // 构造函数
        break;
    default:
        // 按照数据类型解析
        if (isNumberElement(identifierLeader))
        {
            el = (Element *)NewInstance(NumberElement);
            NumberElement_ctor_noBuff((NumberElement *)el, identifierLeader, dataDef);
        }
        else
        {
            set_error(SL651_ERROR_DECODE_ELEMENT_UNKOWN_INDENTIFIERLEADER);
            return NULL;
        }
    }
    if (el != NULL)
    {
        Element_SetDirection(el, head->direction);
        decoded = el->vptr->decode(el, byteBuff); // 解析
        if (!decoded)                             // 解析失败，需要手动删除指针
        {                                         //
            if (el->vptr->dtor != NULL)           // 实现了析构函数
            {                                     //
                el->vptr->dtor(el);               // 调用析构，规范步骤
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
        set_error(SL651_ERROR_DECODE_INSUFFICIENT_HEAD_LEN);
        return NULL;
    }
    // crc
    // uint16_t crcCalc = 0;
    // uint16_t crcInBuf = 0;
    // uint32_t dataEnd = buffSize - (PACKAGE_TAIL_LEN - 1);
    // BB_BE_PeekUInt16At(byteBuff, dataEnd, &crcInBuf);
    // BB_CRC16(byteBuff, &crcCalc, 0, dataEnd);
    // if (crcCalc != crcInBuf)
    // {
    //     set_error(SL651_ERROR_DECODE_INVALID_CRC);
    //     printf("sl651 invalid CRC, E[%4x] A[%4X]\r\n", crcCalc, crcInBuf);
    //     return NULL;
    // }
    // Direction
    uint8_t direction = Down;
    BB_PeekUInt8At(byteBuff, BB_Position(byteBuff) + PACKAGE_HEAD_STX_DIRECTION_INDEX, &direction);
    direction >>= PACKAGE_HEAD_STX_DIRECTION_INDEX_MASK_BIT;
    // @Todo ASCII 模式
    // DataTransMode transMode = TRANS_IN_BINARY;
    Package *pkg = NULL;
    bool decoded = false; //
    switch (direction)
    {
    case Up:
        pkg = (Package *)NewInstance(UplinkMessage);
        UplinkMessage_ctor((UplinkMessage *)pkg, DEFAULT_ELEMENT_NUMBER);
        break;
    case Down:
        pkg = (Package *)NewInstance(DownlinkMessage);
        DownlinkMessage_ctor((DownlinkMessage *)pkg, DEFAULT_ELEMENT_NUMBER);
        break;
    default:
        set_error(SL651_ERROR_INVALID_DIRECTION);
        return NULL;
    }
    if (pkg != NULL)
    {
        decoded = pkg->vptr->decode(pkg, byteBuff); // 解析
        if (!decoded)                               // 解析失败，需要手动删除指针
        {                                           //
            if (pkg->vptr->dtor != NULL)            // 实现了析构函数
            {                                       //
                pkg->vptr->dtor(pkg);               // 调用析构，规范步骤
            }                                       //
            DelInstance(pkg);                       // 删除指针
            return NULL;                            //
        }
    }
    return pkg;
}
// Decode & Encode END
// #pragma pack()
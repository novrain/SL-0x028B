#ifndef H_SL651
#define H_SL651

#ifdef __cplusplus
extern "C"
{
#endif
// std
#include <stdint.h>
#include <stdbool.h>
// others
#include "vec/vec.h"

#include "common/error.h"
#include "bytebuffer/bytebuffer.h"

    typedef enum
    {
        SL651_ERROR_SUCCESS = ERROR_ENUM_BEGIN_RANGE(0),
        // COMMON
        SL651_ERROR_INVALID_SOH,                 // SOH无效
        SL651_ERROR_INVALID_DIRECTION,           // 头部中的上下行标识错误
        SL651_ERROR_INVALID_STATION_ADDR,        // 遥测站地址错误（可能存在无效的BCD码）
        SL651_ERROR_INVALID_STATION_ELEMENT,     // 遥测站地址错误（标识符或地址错误）
        SL651_ERROR_INVALID_DATATIME,            // 发报时间错误（可能存在无效的BCD码）
        SL651_ERROR_INVALID_OBSERVETIME,         // 无效的观测时间（可能存在无效的BCD码）
        SL651_ERROR_INVALID_OBSERVETIME_ELEMENT, // 无效的观测时间（标识符或者观测时间错误）
        SL651_ERROR_INVALID_TIMERANGE,           // 无效的起始时间
        SL651_ERROR_INSUFFICIENT_TIMERANGE_LEN,  // 无足够的空间读写
        // DECODE
        SL651_ERROR_DECODE_INSUFFICIENT_HEAD_LEN,        // 消息头部长度不足
        SL651_ERROR_DECODE_INVALID_HEAD,                 // 无效的消息头
        SL651_ERROR_DECODE_INVALID_UPLINKMESSAGE_HEAD,   // 无效的上行报文头
        SL651_ERROR_DECODE_INVALID_DOWNLINKMESSAGE_HEAD, // 无效的下行报文头
        SL651_ERROR_DECODE_INVALID_CRC,                  // 无效的CRC
        // DECODE ELEMENT
        SL651_ERROR_DECODE_ELEMENT_INSUFFICIENT_LEN,                    // ELEMENT数据长度不足
        SL651_ERROR_DECODE_ELEMENT_UNKOWN_INDENTIFIERLEADER,            // 未知的ELEMENT 标识符前导符
        SL651_ERROR_DECODE_ELEMENT_UNSUPPORTCUSTOM,                     // 暂不支持自定义ELEMENT
        SL651_ERROR_DECODE_ELEMENT_OBSERVETIME_INSUFFICIENT_LEN,        // 观测时间ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_REMOTEADDR_INSUFFICIENT_LEN,         // 遥测站地址ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_ARTIFICIAL_EMPTY,                    // 空的ARTIFICIAL ELEMENT
        SL651_ERROR_DECODE_ELEMENT_PICTURE_EMPTY,                       // 空的PICTURE ELEMENT
        SL651_ERROR_DECODE_ELEMENT_DRP5MIN_DATADEF_ERROR,               // 错误的DPR5MIN ELEMENT 数据定义
        SL651_ERROR_DECODE_ELEMENT_DRP5MIN_INSUFFICIENT_LEN,            // DRP5MIN ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_RELATIVEWATERLEVEL_DATADEF_ERROR,    // 错误的RELATIVEWATERLEVEL ELEMENT 数据定义
        SL651_ERROR_DECODE_ELEMENT_RELATIVEWATERLEVEL_INSUFFICIENT_LEN, // RELATIVEWATERLEVEL ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_FLOWRATE_DATADEF_ERROR,              // 错误的FLOWRATE ELEMENT 数据定义
        SL651_ERROR_DECODE_ELEMENT_FLOWRATE_INSUFFICIENT_LEN,           // FLOWRATE ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_TIMESTEPCODE_DATADEF_ERROR,          // 错误的 TIMESTEPCODE ELEMENT 数据定义
        SL651_ERROR_DECODE_ELEMENT_TIMESTEPCODE_INSUFFICIENT_LEN,       // TIMESTEPCODE ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_STATIONSTATUS_DATADEF_ERROR,         // 错误的 STATIONSTATUS ELEMENT 数据定义
        SL651_ERROR_DECODE_ELEMENT_STATIONSTATUS_INSUFFICIENT_LEN,      // TIMESTEPCODE ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_DURATION_DATADEF_ERROR,              // 错误的 DURATION ELEMENT 数据定义
        SL651_ERROR_DECODE_ELEMENT_DURATION_INSUFFICIENT_LEN,           // DURATION ELEMENT长度不够
        SL651_ERROR_DECODE_ELEMENT_NUMBERLIST_ODD_SIZE,                 // NUMBER LIST 奇数字节数
        SL651_ERROR_DECODE_ELEMENT_NUMBER_SIZE_NOT_MATCH_DATADEF,       // NUMBER ELEMENT MISSING MATCH DATADEF
        // ENCODE
        SL651_ERROR_ENCODE_INVALID_HEAD,                       // 无效的消息头
        SL651_ERROR_ENCODE_INVALID_UPLINKMESSAGE_HEAD,         // 无效的 UPLINKMESSAGEHEAD
        SL651_ERROR_ENCODE_INVALID_DOWNLINKMESSAGE_HEAD,       // 无效的 DOWNLINKMESSAGEHEAD
        SL651_ERROR_ENCODE_CANNOT_PROCESS_RAWBUFF,             // 无法处理自定义数据
        SL651_ERROR_ENCODE_CANNOT_PROCESS_ELEMENTS,            // 无法处理要素
        SL651_ERROR_ENCODE_CANNOT_PROCESS_ELEMENT_INDENTIFIER, // 无法处理 ELEMENT INDENTIFIER
        SL651_ERROR_ENCODE_CANNOT_PROCESS_PICTURE_DATA,        // 无法处理 PICTURE data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_ARTIFICIAL_DATA,     // 无法处理ARTIFICIAL data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_DRP5MIN_DATA,        // 无法处理DRP5MIN data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_FLOWRATE_DATA,       // 无法处理FLOWRATE data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_RELATIVEWATER_DATA,  // 无法处理FLOWRATE data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_STATIONSTATUS_DATA,  // 无法处理STATIONSTATUS data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_DURATION_DATA,       // 无法处理DURATION data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_NUMBER_DATA,         // 无法处理NUMBER data
        SL651_ERROR_ENCODE_CANNOT_PROCESS_NUMBERLIST_DATA,     // 无法处理NUMBERLIST data
        SL651_ERROR_ENCODE_FAIL_CALC_CRC,                      // 无法计算CRC
    } SL651ProtocolError;

    typedef enum
    {
        // 二进制报文，(注：协议中的HEX模式，并非HEX STR；同时也是混杂模式，部分数据为BCD/ASCII)
        TRANS_IN_BINARY,
        // ASCII字符编码报文 (注：不严谨，混杂模式，标识符试用ASCII，数据部分有的转为ASCII(实际是HEX STR)，有的还是BINARY模式)
        TRANS_IN_ASCII
    } DataTransMode;

/**
 *  报文帧控制字符定义 
 */
//SOH(start of header)
#define SOH_ASCII 0x01    //ASCII字符编码报文帧起始
#define SOH_BINARY 0x7E7E //HEX/BCD编码报文帧起始
//STX (start of text)
#define STX 0x02 //传输正文起始
//SYN (synchronous idle)
#define SYN 0x16 //多包传输正文起始
//ETX (end of text)
#define ETX 0x03 //报文结束，后续无报文
//ETB (end of transmission block)
#define ETB 0x17 //报文结束，后续有报文
//ENQ (enquiry)
#define ENQ 0x05 //询问
//EOT (end of transmission)
#define EOT 0x04 //传输结束，退出
//ACK (acknowledge)
#define ACK 0x06 //肯定确认，继续发送
//NAK (negative acknowledge)
#define NAK 0x15 //否定应答，反馈重发
//ESC (escape)
#define ESC 0x1b //传输结束，终端保持在线

    enum StationCategory
    {
        // 降水  P
        RAIN_STATION = 0x50,
        // 河道  H
        RIVER_STATION = 0x48,
        // 水库/湖泊 K
        RESERVOIR_STATION = 0x4B,
        // 闸坝  Z
        DAM_STATION = 0x5A,
        // 泵站  D
        PUMPING_STATION = 0x44,
        // 潮汐  T
        TIDE_STATION = 0x54,
        // 墒情  M
        SOIL_MOISTURE_STAION = 0x4D,
        // 地下水 G
        GROUNDWATER_STATION = 0x47,
        // 水质  Q
        WATER_QUALITY_STATION = 0x51,
        // 取水口 I
        WATER_INTAKE_STATION = 0x49,
        // 排水口 O
        DRIAN_STATION = 0x4F
    };

    typedef enum
    {
        // 链路维持报
        KEEPALIVE = 0x2F,
        // 试试报
        TEST = 0x30,
        // 均匀时段水文信息报
        EVEN_TIME,
        // 遥测站定时报
        INTERVAL,
        // 遥测站加报报
        ADDED,
        // 遥测站小时报
        HOUR,
        // 遥测站人工置数报
        ARTIFICIAL,
        // 遥测站图片报 或 中心站查询遥测站图片采集信息
        PICTURE,
        // 中心站查询遥测站实时数据
        QUERY_REALTIME,
        // 中心站查询遥测站时段数据
        QUERY_TIMERANGE,
        // 中心站查询遥测站人工置数
        QUERY_ARTIFICIAL,
        // 中心站查询遥测站指定要素数据
        QUERY_ELEMENT,
        // 中心站修改遥测站基本配置表
        MODIFY_BASIC_CONFIG = 0x40,
        // 中心站读取遥测站基本配置表/遥测站自报基本配置表
        BASIC_CONFIG,
        // 中心站修改遥测站运行参数配置表
        MODIFY_RUNTIME_CONFIG,
        // 中心站读取遥测站运行参数配置表/遥测站自报运行参数配置表
        RUNTIME_CONFIG,
        // 查询水泵电机实时工作数据
        QUERY_PUMPING_REALTIME,
        // 查询遥测终端软件版本
        QUERY_SOFTWARE_VERSION,
        // 查询遥测站状态和报警信息
        QUERY_STATUS,
        // 初始化固态存储数据
        INIT_STORAGE,
        // 恢复终端出厂设置
        RESET,
        // 修改密码
        CHANGE_PASSWORD,
        // 设置遥测站时钟
        SET_CLOCK,
        // 设置遥测终端IC卡状态
        SET_IC,
        // 控制水泵开关命令/水泵状态信息自报
        PUMPING_SWITCH,
        // 控制阀门开关命令/阀门状态信息自报
        VALVE_SWITCH,
        // 控制闸门开关命令/闸门状态信息自报
        GATE_SWITCH,
        // 水量定值控制命令
        WATER_VOLUME_SETTING,
        // 中心站查询遥测站事件记录
        QUERY_LOG,
        // 中心站查询遥测站时钟
        QUERY_CLOCK
    } FunctionCode;

    typedef enum
    {
        Down = 1 << 3, // 4 bit: 1000
        Up = 0         // 4 bit: 0000
    } Direction;

    /**
     * @description: 根据功能码判断上行报文头是否包含StationCategory
     * @param {type} 
     * @return: 
     */
    static bool inline isContainStationCategoryField(uint8_t functionCode)
    {
        switch (functionCode)
        {
        case HOUR:
        case ADDED:
        case TEST:
        case EVEN_TIME:
        case INTERVAL:
        case PICTURE:
        case QUERY_REALTIME:
        case QUERY_TIMERANGE:
        case QUERY_ELEMENT:
            return true;
        default:
            return false;
        }
    }

    /**
     * @description: 根据功能码判断上行报文头是否包含ObserveTimeElement
     * @param {type} 
     * @return: 
     */
    static bool inline isContainObserveTimeElement(uint8_t functionCode)
    {
        switch (functionCode)
        {
        case HOUR:
        case ADDED:
        case TEST:
        case EVEN_TIME:
        case INTERVAL:
        case PICTURE:
        case QUERY_REALTIME:
        case QUERY_TIMERANGE:
        case QUERY_ELEMENT:
            return true;
        default:
            return false;
        }
    }

    /**
     * @description: 根据功能码判断报文是否为要素组成
     * @param {type} 
     * @return: 
     */
    static bool inline isMessageCombinedByElements(Direction direction, uint8_t functionCode)
    {
        switch (direction)
        {
        case Up:
            switch (functionCode)
            {
            case HOUR:
            case ADDED:
            case TEST:
            case EVEN_TIME:
            case INTERVAL:
            case ARTIFICIAL:
            case PICTURE:
            case QUERY_REALTIME:
            case QUERY_TIMERANGE:
            case QUERY_ELEMENT:
                return true;
            default:
                return false;
            }
        case Down:
            switch (functionCode)
            {
            case QUERY_TIMERANGE:
            case QUERY_ELEMENT:
                return true;
            default:
                return false;
            }
        default:
            return false;
        }
    }

    /**
     * @description: 根据功能码判断报文头是否包含RemoteStationAddrElement
     * @param {type} 
     * @return: 
     */
    static bool inline isContainRemoteStationAddrElement(Direction direction, uint8_t functionCode)
    {
        switch (direction)
        {
        case Up:
            switch (functionCode)
            {
            case HOUR:
            case TEST:
            case ADDED:
            case EVEN_TIME:
            case INTERVAL:
            case PICTURE:
            case QUERY_REALTIME:
            case QUERY_TIMERANGE:
            case QUERY_ELEMENT:
            case MODIFY_BASIC_CONFIG:
            case BASIC_CONFIG:
            case MODIFY_RUNTIME_CONFIG:
            case RUNTIME_CONFIG:
            case QUERY_PUMPING_REALTIME:
            case QUERY_SOFTWARE_VERSION:
            case QUERY_STATUS:
            case INIT_STORAGE:
            case RESET:
            case CHANGE_PASSWORD:
            case SET_CLOCK:
            case SET_IC:
            case PUMPING_SWITCH:
            case VALVE_SWITCH:
            case GATE_SWITCH:
            case WATER_VOLUME_SETTING:
            case QUERY_LOG:
            case QUERY_CLOCK:
                return true;
            default:
                return false;
            }
            break;
        case Down:
            switch (functionCode)
            {
            default:
                return false;
            }
            break;
        default:
            return false;
        }
    }

#define ELEMENT_IDENTIFER_LEN 2
#define ELEMENT_IDENTIFER_LEADER_LEN 1
#define NUMBER_ELEMENT_LEN_OFFSET 3
#define NUMBER_ELEMENT_PRECISION_MASK 0x07 // 00000111

    /* 标识符引导符*/
    enum IdentifierLeader
    {
        /* 
        * 特殊标识符，单独解析 
        * F0 - FD && 04 && 05 && 45
        */
        // 观测时间引导符
        OBSERVETIME = 0xF0,
        // 遥测站编码引导符
        ADDRESS = 0xF1,
        // 人工置数
        ARTIFICIAL_IL = 0xF2,
        // 图片信息
        PICTURE_IL = 0xF3,
        // 1小时内每5min时段雨量
        DRP5MIN = 0xF4,
        // 1小时内每5min间隔相对水位1, 以下相同
        RELATIVE_WATER_LEVEL_5MIN1 = 0xF5,
        RELATIVE_WATER_LEVEL_5MIN2 = 0xF6,
        RELATIVE_WATER_LEVEL_5MIN3 = 0xF7,
        RELATIVE_WATER_LEVEL_5MIN4 = 0xF8,
        RELATIVE_WATER_LEVEL_5MIN5 = 0xF9,
        RELATIVE_WATER_LEVEL_5MIN6 = 0xFA,
        RELATIVE_WATER_LEVEL_5MIN7 = 0xFB,
        RELATIVE_WATER_LEVEL_5MIN8 = 0xFC,
        // 流速批量数据
        FLOW_RATE_DATA = 0xFD,
        // 时间步长码
        TIME_STEP_CODE = 0x04,
        // 时段长，降水，引排水，抽水历时
        DURATION_OF_XX = 0x05,
        // 遥测站状态及报警信息
        STATION_STATUS = 0x45,
        // 用户自定义引导符，暂不支持
        CUSTOM_IDENTIFIER = 0xFF
    };

    // enum DataType
    // {

    // };

    // Package Struct
    // #pragma pack(1)
    /* 遥测站地址 */
    typedef struct
    {
        /**
         * A5 == 0, A4-A1 为BCD码， 组成地址
         * A5 != 0, A5-A3 BCD码，为行政区划；A2A1 HEX 为一个short值
         */
        uint8_t A5;
        uint8_t A4;
        uint8_t A3;
        uint8_t A2;
        uint8_t A1;
        // 当 A5 非 0 时，A2 A1 A0 组成一个自定义地址，为BCD码，从原始的 A2A1 HEX 转来
        uint8_t A0;
    } RemoteStationAddr;
    bool RemoteStationAddr_Encode(RemoteStationAddr const *const me, ByteBuffer *const byteBuff);
    bool RemoteStationAddr_Decode(RemoteStationAddr *const me, ByteBuffer *const byteBuff);
#define A5_HYDROLOGICAL_TELEMETRY_STATION 0
#define REMOTE_STATION_ADDR_LEN 5

    typedef struct
    {
        // 6字节BCD码
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } DateTime;

    void DateTime_now(DateTime *const me);

#define DATETIME_LEN 6

    typedef struct
    {
        // 5字节BCD码
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
    } ObserveTime;

    void ObserveTime_now(ObserveTime *const me);

#define OBSERVETIME_LEN 5

    typedef struct
    {
        uint16_t count;
        uint16_t seq;
    } Sequence;

    /* Head */
    typedef struct
    {
        uint16_t soh;
        uint8_t centerAddr;
        RemoteStationAddr stationAddr;
        uint16_t password;
        uint8_t funcCode;
        uint8_t direction;
        uint16_t len;
        uint8_t stxFlag;
        // if stxFlag == SNY
        Sequence sequence;
    } Head;

#define PACKAGE_HEAD_STX_LEN 14
#define PACKAGE_HEAD_SEQUENCE_LEN 3
#define PACKAGE_HEAD_SNY_LEN (PACKAGE_HEAD_STX_LEN + PACKAGE_HEAD_SEQUENCE_LEN)
#define PACKAGE_HEAD_STX_DIRECTION_INDEX 11
#define PACKAGE_HEAD_STX_DIRECTION_INDEX_MASK_BIT 4 // u16: 12 u8: 4
#define PACKAGE_HEAD_STX_BODY_LEN_MASK 0xFFF
#define PACKAGE_HEAD_SEQUENCE_COUNT_BIT_MASK_LEN 12
#define PACKAGE_HEAD_SEQUENCE_SEQ_MASK 0xFFF
#define PACKAGE_HEAD_SEQUENCE_COUNT_MASK 0xFFF

// oop
#define Head_ctor(ptr_)
#define Head_dtor(ptr_)
    // "AbstractorClass" Element
    struct ElementVtbl; /* forward declaration */
    typedef struct
    {
        struct ElementVtbl const *vptr;
        uint8_t identifierLeader;
        uint8_t dataDef;
        uint8_t direction;
    } Element;

    typedef struct ElementVtbl
    {
        // pure virtual
        bool (*encode)(Element const *const me, ByteBuffer *const byteBuff);
        bool (*decode)(Element *const me, ByteBuffer *const byteBuff);
        size_t (*size)(Element const *const me);
        void (*dtor)(Element *const me);
    } ElementVtbl;

    void Element_ctor(Element *me, uint8_t identifierLeader, uint8_t dataDef);
    // an empty desctrutor implements
    static inline void Element_dtor(Element *const me)
    {
        return;
    }
#define Element_SetDirection(ptr_, d) (ptr_)->direction = (d)
#define Element_GetDirection(ptr_) (ptr_)->direction
    // "AbstractorClass" Element END

    // RemoteStationAddrElement
    typedef struct
    {
        // it is fixed value, 0xF1F1
        Element super;
        RemoteStationAddr stationAddr;
    } RemoteStationAddrElement;
    void RemoteStationAddrElement_ctor(RemoteStationAddrElement *const me);
    void RemoteStationAddrElement_dtor(Element *me);
    // RemoteStationAddrElement END

    // ObserveTimeElement
    typedef struct
    {
        // it is fixed value, 0xF1F1
        Element super;
        ObserveTime observeTime;
    } ObserveTimeElement;

    void ObserveTimeElement_ctor(ObserveTimeElement *const me);
    void ObserveTimeElement_dtor(Element *me); // empty implements
    // ObserveTimeElement END

    typedef struct
    {
        uint16_t seq;
        DateTime sendTime;
        RemoteStationAddrElement stationAddrElement;
        uint8_t stationCategory;
        ObserveTimeElement observeTimeElement;
    } UplinkMessageHead;

#define UPLINK_MESSAGE_HEAD_LEN 22

    typedef struct
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
    } Time;

    typedef struct
    {
        Time start;
        Time end;
    } TimeRange;
#define TIME_STEP_RANGE_LEN 8
    typedef struct
    {
        uint16_t seq;
        DateTime sendTime;
        TimeRange timeRange;
        RemoteStationAddrElement stationAddrElement;
    } DownlinkMessageHead;
    size_t DownlinkMessageHead_Size(DownlinkMessageHead const *const me);

    typedef struct
    {
        uint8_t etxFlag;
        uint16_t crc;
    } Tail;

#define PACKAGE_TAIL_LEN 3
#define PACKAGE_WRAPPER_LEN 17 // PACKAGE_HEAD_STX_LEN + PACKAGE_TAIL_LEN

    // "AbstractClass" Package
    struct PackageVtbl; /* forward declaration */
    typedef struct
    {
        struct PackageVtbl const *vptr; /* <== Package's Virtual Pointer */
        Head head;
        Tail tail;
    } Package;

    /* Package's virtual table */
    typedef struct PackageVtbl
    {
        // pure virtual
        ByteBuffer *(*encode)(Package *const me);
        bool (*decode)(Package *const me, ByteBuffer *const byteBuff);
        size_t (*size)(Package const *const me);
        // to call the subclass's desctrutor as a superclass.
        void (*dtor)(Package *const me);
    } PackageVtbl;

    /* Package Construtor & Destrucor */
    void Package_ctor(Package *const me);
    /* Public methods */
    bool Package_EncodeHead(Package const *const me, ByteBuffer *const byteBuff);
    bool Package_EncodeTail(Package const *const me, ByteBuffer *const byteBuff);
    bool Package_DecodeHead(Package *const me, ByteBuffer *const byteBuff);
    bool Package_DecodeTail(Package *const me, ByteBuffer *const byteBuff);
    size_t Package_HeadSize(Package const *const me);
    size_t Package_TailSize(Package const *const me);
    // an empty desctrutor implements
    static inline void Package_dtor(Package *const me)
    {
        return;
    }
/* Public Helper*/
#define PACAKAGE_UPCAST(ptr_) ((Package *)(ptr_))
#define Package_Direction(me_) (PACAKAGE_UPCAST(me_)->head.direction)
    // "AbstractClass" Package END

    // "Basic" LinkMessage
    // Dynamic Array for Element @see https://github.com/rxi/vec
    typedef vec_t(Element *) ElementPtrVector;
#define DEFAULT_ELEMENT_NUMBER 5
#define MAX_ELEMENT_NUMBER 50
    typedef struct
    {
        Package super;
        ElementPtrVector elements;
        ByteBuffer *rawBuff;
    } LinkMessage;

    /* LinkMessage Construtor & Destrucor */
    void LinkMessage_ctor(LinkMessage *const me, uint16_t initElementCount);
    void LinkMessage_dtor(Package *const me);
    size_t LinkMessage_ElementsSize(LinkMessage const *const me);
    size_t LinkMessage_RawByteBuffSize(LinkMessage const *const me);
    void LinkMessage_PushElement(LinkMessage *const me, Element *const el);
    Element *const LinkMessage_ElementAt(LinkMessage *const me, uint8_t index);
    // "Basic" LinkMessage END

    // "AbstractUpClass" UplinkMessage
    typedef struct
    {
        LinkMessage super;
        UplinkMessageHead messageHead;
    } UplinkMessage;

    /* UplinkMessage Construtor & Destrucor */
    void UplinkMessage_ctor(UplinkMessage *const me, uint16_t initElementCount);
    void UplinkMessage_dtor(Package *const me);
    /* Public methods */
    bool UplinkMessage_EncodeHead(UplinkMessage const *const me, ByteBuffer *const byteBuff);
    // void UplinkMessage_EncodeTail(UplinkMessage const *const me, ByteBuffer* byteBuff, size_t len);
    bool UplinkMessage_DecodeHead(UplinkMessage *const me, ByteBuffer *const byteBuff);
    // void UplinkMessage_DecodeTail(UplinkMessage * const me, ByteBuffer* byteBuff, size_t len);
    // "AbstractUpClass" UplinkMessage END

    // "AbstractUpClass" DownlinkMessage
    typedef struct
    {
        LinkMessage super;
        DownlinkMessageHead messageHead;
    } DownlinkMessage;

    /* DownlinkMessage Construtor  & Destrucor */
    void DownlinkMessage_ctor(DownlinkMessage *const me, uint16_t initElementCount);
    void DownlinkMessage_dtor(Package *const me);
    /* Public methods */
    bool DownlinkMessage_EncodeHead(DownlinkMessage const *const me, ByteBuffer *const byteBuff);
    // void DownlinkMessage_EncodeTail(DownlinkMessage const *const me, ByteBuffer* byteBuff, size_t len);
    bool DownlinkMessage_DecodeHead(DownlinkMessage *const me, ByteBuffer *const byteBuff);
    // void DownlinkMessage_DecodeTail(DownlinkMessage * const me, ByteBuffer* byteBuff, size_t len);
    // "AbstractUpClass" DownlinkMessage END

    // Elements
    // Element Class
    // PictureElement
    typedef struct
    {
        Element super;
        ByteBuffer *buff;
    } PictureElement;

    void PictureElement_ctor(PictureElement *const me);
    void PictureElement_dtor(Element *const me);
    // PictureElement END

    // DRP5MINElement
    typedef struct
    {
        Element super;
        ByteBuffer *buff;
    } DRP5MINElement;

    void DRP5MINElement_ctor(DRP5MINElement *const me);
    void DRP5MINElement_dtor(Element *const me);
    uint8_t DRP5MINElement_ValueAt(DRP5MINElement *const me, uint8_t index, float *val);

#define DRP5MIN_LEN 12
#define DRP5MIN_DATADEF 0x60
    // DRP5MINElement END

    // FlowRateDataElement
    typedef struct
    {
        Element super;
        ByteBuffer *buff;
    } FlowRateDataElement;

    void FlowRateDataElement_ctor(FlowRateDataElement *const me);
    void FlowRateDataElement_dtor(Element *const me);
#define FLOW_RATE_DATA_DATADEF 0xF6
    // FlowRateDataElement END

    // ArtificialElement
    typedef struct
    {
        Element super;
        ByteBuffer *buff;
    } ArtificialElement;

    void ArtificialElement_ctor(ArtificialElement *const me);
    void ArtificialElement_dtor(Element *const me);
    // ArtificialElement END

    // RelativeWaterLevelElement
    typedef struct
    {
        Element super;
        ByteBuffer *buff;
    } RelativeWaterLevelElement;

    void RelativeWaterLevelElement_ctor(RelativeWaterLevelElement *const me, uint8_t identifierLeader);
    void RelativeWaterLevelElement_dtor(Element *const me);
    uint8_t RelativeWaterLevelElement_ValueAt(RelativeWaterLevelElement *const me, uint8_t index, float *val);

#define RELATIVE_WATER_LEVEL_LEN 24
#define RELATIVE_WATER_LEVEL_5MIN1_DATADEF 0xC0
    // RelativeWaterLevelElement END

    // StationStatusElement
    typedef struct
    {
        // it is fixed value, 0x0418
        Element super;
        uint32_t status;
    } StationStatusElement;

    void StationStatusElement_ctor(StationStatusElement *const me);
    void StationStatusElement_dtor(Element *me);
    uint8_t StationStatusElement_StatusAt(StationStatusElement const *const me, uint8_t index);
#define STATION_STATUS_LEN 4
#define STATION_STATUS_DATADEF 0x20
    // StationStatusElement END

    // DurationElement
    typedef struct
    {
        // it is fixed value, 0x05 ?
        Element super;
        uint8_t hour;
        uint8_t minute;
    } DurationElement;

    void DurationElement_ctor(DurationElement *const me);
    void DurationElement_dtor(Element *me);
#define DURATION_OF_XX_LEN 5        // 5 字节 ASCII OR 1BCD + . + 1BCD = 3?
#define DURATION_OF_XX_DATADEF 0x28 // 同上  0x18?
    // StationStatusElement END

    // All NumberElement
    typedef struct
    {
        Element super;
        ByteBuffer *buff;
    } NumberElement;

    void NumberElement_ctor(NumberElement *const me, uint8_t identifierLeader, uint8_t dataDef);
    void NumberElement_dtor(Element *const me);
    uint8_t NumberElement_SetFloat(NumberElement *const me, float val);
    uint8_t NumberElement_SetInteger(NumberElement *const me, uint64_t val);
    uint8_t NumberElement_GetFloat(NumberElement *const me, float *val);
    uint8_t NumberElement_GetInteger(NumberElement *const me, uint64_t *val);
    // All NumberElement END

    // TimeStepCodeElement
    typedef struct
    {
        // 3字节BCD码
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
    } TimeStepCode;

    typedef struct
    {
        Element super;
        uint8_t count;
        ByteBuffer *buff;
    } NumberListElement;

    void NumberListElement_ctor(NumberListElement *const me, uint8_t identifierLeader, uint8_t dataDef);
    void NumberListElement_dtor(Element *const me);
    uint8_t NumberListElement_GetFloatAt(NumberListElement *const me, uint8_t index, float *val);
    uint8_t NumberListElement_GetIntegerAt(NumberListElement *const me, uint8_t index, uint64_t *val);

    typedef struct
    {
        // it is fixed value, 0x0418
        Element super;
        TimeStepCode timeStepCode;
        NumberListElement numberListElement;
    } TimeStepCodeElement;

    void TimeStepCodeElement_ctor(TimeStepCodeElement *const me);
    void TimeStepCodeElement_dtor(Element *me);
#define TIME_STEP_CODE_LEN 3
#define TIME_STEP_CODE_DATADEF 0x18
    // TimeStepCodeElement END

    typedef struct
    {
        Element super;
        uint8_t extIdentifier;
    } ExtendElement;

    typedef struct
    {
        NumberElement super;
        uint8_t extIdentifier;
    } ExtendNumberElement;

    // Elements END

    // Decode & Encode
    // Util Functions
    static bool inline isNumberElement(uint8_t identifierLeader)
    {
        return identifierLeader >= 0x01 && identifierLeader <= 0x75 &&
               identifierLeader != TIME_STEP_CODE &&
               identifierLeader != STATION_STATUS &&
               identifierLeader != DURATION_OF_XX;
    }

    /**
     * @description: Decode an Element from ByteBuffer.
     * @param {ByteBuffer *const} byteBuff
     *        ByteBuffer should flip to read mode.
     * @return: An Instance of Element.
     */
    Element *decodeElement(ByteBuffer *const byteBuff, Direction direction);

    /**
     * @description: Decode a Package from ByteBuffer
     * @param {ByteBuffer *const} byteBuff
     *        ByteBuffer should flip to read mode.
     * @return: An Instance of Package: DownlinkMessage Or UplinkMessage
     */
    Package *decodePackage(ByteBuffer *const byteBuff);
// Decode & Encode END
// #pragma pack()
#ifdef __cplusplus
}
#endif
#endif
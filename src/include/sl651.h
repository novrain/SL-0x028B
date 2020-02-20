#ifndef H_SL651
#define H_SL651

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define SIMPLE_DTOR(ptr_) \
    {                     \
        (free(ptr_));     \
        (ptr_) = NULL;    \
    }

/**
 *  报文帧控制字符定义 
 */
//SOH(start of header)
#define SOH_ASCII 0x01 //ASCII字符编码报文帧起始
#define SOH_HEX 0x7E7E //HEX/BCD编码报文帧起始
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

    enum StationCategory_HEX
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

    enum FunctionCode
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
    };

    // Package Struct
    // #pragma pack(1)
    /* 遥测站地址 */
    typedef struct
    {
        uint8_t A5;
        uint8_t A4;
        uint8_t A3;
        uint8_t A2;
        uint8_t A1;
        uint8_t A0;
    } RemoteStationAddr;

    /* 中心站地址/遥测站地址组合 */
    typedef struct
    {
        uint8_t centerAddr;
        RemoteStationAddr stationAddr;
    } UpAddr;

    typedef struct
    {
        uint8_t centerAddr;
        RemoteStationAddr stationAddr;
    } DownAddr;

    typedef union {
        UpAddr upAddr;
        DownAddr downAddr;
    } AddrPair;

    typedef enum
    {
        Up,
        Down
    } Direction;

    typedef struct
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } DateTime;

    typedef struct
    {
        uint16_t count;
        uint8_t seq;
    } Sequence;

    /* Head */
    typedef struct
    {
        Direction direction;
        uint16_t soh;
        AddrPair addrPair;
        uint16_t pwd;
        uint8_t funcCode;
        uint16_t len;
        uint8_t stxFlag;
        // if stxFlag == SNY
        Sequence seq;
    } Head;

#define Head_ctor(ptr_)
#define Head_dtor(ptr_) (SIMPLE_DTOR(ptr_))

#define PACKAGE_HEAD_STX_LEN 14
#define PACKAGE_HEAD_SNY_LEN 17

    typedef struct
    {
        uint16_t seq;
        DateTime sendTime;
        uint8_t addrFlag[2]; // it is fixed value now, 0xF1F1
        RemoteStationAddr stationAddr;
        uint8_t stationCategory;
        uint8_t observeTimeFlag[2]; // it is fixed value now, 0xF0F0
        DateTime observeTime;
    } UplinkMessageHead;

    typedef struct
    {
        uint16_t seq;
        DateTime sendTime;
        uint8_t addrFlag[2]; // it is fixed value now, 0xF1F1
        RemoteStationAddr stationAddr;
    } DownlinkMessageHead;

    typedef struct
    {
        uint8_t etxFlag;
        uint16_t crc;
    } Tail;

    // oop
    // "AbstractClass" Package
    struct PackageVtbl; /* forward declaration */
    typedef struct
    {
        struct PackageVtbl const *vptr; /* <== Package's Virtual Pointer */
        Head *head;
        Tail *tail;
    } Package;

    /* Package's virtual table */
    typedef struct PackageVtbl
    {
        // pure virtual
        void (*encode2Hex)(Package const *const me, uint8_t *hex, size_t len);
        void (*decodeFromHex)(Package const *const me, const uint8_t *hex, size_t len);
        size_t (*size)();
    } PackageVtbl;

    /* Package Construtor & Destrucor */
    void Package_ctor(Package *const me, Head *head);
    /* Public methods */
    void Package_Head2Hex(Package const *const me, uint8_t *hex, size_t len);
    void Package_Tail2Hex(Package const *const me, uint8_t *hex, size_t len);
    void Package_Hex2Head(Package const *me, uint8_t *hex, size_t len);
    void Package_Hex2Tail(Package const *me, uint8_t *hex, size_t len);
    /* Public Helper*/
#define PACAKAGE_UPCAST(ptr_) ((Package *)(ptr_))
#define Package_dtor(ptr_) (SIMPLE_DTOR(ptr_))
#define Package_Direction(me_) (PACAKAGE_UPCAST(me_)->head->direction)
    // "AbstractClass" Package END

    // "Interface" Element
    struct ElementVtbl; /* forward declaration */
    typedef struct
    {
        struct ElementVtbl const *vptr;
    } Element;

    typedef struct ElementVtbl
    {
        // pure virtual
        void (*encode2Hex)(Package const *const me, uint8_t *hex, size_t len);
        void (*decodeFromHex)(Package const *const me, const uint8_t *hex, size_t len);
        size_t (*size)();
    } ElementVtbl;
    // "Interface" Element END

    // "Basic" LinkMessage
    typedef struct
    {
        Package super;
        uint16_t elementCount;
        Element *elements[0];
        Element *elementCursor;
    } LinkMessage;

    /* LinkMessage Construtor & Destrucor */
    void LinkMessage_ctor(LinkMessage *const me, Head *head, uint16_t elementCount);
    void LinkMessage_dtor(LinkMessage *const me);
    /* Public methods */
    void LinkMessage_Elements2Hex(LinkMessage const *const me, uint8_t *hex, size_t len);
    void LinkMessage_Hex2Elements(LinkMessage const *me, uint8_t *hex, size_t len);
    void LinkMessage_putElement(LinkMessage const *me, uint16_t index, Element *element);
    void LinkMessage_setElement(LinkMessage const *me, uint16_t index, Element *element);
    Element *LinkMessage_getElement(LinkMessage const *me, uint16_t index);
    /* Public Helper*/
#define LINKMESSAGE_UPCAST(ptr_) ((Package *)(ptr_))
#define LinkMessage_ElementCount(me_) (LINKMESSAGE_UPCAST(me_)->elementCount)
#define LinkMessage_rewind(me_) (LINKMESSAGE_UPCAST(me_)->elementCursor = 0)
    // "Basic" LinkMessage END

    // "AbstractUpClass" UplinkMessage
    typedef struct
    {
        LinkMessage super;
        UplinkMessageHead *messageHead;
    } UplinkMessage;

    /* UplinkMessage Construtor & Destrucor */
    void UplinkMessage_ctor(UplinkMessage *const me, Head *head, UplinkMessageHead *upLinkHead, uint16_t elementCount);
    void UplinkMessage_dtor(UplinkMessage *const me);
    /* Public methods */
    void UplinkMessage_Head2Hex(UplinkMessage const *const me, uint8_t *hex, size_t len);
    // void UplinkMessage_Tail2Hex(UplinkMessage const *const me, uint8_t *hex, size_t len);
    void UplinkMessage_Hex2Head(UplinkMessage const *me, uint8_t *hex, size_t len);
    // void UplinkMessage_Hex2Tail(UplinkMessage const *me, uint8_t *hex, size_t len);
    // "AbstractUpClass" UplinkMessage END

    // "AbstractUpClass" DownlinkMessage
    typedef struct
    {
        LinkMessage super;
        DownlinkMessageHead *messageHead;
    } DownlinkMessage;

    /* DownlinkMessage Construtor  & Destrucor */
    void DownlinkMessage_ctor(DownlinkMessage *const me, Head *head, DownlinkMessageHead *downLinkHead, uint16_t elementCount);
    void DownlinkMessage_dtor(DownlinkMessage *const me);
    /* Public methods */
    void DownlinkMessage_Head2Hex(DownlinkMessage const *const me, uint8_t *hex, size_t len);
    // void DownlinkMessage_Tail2Hex(DownlinkMessage const *const me, uint8_t *hex, size_t len);
    void DownlinkMessage_Hex2Head(DownlinkMessage const *me, uint8_t *hex, size_t len);
    // void DownlinkMessage_Hex2Tail(DownlinkMessage const *me, uint8_t *hex, size_t len);
    // "AbstractUpClass" DownlinkMessage END

    // Elements

    // Elements END
    // #pragma pack()
#ifdef __cplusplus
}
#endif
#endif
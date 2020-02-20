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
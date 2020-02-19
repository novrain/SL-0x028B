#ifndef H_SL651
#define H_SL651

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
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

#define PACKAGE_HEAD_STX_LEN 14
#define PACKAGE_HEAD_SNY_LEN 17

    typedef struct
    {
        uint8_t etxFlag;
        uint16_t crc;
    } Tail;

    typedef struct
    {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t second;
    } DateTime;

    // oop
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
        void (*encode2Hex)(Package const *const me, uint8_t *hex, size_t len);
        void (*decodeFromHex)(Package const *const me, const uint8_t *hex, size_t len);
        size_t (*size)();
    } PackageVtbl;

    /* Package Construtor */
    void Package_ctor(Package *const me, Head head);
    /* Public methods */
    void Package_Head2Hex(Package const *const me, uint8_t *hex, size_t len);
    void Package_Tail2Hex(Package const *const me, uint8_t *hex, size_t len);
    void Package_Hex2Head(Package const *me, uint8_t *hex, size_t len);
    void Package_Hex2Tail(Package const *me, uint8_t *hex, size_t len);
    /* Public Helper*/
    // "AbstractClass" Package END
#define PACAKAGE_UPCAST(ptr_) ((Package *)(ptr_))
#define Package_Direction(me_) (PACAKAGE_UPCAST(me_)->head.direction)

    // #pragma pack()
#ifdef __cplusplus
}
#endif
#endif
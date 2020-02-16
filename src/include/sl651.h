#ifndef H_SL651
#define H_SL651

#ifdef __cplusplus
extern "C"
{
#endif
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
#pragma pack(1) //便于内存拷贝，但降低效率
    /**
 * 遥测站地址
 */
    typedef struct
    {
        unsigned char A5;
        unsigned char A4;
        unsigned char A3;
        unsigned char A2;
        unsigned char A1;
        unsigned char A0;
    } RemoteStationAddr;

    /**
 * 中心站地址/遥测站地址组合
 */
    typedef struct
    {
        unsigned char centerAddr;
        RemoteStationAddr stationAddr;
    } UpAddr;

    typedef struct
    {
        unsigned char centerAddr;
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
        unsigned short int count;
        unsigned char seq;
    } Sequence;

    /**
 * Head
 */
    typedef struct
    {
        Direction direction;
        unsigned short int soh;
        AddrPair addrPair;
        unsigned short int pwd;
        unsigned char funcCode;
        unsigned short int len;
        unsigned char stxFlag;
        // if stxFlag == SNY
        Sequence seq;
    } PkgHead;

#define PACKAGE_HEAD_STX_LEN 14
#define PACKAGE_HEAD_SNY_LEN 17

    typedef struct
    {
        unsigned char etxFlag;
        unsigned short int crc;
    } PkgTail;

    typedef struct
    {
        unsigned short int len;
        PkgHead head;
        char *data;
        PkgTail tail;
    } Package;
#pragma pack()

#ifdef __cplusplus
}
#endif
#endif
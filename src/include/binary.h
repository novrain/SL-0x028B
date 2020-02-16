#ifndef H_SL651_BINARY
#define H_SL651_BINARY

#ifdef __cplusplus
extern "C"
{
#endif

    static unsigned short ENDIAN_MAGIC = 0xFEEF;
    static unsigned char ENDIAN_MAGIC_HIGH_BYTE = 0xFE;
    static unsigned char ENDIAN_MAGIC_LOW_BYTE = 0xEF;

    typedef enum
    {
        LittleEndian,
        BigEndian
    } ByteOrder;

    ByteOrder hostEndian();

    // LittleEndian

    // read
    unsigned short toLeUInt16(unsigned char *pBytes);
    unsigned long toLeUInt32(unsigned char *pBytes);
    unsigned long long toLeUInt64(unsigned char *pBytes);

    // write
    void putLeUInt16(unsigned char *pBytes, unsigned short v);
    void putLeUInt32(unsigned char *pBytes, unsigned long v);
    void putLeUInt64(unsigned char *pBytes, unsigned long long v);

    // BigEndian

    // read
    unsigned short toBeUInt16(unsigned char *pBytes);
    unsigned long toBeUInt32(unsigned char *pBytes);
    unsigned long long toBeUInt64(unsigned char *pBytes);

    // write
    void putBeUInt16(unsigned char *pBytes, unsigned short v);
    void putBeUInt32(unsigned char *pBytes, unsigned long v);
    void putBeUInt64(unsigned char *pBytes, unsigned long long v);

#ifdef __cplusplus
}
#endif

#endif
#ifndef H_SL651_BINARY
#define H_SL651_BINARY

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

    static uint16_t ENDIAN_MAGIC = 0xFEEF;
    static uint8_t ENDIAN_MAGIC_HIGH_BYTE = 0xFE;
    static uint8_t ENDIAN_MAGIC_LOW_BYTE = 0xEF;

    typedef enum
    {
        LittleEndian,
        BigEndian
    } ByteOrder;

    ByteOrder hostEndian();

    // LittleEndian

    // read
    uint16_t toLeUInt16(uint8_t *pBytes);
    uint32_t toLeUInt32(uint8_t *pBytes);
    uint64_t toLeUInt64(uint8_t *pBytes);

    // write
    void putLeUInt16(uint8_t *pBytes, uint16_t v);
    void putLeUInt32(uint8_t *pBytes, uint32_t v);
    void putLeUInt64(uint8_t *pBytes, uint64_t v);

    // BigEndian

    // read
    uint16_t toBeUInt16(uint8_t *pBytes);
    uint32_t toBeUInt32(uint8_t *pBytes);
    uint64_t toBeUInt64(uint8_t *pBytes);

    // write
    void putBeUInt16(uint8_t *pBytes, uint16_t v);
    void putBeUInt32(uint8_t *pBytes, uint32_t v);
    void putBeUInt64(uint8_t *pBytes, uint64_t v);

#ifdef __cplusplus
}
#endif

#endif
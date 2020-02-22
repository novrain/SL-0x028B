#ifndef H_SL651_HEX
#define H_SL651_HEX

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

    size_t bin2hex(const uint8_t *bin, size_t binLen, uint8_t *hex, size_t hexLen);
    size_t hex2bin(const uint8_t *hex, size_t hexLen, uint8_t *bin, size_t binLen);

    bool hexToUInt8(const uint8_t *hex, uint8_t *val);
    bool hexToBeUInt16(const uint8_t *hex, uint16_t *val);
    bool hexToBeUInt32(const uint8_t *hex, uint32_t *val);
    bool hexToBeUInt64(const uint8_t *hex, uint64_t *val);

    bool hexToLeUInt16(const uint8_t *hex, uint16_t *val);
    bool hexToLeUInt32(const uint8_t *hex, uint32_t *val);
    bool hexToLeUInt64(const uint8_t *hex, uint64_t *val);

    size_t hexPutUInt8(uint8_t *hex, uint8_t *val);
    size_t hexPutBeUInt16(uint8_t *hex, uint16_t *val);
    size_t hexPutBeUInt32(uint8_t *hex, uint32_t *val);
    size_t hexPutBeUInt64(uint8_t *hex, uint64_t *val);

    size_t hexPutLeUInt16(uint8_t *hex, uint16_t *val);
    size_t hexPutLeUInt32(uint8_t *hex, uint32_t *val);
    size_t hexPutLeUInt64(uint8_t *hex, uint64_t *val);

#ifdef __cplusplus
}
#endif

#endif
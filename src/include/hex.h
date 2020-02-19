#ifndef H_SL651_HEX
#define H_SL651_HEX

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

    size_t bin2hex(const uint8_t *bin, size_t binLen, uint8_t *hex, size_t hexLen);
    size_t hex2bin(const uint8_t *hex, size_t hexLen, uint8_t *bin, size_t binLen);

#ifdef __cplusplus
}
#endif

#endif
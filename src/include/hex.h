#ifndef H_SL651_HEX
#define H_SL651_HEX

#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

    size_t bin2hex(const unsigned char *bin, size_t binLen, char *hex, size_t hexLen);
    size_t hex2bin(const char *hex, size_t hexLen, unsigned char *bin, size_t binLen);

#ifdef __cplusplus
}
#endif

#endif
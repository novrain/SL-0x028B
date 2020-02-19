#include "hex.h"

static char hexChar[16 + 1] = "0123456789ABCDEF";

size_t bin2hex(const uint8_t *bin, size_t binLen, char *hex, size_t hexLen)
{
    if (bin == NULL || binLen == 0 || hex == NULL || hexLen == 0)
    {
        return 0;
    }

    size_t lenWritten = 0;
    size_t i = 0;
    while (i < binLen && (i * 2 + (2 + 1)) <= hexLen)
    {
        uint8_t hB = (bin[i] & 0xF0) >> 4;
        *hex = hexChar[hB];
        hex++;

        uint8_t lB = bin[i] & 0x0F;
        *hex = hexChar[lB];
        hex++;

        lenWritten += 2;
        i++;
    }
    *hex = '\0';

    return lenWritten;
}

static int hexChar2bin(const char hex, uint8_t *byte)
{
    if (byte == NULL)
    {
        return 0;
    }

    if (hex >= '0' && hex <= '9')
    {
        *byte = hex - '0';
    }
    else if (hex >= 'A' && hex <= 'F')
    {
        *byte = hex - 'A' + 10;
    }
    else if (hex >= 'a' && hex <= 'f')
    {
        *byte = hex - 'a' + 10;
    }
    else
    {
        return 0;
    }

    return 1;
}

size_t hex2bin(const char *hex, size_t hexLen, uint8_t *bin, size_t binLen)
{
    if (bin == NULL || binLen == 0 || hex == NULL || hexLen == 0)
    {
        return 0;
    }

    size_t len = hexLen / 2;
    len = MIN(len, binLen);
    uint8_t b1;
    uint8_t b2;
    for (size_t i = 0; i < len; i++)
    {
        if (!hexChar2bin(hex[i * 2], &b1) || !hexChar2bin(hex[i * 2 + 1], &b2))
        {
            return 0;
        }
        bin[i] = (b1 << 4) | b2;
    }
    return len;
}

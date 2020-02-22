#include "hex.h"

static char hexChar[16 + 1] = "0123456789ABCDEF";

size_t bin2hex(const uint8_t *bin, size_t binLen, uint8_t *hex, size_t hexLen)
{
    if (bin == NULL || binLen == 0 || hex == NULL || hexLen == 0)
    {
        return 0;
    }

    size_t lenWritten = 0;
    size_t i = 0;
    while (i < binLen && (i * 2 + 2) <= hexLen)
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
    // *hex = '\0';

    return lenWritten;
}

static size_t hexToBeUInt(const uint8_t *hex, void *val, const size_t size)
{
    uint8_t count = 0;
    while (*hex && count < size * 2)
    {
        // get current character then increment
        uint8_t byte = *hex++;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else if (byte >= 'a' && byte <= 'f')
        {
            byte = byte - 'a' + 10;
        }
        else if (byte >= 'A' && byte <= 'F')
        {
            byte = byte - 'A' + 10;
        }
        else
        {
            return count;
        }
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        if (size <= 1)
        {
            uint8_t *p = (uint8_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        else if (size <= 2)
        {
            uint16_t *p = (uint16_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        else if (size <= 4)
        {
            uint32_t *p = (uint32_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        else
        {
            uint64_t *p = (uint64_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        count++;
    }
    return count;
}

bool hexToUInt8(const uint8_t *hex, uint8_t *val)
{
    *val = 0;
    return hexToBeUInt(hex, (void *)val, 1) == 2; // 1 byte 2 hex char
}

bool hexToBeUInt16(const uint8_t *hex, uint16_t *val)
{
    *val = 0;
    return hexToBeUInt(hex, (void *)val, 2) == 4; // 2 byte 4 hex char
}

bool hexToBeUInt32(const uint8_t *hex, uint32_t *val)
{
    *val = 0;
    return hexToBeUInt(hex, (void *)val, 4) == 8; // 4 byte 8 hex char
}

bool hexToBeUInt64(const uint8_t *hex, uint64_t *val)
{
    *val = 0;
    return hexToBeUInt(hex, (void *)val, 8) == 16; // 4 byte 8 hex char
}

static size_t hexToLeUInt(const uint8_t *hex, void *val, const size_t size)
{
    uint8_t count = 0;
    const uint8_t *oneByteCursor = hex + size * 2 - 2;
    while (*oneByteCursor && count < size * 2)
    {
        // get current character then increment
        uint8_t byte = 0;
        byte = *oneByteCursor;
        // transform hex character to the 4bit equivalent number, using the ascii table indexes
        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else if (byte >= 'a' && byte <= 'f')
        {
            byte = byte - 'a' + 10;
        }
        else if (byte >= 'A' && byte <= 'F')
        {
            byte = byte - 'A' + 10;
        }
        else
        {
            return count;
        }
        // shift 4 to make space for new digit, and add the 4 bits of the new digit
        if (size <= 1)
        {
            uint8_t *p = (uint8_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        else if (size <= 2)
        {
            uint16_t *p = (uint16_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        else if (size <= 4)
        {
            uint32_t *p = (uint32_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        else
        {
            uint64_t *p = (uint64_t *)val;
            *(p) = (*p << 4) | (byte & 0xF);
        }
        count++;

        if (count & 1 == 1)
        {
            oneByteCursor++;
        }
        else
        {
            oneByteCursor -= 3;
        }
    }
    return count;
}

bool hexToLeUInt16(const uint8_t *hex, uint16_t *val)
{
    *val = 0;
    return hexToLeUInt(hex, (void *)val, 2) == 4; // 2 byte 4 hex char
}

bool hexToLeUInt32(const uint8_t *hex, uint32_t *val)
{
    *val = 0;
    return hexToLeUInt(hex, (void *)val, 4) == 8; // 4 byte 8 hex char
}

bool hexToLeUInt64(const uint8_t *hex, uint64_t *val)
{
    *val = 0;
    return hexToLeUInt(hex, (void *)val, 8) == 16; // 4 byte 8 hex char
}

size_t hex2bin(const uint8_t *hex, size_t hexLen, uint8_t *bin, size_t binLen)
{
    if (bin == NULL || binLen == 0 || hex == NULL || hexLen == 0)
    {
        return 0;
    }

    size_t len = hexLen / 2;
    len = MIN(len, binLen);
    uint8_t b;
    for (size_t i = 0; i < len; i++)
    {
        if (!hexToUInt8(hex + i * 2, &b))
        {
            return 0;
        }
        bin[i] = b;
    }
    return len;
}

size_t hexPutUInt8(uint8_t *hex, uint8_t *val)
{
    uint8_t hB = (*val & 0xF0) >> 4;
    *hex = hexChar[hB];
    hex++;
    uint8_t lB = *val & 0x0F;
    *hex = hexChar[lB];
    return 2;
}

size_t hexPutBeUInt16(uint8_t *hex, uint16_t *val)
{
    uint8_t b = *val >> 8;
    hex += hexPutUInt8(hex, &b);
    b = *val;
    hexPutUInt8(hex, &b);
    return 4;
}

size_t hexPutBeUInt32(uint8_t *hex, uint32_t *val)
{
    uint8_t b = *val >> 24;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 16;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 8;
    hex += hexPutUInt8(hex, &b);
    b = *val;
    hex += hexPutUInt8(hex, &b);
    return 8;
}

size_t hexPutBeUInt64(uint8_t *hex, uint64_t *val)
{
    uint8_t b = *val >> 56;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 48;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 40;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 32;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 24;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 16;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 8;
    hex += hexPutUInt8(hex, &b);
    b = *val;
    hexPutUInt8(hex, &b);
    return 16;
}

size_t hexPutLeUInt16(uint8_t *hex, uint16_t *val)
{
    uint8_t b = *val;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 8;
    hexPutUInt8(hex, &b);
    return 4;
}

size_t hexPutLeUInt32(uint8_t *hex, uint32_t *val)
{
    uint8_t b = *val;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 8;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 16;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 24;
    hex += hexPutUInt8(hex, &b);
    return 8;
}

size_t hexPutLeUInt64(uint8_t *hex, uint64_t *val)
{
    uint8_t b = *val;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 8;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 16;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 24;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 32;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 40;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 48;
    hex += hexPutUInt8(hex, &b);
    b = *val >> 56;
    hexPutUInt8(hex, &b);
    return 16;
}
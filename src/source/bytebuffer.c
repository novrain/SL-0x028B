#include "bytebuffer.h"

static size_t binToBeUInt(const uint8_t *bin, void *val, const size_t size)
{
    uint8_t count = 0;
    while (*bin && count < size)
    {
        uint8_t byte = *bin++;
        if (size <= 1)
        {
            uint8_t *p = (uint8_t *)val;
            *(p) = (*p << 8) | byte;
        }
        else if (size <= 2)
        {
            uint16_t *p = (uint16_t *)val;
            *(p) = (*p << 8) | byte;
        }
        else if (size <= 4)
        {
            uint32_t *p = (uint32_t *)val;
            *(p) = (*p << 8) | byte;
        }
        else
        {
            uint64_t *p = (uint64_t *)val;
            *(p) = (*p << 8) | byte;
        }
        count++;
    }
    return count;
}

static size_t binToLeUInt(const uint8_t *bin, void *val, const size_t size)
{
    uint8_t count = size;
    bin += size;
    while (*bin && count > 0)
    {
        uint8_t byte = *bin--;
        if (size <= 1)
        {
            uint8_t *p = (uint8_t *)val;
            *(p) = (*p << 8) | byte;
        }
        else if (size <= 2)
        {
            uint16_t *p = (uint16_t *)val;
            *(p) = (*p << 8) | byte;
        }
        else if (size <= 4)
        {
            uint32_t *p = (uint32_t *)val;
            *(p) = (*p << 8) | byte;
        }
        else
        {
            uint64_t *p = (uint64_t *)val;
            *(p) = (*p << 8) | byte;
        }
        count--;
    }
    return count;
}

static char hexChar[16 + 1] = "0123456789ABCDEF";

static size_t hexToBeUInt(const uint8_t *hex, void *val, const size_t size)
{
    uint8_t count = 0;
    while (*hex && count < size * 2)
    {
        uint8_t byte = *hex++;
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

static size_t hexToBCDUInt(const uint8_t *hex, void *val, const size_t size)
{
    uint8_t count = 0;
    while (*hex && count < size * 2)
    {
        uint8_t byte = *hex++;
        if (byte >= '0' && byte <= '9')
        {
            byte = byte - '0';
        }
        else
        {
            return count;
        }
        if (size <= 1)
        {
            uint8_t *p = (uint8_t *)val;
            *(p) = (*p * 10) + (byte & 0xF);
        }
        else if (size <= 2)
        {
            uint16_t *p = (uint16_t *)val;
            *(p) = (*p * 10) + (byte & 0xF);
        }
        else if (size <= 4)
        {
            uint32_t *p = (uint32_t *)val;
            *(p) = (*p * 10) + (byte & 0xF);
        }
        else
        {
            uint64_t *p = (uint64_t *)val;
            *(p) = (*p * 10) + (byte & 0xF);
        }
        count++;
    }
    return count;
}

static size_t hexToLeUInt(const uint8_t *hex, void *val, const size_t size)
{
    uint8_t count = 0;
    const uint8_t *oneByteCursor = hex + size * 2 - 2;
    while (*oneByteCursor && count < size * 2)
    {
        uint8_t byte = 0;
        byte = *oneByteCursor;
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

void ByteBuffer_ctor(ByteBuffer *const me, uint32_t size)
{
    if (me == NULL)
    {
        return;
    }
    me->size = size;
    me->position = 0;
    me->limit = size;
    me->wrapped = false;
    me->buff = (uint8_t *)malloc(size);
    memset(me->buff, 0, size);
}

void ByteBuffer_ctor_wrapped(ByteBuffer *const me, uint8_t *buff, uint32_t size)
{
    if (me == NULL || buff == NULL)
    {
        return;
    }
    me->size = size;
    me->position = size;
    me->limit = size;
    me->wrapped = true;
    me->buff = buff;
}

void ByteBuffer_ctor_copy(ByteBuffer *const me, uint8_t *buff, uint32_t size)
{
    if (me == NULL || buff == NULL)
    {
        return;
    }
    me->size = size;
    me->position = size;
    me->limit = size;
    me->wrapped = false;
    me->buff = (uint8_t *)malloc(size);
    memcpy(me->buff, buff, size);
}

void ByteBuffer_dtor(ByteBuffer *const me)
{
    if (me == NULL)
    {
        return;
    }
    if (!me->wrapped)
    {
        free(me->buff);
    }
}

void ByteBuffer_Flip(ByteBuffer *const me)
{
    if (me == NULL)
    {
        return;
    }
    me->limit = me->position;
    me->position = 0;
}

void ByteBuffer_Clear(ByteBuffer *const me)
{
    if (me == NULL || me->wrapped)
    {
        return;
    }
    memset(me->buff, 0, me->size);
    me->position = 0;
    me->limit = me->size;
}

void ByteBuffer_Rewind(ByteBuffer *const me)
{
    me->position = 0;
}

ByteBuffer *ByteBuffer_GetByteBuffer(ByteBuffer *const me, uint32_t size)
{
    ByteBuffer *val = ByteBuffer_PeekByteBuffer(me, size);
    if (val != NULL)
    {
        me->position += size;
    }
    return val;
}

ByteBuffer *ByteBuffer_PeekByteBuffer(ByteBuffer *const me, uint32_t size)
{
    if (me == NULL || me->position + size > me->limit)
    {
        return NULL;
    }
    ByteBuffer *val = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(val, me->buff + me->position, size);
    return val;
}

uint8_t ByteBuffer_PeekUInt8(ByteBuffer *const me, uint8_t *val)
{
    if (me == NULL || me->position >= me->limit)
    {
        return 0;
    }
    *val = me->buff[me->position];
    return 1;
}

uint8_t ByteBuffer_BE_GetUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    if (me == NULL || me->position + size - 1 >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = binToBeUInt(me->buff + me->position, val, size);
    if (usedLen == size)
    {
        me->position += usedLen;
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_LE_GetUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    if (me == NULL || me->position + size - 1 >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = binToLeUInt(me->buff + me->position, val, size);
    if (usedLen == size)
    {
        me->position += usedLen;
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_GetUInt8(ByteBuffer *const me, uint8_t *val)
{
    uint8_t usedLen = ByteBuffer_PeekUInt8(me, val);
    if (usedLen == 1)
    {
        me->position++;
    }
    return usedLen;
}

uint8_t ByteBuffer_PutUInt8(ByteBuffer *const me, uint8_t val)
{
    if (me == NULL || me->wrapped || me->position >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val;
    return 1;
}

uint8_t ByteBuffer_BE_GetUInt16(ByteBuffer *const me, uint16_t *val)
{
    return ByteBuffer_BE_GetUInt(me, val, 2);
}

uint8_t ByteBuffer_BE_GetUInt32(ByteBuffer *const me, uint32_t *val)
{
    return ByteBuffer_BE_GetUInt(me, val, 4);
}

uint8_t ByteBuffer_BE_GetUInt64(ByteBuffer *const me, uint64_t *val)
{
    return ByteBuffer_BE_GetUInt(me, val, 8);
}

uint8_t ByteBuffer_LE_GetUInt16(ByteBuffer *const me, uint16_t *val)
{
    return ByteBuffer_LE_GetUInt(me, val, 2);
}

uint8_t ByteBuffer_LE_GetUInt32(ByteBuffer *const me, uint32_t *val)
{
    return ByteBuffer_LE_GetUInt(me, val, 4);
}

uint8_t ByteBuffer_LE_GetUInt64(ByteBuffer *const me, uint64_t *val)
{
    return ByteBuffer_LE_GetUInt(me, val, 8);
}

uint8_t ByteBuffer_BE_HEXGetUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    if (me == NULL || me->position + size * 2 - 1 >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = hexToBeUInt(me->buff + me->position, val, size);
    if (usedLen == size * 2)
    {
        me->position += usedLen;
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_LE_HEXGetUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    if (me == NULL || me->position + size * 2 - 1 >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = hexToLeUInt(me->buff + me->position, val, size);
    if (usedLen == size * 2)
    {
        me->position += usedLen;
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_HEXGetUInt8(ByteBuffer *const me, uint8_t *val)
{
    return ByteBuffer_BE_HEXGetUInt(me, val, 1);
}

static void hexPutUInt8(uint8_t *hex, uint8_t val)
{
    hex[0] = hexChar[(val & 0xF0) >> 4];
    hex[1] = hexChar[val & 0x0F];
}

uint8_t ByteBuffer_HEXPutUInt8(ByteBuffer *const me, uint8_t val)
{
    if (me == NULL || me->position + 1 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val);
    me->position += 2;
    return 2;
}

uint8_t ByteBuffer_BE_HEXGetUInt16(ByteBuffer *const me, uint16_t *val)
{
    return ByteBuffer_BE_HEXGetUInt(me, val, 2);
}

uint8_t ByteBuffer_BE_HEXGetUInt32(ByteBuffer *const me, uint32_t *val)
{
    return ByteBuffer_BE_HEXGetUInt(me, val, 4);
}

uint8_t ByteBuffer_BE_HEXGetUInt64(ByteBuffer *const me, uint64_t *val)
{
    return ByteBuffer_BE_HEXGetUInt(me, val, 8);
}

uint8_t ByteBuffer_LE_HEXGetUInt16(ByteBuffer *const me, uint16_t *val)
{
    return ByteBuffer_LE_HEXGetUInt(me, val, 2);
}

uint8_t ByteBuffer_LE_HEXGetUInt32(ByteBuffer *const me, uint32_t *val)
{
    return ByteBuffer_LE_HEXGetUInt(me, val, 4);
}

uint8_t ByteBuffer_LE_HEXGetUInt64(ByteBuffer *const me, uint64_t *val)
{
    return ByteBuffer_LE_HEXGetUInt(me, val, 8);
}

uint8_t ByteBuffer_BE_PutUInt16(ByteBuffer *const me, uint16_t val)
{
    if (me == NULL || me->position + 1 >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val >> 8;
    me->buff[me->position++] = val;
    return 2;
}

uint8_t ByteBuffer_BE_PutUInt32(ByteBuffer *const me, uint32_t val)
{
    if (me == NULL || me->position + 3 >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val >> 24;
    me->buff[me->position++] = val >> 16;
    me->buff[me->position++] = val >> 8;
    me->buff[me->position++] = val;
    return 4;
}

uint8_t ByteBuffer_BE_PutUInt64(ByteBuffer *const me, uint64_t val)
{
    if (me == NULL || me->position + 7 >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val >> 56;
    me->buff[me->position++] = val >> 48;
    me->buff[me->position++] = val >> 40;
    me->buff[me->position++] = val >> 32;
    me->buff[me->position++] = val >> 24;
    me->buff[me->position++] = val >> 16;
    me->buff[me->position++] = val >> 8;
    me->buff[me->position++] = val;
    return 8;
}

uint8_t ByteBuffer_LE_PutUInt16(ByteBuffer *const me, uint16_t val)
{
    if (me == NULL || me->position + 1 >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val;
    me->buff[me->position++] = val >> 8;
    return 2;
}

uint8_t ByteBuffer_LE_PutUInt32(ByteBuffer *const me, uint32_t val)
{
    if (me == NULL || me->position + 3 >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val;
    me->buff[me->position++] = val >> 8;
    me->buff[me->position++] = val >> 16;
    me->buff[me->position++] = val >> 24;
    return 4;
}

uint8_t ByteBuffer_LE_PutUInt64(ByteBuffer *const me, uint64_t val)
{
    if (me == NULL || me->position + 7 >= me->limit)
    {
        return 0;
    }
    me->buff[me->position++] = val;
    me->buff[me->position++] = val >> 8;
    me->buff[me->position++] = val >> 16;
    me->buff[me->position++] = val >> 24;
    me->buff[me->position++] = val >> 32;
    me->buff[me->position++] = val >> 40;
    me->buff[me->position++] = val >> 48;
    me->buff[me->position++] = val >> 56;
    return 8;
}

uint8_t ByteBuffer_BE_HEXPutUInt16(ByteBuffer *const me, uint16_t val)
{
    if (me == NULL || me->position + 3 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val);
    hexPutUInt8(me->buff + me->position + 2, val >> 8);
    me->position + 4;
}

uint8_t ByteBuffer_BE_HEXPutUInt32(ByteBuffer *const me, uint32_t val)
{
    if (me == NULL || me->position + 7 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val);
    hexPutUInt8(me->buff + me->position + 2, val >> 8);
    hexPutUInt8(me->buff + me->position + 4, val >> 16);
    hexPutUInt8(me->buff + me->position + 6, val >> 24);
    me->position + 8;
}

uint8_t ByteBuffer_BE_HEXPutUInt64(ByteBuffer *const me, uint64_t val)
{
    if (me == NULL || me->position + 15 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val);
    hexPutUInt8(me->buff + me->position + 2, val >> 8);
    hexPutUInt8(me->buff + me->position + 4, val >> 16);
    hexPutUInt8(me->buff + me->position + 6, val >> 24);
    hexPutUInt8(me->buff + me->position + 8, val >> 32);
    hexPutUInt8(me->buff + me->position + 10, val >> 40);
    hexPutUInt8(me->buff + me->position + 12, val >> 48);
    hexPutUInt8(me->buff + me->position + 14, val >> 56);
    me->position + 16;
}

uint8_t ByteBuffer_LE_HEXPutUInt16(ByteBuffer *const me, uint16_t val)
{
    if (me == NULL || me->position + 3 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val >> 8);
    hexPutUInt8(me->buff + me->position + 2, val);
    me->position + 4;
}

uint8_t ByteBuffer_LE_HEXPutUInt32(ByteBuffer *const me, uint32_t val)
{
    if (me == NULL || me->position + 7 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val >> 24);
    hexPutUInt8(me->buff + me->position + 2, val >> 16);
    hexPutUInt8(me->buff + me->position + 4, val >> 8);
    hexPutUInt8(me->buff + me->position + 6, val);
    me->position + 8;
}

uint8_t ByteBuffer_LE_HEXPutUInt64(ByteBuffer *const me, uint64_t val)
{
    if (me == NULL || me->position + 15 >= me->limit)
    {
        return 0;
    }
    hexPutUInt8(me->buff + me->position, val >> 56);
    hexPutUInt8(me->buff + me->position + 2, val >> 48);
    hexPutUInt8(me->buff + me->position + 4, val >> 40);
    hexPutUInt8(me->buff + me->position + 6, val >> 32);
    hexPutUInt8(me->buff + me->position + 8, val >> 24);
    hexPutUInt8(me->buff + me->position + 10, val >> 16);
    hexPutUInt8(me->buff + me->position + 12, val >> 8);
    hexPutUInt8(me->buff + me->position + 14, val);
    me->position + 16;
}

uint8_t ByteBuffer_BCDGetUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    if (me == NULL || me->position + size * 2 - 1 >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = hexToBCDUInt(me->buff + me->position, val, size);
    if (usedLen == size * 2)
    {
        me->position += usedLen;
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_BCDGetUInt8(ByteBuffer *const me, uint8_t *val)
{
    return ByteBuffer_BCDGetUInt(me, val, 1);
}
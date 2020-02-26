#include "bytebuffer.h"

static size_t binToBeUInt(const uint8_t *bin, void *val, const size_t size)
{
    uint8_t count = 0;
    // while (*bin && count < size)
    while (count < size)
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
    uint8_t count = 0;
    bin += size - 1;
    // while (*bin && (size - count > 0))
    while (size - count > 0)
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
        count++;
    }
    return count;
}

static size_t binToBCDUInt(const uint8_t *bin, void *val, const size_t size)
{
    uint8_t count = 0;
    // while (*hex && count < size * 2)
    while (count < size)
    {
        uint8_t byte = *bin++;
        uint8_t h = byte >> 4;
        uint8_t l = byte & 0xF;
        if (h > 9 || l > 9)
        {
            return count;
        }
        if (size <= 1)
        {
            uint8_t *p = (uint8_t *)val;
            *(p) = (*p * 100) + h * 10 + l;
        }
        else if (size <= 2)
        {
            uint16_t *p = (uint16_t *)val;
            *(p) = (*p * 100) + h * 10 + l;
        }
        else if (size <= 4)
        {
            uint32_t *p = (uint32_t *)val;
            *(p) = (*p * 100) + h * 10 + l;
        }
        else
        {
            uint64_t *p = (uint64_t *)val;
            *(p) = (*p * 100) + h * 10 + l;
        }
        count++;
    }
    return count;
}

void ByteBuffer_ctor(ByteBuffer *const me, uint32_t size)
{
    if (me == NULL || size < 0)
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
    if (me == NULL || buff == NULL || size < 0)
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
    if (me == NULL || buff == NULL || size < 0)
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

void ByteBuffer_ctor_fromHexStr(ByteBuffer *const me, char const *const hexStr, uint32_t size)
{
    if (me == NULL || hexStr == NULL || size < 0 || size & 1 == 1)
    {
        return;
    }
    me->size = size / 2;
    me->position = me->size;
    me->limit = me->size;
    me->wrapped = false;
    me->buff = (uint8_t *)malloc(me->size);
    memset(me->buff, 0, me->size);
    hex2bin(hexStr, size, me->buff, me->size);
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

#define CRC_POLY_VALUE 0xA001
static void CRC16(const uint8_t *bin, uint16_t *crc16, uint32_t size)
{
    *crc16 = 0xFFFF;
    for (uint32_t i = 0; i < size; i++)
    {
        *crc16 ^= bin[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            uint8_t b = *crc16 & 0x01;
            *crc16 >>= 1;
            if (b == 1)
            {
                *crc16 ^= CRC_POLY_VALUE;
            }
        }
    }
}

uint8_t ByteBuffer_CRC16(ByteBuffer *const me, uint16_t *crc16, uint32_t start, uint32_t size)
{
    if (me == NULL || start < 0 || size < 0 || start + size > me->limit)
    {
        return 0;
    }
    CRC16(me->buff + start, crc16, size);
    return 1;
}

ByteBuffer *ByteBuffer_GetByteBuffer(ByteBuffer *const me, uint32_t size)
{
    ByteBuffer *val = ByteBuffer_PeekByteBuffer(me, me->position, size);
    if (val != NULL)
    {
        me->position += size;
    }
    return val;
}

ByteBuffer *ByteBuffer_PeekByteBuffer(ByteBuffer *const me, uint32_t start, uint32_t size)
{
    if (me == NULL || start < 0 || size < 0 || start + size > me->limit)
    {
        return NULL;
    }
    ByteBuffer *val = NewInstance(ByteBuffer);
    ByteBuffer_ctor_copy(val, me->buff + start, size);
    return val;
}

uint8_t ByteBuffer_BE_PeekUIntAt(ByteBuffer *const me, uint8_t index, void *val, uint8_t size)
{
    if (me == NULL || val == NULL || size < 0 || index < 0 || index + size >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = binToBeUInt(me->buff + index, val, size);
    if (usedLen == size)
    {
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_LE_PeekUIntAt(ByteBuffer *const me, uint8_t index, void *val, uint8_t size)
{
    if (me == NULL || val == NULL || size < 0 || index < 0 || index + size >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = binToBeUInt(me->buff + me->position, val, size);
    if (usedLen == size)
    {
        return usedLen;
    }
    return 0;
}

uint8_t ByteBuffer_BE_PeekUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    return ByteBuffer_BE_PeekUIntAt(me, 0, val, 2);
}

uint8_t ByteBuffer_BE_PeekUInt16At(ByteBuffer *const me, uint8_t index, uint16_t *val)
{
    return ByteBuffer_BE_PeekUIntAt(me, index, val, 2);
}

uint8_t ByteBuffer_LE_PeekUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    return ByteBuffer_LE_PeekUIntAt(me, 0, val, 2);
}

uint8_t ByteBuffer_PeekUInt8(ByteBuffer *const me, uint8_t *val)
{
    return ByteBuffer_PeekUInt8At(me, me->position, val);
}

uint8_t ByteBuffer_PeekUInt8At(ByteBuffer *const me, uint8_t index, uint8_t *val)
{
    if (me == NULL || val == NULL || index >= me->limit || index < 0)
    {
        return 0;
    }
    *val = me->buff[index];
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
    // *val = 0; // 副作用
    return ByteBuffer_BE_GetUInt(me, val, 2);
}

uint8_t ByteBuffer_BE_GetUInt32(ByteBuffer *const me, uint32_t *val)
{
    // *val = 0; // 副作用
    return ByteBuffer_BE_GetUInt(me, val, 4);
}

uint8_t ByteBuffer_BE_GetUInt64(ByteBuffer *const me, uint64_t *val)
{
    // *val = 0; // 副作用
    return ByteBuffer_BE_GetUInt(me, val, 8);
}

uint8_t ByteBuffer_LE_GetUInt16(ByteBuffer *const me, uint16_t *val)
{
    // *val = 0; // 副作用
    return ByteBuffer_LE_GetUInt(me, val, 2);
}

uint8_t ByteBuffer_LE_GetUInt32(ByteBuffer *const me, uint32_t *val)
{
    // *val = 0; // 副作用
    return ByteBuffer_LE_GetUInt(me, val, 4);
}

uint8_t ByteBuffer_LE_GetUInt64(ByteBuffer *const me, uint64_t *val)
{
    // *val = 0; // 副作用
    return ByteBuffer_LE_GetUInt(me, val, 8);
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

uint8_t ByteBuffer_BCDGetUInt(ByteBuffer *const me, void *val, uint8_t size)
{
    if (me == NULL || me->position + size - 1 >= me->limit)
    {
        return 0;
    }
    uint8_t usedLen = binToBCDUInt(me->buff + me->position, val, size);
    if (usedLen == size)
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
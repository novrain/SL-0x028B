#include "binary.h"

ByteOrder hostEndian()
{
    return *(int8_t *)&ENDIAN_MAGIC == ENDIAN_MAGIC_HIGH_BYTE ? BigEndian : LittleEndian;
}

// LittleEndian

// read
uint16_t toLeUInt16(uint8_t *pBytes)
{
    uint16_t v;

    v = pBytes[0];
    v |= pBytes[1] << 8;
    return v;
}

uint32_t toLeUInt32(uint8_t *pBytes)
{
    uint32_t v;

    v = pBytes[0];
    v |= pBytes[1] << 8;
    v |= pBytes[2] << 16;
    v |= pBytes[3] << 24;
    return v;
}

uint64_t toLeUInt64(uint8_t *pBytes)
{
    uint64_t v;
    v = (uint64_t)pBytes[0];
    v |= (uint64_t)pBytes[1] << 8;
    v |= (uint64_t)pBytes[2] << 16;
    v |= (uint64_t)pBytes[3] << 24;
    v |= (uint64_t)pBytes[4] << 32;
    v |= (uint64_t)pBytes[5] << 40;
    v |= (uint64_t)pBytes[6] << 48;
    v |= (uint64_t)pBytes[7] << 56;
    return v;
}

// write
void putLeUInt16(uint8_t *pBytes, uint16_t v)
{
    pBytes[0] = v;
    pBytes[1] = v >> 8;
    pBytes[2] = v >> 16;
}
void putLeUInt32(uint8_t *pBytes, uint32_t v)
{
    pBytes[0] = v;
    pBytes[1] = v >> 8;
    pBytes[2] = v >> 16;
    pBytes[3] = v >> 24;
}
void putLeUInt64(uint8_t *pBytes, uint64_t v)
{
    pBytes[0] = v;
    pBytes[1] = v >> 8;
    pBytes[2] = v >> 16;
    pBytes[3] = v >> 24;
    pBytes[4] = v >> 32;
    pBytes[5] = v >> 40;
    pBytes[6] = v >> 48;
    pBytes[7] = v >> 56;
}

// BigEndian

// read
uint16_t toBeUInt16(uint8_t *pBytes)
{
    uint16_t v;

    v = pBytes[0] << 8;
    v |= pBytes[1];
    return v;
}
uint32_t toBeUInt32(uint8_t *pBytes)
{
    uint32_t v;

    v = pBytes[0] << 24;
    v |= pBytes[1] << 16;
    v |= pBytes[2] << 8;
    v |= pBytes[3];
    return v;
}
uint64_t toBeUInt64(uint8_t *pBytes)
{
    uint64_t v;

    v = (uint64_t)pBytes[0] << 56;
    v |= (uint64_t)pBytes[1] << 48;
    v |= (uint64_t)pBytes[2] << 40;
    v |= (uint64_t)pBytes[3] << 32;
    v |= (uint64_t)pBytes[4] << 24;
    v |= pBytes[5] << 16;
    v |= pBytes[6] << 8;
    v |= pBytes[7];
    return v;
}

// write
void putBeUInt16(uint8_t *pBytes, uint16_t v)
{
    pBytes[0] = v >> 8;
    pBytes[1] = v;
}

void putBeUInt32(uint8_t *pBytes, uint32_t v)
{
    pBytes[0] = v >> 24;
    pBytes[1] = v >> 16;
    pBytes[2] = v >> 8;
    pBytes[3] = v;
}

void putBeUInt64(uint8_t *pBytes, uint64_t v)
{
    pBytes[0] = v >> 56;
    pBytes[1] = v >> 48;
    pBytes[2] = v >> 40;
    pBytes[3] = v >> 32;
    pBytes[4] = v >> 24;
    pBytes[5] = v >> 16;
    pBytes[6] = v >> 8;
    pBytes[7] = v;
}
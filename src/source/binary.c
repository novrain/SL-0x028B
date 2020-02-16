#include "binary.h"

ByteOrder hostEndian()
{
    return *(char *)&ENDIAN_MAGIC == ENDIAN_MAGIC_HIGH_BYTE ? BigEndian : LittleEndian;
}

// LittleEndian

// read
unsigned short toLeUInt16(unsigned char *pBytes)
{
    unsigned short v;

    v = pBytes[0];
    v |= pBytes[1] << 8;
    return v;
}

unsigned long toLeUInt32(unsigned char *pBytes)
{
    unsigned long v;

    v = pBytes[0];
    v |= pBytes[1] << 8;
    v |= pBytes[2] << 16;
    v |= pBytes[3] << 24;
    return v;
}

unsigned long long toLeUInt64(unsigned char *pBytes)
{
    unsigned long long v;
    v = (unsigned long long)pBytes[0];
    v |= (unsigned long long)pBytes[1] << 8;
    v |= (unsigned long long)pBytes[2] << 16;
    v |= (unsigned long long)pBytes[3] << 24;
    v |= (unsigned long long)pBytes[4] << 32;
    v |= (unsigned long long)pBytes[5] << 40;
    v |= (unsigned long long)pBytes[6] << 48;
    v |= (unsigned long long)pBytes[7] << 56;
    return v;
}

// write
void putLeUInt16(unsigned char *pBytes, unsigned short v)
{
    pBytes[0] = v;
    pBytes[1] = v >> 8;
    pBytes[2] = v >> 16;
}
void putLeUInt32(unsigned char *pBytes, unsigned long v)
{
    pBytes[0] = v;
    pBytes[1] = v >> 8;
    pBytes[2] = v >> 16;
    pBytes[3] = v >> 24;
}
void putLeUInt64(unsigned char *pBytes, unsigned long long v)
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
unsigned short toBeUInt16(unsigned char *pBytes)
{
    unsigned short v;

    v = pBytes[0] << 8;
    v |= pBytes[1];
    return v;
}
unsigned long toBeUInt32(unsigned char *pBytes)
{
    unsigned long v;

    v = pBytes[0] << 24;
    v |= pBytes[1] << 16;
    v |= pBytes[2] << 8;
    v |= pBytes[3];
    return v;
}
unsigned long long toBeUInt64(unsigned char *pBytes)
{
    unsigned long long v;

    v = (unsigned long long)pBytes[0] << 56;
    v |= (unsigned long long)pBytes[1] << 48;
    v |= (unsigned long long)pBytes[2] << 40;
    v |= (unsigned long long)pBytes[3] << 32;
    v |= (unsigned long long)pBytes[4] << 24;
    v |= pBytes[5] << 16;
    v |= pBytes[6] << 8;
    v |= pBytes[7];
    return v;
}

// write
void putBeUInt16(unsigned char *pBytes, unsigned short v)
{
    pBytes[0] = v >> 8;
    pBytes[1] = v;
}

void putBeUInt32(unsigned char *pBytes, unsigned long v)
{
    pBytes[0] = v >> 24;
    pBytes[1] = v >> 16;
    pBytes[2] = v >> 8;
    pBytes[3] = v;
}

void putBeUInt64(unsigned char *pBytes, unsigned long long v)
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
#ifndef H_BYTEBUFFER
#define H_BYTEBUFFER

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>

#include "class.h"

    static uint16_t ENDIAN_MAGIC = 0xFEEF;
    static uint8_t ENDIAN_MAGIC_HIGH_BYTE = 0xFE;
    static uint8_t ENDIAN_MAGIC_LOW_BYTE = 0xEF;

    typedef enum
    {
        LittleEndian,
        BigEndian
    } ByteOrder;

    static inline ByteOrder hostEndian()
    {
        return *(int8_t *)&ENDIAN_MAGIC == ENDIAN_MAGIC_HIGH_BYTE ? BigEndian : LittleEndian;
    }

    typedef struct
    {
        uint32_t size;
        uint8_t *buff;
        uint32_t position;
        uint32_t limit;
        bool wrapped;
    } ByteBuffer;

    /**
     * Construtor
     * @param size buffer 大小
     */
    void ByteBuffer_ctor(ByteBuffer *const me, uint32_t size);
    void ByteBuffer_ctor_wrapped(ByteBuffer *const me, uint8_t *buff, uint32_t len);
    void ByteBuffer_ctor_copy(ByteBuffer *const me, uint8_t *buff, uint32_t len);

    void ByteBuffer_dtor(ByteBuffer *const me);

    /**
     * Destructor
     */
    void ByteBuffer_dtor(ByteBuffer *const me);

    /**
     * Reset the Buffer.
     */
    void ByteBuffer_Clear(ByteBuffer *const me);

    /**
     * Compacts the buffer. Drop readed
     */
    void ByteBuffer_Compact(ByteBuffer *const me);

    /**
     * Flip the buffer. Change to Read mode.
     */
    void ByteBuffer_Flip(ByteBuffer *const me);

    /**
     * 重新开始读取
     */
    void ByteBuffer_Rewind(ByteBuffer *const me);

    ByteBuffer *ByteBuffer_GetByteBuffer(ByteBuffer *const me, uint32_t size);
    ByteBuffer *ByteBuffer_PeekByteBuffer(ByteBuffer *const me, uint32_t size);

#define ByteBuffer_Available(ptr_) ((ptr_)->limit - (ptr_)->position)

    uint8_t ByteBuffer_PeekUInt8(ByteBuffer *const me, uint8_t *val);

    uint8_t ByteBuffer_BE_GetUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t ByteBuffer_LE_GetUInt(ByteBuffer *const me, void *val, uint8_t size);

    uint8_t ByteBuffer_GetUInt8(ByteBuffer *const me, uint8_t *val);
    uint8_t ByteBuffer_PutUInt8(ByteBuffer *const me, uint8_t val);

    uint8_t ByteBuffer_BE_HEXGetUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t ByteBuffer_LE_HEXGetUInt(ByteBuffer *const me, void *val, uint8_t size);
    // uint8_t ByteBuffer_BE_HEXPutUInt(ByteBuffer *const me, void *val, uint8_t size);

    uint8_t ByteBuffer_HEXGetUInt8(ByteBuffer *const me, uint8_t *val);
    uint8_t ByteBuffer_HEXPutUInt8(ByteBuffer *const me, uint8_t val);
    // BigEndian GET integer as RAW
    uint8_t ByteBuffer_BE_GetUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t ByteBuffer_BE_GetUInt32(ByteBuffer *const me, uint32_t *val);
    uint8_t ByteBuffer_BE_GetUInt64(ByteBuffer *const me, uint64_t *val);
    // BigEndian GET integer as HEX
    uint8_t ByteBuffer_BE_HEXGetUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t ByteBuffer_BE_HEXGetUInt32(ByteBuffer *const me, uint32_t *val);
    uint8_t ByteBuffer_BE_HEXGetUInt64(ByteBuffer *const me, uint64_t *val);
    // LittleEndian GET integer as RAW
    uint8_t ByteBuffer_LE_GetUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t ByteBuffer_LE_GetUInt32(ByteBuffer *const me, uint32_t *val);
    uint8_t ByteBuffer_LE_GetUInt64(ByteBuffer *const me, uint64_t *val);
    // LittleEndian GET integer as HEX
    uint8_t ByteBuffer_LE_HEXGetUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t ByteBuffer_LE_HEXGetUInt32(ByteBuffer *const me, uint32_t *val);
    uint8_t ByteBuffer_LE_HEXGetUInt64(ByteBuffer *const me, uint64_t *val);
    // BigEndian PUT integer as RAW
    uint8_t ByteBuffer_BE_PutUInt16(ByteBuffer *const me, uint16_t val);
    uint8_t ByteBuffer_BE_PutUInt32(ByteBuffer *const me, uint32_t val);
    uint8_t ByteBuffer_BE_PutUInt64(ByteBuffer *const me, uint64_t val);
    // BigEndian PUT integer as HEX
    uint8_t ByteBuffer_BE_HEXPutUInt16(ByteBuffer *const me, uint16_t val);
    uint8_t ByteBuffer_BE_HEXPutUInt32(ByteBuffer *const me, uint32_t val);
    uint8_t ByteBuffer_BE_HEXPutUInt64(ByteBuffer *const me, uint64_t val);
    // LittleEndian PUT integer as RAW
    uint8_t ByteBuffer_LE_PutUInt16(ByteBuffer *const me, uint16_t val);
    uint8_t ByteBuffer_LE_PutUInt32(ByteBuffer *const me, uint32_t val);
    uint8_t ByteBuffer_LE_PutUInt64(ByteBuffer *const me, uint64_t val);
    // LittleEndian PUT integer as HEX
    uint8_t ByteBuffer_LE_HEXPutUInt16(ByteBuffer *const me, uint16_t val);
    uint8_t ByteBuffer_LE_HEXPutUInt32(ByteBuffer *const me, uint32_t val);
    uint8_t ByteBuffer_LE_HEXPutUInt64(ByteBuffer *const me, uint64_t val);

    // BCD
    uint8_t ByteBuffer_BCDGetUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t ByteBuffer_BCDGetUInt8(ByteBuffer *const me, uint8_t *val);

#ifdef __cplusplus
}
#endif

#endif
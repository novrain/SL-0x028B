#ifndef H_BYTEBUFFER
#define H_BYTEBUFFER

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>

    typedef enum
    {
        LittleEndian,
        BigEndian
    } ByteOrder;

    inline uint8_t charToByte(const char ch)
    {
        switch (ch)
        {
        case '0' ... '9':
            return ch - '0';
        case 'a' ... 'f':
            return 0xa + ch - 'a';
        case 'A' ... 'F':
            return 0xa + (ch - 'A');
        default:
            return 0xF;
        }
    }

    inline void hex2bin(char const *const hexStr, uint32_t hexSize, uint8_t *bin, uint32_t binSize)
    {
        if (hexSize != binSize * 2)
        {
            return;
        }

        for (uint32_t i = 0; i < binSize; i++)
        {
            bin[i] = (charToByte(hexStr[i * 2]) << 4) + charToByte(hexStr[i * 2 + 1]);
        }
    }

    inline ByteOrder hostEndian()
    {
        uint16_t ENDIAN_MAGIC = 0xFEEF;
        uint8_t ENDIAN_MAGIC_HIGH_BYTE = 0xFE;
        // uint8_t ENDIAN_MAGIC_LOW_BYTE = 0xEF;
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

#define BB_Position(ptr_) (ptr_)->position
#define BB_Limit(ptr_) (ptr_)->limit
#define BB_Size(ptr_) (ptr_)->size
#define BB_Available(ptr_) ((ptr_)->limit - (ptr_)->position)
#define BB_Equal(sPtr_, dPtr_) ((sPtr_) != NULL) &&                    \
                                   ((dPtr_) != NULL) &&                \
                                   ((sPtr_->buff) != NULL) &&          \
                                   ((dPtr_->buff) != NULL) &&          \
                                   ((sPtr_)->size == (dPtr_)->size) && \
                                   (memcmp((sPtr_)->buff, (dPtr_)->buff, (sPtr_)->size) == 0)

    /**
     * Construtor
     * @param size buffer 大小
     */
    void BB_ctor(ByteBuffer *const me, uint32_t size);
    void BB_ctor_wrapped(ByteBuffer *const me, uint8_t *buff, uint32_t len);
    void BB_ctor_wrappedAnother(ByteBuffer *const me, ByteBuffer *another, uint32_t start, uint32_t end);
    void BB_ctor_copy(ByteBuffer *const me, uint8_t *buff, uint32_t len);
    // Test Only
    void BB_ctor_fromHexStr(ByteBuffer *const me, char const *const buff, uint32_t len);

    /**
     * Destructor
     */
    void BB_dtor(ByteBuffer *const me);

    /**
     * Reset the Buffer.
     */
    void BB_Clear(ByteBuffer *const me);

    /**
     * Compacts the buffer. Drop readed
     */
    void BB_Compact(ByteBuffer *const me);

    /**
     * Flip the buffer. Change to Read mode.
     */
    void BB_Flip(ByteBuffer *const me);

    /**
     * 重新开始读取
     */
    void BB_Rewind(ByteBuffer *const me);

    void BB_Skip(ByteBuffer *const me, uint32_t size);

    void BB_Expand(ByteBuffer *const me, uint32_t size);

    /**
     * CRC16
     */
    uint8_t BB_CRC16(ByteBuffer *const me, uint16_t *crc16, uint32_t start, uint32_t size);

    ByteBuffer *BB_GetByteBuffer(ByteBuffer *const me, uint32_t size);
    bool BB_CopyToByteBufferAt(ByteBuffer *const me, uint32_t start, uint32_t size, ByteBuffer *const dest);
    bool BB_CopyToByteBuffer(ByteBuffer *const me, uint32_t size, ByteBuffer *const dest);
    bool BB_PutByteBuffer(ByteBuffer *const me, ByteBuffer *const src);
    ByteBuffer *BB_PeekByteBuffer(ByteBuffer *const me, uint32_t start, uint32_t size);
    bool BB_PutString(ByteBuffer *const me, char *src);
    char *BB_GetString(ByteBuffer *const me, uint32_t size);
    char *BB_PeekString(ByteBuffer *const me, uint32_t start, uint32_t size);

    uint8_t BB_PeekUInt8(ByteBuffer *const me, uint8_t *val);
    uint8_t BB_PeekUInt8At(ByteBuffer *const me, uint32_t index, uint8_t *val);

    uint8_t BB_BE_PeekUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t BB_LE_PeekUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t BB_BE_PeekUIntAt(ByteBuffer *const me, uint32_t index, void *val, uint8_t size);
    uint8_t BB_LE_PeekUIntAt(ByteBuffer *const me, uint32_t index, void *val, uint8_t size);
    uint8_t BB_BE_PeekUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t BB_BE_PeekUInt16At(ByteBuffer *const me, uint32_t index, uint16_t *val);

    uint8_t BB_BE_GetUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t BB_LE_GetUInt(ByteBuffer *const me, void *val, uint8_t size);

    uint8_t BB_GetUInt8(ByteBuffer *const me, uint8_t *val);
    uint8_t BB_PutUInt8(ByteBuffer *const me, uint8_t val);

    uint8_t BB_BE_GetUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t BB_BE_GetUInt32(ByteBuffer *const me, uint32_t *val);
    uint8_t BB_BE_GetUInt64(ByteBuffer *const me, uint64_t *val);
    uint8_t BB_LE_GetUInt16(ByteBuffer *const me, uint16_t *val);
    uint8_t BB_LE_GetUInt32(ByteBuffer *const me, uint32_t *val);
    uint8_t BB_LE_GetUInt64(ByteBuffer *const me, uint64_t *val);
    uint8_t BB_BE_PutUInt(ByteBuffer *const me, uint64_t val, uint8_t size);
    uint8_t BB_BE_PutUInt16(ByteBuffer *const me, uint16_t val);
    uint8_t BB_BE_PutUInt32(ByteBuffer *const me, uint32_t val);
    uint8_t BB_BE_PutUInt64(ByteBuffer *const me, uint64_t val);
    uint8_t BB_LE_PutUInt16(ByteBuffer *const me, uint16_t val);
    uint8_t BB_LE_PutUInt32(ByteBuffer *const me, uint32_t val);
    uint8_t BB_LE_PutUInt64(ByteBuffer *const me, uint64_t val);
    // BCD
    uint8_t BB_BCDPeekUIntAt(ByteBuffer *const me, uint32_t index, void *val, uint8_t size);
    uint8_t BB_BCDGetUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t BB_BCDGetUInt8(ByteBuffer *const me, uint8_t *val);

    uint8_t BB_BE_BCDPutUInt(ByteBuffer *const me, void *val, uint8_t size);
    uint8_t BB_BCDPutUInt8(ByteBuffer *const me, uint8_t val);

#ifdef __cplusplus
}
#endif

#endif
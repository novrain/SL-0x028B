#include <assert.h>

#include "sl651.h"

// "AbstractClass" Package
/* purely-virtual */
static void Package_Virtual_Encode2Hex(Package const *const me, uint8_t *hex, size_t len)
{
    assert(0);
}
static void Package_Virtual_DecodeFromHex(Package const *const me, const uint8_t *hex, size_t len)
{
    assert(0);
}
static size_t Package_Virtual_Size()
{
    assert(0);
    return 0;
}

/* Constructor */
void Package_ctor(Package *me, Head head)
{
    PackageVtbl const vtbl = {
        &Package_Virtual_Encode2Hex,
        &Package_Virtual_DecodeFromHex,
        &Package_Virtual_Size};
    me->head = head;
}

/* Methods */
void Package_Head2Hex(Package const *const me, uint8_t *hex, size_t len)
{
}

void Package_Tail2Hex(Package const *const me, uint8_t *hex, size_t len)
{
}

void Package_Hex2Head(Package const *me, uint8_t *hex, size_t len)
{
}
void Package_Hex2Tail(Package const *me, uint8_t *hex, size_t len)
{
}

// "AbstractClass" Package END

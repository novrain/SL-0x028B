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

/* Package Constructor */
void Package_ctor(Package *me, Head *head)
{
    assert(me);
    static PackageVtbl const vtbl = {
        &Package_Virtual_Encode2Hex,
        &Package_Virtual_DecodeFromHex,
        &Package_Virtual_Size};
    me->vptr = &vtbl;
    me->head = head;
}

/* Methods  & Destrucor */
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

/* LinkMessage Construtor & Destrucor */
void LinkMessage_ctor(LinkMessage *const me, Head *head, uint16_t elementCount)
{
}
void LinkMessage_dtor(LinkMessage *const me)
{
}
/* Public methods */
void LinkMessage_Elements2Hex(LinkMessage const *const me, uint8_t *hex, size_t len)
{
}
void LinkMessage_Hex2Elements(LinkMessage const *me, uint8_t *hex, size_t len)
{
}

void LinkMessage_putElement(LinkMessage const *me, uint16_t index, Element *element)
{
}
void LinkMessage_setElement(LinkMessage const *me, uint16_t index, Element *element)
{
}
Element *LinkMessage_getElement(LinkMessage const *me, uint16_t index)
{
    return NULL;
}
// "Basic" LinkMessage END

// "AbstractUpClass" UplinkMessage
/* UplinkMessage Construtor & Destrucor */
void UplinkMessage_ctor(UplinkMessage *const me, Head *head, UplinkMessageHead *upLinkHead, uint16_t elementCount)
{
}
void UplinkMessage_dtor(UplinkMessage *const me)
{
}
/* Public methods */
void UplinkMessage_Head2Hex(UplinkMessage const *const me, uint8_t *hex, size_t len)
{
}
void UplinkMessage_Hex2Head(UplinkMessage const *me, uint8_t *hex, size_t len)
{
}
// "AbstractUpClass" UplinkMessage END

// "AbstractUpClass" DownlinkMessage

/* DownlinkMessage Construtor  & Destrucor */
void DownlinkMessage_ctor(DownlinkMessage *const me, Head *head, DownlinkMessageHead *downLinkHead, uint16_t elementCount)
{
}
void DownlinkMessage_dtor(DownlinkMessage *const me)
{
}
/* Public methods */
void DownlinkMessage_Head2Hex(DownlinkMessage const *const me, uint8_t *hex, size_t len)
{
}
void DownlinkMessage_Hex2Head(DownlinkMessage const *me, uint8_t *hex, size_t len)
{
}
// "AbstractUpClass" DownlinkMessage END
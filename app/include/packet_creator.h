#ifndef H_PACKET_CREATOR
#define H_PACKET_CREATOR

#ifdef __cplusplus
extern "C"
{
#endif

#include "cJSON/cJSON.h"
#include "vec/vec.h"
#include "tinydir/tinydir.h"
#include "sl651/sl651.h"

    // Element Creator，对应一个Element
    typedef struct
    {
        struct ElementCreatorVtbl const *vptr; // 虚函数，由子类实现
    } ElementCreator;
    void ElementCreator_ctor(ElementCreator *const me);
    void ElementCreator_dtor(ElementCreator *const me);
    typedef vec_t(ElementCreator *) ElementCreatorPtrVector;

    // 虚函数表
    typedef struct ElementCreatorVtbl
    {

        // 暂时不支持分包
        Element *(*createElement)(ElementCreator *const me, cJSON *const data);
        void (*dtor)(ElementCreator *const me);
    } ElementCreatorVtbl;

    typedef struct
    {
        ElementCreator super;
    } NumberElementCreator;
    void NumberElementCreator_ctor(NumberElementCreator *const me);

    typedef struct
    {
        ElementCreator super;
    } TimeStepCodeElementCreator;
    void TimeStepCodeElementCreator_ctor(TimeStepCodeElementCreator *const me);

    typedef struct
    {
        ElementCreator super;
    } ObserveTimeElementCreator;
    void ObserveTimeElementCreator_ctor(ObserveTimeElementCreator *const me);

    Package *createPackage(cJSON *const data);

#ifdef __cplusplus
}
#endif
#endif
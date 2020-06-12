#include <assert.h>
#include <stdio.h>
#include "cJSON/cJSON_Helper.h"
#include "common/class.h"
#include "packet_creator.h"

// Element Creators
// ElementCreator Interface
void ElementCreator_ctor(ElementCreator *const me)
{
    assert(me);
}
void ElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    // do nothing
}
// ElementCreator Interface END

// NumberElementCreator
Element *NumberElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    // assert(data);
    cJSON_GET_NUMBER(id, uint8_t, data, 0, 16);
    cJSON_GET_NUMBER(vt, uint8_t, data, 0, 16);
    cJSON_GET_NUMBER(sign, bool, data, 0, 16);
    if (id == 0 || vt == 0)
    {
        return NULL;
    }
    double value = 0;
    cJSON_COPY_VALUE(value, v, data, valuedouble);
    Element *el = (Element *)NewInstance(NumberElement);
    NumberElement_ctor((NumberElement *)el, id, vt, sign);
    if ((vt & NUMBER_ELEMENT_PRECISION_MASK) == 0)
    {
        NumberElement_SetInteger((NumberElement *)el, value);
    }
    else
    {
        NumberElement_SetDouble((NumberElement *)el, value);
    }
    return el;
}

void NumberElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void NumberElementCreator_ctor(NumberElementCreator *const me)
{
    assert(me);
    static ElementCreatorVtbl const vtbl = {
        &NumberElementCreator_createElement,
        &NumberElementCreator_dtor};
    ElementCreator_ctor(&me->super);
    me->super.vptr = &vtbl;
}
// NumberElementCreator END

// TimeStepCodeElementCreator
Element *TimeStepCodeElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    assert(data);
    cJSON_GET_VALUE(step, char *, data, valuestring, NULL);
    cJSON_GET_NUMBER(id, uint8_t, data, 0, 16);
    cJSON_GET_NUMBER(vt, uint8_t, data, 0, 16);
    cJSON *values = cJSON_GetObjectItem(data, "v");
    if (step == NULL ||
        strlen(step) != 6 ||
        id == 0 ||
        vt == 0 ||
        values == NULL ||
        values->type != cJSON_Array ||
        cJSON_GetArraySize(values) <= 0)
    {
        return NULL;
    }
    ByteBuffer buff = {0};
    BB_ctor_fromHexStr(&buff, step, 6);
    BB_Flip(&buff);
    cJSON_GET_NUMBER(sign, bool, data, 0, 16);
    TimeStepCodeElement *el = NewInstance(TimeStepCodeElement);
    TimeStepCodeElement_ctor(el, sign);
    TimeStepCode_Decode(&el->timeStepCode, &buff);
    BB_dtor(&buff);
    NumberListElement_ctor(&el->numberListElement, id, vt, sign, cJSON_GetArraySize(values));
    cJSON *v;
    size_t i = 0;
    cJSON_ArrayForEach(v, values)
    {
        if ((vt & NUMBER_ELEMENT_PRECISION_MASK) == 0)
        {
            NumberListElement_SetIntegerAt(&el->numberListElement, i, v->valuedouble);
        }
        else
        {
            NumberListElement_SetDoubleAt(&el->numberListElement, i, v->valuedouble);
        }
        i++;
    }
    return (Element *)el;
}

void TimeStepCodeElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void TimeStepCodeElementCreator_ctor(TimeStepCodeElementCreator *const me)
{
    assert(me);
    static ElementCreatorVtbl const vtbl = {
        &TimeStepCodeElementCreator_createElement,
        &TimeStepCodeElementCreator_dtor};
    ElementCreator_ctor(&me->super);
    me->super.vptr = &vtbl;
}
// TimeStepCodeElementCreator END

// ObserveTimeElementCreator
Element *ObserveTimeElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    assert(data);
    cJSON_GET_VALUE(v, char *, data, valuestring, NULL);
    if (v != NULL && strlen(v) == 10)
    {
        ByteBuffer buff = {0};
        BB_ctor_fromHexStr(&buff, v, 10);
        BB_Flip(&buff);
        ObserveTimeElement *el = NewInstance(ObserveTimeElement);
        ObserveTimeElement_ctor(el);
        ObserveTime_Decode(&el->observeTime, &buff);
        BB_dtor(&buff);
        return (Element *)el;
    }
    else
    {
        return NULL;
    }
}

void ObserveTimeElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void ObserveTimeElementCreator_ctor(ObserveTimeElementCreator *const me)
{
    assert(me);
    static ElementCreatorVtbl const vtbl = {
        &ObserveTimeElementCreator_createElement,
        &ObserveTimeElementCreator_dtor};
    ElementCreator_ctor(&me->super);
    me->super.vptr = &vtbl;
}
// ObserveTimeElementCreator END

// DRP5MINElementCreator
Element *DRP5MINElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    assert(data);
    cJSON *values = cJSON_GetObjectItem(data, "v");
    if (values == NULL ||
        values->type != cJSON_Array ||
        cJSON_GetArraySize(values) != 12)
    {
        return NULL;
    }
    DRP5MINElement *el = NewInstance(DRP5MINElement);
    DRP5MINElement_ctor(el);
    cJSON *v;
    size_t i = 0;
    cJSON_ArrayForEach(v, values)
    {
        DRP5MINElement_SetValueAt(el, i, v->valuedouble);
        i++;
    }
    return (Element *)el;
}

void DRP5MINElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void DRP5MINElementCreator_ctor(DRP5MINElementCreator *const me)
{
    assert(me);
    static ElementCreatorVtbl const vtbl = {
        &DRP5MINElementCreator_createElement,
        &DRP5MINElementCreator_dtor};
    ElementCreator_ctor(&me->super);
    me->super.vptr = &vtbl;
}
// DRP5MINElementCreator END

// RelativeWaterLevelElementCreator
Element *RelativeWaterLevelElementCreator_createElement(ElementCreator *const me, cJSON *const data)
{
    assert(me);
    assert(data);
    cJSON_GET_NUMBER(id, uint8_t, data, 0, 16);
    cJSON *values = cJSON_GetObjectItem(data, "v");
    if (id == 0 ||
        values == NULL ||
        values->type != cJSON_Array ||
        cJSON_GetArraySize(values) != 12)
    {
        return NULL;
    }
    RelativeWaterLevelElement *el = NewInstance(RelativeWaterLevelElement);
    RelativeWaterLevelElement_ctor(el, id);
    cJSON *v;
    size_t i = 0;
    cJSON_ArrayForEach(v, values)
    {
        RelativeWaterLevelElement_SetValueAt(el, i, v->valuedouble);
        i++;
    }
    return (Element *)el;
}

void RelativeWaterLevelElementCreator_dtor(ElementCreator *const me)
{
    assert(me);
    ElementCreator_dtor(me);
}

void RelativeWaterLevelElementCreator_ctor(RelativeWaterLevelElementCreator *const me)
{
    assert(me);
    static ElementCreatorVtbl const vtbl = {
        &RelativeWaterLevelElementCreator_createElement,
        &RelativeWaterLevelElementCreator_dtor};
    ElementCreator_ctor(&me->super);
    me->super.vptr = &vtbl;
}
// RelativeWaterLevelElementCreator END
// Element Creators END

Package *createPackage(cJSON *const schema)
{
    assert(schema);
    // assert(data);
    Package *pkg = NULL;
    LinkMessage *linkMsg = NULL;
    // functionCode 不做有效性判断
    cJSON_GET_NUMBER(fcode, uint8_t, schema, 0, 16);
    cJSON_GET_VALUE(direction, Direction, schema, valuedouble, Up);
    if (direction == Down)
    {
        DownlinkMessage *msg = NewInstance(DownlinkMessage);
        DownlinkMessage_ctor((DownlinkMessage *)msg, DEFAULT_ELEMENT_NUMBER);
        pkg = (Package *)msg;
        linkMsg = (LinkMessage *)msg;
    }
    else
    {
        UplinkMessage *msg = NewInstance(UplinkMessage);
        UplinkMessage_ctor((UplinkMessage *)msg, DEFAULT_ELEMENT_NUMBER);
        pkg = (Package *)msg;
        linkMsg = (LinkMessage *)msg;
        cJSON_GET_VALUE(observetime, char *, schema, valuestring, NULL);
        if (observetime != NULL && strlen(observetime) == 10)
        {
            ByteBuffer buff = {0};
            BB_ctor_fromHexStr(&buff, observetime, 10);
            BB_Flip(&buff);
            ObserveTime_Decode(&msg->messageHead.observeTimeElement.observeTime, &buff);
            BB_dtor(&buff);
        }
        else
        {
            ObserveTime_now(&msg->messageHead.observeTimeElement.observeTime);
        }
    }
    bool validSchema = true;
    cJSON *elements = cJSON_GetObjectItem(schema, "elements");
    if (elements != NULL && elements->type == cJSON_Array)
    {
        cJSON *elementSchema;
        cJSON_ArrayForEach(elementSchema, elements)
        {
            cJSON_GET_VALUE(t, char *, elementSchema, valuestring, NULL);
            if (t == NULL)
            {
                validSchema = false;
                break;
            }
            ElementCreator *ec = NULL;
            // switch case ... 每种类型，创建对应的ElementCreator
            if (strcmp(t, "number") == 0)
            {
                ec = (ElementCreator *)(NewInstance(NumberElementCreator)); // 创建指针，需要转为Element*
                NumberElementCreator_ctor((NumberElementCreator *)ec);
            }
            if (strcmp(t, "time_step_code") == 0)
            {
                ec = (ElementCreator *)(NewInstance(TimeStepCodeElementCreator)); // 创建指针，需要转为Element*
                TimeStepCodeElementCreator_ctor((TimeStepCodeElementCreator *)ec);
            }
            if (strcmp(t, "observetime") == 0)
            {
                ec = (ElementCreator *)(NewInstance(ObserveTimeElementCreator)); // 创建指针，需要转为Element*
                ObserveTimeElementCreator_ctor((ObserveTimeElementCreator *)ec);
            }
            if (strcmp(t, "rain_hour_5min") == 0)
            {
                ec = (ElementCreator *)(NewInstance(DRP5MINElementCreator)); // 创建指针，需要转为Element*
                DRP5MINElementCreator_ctor((DRP5MINElementCreator *)ec);
            }
            if (strcmp(t, "water_hour_5min") == 0)
            {
                ec = (ElementCreator *)(NewInstance(RelativeWaterLevelElementCreator)); // 创建指针，需要转为Element*
                RelativeWaterLevelElementCreator_ctor((RelativeWaterLevelElementCreator *)ec);
            }
            // 无效的Element配置
            if (ec == NULL)
            {
                validSchema = false;
                break;
            }
            Element *el = ec->vptr->createElement(ec, elementSchema);
            // 无法创建Element
            if (el == NULL)
            {
                validSchema = false;
                ec->vptr->dtor(ec);
                DelInstance(ec);
                break;
            }
            ec->vptr->dtor(ec);
            DelInstance(ec);
            el->direction = direction;
            LinkMessage_PushElement(linkMsg, el);
        }
    }
    if (validSchema)
    {
        pkg->head.funcCode = fcode;
    }
    else
    {
        pkg->vptr->dtor(pkg);
        DelInstance(pkg);
    }
    return pkg;
}
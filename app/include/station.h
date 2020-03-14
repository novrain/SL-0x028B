
#ifndef H_STATION
#define H_STATION

#ifdef __cplusplus
extern "C"
{
#endif

#define GET_VALUE(target, jParent, jChild, vField)                   \
    (jChild) = cJSON_GetObjectItemCaseSensitive((jParent), #jChild); \
    if ((jChild) != NULL)                                            \
    {                                                                \
        (target) = (vField);                                         \
    }
#ifdef __cplusplus
}
#endif
#endif
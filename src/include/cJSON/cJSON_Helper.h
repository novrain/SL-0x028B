#ifndef H_CJSON_HELPER
#define H_CJSON_HELPER

#ifdef __cplusplus
extern "C"
{
#endif

#include "cJSON/cJSON.h"

#define cJSON_GET_VALUE(target, t, jParent, vField, defaultValue)                 \
    cJSON *target##InJSON = cJSON_GetObjectItemCaseSensitive((jParent), #target); \
    t target = defaultValue;                                                      \
    if (target##InJSON != NULL)                                                   \
    {                                                                             \
        (target) = (t)target##InJSON->vField;                                     \
    }

#define cJSON_GET_NUMBER(target, t, jParent, defaultValue, base)                  \
    cJSON *target##InJSON = cJSON_GetObjectItemCaseSensitive((jParent), #target); \
    t target = defaultValue;                                                      \
    if (target##InJSON != NULL)                                                   \
    {                                                                             \
        if (target##InJSON->type == cJSON_Number)                                 \
        {                                                                         \
            (target) = (t)target##InJSON->valuedouble;                            \
        }                                                                         \
        else if (target##InJSON->type == cJSON_String)                            \
        {                                                                         \
            (target) = (t)strtol(target##InJSON->valuestring, NULL, (base));      \
        }                                                                         \
    }

#define cJSON_COPY_VALUE(target, jField, jParent, vField)                   \
    cJSON *(jField) = cJSON_GetObjectItemCaseSensitive((jParent), #jField); \
    if ((jField) != NULL)                                                   \
    {                                                                       \
        (target) = (jField)->vField;                                        \
    }

#define cJSON_COPY_VALUE_CAST(target, jField, jParent, vField, t)           \
    cJSON *(jField) = cJSON_GetObjectItemCaseSensitive((jParent), #jField); \
    if ((jField) != NULL)                                                   \
    {                                                                       \
        (target) = (t)(jField)->vField;                                     \
    }

    cJSON *cJSON_FromFile(char const *const file);
    int cJSON_WriteFile(cJSON *const json, char const *const file);

#ifdef __cplusplus
}
#endif
#endif
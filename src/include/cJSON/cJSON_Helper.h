#ifndef H_CJSON_HELPER
#define H_CJSON_HELPER

#ifdef __cplusplus
extern "C"
{
#endif

#include "cJSON/cJSON.h"

    cJSON *cJSON_FromFile(char const *const file);
    int cJSON_WriteFile(cJSON *const json, char const *const file);

#ifdef __cplusplus
}
#endif
#endif
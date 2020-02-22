#ifndef H_CLASS
#define H_CLASS

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <memory.h>

#define NewInstance(Class)                              \
    ({                                                  \
        Class *ptr_ = ((Class *)malloc(sizeof(Class))); \
        (memset(ptr_, 0, sizeof(Class)));               \
        ptr_;                                           \
    })

#define DelInstance(ptr_) \
    do                    \
    {                     \
        free(ptr_);       \
        ptr_ = NULL;      \
    } while (0);

#ifdef __cplusplus
}
#endif

#endif
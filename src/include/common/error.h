#ifndef H_ERROR
#define H_ERROR

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "common/macros.h"

#define ERROR_DEFAULT_SUCCESS 0

#define ERROR_ENUM_STRIDE_BITS 10
#define ERROR_ENUM_STRIDE (1U << ERROR_ENUM_STRIDE_BITS)
#define ERROR_ENUM_BEGIN_RANGE(x) ((x)*ERROR_ENUM_STRIDE)
#define ERROR_ENUM_END_RANGE(x) (((x) + 1) * ERROR_ENUM_STRIDE - 1)

    struct error_info
    {
        int error_code;
        const char *literal_name;
        const char *error_str;
        const char *module_name;
        const char *formatted_name;
    };

    struct error_info_list
    {
        const struct error_info *error_list;
        uint16_t count;
    };

#define DEFINE_ERROR_INFO(C, ES, MOD)                                                   \
    {                                                                                   \
        .error_code = (C), .literal_name = #C, .error_str = (ES), .module_name = (MOD), \
        .formatted_name = MOD ": " #C ", " ES,                                          \
    }

    typedef void(error_handler_fn)(int err, void *ctx);

    /*
    * 返回当前错误码，0表示没有错误.
    */
    int last_error(void);

    /*
    * 返回当前错误码的描述信息.
    */
    const char *error_str(int err);

    /*
    * 返回当前错误码的名称.
    */
    const char *error_name(int err);

    /*
    * 返回当前错误码归属的模块.
    */
    const char *error_module_name(int err);

    /*
    * 返回整合的错误码信息.
    */
    const char *error_debug_str(int err);

    /*
    * 设置当前错误码.
    */
    void set_error(int err);

    /*
    * 设置当前错误码.
    */
    bool set_error_indicate(int err);

    /*
    * 重置错误为0.
    */
    void reset_error(void);

    /**
     * 注册错误模块
     */
    void register_error_info(const struct error_info_list *error_info);

#ifdef __cplusplus
}
#endif

#endif
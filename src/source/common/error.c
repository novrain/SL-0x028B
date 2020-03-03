#include <stdio.h>
#include <assert.h>

#include "common/error.h"

static THREAD_LOCAL int tl_last_error = 0;

#define MODULE_SLOTS 16
#define SLOT_MASK (ERROR_ENUM_STRIDE - 1)

static const int MAX_ERROR_CODE = ERROR_ENUM_STRIDE * MODULE_SLOTS;

static const struct error_info_list *volatile ERROR_SLOTS[MODULE_SLOTS] = {0};

int last_error(void)
{
    return tl_last_error;
}

static const struct error_info *get_error_by_code(int err)
{
    if (err >= MAX_ERROR_CODE || err < 0)
    {
        return NULL;
    }

    uint32_t slot_index = (uint32_t)err >> ERROR_ENUM_STRIDE_BITS;
    uint32_t error_index = (uint32_t)err & SLOT_MASK;

    const struct error_info_list *error_slot = ERROR_SLOTS[slot_index];

    if (!error_slot || error_index >= error_slot->count)
    {
        return NULL;
    }

    return &error_slot->error_list[error_index];
}

const char *error_str(int err)
{
    const struct error_info *error_info = get_error_by_code(err);

    if (error_info)
    {
        return error_info->error_str;
    }

    return "Unknown Error Code";
}

const char *error_name(int err)
{
    const struct error_info *error_info = get_error_by_code(err);

    if (error_info)
    {
        return error_info->literal_name;
    }

    return "Unknown Error Code";
}

const char *error_module_name(int err)
{
    const struct error_info *error_info = get_error_by_code(err);

    if (error_info)
    {
        return error_info->module_name;
    }

    return "Unknown Error Code";
}

const char *error_debug_str(int err)
{
    const struct error_info *error_info = get_error_by_code(err);

    if (error_info)
    {
        return error_info->formatted_name;
    }

    return "Unknown Error Code";
}

void set_error(int err)
{
    tl_last_error = err;
}

bool set_error_indicate(int err)
{
    tl_last_error = err;
    return err == ERROR_DEFAULT_SUCCESS;
}

void reset_error(void)
{
    tl_last_error = 0;
}

void register_error_info(const struct error_info_list *error_info)
{
    assert(error_info);
    assert(error_info->error_list);
    assert(error_info->count);

    const int min_range = error_info->error_list[0].error_code;
    const int slot_index = min_range >> ERROR_ENUM_STRIDE_BITS;

    if (slot_index >= MODULE_SLOTS || slot_index < 0)
    {
        fprintf(stderr, "Bad error slot index %d\n", slot_index);
        assert(false);
    }

    ERROR_SLOTS[slot_index] = error_info;
}
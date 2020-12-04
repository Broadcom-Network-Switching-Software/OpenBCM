
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL



#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/types/sw_state_string.h>
#include <soc/dnxc/swstate/dnxc_sw_state.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <bcm/types.h>








shr_error_e
sw_state_string_strncat(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *src)
{
    size_t offset, src_len;

    SHR_FUNC_INIT_VARS(unit);

    assert(string != NULL);

    offset = sal_strlen(string);
    src_len = sal_strlen(src);

    if ((offset + src_len) >= size)
    {
        src_len = size - offset;
    }

    DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, string + offset, src, src_len, DNXC_SW_STATE_NO_FLAGS, "string");

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_string_strncmp(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *cmp_string,
    int *result)
{
    int i = 0;
    SHR_FUNC_INIT_VARS(unit);

    assert(string != NULL);

    while (i < size && (string[i] == cmp_string[i]))
    {
        i++;
    }

    if (i == size)
    {
        *result = 0;
    }
    else if (string[i] < cmp_string[i])
    {
        *result = -1;
    }
    else
    {
        *result = 1;
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
sw_state_string_strncpy(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *src)
{
    size_t src_len;

    SHR_FUNC_INIT_VARS(unit);

    assert(string != NULL);

    src_len = sal_strlen(src);

    if (src_len >= size)
    {
        src_len = size - 1;
    }

    DNX_SW_STATE_MEMSET(unit, module_id, string, 0, 0x00, size, DNXC_SW_STATE_NO_FLAGS,
                        "sw_state string");

    if (src_len > 0)
    {
        DNX_SW_STATE_MEMCPY_BASIC(unit, module_id, string, src, src_len, DNXC_SW_STATE_NO_FLAGS, "sw state string");
    }

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_string_strlen(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 *size)
{
    SHR_FUNC_INIT_VARS(unit);

    assert(string != NULL);

    *size = sal_strlen(string);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_stringget(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    char *dest)
{
    SHR_FUNC_INIT_VARS(unit);

    assert(string != NULL);

    DNX_SW_STATE_MEMREAD(unit, dest, string, 0, sal_strlen(string) + 1, DNXC_SW_STATE_NO_FLAGS, "get the string.");

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sw_state_string_print(
    int unit,
    uint32 module_id,
    sw_state_string_t * string)
{
    SHR_FUNC_INIT_VARS(unit);
    if (string == NULL)
    {
        SHR_EXIT();
    }

    DNX_SW_STATE_PRINT(unit, "String :%s:\n", string);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

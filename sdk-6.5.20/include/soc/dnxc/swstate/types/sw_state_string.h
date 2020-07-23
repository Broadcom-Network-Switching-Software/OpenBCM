
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __SW_STATE_STRING_H__
#define __SW_STATE_STRING_H__

#include <soc/dnxc/swstate/sw_state_features.h>

#include <soc/dnxc/swstate/sw_state_defs.h>
#include <soc/dnxc/swstate/dnx_sw_state_logging.h>
#include <soc/dnxc/swstate/sw_state_diagnostics.h>

#define SW_STATE_STRING_STRINGNCAT(module_id, string, src, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strncat(unit, module_id, string, size, src))

#define SW_STATE_STRING_STRINGNCMP(module_id, string, cmp_string, result, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strncmp(unit, module_id, string, size, cmp_string, result))

#define SW_STATE_STRING_STRINGNCPY(module_id, string, src, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strncpy(unit, module_id, string, size, src))

#define SW_STATE_STRING_STRINGLEN(module_id, string, size)\
        SHR_IF_ERR_EXIT(sw_state_string_strlen(unit, module_id, string, size))

#define SW_STATE_STRING_STRINGGET(module_id, string, dest)\
        SHR_IF_ERR_EXIT(sw_state_stringget(unit, module_id, string, dest))

#define SW_STATE_STRING_STRING_PRINT(unit, module_id, string)\
        SHR_IF_ERR_EXIT(sw_state_string_print(unit, module_id, *(string)))

typedef char sw_state_string_t;

shr_error_e sw_state_string_strncat(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *src);

shr_error_e sw_state_string_strncmp(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *cmp_string,
    int *result);

shr_error_e sw_state_string_strncpy(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 size,
    char *src);

shr_error_e sw_state_string_strlen(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    uint32 *size);

shr_error_e sw_state_stringget(
    int unit,
    uint32 module_id,
    sw_state_string_t * string,
    char *dest);

shr_error_e sw_state_string_print(
    int unit,
    uint32 module_id,
    sw_state_string_t * string);
#endif

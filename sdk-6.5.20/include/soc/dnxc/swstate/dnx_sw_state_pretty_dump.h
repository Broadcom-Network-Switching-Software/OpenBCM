/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_SW_STATE_DUMP_PRETTY_H
#define _DNX_SW_STATE_DUMP_PRETTY_H

#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/dnxc/swstate/sw_state_defs.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
#include <bcm/types.h>
#include <soc/types.h>

#define SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE 1000

typedef enum
{
    SW_STATE_PRETTY_PRINT_TYPE_INVALID = -1,
    SW_STATE_PRETTY_PRINT_TYPE_UINT8,
    SW_STATE_PRETTY_PRINT_TYPE_UINT16,
    SW_STATE_PRETTY_PRINT_TYPE_UINT32,
    SW_STATE_PRETTY_PRINT_TYPE_CHAR,
    SW_STATE_PRETTY_PRINT_TYPE_SHORT,
    SW_STATE_PRETTY_PRINT_TYPE_INT,
    SW_STATE_PRETTY_PRINT_TYPE_MAC,
    SW_STATE_PRETTY_PRINT_TYPE_IPV4,
    SW_STATE_PRETTY_PRINT_TYPE_IPV6,
    SW_STATE_PRETTY_PRINT_TYPE_STRING,
    SW_STATE_PRETTY_PRINT_TYPE_COUNT
} dnx_sw_state_pretty_print_variable_type_e;

typedef struct
{

    dnx_sw_state_pretty_print_variable_type_e type;

    char *name;

    char *format_string;

    char *comment;

    uint8 sub_struct_level;

    uint8 is_arr_flag;

    uint8 arr_index;
} dnx_sw_state_pretty_print_t;

typedef struct
{
    int integer;
    bcm_mac_t mac;

} dnx_sw_state_pretty_print_example_t;

#define SW_STATE_PRETTY_PRINT_INIT_VARIABLES()\
    dnx_sw_state_pretty_print_t _internal_print_cb_data;\
    sal_memset(&_internal_print_cb_data, 0, sizeof(dnx_sw_state_pretty_print_t));

#define SW_STATE_PRETTY_PRINT_ADD_ARR(variable_type, variable_name, variable_data, variable_comment, variable_format, arr_elements_count)\
    for(int _arr_index = 0; _arr_index < arr_elements_count; _arr_index++)\
    {\
        SW_STATE_PRETTY_PRINT_ADD_ARR_SINGLE_LINE(variable_type, variable_name, variable_data[_arr_index], variable_comment, variable_format, _arr_index)\
    }

#define SW_STATE_PRETTY_PRINT_ADD_ARR_SINGLE_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format, array_index)\
    _internal_print_cb_data.is_arr_flag = TRUE;\
    _internal_print_cb_data.arr_index = _arr_index;\
    SW_STATE_PRETTY_PRINT_ADD_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format);

#define SW_STATE_PRETTY_PRINT_ADD_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format)\
    _internal_print_cb_data.type = variable_type;\
    _internal_print_cb_data.name = variable_name;\
    _internal_print_cb_data.comment = variable_comment;\
    _internal_print_cb_data.format_string = variable_format;\
    dnx_sw_state_pretty_print(unit, &_internal_print_cb_data, &variable_data);\
    _internal_print_cb_data.sub_struct_level = _internal_print_cb_data.sub_struct_level;

#define SW_STATE_PRETTY_PRINT_FINISH()\
    _internal_print_cb_data.type = SW_STATE_PRETTY_PRINT_TYPE_COUNT;

#define SW_STATE_PRETTY_PRINT_SUB_STRUCT_START()\
    _internal_print_cb_data.sub_struct_level += 1;

#define SW_STATE_PRETTY_PRINT_SUB_STRUCT_END()\
    if(_internal_print_cb_data.sub_struct_level > 0)\
    {\
        _internal_print_cb_data.sub_struct_level -= 1;\
    }

shr_error_e dnx_sw_state_pretty_print(
    int unit,
    dnx_sw_state_pretty_print_t * print_cb_data,
    void *data);

void dnx_sw_state_pretty_print_example(
    int unit,
    void *data);

#else

#define SW_STATE_PRETTY_PRINT_INIT_VARIABLES(_print_cb_data)
#define SW_STATE_PRETTY_PRINT_ADD_ARR(variable_type, variable_name, variable_data, variable_comment, variable_format, arr_elements_count)
#define SW_STATE_PRETTY_PRINT_ADD_ARR_SINGLE_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format, array_index)
#define SW_STATE_PRETTY_PRINT_ADD_LINE(variable_type, variable_name, variable_data, variable_comment, variable_format)
#define SW_STATE_PRETTY_PRINT_FINISH(_print_cb_data)
#define SW_STATE_PRETTY_PRINT_SUB_STRUCT_START()
#define SW_STATE_PRETTY_PRINT_SUB_STRUCT_END()
#endif
#endif

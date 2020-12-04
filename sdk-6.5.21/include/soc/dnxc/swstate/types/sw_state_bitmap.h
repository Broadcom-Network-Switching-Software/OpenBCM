
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_BITMAP_H

#define _DNXC_SW_STATE_BITMAP_H

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>

#define DNX_SW_STATE_ALLOC_BITMAP(unit, module_id, location, nof_bits_to_alloc, flags, dbg_string)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_alloc(unit, module_id, (uint8 **)(&location), nof_bits_to_alloc, flags, dbg_string));\
    } while(0)

#define DNX_SW_STATE_BIT_SET(_unit, _module_id, _flags, _sw_state_bmp, __bit_num)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_set(_unit, _module_id, _sw_state_bmp, __bit_num, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_CLEAR(_unit, _module_id, _flags ,_sw_state_bmp, __bit_num)\
     do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_clear(_unit, _module_id, _sw_state_bmp, __bit_num, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_GET(_unit, _module_id, _flags, _sw_state_bmp, __bit_num , _bit_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_get(_unit, _module_id, _sw_state_bmp, __bit_num, _bit_result, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_READ(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, _result_first, __range, _result_bmp)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_read(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _result_bmp, _result_first, __range, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_WRITE(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, _input_bmp_first, __range, _input_bmp)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_write(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, _input_bmp_first, __range, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_AND(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_and(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_OR(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_or(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_XOR(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_xor(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_REMOVE(_unit, _module_id, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_remove(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, _input_bmp, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_NEGATE(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_negate(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_CLEAR(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_clear(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_SET(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_set(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_NULL(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_null(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_TEST(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_test(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_EQ(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _input_bmp, _bool_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_eq(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, input_bmp, __count, _bool_result, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BIT_RANGE_COUNT(_unit, _module_id, _flags, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result)\
    do {\
        SHR_IF_ERR_EXIT(dnxc_sw_state_bitmap_range_count(_unit, _module_id, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result, _flags, NULL));\
    } while(0)

#define DNX_SW_STATE_BITMAP_DEFAULT_VALUE_SET(ptr, _nof_bits_to_alloc)\
    do{\
        memset((void *)(ptr), 0xFF, SHR_BITALLOCSIZE(_nof_bits_to_alloc) + 1);\
    } while(0)

int dnxc_sw_state_bitmap_alloc(
    int unit,
    uint32 module_id,
    uint8 **location,
    uint32 nof_bits_to_alloc,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 idx_bit,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_get(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 idx_bit,
    uint8 *bit_result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_read(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 source_first,
    SHR_BITDCL * result_location,
    uint32 result_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_write(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 source_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_and(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_or(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_xor(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_remove(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_negate(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_clear(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_set(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_null(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_test(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_eq(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    SHR_BITDCL * source_location,
    uint32 range,
    uint8 *result,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_bitmap_range_count(
    int unit,
    uint32 module_id,
    SHR_BITDCL * location,
    uint32 dest_first,
    uint32 range,
    int *result,
    uint32 flags,
    char *dbg_string);

#endif

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * INFO: this module is the entry point (the root) for the entire SDK global 
 *       sw state.
 *       if we look at the sw state as a tree, then the root lies at shr_sw_state_t,
 *       it's main branches (module's entry points) are defined here inside this file.
 *       and it continue to expand in the specific module's .h files (which are
 *       the files in shared/swstate/sw_state_includes.h)
 */
#ifndef _SHR_SW_STATE_DEFS_H
/* { */
#define _SHR_SW_STATE_DEFS_H

#include <soc/types.h>
#include <sal/core/sync.h>
#include <shared/swstate/sw_state_includes.h>
#include <shared/swstate/sw_state_example.h>
#ifdef BCM_DNX_SUPPORT
/* { */
#include <soc/dnx/dbal/dbal_structures.h>
/* } */
#endif
#if (defined(BCM_PETRA_SUPPORT))
#include <soc/dpp/dpp_sw_state.h>
#include <bcm_int/dpp/bcm_sw_state.h>
#endif /* (BCM_PETRA_SUPPORT) */
#include <shared/swstate/sw_state_workarounds.h>
#include <shared/swstate/sw_state_def_init_indexes.h>
#include <shared/swstate/sw_state.h>
#include <soc/error.h>
#include <bcm/error.h>
#include <shared/swstate/sw_state_defs_issu_test.h>


#if !defined(BCM_WARM_BOOT_SUPPORT)
#include <shared/swstate/sw_state_no_wb_defs.h>
#else
/* if we do SOC_INIT_FUNC_DEFS it breaks some tests */
#define SW_STATE_INIT_FUNC_DEFS\
    int _rv = SOC_E_NONE

/* if we do SOC_FUNC_RETURN it breaks some tests */
#define SW_STATE_FUNC_RETURN\
    goto exit;\
exit:\
    return _rv

/* if we do SOC_FUNC_RETURN it breaks some tests */
#define SW_STATE_IF_ERR_EXIT(_rc)\
    _SOC_IF_ERR_EXIT(_rc)

/*
 * allow masking of some of the sw state autosync calls as specified by the specific set/get access functions.
 * in order to mask autosynv call for some of the state the sw state autocoder should be re-run in order to regenerate the access functions,
 * the following hint should be added to the struct that need to be excluded PARSER_HINT_AUTOSYNC_EXCLUDE
 * usage example: mask autosync for counter structures as they are being extensively accessed by counter thread.
 */
#define ALLOW_MASKING_AUTOSYNC_CALLS (1)

#define SW_STATE_DO_AUTOSYNC(access_function_permission) \
    (!ALLOW_MASKING_AUTOSYNC_CALLS || access_function_permission)

/* for now follow the autosync routine */
#define SW_STATE_DO_JOURNALING(unit)\
    (SOC_CR_ENABALED(unit))

/* 
 * define this flag to give away High Availability 
 * and SW state validations/diags : 
 * !!! SHR_THIN_SW_STATE !!! 
 * if defined: 
 * AutoSync warnboot will not be supported 
 * Journaling/Crash Recovery will not be supported 
 * Sanity access validations will not be supported 
 * diagnostic tools will not be supported 
 *  
 * this flag should be disabled in case these features are not needed
 * and performance is crutial to the system
 */
#if !defined(CRASH_RECOVERY_SUPPORT)
#if 1
#define SHR_THIN_SW_STATE 1
#endif
#endif

/* 
 * When this flag is defined some sanity Validations will be added to
 * the SW State Access function, this may introduce some performance decrease
 */


/* 
 * define this flag to enable autosync optimization (autosync only if modified)
 * use this empiricly to get better performance in autosync mode.
 * the optimization will only call external storage I/O callbacks if
 * state has changed by the "set" call.
 * it introduce a negligble overhead of checking if the set changes the state
 * but if application's storage is slow it may save a lot of time by not accessing
 * it in cases data wasn't modified 
 *  
 * #define SW_STATE_AUTOSYNC_OPTIMIZATION 
 */




/* sw state flags */
#define SW_STATE_NONE               (0)
#define SW_STATE_IS_WB_CHECK        (1 << 0)
#define SW_STATE_ALLOW_AUTOSYNC     (1 << 1)

#ifdef SHR_SW_STATE_VERIFICATIONS
#define SHR_SW_STATE_VERIFY(op)\
    SW_STATE_IF_ERR_EXIT(op)
#else
#define SHR_SW_STATE_VERIFY(op)
#endif

/**********************************************/
/*********** CORE ACCESS MACROS ***************/
/**********************************************/
#ifdef SHR_THIN_SW_STATE /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_SET(unit, location, input, type, flags, dbg_string)\
    location = input;
#define SHR_SW_STATE_MEMSET(unit, location, input, type, flags, dbg_string)\
    sal_memcpy(&location, input, sizeof(type));

#else /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_SET(unit, location, input, type, flags, dbg_string)\
    shr_sw_state_memcpy(unit, (uint8 *) &location, (uint8 *) &input, sizeof(type), flags);


/* the following macro is for catching valgrind issues of "variable.set(&value)" with uninitalized value */ 

#if 1
#define SW_STATE_VALGRIND_USE(location, type)
#endif

#define SHR_SW_STATE_MEMSET(unit, location, input, type, flags, dbg_string)\
    do { \
        shr_sw_state_memcpy(unit,(uint8 *) &location, (uint8 *) input, sizeof(type), flags); \
        SW_STATE_VALGRIND_USE(location, type); \
    } while(0)

#endif /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_ALLOC(unit, location, size, flags, dbg_string)\
    shr_sw_state_alloc(unit, (uint8**) &location, size, flags, dbg_string);
#define SHR_SW_STATE_FREE(unit, location, flags, dbg_string)\
    shr_sw_state_free(unit, (uint8*) location, flags);
#define SHR_SW_STATE_IS_ALLOC(unit, location, is_alloc, flags, dbg_string)\
    *is_alloc = (location) ? 1 : 0;

/**********************************************/
/**********************************************/
/**********************************************/




/**********************************************/
/******** SHR_BITDCL ACCESS MACROS ************/
/**********************************************/
/* we allocate one more byte to save the num of extra bit allocated */
#define SHR_SW_STATE_ALLOC_BITMAP(unit, location, nof_bits_to_alloc, flags, dbg_string)\
    shr_sw_state_alloc(unit, (uint8**) &location, SHR_BITALLOCSIZE(_nof_bits_to_alloc) + 1, flags, dbg_string);\
    shr_sw_state_shr_bitdcl_nof_bits_update(unit, (uint8 *)(location), (nof_bits_to_alloc), flags);

#ifdef SHR_THIN_SW_STATE /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_BIT_SET(_unit, _flags, _sw_state_bmp, __bit_num)\
    SHR_BITSET(_sw_state_bmp, __bit_num);
#define SHR_SW_STATE_BIT_CLEAR(_unit, _flags ,_sw_state_bmp, __bit_num)\
    SHR_BITCLR(_sw_state_bmp, __bit_num);
#define SHR_SW_STATE_BIT_GET(_unit, _flags, _sw_state_bmp, __bit_num , _bit_result)\
    *(_bit_result) = (SHR_BITGET(_sw_state_bmp, __bit_num) > 0) ? 1: 0;
#define SHR_SW_STATE_BIT_RANGE_READ(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, _result_first, __range, _result_bmp)\
    SHR_BITCOPY_RANGE(_result_bmp, result_first, _sw_state_bmp, _sw_state_bmp_first, __range);
#define SHR_SW_STATE_BIT_RANGE_WRITE(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, _input_bmp_first, __range, _input_bmp)\
    SHR_BITCOPY_RANGE(_sw_state_bmp, _sw_state_bmp_first, _input_bmp, _input_bmp_first, __range);
#define SHR_SW_STATE_BIT_RANGE_AND(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITAND_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define SHR_SW_STATE_BIT_RANGE_OR(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITOR_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define SHR_SW_STATE_BIT_RANGE_XOR(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITXOR_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define SHR_SW_STATE_BIT_RANGE_REMOVE(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITREMOVE_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define SHR_SW_STATE_BIT_RANGE_NEGATE(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    SHR_BITNEGATE_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define SHR_SW_STATE_BIT_RANGE_CLEAR(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    SHR_BITCLR_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count);
#define SHR_SW_STATE_BIT_RANGE_SET(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    SHR_BITSET_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count);
#define SHR_SW_STATE_BIT_RANGE_NULL(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    *(_bool_result) = (SHR_BITNULL_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count) > 0) ? 1: 0;
#define SHR_SW_STATE_BIT_RANGE_TEST(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    SHR_BITTEST_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count, *(_bool_result));
#define SHR_SW_STATE_BIT_RANGE_EQ(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _input_bmp, _bool_result)\
    *(_bool_result) = (SHR_BITEQ_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count) > 0) ? 1: 0;
#define SHR_SW_STATE_BIT_RANGE_COUNT(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result)\
    shr_bitop_range_count(_sw_state_bmp, _sw_state_bmp_first, __range, _int_result);

#else /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_BIT_SET(_unit, _flags, _sw_state_bmp, __bit_num)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_SET; param.sw_state_bmp = _sw_state_bmp; param._bit_num = __bit_num;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_CLEAR(_unit, _flags ,_sw_state_bmp, __bit_num)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_CLEAR;\
    param.sw_state_bmp = _sw_state_bmp; param._bit_num = __bit_num;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_GET(_unit, _flags, _sw_state_bmp, __bit_num , _bit_result)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_GET; param.sw_state_bmp = _sw_state_bmp;\
    param._bit_num = __bit_num; param.bit_result = _bit_result;\
     SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_READ(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, _result_first, __range, _result_bmp)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_READ;\
    param.sw_state_bmp = _sw_state_bmp; param.sw_state_bmp_first = _sw_state_bmp_first; param.result_first = _result_first;\
        param._range = __range; param.result_bmp = _result_bmp;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_WRITE(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, _input_bmp_first, __range, _input_bmp)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_WRITE; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param.input_bmp_first = _input_bmp_first; param._range = __range; param.input_bmp = _input_bmp;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_AND(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_AND;\
    param.sw_state_bmp = _sw_state_bmp; param.input_bmp = _input_bmp; param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_OR(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_OR;\
    param.sw_state_bmp = _sw_state_bmp; param.input_bmp = _input_bmp; param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_XOR(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_XOR;\
    param.sw_state_bmp = _sw_state_bmp; param.input_bmp = _input_bmp; param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_REMOVE(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_REMOVE;\
    param.sw_state_bmp = _sw_state_bmp; param.input_bmp = _input_bmp; param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_NEGATE(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_NEGATE; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_CLEAR(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_CLEAR; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_SET(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_SET; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_NULL(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_NULL; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count; param.bool_result = _bool_result;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_TEST(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_TEST; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count; param.bool_result = _bool_result;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_EQ(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _input_bmp, _bool_result)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_EQ; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __count; param.input_bmp = _input_bmp; param.bool_result = _bool_result;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)
#define SHR_SW_STATE_BIT_RANGE_COUNT(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result)\
do {\
    sw_state_bitdcl_param_t param;\
    param.flags = _flags; param.op = SW_ST_BIT_RANGE_COUNT; param.sw_state_bmp = _sw_state_bmp;\
    param.sw_state_bmp_first = _sw_state_bmp_first; param._range = __range; param.int_result = _int_result;\
    SHR_SW_STATE_BIT_OP(_unit, param);\
} while(0)

#endif /*SHR_THIN_SW_STATE*/

/**********************************************/
/**********************************************/
/**********************************************/




/**********************************************/
/****** SW_STATE_BUFFER ACCESS MACROS *********/
/**********************************************/
#define SW_STATE_MEMREAD(unit, dst, location, offset, length, flags, debug_str)\
    sal_memcpy((void *) dst, (void *) (location + offset), length);
#define SW_STATE_MEMCMP(unit, buffer, location, offset, length, output, flags, debug_str)\
    *output = sal_memcmp((void *) buffer, (void *) (location + offset), length);

/* can fall back into regular assignments for improved performance */
#ifdef SHR_THIN_SW_STATE /*SHR_THIN_SW_STATE*/

#define SW_STATE_MEMSET(unit, location, offset, value, length, flags, debug_str)\
    sal_memset((uint8 *) (location + offset), value, length);
#define SW_STATE_MEMWRITE(unit, input, location, offset, length, flags, debug_str)\
    sal_memcpy((uint8 *) (location + offset), (uint8 *) input, length);

#else /*SHR_THIN_SW_STATE*/

#define SW_STATE_MEMSET(unit, location, offset, value, length, flags, debug_str)\
    shr_sw_state_memset(unit,(uint8 *) (location + offset), value, length, flags);
#define SW_STATE_MEMWRITE(unit, input, location, offset, length, flags, debug_str)\
    shr_sw_state_memcpy(unit,(uint8 *) (location + offset), (uint8 *) input, length, flags);

#endif /*SHR_THIN_SW_STATE*/



/**********************************************/
/********** PBMP ACCESS MACROS ****************/
/**********************************************/
/* can fall back into regular assignments for improved performance */
#ifdef SHR_THIN_SW_STATE /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_PBMP_NEQ(unit, path, input_pbmp, result, flags, debug_string)\
    *result = (_SHR_PBMP_NEQ(path, input_pbmp) > 0) ? 1: 0;
#define SHR_SW_STATE_PBMP_EQ(unit, path, input_pbmp, result, flags, debug_string)\
    *result = (_SHR_PBMP_EQ(path, input_pbmp) > 0) ? 1: 0;
#define SHR_SW_STATE_PBMP_MEMBER(unit, path, input_port, result, flags, debug_string)\
    *result = (_SHR_PBMP_MEMBER(path, input_port) > 0) ? 1: 0;
#define SHR_SW_STATE_PBMP_NOT_NULL(unit, path, result, flags, debug_string)\
    *result = (_SHR_PBMP_NOT_NULL(path) > 0) ? 1: 0;
#define SHR_SW_STATE_PBMP_IS_NULL(unit, path, result, flags, debug_string)\
    *result = (_SHR_PBMP_IS_NULL(path) > 0) ? 1: 0;
#define SHR_SW_STATE_PBMP_COUNT(unit, path, result, flags, debug_string)\
    _SHR_PBMP_COUNT(path, *result);
#define SHR_SW_STATE_PBMP_XOR(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_XOR(path, input_pbmp);
#define SHR_SW_STATE_PBMP_REMOVE(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_REMOVE(path, input_pbmp);
#define SHR_SW_STATE_PBMP_ASSIGN(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_ASSIGN(path, input_pbmp);
#define SHR_SW_STATE_PBMP_GET(unit, path, output_pbmp, flags, debug_string)\
    _SHR_PBMP_ASSIGN(*output_pbmp, path);
#define SHR_SW_STATE_PBMP_AND(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_AND(path, input_pbmp);
#define SHR_SW_STATE_PBMP_NEGATE(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_NEGATE(path, input_pbmp);
#define SHR_SW_STATE_PBMP_OR(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_OR(path, input_pbmp);
#define SHR_SW_STATE_PBMP_CLEAR(unit, path, flags, debug_string)\
    _SHR_PBMP_CLEAR(path);
#define SHR_SW_STATE_PBMP_PORT_ADD(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_ADD(path, input_port);
#define SHR_SW_STATE_PBMP_PORT_FLIP(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_FLIP(path, input_port);
#define SHR_SW_STATE_PBMP_PORT_REMOVE(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_REMOVE(path, input_port);
#define SHR_SW_STATE_PBMP_PORT_SET(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_SET(path, input_port);
#define SHR_SW_STATE_PBMP_PORTS_RANGE_ADD(unit, path, input_port, range, flags, debug_string)\
    _SHR_PBMP_PORTS_RANGE_ADD(path, input_port, range);
#define SHR_SW_STATE_PBMP_FMT(unit, path, buffer, flags, debug_string)\
    _SHR_PBMP_FMT(path, buffer);

#else /*SHR_THIN_SW_STATE*/

#define SHR_SW_STATE_PBMP_NEQ(unit, path, input_pbmp, result, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_NEQ, &input_pbmp, NULL, -1, -1, result, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_EQ(unit, path, input_pbmp, result, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_EQ, &input_pbmp, NULL, -1, -1, result, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_MEMBER(unit, path, input_port, result, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_MEMBER, NULL, NULL, input_port, -1, result, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_NOT_NULL(unit, path, result, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_NOT_NULL, NULL, NULL, -1, -1, result, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_IS_NULL(unit, path, result, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_IS_NULL, NULL, NULL, -1, -1, result, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_COUNT(unit, path, result, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_COUNT, NULL, NULL, -1, -1, NULL, result, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_XOR(unit, path, input_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_XOR, &input_pbmp, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_REMOVE(unit, path, input_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_REMOVE, &input_pbmp, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_ASSIGN(unit, path, input_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_ASSIGN, &input_pbmp, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_GET(unit, path, output_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_GET, NULL, output_pbmp, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_AND(unit, path, input_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_AND, &input_pbmp, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_NEGATE(unit, path, input_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_NEGATE, &input_pbmp, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_OR(unit, path, input_pbmp, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_OR, &input_pbmp, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_CLEAR(unit, path, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_CLEAR, NULL, NULL, -1, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_PORT_ADD(unit, path, input_port, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_PORT_ADD, NULL, NULL, input_port, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_PORT_FLIP(unit, path, input_port, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_PORT_FLIP, NULL, NULL, input_port, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_PORT_REMOVE(unit, path, input_port, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_PORT_REMOVE, NULL, NULL, input_port, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_PORT_SET(unit, path, input_port, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_PORT_SET, NULL, NULL, input_port, -1, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_PORTS_RANGE_ADD(unit, path, input_port, range, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_PORTS_RANGE_ADD, NULL, NULL, input_port, range, NULL, NULL, NULL, flags, debug_string)
#define SHR_SW_STATE_PBMP_FMT(unit, path, buffer, flags, debug_string)\
    shr_sw_state_pbmp_operation(unit, &path, SW_ST_PBMP_FMT, NULL, NULL, -1, -1, NULL, NULL, buffer, flags, debug_string)

#endif /*SHR_THIN_SW_STATE*/

/**********************************************/
/**********************************************/
/**********************************************/



#ifdef SHR_SW_STATE_VERIFICATIONS
#define SW_STATE_NULL_CHECK(param) \
    do{ \
        if((param) == NULL){ \
            _sw_state_access_debug_hook(0);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: trying to free something that was never allocated\n")));\
            /* for now - just print to screen; in the future - consider returnning error */ \
            return _SHR_E_NONE; \
            } \
    } while(0)

#define SW_STATE_IS_WARM_BOOT_CHECK() \
    do{ \
        if (SOC_WARM_BOOT(unit)) {\
            _sw_state_access_debug_hook(1);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: trying to set/alloc during warm reboot. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_DISABLED;\
        }\
    } while(0)

#define SW_STATE_IS_DEINIT_CHECK() \
    do{ \
        if (SOC_IS_DETACHING(unit)) {\
            _sw_state_access_debug_hook(2);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: trying to free/modify during deinit. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_DISABLED;\
        }\
    } while(0)

#define SW_STATE_IS_ALREADY_ALLOCATED_CHECK(__ptr) \
    do{ \
        if (__ptr) {\
            _sw_state_access_debug_hook(3);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: trying to allocate something that is already allocated. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_EXISTS;\
        }\
    } while(0)

#define SW_STATE_OUT_OF_BOUND_CHECK(__index, __max, __param) \
    do{ \
        if (__index > __max) {\
            _sw_state_access_debug_hook(4);\
			LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
                "\nSW STATE ERROR: INDEX OUT OF BOUND.\n\
                Trying to access "#__param" with index %d out of %d.\n\
                For debug, use breakpoint on _sw_state_access_debug_hook\n"),\
                (int)(__index), (int)(__max)));\
            return _SHR_E_PARAM;\
        }\
    } while(0)


#else /*SHR_SW_STATE_VERIFICATIONS*/
#define SW_STATE_NULL_CHECK(param)
#define SW_STATE_IS_WARM_BOOT_CHECK()
#define SW_STATE_IS_DEINIT_CHECK()
#define SW_STATE_IS_ALREADY_ALLOCATED_CHECK(__ptr)
#define SW_STATE_OUT_OF_BOUND_CHECK(__index, __max, __param)
#endif /*SHR_SW_STATE_VERIFICATIONS*/

#define SW_STATE_MODIFY_OPS_CHECKS(flags)\
    if (flags & SW_STATE_IS_WB_CHECK) {\
        SW_STATE_IS_WARM_BOOT_CHECK();\
    }\
    SW_STATE_IS_DEINIT_CHECK();

/* there is another null check above SW_STATE_NULL_CHECK for free purpose
    this null check is for use purpose */

#define SW_STATE_NULL_ACCESS_CHECK(param) \
    do{ \
        if((param) == NULL){ \
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "\nsw state ERROR: NULL REFERENCE \n")));\
            return _SHR_E_MEMORY; \
        } \
    } while(0)

#ifdef SHR_SW_STATE_VERIFICATIONS
#define SW_STATE_BITDCL_VERIFY(parent_verify_call, ptr, from_bit, to_bit, param_name) \
    do{	\
        uint32 nof_allocated_bits = 0; \
        _rv = parent_verify_call; \
        if (_rv != _SHR_E_NONE) return _rv; \
        SW_STATE_NULL_ACCESS_CHECK(ptr); \
        shr_sw_state_shr_bitdcl_nof_bits_get(unit, (uint8*)ptr, &nof_allocated_bits); \
        SW_STATE_OUT_OF_BOUND_CHECK(from_bit, nof_allocated_bits, param_name); \
        SW_STATE_OUT_OF_BOUND_CHECK(to_bit  , nof_allocated_bits, param_name); \
    } while(0)
#else
#define SW_STATE_BITDCL_VERIFY(parent_verify_call, ptr, from_bit, to_bit, param_name)
#endif

#endif /* BCM_WARM_BOOT_SUPPORT */
				
#ifdef BCM_PETRA_SUPPORT
/* { */
typedef struct soc_dpp_sw_state_s {
    soc_dpp_bcm_sw_state_t  bcm;
    soc_dpp_soc_sw_state_t  soc;
    soc_dpp_shr_sw_state_t  shr;
} soc_dpp_sw_state_t;
/* } */
#endif

/*
 * shr_sw_state_t
 *   This is the highest level view of memory managed by 'sw state'
 *   it is split into two sections: DNX (JR2) and DPP (pre JR2 - PETRA)
 */
typedef struct shr_sw_state_s {
#ifdef BCM_PETRA_SUPPORT
/* { */
    soc_dpp_sw_state_t        dpp;
/* } */
#endif
#ifdef _SHR_SW_STATE_EXM
    PARSER_HINT_PTR two_levels_example_t      *exm;
    PARSER_HINT_PTR mix_stat_dyn_example_t    *exmMixStatDyn; 
    PARSER_HINT_PTR mini_example_t            *miniExm;
    PARSER_HINT_PTR sw_state_issu_example_t   *issu_example;
#endif
    PARSER_HINT_PTR sw_state_issu_test_t      *issu_test;
} shr_sw_state_t;
/* } */

#endif /* _SHR_SW_STATE_DEFS_H */

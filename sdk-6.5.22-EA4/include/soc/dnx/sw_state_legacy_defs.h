
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *
 * INFO: here we have the internal implementation for the ACCESS layer MACROs.
 *       this allow changing the behaviour of the infrastructure without changing the autocoder
 *       and the autogenerated access code
 */
#ifndef _LEGACY_SW_STATE_DEFS_H

#define _LEGACY_SW_STATE_DEFS_H

#include <shared/swstate/sw_state_includes.h>
#include <soc/dnx/dbal/dbal_structures.h>

#include <soc/types.h>
#include <sal/core/sync.h>
#include <sal/core/alloc.h>
#include <soc/error.h>
#include <assert.h>
#include <soc/drv.h>

#define LEGACY_SW_STATE_INIT_FUNC_DEFS\
    int _rv = SOC_E_NONE

#define LEGACY_SW_STATE_FUNC_RETURN\
    goto exit;\
exit:\
    return _rv

#define LEGACY_SW_STATE_IF_ERR_EXIT(_rc)\
    _SOC_IF_ERR_EXIT(_rc)

#define LEGACY_SW_STATE_NONE                       (0)
#define LEGACY_SW_STATE_IS_WB_CHECK                (1 << 0)
#define LEGACY_SW_STATE_ALLOW_AUTOSYNC             (1 << 1)
#define LEGACY_SW_STATE_DNX_DATA_ALLOC             (1 << 2)
#define LEGACY_SW_STATE_DNX_DATA_ALLOC_EXCEPTION   (1 << 3)
#define LEGACY_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION   (1 << 4)

#ifdef LEGACY_SW_STATE_VERIFICATIONS
#define LEGACY_SW_STATE_VERIFY(op)\
    LEGACY_SW_STATE_IF_ERR_EXIT(op)
#else
#define LEGACY_SW_STATE_VERIFY(op)
#endif

typedef uint8 LEGACY_SW_STATE_BUFF;

#define LEGACY_SW_STATE_SET(unit, location, input, type, flags, dbg_string)\
    location = input;

#define LEGACY_SW_STATE_MEMCPY_BASIC(unit, destination, source, size, flags, dbg_string) \
    sal_memcpy((destination), (source), (size));
#define LEGACY_SW_STATE_MEMCPY(unit, location, input, type, flags, dbg_string)\
    LEGACY_SW_STATE_MEMCPY_BASIC(unit, &location, input, sizeof(type), flags, dbg_string);
#define LEGACY_SW_STATE_RANGE_COPY(unit, location, input, type, num_of_elements, flags, dbg_string) \
    LEGACY_SW_STATE_MEMCPY_BASIC(unit, location, input, num_of_elements * sizeof(type), flags, dbg_string);
#define LEGACY_SW_STATE_ALLOC(unit, location, size, flags, dbg_string)\
    if(dnxc_sw_state_alloc_during_test_get(unit) || !(SOC_IS_DONE_INIT(unit)) || (flags & LEGACY_SW_STATE_ALLOC_AFTER_INIT_EXCEPTION ))\
    {\
        location = sal_alloc(size, dbg_string);\
        sal_memset(location, 0, size);\
    }else{\
                LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: trying allocate data after init \n")));\
                return _SHR_E_INTERNAL;\
    }
#define LEGACY_SW_STATE_FREE(unit, location, flags, dbg_string)\
    do {\
        if (location != NULL)\
            {\
                sal_free((uint8 *)location);\
                location = NULL;\
            }\
    } while(0)
#define LEGACY_SW_STATE_IS_ALLOC(unit, location, is_alloc, flags, dbg_string)\
    *is_alloc = (location) ? 1 : 0;
#define LEGACY_SW_STATE_VALID_VALUE_RANGE(unit, input, min, max)\
        do{\
            if(input < min || input > max)\
            {\
                LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: trying allocate date after init \n")));\
                return _SHR_E_INTERNAL;\
            }\
        } while(0)

#define LEGACY_SW_STATE_ALLOC_BITMAP(unit, location, nof_bits_to_alloc, flags, dbg_string)\
    LEGACY_SW_STATE_ALLOC(unit, location, SHR_BITALLOCSIZE(_nof_bits_to_alloc) + 1, flags, dbg_string);\

#define LEGACY_SW_STATE_BIT_SET(_unit, _flags, _sw_state_bmp, __bit_num)\
    SHR_BITSET(_sw_state_bmp, _bit_num);
#define LEGACY_SW_STATE_BIT_CLEAR(_unit, _flags ,_sw_state_bmp, __bit_num)\
    SHR_BITCLR(_sw_state_bmp, _bit_num);
#define LEGACY_SW_STATE_BIT_GET(_unit, _flags, _sw_state_bmp, __bit_num , _bit_result)\
    *(_bit_result) = (SHR_BITGET(_sw_state_bmp, _bit_num) > 0) ? 1: 0;
#define LEGACY_SW_STATE_BIT_RANGE_READ(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, _result_first, __range, _result_bmp)\
    SHR_BITCOPY_RANGE(_result_bmp, result_first, _sw_state_bmp, _sw_state_bmp_first, __range);
#define LEGACY_SW_STATE_BIT_RANGE_WRITE(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, _input_bmp_first, __range, _input_bmp)\
    SHR_BITCOPY_RANGE(_sw_state_bmp, _sw_state_bmp_first, _input_bmp, _input_bmp_first, __range);
#define LEGACY_SW_STATE_BIT_RANGE_AND(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITAND_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define LEGACY_SW_STATE_BIT_RANGE_OR(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITOR_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define LEGACY_SW_STATE_BIT_RANGE_XOR(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITXOR_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define LEGACY_SW_STATE_BIT_RANGE_REMOVE(_unit, _flags, _sw_state_bmp, _input_bmp, _sw_state_bmp_first, __count)\
    SHR_BITREMOVE_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define LEGACY_SW_STATE_BIT_RANGE_NEGATE(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    SHR_BITNEGATE_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count, _sw_state_bmp);
#define LEGACY_SW_STATE_BIT_RANGE_CLEAR(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    SHR_BITCLR_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count);
#define LEGACY_SW_STATE_BIT_RANGE_SET(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count)\
    SHR_BITSET_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count);
#define LEGACY_SW_STATE_BIT_RANGE_NULL(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    *(_bool_result) = (SHR_BITNULL_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count) > 0) ? 1: 0;
#define LEGACY_SW_STATE_BIT_RANGE_TEST(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _bool_result)\
    SHR_BITTEST_RANGE(_sw_state_bmp, _sw_state_bmp_first, __count, *(_bool_result));
#define LEGACY_SW_STATE_BIT_RANGE_EQ(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __count, _input_bmp, _bool_result)\
    *(_bool_result) = (SHR_BITEQ_RANGE(_input_bmp, _sw_state_bmp, _sw_state_bmp_first, __count) > 0) ? 1: 0;
#define LEGACY_SW_STATE_BIT_RANGE_COUNT(_unit, _flags, _sw_state_bmp, _sw_state_bmp_first, __range, _int_result)\
    shr_bitop_range_count(_sw_state_bmp, _sw_state_bmp_first, __range, _int_result);

#define LEGACY_SW_STATE_MEMREAD(unit, dst, location, offset, length, flags, debug_str)\
    sal_memcpy((void *) dst, (void *) (location + offset), length);
#define LEGACY_SW_STATE_MEMCMP(unit, buffer, location, offset, length, output, flags, debug_str)\
    *output = sal_memcmp((void *) buffer, (void *) (location + offset), length);
#define LEGACY_SW_STATE_MEMSET(unit, location, input, type, flags, dbg_string)\
    sal_memcpy(&location, input, sizeof(type));
#define LEGACY_SW_STATE_BUFFER_MEMSET(unit, location, offset, value, length, flags, debug_str)\
    sal_memset((uint8 *) (location + offset), value, length);
#define LEGACY_SW_STATE_MEMWRITE(unit, input, location, offset, length, flags, debug_str)\
    sal_memcpy((uint8 *) (location + offset), (uint8 *) input, length);

#define LEGACY_SW_STATE_PBMP_NEQ(unit, path, input_pbmp, result, flags, debug_string)\
    *result = (_SHR_PBMP_NEQ(path, input_pbmp) > 0) ? 1: 0;
#define LEGACY_SW_STATE_PBMP_EQ(unit, path, input_pbmp, result, flags, debug_string)\
    *result = (_SHR_PBMP_EQ(path, input_pbmp) > 0) ? 1: 0;
#define LEGACY_SW_STATE_PBMP_MEMBER(unit, path, input_port, result, flags, debug_string)\
    *result = (_SHR_PBMP_MEMBER(path, input_port) > 0) ? 1: 0;
#define LEGACY_SW_STATE_PBMP_NOT_NULL(unit, path, result, flags, debug_string)\
    *result = (_SHR_PBMP_NOT_NULL(path) > 0) ? 1: 0;
#define LEGACY_SW_STATE_PBMP_IS_NULL(unit, path, result, flags, debug_string)\
    *result = (_SHR_PBMP_IS_NULL(path) > 0) ? 1: 0;
#define LEGACY_SW_STATE_PBMP_COUNT(unit, path, result, flags, debug_string)\
    _SHR_PBMP_COUNT(path, *result);
#define LEGACY_SW_STATE_PBMP_XOR(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_XOR(path, input_pbmp);
#define LEGACY_SW_STATE_PBMP_REMOVE(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_REMOVE(path, input_pbmp);
#define LEGACY_SW_STATE_PBMP_ASSIGN(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_ASSIGN(path, input_pbmp);
#define LEGACY_SW_STATE_PBMP_GET(unit, path, output_pbmp, flags, debug_string)\
    _SHR_PBMP_ASSIGN(*output_pbmp, path);
#define LEGACY_SW_STATE_PBMP_AND(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_AND(path, input_pbmp);
#define LEGACY_SW_STATE_PBMP_NEGATE(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_NEGATE(path, input_pbmp);
#define LEGACY_SW_STATE_PBMP_OR(unit, path, input_pbmp, flags, debug_string)\
    _SHR_PBMP_OR(path, input_pbmp);
#define LEGACY_SW_STATE_PBMP_CLEAR(unit, path, flags, debug_string)\
    _SHR_PBMP_CLEAR(path);
#define LEGACY_SW_STATE_PBMP_PORT_ADD(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_ADD(path, input_port);
#define LEGACY_SW_STATE_PBMP_PORT_FLIP(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_FLIP(path, input_port);
#define LEGACY_SW_STATE_PBMP_PORT_REMOVE(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_REMOVE(path, input_port);
#define LEGACY_SW_STATE_PBMP_PORT_SET(unit, path, input_port, flags, debug_string)\
    _SHR_PBMP_PORT_SET(path, input_port);
#define LEGACY_SW_STATE_PBMP_PORTS_RANGE_ADD(unit, path, input_port, range, flags, debug_string)\
    _SHR_PBMP_PORTS_RANGE_ADD(path, input_port, range);
#define LEGACY_SW_STATE_PBMP_FMT(unit, path, buffer, flags, debug_string)\
    _SHR_PBMP_FMT(path, buffer);

#ifdef LEGACY_SW_STATE_VERIFICATIONS
#define LEGACY_SW_STATE_NULL_CHECK(param) \
    do{ \
        if((param) == NULL){ \
            _sw_state_access_debug_hook(0);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "sw state ERROR: trying to free something that was never allocated\n")));\
             \
            return _SHR_E_NONE; \
            } \
    } while(0)

#define LEGACY_SW_STATE_IS_WARM_BOOT_CHECK() \
    do{ \
        if (SOC_WARM_BOOT(unit)) {\
            _sw_state_access_debug_hook(1);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: trying to set/alloc during warm reboot. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_DISABLED;\
        }\
    } while(0)

#define LEGACY_SW_STATE_IS_DEINIT_CHECK() \
    do{ \
        if (SOC_IS_DETACHING(unit)) {\
            _sw_state_access_debug_hook(2);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: trying to free/modify during deinit. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_DISABLED;\
        }\
    } while(0)

#define LEGACY_SW_STATE_IS_ALREADY_ALLOCATED_CHECK(__ptr) \
    do{ \
        if (__ptr) {\
            _sw_state_access_debug_hook(3);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit,\
            "sw state ERROR: trying to allocate something that is already allocated. for debug, use breakpoint on _sw_state_access_debug_hook\n")));\
            return _SHR_E_EXISTS;\
        }\
    } while(0)

#define LEGACY_SW_STATE_OUT_OF_BOUND_CHECK(__index, __max, __param) \
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

#else
#define LEGACY_SW_STATE_NULL_CHECK(param)
#define LEGACY_SW_STATE_IS_WARM_BOOT_CHECK()
#define LEGACY_SW_STATE_IS_DEINIT_CHECK()
#define LEGACY_SW_STATE_IS_ALREADY_ALLOCATED_CHECK(__ptr)
#define LEGACY_SW_STATE_OUT_OF_BOUND_CHECK(__index, __max, __param)
#endif

#define LEGACY_SW_STATE_MODIFY_OPS_CHECKS(flags)\
    if (flags & LEGACY_SW_STATE_IS_WB_CHECK) {\
        LEGACY_SW_STATE_IS_WARM_BOOT_CHECK();\
    }\
    LEGACY_SW_STATE_IS_DEINIT_CHECK();

#define LEGACY_SW_STATE_NULL_ACCESS_CHECK(param) \
    do{ \
        if((param) == NULL){ \
            _sw_state_access_debug_hook(5);\
            LOG_ERROR(BSL_LS_SHARED_SWSTATE, (BSL_META_U(unit, "\nsw state ERROR: NULL REFERENCE \n")));\
            return _SHR_E_MEMORY; \
        } \
    } while(0)

#ifdef LEGACY_SW_STATE_VERIFICATIONS
#define LEGACY_SW_STATE_BITDCL_VERIFY(parent_verify_call, ptr, from_bit, to_bit, param_name) \
    do{    \
        uint32 nof_allocated_bits = 0; \
        _rv = parent_verify_call; \
        if (_rv != _SHR_E_NONE) return _rv; \
        LEGACY_SW_STATE_NULL_ACCESS_CHECK(ptr); \
        shr_sw_state_shr_bitdcl_nof_bits_get(unit, (uint8*)ptr, &nof_allocated_bits); \
        LEGACY_SW_STATE_OUT_OF_BOUND_CHECK(from_bit, nof_allocated_bits, param_name); \
        LEGACY_SW_STATE_OUT_OF_BOUND_CHECK(to_bit  , nof_allocated_bits, param_name); \
    } while(0)
#else
#define LEGACY_SW_STATE_BITDCL_VERIFY(parent_verify_call, ptr, from_bit, to_bit, param_name)
#endif
#if (defined(BCM_DNX_SUPPORT))

typedef struct
{
    sw_state_res_tag_bitmap_info_t res_tag_bmp_info;
} shareddnx_sw_state_legacy_t;

#endif

typedef struct
{
    int dummy;
} shr_sw_state_legacy_t;

#endif

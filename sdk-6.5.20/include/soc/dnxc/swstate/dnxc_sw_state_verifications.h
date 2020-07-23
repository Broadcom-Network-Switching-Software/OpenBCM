
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _DNXC_SW_STATE_VERIFICATIONS_H

#define _DNXC_SW_STATE_VERIFICATIONS_H

#include <soc/dnxc/swstate/sw_state_features.h>

#include <soc/types.h>
#include <soc/error.h>
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_verify.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#ifdef DNX_SW_STATE_VERIFICATIONS
#define DNXC_SW_STATE_VERIFICATIONS_ONLY(op)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        op;\
    }
#else
#define DNXC_SW_STATE_VERIFICATIONS_ONLY(op)
#endif

#ifdef DNX_SW_STATE_VERIFICATIONS

#define DNX_SW_STATE_VERIFICATIONS_ASSERT

#ifdef DNX_SW_STATE_VERIFICATIONS_ASSERT

#define DNXC_SW_STATE_IF_ERR_WITH_LOG_COMMON(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)\
do {                                                                                                     \
    int _r_rv = (_expr);                                                                                 \
    if(SHR_FAILURE(_r_rv))                                                                               \
    {                                                                                                    \
        LOG_ERROR_EX(BSL_LOG_MODULE,                                                                     \
                     _formatting_string,                                                                 \
                     shrextend_errmsg_get(_r_rv) ,_param2,_param3,_param4);                              \
        if((_is_assert) && !dnxc_sw_state_err_assert_is_disabled(unit)) {                                \
            dnxc_sw_state_err_assert_fail_debug_hook(unit);                                              \
            assert(0);                                                                                   \
        } else {                                                                                         \
            _func_rv = _r_rv;                                                                            \
            SHR_EXIT() ;                                                                                 \
        }                                                                                                \
    }                                                                                                    \
} while(0)

#define DNXC_SW_STATE_IF_ERR_INTERNAL(_is_assert, _expr)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_COMMON(_is_assert,_expr," Error '%s' indicated ; %s%s%s\r\n",EMPTY,EMPTY,EMPTY)

#define DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_COMMON(_is_assert,_expr," Error: %s ; " _formatting_string,_param2,_param3,_param4)

#else
#define DNXC_SW_STATE_IF_ERR_INTERNAL(_is_assert, _expr)\
    SHR_IF_ERR_EXIT(_expr)

#define DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)\
    SHR_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)
#endif

#else
#define DNXC_SW_STATE_IF_ERR_INTERNAL(_is_assert, _expr)
#define DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(_is_assert,_expr,_formatting_string,_param2,_param3,_param4)
#endif

#define DNXC_SW_STATE_IF_ERR_ASSERT(_expr)\
    DNXC_SW_STATE_IF_ERR_INTERNAL(TRUE, _expr)

#define DNXC_SW_STATE_IF_ERR_EXIT(_expr)\
    DNXC_SW_STATE_IF_ERR_INTERNAL(FALSE, _expr)

#define DNXC_SW_STATE_IF_ERR_ASSERT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(TRUE, _expr,_formatting_string,_param2,_param3,_param4)

#define DNXC_SW_STATE_IF_ERR_EXIT_WITH_LOG(_expr,_formatting_string,_param2,_param3,_param4)\
    DNXC_SW_STATE_IF_ERR_WITH_LOG_INTERNAL(FALSE, _expr,_formatting_string,_param2,_param3,_param4)

#ifdef DNX_SW_STATE_VERIFICATIONS
typedef enum
{
    SW_STATE_FUNC_VERIFY,
    SW_STATE_FUNC_CREATE,
    SW_STATE_FUNC_IS_CREATED,
    SW_STATE_FUNC_DESTROY,
    SW_STATE_FUNC_GET_NEXT,
    SW_STATE_FUNC_GET_NEXT_IN_RANGE,
    SW_STATE_FUNC_STATUS_SET,
    SW_STATE_FUNC_IS_OCCUPIED,
    SW_STATE_FUNC_ALLOC_NEXT,
    SW_STATE_FUNC_CLEAR,
    SW_STATE_FUNC_PRINT,
    SW_STATE_FUNC_FIND,
    SW_STATE_FUNC_INSERT,
    SW_STATE_FUNC_DELETE,
    SW_STATE_FUNC_DELETE_ALL,
    SW_STATE_FUNC_INSERT_AT_INDEX,
    SW_STATE_FUNC_DELETE_BY_INDEX,
    SW_STATE_FUNC_GET_BY_INDEX,
    SW_STATE_FUNC_ITERATE,
    SW_STATE_FUNC_CREATE_EMPTY,
    SW_STATE_FUNC_NOF_ELEMENTS,
    SW_STATE_FUNC_NODE_KEY,
    SW_STATE_FUNC_NODE_VALUE,
    SW_STATE_FUNC_NODE_UPDATE,
    SW_STATE_FUNC_NEXT_NODE,
    SW_STATE_FUNC_PREVIOUS_NODE,
    SW_STATE_FUNC_ADD,
    SW_STATE_FUNC_ADD_FIRST,
    SW_STATE_FUNC_ADD_LAST,
    SW_STATE_FUNC_ADD_BEFORE,
    SW_STATE_FUNC_ADD_AFTER,
    SW_STATE_FUNC_ADD_AT_INDEX,
    SW_STATE_FUNC_ADD_AT_INDEX_MULTIPLE,
    SW_STATE_FUNC_REMOVE,
    SW_STATE_FUNC_REMOVE_BY_INDEX,
    SW_STATE_FUNC_REMOVE_BY_INDEX_MULTIPLE,
    SW_STATE_FUNC_REMOVE_NODE,
    SW_STATE_FUNC_GET_LAST,
    SW_STATE_FUNC_GET_FIRST,
    SW_STATE_FUNC_GET_ROOT,
    SW_STATE_FUNC_GET_PARENT,
    SW_STATE_FUNC_GET_LEFT_CHILD,
    SW_STATE_FUNC_GET_RIGHT_CHILD,
    SW_STATE_FUNC_ADD_ROOT,
    SW_STATE_FUNC_ADD_LEFT_CHILD,
    SW_STATE_FUNC_ADD_RIGHT_CHILD,
    SW_STATE_FUNC_SET_PARENT,
    SW_STATE_FUNC_SET_LEFT_CHILD,
    SW_STATE_FUNC_SET_RIGHT_CHILD,
    SW_STATE_FUNC_NODE_FREE,
    SW_STATE_FUNC_TAKE,
    SW_STATE_FUNC_GIVE,
    SW_STATE_FUNC_ALLOC,
    SW_STATE_FUNC_PTR_ALLOC,
    SW_STATE_FUNC_FREE,
    SW_STATE_FUNC_PTR_FREE,
    SW_STATE_FUNC_IS_ALLOCATED,
    SW_STATE_FUNC_SET,
    SW_STATE_FUNC_GET,
    SW_STATE_FUNC_INC,
    SW_STATE_FUNC_DEC,
    SW_STATE_FUNC_RANGE_READ,
    SW_STATE_FUNC_RANGE_WRITE,
    SW_STATE_FUNC_RANGE_FILL,
    SW_STATE_FUNC_ALLOC_BITMAP,
    SW_STATE_FUNC_BIT_SET,
    SW_STATE_FUNC_BIT_CLEAR,
    SW_STATE_FUNC_BIT_GET,
    SW_STATE_FUNC_BIT_RANGE_READ,
    SW_STATE_FUNC_BIT_RANGE_WRITE,
    SW_STATE_FUNC_BIT_RANGE_AND,
    SW_STATE_FUNC_BIT_RANGE_OR,
    SW_STATE_FUNC_BIT_RANGE_XOR,
    SW_STATE_FUNC_BIT_RANGE_REMOVE,
    SW_STATE_FUNC_BIT_RANGE_NEGATE,
    SW_STATE_FUNC_BIT_RANGE_CLEAR,
    SW_STATE_FUNC_BIT_RANGE_SET,
    SW_STATE_FUNC_BIT_RANGE_NULL,
    SW_STATE_FUNC_BIT_RANGE_TEST,
    SW_STATE_FUNC_BIT_RANGE_EQ,
    SW_STATE_FUNC_BIT_RANGE_COUNT,
    SW_STATE_FUNC_MEMREAD,
    SW_STATE_FUNC_MEMWRITE,
    SW_STATE_FUNC_MEMCMP,
    SW_STATE_FUNC_MEMSET,
    SW_STATE_FUNC_PBMP_NEQ,
    SW_STATE_FUNC_PBMP_EQ,
    SW_STATE_FUNC_PBMP_MEMBER,
    SW_STATE_FUNC_PBMP_NOT_NULL,
    SW_STATE_FUNC_PBMP_IS_NULL,
    SW_STATE_FUNC_PBMP_COUNT,
    SW_STATE_FUNC_PBMP_XOR,
    SW_STATE_FUNC_PBMP_REMOVE,
    SW_STATE_FUNC_PBMP_ASSIGN,
    SW_STATE_FUNC_PBMP_GET,
    SW_STATE_FUNC_PBMP_AND,
    SW_STATE_FUNC_PBMP_NEGATE,
    SW_STATE_FUNC_PBMP_OR,
    SW_STATE_FUNC_PBMP_CLEAR,
    SW_STATE_FUNC_PBMP_PORT_ADD,
    SW_STATE_FUNC_PBMP_PORT_FLIP,
    SW_STATE_FUNC_PBMP_PORT_REMOVE,
    SW_STATE_FUNC_PBMP_PORT_SET,
    SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
    SW_STATE_FUNC_PBMP_FMT,
    SW_STATE_FUNC_DUMP,
    SW_STATE_FUNC_INIT,
    SW_STATE_FUNC_DEINIT,
    SW_STATE_FUNC_IS_INIT,
    SW_STATE_FUNC_GET_NAME,
    SW_STATE_FUNC_GET_CB,
    SW_STATE_FUNC_REGISTER_CB,
    SW_STATE_FUNC_UNREGISTER_CB,
    SW_STATE_FUNC_PIECE_ALLOC,
    SW_STATE_FUNC_PIECE_FREE,
    SW_STATE_FUNC_CHECK_ALL,
    SW_STATE_FUNC_CHECK_ALL_TAG,
    SW_STATE_FUNC_CHECK,
    SW_STATE_FUNC_TAG_GET,
    SW_STATE_FUNC_TAG_SET,
    SW_STATE_FUNC_NOF_FREE_ELEMENTS_GET,
    SW_STATE_FUNC_NOF_USED_ELEMENTS_IN_GRAIN_GET,
    SW_STATE_FUNC_ALLOCATE_SINGLE,
    SW_STATE_FUNC_FREE_SINGLE,
    SW_STATE_FUNC_ALLOCATE_SEVERAL,
    SW_STATE_FUNC_FREE_SEVERAL,
    SW_STATE_FUNC_DUMP_INFO_GET,
    SW_STATE_FUNC_CLEAR_ALL,
    SW_STATE_FUNC_BITMAP_ALLOC,
    SW_STATE_FUNC_BITMAP_FREE,
    SW_STATE_FUNC_EXCHANGE,
    SW_STATE_FUNC_REPLACE_DATA,
    SW_STATE_FUNC_PROFILE_DATA_GET,
    SW_STATE_FUNC_PROFILE_GET,
    SW_STATE_FUNC_FREE_ALL,
    SW_STATE_FUNC_CREATE_INFO_GET,
    SW_STATE_FUNC_STRINGNCAT,
    SW_STATE_FUNC_STRINGNCMP,
    SW_STATE_FUNC_STRINGNCPY,
    SW_STATE_FUNC_STRINGLEN,
    SW_STATE_FUNC_STRINGGET,
    SW_STATE_FUNC_NOF_ALLOCATED_ELEMENTS_IN_RANGE_GET,
    SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_SET,
    SW_STATE_FUNC_ADVANCED_ALGORITHM_INFO_GET,

    SW_STATE_FUNC_NOF
} dnxc_sw_state_function_types_e;
#endif

#ifdef DNX_SW_STATE_VERIFICATIONS

#define DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr) sizeof(*(ptr))

#define DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(module_id, ptr, idx)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_dynamic_array_check(unit, module_id, idx, (void *)ptr, DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)));\
    }

#define DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(idx, size)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_static_array_check(unit, (uint32) idx, (uint32) size));\
    }

#define DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(module_id, ptr, idx)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_dynamic_buffer_check(unit, module_id, (uint32) idx, (void *)ptr, DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)));\
    }

#define DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(idx, size)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_static_buffer_check(unit, (uint32) idx, (uint32) size));\
    }

#define DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(module_id, ptr, idx)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_dynamic_bitmap_check(unit, module_id, idx, (void *)ptr));\
    }

#define DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(idx, size)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnx_sw_state_oob_static_bitmap_check(unit, (uint32) idx, (uint32) size));\
    }

#define DNX_SW_STATE_PTR_NULL_CHECK(unit, module_id, ptr)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_null_check(unit, module_id, (void*) ptr));\
    }

#define VERIFY_FUNCTION_CALL_ALLOWED(unit, module, func_type, flags)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_func_call_allowed(unit, module, func_type, flags));\
    }

#define VERIFY_MODULE_IS_INITIALIZED(unit, module_id, module_ptr)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_module_is_init(unit, module_id, module_ptr));\
    }

#define DNX_SW_STATE_FAMILY_CHECK(unit, module_id, family)\
    if (DNXC_VERIFY_ALLOWED_GET(unit))\
    {\
        DNXC_SW_STATE_IF_ERR_ASSERT(dnxc_verifications_family(unit, module_id, family));\
    }

#else

#define DNX_SW_STATE_OOB_GET_TYPE_SIZE(ptr)
#define DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(module_id, ptr, idx)
#define DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(idx, size)
#define DNX_SW_STATE_OOB_DYNAMIC_BUFFER_CHECK(module_id, ptr, idx)
#define DNX_SW_STATE_OOB_STATIC_BUFFER_CHECK(idx, size)
#define DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(module_id, ptr, idx)
#define DNX_SW_STATE_OOB_STATIC_BITMAP_CHECK(idx, size)

#define DNX_SW_STATE_PTR_NULL_CHECK(unit, module_id, ptr)
#define VERIFY_FUNCTION_CALL_ALLOWED(unit, module, func_type, flags)
#define VERIFY_MODULE_IS_INITIALIZED(unit, module_id, module_ptr)
#define DNX_SW_STATE_FAMILY_CHECK(unit, module_id, family)

#endif

#ifdef DNX_SW_STATE_VERIFICATIONS

void dnxc_sw_state_err_assert_fail_debug_hook(
    int unit);

int dnxc_sw_state_err_assert_is_disabled(
    int unit);

int dnxc_sw_state_err_assert_state_change(
    int unit,
    uint8 is_disabled);

int dnxc_sw_state_verifications_deinit(
    int unit);

int dnxc_sw_state_verifications_init(
    int unit);

int dnxc_verifications_func_call_allowed(
    int unit,
    uint32 module,
    dnxc_sw_state_function_types_e func_type,
    uint32 flags);

int dnxc_verifications_module_is_init(
    int unit,
    uint32 module_id,
    void *module_ptr);

int dnxc_verifications_null_check(
    int unit,
    uint32 module_id,
    void *ptr);

int dnxc_verifications_family(
    int unit,
    uint32 module_id,
    soc_is_device_e soc_device);

int dnx_sw_state_oob_static_buffer_check(
    int unit,
    uint32 idx,
    uint32 arr_size);

int dnx_sw_state_oob_static_bitmap_check(
    int unit,
    uint32 idx,
    uint32 arr_size);

int dnx_sw_state_oob_static_array_check(
    int unit,
    uint32 idx,
    uint32 arr_size);

int dnx_sw_state_oob_dynamic_array_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr,
    uint32 type_size);

int dnx_sw_state_oob_dynamic_buffer_check(
    int unit,
    uint32 module_id,
    uint32 idx,
    void *ptr,
    uint32 type_size);

int dnx_sw_state_oob_dynamic_bitmap_check(
    int unit,
    uint32 module_id,
    int idx,
    void *ptr);

int dnxc_sw_state_alloc_verify_common(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 nof_elements,
    uint32 element_size,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_free_verify_common(
    int unit,
    uint32 module_id,
    uint8 **ptr_location,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_alloc_size_verify_common(
    int unit,
    uint32 module,
    uint8 *location,
    uint32 *nof_elements,
    uint32 *element_size);

int dnxc_sw_state_memcpy_verify_common(
    int unit,
    uint32 module_id,
    uint32 size,
    uint8 *dest,
    uint8 *src,
    uint32 flags,
    char *dbg_string);

int dnxc_sw_state_memset_verify_common(
    int unit,
    uint32 module_id,
    uint8 *dest,
    uint32 value,
    uint32 size,
    uint32 flags,
    char *dbg_string);

#else

shr_error_e dnx_sw_state_verification_dummy_func_to_make_compiler_happy(
    int unit);

#endif

#endif


/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/swstate/auto_generated/diagnostic/dnxf_diagnostic.h>
#include <soc/dnxf/swstate/auto_generated/layout/dnxf_layout.h>





int
dnxf_state_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNXF_STATE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]),
        "dnxf_state[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNXF_STATE,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnxf_state_t,
        DNX_SW_STATE_DNXF_STATE_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnxf_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNXF_STATE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]),
        "dnxf_state[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE,
        dnxf_state_info,
        DNXF_STATE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID]),
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_active_core_bmp_get(int unit, CONST SHR_BITDCL **active_core_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        active_core_bmp);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        DNX_SW_STATE_DIAG_READ);

    *active_core_bmp = &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp[0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp[0],
        "dnxf_state[%d]->info.active_core_bmp[%d]",
        unit, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        dnxf_state_info,
        DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_active_core_bmp_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_info_active_core_bmp_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        "dnxf_state[%d]->info.active_core_bmp",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        dnxf_state_info,
        DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_active_core_bmp_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        "dnxf_state[%d]->info.active_core_bmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        dnxf_state_info,
        DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_active_core_bmp_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        "dnxf_state[%d]->info.active_core_bmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        dnxf_state_info,
        DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_active_core_bmp_bit_range_clear(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        "dnxf_state[%d]->info.active_core_bmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        dnxf_state_info,
        DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_active_core_bmp_bit_range_set(int unit, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.active_core_bmp,
        "dnxf_state[%d]->info.active_core_bmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__ACTIVE_CORE_BMP,
        dnxf_state_info,
        DNXF_STATE_INFO_ACTIVE_CORE_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_soft_init_reg_above_64_val_set(int unit, uint32 soft_init_reg_above_64_val_idx_0, uint32 soft_init_reg_above_64_val)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val,
        soft_init_reg_above_64_val_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val[soft_init_reg_above_64_val_idx_0],
        soft_init_reg_above_64_val,
        uint32,
        0,
        "dnxf_state_info_soft_init_reg_above_64_val_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        &soft_init_reg_above_64_val,
        "dnxf_state[%d]->info.soft_init_reg_above_64_val[%d]",
        unit, soft_init_reg_above_64_val_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        dnxf_state_info,
        DNXF_STATE_INFO_SOFT_INIT_REG_ABOVE_64_VAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_soft_init_reg_above_64_val_get(int unit, uint32 soft_init_reg_above_64_val_idx_0, uint32 *soft_init_reg_above_64_val)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val,
        soft_init_reg_above_64_val_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        soft_init_reg_above_64_val);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        DNX_SW_STATE_DIAG_READ);

    *soft_init_reg_above_64_val = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val[soft_init_reg_above_64_val_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val[soft_init_reg_above_64_val_idx_0],
        "dnxf_state[%d]->info.soft_init_reg_above_64_val[%d]",
        unit, soft_init_reg_above_64_val_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        dnxf_state_info,
        DNXF_STATE_INFO_SOFT_INIT_REG_ABOVE_64_VAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_info_soft_init_reg_above_64_val_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_info_soft_init_reg_above_64_val_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val,
        "dnxf_state[%d]->info.soft_init_reg_above_64_val",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->info.soft_init_reg_above_64_val));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INFO__SOFT_INIT_REG_ABOVE_64_VAL,
        dnxf_state_info,
        DNXF_STATE_INFO_SOFT_INIT_REG_ABOVE_64_VAL_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_set(int unit, uint32 local_map_idx_0, CONST soc_dnxf_modid_local_map_t *local_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        local_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0],
        local_map,
        soc_dnxf_modid_local_map_t,
        0,
        "dnxf_state_modid_local_map_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        local_map,
        "dnxf_state[%d]->modid.local_map[%d]",
        unit, local_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_get(int unit, uint32 local_map_idx_0, soc_dnxf_modid_local_map_t *local_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        local_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        local_map);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        DNX_SW_STATE_DIAG_READ);

    *local_map = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0],
        "dnxf_state[%d]->modid.local_map[%d]",
        unit, local_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        soc_dnxf_modid_local_map_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_modid_local_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        "dnxf_state[%d]->modid.local_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(soc_dnxf_modid_local_map_t) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_valid_set(int unit, uint32 local_map_idx_0, uint32 valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        local_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0].valid,
        valid,
        uint32,
        0,
        "dnxf_state_modid_local_map_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        &valid,
        "dnxf_state[%d]->modid.local_map[%d].valid",
        unit, local_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_valid_get(int unit, uint32 local_map_idx_0, uint32 *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        local_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        valid);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0].valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0].valid,
        "dnxf_state[%d]->modid.local_map[%d].valid",
        unit, local_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__VALID,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_module_id_set(int unit, uint32 local_map_idx_0, uint32 module_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        local_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0].module_id,
        module_id,
        uint32,
        0,
        "dnxf_state_modid_local_map_module_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        &module_id,
        "dnxf_state[%d]->modid.local_map[%d].module_id",
        unit, local_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_MODULE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_local_map_module_id_get(int unit, uint32 local_map_idx_0, uint32 *module_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map,
        local_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        module_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        DNX_SW_STATE_DIAG_READ);

    *module_id = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0].module_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.local_map[local_map_idx_0].module_id,
        "dnxf_state[%d]->modid.local_map[%d].module_id",
        unit, local_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__LOCAL_MAP__MODULE_ID,
        dnxf_state_info,
        DNXF_STATE_MODID_LOCAL_MAP_MODULE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_group_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        soc_dnxf_modid_group_map_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_modid_group_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        "dnxf_state[%d]->modid.group_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(soc_dnxf_modid_group_map_t) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_GROUP_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_group_map_raw_alloc_bitmap(int unit, uint32 group_map_idx_0, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_modid_group_map_raw_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        "dnxf_state[%d]->modid.group_map[%d].raw",
        unit, group_map_idx_0,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        dnxf_state_info,
        DNXF_STATE_MODID_GROUP_MAP_RAW_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_group_map_raw_bit_set(int unit, uint32 group_map_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        "dnxf_state[%d]->modid.group_map[%d].raw",
        unit, group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        dnxf_state_info,
        DNXF_STATE_MODID_GROUP_MAP_RAW_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_group_map_raw_bit_range_read(int unit, uint32 group_map_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        "dnxf_state[%d]->modid.group_map[%d].raw",
        unit, group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        dnxf_state_info,
        DNXF_STATE_MODID_GROUP_MAP_RAW_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_group_map_raw_bit_range_write(int unit, uint32 group_map_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        "dnxf_state[%d]->modid.group_map[%d].raw",
        unit, group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        dnxf_state_info,
        DNXF_STATE_MODID_GROUP_MAP_RAW_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_group_map_raw_bit_range_clear(int unit, uint32 group_map_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map,
        group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.group_map[group_map_idx_0].raw,
        "dnxf_state[%d]->modid.group_map[%d].raw",
        unit, group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__GROUP_MAP__RAW,
        dnxf_state_info,
        DNXF_STATE_MODID_GROUP_MAP_RAW_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_modid_to_group_map_set(int unit, uint32 modid_to_group_map_idx_0, soc_module_t modid_to_group_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map,
        modid_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map[modid_to_group_map_idx_0],
        modid_to_group_map,
        soc_module_t,
        0,
        "dnxf_state_modid_modid_to_group_map_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        &modid_to_group_map,
        "dnxf_state[%d]->modid.modid_to_group_map[%d]",
        unit, modid_to_group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_MODID_TO_GROUP_MAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_modid_to_group_map_get(int unit, uint32 modid_to_group_map_idx_0, soc_module_t *modid_to_group_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map,
        modid_to_group_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        modid_to_group_map);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        DNX_SW_STATE_DIAG_READ);

    *modid_to_group_map = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map[modid_to_group_map_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map[modid_to_group_map_idx_0],
        "dnxf_state[%d]->modid.modid_to_group_map[%d]",
        unit, modid_to_group_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_MODID_TO_GROUP_MAP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_modid_modid_to_group_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map,
        soc_module_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_modid_modid_to_group_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map,
        "dnxf_state[%d]->modid.modid_to_group_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(soc_module_t) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->modid.modid_to_group_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MODID__MODID_TO_GROUP_MAP,
        dnxf_state_info,
        DNXF_STATE_MODID_MODID_TO_GROUP_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_mode_set(int unit, uint32 mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.mode,
        mode,
        uint32,
        0,
        "dnxf_state_mc_mode_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        &mode,
        "dnxf_state[%d]->mc.mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        dnxf_state_info,
        DNXF_STATE_MC_MODE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_mode_get(int unit, uint32 *mode)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        mode);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        DNX_SW_STATE_DIAG_READ);

    *mode = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.mode;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.mode,
        "dnxf_state[%d]->mc.mode",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__MODE,
        dnxf_state_info,
        DNXF_STATE_MC_MODE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_id_map_set(int unit, uint32 id_map_idx_0, uint32 id_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map,
        id_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map[id_map_idx_0],
        id_map,
        uint32,
        0,
        "dnxf_state_mc_id_map_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        &id_map,
        "dnxf_state[%d]->mc.id_map[%d]",
        unit, id_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        dnxf_state_info,
        DNXF_STATE_MC_ID_MAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_id_map_get(int unit, uint32 id_map_idx_0, uint32 *id_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map,
        id_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        id_map);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        DNX_SW_STATE_DIAG_READ);

    *id_map = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map[id_map_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map[id_map_idx_0],
        "dnxf_state[%d]->mc.id_map[%d]",
        unit, id_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        dnxf_state_info,
        DNXF_STATE_MC_ID_MAP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_id_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_mc_id_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map,
        "dnxf_state[%d]->mc.id_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.id_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__ID_MAP,
        dnxf_state_info,
        DNXF_STATE_MC_ID_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_nof_mc_groups_created_set(int unit, uint32 nof_mc_groups_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.nof_mc_groups_created,
        nof_mc_groups_created,
        uint32,
        0,
        "dnxf_state_mc_nof_mc_groups_created_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        &nof_mc_groups_created,
        "dnxf_state[%d]->mc.nof_mc_groups_created",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        dnxf_state_info,
        DNXF_STATE_MC_NOF_MC_GROUPS_CREATED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mc_nof_mc_groups_created_get(int unit, uint32 *nof_mc_groups_created)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        nof_mc_groups_created);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        DNX_SW_STATE_DIAG_READ);

    *nof_mc_groups_created = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.nof_mc_groups_created;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mc.nof_mc_groups_created,
        "dnxf_state[%d]->mc.nof_mc_groups_created",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MC__NOF_MC_GROUPS_CREATED,
        dnxf_state_info,
        DNXF_STATE_MC_NOF_MC_GROUPS_CREATED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lb_group_to_first_link_set(int unit, uint32 group_to_first_link_idx_0, uint32 group_to_first_link_idx_1, soc_port_t group_to_first_link)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link,
        group_to_first_link_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0],
        group_to_first_link_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0][group_to_first_link_idx_1],
        group_to_first_link,
        soc_port_t,
        0,
        "dnxf_state_lb_group_to_first_link_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        &group_to_first_link,
        "dnxf_state[%d]->lb.group_to_first_link[%d][%d]",
        unit, group_to_first_link_idx_0, group_to_first_link_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        dnxf_state_info,
        DNXF_STATE_LB_GROUP_TO_FIRST_LINK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lb_group_to_first_link_get(int unit, uint32 group_to_first_link_idx_0, uint32 group_to_first_link_idx_1, soc_port_t *group_to_first_link)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link,
        group_to_first_link_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        group_to_first_link);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0],
        group_to_first_link_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        DNX_SW_STATE_DIAG_READ);

    *group_to_first_link = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0][group_to_first_link_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0][group_to_first_link_idx_1],
        "dnxf_state[%d]->lb.group_to_first_link[%d][%d]",
        unit, group_to_first_link_idx_0, group_to_first_link_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        dnxf_state_info,
        DNXF_STATE_LB_GROUP_TO_FIRST_LINK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lb_group_to_first_link_alloc(int unit, uint32 nof_instances_to_alloc_0, uint32 nof_instances_to_alloc_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        nof_instances_to_alloc_0,
        nof_instances_to_alloc_1,
        sizeof(soc_port_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link,
        soc_port_t*,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_lb_group_to_first_link_alloc");

    for(uint32 group_to_first_link_idx_0 = 0;
         group_to_first_link_idx_0 < nof_instances_to_alloc_0;
         group_to_first_link_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link[group_to_first_link_idx_0],
        soc_port_t,
        nof_instances_to_alloc_1,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        1,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_lb_group_to_first_link_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link,
        "dnxf_state[%d]->lb.group_to_first_link",
        unit,
        nof_instances_to_alloc_1 * sizeof(soc_port_t) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lb.group_to_first_link)+(nof_instances_to_alloc_1 * sizeof(soc_port_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LB__GROUP_TO_FIRST_LINK,
        dnxf_state_info,
        DNXF_STATE_LB_GROUP_TO_FIRST_LINK_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_cl72_conf_set(int unit, uint32 cl72_conf_idx_0, uint32 cl72_conf)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf,
        cl72_conf_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf[cl72_conf_idx_0],
        cl72_conf,
        uint32,
        0,
        "dnxf_state_port_cl72_conf_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        &cl72_conf,
        "dnxf_state[%d]->port.cl72_conf[%d]",
        unit, cl72_conf_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        dnxf_state_info,
        DNXF_STATE_PORT_CL72_CONF_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_cl72_conf_get(int unit, uint32 cl72_conf_idx_0, uint32 *cl72_conf)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf,
        cl72_conf_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        cl72_conf);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        DNX_SW_STATE_DIAG_READ);

    *cl72_conf = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf[cl72_conf_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf[cl72_conf_idx_0],
        "dnxf_state[%d]->port.cl72_conf[%d]",
        unit, cl72_conf_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        dnxf_state_info,
        DNXF_STATE_PORT_CL72_CONF_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_cl72_conf_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_cl72_conf_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf,
        "dnxf_state[%d]->port.cl72_conf",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.cl72_conf));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__CL72_CONF,
        dnxf_state_info,
        DNXF_STATE_PORT_CL72_CONF_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_phy_lane_config_set(int unit, uint32 phy_lane_config_idx_0, int phy_lane_config)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config,
        phy_lane_config_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config[phy_lane_config_idx_0],
        phy_lane_config,
        int,
        0,
        "dnxf_state_port_phy_lane_config_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        &phy_lane_config,
        "dnxf_state[%d]->port.phy_lane_config[%d]",
        unit, phy_lane_config_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        dnxf_state_info,
        DNXF_STATE_PORT_PHY_LANE_CONFIG_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_phy_lane_config_get(int unit, uint32 phy_lane_config_idx_0, int *phy_lane_config)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config,
        phy_lane_config_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        phy_lane_config);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        DNX_SW_STATE_DIAG_READ);

    *phy_lane_config = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config[phy_lane_config_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config[phy_lane_config_idx_0],
        "dnxf_state[%d]->port.phy_lane_config[%d]",
        unit, phy_lane_config_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        dnxf_state_info,
        DNXF_STATE_PORT_PHY_LANE_CONFIG_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_phy_lane_config_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_phy_lane_config_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config,
        "dnxf_state[%d]->port.phy_lane_config",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.phy_lane_config));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__PHY_LANE_CONFIG,
        dnxf_state_info,
        DNXF_STATE_PORT_PHY_LANE_CONFIG_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_speed_set(int unit, uint32 speed_idx_0, int speed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed,
        speed_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed[speed_idx_0],
        speed,
        int,
        0,
        "dnxf_state_port_speed_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        &speed,
        "dnxf_state[%d]->port.speed[%d]",
        unit, speed_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        dnxf_state_info,
        DNXF_STATE_PORT_SPEED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_speed_get(int unit, uint32 speed_idx_0, int *speed)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed,
        speed_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        speed);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        DNX_SW_STATE_DIAG_READ);

    *speed = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed[speed_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed[speed_idx_0],
        "dnxf_state[%d]->port.speed[%d]",
        unit, speed_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        dnxf_state_info,
        DNXF_STATE_PORT_SPEED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_speed_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_speed_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed,
        "dnxf_state[%d]->port.speed",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.speed));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__SPEED,
        dnxf_state_info,
        DNXF_STATE_PORT_SPEED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_fec_type_set(int unit, uint32 fec_type_idx_0, int fec_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type,
        fec_type_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type[fec_type_idx_0],
        fec_type,
        int,
        0,
        "dnxf_state_port_fec_type_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        &fec_type,
        "dnxf_state[%d]->port.fec_type[%d]",
        unit, fec_type_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        dnxf_state_info,
        DNXF_STATE_PORT_FEC_TYPE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_fec_type_get(int unit, uint32 fec_type_idx_0, int *fec_type)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type,
        fec_type_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        fec_type);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        DNX_SW_STATE_DIAG_READ);

    *fec_type = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type[fec_type_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type[fec_type_idx_0],
        "dnxf_state[%d]->port.fec_type[%d]",
        unit, fec_type_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        dnxf_state_info,
        DNXF_STATE_PORT_FEC_TYPE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_fec_type_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_fec_type_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type,
        "dnxf_state[%d]->port.fec_type",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.fec_type));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__FEC_TYPE,
        dnxf_state_info,
        DNXF_STATE_PORT_FEC_TYPE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_is_connected_to_repeater_alloc_bitmap(int unit, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_is_connected_to_repeater_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        "dnxf_state[%d]->port.is_connected_to_repeater",
        unit,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        dnxf_state_info,
        DNXF_STATE_PORT_IS_CONNECTED_TO_REPEATER_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_is_connected_to_repeater_bit_set(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        "dnxf_state[%d]->port.is_connected_to_repeater",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        dnxf_state_info,
        DNXF_STATE_PORT_IS_CONNECTED_TO_REPEATER_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_is_connected_to_repeater_bit_clear(int unit, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        "dnxf_state[%d]->port.is_connected_to_repeater",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        dnxf_state_info,
        DNXF_STATE_PORT_IS_CONNECTED_TO_REPEATER_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_is_connected_to_repeater_bit_get(int unit, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
         0,
         ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.is_connected_to_repeater,
        "dnxf_state[%d]->port.is_connected_to_repeater",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__IS_CONNECTED_TO_REPEATER,
        dnxf_state_info,
        DNXF_STATE_PORT_IS_CONNECTED_TO_REPEATER_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_isolation_status_stored_set(int unit, uint32 isolation_status_stored_idx_0, int isolation_status_stored)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored,
        isolation_status_stored_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0],
        isolation_status_stored,
        int,
        0,
        "dnxf_state_port_isolation_status_stored_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        &isolation_status_stored,
        "dnxf_state[%d]->port.isolation_status_stored[%d]",
        unit, isolation_status_stored_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        dnxf_state_info,
        DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_isolation_status_stored_get(int unit, uint32 isolation_status_stored_idx_0, int *isolation_status_stored)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored,
        isolation_status_stored_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        isolation_status_stored);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        DNX_SW_STATE_DIAG_READ);

    *isolation_status_stored = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0],
        "dnxf_state[%d]->port.isolation_status_stored[%d]",
        unit, isolation_status_stored_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        dnxf_state_info,
        DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_isolation_status_stored_inc(int unit, uint32 isolation_status_stored_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored,
        isolation_status_stored_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0],
        inc_value,
        int,
        0,
        "dnxf_state_port_isolation_status_stored_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0],
        "dnxf_state[%d]->port.isolation_status_stored[%d]",
        unit, isolation_status_stored_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        dnxf_state_info,
        DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_isolation_status_stored_dec(int unit, uint32 isolation_status_stored_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored,
        isolation_status_stored_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0],
        dec_value,
        int,
        0,
        "dnxf_state_port_isolation_status_stored_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored[isolation_status_stored_idx_0],
        "dnxf_state[%d]->port.isolation_status_stored[%d]",
        unit, isolation_status_stored_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        dnxf_state_info,
        DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_isolation_status_stored_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_isolation_status_stored_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored,
        "dnxf_state[%d]->port.isolation_status_stored",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.isolation_status_stored));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ISOLATION_STATUS_STORED,
        dnxf_state_info,
        DNXF_STATE_PORT_ISOLATION_STATUS_STORED_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_orig_isolated_status_set(int unit, uint32 orig_isolated_status_idx_0, int orig_isolated_status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status,
        orig_isolated_status_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status[orig_isolated_status_idx_0],
        orig_isolated_status,
        int,
        0,
        "dnxf_state_port_orig_isolated_status_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        &orig_isolated_status,
        "dnxf_state[%d]->port.orig_isolated_status[%d]",
        unit, orig_isolated_status_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        dnxf_state_info,
        DNXF_STATE_PORT_ORIG_ISOLATED_STATUS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_orig_isolated_status_get(int unit, uint32 orig_isolated_status_idx_0, int *orig_isolated_status)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status,
        orig_isolated_status_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        orig_isolated_status);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        DNX_SW_STATE_DIAG_READ);

    *orig_isolated_status = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status[orig_isolated_status_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status[orig_isolated_status_idx_0],
        "dnxf_state[%d]->port.orig_isolated_status[%d]",
        unit, orig_isolated_status_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        dnxf_state_info,
        DNXF_STATE_PORT_ORIG_ISOLATED_STATUS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_port_orig_isolated_status_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_port_orig_isolated_status_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status,
        "dnxf_state[%d]->port.orig_isolated_status",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->port.orig_isolated_status));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__PORT__ORIG_ISOLATED_STATUS,
        dnxf_state_info,
        DNXF_STATE_PORT_ORIG_ISOLATED_STATUS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_cable_swap_is_enable_set(int unit, uint32 is_enable_idx_0, int is_enable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable,
        is_enable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable[is_enable_idx_0],
        is_enable,
        int,
        0,
        "dnxf_state_cable_swap_is_enable_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        &is_enable,
        "dnxf_state[%d]->cable_swap.is_enable[%d]",
        unit, is_enable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        dnxf_state_info,
        DNXF_STATE_CABLE_SWAP_IS_ENABLE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_cable_swap_is_enable_get(int unit, uint32 is_enable_idx_0, int *is_enable)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable,
        is_enable_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        is_enable);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        DNX_SW_STATE_DIAG_READ);

    *is_enable = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable[is_enable_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable[is_enable_idx_0],
        "dnxf_state[%d]->cable_swap.is_enable[%d]",
        unit, is_enable_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        dnxf_state_info,
        DNXF_STATE_CABLE_SWAP_IS_ENABLE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_cable_swap_is_enable_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_cable_swap_is_enable_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable,
        "dnxf_state[%d]->cable_swap.is_enable",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_enable));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_ENABLE,
        dnxf_state_info,
        DNXF_STATE_CABLE_SWAP_IS_ENABLE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_cable_swap_is_master_set(int unit, uint32 is_master_idx_0, int is_master)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master,
        is_master_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master[is_master_idx_0],
        is_master,
        int,
        0,
        "dnxf_state_cable_swap_is_master_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        &is_master,
        "dnxf_state[%d]->cable_swap.is_master[%d]",
        unit, is_master_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        dnxf_state_info,
        DNXF_STATE_CABLE_SWAP_IS_MASTER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_cable_swap_is_master_get(int unit, uint32 is_master_idx_0, int *is_master)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master,
        is_master_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        is_master);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        DNX_SW_STATE_DIAG_READ);

    *is_master = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master[is_master_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master[is_master_idx_0],
        "dnxf_state[%d]->cable_swap.is_master[%d]",
        unit, is_master_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        dnxf_state_info,
        DNXF_STATE_CABLE_SWAP_IS_MASTER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_cable_swap_is_master_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_cable_swap_is_master_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master,
        "dnxf_state[%d]->cable_swap.is_master",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->cable_swap.is_master));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__CABLE_SWAP__IS_MASTER,
        dnxf_state_info,
        DNXF_STATE_CABLE_SWAP_IS_MASTER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_flags_set(int unit, uint32 flags_idx_0, uint32 flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags,
        flags_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags[flags_idx_0],
        flags,
        uint32,
        0,
        "dnxf_state_intr_flags_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        &flags,
        "dnxf_state[%d]->intr.flags[%d]",
        unit, flags_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        dnxf_state_info,
        DNXF_STATE_INTR_FLAGS_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_flags_get(int unit, uint32 flags_idx_0, uint32 *flags)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags,
        flags_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        flags);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        DNX_SW_STATE_DIAG_READ);

    *flags = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags[flags_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags[flags_idx_0],
        "dnxf_state[%d]->intr.flags[%d]",
        unit, flags_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        dnxf_state_info,
        DNXF_STATE_INTR_FLAGS_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_flags_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_intr_flags_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags,
        "dnxf_state[%d]->intr.flags",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.flags));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__FLAGS,
        dnxf_state_info,
        DNXF_STATE_INTR_FLAGS_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_timed_count_set(int unit, uint32 storm_timed_count_idx_0, uint32 storm_timed_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count,
        storm_timed_count_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count[storm_timed_count_idx_0],
        storm_timed_count,
        uint32,
        0,
        "dnxf_state_intr_storm_timed_count_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        &storm_timed_count,
        "dnxf_state[%d]->intr.storm_timed_count[%d]",
        unit, storm_timed_count_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_TIMED_COUNT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_timed_count_get(int unit, uint32 storm_timed_count_idx_0, uint32 *storm_timed_count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count,
        storm_timed_count_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        storm_timed_count);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        DNX_SW_STATE_DIAG_READ);

    *storm_timed_count = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count[storm_timed_count_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count[storm_timed_count_idx_0],
        "dnxf_state[%d]->intr.storm_timed_count[%d]",
        unit, storm_timed_count_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_TIMED_COUNT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_timed_count_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_intr_storm_timed_count_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count,
        "dnxf_state[%d]->intr.storm_timed_count",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_count));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_COUNT,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_TIMED_COUNT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_timed_period_set(int unit, uint32 storm_timed_period_idx_0, uint32 storm_timed_period)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period,
        storm_timed_period_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period[storm_timed_period_idx_0],
        storm_timed_period,
        uint32,
        0,
        "dnxf_state_intr_storm_timed_period_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        &storm_timed_period,
        "dnxf_state[%d]->intr.storm_timed_period[%d]",
        unit, storm_timed_period_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_TIMED_PERIOD_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_timed_period_get(int unit, uint32 storm_timed_period_idx_0, uint32 *storm_timed_period)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period,
        storm_timed_period_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        storm_timed_period);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        DNX_SW_STATE_DIAG_READ);

    *storm_timed_period = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period[storm_timed_period_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period[storm_timed_period_idx_0],
        "dnxf_state[%d]->intr.storm_timed_period[%d]",
        unit, storm_timed_period_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_TIMED_PERIOD_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_timed_period_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period,
        uint32,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_intr_storm_timed_period_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period,
        "dnxf_state[%d]->intr.storm_timed_period",
        unit,
        nof_instances_to_alloc_0 * sizeof(uint32) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_timed_period));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_TIMED_PERIOD,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_TIMED_PERIOD_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_nominal_set(int unit, uint32 storm_nominal)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_nominal,
        storm_nominal,
        uint32,
        0,
        "dnxf_state_intr_storm_nominal_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        &storm_nominal,
        "dnxf_state[%d]->intr.storm_nominal",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_NOMINAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_intr_storm_nominal_get(int unit, uint32 *storm_nominal)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        storm_nominal);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        DNX_SW_STATE_DIAG_READ);

    *storm_nominal = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_nominal;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->intr.storm_nominal,
        "dnxf_state[%d]->intr.storm_nominal",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__INTR__STORM_NOMINAL,
        dnxf_state_info,
        DNXF_STATE_INTR_STORM_NOMINAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_map_size_set(int unit, int map_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.map_size,
        map_size,
        int,
        0,
        "dnxf_state_lane_map_db_serdes_map_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        &map_size,
        "dnxf_state[%d]->lane_map_db.serdes_map.map_size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_map_size_get(int unit, int *map_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        map_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *map_size = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.map_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.map_size,
        "dnxf_state[%d]->lane_map_db.serdes_map.map_size",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__MAP_SIZE,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_set(int unit, uint32 lane2serdes_idx_0, CONST soc_dnxc_lane_map_db_map_t *lane2serdes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0],
        lane2serdes,
        soc_dnxc_lane_map_db_map_t,
        0,
        "dnxf_state_lane_map_db_serdes_map_lane2serdes_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        lane2serdes,
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes[%d]",
        unit, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_get(int unit, uint32 lane2serdes_idx_0, soc_dnxc_lane_map_db_map_t *lane2serdes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        lane2serdes);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        DNX_SW_STATE_DIAG_READ);

    *lane2serdes = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0],
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes[%d]",
        unit, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        soc_dnxc_lane_map_db_map_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_lane_map_db_serdes_map_lane2serdes_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes",
        unit,
        nof_instances_to_alloc_0 * sizeof(soc_dnxc_lane_map_db_map_t) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_set(int unit, uint32 lane2serdes_idx_0, int rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0].rx_id,
        rx_id,
        int,
        0,
        "dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        &rx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes[%d].rx_id",
        unit, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_RX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_get(int unit, uint32 lane2serdes_idx_0, int *rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        rx_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        DNX_SW_STATE_DIAG_READ);

    *rx_id = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0].rx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0].rx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes[%d].rx_id",
        unit, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__RX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_RX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_set(int unit, uint32 lane2serdes_idx_0, int tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0].tx_id,
        tx_id,
        int,
        0,
        "dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        &tx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes[%d].tx_id",
        unit, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_TX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_get(int unit, uint32 lane2serdes_idx_0, int *tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        tx_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        DNX_SW_STATE_DIAG_READ);

    *tx_id = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0].tx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.lane2serdes[lane2serdes_idx_0].tx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.lane2serdes[%d].tx_id",
        unit, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__LANE2SERDES__TX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_LANE2SERDES_TX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_set(int unit, uint32 serdes2lane_idx_0, CONST soc_dnxc_lane_map_db_map_t *serdes2lane)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0],
        serdes2lane,
        soc_dnxc_lane_map_db_map_t,
        0,
        "dnxf_state_lane_map_db_serdes_map_serdes2lane_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        serdes2lane,
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane[%d]",
        unit, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_get(int unit, uint32 serdes2lane_idx_0, soc_dnxc_lane_map_db_map_t *serdes2lane)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        serdes2lane);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        DNX_SW_STATE_DIAG_READ);

    *serdes2lane = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0],
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane[%d]",
        unit, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        soc_dnxc_lane_map_db_map_t,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_lane_map_db_serdes_map_serdes2lane_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane",
        unit,
        nof_instances_to_alloc_0 * sizeof(soc_dnxc_lane_map_db_map_t) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_set(int unit, uint32 serdes2lane_idx_0, int rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0].rx_id,
        rx_id,
        int,
        0,
        "dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        &rx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane[%d].rx_id",
        unit, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_RX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_get(int unit, uint32 serdes2lane_idx_0, int *rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        rx_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        DNX_SW_STATE_DIAG_READ);

    *rx_id = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0].rx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0].rx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane[%d].rx_id",
        unit, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__RX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_RX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_set(int unit, uint32 serdes2lane_idx_0, int tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0].tx_id,
        tx_id,
        int,
        0,
        "dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        &tx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane[%d].tx_id",
        unit, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_TX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_get(int unit, uint32 serdes2lane_idx_0, int *tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        tx_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        DNX_SW_STATE_DIAG_READ);

    *tx_id = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0].tx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.serdes_map.serdes2lane[serdes2lane_idx_0].tx_id,
        "dnxf_state[%d]->lane_map_db.serdes_map.serdes2lane[%d].tx_id",
        unit, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__SERDES_MAP__SERDES2LANE__TX_ID,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_SERDES_MAP_SERDES2LANE_TX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_link_to_fmac_lane_map_set(int unit, uint32 link_to_fmac_lane_map_idx_0, int link_to_fmac_lane_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map,
        link_to_fmac_lane_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map[link_to_fmac_lane_map_idx_0],
        link_to_fmac_lane_map,
        int,
        0,
        "dnxf_state_lane_map_db_link_to_fmac_lane_map_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        &link_to_fmac_lane_map,
        "dnxf_state[%d]->lane_map_db.link_to_fmac_lane_map[%d]",
        unit, link_to_fmac_lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_LINK_TO_FMAC_LANE_MAP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_link_to_fmac_lane_map_get(int unit, uint32 link_to_fmac_lane_map_idx_0, int *link_to_fmac_lane_map)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map,
        link_to_fmac_lane_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        link_to_fmac_lane_map);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        DNX_SW_STATE_DIAG_READ);

    *link_to_fmac_lane_map = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map[link_to_fmac_lane_map_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map[link_to_fmac_lane_map_idx_0],
        "dnxf_state[%d]->lane_map_db.link_to_fmac_lane_map[%d]",
        unit, link_to_fmac_lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_LINK_TO_FMAC_LANE_MAP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_lane_map_db_link_to_fmac_lane_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map,
        int,
        nof_instances_to_alloc_0,
        sw_state_layout_array[unit][DNXF_MODULE_ID],
        0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnxf_state_lane_map_db_link_to_fmac_lane_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map,
        "dnxf_state[%d]->lane_map_db.link_to_fmac_lane_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(int) / sizeof(*((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->lane_map_db.link_to_fmac_lane_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__LANE_MAP_DB__LINK_TO_FMAC_LANE_MAP,
        dnxf_state_info,
        DNXF_STATE_LANE_MAP_DB_LINK_TO_FMAC_LANE_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_master_synce_enabled_set(int unit, int master_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_synce_enabled,
        master_synce_enabled,
        int,
        0,
        "dnxf_state_synce_master_synce_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        &master_synce_enabled,
        "dnxf_state[%d]->synce.master_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_MASTER_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_master_synce_enabled_get(int unit, int *master_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        master_synce_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *master_synce_enabled = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_synce_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_synce_enabled,
        "dnxf_state[%d]->synce.master_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_MASTER_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_slave_synce_enabled_set(int unit, int slave_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_synce_enabled,
        slave_synce_enabled,
        int,
        0,
        "dnxf_state_synce_slave_synce_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        &slave_synce_enabled,
        "dnxf_state[%d]->synce.slave_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_SLAVE_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_slave_synce_enabled_get(int unit, int *slave_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        slave_synce_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *slave_synce_enabled = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_synce_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_synce_enabled,
        "dnxf_state[%d]->synce.slave_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_SLAVE_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_master_set1_synce_enabled_set(int unit, int master_set1_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_set1_synce_enabled,
        master_set1_synce_enabled,
        int,
        0,
        "dnxf_state_synce_master_set1_synce_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        &master_set1_synce_enabled,
        "dnxf_state[%d]->synce.master_set1_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_MASTER_SET1_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_master_set1_synce_enabled_get(int unit, int *master_set1_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        master_set1_synce_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *master_set1_synce_enabled = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_set1_synce_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.master_set1_synce_enabled,
        "dnxf_state[%d]->synce.master_set1_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__MASTER_SET1_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_MASTER_SET1_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_slave_set1_synce_enabled_set(int unit, int slave_set1_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_set1_synce_enabled,
        slave_set1_synce_enabled,
        int,
        0,
        "dnxf_state_synce_slave_set1_synce_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        &slave_set1_synce_enabled,
        "dnxf_state[%d]->synce.slave_set1_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_SLAVE_SET1_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_synce_slave_set1_synce_enabled_get(int unit, int *slave_set1_synce_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        slave_set1_synce_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *slave_set1_synce_enabled = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_set1_synce_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->synce.slave_set1_synce_enabled,
        "dnxf_state[%d]->synce.slave_set1_synce_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYNCE__SLAVE_SET1_SYNCE_ENABLED,
        dnxf_state_info,
        DNXF_STATE_SYNCE_SLAVE_SET1_SYNCE_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_system_upgrade_state_0_set(int unit, uint32 state_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_0,
        state_0,
        uint32,
        0,
        "dnxf_state_system_upgrade_state_0_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        &state_0,
        "dnxf_state[%d]->system_upgrade.state_0",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        dnxf_state_info,
        DNXF_STATE_SYSTEM_UPGRADE_STATE_0_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_system_upgrade_state_0_get(int unit, uint32 *state_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        state_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        DNX_SW_STATE_DIAG_READ);

    *state_0 = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_0;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_0,
        "dnxf_state[%d]->system_upgrade.state_0",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_0,
        dnxf_state_info,
        DNXF_STATE_SYSTEM_UPGRADE_STATE_0_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_system_upgrade_state_1_set(int unit, uint32 state_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_1,
        state_1,
        uint32,
        0,
        "dnxf_state_system_upgrade_state_1_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        &state_1,
        "dnxf_state[%d]->system_upgrade.state_1",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        dnxf_state_info,
        DNXF_STATE_SYSTEM_UPGRADE_STATE_1_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_system_upgrade_state_1_get(int unit, uint32 *state_1)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        state_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        DNX_SW_STATE_DIAG_READ);

    *state_1 = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_1;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_1,
        "dnxf_state[%d]->system_upgrade.state_1",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_1,
        dnxf_state_info,
        DNXF_STATE_SYSTEM_UPGRADE_STATE_1_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_system_upgrade_state_2_set(int unit, uint32 state_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_2,
        state_2,
        uint32,
        0,
        "dnxf_state_system_upgrade_state_2_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        &state_2,
        "dnxf_state[%d]->system_upgrade.state_2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        dnxf_state_info,
        DNXF_STATE_SYSTEM_UPGRADE_STATE_2_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_system_upgrade_state_2_get(int unit, uint32 *state_2)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        state_2);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        DNX_SW_STATE_DIAG_READ);

    *state_2 = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_2;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->system_upgrade.state_2,
        "dnxf_state[%d]->system_upgrade.state_2",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__SYSTEM_UPGRADE__STATE_2,
        dnxf_state_info,
        DNXF_STATE_SYSTEM_UPGRADE_STATE_2_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_hard_reset_callback_set(int unit, bcm_switch_hard_reset_callback_t callback)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->hard_reset.callback,
        callback,
        bcm_switch_hard_reset_callback_t,
        0,
        "dnxf_state_hard_reset_callback_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        &callback,
        "dnxf_state[%d]->hard_reset.callback",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        dnxf_state_info,
        DNXF_STATE_HARD_RESET_CALLBACK_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_hard_reset_callback_get(int unit, bcm_switch_hard_reset_callback_t *callback)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        callback);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        DNX_SW_STATE_DIAG_READ);

    *callback = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->hard_reset.callback;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->hard_reset.callback,
        "dnxf_state[%d]->hard_reset.callback",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__CALLBACK,
        dnxf_state_info,
        DNXF_STATE_HARD_RESET_CALLBACK_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_hard_reset_userdata_set(int unit, uint32* userdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->hard_reset.userdata,
        userdata,
        uint32*,
        0,
        "dnxf_state_hard_reset_userdata_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        &userdata,
        "dnxf_state[%d]->hard_reset.userdata",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        dnxf_state_info,
        DNXF_STATE_HARD_RESET_USERDATA_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_hard_reset_userdata_get(int unit, uint32* *userdata)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        userdata);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        DNX_SW_STATE_DIAG_READ);

    *userdata = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->hard_reset.userdata;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->hard_reset.userdata,
        "dnxf_state[%d]->hard_reset.userdata",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__HARD_RESET__USERDATA,
        dnxf_state_info,
        DNXF_STATE_HARD_RESET_USERDATA_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_set(int unit, bcm_pbmp_t counter_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        counter_pbmp,
        bcm_pbmp_t,
        0,
        "dnxf_state_mib_counter_pbmp_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_get(int unit, bcm_pbmp_t *counter_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        counter_pbmp);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_READ);

    *counter_pbmp = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_neq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_NEQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEQ(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEQ,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_eq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_EQ(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPEQ,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_member(int unit, uint32 _input_port, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_MEMBER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_MEMBER(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_MEMBER_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPMEMBER,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_not_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_NOT_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NOT_NULL(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NOT_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNOTNULL,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_is_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_IS_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_IS_NULL(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_IS_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPISNULL,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_count(int unit, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_COUNT(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCOUNT,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_xor(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_XOR(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPXOR,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_remove(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_REMOVE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPREMOVE,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_assign(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_ASSIGN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_ASSIGN(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_ASSIGN_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPASSIGN,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_get(int unit, _shr_pbmp_t *output_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_GET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        output_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_GET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPGET,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_and(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_AND(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_AND_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPAND,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_negate(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEGATE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEGATE,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_or(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_OR(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_OR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPOR,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_CLEAR(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCLEAR,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_port_add(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_ADD(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTADD,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_port_flip(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_FLIP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_FLIP(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTFLIP,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_port_remove(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_REMOVE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTREMOVE,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_port_set(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSET,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_ports_range_add(int unit, uint32 _first_port, uint32 _range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _first_port,
        _range,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSRANGEADD,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_counter_pbmp_pbmp_fmt(int unit, char* _buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        SW_STATE_FUNC_PBMP_FMT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_FMT(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        _buffer,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_FMT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPFMT,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.counter_pbmp,
        "dnxf_state[%d]->mib.counter_pbmp",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__COUNTER_PBMP,
        dnxf_state_info,
        DNXF_STATE_MIB_COUNTER_PBMP_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_interval_set(int unit, int interval)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.interval,
        interval,
        int,
        0,
        "dnxf_state_mib_interval_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        &interval,
        "dnxf_state[%d]->mib.interval",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        dnxf_state_info,
        DNXF_STATE_MIB_INTERVAL_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnxf_state_mib_interval_get(int unit, int *interval)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        sw_state_roots_array[unit][DNXF_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        interval);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        DNX_SW_STATE_DIAG_READ);

    *interval = ((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.interval;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        &((dnxf_state_t*)sw_state_roots_array[unit][DNXF_MODULE_ID])->mib.interval,
        "dnxf_state[%d]->mib.interval",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNXF_STATE__MIB__INTERVAL,
        dnxf_state_info,
        DNXF_STATE_MIB_INTERVAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNXF_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnxf_state_cbs dnxf_state = 	{
	
	dnxf_state_is_init,
	dnxf_state_init,
		{
		
			{
			
			dnxf_state_info_active_core_bmp_get,
			dnxf_state_info_active_core_bmp_alloc_bitmap,
			dnxf_state_info_active_core_bmp_bit_set,
			dnxf_state_info_active_core_bmp_bit_get,
			dnxf_state_info_active_core_bmp_bit_range_clear,
			dnxf_state_info_active_core_bmp_bit_range_set}
		,
			{
			
			dnxf_state_info_soft_init_reg_above_64_val_set,
			dnxf_state_info_soft_init_reg_above_64_val_get,
			dnxf_state_info_soft_init_reg_above_64_val_alloc}
		}
	,
		{
		
			{
			
			dnxf_state_modid_local_map_set,
			dnxf_state_modid_local_map_get,
			dnxf_state_modid_local_map_alloc,
				{
				
				dnxf_state_modid_local_map_valid_set,
				dnxf_state_modid_local_map_valid_get}
			,
				{
				
				dnxf_state_modid_local_map_module_id_set,
				dnxf_state_modid_local_map_module_id_get}
			}
		,
			{
			
			dnxf_state_modid_group_map_alloc,
				{
				
				dnxf_state_modid_group_map_raw_alloc_bitmap,
				dnxf_state_modid_group_map_raw_bit_set,
				dnxf_state_modid_group_map_raw_bit_range_read,
				dnxf_state_modid_group_map_raw_bit_range_write,
				dnxf_state_modid_group_map_raw_bit_range_clear}
			}
		,
			{
			
			dnxf_state_modid_modid_to_group_map_set,
			dnxf_state_modid_modid_to_group_map_get,
			dnxf_state_modid_modid_to_group_map_alloc}
		}
	,
		{
		
			{
			
			dnxf_state_mc_mode_set,
			dnxf_state_mc_mode_get}
		,
			{
			
			dnxf_state_mc_id_map_set,
			dnxf_state_mc_id_map_get,
			dnxf_state_mc_id_map_alloc}
		,
			{
			
			dnxf_state_mc_nof_mc_groups_created_set,
			dnxf_state_mc_nof_mc_groups_created_get}
		}
	,
		{
		
			{
			
			dnxf_state_lb_group_to_first_link_set,
			dnxf_state_lb_group_to_first_link_get,
			dnxf_state_lb_group_to_first_link_alloc}
		}
	,
		{
		
			{
			
			dnxf_state_port_cl72_conf_set,
			dnxf_state_port_cl72_conf_get,
			dnxf_state_port_cl72_conf_alloc}
		,
			{
			
			dnxf_state_port_phy_lane_config_set,
			dnxf_state_port_phy_lane_config_get,
			dnxf_state_port_phy_lane_config_alloc}
		,
			{
			
			dnxf_state_port_speed_set,
			dnxf_state_port_speed_get,
			dnxf_state_port_speed_alloc}
		,
			{
			
			dnxf_state_port_fec_type_set,
			dnxf_state_port_fec_type_get,
			dnxf_state_port_fec_type_alloc}
		,
			{
			
			dnxf_state_port_is_connected_to_repeater_alloc_bitmap,
			dnxf_state_port_is_connected_to_repeater_bit_set,
			dnxf_state_port_is_connected_to_repeater_bit_clear,
			dnxf_state_port_is_connected_to_repeater_bit_get}
		,
			{
			
			dnxf_state_port_isolation_status_stored_set,
			dnxf_state_port_isolation_status_stored_get,
			dnxf_state_port_isolation_status_stored_inc,
			dnxf_state_port_isolation_status_stored_dec,
			dnxf_state_port_isolation_status_stored_alloc}
		,
			{
			
			dnxf_state_port_orig_isolated_status_set,
			dnxf_state_port_orig_isolated_status_get,
			dnxf_state_port_orig_isolated_status_alloc}
		}
	,
		{
		
			{
			
			dnxf_state_cable_swap_is_enable_set,
			dnxf_state_cable_swap_is_enable_get,
			dnxf_state_cable_swap_is_enable_alloc}
		,
			{
			
			dnxf_state_cable_swap_is_master_set,
			dnxf_state_cable_swap_is_master_get,
			dnxf_state_cable_swap_is_master_alloc}
		}
	,
		{
		
			{
			
			dnxf_state_intr_flags_set,
			dnxf_state_intr_flags_get,
			dnxf_state_intr_flags_alloc}
		,
			{
			
			dnxf_state_intr_storm_timed_count_set,
			dnxf_state_intr_storm_timed_count_get,
			dnxf_state_intr_storm_timed_count_alloc}
		,
			{
			
			dnxf_state_intr_storm_timed_period_set,
			dnxf_state_intr_storm_timed_period_get,
			dnxf_state_intr_storm_timed_period_alloc}
		,
			{
			
			dnxf_state_intr_storm_nominal_set,
			dnxf_state_intr_storm_nominal_get}
		}
	,
		{
		
			{
			
				{
				
				dnxf_state_lane_map_db_serdes_map_map_size_set,
				dnxf_state_lane_map_db_serdes_map_map_size_get}
			,
				{
				
				dnxf_state_lane_map_db_serdes_map_lane2serdes_set,
				dnxf_state_lane_map_db_serdes_map_lane2serdes_get,
				dnxf_state_lane_map_db_serdes_map_lane2serdes_alloc,
					{
					
					dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_set,
					dnxf_state_lane_map_db_serdes_map_lane2serdes_rx_id_get}
				,
					{
					
					dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_set,
					dnxf_state_lane_map_db_serdes_map_lane2serdes_tx_id_get}
				}
			,
				{
				
				dnxf_state_lane_map_db_serdes_map_serdes2lane_set,
				dnxf_state_lane_map_db_serdes_map_serdes2lane_get,
				dnxf_state_lane_map_db_serdes_map_serdes2lane_alloc,
					{
					
					dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_set,
					dnxf_state_lane_map_db_serdes_map_serdes2lane_rx_id_get}
				,
					{
					
					dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_set,
					dnxf_state_lane_map_db_serdes_map_serdes2lane_tx_id_get}
				}
			}
		,
			{
			
			dnxf_state_lane_map_db_link_to_fmac_lane_map_set,
			dnxf_state_lane_map_db_link_to_fmac_lane_map_get,
			dnxf_state_lane_map_db_link_to_fmac_lane_map_alloc}
		}
	,
		{
		
			{
			
			dnxf_state_synce_master_synce_enabled_set,
			dnxf_state_synce_master_synce_enabled_get}
		,
			{
			
			dnxf_state_synce_slave_synce_enabled_set,
			dnxf_state_synce_slave_synce_enabled_get}
		,
			{
			
			dnxf_state_synce_master_set1_synce_enabled_set,
			dnxf_state_synce_master_set1_synce_enabled_get}
		,
			{
			
			dnxf_state_synce_slave_set1_synce_enabled_set,
			dnxf_state_synce_slave_set1_synce_enabled_get}
		}
	,
		{
		
			{
			
			dnxf_state_system_upgrade_state_0_set,
			dnxf_state_system_upgrade_state_0_get}
		,
			{
			
			dnxf_state_system_upgrade_state_1_set,
			dnxf_state_system_upgrade_state_1_get}
		,
			{
			
			dnxf_state_system_upgrade_state_2_set,
			dnxf_state_system_upgrade_state_2_get}
		}
	,
		{
		
			{
			
			dnxf_state_hard_reset_callback_set,
			dnxf_state_hard_reset_callback_get}
		,
			{
			
			dnxf_state_hard_reset_userdata_set,
			dnxf_state_hard_reset_userdata_get}
		}
	,
		{
		
			{
			
			dnxf_state_mib_counter_pbmp_set,
			dnxf_state_mib_counter_pbmp_get,
			dnxf_state_mib_counter_pbmp_pbmp_neq,
			dnxf_state_mib_counter_pbmp_pbmp_eq,
			dnxf_state_mib_counter_pbmp_pbmp_member,
			dnxf_state_mib_counter_pbmp_pbmp_not_null,
			dnxf_state_mib_counter_pbmp_pbmp_is_null,
			dnxf_state_mib_counter_pbmp_pbmp_count,
			dnxf_state_mib_counter_pbmp_pbmp_xor,
			dnxf_state_mib_counter_pbmp_pbmp_remove,
			dnxf_state_mib_counter_pbmp_pbmp_assign,
			dnxf_state_mib_counter_pbmp_pbmp_get,
			dnxf_state_mib_counter_pbmp_pbmp_and,
			dnxf_state_mib_counter_pbmp_pbmp_negate,
			dnxf_state_mib_counter_pbmp_pbmp_or,
			dnxf_state_mib_counter_pbmp_pbmp_clear,
			dnxf_state_mib_counter_pbmp_pbmp_port_add,
			dnxf_state_mib_counter_pbmp_pbmp_port_flip,
			dnxf_state_mib_counter_pbmp_pbmp_port_remove,
			dnxf_state_mib_counter_pbmp_pbmp_port_set,
			dnxf_state_mib_counter_pbmp_pbmp_ports_range_add,
			dnxf_state_mib_counter_pbmp_pbmp_fmt}
		,
			{
			
			dnxf_state_mib_interval_set,
			dnxf_state_mib_interval_get}
		}
	,
	}
;
#undef BSL_LOG_MODULE

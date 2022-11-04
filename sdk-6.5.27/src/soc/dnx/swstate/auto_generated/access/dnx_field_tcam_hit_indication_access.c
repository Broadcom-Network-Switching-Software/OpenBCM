
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_hit_indication_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_field_tcam_hit_indication_layout.h>





int
dnx_field_tcam_hit_indication_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]),
        "dnx_field_tcam_hit_indication[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_hit_indication_t,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_field_tcam_hit_indication_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]),
        "dnx_field_tcam_hit_indication[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_field.tcam.nof_banks_hitbit_units_get(unit),
        sizeof(dnx_field_tcam_hit_indication_hitbit_unit_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        dnx_field_tcam_hit_indication_hitbit_unit_info_t*,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc");

    for(uint32 tcam_action_hit_indication_idx_0 = 0;
         tcam_action_hit_indication_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         tcam_action_hit_indication_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        dnx_field_tcam_hit_indication_hitbit_unit_info_t,
        dnx_data_field.tcam.nof_banks_hitbit_units_get(unit),
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication",
        unit,
        dnx_data_field.tcam.nof_banks_hitbit_units_get(unit) * sizeof(dnx_field_tcam_hit_indication_hitbit_unit_info_t) / sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication)+(dnx_data_field.tcam.nof_banks_hitbit_units_get(unit) * sizeof(dnx_field_tcam_hit_indication_hitbit_unit_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_alloc_bitmap(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _nof_bits_to_alloc)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        _nof_bits_to_alloc,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1,
        _nof_bits_to_alloc);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_set(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_clear(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_get(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_read(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_write(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_and(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_or(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_xor(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_remove(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_negate(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_clear(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_set(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_null(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_test(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_eq(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_count(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 tcam_action_hit_indication_idx_1, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0],
        tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        (_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
         0,
         ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0][tcam_action_hit_indication_idx_1].bmp,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d][%d].bmp",
        unit, tcam_action_hit_indication_idx_0, tcam_action_hit_indication_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_FIELD_TCAM_HIT_INDICATION__TCAM_ACTION_HIT_INDICATION__BMP,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BMP_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_field_tcam_hit_indication_cbs dnx_field_tcam_hit_indication = 	{
	
	dnx_field_tcam_hit_indication_is_init,
	dnx_field_tcam_hit_indication_init,
		{
		
		dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc,
			{
			
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_alloc_bitmap,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_set,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_clear,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_get,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_read,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_write,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_and,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_or,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_xor,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_remove,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_negate,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_clear,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_set,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_null,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_test,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_eq,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bmp_bit_range_count}
		}
	}
;
#undef BSL_LOG_MODULE

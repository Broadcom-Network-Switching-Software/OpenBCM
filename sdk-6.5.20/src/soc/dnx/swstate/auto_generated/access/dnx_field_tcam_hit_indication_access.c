
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnxc/swstate/dnxc_sw_state_c_includes.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_tcam_hit_indication_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_field_tcam_hit_indication_diagnostic.h>



dnx_field_tcam_hit_indication_t* dnx_field_tcam_hit_indication_dummy = NULL;



int
dnx_field_tcam_hit_indication_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
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
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_field_tcam_hit_indication_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]),
        "dnx_field_tcam_hit_indication[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_field_tcam_hit_indication_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        dnx_field_tcam_hit_indication_big_banks_sram_info_t,
        dnx_data_field.tcam.nof_big_banks_srams_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication",
        unit,
        dnx_data_field.tcam.nof_big_banks_srams_get(unit) * sizeof(dnx_field_tcam_hit_indication_big_banks_sram_info_t) / sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_set(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 bank_line_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id,
        bank_line_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0],
        bank_line_id_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0][bank_line_id_idx_1],
        bank_line_id,
        uint8,
        0,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        &bank_line_id,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d].bank_line_id[%d][%d]",
        unit, tcam_action_hit_indication_idx_0, bank_line_id_idx_0, bank_line_id_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BANK_LINE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_get(int unit, uint32 tcam_action_hit_indication_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 *bank_line_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication,
        tcam_action_hit_indication_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        bank_line_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id,
        bank_line_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0],
        bank_line_id_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    *bank_line_id = ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0][bank_line_id_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0][bank_line_id_idx_1],
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d].bank_line_id[%d][%d]",
        unit, tcam_action_hit_indication_idx_0, bank_line_id_idx_0, bank_line_id_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BANK_LINE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_alloc(int unit, uint32 tcam_action_hit_indication_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id,
        uint8*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_alloc");

    for(uint32 bank_line_id_idx_0 = 0;
         bank_line_id_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         bank_line_id_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id[bank_line_id_idx_0],
        uint8,
        dnx_data_field.tcam.nof_big_bank_lines_per_sram_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication[%d].bank_line_id",
        unit, tcam_action_hit_indication_idx_0,
        dnx_data_field.tcam.nof_big_bank_lines_per_sram_get(unit) * sizeof(uint8) / sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication[tcam_action_hit_indication_idx_0].bank_line_id)+(dnx_data_field.tcam.nof_big_bank_lines_per_sram_get(unit) * sizeof(uint8)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_BANK_LINE_ID_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small,
        dnx_field_tcam_hit_indication_small_banks_sram_info_t,
        dnx_data_field.tcam.nof_small_banks_srams_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication_small",
        unit,
        dnx_data_field.tcam.nof_small_banks_srams_get(unit) * sizeof(dnx_field_tcam_hit_indication_small_banks_sram_info_t) / sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_SMALL_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_set(int unit, uint32 tcam_action_hit_indication_small_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 bank_line_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small,
        tcam_action_hit_indication_small_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id,
        bank_line_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0],
        bank_line_id_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0][bank_line_id_idx_1],
        bank_line_id,
        uint8,
        0,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        &bank_line_id,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication_small[%d].bank_line_id[%d][%d]",
        unit, tcam_action_hit_indication_small_idx_0, bank_line_id_idx_0, bank_line_id_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_SMALL_BANK_LINE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_get(int unit, uint32 tcam_action_hit_indication_small_idx_0, uint32 bank_line_id_idx_0, uint32 bank_line_id_idx_1, uint8 *bank_line_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small,
        tcam_action_hit_indication_small_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        bank_line_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id,
        bank_line_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0],
        bank_line_id_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    *bank_line_id = ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0][bank_line_id_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        &((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0][bank_line_id_idx_1],
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication_small[%d].bank_line_id[%d][%d]",
        unit, tcam_action_hit_indication_small_idx_0, bank_line_id_idx_0, bank_line_id_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_SMALL_BANK_LINE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_alloc(int unit, uint32 tcam_action_hit_indication_small_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id,
        uint8*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_alloc");

    for(uint32 bank_line_id_idx_0 = 0;
         bank_line_id_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         bank_line_id_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id[bank_line_id_idx_0],
        uint8,
        dnx_data_field.tcam.nof_small_bank_lines_per_sram_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        ((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id,
        "dnx_field_tcam_hit_indication[%d]->tcam_action_hit_indication_small[%d].bank_line_id",
        unit, tcam_action_hit_indication_small_idx_0,
        dnx_data_field.tcam.nof_small_bank_lines_per_sram_get(unit) * sizeof(uint8) / sizeof(*((dnx_field_tcam_hit_indication_t*)sw_state_roots_array[unit][DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID])->tcam_action_hit_indication_small[tcam_action_hit_indication_small_idx_0].bank_line_id)+(dnx_data_field.tcam.nof_small_bank_lines_per_sram_get(unit) * sizeof(uint8)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_FIELD_TCAM_HIT_INDICATION_MODULE_ID,
        dnx_field_tcam_hit_indication_info,
        DNX_FIELD_TCAM_HIT_INDICATION_TCAM_ACTION_HIT_INDICATION_SMALL_BANK_LINE_ID_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_field_tcam_hit_indication_cbs dnx_field_tcam_hit_indication = 	{
	
	dnx_field_tcam_hit_indication_is_init,
	dnx_field_tcam_hit_indication_init,
		{
		
		dnx_field_tcam_hit_indication_tcam_action_hit_indication_alloc,
			{
			
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_set,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_get,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_bank_line_id_alloc}
		}
	,
		{
		
		dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_alloc,
			{
			
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_set,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_get,
			dnx_field_tcam_hit_indication_tcam_action_hit_indication_small_bank_line_id_alloc}
		}
	}
;
#undef BSL_LOG_MODULE

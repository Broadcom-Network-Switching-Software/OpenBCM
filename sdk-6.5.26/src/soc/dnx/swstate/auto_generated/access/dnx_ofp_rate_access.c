
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ofp_rate_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_ofp_rate_layout.h>





int
dnx_ofp_rate_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_OFP_RATE_DB,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]),
        "dnx_ofp_rate_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_ofp_rate_db_t,
        DNX_SW_STATE_DNX_OFP_RATE_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ofp_rate_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_ofp_rate_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_OFP_RATE_DB,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]),
        "dnx_ofp_rate_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]),
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_alloc(int unit, uint32 otm_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        dnx_ofp_rate_db_otm_t,
        dnx_data_egr_queuing.params.nof_q_pairs_get(unit),
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ofp_rate_db_otm_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        "dnx_ofp_rate_db[%d]->otm[%d]",
        unit, otm_idx_0,
        dnx_data_egr_queuing.params.nof_q_pairs_get(unit) * sizeof(dnx_ofp_rate_db_otm_t) / sizeof(*((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_instance_set(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 cal_instance)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].cal_instance,
        cal_instance,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_cal_instance_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        &cal_instance,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].cal_instance",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_instance_get(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *cal_instance)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        cal_instance);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        DNX_SW_STATE_DIAG_READ);

    *cal_instance = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].cal_instance;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].cal_instance,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].cal_instance",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__CAL_INSTANCE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_INSTANCE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_shaping_rate_set(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.rate,
        rate,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_shaping_rate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        &rate,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].shaping.rate",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_SHAPING_RATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_shaping_rate_get(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        rate);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        DNX_SW_STATE_DIAG_READ);

    *rate = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.rate;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.rate,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].shaping.rate",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_SHAPING_RATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_shaping_burst_set(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 burst)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.burst,
        burst,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_shaping_burst_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        &burst,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].shaping.burst",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_SHAPING_BURST_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_shaping_burst_get(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *burst)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        burst);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        DNX_SW_STATE_DIAG_READ);

    *burst = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.burst;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.burst,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].shaping.burst",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__BURST,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_SHAPING_BURST_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_shaping_valid_set(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.valid,
        valid,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_shaping_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        &valid,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].shaping.valid",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_SHAPING_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_shaping_valid_get(int unit, uint32 otm_idx_0, uint32 otm_idx_1, uint32 *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        valid);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0],
        otm_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm[otm_idx_0][otm_idx_1].shaping.valid,
        "dnx_ofp_rate_db[%d]->otm[%d][%d].shaping.valid",
        unit, otm_idx_0, otm_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM__SHAPING__VALID,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_SHAPING_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_qpair_rate_set(int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[qpair_idx_0][qpair_idx_1].rate,
        rate,
        uint32,
        0,
        "dnx_ofp_rate_db_qpair_rate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        &rate,
        "dnx_ofp_rate_db[%d]->qpair[%d][%d].rate",
        unit, qpair_idx_0, qpair_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_QPAIR_RATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_qpair_rate_get(int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 *rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        rate);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        DNX_SW_STATE_DIAG_READ);

    *rate = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[qpair_idx_0][qpair_idx_1].rate;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[qpair_idx_0][qpair_idx_1].rate,
        "dnx_ofp_rate_db[%d]->qpair[%d][%d].rate",
        unit, qpair_idx_0, qpair_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_QPAIR_RATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_qpair_valid_set(int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[qpair_idx_0][qpair_idx_1].valid,
        valid,
        uint32,
        0,
        "dnx_ofp_rate_db_qpair_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        &valid,
        "dnx_ofp_rate_db[%d]->qpair[%d][%d].valid",
        unit, qpair_idx_0, qpair_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_QPAIR_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_qpair_valid_get(int unit, uint32 qpair_idx_0, uint32 qpair_idx_1, uint32 *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        valid);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        qpair_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_Q_PAIRS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[qpair_idx_0][qpair_idx_1].valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->qpair[qpair_idx_0][qpair_idx_1].valid,
        "dnx_ofp_rate_db[%d]->qpair[%d][%d].valid",
        unit, qpair_idx_0, qpair_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__QPAIR__VALID,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_QPAIR_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_tcg_rate_set(int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_2,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[tcg_idx_0][tcg_idx_1][tcg_idx_2].rate,
        rate,
        uint32,
        0,
        "dnx_ofp_rate_db_tcg_rate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        &rate,
        "dnx_ofp_rate_db[%d]->tcg[%d][%d][%d].rate",
        unit, tcg_idx_0, tcg_idx_1, tcg_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_TCG_RATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_tcg_rate_get(int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 *rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        rate);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_2,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        DNX_SW_STATE_DIAG_READ);

    *rate = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[tcg_idx_0][tcg_idx_1][tcg_idx_2].rate;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[tcg_idx_0][tcg_idx_1][tcg_idx_2].rate,
        "dnx_ofp_rate_db[%d]->tcg[%d][%d][%d].rate",
        unit, tcg_idx_0, tcg_idx_1, tcg_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_TCG_RATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_tcg_valid_set(int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_2,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[tcg_idx_0][tcg_idx_1][tcg_idx_2].valid,
        valid,
        uint32,
        0,
        "dnx_ofp_rate_db_tcg_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        &valid,
        "dnx_ofp_rate_db[%d]->tcg[%d][%d][%d].valid",
        unit, tcg_idx_0, tcg_idx_1, tcg_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_TCG_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_tcg_valid_get(int unit, uint32 tcg_idx_0, uint32 tcg_idx_1, uint32 tcg_idx_2, uint32 *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        valid);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_PORT_SCHEDULERS);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        tcg_idx_2,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_TCG);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[tcg_idx_0][tcg_idx_1][tcg_idx_2].valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->tcg[tcg_idx_0][tcg_idx_1][tcg_idx_2].valid,
        "dnx_ofp_rate_db[%d]->tcg[%d][%d][%d].valid",
        unit, tcg_idx_0, tcg_idx_1, tcg_idx_2);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__TCG__VALID,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_TCG_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_rate_set(int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].rate,
        rate,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_cal_rate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        &rate,
        "dnx_ofp_rate_db[%d]->otm_cal[%d][%d].rate",
        unit, otm_cal_idx_0, otm_cal_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_RATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_rate_get(int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 *rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        rate);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        DNX_SW_STATE_DIAG_READ);

    *rate = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].rate;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].rate,
        "dnx_ofp_rate_db[%d]->otm_cal[%d][%d].rate",
        unit, otm_cal_idx_0, otm_cal_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__RATE,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_RATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_modified_set(int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 modified)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].modified,
        modified,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_cal_modified_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        &modified,
        "dnx_ofp_rate_db[%d]->otm_cal[%d][%d].modified",
        unit, otm_cal_idx_0, otm_cal_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_modified_get(int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 *modified)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        modified);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        DNX_SW_STATE_DIAG_READ);

    *modified = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].modified;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].modified,
        "dnx_ofp_rate_db[%d]->otm_cal[%d][%d].modified",
        unit, otm_cal_idx_0, otm_cal_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__MODIFIED,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_nof_calcal_instances_set(int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 nof_calcal_instances)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].nof_calcal_instances,
        nof_calcal_instances,
        uint32,
        0,
        "dnx_ofp_rate_db_otm_cal_nof_calcal_instances_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        &nof_calcal_instances,
        "dnx_ofp_rate_db[%d]->otm_cal[%d][%d].nof_calcal_instances",
        unit, otm_cal_idx_0, otm_cal_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_NOF_CALCAL_INSTANCES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_otm_cal_nof_calcal_instances_get(int unit, uint32 otm_cal_idx_0, uint32 otm_cal_idx_1, uint32 *nof_calcal_instances)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        nof_calcal_instances);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        otm_cal_idx_1,
        DNX_DATA_MAX_EGR_QUEUING_PARAMS_NOF_CALENDARS);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        DNX_SW_STATE_DIAG_READ);

    *nof_calcal_instances = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].nof_calcal_instances;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->otm_cal[otm_cal_idx_0][otm_cal_idx_1].nof_calcal_instances,
        "dnx_ofp_rate_db[%d]->otm_cal[%d][%d].nof_calcal_instances",
        unit, otm_cal_idx_0, otm_cal_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__OTM_CAL__NOF_CALCAL_INSTANCES,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_OTM_CAL_NOF_CALCAL_INSTANCES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_calcal_is_modified_set(int unit, uint32 calcal_is_modified_idx_0, uint32 calcal_is_modified)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        calcal_is_modified_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified[calcal_is_modified_idx_0],
        calcal_is_modified,
        uint32,
        0,
        "dnx_ofp_rate_db_calcal_is_modified_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        &calcal_is_modified,
        "dnx_ofp_rate_db[%d]->calcal_is_modified[%d]",
        unit, calcal_is_modified_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_CALCAL_IS_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_calcal_is_modified_get(int unit, uint32 calcal_is_modified_idx_0, uint32 *calcal_is_modified)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        calcal_is_modified_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        calcal_is_modified);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        DNX_SW_STATE_DIAG_READ);

    *calcal_is_modified = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified[calcal_is_modified_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_is_modified[calcal_is_modified_idx_0],
        "dnx_ofp_rate_db[%d]->calcal_is_modified[%d]",
        unit, calcal_is_modified_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_IS_MODIFIED,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_CALCAL_IS_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_calcal_len_set(int unit, uint32 calcal_len_idx_0, uint32 calcal_len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        calcal_len_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_len[calcal_len_idx_0],
        calcal_len,
        uint32,
        0,
        "dnx_ofp_rate_db_calcal_len_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        &calcal_len,
        "dnx_ofp_rate_db[%d]->calcal_len[%d]",
        unit, calcal_len_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_CALCAL_LEN_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ofp_rate_db_calcal_len_get(int unit, uint32 calcal_len_idx_0, uint32 *calcal_len)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        calcal_len_idx_0,
        DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        calcal_len);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        DNX_SW_STATE_DIAG_READ);

    *calcal_len = ((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_len[calcal_len_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        &((dnx_ofp_rate_db_t*)sw_state_roots_array[unit][DNX_OFP_RATE_MODULE_ID])->calcal_len[calcal_len_idx_0],
        "dnx_ofp_rate_db[%d]->calcal_len[%d]",
        unit, calcal_len_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_OFP_RATE_DB__CALCAL_LEN,
        dnx_ofp_rate_db_info,
        DNX_OFP_RATE_DB_CALCAL_LEN_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_OFP_RATE_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_ofp_rate_db_cbs dnx_ofp_rate_db = 	{
	
	dnx_ofp_rate_db_is_init,
	dnx_ofp_rate_db_init,
		{
		
		dnx_ofp_rate_db_otm_alloc,
			{
			
			dnx_ofp_rate_db_otm_cal_instance_set,
			dnx_ofp_rate_db_otm_cal_instance_get}
		,
			{
			
				{
				
				dnx_ofp_rate_db_otm_shaping_rate_set,
				dnx_ofp_rate_db_otm_shaping_rate_get}
			,
				{
				
				dnx_ofp_rate_db_otm_shaping_burst_set,
				dnx_ofp_rate_db_otm_shaping_burst_get}
			,
				{
				
				dnx_ofp_rate_db_otm_shaping_valid_set,
				dnx_ofp_rate_db_otm_shaping_valid_get}
			}
		}
	,
		{
		
			{
			
			dnx_ofp_rate_db_qpair_rate_set,
			dnx_ofp_rate_db_qpair_rate_get}
		,
			{
			
			dnx_ofp_rate_db_qpair_valid_set,
			dnx_ofp_rate_db_qpair_valid_get}
		}
	,
		{
		
			{
			
			dnx_ofp_rate_db_tcg_rate_set,
			dnx_ofp_rate_db_tcg_rate_get}
		,
			{
			
			dnx_ofp_rate_db_tcg_valid_set,
			dnx_ofp_rate_db_tcg_valid_get}
		}
	,
		{
		
			{
			
			dnx_ofp_rate_db_otm_cal_rate_set,
			dnx_ofp_rate_db_otm_cal_rate_get}
		,
			{
			
			dnx_ofp_rate_db_otm_cal_modified_set,
			dnx_ofp_rate_db_otm_cal_modified_get}
		,
			{
			
			dnx_ofp_rate_db_otm_cal_nof_calcal_instances_set,
			dnx_ofp_rate_db_otm_cal_nof_calcal_instances_get}
		}
	,
		{
		
		dnx_ofp_rate_db_calcal_is_modified_set,
		dnx_ofp_rate_db_calcal_is_modified_get}
	,
		{
		
		dnx_ofp_rate_db_calcal_len_set,
		dnx_ofp_rate_db_calcal_len_get}
	}
;
#undef BSL_LOG_MODULE

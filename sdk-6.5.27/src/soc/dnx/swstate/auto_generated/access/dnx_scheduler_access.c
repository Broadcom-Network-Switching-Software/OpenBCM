
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_scheduler_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_scheduler_layout.h>





int
dnx_scheduler_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_SCHEDULER_DB,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]),
        "dnx_scheduler_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_scheduler_db_t,
        DNX_SW_STATE_DNX_SCHEDULER_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_scheduler_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_SCHEDULER_DB,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]),
        "dnx_scheduler_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_enabled,
        SCH_VIRTUAL_FLOWS_UNINITALIZED);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_port_shaper_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_sch.flow.nof_hr_get(unit),
        sizeof(dnx_scheduler_db_shaper_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper,
        dnx_scheduler_db_shaper_t*,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_port_shaper_alloc");

    for(uint32 port_shaper_idx_0 = 0;
         port_shaper_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         port_shaper_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0],
        dnx_scheduler_db_shaper_t,
        dnx_data_sch.flow.nof_hr_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_port_shaper_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper,
        "dnx_scheduler_db[%d]->port_shaper",
        unit,
        dnx_data_sch.flow.nof_hr_get(unit) * sizeof(dnx_scheduler_db_shaper_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper)+(dnx_data_sch.flow.nof_hr_get(unit) * sizeof(dnx_scheduler_db_shaper_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_SHAPER_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_port_shaper_rate_set(int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper,
        port_shaper_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0],
        port_shaper_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0][port_shaper_idx_1].rate,
        rate,
        int,
        0,
        "dnx_scheduler_db_port_shaper_rate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        &rate,
        "dnx_scheduler_db[%d]->port_shaper[%d][%d].rate",
        unit, port_shaper_idx_0, port_shaper_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_SHAPER_RATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_port_shaper_rate_get(int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int *rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper,
        port_shaper_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        rate);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0],
        port_shaper_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        DNX_SW_STATE_DIAG_READ);

    *rate = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0][port_shaper_idx_1].rate;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0][port_shaper_idx_1].rate,
        "dnx_scheduler_db[%d]->port_shaper[%d][%d].rate",
        unit, port_shaper_idx_0, port_shaper_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__RATE,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_SHAPER_RATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_port_shaper_valid_set(int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper,
        port_shaper_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0],
        port_shaper_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0][port_shaper_idx_1].valid,
        valid,
        int,
        0,
        "dnx_scheduler_db_port_shaper_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        &valid,
        "dnx_scheduler_db[%d]->port_shaper[%d][%d].valid",
        unit, port_shaper_idx_0, port_shaper_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_SHAPER_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_port_shaper_valid_get(int unit, uint32 port_shaper_idx_0, uint32 port_shaper_idx_1, int *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper,
        port_shaper_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        valid);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0],
        port_shaper_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0][port_shaper_idx_1].valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port_shaper[port_shaper_idx_0][port_shaper_idx_1].valid,
        "dnx_scheduler_db[%d]->port_shaper[%d][%d].valid",
        unit, port_shaper_idx_0, port_shaper_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__PORT_SHAPER__VALID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_SHAPER_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_hr_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_sch.flow.nof_hr_get(unit),
        sizeof(dnx_scheduler_db_hr_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        dnx_scheduler_db_hr_info_t*,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_hr_alloc");

    for(uint32 hr_idx_0 = 0;
         hr_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         hr_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0],
        dnx_scheduler_db_hr_info_t,
        dnx_data_sch.flow.nof_hr_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_hr_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        "dnx_scheduler_db[%d]->hr",
        unit,
        dnx_data_sch.flow.nof_hr_get(unit) * sizeof(dnx_scheduler_db_hr_info_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr)+(dnx_data_sch.flow.nof_hr_get(unit) * sizeof(dnx_scheduler_db_hr_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_HR_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_hr_is_colored_set(int unit, uint32 hr_idx_0, uint32 hr_idx_1, int is_colored)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        hr_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0],
        hr_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0][hr_idx_1].is_colored,
        is_colored,
        int,
        0,
        "dnx_scheduler_db_hr_is_colored_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        &is_colored,
        "dnx_scheduler_db[%d]->hr[%d][%d].is_colored",
        unit, hr_idx_0, hr_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_HR_IS_COLORED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_hr_is_colored_get(int unit, uint32 hr_idx_0, uint32 hr_idx_1, int *is_colored)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        hr_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        is_colored);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0],
        hr_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        DNX_SW_STATE_DIAG_READ);

    *is_colored = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0][hr_idx_1].is_colored;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0][hr_idx_1].is_colored,
        "dnx_scheduler_db[%d]->hr[%d][%d].is_colored",
        unit, hr_idx_0, hr_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__HR__IS_COLORED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_HR_IS_COLORED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        dnx_scheduler_db_interface_info_t,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_interface_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        "dnx_scheduler_db[%d]->interface",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_scheduler_db_interface_info_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_alloc_bitmap(int unit, uint32 interface_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        dnx_data_sch.interface.nof_sch_interfaces_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_interface_modified_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0,
        dnx_data_sch.interface.nof_sch_interfaces_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_set(int unit, uint32 interface_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_clear(int unit, uint32 interface_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_get(int unit, uint32 interface_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_read(int unit, uint32 interface_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_write(int unit, uint32 interface_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_and(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_or(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_xor(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_remove(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_negate(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_clear(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_set(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_null(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_test(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_eq(int unit, uint32 interface_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_interface_modified_bit_range_count(int unit, uint32 interface_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__INTERFACE__MODIFIED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        dnx_scheduler_db_fmq_info,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_fmq_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        "dnx_scheduler_db[%d]->fmq",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_scheduler_db_fmq_info) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_reserved_hr_flow_id_set(int unit, uint32 fmq_idx_0, uint32 reserved_hr_flow_id_idx_0, int reserved_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id[reserved_hr_flow_id_idx_0],
        reserved_hr_flow_id,
        int,
        0,
        "dnx_scheduler_db_fmq_reserved_hr_flow_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        &reserved_hr_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].reserved_hr_flow_id[%d]",
        unit, fmq_idx_0, reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_reserved_hr_flow_id_get(int unit, uint32 fmq_idx_0, uint32 reserved_hr_flow_id_idx_0, int *reserved_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        reserved_hr_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        DNX_SW_STATE_DIAG_READ);

    *reserved_hr_flow_id = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id[reserved_hr_flow_id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id[reserved_hr_flow_id_idx_0],
        "dnx_scheduler_db[%d]->fmq[%d].reserved_hr_flow_id[%d]",
        unit, fmq_idx_0, reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc(int unit, uint32 fmq_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        int,
        dnx_data_sch.general.nof_fmq_class_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].reserved_hr_flow_id",
        unit, fmq_idx_0,
        dnx_data_sch.general.nof_fmq_class_get(unit) * sizeof(int) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__RESERVED_HR_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_nof_reserved_hr_set(int unit, uint32 fmq_idx_0, int nof_reserved_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        nof_reserved_hr,
        int,
        0,
        "dnx_scheduler_db_fmq_nof_reserved_hr_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        &nof_reserved_hr,
        "dnx_scheduler_db[%d]->fmq[%d].nof_reserved_hr",
        unit, fmq_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_nof_reserved_hr_get(int unit, uint32 fmq_idx_0, int *nof_reserved_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        nof_reserved_hr);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        DNX_SW_STATE_DIAG_READ);

    *nof_reserved_hr = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        "dnx_scheduler_db[%d]->fmq[%d].nof_reserved_hr",
        unit, fmq_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_nof_reserved_hr_inc(int unit, uint32 fmq_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        inc_value,
        int,
        0,
        "dnx_scheduler_db_fmq_nof_reserved_hr_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        "dnx_scheduler_db[%d]->fmq[%d].nof_reserved_hr",
        unit, fmq_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_nof_reserved_hr_dec(int unit, uint32 fmq_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        dec_value,
        int,
        0,
        "dnx_scheduler_db_fmq_nof_reserved_hr_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        "dnx_scheduler_db[%d]->fmq[%d].nof_reserved_hr",
        unit, fmq_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__NOF_RESERVED_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_fmq_flow_id_set(int unit, uint32 fmq_idx_0, uint32 fmq_flow_id_idx_0, int fmq_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        fmq_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[fmq_flow_id_idx_0],
        fmq_flow_id,
        int,
        0,
        "dnx_scheduler_db_fmq_fmq_flow_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        &fmq_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].fmq_flow_id[%d]",
        unit, fmq_idx_0, fmq_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_fmq_flow_id_get(int unit, uint32 fmq_idx_0, uint32 fmq_flow_id_idx_0, int *fmq_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        fmq_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        fmq_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        DNX_SW_STATE_DIAG_READ);

    *fmq_flow_id = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[fmq_flow_id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[fmq_flow_id_idx_0],
        "dnx_scheduler_db[%d]->fmq[%d].fmq_flow_id[%d]",
        unit, fmq_idx_0, fmq_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_fmq_fmq_flow_id_alloc(int unit, uint32 fmq_idx_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        int,
        dnx_data_sch.general.nof_fmq_class_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_fmq_fmq_flow_id_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].fmq_flow_id",
        unit, fmq_idx_0,
        dnx_data_sch.general.nof_fmq_class_get(unit) * sizeof(int) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__FMQ__FMQ_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_sch.general.nof_fmq_class_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[def_val_idx[0]],
        -1);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_enabled_set(int unit, sch_virtual_flows_sw_state_type_e is_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_enabled,
        is_enabled,
        sch_virtual_flows_sw_state_type_e,
        0,
        "dnx_scheduler_db_virtual_flows_is_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        &is_enabled,
        "dnx_scheduler_db[%d]->virtual_flows.is_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_enabled_get(int unit, sch_virtual_flows_sw_state_type_e *is_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        is_enabled);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *is_enabled = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_enabled,
        "dnx_scheduler_db[%d]->virtual_flows.is_enabled",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_ENABLED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_get(int unit, uint32 is_flow_virtual_idx_0, CONST SHR_BITDCL **is_flow_virtual)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        is_flow_virtual);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_READ);

    *is_flow_virtual = &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0][0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0][0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d][%d]",
        unit, is_flow_virtual_idx_0, 0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc_bitmap(int unit, uint32 is_flow_virtual_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        dnx_data_sch.flow.nof_flows_get(unit)/dnx_data_sch.flow.min_connector_bundle_size_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0,
        dnx_data_sch.flow.nof_flows_get(unit)/dnx_data_sch.flow.min_connector_bundle_size_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        SHR_BITDCL*,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(SHR_BITDCL));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_set(int unit, uint32 is_flow_virtual_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_clear(int unit, uint32 is_flow_virtual_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_get(int unit, uint32 is_flow_virtual_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        _bit_num);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_read(int unit, uint32 is_flow_virtual_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         sw_state_bmp_first,
         result_first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_READ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREAD,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_write(int unit, uint32 is_flow_virtual_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         sw_state_bmp_first,
         input_bmp_first,
          _range,
         input_bmp);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_WRITE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEWRITE,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_and(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_or(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_xor(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_remove(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_negate(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_clear(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_set(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_null(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_test(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_eq(int unit, uint32 is_flow_virtual_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _count - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _count,
         input_bmp,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEEQ,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_count(int unit, uint32 is_flow_virtual_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual,
        is_flow_virtual_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        (_first + _range - 1));

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        DNX_SW_STATE_DIAG_BITMAP);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->virtual_flows.is_flow_virtual[is_flow_virtual_idx_0],
        "dnx_scheduler_db[%d]->virtual_flows.is_flow_virtual[%d]",
        unit, is_flow_virtual_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__VIRTUAL_FLOWS__IS_FLOW_VIRTUAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_VIRTUAL_FLOWS_IS_FLOW_VIRTUAL_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        dnx_scheduler_db_general_info,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_general_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        "dnx_scheduler_db[%d]->general",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_scheduler_db_general_info) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_reserved_erp_hr_flow_id_set(int unit, uint32 general_idx_0, uint32 reserved_erp_hr_flow_id_idx_0, int reserved_erp_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        reserved_erp_hr_flow_id_idx_0,
        8);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].reserved_erp_hr_flow_id[reserved_erp_hr_flow_id_idx_0],
        reserved_erp_hr_flow_id,
        int,
        0,
        "dnx_scheduler_db_general_reserved_erp_hr_flow_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        &reserved_erp_hr_flow_id,
        "dnx_scheduler_db[%d]->general[%d].reserved_erp_hr_flow_id[%d]",
        unit, general_idx_0, reserved_erp_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_RESERVED_ERP_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_reserved_erp_hr_flow_id_get(int unit, uint32 general_idx_0, uint32 reserved_erp_hr_flow_id_idx_0, int *reserved_erp_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        reserved_erp_hr_flow_id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        reserved_erp_hr_flow_id_idx_0,
        8);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        DNX_SW_STATE_DIAG_READ);

    *reserved_erp_hr_flow_id = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].reserved_erp_hr_flow_id[reserved_erp_hr_flow_id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].reserved_erp_hr_flow_id[reserved_erp_hr_flow_id_idx_0],
        "dnx_scheduler_db[%d]->general[%d].reserved_erp_hr_flow_id[%d]",
        unit, general_idx_0, reserved_erp_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__RESERVED_ERP_HR_FLOW_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_RESERVED_ERP_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_nof_reserved_erp_hr_set(int unit, uint32 general_idx_0, int nof_reserved_erp_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        nof_reserved_erp_hr,
        int,
        0,
        "dnx_scheduler_db_general_nof_reserved_erp_hr_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        &nof_reserved_erp_hr,
        "dnx_scheduler_db[%d]->general[%d].nof_reserved_erp_hr",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_nof_reserved_erp_hr_get(int unit, uint32 general_idx_0, int *nof_reserved_erp_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        nof_reserved_erp_hr);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        DNX_SW_STATE_DIAG_READ);

    *nof_reserved_erp_hr = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        "dnx_scheduler_db[%d]->general[%d].nof_reserved_erp_hr",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_nof_reserved_erp_hr_inc(int unit, uint32 general_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        inc_value,
        int,
        0,
        "dnx_scheduler_db_general_nof_reserved_erp_hr_inc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_INC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_INC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        "dnx_scheduler_db[%d]->general[%d].nof_reserved_erp_hr",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_nof_reserved_erp_hr_dec(int unit, uint32 general_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        dec_value,
        int,
        0,
        "dnx_scheduler_db_general_nof_reserved_erp_hr_dec");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_COUNTER_DEC_LOGGING,
        BSL_LS_SWSTATEDNX_COUNTER_DEC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        "dnx_scheduler_db[%d]->general[%d].nof_reserved_erp_hr",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NOF_RESERVED_ERP_HR,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_non_default_flow_order_set(int unit, uint32 general_idx_0, uint8 non_default_flow_order)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].non_default_flow_order,
        non_default_flow_order,
        uint8,
        0,
        "dnx_scheduler_db_general_non_default_flow_order_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        &non_default_flow_order,
        "dnx_scheduler_db[%d]->general[%d].non_default_flow_order",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NON_DEFAULT_FLOW_ORDER_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_non_default_flow_order_get(int unit, uint32 general_idx_0, uint8 *non_default_flow_order)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        non_default_flow_order);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        DNX_SW_STATE_DIAG_READ);

    *non_default_flow_order = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].non_default_flow_order;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].non_default_flow_order,
        "dnx_scheduler_db[%d]->general[%d].non_default_flow_order",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__NON_DEFAULT_FLOW_ORDER,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NON_DEFAULT_FLOW_ORDER_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_is_composite_fq_supported_set(int unit, uint32 general_idx_0, uint8 is_composite_fq_supported)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].is_composite_fq_supported,
        is_composite_fq_supported,
        uint8,
        0,
        "dnx_scheduler_db_general_is_composite_fq_supported_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        &is_composite_fq_supported,
        "dnx_scheduler_db[%d]->general[%d].is_composite_fq_supported",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_IS_COMPOSITE_FQ_SUPPORTED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_general_is_composite_fq_supported_get(int unit, uint32 general_idx_0, uint8 *is_composite_fq_supported)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        is_composite_fq_supported);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        DNX_SW_STATE_DIAG_READ);

    *is_composite_fq_supported = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].is_composite_fq_supported;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].is_composite_fq_supported,
        "dnx_scheduler_db[%d]->general[%d].is_composite_fq_supported",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__GENERAL__IS_COMPOSITE_FQ_SUPPORTED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_IS_COMPOSITE_FQ_SUPPORTED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_alloc(int unit)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_sch.flow.nof_low_rate_ranges_get(unit),
        sizeof(dnx_scheduler_db_low_rate_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        dnx_scheduler_db_low_rate_info_t*,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_low_rate_ranges_alloc");

    for(uint32 low_rate_ranges_idx_0 = 0;
         low_rate_ranges_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         low_rate_ranges_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        dnx_scheduler_db_low_rate_info_t,
        dnx_data_sch.flow.nof_low_rate_ranges_get(unit),
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_low_rate_ranges_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        "dnx_scheduler_db[%d]->low_rate_ranges",
        unit,
        dnx_data_sch.flow.nof_low_rate_ranges_get(unit) * sizeof(dnx_scheduler_db_low_rate_info_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges)+(dnx_data_sch.flow.nof_low_rate_ranges_get(unit) * sizeof(dnx_scheduler_db_low_rate_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_device.general.nof_cores_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[1], dnx_data_sch.flow.nof_low_rate_ranges_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[def_val_idx[0]][def_val_idx[1]].pattern,
        dnx_data_sch.flow.low_rate_pattern_mask_get(unit));

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_pattern_set(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 pattern)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].pattern,
        pattern,
        uint32,
        0,
        "dnx_scheduler_db_low_rate_ranges_pattern_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        &pattern,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].pattern",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_PATTERN_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_pattern_get(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *pattern)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        pattern);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        DNX_SW_STATE_DIAG_READ);

    *pattern = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].pattern;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].pattern,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].pattern",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__PATTERN,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_PATTERN_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_range_start_set(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 range_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].range_start,
        range_start,
        uint32,
        0,
        "dnx_scheduler_db_low_rate_ranges_range_start_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        &range_start,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].range_start",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_RANGE_START_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_range_start_get(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *range_start)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        range_start);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        DNX_SW_STATE_DIAG_READ);

    *range_start = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].range_start;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].range_start,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].range_start",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_START,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_RANGE_START_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_range_end_set(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 range_end)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].range_end,
        range_end,
        uint32,
        0,
        "dnx_scheduler_db_low_rate_ranges_range_end_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        &range_end,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].range_end",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_RANGE_END_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_range_end_get(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *range_end)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        range_end);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        DNX_SW_STATE_DIAG_READ);

    *range_end = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].range_end;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].range_end,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].range_end",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__RANGE_END,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_RANGE_END_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_is_enabled_set(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 is_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].is_enabled,
        is_enabled,
        uint32,
        0,
        "dnx_scheduler_db_low_rate_ranges_is_enabled_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        &is_enabled,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].is_enabled",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_IS_ENABLED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_scheduler_db_low_rate_ranges_is_enabled_get(int unit, uint32 low_rate_ranges_idx_0, uint32 low_rate_ranges_idx_1, uint32 *is_enabled)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges,
        low_rate_ranges_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        is_enabled);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0],
        low_rate_ranges_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        DNX_SW_STATE_DIAG_READ);

    *is_enabled = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].is_enabled;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->low_rate_ranges[low_rate_ranges_idx_0][low_rate_ranges_idx_1].is_enabled,
        "dnx_scheduler_db[%d]->low_rate_ranges[%d][%d].is_enabled",
        unit, low_rate_ranges_idx_0, low_rate_ranges_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_SCHEDULER_DB__LOW_RATE_RANGES__IS_ENABLED,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_LOW_RATE_RANGES_IS_ENABLED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



const char *
sch_virtual_flows_sw_state_type_e_get_name(sch_virtual_flows_sw_state_type_e value)
{
    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("SCH_VIRTUAL_FLOWS_UNINITALIZED", value, SCH_VIRTUAL_FLOWS_UNINITALIZED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("SCH_VIRTUAL_FLOWS_ENABLED", value, SCH_VIRTUAL_FLOWS_ENABLED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("SCH_VIRTUAL_FLOWS_DISABLED", value, SCH_VIRTUAL_FLOWS_DISABLED);

    DNX_SW_STATE_ENUM_COMPARE_AND_GET_NAME("SCH_NOF_VIRTUAL_FLOWS_TYPES", value, SCH_NOF_VIRTUAL_FLOWS_TYPES);

    return NULL;
}




dnx_scheduler_db_cbs dnx_scheduler_db = 	{
	
	dnx_scheduler_db_is_init,
	dnx_scheduler_db_init,
		{
		
		dnx_scheduler_db_port_shaper_alloc,
			{
			
			dnx_scheduler_db_port_shaper_rate_set,
			dnx_scheduler_db_port_shaper_rate_get}
		,
			{
			
			dnx_scheduler_db_port_shaper_valid_set,
			dnx_scheduler_db_port_shaper_valid_get}
		}
	,
		{
		
		dnx_scheduler_db_hr_alloc,
			{
			
			dnx_scheduler_db_hr_is_colored_set,
			dnx_scheduler_db_hr_is_colored_get}
		}
	,
		{
		
		dnx_scheduler_db_interface_alloc,
			{
			
			dnx_scheduler_db_interface_modified_alloc_bitmap,
			dnx_scheduler_db_interface_modified_bit_set,
			dnx_scheduler_db_interface_modified_bit_clear,
			dnx_scheduler_db_interface_modified_bit_get,
			dnx_scheduler_db_interface_modified_bit_range_read,
			dnx_scheduler_db_interface_modified_bit_range_write,
			dnx_scheduler_db_interface_modified_bit_range_and,
			dnx_scheduler_db_interface_modified_bit_range_or,
			dnx_scheduler_db_interface_modified_bit_range_xor,
			dnx_scheduler_db_interface_modified_bit_range_remove,
			dnx_scheduler_db_interface_modified_bit_range_negate,
			dnx_scheduler_db_interface_modified_bit_range_clear,
			dnx_scheduler_db_interface_modified_bit_range_set,
			dnx_scheduler_db_interface_modified_bit_range_null,
			dnx_scheduler_db_interface_modified_bit_range_test,
			dnx_scheduler_db_interface_modified_bit_range_eq,
			dnx_scheduler_db_interface_modified_bit_range_count}
		}
	,
		{
		
		dnx_scheduler_db_fmq_alloc,
			{
			
			dnx_scheduler_db_fmq_reserved_hr_flow_id_set,
			dnx_scheduler_db_fmq_reserved_hr_flow_id_get,
			dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc}
		,
			{
			
			dnx_scheduler_db_fmq_nof_reserved_hr_set,
			dnx_scheduler_db_fmq_nof_reserved_hr_get,
			dnx_scheduler_db_fmq_nof_reserved_hr_inc,
			dnx_scheduler_db_fmq_nof_reserved_hr_dec}
		,
			{
			
			dnx_scheduler_db_fmq_fmq_flow_id_set,
			dnx_scheduler_db_fmq_fmq_flow_id_get,
			dnx_scheduler_db_fmq_fmq_flow_id_alloc}
		}
	,
		{
		
			{
			
			dnx_scheduler_db_virtual_flows_is_enabled_set,
			dnx_scheduler_db_virtual_flows_is_enabled_get}
		,
			{
			
			dnx_scheduler_db_virtual_flows_is_flow_virtual_get,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc_bitmap,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_alloc,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_set,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_clear,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_get,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_read,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_write,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_and,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_or,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_xor,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_remove,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_negate,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_clear,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_set,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_null,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_test,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_eq,
			dnx_scheduler_db_virtual_flows_is_flow_virtual_bit_range_count}
		}
	,
		{
		
		dnx_scheduler_db_general_alloc,
			{
			
			dnx_scheduler_db_general_reserved_erp_hr_flow_id_set,
			dnx_scheduler_db_general_reserved_erp_hr_flow_id_get}
		,
			{
			
			dnx_scheduler_db_general_nof_reserved_erp_hr_set,
			dnx_scheduler_db_general_nof_reserved_erp_hr_get,
			dnx_scheduler_db_general_nof_reserved_erp_hr_inc,
			dnx_scheduler_db_general_nof_reserved_erp_hr_dec}
		,
			{
			
			dnx_scheduler_db_general_non_default_flow_order_set,
			dnx_scheduler_db_general_non_default_flow_order_get}
		,
			{
			
			dnx_scheduler_db_general_is_composite_fq_supported_set,
			dnx_scheduler_db_general_is_composite_fq_supported_get}
		}
	,
		{
		
		dnx_scheduler_db_low_rate_ranges_alloc,
			{
			
			dnx_scheduler_db_low_rate_ranges_pattern_set,
			dnx_scheduler_db_low_rate_ranges_pattern_get}
		,
			{
			
			dnx_scheduler_db_low_rate_ranges_range_start_set,
			dnx_scheduler_db_low_rate_ranges_range_start_get}
		,
			{
			
			dnx_scheduler_db_low_rate_ranges_range_end_set,
			dnx_scheduler_db_low_rate_ranges_range_end_get}
		,
			{
			
			dnx_scheduler_db_low_rate_ranges_is_enabled_set,
			dnx_scheduler_db_low_rate_ranges_is_enabled_get}
		}
	}
;
#undef BSL_LOG_MODULE

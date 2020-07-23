
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
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_scheduler_diagnostic.h>



dnx_scheduler_db_t* dnx_scheduler_db_dummy = NULL;



int
dnx_scheduler_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SCHEDULER_MODULE_ID,
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
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_scheduler_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]),
        "dnx_scheduler_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_scheduler_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port,
        dnx_scheduler_db_shaper_t,
        dnx_data_port.general.fabric_port_base_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_port_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port,
        "dnx_scheduler_db[%d]->port",
        unit,
        dnx_data_port.general.fabric_port_base_get(unit) * sizeof(dnx_scheduler_db_shaper_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_rate_set(int unit, uint32 port_idx_0, int rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port,
        port_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[port_idx_0].rate,
        rate,
        int,
        0,
        "dnx_scheduler_db_port_rate_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &rate,
        "dnx_scheduler_db[%d]->port[%d].rate",
        unit, port_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_RATE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_rate_get(int unit, uint32 port_idx_0, int *rate)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port,
        port_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        rate);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *rate = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[port_idx_0].rate;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[port_idx_0].rate,
        "dnx_scheduler_db[%d]->port[%d].rate",
        unit, port_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_RATE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_valid_set(int unit, uint32 port_idx_0, int valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port,
        port_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[port_idx_0].valid,
        valid,
        int,
        0,
        "dnx_scheduler_db_port_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &valid,
        "dnx_scheduler_db[%d]->port[%d].valid",
        unit, port_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_port_valid_get(int unit, uint32 port_idx_0, int *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port,
        port_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        valid);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *valid = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[port_idx_0].valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->port[port_idx_0].valid,
        "dnx_scheduler_db[%d]->port[%d].valid",
        unit, port_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_PORT_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_hr_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        dnx_scheduler_db_hr_info_t*,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_hr_alloc");

    for(uint32 hr_idx_0 = 0;
         hr_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         hr_idx_0++)

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0],
        dnx_scheduler_db_hr_info_t,
        dnx_data_sch.flow.nof_hr_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_hr_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        "dnx_scheduler_db[%d]->hr",
        unit,
        dnx_data_sch.flow.nof_hr_get(unit) * sizeof(dnx_scheduler_db_hr_info_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr)+(dnx_data_sch.flow.nof_hr_get(unit) * sizeof(dnx_scheduler_db_hr_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_HR_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_hr_is_colored_set(int unit, uint32 hr_idx_0, uint32 hr_idx_1, int is_colored)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        hr_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0],
        hr_idx_1);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0][hr_idx_1].is_colored,
        is_colored,
        int,
        0,
        "dnx_scheduler_db_hr_is_colored_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &is_colored,
        "dnx_scheduler_db[%d]->hr[%d][%d].is_colored",
        unit, hr_idx_0, hr_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_HR_IS_COLORED_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_hr_is_colored_get(int unit, uint32 hr_idx_0, uint32 hr_idx_1, int *is_colored)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr,
        hr_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        is_colored);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0],
        hr_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *is_colored = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0][hr_idx_1].is_colored;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->hr[hr_idx_0][hr_idx_1].is_colored,
        "dnx_scheduler_db[%d]->hr[%d][%d].is_colored",
        unit, hr_idx_0, hr_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_HR_IS_COLORED_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        dnx_scheduler_db_interface_info_t,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_interface_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        "dnx_scheduler_db[%d]->interface",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_scheduler_db_interface_info_t) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_alloc_bitmap(int unit, uint32 interface_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC_BITMAP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC_BITMAP(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        dnx_data_sch.interface.nof_sch_interfaces_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_interface_modified_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_BITMAP_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOCBITMAP,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0,
        dnx_data_sch.interface.nof_sch_interfaces_get(unit));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_ALLOC_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_set(int unit, uint32 interface_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_SET(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITSET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_clear(int unit, uint32 interface_idx_0, uint32 _bit_num)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_CLEAR(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _bit_num);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITCLEAR,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_get(int unit, uint32 interface_idx_0, uint32 _bit_num, uint8* result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        _bit_num);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_GET(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _bit_num,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_GET_LOGGING,
        BSL_LS_SWSTATEDNX_BITGET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_read(int unit, uint32 interface_idx_0, uint32 sw_state_bmp_first, uint32 result_first, uint32 _range, SHR_BITDCL *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_READ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_READ(
        unit,
         DNX_SCHEDULER_MODULE_ID,
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
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_write(int unit, uint32 interface_idx_0, uint32 sw_state_bmp_first, uint32 input_bmp_first, uint32 _range, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_WRITE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (sw_state_bmp_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_WRITE(
        unit,
         DNX_SCHEDULER_MODULE_ID,
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
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_and(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_AND(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_AND_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEAND,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_or(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_OR(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_OR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEOR,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_xor(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_XOR(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEXOR,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_remove(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, SHR_BITDCL *input_bmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_REMOVE(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         input_bmp,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGEREMOVE,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_negate(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NEGATE(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENEGATE,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_clear(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_CLEAR(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECLEAR,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_set(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_SET(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGESET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_null(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_NULL(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGENULL,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_test(int unit, uint32 interface_idx_0, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_TEST,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_TEST(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _count,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_TEST_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGETEST,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_eq(int unit, uint32 interface_idx_0, SHR_BITDCL *input_bmp, uint32 _first, uint32 _count, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _count - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_EQ(
        unit,
         DNX_SCHEDULER_MODULE_ID,
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
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_interface_modified_bit_range_count(int unit, uint32 interface_idx_0, uint32 _first, uint32 _range, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_BIT_RANGE_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface,
        interface_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified);

    DNX_SW_STATE_OOB_DYNAMIC_BITMAP_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        (_first + _range - 1));

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_BIT_RANGE_COUNT(
        unit,
         DNX_SCHEDULER_MODULE_ID,
         0,
         ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
         _first,
         _range,
         result);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_BIT_RANGE_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_BITRANGECOUNT,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->interface[interface_idx_0].modified,
        "dnx_scheduler_db[%d]->interface[%d].modified",
        unit, interface_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_INTERFACE_MODIFIED_INFO,
        DNX_SW_STATE_DIAG_BITMAP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        dnx_scheduler_db_fmq_info,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_fmq_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        "dnx_scheduler_db[%d]->fmq",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_scheduler_db_fmq_info) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_reserved_hr_flow_id_set(int unit, uint32 fmq_idx_0, uint32 reserved_hr_flow_id_idx_0, int reserved_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id[reserved_hr_flow_id_idx_0],
        reserved_hr_flow_id,
        int,
        0,
        "dnx_scheduler_db_fmq_reserved_hr_flow_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &reserved_hr_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].reserved_hr_flow_id[%d]",
        unit, fmq_idx_0, reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_reserved_hr_flow_id_get(int unit, uint32 fmq_idx_0, uint32 reserved_hr_flow_id_idx_0, int *reserved_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        reserved_hr_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *reserved_hr_flow_id = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id[reserved_hr_flow_id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id[reserved_hr_flow_id_idx_0],
        "dnx_scheduler_db[%d]->fmq[%d].reserved_hr_flow_id[%d]",
        unit, fmq_idx_0, reserved_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc(int unit, uint32 fmq_idx_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        int,
        dnx_data_sch.general.nof_fmq_class_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_fmq_reserved_hr_flow_id_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].reserved_hr_flow_id",
        unit, fmq_idx_0,
        dnx_data_sch.general.nof_fmq_class_get(unit) * sizeof(int) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].reserved_hr_flow_id));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_RESERVED_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_nof_reserved_hr_set(int unit, uint32 fmq_idx_0, int nof_reserved_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        nof_reserved_hr,
        int,
        0,
        "dnx_scheduler_db_fmq_nof_reserved_hr_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &nof_reserved_hr,
        "dnx_scheduler_db[%d]->fmq[%d].nof_reserved_hr",
        unit, fmq_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_nof_reserved_hr_get(int unit, uint32 fmq_idx_0, int *nof_reserved_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        nof_reserved_hr);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *nof_reserved_hr = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        "dnx_scheduler_db[%d]->fmq[%d].nof_reserved_hr",
        unit, fmq_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_nof_reserved_hr_inc(int unit, uint32 fmq_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        inc_value,
        int,
        0,
        "dnx_scheduler_db_fmq_nof_reserved_hr_inc");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_nof_reserved_hr_dec(int unit, uint32 fmq_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].nof_reserved_hr,
        dec_value,
        int,
        0,
        "dnx_scheduler_db_fmq_nof_reserved_hr_dec");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_NOF_RESERVED_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_fmq_flow_id_set(int unit, uint32 fmq_idx_0, uint32 fmq_flow_id_idx_0, int fmq_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        fmq_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[fmq_flow_id_idx_0],
        fmq_flow_id,
        int,
        0,
        "dnx_scheduler_db_fmq_fmq_flow_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &fmq_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].fmq_flow_id[%d]",
        unit, fmq_idx_0, fmq_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_fmq_flow_id_get(int unit, uint32 fmq_idx_0, uint32 fmq_flow_id_idx_0, int *fmq_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        fmq_flow_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        fmq_flow_id_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *fmq_flow_id = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[fmq_flow_id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[fmq_flow_id_idx_0],
        "dnx_scheduler_db[%d]->fmq[%d].fmq_flow_id[%d]",
        unit, fmq_idx_0, fmq_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_fmq_fmq_flow_id_alloc(int unit, uint32 fmq_idx_0)
{
    DNX_SW_STATE_DEFAULT_VALUE_DEFS;
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq,
        fmq_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        int,
        dnx_data_sch.general.nof_fmq_class_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_fmq_fmq_flow_id_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id,
        "dnx_scheduler_db[%d]->fmq[%d].fmq_flow_id",
        unit, fmq_idx_0,
        dnx_data_sch.general.nof_fmq_class_get(unit) * sizeof(int) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_FMQ_FMQ_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNX_SW_STATE_DEFAULT_VALUE_LOOP(def_val_idx[0], dnx_data_sch.general.nof_fmq_class_get(unit))

    DNX_SW_STATE_DEFAULT_VALUE_ASSIGN(
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->fmq[fmq_idx_0].fmq_flow_id[def_val_idx[0]],
        -1);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        dnx_scheduler_db_general_info,
        dnx_data_device.general.nof_cores_get(unit),
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_scheduler_db_general_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        "dnx_scheduler_db[%d]->general",
        unit,
        dnx_data_device.general.nof_cores_get(unit) * sizeof(dnx_scheduler_db_general_info) / sizeof(*((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_reserved_erp_hr_flow_id_set(int unit, uint32 general_idx_0, uint32 reserved_erp_hr_flow_id_idx_0, int reserved_erp_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        reserved_erp_hr_flow_id_idx_0,
        8);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].reserved_erp_hr_flow_id[reserved_erp_hr_flow_id_idx_0],
        reserved_erp_hr_flow_id,
        int,
        0,
        "dnx_scheduler_db_general_reserved_erp_hr_flow_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &reserved_erp_hr_flow_id,
        "dnx_scheduler_db[%d]->general[%d].reserved_erp_hr_flow_id[%d]",
        unit, general_idx_0, reserved_erp_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_RESERVED_ERP_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_reserved_erp_hr_flow_id_get(int unit, uint32 general_idx_0, uint32 reserved_erp_hr_flow_id_idx_0, int *reserved_erp_hr_flow_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        reserved_erp_hr_flow_id);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        reserved_erp_hr_flow_id_idx_0,
        8);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *reserved_erp_hr_flow_id = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].reserved_erp_hr_flow_id[reserved_erp_hr_flow_id_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].reserved_erp_hr_flow_id[reserved_erp_hr_flow_id_idx_0],
        "dnx_scheduler_db[%d]->general[%d].reserved_erp_hr_flow_id[%d]",
        unit, general_idx_0, reserved_erp_hr_flow_id_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_RESERVED_ERP_HR_FLOW_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_nof_reserved_erp_hr_set(int unit, uint32 general_idx_0, int nof_reserved_erp_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        nof_reserved_erp_hr,
        int,
        0,
        "dnx_scheduler_db_general_nof_reserved_erp_hr_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SCHEDULER_MODULE_ID,
        &nof_reserved_erp_hr,
        "dnx_scheduler_db[%d]->general[%d].nof_reserved_erp_hr",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_nof_reserved_erp_hr_get(int unit, uint32 general_idx_0, int *nof_reserved_erp_hr)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        nof_reserved_erp_hr);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    *nof_reserved_erp_hr = ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SCHEDULER_MODULE_ID,
        &((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        "dnx_scheduler_db[%d]->general[%d].nof_reserved_erp_hr",
        unit, general_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_nof_reserved_erp_hr_inc(int unit, uint32 general_idx_0, uint32 inc_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_INC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_COUNTER_INC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        inc_value,
        int,
        0,
        "dnx_scheduler_db_general_nof_reserved_erp_hr_inc");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_scheduler_db_general_nof_reserved_erp_hr_dec(int unit, uint32 general_idx_0, uint32 dec_value)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        SW_STATE_FUNC_DEC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general,
        general_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SCHEDULER_MODULE_ID);

    DNX_SW_STATE_COUNTER_DEC(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        ((dnx_scheduler_db_t*)sw_state_roots_array[unit][DNX_SCHEDULER_MODULE_ID])->general[general_idx_0].nof_reserved_erp_hr,
        dec_value,
        int,
        0,
        "dnx_scheduler_db_general_nof_reserved_erp_hr_dec");

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SCHEDULER_MODULE_ID,
        dnx_scheduler_db_info,
        DNX_SCHEDULER_DB_GENERAL_NOF_RESERVED_ERP_HR_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_scheduler_db_cbs dnx_scheduler_db = 	{
	
	dnx_scheduler_db_is_init,
	dnx_scheduler_db_init,
		{
		
		dnx_scheduler_db_port_alloc,
			{
			
			dnx_scheduler_db_port_rate_set,
			dnx_scheduler_db_port_rate_get}
		,
			{
			
			dnx_scheduler_db_port_valid_set,
			dnx_scheduler_db_port_valid_get}
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
		}
	}
;
#undef BSL_LOG_MODULE

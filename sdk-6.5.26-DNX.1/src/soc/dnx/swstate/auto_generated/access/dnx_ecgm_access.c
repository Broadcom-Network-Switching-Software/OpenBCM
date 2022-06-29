
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_ecgm_access.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_ecgm_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_ecgm_layout.h>





int
dnx_ecgm_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_ECGM_DB,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]),
        "dnx_ecgm_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_ecgm_sw_state_t,
        DNX_SW_STATE_DNX_ECGM_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ecgm_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_ecgm_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_ECGM_DB,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]),
        "dnx_ecgm_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]),
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_info_set(int unit, uint32 port_info_idx_0, uint32 port_info_idx_1, CONST dnx_ecgm_port_profile_info_t *port_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info,
        port_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0],
        port_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0][port_info_idx_1],
        port_info,
        dnx_ecgm_port_profile_info_t,
        0,
        "dnx_ecgm_db_port_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        port_info,
        "dnx_ecgm_db[%d]->port_info[%d][%d]",
        unit, port_info_idx_0, port_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_info_get(int unit, uint32 port_info_idx_0, uint32 port_info_idx_1, CONST dnx_ecgm_port_profile_info_t **port_info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info,
        port_info_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        port_info);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0],
        port_info_idx_1);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        DNX_SW_STATE_DIAG_READ);

    *port_info = &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0][port_info_idx_1];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0][port_info_idx_1],
        "dnx_ecgm_db[%d]->port_info[%d][%d]",
        unit, port_info_idx_0, port_info_idx_1);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_info_alloc(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_DOUBLE_DYNAMIC_ARRAY_SIZE_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        dnx_data_device.general.nof_cores_get(unit),
        dnx_data_port.general.nof_out_tm_ports_get(unit),
        sizeof(dnx_ecgm_port_profile_info_t*));

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info,
        dnx_ecgm_port_profile_info_t*,
        dnx_data_device.general.nof_cores_get(unit),
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ecgm_db_port_info_alloc");

    for(uint32 port_info_idx_0 = 0;
         port_info_idx_0 < dnx_data_device.general.nof_cores_get(unit);
         port_info_idx_0++)

    DNX_SW_STATE_AUTOGEN_ALLOC(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info[port_info_idx_0],
        dnx_ecgm_port_profile_info_t,
        dnx_data_port.general.nof_out_tm_ports_get(unit),
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID],
        DNXC_SWSTATE_LAYOUT_MAX_ARRAY_DIMENSIONS+1,
        DNXC_SW_STATE_DNX_DATA_ALLOC | DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_ecgm_db_port_info_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info,
        "dnx_ecgm_db[%d]->port_info",
        unit,
        dnx_data_port.general.nof_out_tm_ports_get(unit) * sizeof(dnx_ecgm_port_profile_info_t) / sizeof(*((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_info)+(dnx_data_port.general.nof_out_tm_ports_get(unit) * sizeof(dnx_ecgm_port_profile_info_t)));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_INFO_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_port_set(int unit, bcm_port_t port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port,
        port,
        bcm_port_t,
        0,
        "dnx_ecgm_db_interface_caching_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        &port,
        "dnx_ecgm_db[%d]->interface_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_port_get(int unit, bcm_port_t *port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        DNX_SW_STATE_DIAG_READ);

    *port = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.port,
        "dnx_ecgm_db[%d]->interface_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PORT,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_info_set(int unit, dnx_ecgm_interface_profile_info_t info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info,
        info,
        dnx_ecgm_interface_profile_info_t,
        0,
        "dnx_ecgm_db_interface_caching_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        &info,
        "dnx_ecgm_db[%d]->interface_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_info_get(int unit, dnx_ecgm_interface_profile_info_t *info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        DNX_SW_STATE_DIAG_READ);

    *info = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.info,
        "dnx_ecgm_db[%d]->interface_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_profile_id_set(int unit, int profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id,
        profile_id,
        int,
        0,
        "dnx_ecgm_db_interface_caching_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        &profile_id,
        "dnx_ecgm_db[%d]->interface_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_profile_id_get(int unit, int *profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        profile_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        DNX_SW_STATE_DIAG_READ);

    *profile_id = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.profile_id,
        "dnx_ecgm_db[%d]->interface_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__PROFILE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_valid_set(int unit, int valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid,
        valid,
        int,
        0,
        "dnx_ecgm_db_interface_caching_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        &valid,
        "dnx_ecgm_db[%d]->interface_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_interface_caching_valid_get(int unit, int *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->interface_caching.valid,
        "dnx_ecgm_db[%d]->interface_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__INTERFACE_CACHING__VALID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_INTERFACE_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_port_set(int unit, bcm_port_t port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port,
        port,
        bcm_port_t,
        0,
        "dnx_ecgm_db_port_caching_port_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        &port,
        "dnx_ecgm_db[%d]->port_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_port_get(int unit, bcm_port_t *port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        port);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        DNX_SW_STATE_DIAG_READ);

    *port = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.port,
        "dnx_ecgm_db[%d]->port_caching.port",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PORT,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PORT_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_info_set(int unit, CONST dnx_ecgm_port_profile_info_t *info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info,
        info,
        dnx_ecgm_port_profile_info_t,
        0,
        "dnx_ecgm_db_port_caching_info_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        info,
        "dnx_ecgm_db[%d]->port_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_info_get(int unit, CONST dnx_ecgm_port_profile_info_t **info)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        info);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        DNX_SW_STATE_DIAG_READ);

    *info = &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.info,
        "dnx_ecgm_db[%d]->port_caching.info",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__INFO,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_INFO_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_profile_id_set(int unit, int profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id,
        profile_id,
        int,
        0,
        "dnx_ecgm_db_port_caching_profile_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        &profile_id,
        "dnx_ecgm_db[%d]->port_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_profile_id_get(int unit, int *profile_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        profile_id);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        DNX_SW_STATE_DIAG_READ);

    *profile_id = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.profile_id,
        "dnx_ecgm_db[%d]->port_caching.profile_id",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__PROFILE_ID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_PROFILE_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_valid_set(int unit, int valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid,
        valid,
        int,
        0,
        "dnx_ecgm_db_port_caching_valid_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        &valid,
        "dnx_ecgm_db[%d]->port_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}



int
dnx_ecgm_db_port_caching_valid_get(int unit, int *valid)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        sw_state_roots_array[unit][DNX_ECGM_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        valid);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        DNX_SW_STATE_DIAG_READ);

    *valid = ((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        &((dnx_ecgm_sw_state_t*)sw_state_roots_array[unit][DNX_ECGM_MODULE_ID])->port_caching.valid,
        "dnx_ecgm_db[%d]->port_caching.valid",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ECGM_DB__PORT_CACHING__VALID,
        dnx_ecgm_db_info,
        DNX_ECGM_DB_PORT_CACHING_VALID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL,
        sw_state_layout_array[unit][DNX_ECGM_MODULE_ID]);

    DNXC_SW_STATE_FUNC_RETURN;
}




dnx_ecgm_db_cbs dnx_ecgm_db = 	{
	
	dnx_ecgm_db_is_init,
	dnx_ecgm_db_init,
		{
		
		dnx_ecgm_db_port_info_set,
		dnx_ecgm_db_port_info_get,
		dnx_ecgm_db_port_info_alloc}
	,
		{
		
			{
			
			dnx_ecgm_db_interface_caching_port_set,
			dnx_ecgm_db_interface_caching_port_get}
		,
			{
			
			dnx_ecgm_db_interface_caching_info_set,
			dnx_ecgm_db_interface_caching_info_get}
		,
			{
			
			dnx_ecgm_db_interface_caching_profile_id_set,
			dnx_ecgm_db_interface_caching_profile_id_get}
		,
			{
			
			dnx_ecgm_db_interface_caching_valid_set,
			dnx_ecgm_db_interface_caching_valid_get}
		}
	,
		{
		
			{
			
			dnx_ecgm_db_port_caching_port_set,
			dnx_ecgm_db_port_caching_port_get}
		,
			{
			
			dnx_ecgm_db_port_caching_info_set,
			dnx_ecgm_db_port_caching_info_get}
		,
			{
			
			dnx_ecgm_db_port_caching_profile_id_set,
			dnx_ecgm_db_port_caching_profile_id_get}
		,
			{
			
			dnx_ecgm_db_port_caching_valid_set,
			dnx_ecgm_db_port_caching_valid_get}
		}
	}
;
#undef BSL_LOG_MODULE

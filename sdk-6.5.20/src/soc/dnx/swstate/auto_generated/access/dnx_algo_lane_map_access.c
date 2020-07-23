
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
#include <soc/dnx/swstate/auto_generated/access/dnx_algo_lane_map_access.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_lane_map_diagnostic.h>



dnx_algo_lane_map_db_t* dnx_algo_lane_map_db_dummy = NULL;



int
dnx_algo_lane_map_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]),
        "dnx_algo_lane_map_db[%d]",
        unit);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_init(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_algo_lane_map_db_t,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_init");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]),
        "dnx_algo_lane_map_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_UPDATE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        sizeof(dnx_algo_lane_map_db_t),
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_map_size_set(int unit, uint32 lane_map_idx_0, int map_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].map_size,
        map_size,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &map_size,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].map_size",
        unit, lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_map_size_get(int unit, uint32 lane_map_idx_0, int *map_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        map_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *map_size = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].map_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].map_size,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].map_size",
        unit, lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_MAP_SIZE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_set(int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, CONST soc_dnxc_lane_map_db_map_t *lane2serdes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0],
        lane2serdes,
        soc_dnxc_lane_map_db_map_t,
        0,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        lane2serdes,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d]",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_get(int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, soc_dnxc_lane_map_db_map_t *lane2serdes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *lane2serdes = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0],
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d]",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_alloc(int unit, uint32 lane_map_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        soc_dnxc_lane_map_db_map_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes",
        unit, lane_map_idx_0,
        nof_instances_to_alloc_0 * sizeof(soc_dnxc_lane_map_db_map_t) / sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_set(int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].rx_id,
        rx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].rx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_RX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_get(int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int *rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        rx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *rx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].rx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].rx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_RX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_set(int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].tx_id,
        tx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].tx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_TX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_get(int unit, uint32 lane_map_idx_0, uint32 lane2serdes_idx_0, int *tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        tx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *tx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].tx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].tx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_LANE2SERDES_TX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_set(int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, CONST soc_dnxc_lane_map_db_map_t *serdes2lane)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0],
        serdes2lane,
        soc_dnxc_lane_map_db_map_t,
        0,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        serdes2lane,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d]",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_get(int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, soc_dnxc_lane_map_db_map_t *serdes2lane)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *serdes2lane = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0],
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d]",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_alloc(int unit, uint32 lane_map_idx_0, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        soc_dnxc_lane_map_db_map_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane",
        unit, lane_map_idx_0,
        nof_instances_to_alloc_0 * sizeof(soc_dnxc_lane_map_db_map_t) / sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_set(int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].rx_id,
        rx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].rx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_RX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_get(int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int *rx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        rx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *rx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].rx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].rx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_RX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_set(int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    DNX_SW_STATE_SET(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].tx_id,
        tx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].tx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_TX_ID_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_get(int unit, uint32 lane_map_idx_0, uint32 serdes2lane_idx_0, int *tx_id)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        tx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID);

    *tx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].tx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].tx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_ALGO_LANE_MAP_MODULE_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_TX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}





dnx_algo_lane_map_db_cbs dnx_algo_lane_map_db = 	{
	
	dnx_algo_lane_map_db_is_init,
	dnx_algo_lane_map_db_init,
		{
		
			{
			
			dnx_algo_lane_map_db_lane_map_map_size_set,
			dnx_algo_lane_map_db_lane_map_map_size_get}
		,
			{
			
			dnx_algo_lane_map_db_lane_map_lane2serdes_set,
			dnx_algo_lane_map_db_lane_map_lane2serdes_get,
			dnx_algo_lane_map_db_lane_map_lane2serdes_alloc,
				{
				
				dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_set,
				dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_get}
			,
				{
				
				dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_set,
				dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_get}
			}
		,
			{
			
			dnx_algo_lane_map_db_lane_map_serdes2lane_set,
			dnx_algo_lane_map_db_lane_map_serdes2lane_get,
			dnx_algo_lane_map_db_lane_map_serdes2lane_alloc,
				{
				
				dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_set,
				dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_get}
			,
				{
				
				dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_set,
				dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_get}
			}
		}
	}
;
#undef BSL_LOG_MODULE

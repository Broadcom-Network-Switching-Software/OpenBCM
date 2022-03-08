
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/access/dnx_algo_lane_map_access.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
#include <soc/dnx/swstate/auto_generated/diagnostic/dnx_algo_lane_map_diagnostic.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_algo_lane_map_layout.h>





int
dnx_algo_lane_map_db_is_init(int unit, uint8 *is_init)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB,
        SW_STATE_FUNC_IS_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    *is_init = (NULL != ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_IS_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_ISINIT,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB,
        SW_STATE_FUNC_INIT,
        DNXC_SW_STATE_NO_FLAGS);

    DNX_SW_STATE_MODULE_INIT(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB,
        DNXC_SW_STATE_IMPLEMENTATION_WB,
        dnx_algo_lane_map_db_t,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB_NOF_PARAMS,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_init",
        DNX_SW_STATE_DIAG_ALLOC,
        dnx_algo_lane_map_init_layout_structure);

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_INIT_LOGGING,
        BSL_LS_SWSTATEDNX_INIT,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]),
        "dnx_algo_lane_map_db[%d]",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]));

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_lane_map_map_size_set(int unit, uint32 lane_map_idx_0, int map_size)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].map_size,
        map_size,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_map_size_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        &map_size,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].map_size",
        unit, lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        map_size);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        DNX_SW_STATE_DIAG_READ);

    *map_size = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].map_size;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].map_size,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].map_size",
        unit, lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__MAP_SIZE,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0],
        lane2serdes,
        soc_dnxc_lane_map_db_map_t,
        0,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        lane2serdes,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d]",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        DNX_SW_STATE_DIAG_READ);

    *lane2serdes = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0],
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d]",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        soc_dnxc_lane_map_db_map_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes",
        unit, lane_map_idx_0,
        nof_instances_to_alloc_0 * sizeof(soc_dnxc_lane_map_db_map_t) / sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].rx_id,
        rx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_rx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        &rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].rx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        rx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        DNX_SW_STATE_DIAG_READ);

    *rx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].rx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].rx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__RX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].tx_id,
        tx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_lane2serdes_tx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        &tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].tx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        tx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes,
        lane2serdes_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        DNX_SW_STATE_DIAG_READ);

    *tx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].tx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].lane2serdes[lane2serdes_idx_0].tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].lane2serdes[%d].tx_id",
        unit, lane_map_idx_0, lane2serdes_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__LANE2SERDES__TX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_MEMCPY(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0],
        serdes2lane,
        soc_dnxc_lane_map_db_map_t,
        0,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        serdes2lane,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d]",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        DNX_SW_STATE_DIAG_READ);

    *serdes2lane = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0];
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0],
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d]",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        soc_dnxc_lane_map_db_map_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane",
        unit, lane_map_idx_0,
        nof_instances_to_alloc_0 * sizeof(soc_dnxc_lane_map_db_map_t) / sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].rx_id,
        rx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_rx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        &rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].rx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        rx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        DNX_SW_STATE_DIAG_READ);

    *rx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].rx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].rx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].rx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__RX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].tx_id,
        tx_id,
        int,
        0,
        "dnx_algo_lane_map_db_lane_map_serdes2lane_tx_id_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        &tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].tx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
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
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_STATIC_ARRAY_CHECK(
        lane_map_idx_0,
        DNX_ALGO_NOF_LANE_MAP_TYPES);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        tx_id);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane,
        serdes2lane_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        DNX_SW_STATE_DIAG_READ);

    *tx_id = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].tx_id;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->lane_map[lane_map_idx_0].serdes2lane[serdes2lane_idx_0].tx_id,
        "dnx_algo_lane_map_db[%d]->lane_map[%d].serdes2lane[%d].tx_id",
        unit, lane_map_idx_0, serdes2lane_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__LANE_MAP__SERDES2LANE__TX_ID,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_LANE_MAP_SERDES2LANE_TX_ID_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_set(int unit, bcm_pbmp_t logical_lanes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        logical_lanes,
        bcm_pbmp_t,
        0,
        "dnx_algo_lane_map_db_otn_logical_lanes_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_get(int unit, bcm_pbmp_t *logical_lanes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        logical_lanes);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_READ);

    *logical_lanes = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_neq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_NEQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEQ(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEQ,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_eq(int unit, _shr_pbmp_t input_pbmp, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_EQ,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_EQ(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_EQ_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPEQ,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_member(int unit, uint32 _input_port, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_MEMBER,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_MEMBER(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _input_port,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_MEMBER_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPMEMBER,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_not_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_NOT_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NOT_NULL(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NOT_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNOTNULL,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_is_null(int unit, uint8 *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_IS_NULL,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_IS_NULL(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_IS_NULL_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPISNULL,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_count(int unit, int *result)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_COUNT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_COUNT(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        result,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_COUNT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCOUNT,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_xor(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_XOR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_XOR(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_XOR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPXOR,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_remove(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_REMOVE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPREMOVE,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_assign(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_ASSIGN,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_ASSIGN(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_ASSIGN_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPASSIGN,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_get(int unit, _shr_pbmp_t *output_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_GET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        output_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_GET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPGET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_and(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_AND,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_AND(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_AND_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPAND,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_negate(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_NEGATE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_NEGATE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_NEGATE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPNEGATE,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_or(int unit, _shr_pbmp_t input_pbmp)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_OR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_OR(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        input_pbmp,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_OR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPOR,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_clear(int unit)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_CLEAR,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_CLEAR(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_CLEAR_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPCLEAR,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_add(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_PORT_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_ADD(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTADD,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_flip(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_PORT_FLIP,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_FLIP(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_FLIP_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTFLIP,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_remove(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_PORT_REMOVE,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_REMOVE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_REMOVE_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTREMOVE,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_set(int unit, uint32 _input_port)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_PORT_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORT_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _input_port,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORT_SET_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_ports_range_add(int unit, uint32 _first_port, uint32 _range)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_PORTS_RANGE_ADD,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_PORTS_RANGE_ADD(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _first_port,
        _range,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_PORTS_RANGE_ADD_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPPORTSRANGEADD,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lanes_pbmp_fmt(int unit, char* _buffer)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        SW_STATE_FUNC_PBMP_FMT,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        DNX_SW_STATE_DIAG_PBMP);

    DNX_SW_STATE_PBMP_FMT(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        _buffer,
        0,
        "");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_PBMP_FMT_LOGGING,
        BSL_LS_SWSTATEDNX_PBMPFMT,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lanes",
        unit);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANES_INFO,
        DNX_SW_STATE_DIAG_PBMP,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lane_map_alloc(int unit, uint32 nof_instances_to_alloc_0)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP,
        SW_STATE_FUNC_ALLOC,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP,
        DNX_SW_STATE_DIAG_ALLOC);

    DNX_SW_STATE_ALLOC(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map,
        dnx_algo_lane_map_db_otn_logical_lane_map_t,
        nof_instances_to_alloc_0,
        DNXC_SW_STATE_AUTO_GENERATED_ALLOCATION,
        "dnx_algo_lane_map_db_otn_logical_lane_map_alloc");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_ALLOC_LOGGING,
        BSL_LS_SWSTATEDNX_ALLOC,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map,
        "dnx_algo_lane_map_db[%d]->otn.logical_lane_map",
        unit,
        nof_instances_to_alloc_0 * sizeof(dnx_algo_lane_map_db_otn_logical_lane_map_t) / sizeof(*((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map));

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANE_MAP_INFO,
        DNX_SW_STATE_DIAG_ALLOC,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_set(int unit, uint32 logical_lane_map_idx_0, uint16 logical_otn_lane)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map,
        logical_lane_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map[logical_lane_map_idx_0].logical_otn_lane,
        logical_otn_lane,
        uint16,
        0,
        "dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        &logical_otn_lane,
        "dnx_algo_lane_map_db[%d]->otn.logical_lane_map[%d].logical_otn_lane",
        unit, logical_lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANE_MAP_LOGICAL_OTN_LANE_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_get(int unit, uint32 logical_lane_map_idx_0, uint16 *logical_otn_lane)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map,
        logical_lane_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        logical_otn_lane);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        DNX_SW_STATE_DIAG_READ);

    *logical_otn_lane = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map[logical_lane_map_idx_0].logical_otn_lane;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map[logical_lane_map_idx_0].logical_otn_lane,
        "dnx_algo_lane_map_db[%d]->otn.logical_lane_map[%d].logical_otn_lane",
        unit, logical_lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__LOGICAL_OTN_LANE,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANE_MAP_LOGICAL_OTN_LANE_INFO,
        DNX_SW_STATE_DIAG_READ,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_set(int unit, uint32 logical_lane_map_idx_0, uint16 nof_lanes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        SW_STATE_FUNC_SET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map,
        logical_lane_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        DNX_SW_STATE_DIAG_MODIFY);

    DNX_SW_STATE_SET(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map[logical_lane_map_idx_0].nof_lanes,
        nof_lanes,
        uint16,
        0,
        "dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_set");

    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_SET_LOGGING,
        BSL_LS_SWSTATEDNX_SET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        &nof_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lane_map[%d].nof_lanes",
        unit, logical_lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANE_MAP_NOF_LANES_INFO,
        DNX_SW_STATE_DIAG_MODIFY,
        NULL);

    DNXC_SW_STATE_FUNC_RETURN;
}




int
dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_get(int unit, uint32 logical_lane_map_idx_0, uint16 *nof_lanes)
{
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    VERIFY_FUNCTION_CALL_ALLOWED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        SW_STATE_FUNC_GET,
        DNXC_SW_STATE_NO_FLAGS);

    VERIFY_MODULE_IS_INITIALIZED(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID]);

    DNX_SW_STATE_OOB_DYNAMIC_ARRAY_CHECK(
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map,
        logical_lane_map_idx_0);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        nof_lanes);

    DNX_SW_STATE_PTR_NULL_CHECK(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map);

    DNX_SW_STATE_DIAG_INFO_PRE(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        DNX_SW_STATE_DIAG_READ);

    *nof_lanes = ((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map[logical_lane_map_idx_0].nof_lanes;
    DNX_SW_STATE_LOG(
        unit,
        DNX_SW_STATE_GET_LOGGING,
        BSL_LS_SWSTATEDNX_GET,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        &((dnx_algo_lane_map_db_t*)sw_state_roots_array[unit][DNX_ALGO_LANE_MAP_MODULE_ID])->otn.logical_lane_map[logical_lane_map_idx_0].nof_lanes,
        "dnx_algo_lane_map_db[%d]->otn.logical_lane_map[%d].nof_lanes",
        unit, logical_lane_map_idx_0);

    DNX_SW_STATE_DIAG_INFO_POST(
        unit,
        DNX_SW_STATE_DNX_ALGO_LANE_MAP_DB__OTN__LOGICAL_LANE_MAP__NOF_LANES,
        dnx_algo_lane_map_db_info,
        DNX_ALGO_LANE_MAP_DB_OTN_LOGICAL_LANE_MAP_NOF_LANES_INFO,
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
	,
		{
		
			{
			
			dnx_algo_lane_map_db_otn_logical_lanes_set,
			dnx_algo_lane_map_db_otn_logical_lanes_get,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_neq,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_eq,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_member,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_not_null,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_is_null,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_count,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_xor,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_remove,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_assign,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_get,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_and,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_negate,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_or,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_clear,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_add,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_flip,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_remove,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_port_set,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_ports_range_add,
			dnx_algo_lane_map_db_otn_logical_lanes_pbmp_fmt}
		,
			{
			
			dnx_algo_lane_map_db_otn_logical_lane_map_alloc,
				{
				
				dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_set,
				dnx_algo_lane_map_db_otn_logical_lane_map_logical_otn_lane_get}
			,
				{
				
				dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_set,
				dnx_algo_lane_map_db_otn_logical_lane_map_nof_lanes_get}
			}
		}
	}
;
#undef BSL_LOG_MODULE

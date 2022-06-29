
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/instru_counters_types.h>
#include <soc/dnx/swstate/auto_generated/layout/instru_counters_layout.h>

dnxc_sw_state_layout_t layout_array_instru_counters[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS_NOF_PARAMS)];

shr_error_e
instru_counters_init_layout_structure(int unit)
{

    uint8 instru_sync_counters__icgm_countes__sync_counters_started__default_val = FALSE;
    uint8 instru_sync_counters__nif_countes__sync_counters_started__default_val = FALSE;


    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS);
    layout_array_instru_counters[idx].name = "instru_sync_counters";
    layout_array_instru_counters[idx].type = "instru_sync_counters_sw_state_t";
    layout_array_instru_counters[idx].doc = "SW state for current state of synchronized counters.";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(instru_sync_counters_sw_state_t);
    layout_array_instru_counters[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__FIRST);
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES);
    layout_array_instru_counters[idx].name = "instru_sync_counters__icgm_countes";
    layout_array_instru_counters[idx].type = "icgm_sync_counters_info_t";
    layout_array_instru_counters[idx].doc = "SW state ICGM synchronized counters";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(icgm_sync_counters_info_t);
    layout_array_instru_counters[idx].parent  = 0;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__FIRST);
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES);
    layout_array_instru_counters[idx].name = "instru_sync_counters__nif_countes";
    layout_array_instru_counters[idx].type = "nif_sync_counters_info_t";
    layout_array_instru_counters[idx].doc = "SW state NIF synchronized counters";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(nif_sync_counters_info_t);
    layout_array_instru_counters[idx].parent  = 0;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__FIRST);
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__SYNC_COUNTERS_STARTED);
    layout_array_instru_counters[idx].name = "instru_sync_counters__icgm_countes__sync_counters_started";
    layout_array_instru_counters[idx].type = "uint8";
    layout_array_instru_counters[idx].doc = "If TRUE The structure was written to.";
    layout_array_instru_counters[idx].default_value= &(instru_sync_counters__icgm_countes__sync_counters_started__default_val);
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(uint8);
    layout_array_instru_counters[idx].parent  = 1;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__TOD_1588_TIMESTAMP_START);
    layout_array_instru_counters[idx].name = "instru_sync_counters__icgm_countes__tod_1588_timestamp_start";
    layout_array_instru_counters[idx].type = "uint64";
    layout_array_instru_counters[idx].doc = "The ECI timestamp in 1588 format, when the recording started.";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(uint64);
    layout_array_instru_counters[idx].parent  = 1;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO);
    layout_array_instru_counters[idx].name = "instru_sync_counters__icgm_countes__per_core_info";
    layout_array_instru_counters[idx].type = "sync_counters_icgm_per_core_t*";
    layout_array_instru_counters[idx].doc = "The number of record written up to the last record read, per core";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(sync_counters_icgm_per_core_t*);
    layout_array_instru_counters[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_instru_counters[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_instru_counters[idx].parent  = 1;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO__FIRST);
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__ICGM_COUNTES__PER_CORE_INFO__RECORD_LAST_READ_NOF);
    layout_array_instru_counters[idx].name = "instru_sync_counters__icgm_countes__per_core_info__record_last_read_nof";
    layout_array_instru_counters[idx].type = "uint32";
    layout_array_instru_counters[idx].doc = "The number of record written up to the last record read.";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(uint32);
    layout_array_instru_counters[idx].parent  = 5;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__SYNC_COUNTERS_STARTED);
    layout_array_instru_counters[idx].name = "instru_sync_counters__nif_countes__sync_counters_started";
    layout_array_instru_counters[idx].type = "uint8";
    layout_array_instru_counters[idx].doc = "If TRUE The structure was written to.";
    layout_array_instru_counters[idx].default_value= &(instru_sync_counters__nif_countes__sync_counters_started__default_val);
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(uint8);
    layout_array_instru_counters[idx].parent  = 2;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__TOD_1588_TIMESTAMP_START);
    layout_array_instru_counters[idx].name = "instru_sync_counters__nif_countes__tod_1588_timestamp_start";
    layout_array_instru_counters[idx].type = "uint64";
    layout_array_instru_counters[idx].doc = "The ECI timestamp in 1588 format, when the recording started.";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(uint64);
    layout_array_instru_counters[idx].parent  = 2;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO);
    layout_array_instru_counters[idx].name = "instru_sync_counters__nif_countes__per_ethu_info";
    layout_array_instru_counters[idx].type = "sync_counters_nif_per_eth_info_t**";
    layout_array_instru_counters[idx].doc = "The number of record written up to the last record read per info";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(sync_counters_nif_per_eth_info_t**);
    layout_array_instru_counters[idx].array_indexes[0].num_elements = dnx_data_device.general.nof_cores_get(unit);
    layout_array_instru_counters[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_instru_counters[idx].array_indexes[1].num_elements = dnx_data_nif.eth.max_ethu_nof_per_core_get(unit);
    layout_array_instru_counters[idx].array_indexes[1].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_instru_counters[idx].parent  = 2;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__FIRST);
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_INSTRU_SYNC_COUNTERS__NIF_COUNTES__PER_ETHU_INFO__RECORD_LAST_READ_NOF);
    layout_array_instru_counters[idx].name = "instru_sync_counters__nif_countes__per_ethu_info__record_last_read_nof";
    layout_array_instru_counters[idx].type = "uint32*";
    layout_array_instru_counters[idx].doc = "The number of record written up to the last record read.";
    layout_array_instru_counters[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].size_of = sizeof(uint32*);
    layout_array_instru_counters[idx].array_indexes[0].num_elements = dnx_data_nif.eth.max_nof_lanes_in_ethu_get(unit);
    layout_array_instru_counters[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_DYNAMIC_DNX_DATA;
    layout_array_instru_counters[idx].parent  = 9;
    layout_array_instru_counters[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_instru_counters[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_INSTRU_SYNC_COUNTERS, layout_array_instru_counters, sw_state_layout_array[unit][INSTRU_COUNTERS_MODULE_ID], DNX_SW_STATE_INSTRU_SYNC_COUNTERS_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE

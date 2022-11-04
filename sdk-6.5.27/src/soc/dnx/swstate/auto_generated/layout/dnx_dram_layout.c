
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SWSTATEDNX_GENERAL

#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <soc/dnx/swstate/auto_generated/layout/dnx_dram_layout.h>

dnxc_sw_state_layout_t layout_array_dnx_dram[DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB_NOF_PARAMS)];

shr_error_e
dnx_dram_init_layout_structure(int unit)
{

    int idx;

    DNXC_SW_STATE_INIT_FUNC_DEFS;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB);
    layout_array_dnx_dram[idx].name = "dnx_dram_db";
    layout_array_dnx_dram[idx].type = "dnx_dram_db_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(dnx_dram_db_t);
    layout_array_dnx_dram[idx].parent  = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__FIRST);
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__IS_INITIALIZED);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__is_initialized";
    layout_array_dnx_dram[idx].type = "int";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(int);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__SYNC_MANAGER);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__sync_manager";
    layout_array_dnx_dram[idx].type = "dnx_dram_sync_manager_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(dnx_dram_sync_manager_t);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__SYNC_MANAGER__FIRST);
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__SYNC_MANAGER__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__POWER_DOWN_CALLBACK);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__power_down_callback";
    layout_array_dnx_dram[idx].type = "dnx_power_down_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(dnx_power_down_t);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__POWER_DOWN_CALLBACK__FIRST);
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__POWER_DOWN_CALLBACK__LAST)-1;
    layout_array_dnx_dram[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__OVERRIDE_BIST_CONFIGURATIONS_DURING_TUNING);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__override_bist_configurations_during_tuning";
    layout_array_dnx_dram[idx].type = "int";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(int);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__CHANNEL_SOFT_INIT_AFTER_BIST);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__channel_soft_init_after_bist";
    layout_array_dnx_dram[idx].type = "int";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(int);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__TRAFFIC_TUNING_CORE_MODE);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__traffic_tuning_core_mode";
    layout_array_dnx_dram[idx].type = "int";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(int);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__DYNAMIC_CALIBRATION_ENABLED);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__dynamic_calibration_enabled";
    layout_array_dnx_dram[idx].type = "int";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(int);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__DYNAMIC_CALIBRATION_WAS_ENABLED_AFTER_DRAM_TUNE);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__dynamic_calibration_was_enabled_after_dram_tune";
    layout_array_dnx_dram[idx].type = "int";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(int);
    layout_array_dnx_dram[idx].array_indexes[0].num_elements = 2;
    layout_array_dnx_dram[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__TEMPERATURE_MONITORING_THREAD_STATE);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__temperature_monitoring_thread_state";
    layout_array_dnx_dram[idx].type = "dnx_dram_temperature_monitoring_state_e";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(dnx_dram_temperature_monitoring_state_e);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__TEMPERATURE_MONITORING_THREAD_HANDLER);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__temperature_monitoring_thread_handler";
    layout_array_dnx_dram[idx].type = "shr_thread_manager_handler_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(shr_thread_manager_handler_t);
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB | DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__TEMPERATURE_MONITORING_LAST_READ);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__temperature_monitoring_last_read";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].array_indexes[0].num_elements = 2;
    layout_array_dnx_dram[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].labels = DNXC_SW_STATE_LAYOUT_LABEL_ALLOW_WRITES_DURING_WB | DNXC_SW_STATE_LAYOUT_LABEL_DONT_DUMP ;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info";
    layout_array_dnx_dram[idx].type = "hbm_dram_vendor_info_hbm_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(hbm_dram_vendor_info_hbm_t);
    layout_array_dnx_dram[idx].array_indexes[0].num_elements = 2;
    layout_array_dnx_dram[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__FIRST);
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info";
    layout_array_dnx_dram[idx].type = "gddr6_dram_vendor_info_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(gddr6_dram_vendor_info_t);
    layout_array_dnx_dram[idx].array_indexes[0].num_elements = 2;
    layout_array_dnx_dram[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_dram[idx].parent  = 0;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__FIRST);
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__LAST)-1;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__SYNC_MANAGER__DELETED_BUFFERS_FILE_MUTEX);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__sync_manager__deleted_buffers_file_mutex";
    layout_array_dnx_dram[idx].type = "sw_state_mutex_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_dnx_dram[idx].parent  = 2;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__SYNC_MANAGER__FIRMWARE_MUTEX);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__sync_manager__firmware_mutex";
    layout_array_dnx_dram[idx].type = "sw_state_mutex_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(sw_state_mutex_t);
    layout_array_dnx_dram[idx].parent  = 2;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__POWER_DOWN_CALLBACK__CALLBACK);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__power_down_callback__callback";
    layout_array_dnx_dram[idx].type = "bcm_switch_dram_power_down_callback_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(bcm_switch_dram_power_down_callback_t);
    layout_array_dnx_dram[idx].parent  = 3;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__POWER_DOWN_CALLBACK__POWER_ON);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__power_down_callback__power_on";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 3;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__POWER_DOWN_CALLBACK__USERDATA);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__power_down_callback__userdata";
    layout_array_dnx_dram[idx].type = "dnx_power_down_callback_userdata_t";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(dnx_power_down_callback_userdata_t);
    layout_array_dnx_dram[idx].parent  = 3;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__IS_INIT);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__is_init";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__GEN2_TEST);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__gen2_test";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__ECC);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__ecc";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__DENSITY);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__density";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__MANUFACTURER_ID);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__manufacturer_id";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__MANUFACTURING_LOCATION);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__manufacturing_location";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__MANUFACTURING_YEAR);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__manufacturing_year";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__MANUFACTURING_WEEK);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__manufacturing_week";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__SERIAL_NUMBER);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__serial_number";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].array_indexes[0].num_elements = 2;
    layout_array_dnx_dram[idx].array_indexes[0].index_type = DNXC_SWSTATE_ARRAY_INDEX_STATIC;
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__ADDRESSING_MODE);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__addressing_mode";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__CHANNEL_AVAILABLE);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__channel_available";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__HBM_STACK_HIGHT);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__hbm_stack_hight";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__VENDOR_INFO__MODEL_PART_NUMBER);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__vendor_info__model_part_number";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 12;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__IS_INIT);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__is_init";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__MANUFACTURER_VENDOR_CODE);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__manufacturer_vendor_code";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__REVISION_ID);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__revision_id";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__DENSITY);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__density";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__INTERNAL_WCK);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__internal_wck";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__WCK_GRANULARITY);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__wck_granularity";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__WCK_FREQUENCY);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__wck_frequency";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;

    idx = DNXC_SW_STATE_LAYOUT_GET_PARAM_IDX(DNX_SW_STATE_DNX_DRAM_DB__GDDR6_VENDOR_INFO__VDDQ_OFF);
    layout_array_dnx_dram[idx].name = "dnx_dram_db__gddr6_vendor_info__vddq_off";
    layout_array_dnx_dram[idx].type = "uint32";
    layout_array_dnx_dram[idx].sum_of_allocation = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].size_of = sizeof(uint32);
    layout_array_dnx_dram[idx].parent  = 13;
    layout_array_dnx_dram[idx].first_child_index = DNXC_SW_STATE_LAYOUT_INVALID;
    layout_array_dnx_dram[idx].last_child_index = DNXC_SW_STATE_LAYOUT_INVALID;


    dnxc_sw_state_layout_init_structure(unit, DNX_SW_STATE_DNX_DRAM_DB, layout_array_dnx_dram, sw_state_layout_array[unit][DNX_DRAM_MODULE_ID], DNX_SW_STATE_DNX_DRAM_DB_NOF_PARAMS);

    DNXC_SW_STATE_FUNC_RETURN;
}

#undef BSL_LOG_MODULE

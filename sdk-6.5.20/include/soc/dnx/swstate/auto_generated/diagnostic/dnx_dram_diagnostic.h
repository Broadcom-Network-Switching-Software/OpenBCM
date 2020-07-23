
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_DRAM_DIAGNOSTIC_H__
#define __DNX_DRAM_DIAGNOSTIC_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_size.h>
#include <soc/dnxc/swstate/dnx_sw_state_diagnostic_operation_counters.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <include/bcm/switch.h>
#include <include/shared/periodic_event.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>
#if defined(DNX_SW_STATE_DIAGNOSTIC)
typedef enum {
    DNX_DRAM_DB_INFO,
    DNX_DRAM_DB_IS_INITIALIZED_INFO,
    DNX_DRAM_DB_SYNC_MANAGER_INFO,
    DNX_DRAM_DB_SYNC_MANAGER_DELETED_BUFFERS_FILE_MUTEX_INFO,
    DNX_DRAM_DB_POWER_DOWN_CALLBACK_INFO,
    DNX_DRAM_DB_POWER_DOWN_CALLBACK_CALLBACK_INFO,
    DNX_DRAM_DB_POWER_DOWN_CALLBACK_USERDATA_INFO,
    DNX_DRAM_DB_POWER_DOWN_CALLBACK_POWER_ON_INFO,
    DNX_DRAM_DB_OVERRIDE_BIST_CONFIGURATIONS_DURING_TUNING_INFO,
    DNX_DRAM_DB_CHANNEL_SOFT_INIT_AFTER_BIST_INFO,
    DNX_DRAM_DB_TRAFFIC_TUNING_CORE_MODE_INFO,
    DNX_DRAM_DB_DYNAMIC_CALIBRATION_ENABLED_INFO,
    DNX_DRAM_DB_DYNAMIC_CALIBRATION_WAS_ENABLED_AFTER_DRAM_TUNE_INFO,
    DNX_DRAM_DB_TEMPERATURE_MONITORING_THREAD_STATE_INFO,
    DNX_DRAM_DB_TEMPERATURE_MONITORING_THREAD_HANDLER_INFO,
    DNX_DRAM_DB_VENDOR_INFO_INFO,
    DNX_DRAM_DB_VENDOR_INFO_IS_INIT_INFO,
    DNX_DRAM_DB_VENDOR_INFO_GEN2_TEST_INFO,
    DNX_DRAM_DB_VENDOR_INFO_ECC_INFO,
    DNX_DRAM_DB_VENDOR_INFO_DENSITY_INFO,
    DNX_DRAM_DB_VENDOR_INFO_MANUFACTURER_ID_INFO,
    DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_LOCATION_INFO,
    DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_YEAR_INFO,
    DNX_DRAM_DB_VENDOR_INFO_MANUFACTURING_WEEK_INFO,
    DNX_DRAM_DB_VENDOR_INFO_SERIAL_NUMBER_INFO,
    DNX_DRAM_DB_VENDOR_INFO_ADDRESSING_MODE_INFO,
    DNX_DRAM_DB_VENDOR_INFO_CHANNEL_AVAILABLE_INFO,
    DNX_DRAM_DB_VENDOR_INFO_HBM_STACK_HIGHT_INFO,
    DNX_DRAM_DB_VENDOR_INFO_MODEL_PART_NUMBER_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_IS_INIT_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_MANUFACTURER_VENDOR_CODE_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_REVISION_ID_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_DENSITY_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_INTERNAL_WCK_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_WCK_GRANULARITY_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_WCK_FREQUENCY_INFO,
    DNX_DRAM_DB_GDDR6_VENDOR_INFO_VDDQ_OFF_INFO,
    DNX_DRAM_DB_INFO_NOF_ENTRIES
} sw_state_dnx_dram_db_layout_e;


extern dnx_sw_state_diagnostic_info_t dnx_dram_db_info[SOC_MAX_NUM_DEVICES][DNX_DRAM_DB_INFO_NOF_ENTRIES];

extern const char* dnx_dram_db_layout_str[DNX_DRAM_DB_INFO_NOF_ENTRIES];
int dnx_dram_db_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_is_initialized_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_sync_manager_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_sync_manager_deleted_buffers_file_mutex_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_power_down_callback_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_power_down_callback_callback_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_power_down_callback_userdata_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_power_down_callback_power_on_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_override_bist_configurations_during_tuning_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_channel_soft_init_after_bist_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_traffic_tuning_core_mode_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_dynamic_calibration_enabled_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_dump(
    int unit, int dynamic_calibration_was_enabled_after_dram_tune_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_temperature_monitoring_thread_state_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_temperature_monitoring_thread_handler_dump(
    int unit, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_is_init_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_gen2_test_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_ecc_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_density_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_manufacturer_id_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_manufacturing_location_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_manufacturing_year_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_manufacturing_week_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_serial_number_dump(
    int unit, int vendor_info_idx_0, int serial_number_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_addressing_mode_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_channel_available_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_hbm_stack_hight_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_vendor_info_model_part_number_dump(
    int unit, int vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_is_init_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_revision_id_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_density_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_internal_wck_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_wck_granularity_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_wck_frequency_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

int dnx_dram_db_gddr6_vendor_info_vddq_off_dump(
    int unit, int gddr6_vendor_info_idx_0, dnx_sw_state_dump_filters_t filters);

#endif 

#endif 

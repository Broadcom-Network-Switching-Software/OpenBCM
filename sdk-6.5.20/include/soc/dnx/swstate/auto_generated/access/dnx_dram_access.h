
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_DRAM_ACCESS_H__
#define __DNX_DRAM_ACCESS_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_dram_types.h>
#include <include/bcm/switch.h>
#include <include/shared/periodic_event.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>



typedef int (*dnx_dram_db_is_init_cb)(
    int unit, uint8 *is_init);


typedef int (*dnx_dram_db_init_cb)(
    int unit);


typedef int (*dnx_dram_db_is_initialized_set_cb)(
    int unit, int is_initialized);


typedef int (*dnx_dram_db_is_initialized_get_cb)(
    int unit, int *is_initialized);


typedef int (*dnx_dram_db_sync_manager_deleted_buffers_file_mutex_create_cb)(
    int unit);


typedef int (*dnx_dram_db_sync_manager_deleted_buffers_file_mutex_is_created_cb)(
    int unit, uint8 *is_created);


typedef int (*dnx_dram_db_sync_manager_deleted_buffers_file_mutex_take_cb)(
    int unit, int usec);


typedef int (*dnx_dram_db_sync_manager_deleted_buffers_file_mutex_give_cb)(
    int unit);


typedef int (*dnx_dram_db_power_down_callback_callback_set_cb)(
    int unit, bcm_switch_dram_power_down_callback_t callback);


typedef int (*dnx_dram_db_power_down_callback_callback_get_cb)(
    int unit, bcm_switch_dram_power_down_callback_t *callback);


typedef int (*dnx_dram_db_power_down_callback_userdata_set_cb)(
    int unit, dnx_power_down_callback_userdata_t userdata);


typedef int (*dnx_dram_db_power_down_callback_userdata_get_cb)(
    int unit, dnx_power_down_callback_userdata_t *userdata);


typedef int (*dnx_dram_db_power_down_callback_power_on_set_cb)(
    int unit, uint32 power_on);


typedef int (*dnx_dram_db_power_down_callback_power_on_get_cb)(
    int unit, uint32 *power_on);


typedef int (*dnx_dram_db_override_bist_configurations_during_tuning_set_cb)(
    int unit, int override_bist_configurations_during_tuning);


typedef int (*dnx_dram_db_override_bist_configurations_during_tuning_get_cb)(
    int unit, int *override_bist_configurations_during_tuning);


typedef int (*dnx_dram_db_channel_soft_init_after_bist_set_cb)(
    int unit, int channel_soft_init_after_bist);


typedef int (*dnx_dram_db_channel_soft_init_after_bist_get_cb)(
    int unit, int *channel_soft_init_after_bist);


typedef int (*dnx_dram_db_traffic_tuning_core_mode_set_cb)(
    int unit, int traffic_tuning_core_mode);


typedef int (*dnx_dram_db_traffic_tuning_core_mode_get_cb)(
    int unit, int *traffic_tuning_core_mode);


typedef int (*dnx_dram_db_dynamic_calibration_enabled_set_cb)(
    int unit, int dynamic_calibration_enabled);


typedef int (*dnx_dram_db_dynamic_calibration_enabled_get_cb)(
    int unit, int *dynamic_calibration_enabled);


typedef int (*dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_set_cb)(
    int unit, uint32 dynamic_calibration_was_enabled_after_dram_tune_idx_0, int dynamic_calibration_was_enabled_after_dram_tune);


typedef int (*dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_get_cb)(
    int unit, uint32 dynamic_calibration_was_enabled_after_dram_tune_idx_0, int *dynamic_calibration_was_enabled_after_dram_tune);


typedef int (*dnx_dram_db_temperature_monitoring_thread_state_set_cb)(
    int unit, dnx_dram_temperature_monitoring_state_e temperature_monitoring_thread_state);


typedef int (*dnx_dram_db_temperature_monitoring_thread_state_get_cb)(
    int unit, dnx_dram_temperature_monitoring_state_e *temperature_monitoring_thread_state);


typedef int (*dnx_dram_db_temperature_monitoring_thread_handler_set_cb)(
    int unit, periodic_event_handler_t temperature_monitoring_thread_handler);


typedef int (*dnx_dram_db_temperature_monitoring_thread_handler_get_cb)(
    int unit, periodic_event_handler_t *temperature_monitoring_thread_handler);


typedef int (*dnx_dram_db_vendor_info_is_init_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 is_init);


typedef int (*dnx_dram_db_vendor_info_is_init_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *is_init);


typedef int (*dnx_dram_db_vendor_info_gen2_test_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 gen2_test);


typedef int (*dnx_dram_db_vendor_info_gen2_test_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *gen2_test);


typedef int (*dnx_dram_db_vendor_info_ecc_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 ecc);


typedef int (*dnx_dram_db_vendor_info_ecc_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *ecc);


typedef int (*dnx_dram_db_vendor_info_density_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 density);


typedef int (*dnx_dram_db_vendor_info_density_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *density);


typedef int (*dnx_dram_db_vendor_info_manufacturer_id_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 manufacturer_id);


typedef int (*dnx_dram_db_vendor_info_manufacturer_id_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *manufacturer_id);


typedef int (*dnx_dram_db_vendor_info_manufacturing_location_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 manufacturing_location);


typedef int (*dnx_dram_db_vendor_info_manufacturing_location_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *manufacturing_location);


typedef int (*dnx_dram_db_vendor_info_manufacturing_year_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 manufacturing_year);


typedef int (*dnx_dram_db_vendor_info_manufacturing_year_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *manufacturing_year);


typedef int (*dnx_dram_db_vendor_info_manufacturing_week_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 manufacturing_week);


typedef int (*dnx_dram_db_vendor_info_manufacturing_week_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *manufacturing_week);


typedef int (*dnx_dram_db_vendor_info_serial_number_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 serial_number_idx_0, uint32 serial_number);


typedef int (*dnx_dram_db_vendor_info_serial_number_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 serial_number_idx_0, uint32 *serial_number);


typedef int (*dnx_dram_db_vendor_info_addressing_mode_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 addressing_mode);


typedef int (*dnx_dram_db_vendor_info_addressing_mode_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *addressing_mode);


typedef int (*dnx_dram_db_vendor_info_channel_available_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 channel_available);


typedef int (*dnx_dram_db_vendor_info_channel_available_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *channel_available);


typedef int (*dnx_dram_db_vendor_info_hbm_stack_hight_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 hbm_stack_hight);


typedef int (*dnx_dram_db_vendor_info_hbm_stack_hight_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *hbm_stack_hight);


typedef int (*dnx_dram_db_vendor_info_model_part_number_set_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 model_part_number);


typedef int (*dnx_dram_db_vendor_info_model_part_number_get_cb)(
    int unit, uint32 vendor_info_idx_0, uint32 *model_part_number);


typedef int (*dnx_dram_db_gddr6_vendor_info_is_init_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 is_init);


typedef int (*dnx_dram_db_gddr6_vendor_info_is_init_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *is_init);


typedef int (*dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 manufacturer_vendor_code);


typedef int (*dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *manufacturer_vendor_code);


typedef int (*dnx_dram_db_gddr6_vendor_info_revision_id_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 revision_id);


typedef int (*dnx_dram_db_gddr6_vendor_info_revision_id_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *revision_id);


typedef int (*dnx_dram_db_gddr6_vendor_info_density_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 density);


typedef int (*dnx_dram_db_gddr6_vendor_info_density_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *density);


typedef int (*dnx_dram_db_gddr6_vendor_info_internal_wck_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 internal_wck);


typedef int (*dnx_dram_db_gddr6_vendor_info_internal_wck_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *internal_wck);


typedef int (*dnx_dram_db_gddr6_vendor_info_wck_granularity_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 wck_granularity);


typedef int (*dnx_dram_db_gddr6_vendor_info_wck_granularity_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *wck_granularity);


typedef int (*dnx_dram_db_gddr6_vendor_info_wck_frequency_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 wck_frequency);


typedef int (*dnx_dram_db_gddr6_vendor_info_wck_frequency_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *wck_frequency);


typedef int (*dnx_dram_db_gddr6_vendor_info_vddq_off_set_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 vddq_off);


typedef int (*dnx_dram_db_gddr6_vendor_info_vddq_off_get_cb)(
    int unit, uint32 gddr6_vendor_info_idx_0, uint32 *vddq_off);




typedef struct {
    dnx_dram_db_is_initialized_set_cb set;
    dnx_dram_db_is_initialized_get_cb get;
} dnx_dram_db_is_initialized_cbs;


typedef struct {
    dnx_dram_db_sync_manager_deleted_buffers_file_mutex_create_cb create;
    dnx_dram_db_sync_manager_deleted_buffers_file_mutex_is_created_cb is_created;
    dnx_dram_db_sync_manager_deleted_buffers_file_mutex_take_cb take;
    dnx_dram_db_sync_manager_deleted_buffers_file_mutex_give_cb give;
} dnx_dram_db_sync_manager_deleted_buffers_file_mutex_cbs;


typedef struct {
    
    dnx_dram_db_sync_manager_deleted_buffers_file_mutex_cbs deleted_buffers_file_mutex;
} dnx_dram_db_sync_manager_cbs;


typedef struct {
    dnx_dram_db_power_down_callback_callback_set_cb set;
    dnx_dram_db_power_down_callback_callback_get_cb get;
} dnx_dram_db_power_down_callback_callback_cbs;


typedef struct {
    dnx_dram_db_power_down_callback_userdata_set_cb set;
    dnx_dram_db_power_down_callback_userdata_get_cb get;
} dnx_dram_db_power_down_callback_userdata_cbs;


typedef struct {
    dnx_dram_db_power_down_callback_power_on_set_cb set;
    dnx_dram_db_power_down_callback_power_on_get_cb get;
} dnx_dram_db_power_down_callback_power_on_cbs;


typedef struct {
    
    dnx_dram_db_power_down_callback_callback_cbs callback;
    
    dnx_dram_db_power_down_callback_userdata_cbs userdata;
    
    dnx_dram_db_power_down_callback_power_on_cbs power_on;
} dnx_dram_db_power_down_callback_cbs;


typedef struct {
    dnx_dram_db_override_bist_configurations_during_tuning_set_cb set;
    dnx_dram_db_override_bist_configurations_during_tuning_get_cb get;
} dnx_dram_db_override_bist_configurations_during_tuning_cbs;


typedef struct {
    dnx_dram_db_channel_soft_init_after_bist_set_cb set;
    dnx_dram_db_channel_soft_init_after_bist_get_cb get;
} dnx_dram_db_channel_soft_init_after_bist_cbs;


typedef struct {
    dnx_dram_db_traffic_tuning_core_mode_set_cb set;
    dnx_dram_db_traffic_tuning_core_mode_get_cb get;
} dnx_dram_db_traffic_tuning_core_mode_cbs;


typedef struct {
    dnx_dram_db_dynamic_calibration_enabled_set_cb set;
    dnx_dram_db_dynamic_calibration_enabled_get_cb get;
} dnx_dram_db_dynamic_calibration_enabled_cbs;


typedef struct {
    dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_set_cb set;
    dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_get_cb get;
} dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_cbs;


typedef struct {
    dnx_dram_db_temperature_monitoring_thread_state_set_cb set;
    dnx_dram_db_temperature_monitoring_thread_state_get_cb get;
} dnx_dram_db_temperature_monitoring_thread_state_cbs;


typedef struct {
    dnx_dram_db_temperature_monitoring_thread_handler_set_cb set;
    dnx_dram_db_temperature_monitoring_thread_handler_get_cb get;
} dnx_dram_db_temperature_monitoring_thread_handler_cbs;


typedef struct {
    dnx_dram_db_vendor_info_is_init_set_cb set;
    dnx_dram_db_vendor_info_is_init_get_cb get;
} dnx_dram_db_vendor_info_is_init_cbs;


typedef struct {
    dnx_dram_db_vendor_info_gen2_test_set_cb set;
    dnx_dram_db_vendor_info_gen2_test_get_cb get;
} dnx_dram_db_vendor_info_gen2_test_cbs;


typedef struct {
    dnx_dram_db_vendor_info_ecc_set_cb set;
    dnx_dram_db_vendor_info_ecc_get_cb get;
} dnx_dram_db_vendor_info_ecc_cbs;


typedef struct {
    dnx_dram_db_vendor_info_density_set_cb set;
    dnx_dram_db_vendor_info_density_get_cb get;
} dnx_dram_db_vendor_info_density_cbs;


typedef struct {
    dnx_dram_db_vendor_info_manufacturer_id_set_cb set;
    dnx_dram_db_vendor_info_manufacturer_id_get_cb get;
} dnx_dram_db_vendor_info_manufacturer_id_cbs;


typedef struct {
    dnx_dram_db_vendor_info_manufacturing_location_set_cb set;
    dnx_dram_db_vendor_info_manufacturing_location_get_cb get;
} dnx_dram_db_vendor_info_manufacturing_location_cbs;


typedef struct {
    dnx_dram_db_vendor_info_manufacturing_year_set_cb set;
    dnx_dram_db_vendor_info_manufacturing_year_get_cb get;
} dnx_dram_db_vendor_info_manufacturing_year_cbs;


typedef struct {
    dnx_dram_db_vendor_info_manufacturing_week_set_cb set;
    dnx_dram_db_vendor_info_manufacturing_week_get_cb get;
} dnx_dram_db_vendor_info_manufacturing_week_cbs;


typedef struct {
    dnx_dram_db_vendor_info_serial_number_set_cb set;
    dnx_dram_db_vendor_info_serial_number_get_cb get;
} dnx_dram_db_vendor_info_serial_number_cbs;


typedef struct {
    dnx_dram_db_vendor_info_addressing_mode_set_cb set;
    dnx_dram_db_vendor_info_addressing_mode_get_cb get;
} dnx_dram_db_vendor_info_addressing_mode_cbs;


typedef struct {
    dnx_dram_db_vendor_info_channel_available_set_cb set;
    dnx_dram_db_vendor_info_channel_available_get_cb get;
} dnx_dram_db_vendor_info_channel_available_cbs;


typedef struct {
    dnx_dram_db_vendor_info_hbm_stack_hight_set_cb set;
    dnx_dram_db_vendor_info_hbm_stack_hight_get_cb get;
} dnx_dram_db_vendor_info_hbm_stack_hight_cbs;


typedef struct {
    dnx_dram_db_vendor_info_model_part_number_set_cb set;
    dnx_dram_db_vendor_info_model_part_number_get_cb get;
} dnx_dram_db_vendor_info_model_part_number_cbs;


typedef struct {
    
    dnx_dram_db_vendor_info_is_init_cbs is_init;
    
    dnx_dram_db_vendor_info_gen2_test_cbs gen2_test;
    
    dnx_dram_db_vendor_info_ecc_cbs ecc;
    
    dnx_dram_db_vendor_info_density_cbs density;
    
    dnx_dram_db_vendor_info_manufacturer_id_cbs manufacturer_id;
    
    dnx_dram_db_vendor_info_manufacturing_location_cbs manufacturing_location;
    
    dnx_dram_db_vendor_info_manufacturing_year_cbs manufacturing_year;
    
    dnx_dram_db_vendor_info_manufacturing_week_cbs manufacturing_week;
    
    dnx_dram_db_vendor_info_serial_number_cbs serial_number;
    
    dnx_dram_db_vendor_info_addressing_mode_cbs addressing_mode;
    
    dnx_dram_db_vendor_info_channel_available_cbs channel_available;
    
    dnx_dram_db_vendor_info_hbm_stack_hight_cbs hbm_stack_hight;
    
    dnx_dram_db_vendor_info_model_part_number_cbs model_part_number;
} dnx_dram_db_vendor_info_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_is_init_set_cb set;
    dnx_dram_db_gddr6_vendor_info_is_init_get_cb get;
} dnx_dram_db_gddr6_vendor_info_is_init_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_set_cb set;
    dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_get_cb get;
} dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_revision_id_set_cb set;
    dnx_dram_db_gddr6_vendor_info_revision_id_get_cb get;
} dnx_dram_db_gddr6_vendor_info_revision_id_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_density_set_cb set;
    dnx_dram_db_gddr6_vendor_info_density_get_cb get;
} dnx_dram_db_gddr6_vendor_info_density_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_internal_wck_set_cb set;
    dnx_dram_db_gddr6_vendor_info_internal_wck_get_cb get;
} dnx_dram_db_gddr6_vendor_info_internal_wck_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_wck_granularity_set_cb set;
    dnx_dram_db_gddr6_vendor_info_wck_granularity_get_cb get;
} dnx_dram_db_gddr6_vendor_info_wck_granularity_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_wck_frequency_set_cb set;
    dnx_dram_db_gddr6_vendor_info_wck_frequency_get_cb get;
} dnx_dram_db_gddr6_vendor_info_wck_frequency_cbs;


typedef struct {
    dnx_dram_db_gddr6_vendor_info_vddq_off_set_cb set;
    dnx_dram_db_gddr6_vendor_info_vddq_off_get_cb get;
} dnx_dram_db_gddr6_vendor_info_vddq_off_cbs;


typedef struct {
    
    dnx_dram_db_gddr6_vendor_info_is_init_cbs is_init;
    
    dnx_dram_db_gddr6_vendor_info_manufacturer_vendor_code_cbs manufacturer_vendor_code;
    
    dnx_dram_db_gddr6_vendor_info_revision_id_cbs revision_id;
    
    dnx_dram_db_gddr6_vendor_info_density_cbs density;
    
    dnx_dram_db_gddr6_vendor_info_internal_wck_cbs internal_wck;
    
    dnx_dram_db_gddr6_vendor_info_wck_granularity_cbs wck_granularity;
    
    dnx_dram_db_gddr6_vendor_info_wck_frequency_cbs wck_frequency;
    
    dnx_dram_db_gddr6_vendor_info_vddq_off_cbs vddq_off;
} dnx_dram_db_gddr6_vendor_info_cbs;


typedef struct {
    dnx_dram_db_is_init_cb is_init;
    dnx_dram_db_init_cb init;
    
    dnx_dram_db_is_initialized_cbs is_initialized;
    
    dnx_dram_db_sync_manager_cbs sync_manager;
    
    dnx_dram_db_power_down_callback_cbs power_down_callback;
    
    dnx_dram_db_override_bist_configurations_during_tuning_cbs override_bist_configurations_during_tuning;
    
    dnx_dram_db_channel_soft_init_after_bist_cbs channel_soft_init_after_bist;
    
    dnx_dram_db_traffic_tuning_core_mode_cbs traffic_tuning_core_mode;
    
    dnx_dram_db_dynamic_calibration_enabled_cbs dynamic_calibration_enabled;
    
    dnx_dram_db_dynamic_calibration_was_enabled_after_dram_tune_cbs dynamic_calibration_was_enabled_after_dram_tune;
    
    dnx_dram_db_temperature_monitoring_thread_state_cbs temperature_monitoring_thread_state;
    
    dnx_dram_db_temperature_monitoring_thread_handler_cbs temperature_monitoring_thread_handler;
    
    dnx_dram_db_vendor_info_cbs vendor_info;
    
    dnx_dram_db_gddr6_vendor_info_cbs gddr6_vendor_info;
} dnx_dram_db_cbs;






const char *
dnx_dram_temperature_monitoring_state_e_get_name(dnx_dram_temperature_monitoring_state_e value);




extern dnx_dram_db_cbs dnx_dram_db;

#endif 

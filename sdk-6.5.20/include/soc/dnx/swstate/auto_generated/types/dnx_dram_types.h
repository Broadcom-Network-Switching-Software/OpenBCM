
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef __DNX_DRAM_TYPES_H__
#define __DNX_DRAM_TYPES_H__

#include <soc/dnxc/swstate/dnxc_sw_state_h_includes.h>
#include <include/bcm/switch.h>
#include <include/shared/periodic_event.h>
#include <soc/dnxc/swstate/types/sw_state_mutex.h>


typedef sw_state_user_def_t* dnx_power_down_callback_userdata_t;




typedef enum {
    
    DNX_DRAM_NORMAL_TEMP = 0,
    
    DNX_DRAM_HIGH_TEMP = 1,
    
    DNX_DRAM_LOW_TEMP = 2
} dnx_dram_temperature_monitoring_state_e;



typedef struct {
    
    uint32 is_init;
    
    uint32 gen2_test;
    
    uint32 ecc;
    
    uint32 density;
    
    uint32 manufacturer_id;
    
    uint32 manufacturing_location;
    
    uint32 manufacturing_year;
    
    uint32 manufacturing_week;
    
    uint32 serial_number[2];
    
    uint32 addressing_mode;
    
    uint32 channel_available;
    
    uint32 hbm_stack_hight;
    
    uint32 model_part_number;
} hbm_dram_vendor_info_hbm_t;

typedef struct {
    
    uint32 is_init;
    
    uint32 manufacturer_vendor_code;
    
    uint32 revision_id;
    
    uint32 density;
    
    uint32 internal_wck;
    
    uint32 wck_granularity;
    
    uint32 wck_frequency;
    
    uint32 vddq_off;
} gddr6_dram_vendor_info_t;

typedef struct {
    
    sw_state_mutex_t deleted_buffers_file_mutex;
} dnx_dram_sync_manager_t;

typedef struct {
    
    bcm_switch_dram_power_down_callback_t callback;
    
    dnx_power_down_callback_userdata_t userdata;
    
    uint32 power_on;
} dnx_power_down_t;


typedef struct {
    
    int is_initialized;
    
    dnx_dram_sync_manager_t sync_manager;
    
    dnx_power_down_t power_down_callback;
    
    int override_bist_configurations_during_tuning;
    
    int channel_soft_init_after_bist;
    
    int traffic_tuning_core_mode;
    
    int dynamic_calibration_enabled;
    
    int dynamic_calibration_was_enabled_after_dram_tune[2];
    
    dnx_dram_temperature_monitoring_state_e temperature_monitoring_thread_state;
    
    periodic_event_handler_t temperature_monitoring_thread_handler;
    
    hbm_dram_vendor_info_hbm_t vendor_info[2];
    
    gddr6_dram_vendor_info_t gddr6_vendor_info[2];
} dnx_dram_db_t;


#endif 

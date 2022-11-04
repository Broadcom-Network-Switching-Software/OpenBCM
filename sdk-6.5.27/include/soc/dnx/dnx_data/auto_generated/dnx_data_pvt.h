
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_PVT_H_

#define _DNX_DATA_PVT_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_pvt.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_pvt_init(
    int unit);






typedef struct
{
    char *display_name;
    uint32 valid;
} dnx_data_pvt_general_pvt_name_t;



typedef enum
{
    dnx_data_pvt_general_pvt_temp_monitors_hw_support,
    dnx_data_pvt_general_pvt_peak_clear_support,
    dnx_data_pvt_general_pvt_peak_faulty,
    dnx_data_pvt_general_pvt_lock_support,

    
    _dnx_data_pvt_general_feature_nof
} dnx_data_pvt_general_feature_e;



typedef int(
    *dnx_data_pvt_general_feature_get_f) (
    int unit,
    dnx_data_pvt_general_feature_e feature);


typedef uint32(
    *dnx_data_pvt_general_nof_pvt_monitors_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_pvt_base_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_pvt_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_pvt_offset_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_nof_main_monitors_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_thermal_data_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_vol_sel_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_vol_factor_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_vol_multiple_get_f) (
    int unit);


typedef uint32(
    *dnx_data_pvt_general_vol_divisor_get_f) (
    int unit);


typedef const dnx_data_pvt_general_pvt_name_t *(
    *dnx_data_pvt_general_pvt_name_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_pvt_general_feature_get_f feature_get;
    
    dnx_data_pvt_general_nof_pvt_monitors_get_f nof_pvt_monitors_get;
    
    dnx_data_pvt_general_pvt_base_get_f pvt_base_get;
    
    dnx_data_pvt_general_pvt_factor_get_f pvt_factor_get;
    
    dnx_data_pvt_general_pvt_offset_get_f pvt_offset_get;
    
    dnx_data_pvt_general_nof_main_monitors_get_f nof_main_monitors_get;
    
    dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get_f pvt_mon_control_sel_nof_bits_get;
    
    dnx_data_pvt_general_thermal_data_nof_bits_get_f thermal_data_nof_bits_get;
    
    dnx_data_pvt_general_vol_sel_get_f vol_sel_get;
    
    dnx_data_pvt_general_vol_factor_get_f vol_factor_get;
    
    dnx_data_pvt_general_vol_multiple_get_f vol_multiple_get;
    
    dnx_data_pvt_general_vol_divisor_get_f vol_divisor_get;
    
    dnx_data_pvt_general_pvt_name_get_f pvt_name_get;
    
    dnxc_data_table_info_get_f pvt_name_info_get;
} dnx_data_if_pvt_general_t;






typedef struct
{
    char *name;
    int control_reg;
    int lock_bit;
    int remote_sensor_supported;
    uint32 vtmon_sel_field;
    int peak_data_clear_field;
    int temp_mon_sel;
    int data_reg;
    int thermal_data_field;
    int peak_data_field;
    int block_instance;
    int voltage_sensing;
    uint32 vdd_sel;
    uint32 vss_sel;
    int voltage_bit;
} dnx_data_pvt_vtmon_vtmon_access_t;



typedef enum
{
    dnx_data_pvt_vtmon_remote_temp_sensors_supported,

    
    _dnx_data_pvt_vtmon_feature_nof
} dnx_data_pvt_vtmon_feature_e;



typedef int(
    *dnx_data_pvt_vtmon_feature_get_f) (
    int unit,
    dnx_data_pvt_vtmon_feature_e feature);


typedef const dnx_data_pvt_vtmon_vtmon_access_t *(
    *dnx_data_pvt_vtmon_vtmon_access_get_f) (
    int unit,
    int index);



typedef struct
{
    
    dnx_data_pvt_vtmon_feature_get_f feature_get;
    
    dnx_data_pvt_vtmon_vtmon_access_get_f vtmon_access_get;
    
    dnxc_data_table_info_get_f vtmon_access_info_get;
} dnx_data_if_pvt_vtmon_t;





typedef struct
{
    
    dnx_data_if_pvt_general_t general;
    
    dnx_data_if_pvt_vtmon_t vtmon;
} dnx_data_if_pvt_t;




extern dnx_data_if_pvt_t dnx_data_pvt;


#endif 


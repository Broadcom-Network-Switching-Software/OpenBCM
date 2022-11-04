
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PVT_H_

#define _DNX_DATA_INTERNAL_PVT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_pvt_submodule_general,
    dnx_data_pvt_submodule_vtmon,

    
    _dnx_data_pvt_submodule_nof
} dnx_data_pvt_submodule_e;








int dnx_data_pvt_general_feature_get(
    int unit,
    dnx_data_pvt_general_feature_e feature);



typedef enum
{
    dnx_data_pvt_general_define_nof_pvt_monitors,
    dnx_data_pvt_general_define_pvt_base,
    dnx_data_pvt_general_define_pvt_factor,
    dnx_data_pvt_general_define_pvt_offset,
    dnx_data_pvt_general_define_nof_main_monitors,
    dnx_data_pvt_general_define_pvt_mon_control_sel_nof_bits,
    dnx_data_pvt_general_define_thermal_data_nof_bits,
    dnx_data_pvt_general_define_vol_sel,
    dnx_data_pvt_general_define_vol_factor,
    dnx_data_pvt_general_define_vol_multiple,
    dnx_data_pvt_general_define_vol_divisor,

    
    _dnx_data_pvt_general_define_nof
} dnx_data_pvt_general_define_e;



uint32 dnx_data_pvt_general_nof_pvt_monitors_get(
    int unit);


uint32 dnx_data_pvt_general_pvt_base_get(
    int unit);


uint32 dnx_data_pvt_general_pvt_factor_get(
    int unit);


uint32 dnx_data_pvt_general_pvt_offset_get(
    int unit);


uint32 dnx_data_pvt_general_nof_main_monitors_get(
    int unit);


uint32 dnx_data_pvt_general_pvt_mon_control_sel_nof_bits_get(
    int unit);


uint32 dnx_data_pvt_general_thermal_data_nof_bits_get(
    int unit);


uint32 dnx_data_pvt_general_vol_sel_get(
    int unit);


uint32 dnx_data_pvt_general_vol_factor_get(
    int unit);


uint32 dnx_data_pvt_general_vol_multiple_get(
    int unit);


uint32 dnx_data_pvt_general_vol_divisor_get(
    int unit);



typedef enum
{
    dnx_data_pvt_general_table_pvt_name,

    
    _dnx_data_pvt_general_table_nof
} dnx_data_pvt_general_table_e;



const dnx_data_pvt_general_pvt_name_t * dnx_data_pvt_general_pvt_name_get(
    int unit,
    int index);



shr_error_e dnx_data_pvt_general_pvt_name_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pvt_general_pvt_name_info_get(
    int unit);






int dnx_data_pvt_vtmon_feature_get(
    int unit,
    dnx_data_pvt_vtmon_feature_e feature);



typedef enum
{

    
    _dnx_data_pvt_vtmon_define_nof
} dnx_data_pvt_vtmon_define_e;




typedef enum
{
    dnx_data_pvt_vtmon_table_vtmon_access,

    
    _dnx_data_pvt_vtmon_table_nof
} dnx_data_pvt_vtmon_table_e;



const dnx_data_pvt_vtmon_vtmon_access_t * dnx_data_pvt_vtmon_vtmon_access_get(
    int unit,
    int index);



shr_error_e dnx_data_pvt_vtmon_vtmon_access_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_pvt_vtmon_vtmon_access_info_get(
    int unit);



shr_error_e dnx_data_pvt_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


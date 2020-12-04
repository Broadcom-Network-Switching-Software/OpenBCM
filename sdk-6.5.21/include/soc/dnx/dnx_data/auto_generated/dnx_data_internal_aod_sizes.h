

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_AOD_SIZES_H_

#define _DNX_DATA_INTERNAL_AOD_SIZES_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_aod_sizes.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_aod_sizes_submodule_AOD,

    
    _dnx_data_aod_sizes_submodule_nof
} dnx_data_aod_sizes_submodule_e;








int dnx_data_aod_sizes_AOD_feature_get(
    int unit,
    dnx_data_aod_sizes_AOD_feature_e feature);



typedef enum
{
    dnx_data_aod_sizes_AOD_define_count,

    
    _dnx_data_aod_sizes_AOD_define_nof
} dnx_data_aod_sizes_AOD_define_e;



uint32 dnx_data_aod_sizes_AOD_count_get(
    int unit);



typedef enum
{
    dnx_data_aod_sizes_AOD_table_sizes,
    dnx_data_aod_sizes_AOD_table_dynamic_sizes,

    
    _dnx_data_aod_sizes_AOD_table_nof
} dnx_data_aod_sizes_AOD_table_e;



const dnx_data_aod_sizes_AOD_sizes_t * dnx_data_aod_sizes_AOD_sizes_get(
    int unit,
    int index);


const dnx_data_aod_sizes_AOD_dynamic_sizes_t * dnx_data_aod_sizes_AOD_dynamic_sizes_get(
    int unit,
    int index);



shr_error_e dnx_data_aod_sizes_AOD_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_aod_sizes_AOD_dynamic_sizes_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_aod_sizes_AOD_sizes_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_aod_sizes_AOD_dynamic_sizes_info_get(
    int unit);



shr_error_e dnx_data_aod_sizes_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 




/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FIFODMA_H_

#define _DNX_DATA_INTERNAL_FIFODMA_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fifodma.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_fifodma_submodule_general,

    
    _dnx_data_fifodma_submodule_nof
} dnx_data_fifodma_submodule_e;








int dnx_data_fifodma_general_feature_get(
    int unit,
    dnx_data_fifodma_general_feature_e feature);



typedef enum
{
    dnx_data_fifodma_general_define_nof_fifodma_channels,
    dnx_data_fifodma_general_define_min_nof_host_entries,
    dnx_data_fifodma_general_define_max_nof_host_entries,

    
    _dnx_data_fifodma_general_define_nof
} dnx_data_fifodma_general_define_e;



uint32 dnx_data_fifodma_general_nof_fifodma_channels_get(
    int unit);


uint32 dnx_data_fifodma_general_min_nof_host_entries_get(
    int unit);


uint32 dnx_data_fifodma_general_max_nof_host_entries_get(
    int unit);



typedef enum
{
    dnx_data_fifodma_general_table_fifodma_map,

    
    _dnx_data_fifodma_general_table_nof
} dnx_data_fifodma_general_table_e;



const dnx_data_fifodma_general_fifodma_map_t * dnx_data_fifodma_general_fifodma_map_get(
    int unit,
    int fifodma_source);



shr_error_e dnx_data_fifodma_general_fifodma_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_fifodma_general_fifodma_map_info_get(
    int unit);



shr_error_e dnx_data_fifodma_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


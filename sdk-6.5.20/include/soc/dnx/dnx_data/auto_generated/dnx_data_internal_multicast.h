

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MULTICAST_H_

#define _DNX_DATA_INTERNAL_MULTICAST_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_multicast_submodule_params,

    
    _dnx_data_multicast_submodule_nof
} dnx_data_multicast_submodule_e;








int dnx_data_multicast_params_feature_get(
    int unit,
    dnx_data_multicast_params_feature_e feature);



typedef enum
{
    dnx_data_multicast_params_define_max_ing_mc_groups,
    dnx_data_multicast_params_define_max_egr_mc_groups,
    dnx_data_multicast_params_define_nof_ing_mc_bitmaps,
    dnx_data_multicast_params_define_nof_egr_mc_bitmaps,
    dnx_data_multicast_params_define_mcdb_allocation_method,
    dnx_data_multicast_params_define_nof_mcdb_entries_bits,
    dnx_data_multicast_params_define_mcdb_block_size,
    dnx_data_multicast_params_define_nof_mcdb_entries,

    
    _dnx_data_multicast_params_define_nof
} dnx_data_multicast_params_define_e;



int dnx_data_multicast_params_max_ing_mc_groups_get(
    int unit);


int dnx_data_multicast_params_max_egr_mc_groups_get(
    int unit);


uint32 dnx_data_multicast_params_nof_ing_mc_bitmaps_get(
    int unit);


uint32 dnx_data_multicast_params_nof_egr_mc_bitmaps_get(
    int unit);


uint32 dnx_data_multicast_params_mcdb_allocation_method_get(
    int unit);


uint32 dnx_data_multicast_params_nof_mcdb_entries_bits_get(
    int unit);


uint32 dnx_data_multicast_params_mcdb_block_size_get(
    int unit);


uint32 dnx_data_multicast_params_nof_mcdb_entries_get(
    int unit);



typedef enum
{

    
    _dnx_data_multicast_params_table_nof
} dnx_data_multicast_params_table_e;






shr_error_e dnx_data_multicast_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


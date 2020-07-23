

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MULTICAST_H_

#define _DNX_DATA_MULTICAST_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_multicast_init(
    int unit);







typedef enum
{

    
    _dnx_data_multicast_params_feature_nof
} dnx_data_multicast_params_feature_e;



typedef int(
    *dnx_data_multicast_params_feature_get_f) (
    int unit,
    dnx_data_multicast_params_feature_e feature);


typedef int(
    *dnx_data_multicast_params_max_ing_mc_groups_get_f) (
    int unit);


typedef int(
    *dnx_data_multicast_params_max_egr_mc_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_ing_mc_bitmaps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_egr_mc_bitmaps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_mcdb_allocation_method_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_mcdb_entries_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_mcdb_block_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_mcdb_entries_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_multicast_params_feature_get_f feature_get;
    
    dnx_data_multicast_params_max_ing_mc_groups_get_f max_ing_mc_groups_get;
    
    dnx_data_multicast_params_max_egr_mc_groups_get_f max_egr_mc_groups_get;
    
    dnx_data_multicast_params_nof_ing_mc_bitmaps_get_f nof_ing_mc_bitmaps_get;
    
    dnx_data_multicast_params_nof_egr_mc_bitmaps_get_f nof_egr_mc_bitmaps_get;
    
    dnx_data_multicast_params_mcdb_allocation_method_get_f mcdb_allocation_method_get;
    
    dnx_data_multicast_params_nof_mcdb_entries_bits_get_f nof_mcdb_entries_bits_get;
    
    dnx_data_multicast_params_mcdb_block_size_get_f mcdb_block_size_get;
    
    dnx_data_multicast_params_nof_mcdb_entries_get_f nof_mcdb_entries_get;
} dnx_data_if_multicast_params_t;





typedef struct
{
    
    dnx_data_if_multicast_params_t params;
} dnx_data_if_multicast_t;




extern dnx_data_if_multicast_t dnx_data_multicast;


#endif 


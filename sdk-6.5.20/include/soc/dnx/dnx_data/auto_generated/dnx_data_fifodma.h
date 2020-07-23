

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FIFODMA_H_

#define _DNX_DATA_FIFODMA_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_fifodma.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_fifodma_init(
    int unit);






typedef struct
{
    
    int fifodma_channel;
} dnx_data_fifodma_general_fifodma_map_t;



typedef enum
{

    
    _dnx_data_fifodma_general_feature_nof
} dnx_data_fifodma_general_feature_e;



typedef int(
    *dnx_data_fifodma_general_feature_get_f) (
    int unit,
    dnx_data_fifodma_general_feature_e feature);


typedef uint32(
    *dnx_data_fifodma_general_nof_fifodma_channels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fifodma_general_min_nof_host_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_fifodma_general_max_nof_host_entries_get_f) (
    int unit);


typedef const dnx_data_fifodma_general_fifodma_map_t *(
    *dnx_data_fifodma_general_fifodma_map_get_f) (
    int unit,
    int fifodma_source);



typedef struct
{
    
    dnx_data_fifodma_general_feature_get_f feature_get;
    
    dnx_data_fifodma_general_nof_fifodma_channels_get_f nof_fifodma_channels_get;
    
    dnx_data_fifodma_general_min_nof_host_entries_get_f min_nof_host_entries_get;
    
    dnx_data_fifodma_general_max_nof_host_entries_get_f max_nof_host_entries_get;
    
    dnx_data_fifodma_general_fifodma_map_get_f fifodma_map_get;
    
    dnxc_data_table_info_get_f fifodma_map_info_get;
} dnx_data_if_fifodma_general_t;





typedef struct
{
    
    dnx_data_if_fifodma_general_t general;
} dnx_data_if_fifodma_t;




extern dnx_data_if_fifodma_t dnx_data_fifodma;


#endif 


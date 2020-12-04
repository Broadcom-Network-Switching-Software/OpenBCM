

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_UTILEX_H_

#define _DNX_DATA_INTERNAL_UTILEX_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_utilex.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_utilex_submodule_common,

    
    _dnx_data_utilex_submodule_nof
} dnx_data_utilex_submodule_e;








int dnx_data_utilex_common_feature_get(
    int unit,
    dnx_data_utilex_common_feature_e feature);



typedef enum
{
    dnx_data_utilex_common_define_max_nof_dss_per_core,
    dnx_data_utilex_common_define_max_nof_hashs_per_core,
    dnx_data_utilex_common_define_max_nof_lists_per_core,
    dnx_data_utilex_common_define_max_nof_multis_per_core,
    dnx_data_utilex_common_define_max_nof_res_tag_bitmaps_per_core,
    dnx_data_utilex_common_define_max_nof_defragmented_chunks_per_core,

    
    _dnx_data_utilex_common_define_nof
} dnx_data_utilex_common_define_e;



uint32 dnx_data_utilex_common_max_nof_dss_per_core_get(
    int unit);


uint32 dnx_data_utilex_common_max_nof_hashs_per_core_get(
    int unit);


uint32 dnx_data_utilex_common_max_nof_lists_per_core_get(
    int unit);


uint32 dnx_data_utilex_common_max_nof_multis_per_core_get(
    int unit);


uint32 dnx_data_utilex_common_max_nof_res_tag_bitmaps_per_core_get(
    int unit);


uint32 dnx_data_utilex_common_max_nof_defragmented_chunks_per_core_get(
    int unit);



typedef enum
{

    
    _dnx_data_utilex_common_table_nof
} dnx_data_utilex_common_table_e;






shr_error_e dnx_data_utilex_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


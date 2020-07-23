

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_STG_H_

#define _DNX_DATA_INTERNAL_STG_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stg.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_stg_submodule_stg,

    
    _dnx_data_stg_submodule_nof
} dnx_data_stg_submodule_e;








int dnx_data_stg_stg_feature_get(
    int unit,
    dnx_data_stg_stg_feature_e feature);



typedef enum
{
    dnx_data_stg_stg_define_nof_topology_ids,

    
    _dnx_data_stg_stg_define_nof
} dnx_data_stg_stg_define_e;



uint32 dnx_data_stg_stg_nof_topology_ids_get(
    int unit);



typedef enum
{

    
    _dnx_data_stg_stg_table_nof
} dnx_data_stg_stg_table_e;






shr_error_e dnx_data_stg_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


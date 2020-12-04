

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PPMC_H_

#define _DNX_DATA_INTERNAL_PPMC_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ppmc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_ppmc_submodule_param,

    
    _dnx_data_ppmc_submodule_nof
} dnx_data_ppmc_submodule_e;








int dnx_data_ppmc_param_feature_get(
    int unit,
    dnx_data_ppmc_param_feature_e feature);



typedef enum
{
    dnx_data_ppmc_param_define_max_mc_replication_id,
    dnx_data_ppmc_param_define_min_mc_replication_id,

    
    _dnx_data_ppmc_param_define_nof
} dnx_data_ppmc_param_define_e;



uint32 dnx_data_ppmc_param_max_mc_replication_id_get(
    int unit);


uint32 dnx_data_ppmc_param_min_mc_replication_id_get(
    int unit);



typedef enum
{

    
    _dnx_data_ppmc_param_table_nof
} dnx_data_ppmc_param_table_e;






shr_error_e dnx_data_ppmc_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


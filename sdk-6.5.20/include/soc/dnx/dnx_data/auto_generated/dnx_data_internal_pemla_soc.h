

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PEMLA_SOC_H_

#define _DNX_DATA_INTERNAL_PEMLA_SOC_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pemla_soc.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_pemla_soc_submodule_soc,

    
    _dnx_data_pemla_soc_submodule_nof
} dnx_data_pemla_soc_submodule_e;








int dnx_data_pemla_soc_soc_feature_get(
    int unit,
    dnx_data_pemla_soc_soc_feature_e feature);



typedef enum
{
    dnx_data_pemla_soc_soc_define_JR1_MODE,
    dnx_data_pemla_soc_soc_define_JR2_MODE,
    dnx_data_pemla_soc_soc_define_system_headers_mode,

    
    _dnx_data_pemla_soc_soc_define_nof
} dnx_data_pemla_soc_soc_define_e;



uint32 dnx_data_pemla_soc_soc_JR1_MODE_get(
    int unit);


uint32 dnx_data_pemla_soc_soc_JR2_MODE_get(
    int unit);


uint32 dnx_data_pemla_soc_soc_system_headers_mode_get(
    int unit);



typedef enum
{

    
    _dnx_data_pemla_soc_soc_table_nof
} dnx_data_pemla_soc_soc_table_e;






shr_error_e dnx_data_pemla_soc_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


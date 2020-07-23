

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_PVT_H_

#define _DNX_DATA_INTERNAL_PVT_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pvt.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_pvt_submodule_general,

    
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

    
    _dnx_data_pvt_general_define_nof
} dnx_data_pvt_general_define_e;



uint32 dnx_data_pvt_general_nof_pvt_monitors_get(
    int unit);


uint32 dnx_data_pvt_general_pvt_base_get(
    int unit);


uint32 dnx_data_pvt_general_pvt_factor_get(
    int unit);



typedef enum
{

    
    _dnx_data_pvt_general_table_nof
} dnx_data_pvt_general_table_e;






shr_error_e dnx_data_pvt_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


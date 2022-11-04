
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_COSQ_AQM_H_

#define _DNX_DATA_INTERNAL_COSQ_AQM_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_cosq_aqm.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_cosq_aqm_submodule_general,

    
    _dnx_data_cosq_aqm_submodule_nof
} dnx_data_cosq_aqm_submodule_e;








int dnx_data_cosq_aqm_general_feature_get(
    int unit,
    dnx_data_cosq_aqm_general_feature_e feature);



typedef enum
{
    dnx_data_cosq_aqm_general_define_profile_nof,
    dnx_data_cosq_aqm_general_define_flows_nof,
    dnx_data_cosq_aqm_general_define_reserved_flows_nof,

    
    _dnx_data_cosq_aqm_general_define_nof
} dnx_data_cosq_aqm_general_define_e;



uint32 dnx_data_cosq_aqm_general_profile_nof_get(
    int unit);


uint32 dnx_data_cosq_aqm_general_flows_nof_get(
    int unit);


uint32 dnx_data_cosq_aqm_general_reserved_flows_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_cosq_aqm_general_table_nof
} dnx_data_cosq_aqm_general_table_e;






shr_error_e dnx_data_cosq_aqm_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


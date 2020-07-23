

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_SRV6_H_

#define _DNX_DATA_INTERNAL_SRV6_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_srv6_submodule_termination,
    dnx_data_srv6_submodule_encapsulation,

    
    _dnx_data_srv6_submodule_nof
} dnx_data_srv6_submodule_e;








int dnx_data_srv6_termination_feature_get(
    int unit,
    dnx_data_srv6_termination_feature_e feature);



typedef enum
{
    dnx_data_srv6_termination_define_min_nof_terminated_sids_psp,
    dnx_data_srv6_termination_define_max_nof_terminated_sids_psp,
    dnx_data_srv6_termination_define_min_nof_terminated_sids_usp,
    dnx_data_srv6_termination_define_max_nof_terminated_sids_usp,
    dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass,

    
    _dnx_data_srv6_termination_define_nof
} dnx_data_srv6_termination_define_e;



uint32 dnx_data_srv6_termination_min_nof_terminated_sids_psp_get(
    int unit);


uint32 dnx_data_srv6_termination_max_nof_terminated_sids_psp_get(
    int unit);


uint32 dnx_data_srv6_termination_min_nof_terminated_sids_usp_get(
    int unit);


uint32 dnx_data_srv6_termination_max_nof_terminated_sids_usp_get(
    int unit);


uint32 dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get(
    int unit);



typedef enum
{

    
    _dnx_data_srv6_termination_table_nof
} dnx_data_srv6_termination_table_e;









int dnx_data_srv6_encapsulation_feature_get(
    int unit,
    dnx_data_srv6_encapsulation_feature_e feature);



typedef enum
{
    dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass,
    dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass,

    
    _dnx_data_srv6_encapsulation_define_nof
} dnx_data_srv6_encapsulation_define_e;



uint32 dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_get(
    int unit);


uint32 dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_get(
    int unit);



typedef enum
{

    
    _dnx_data_srv6_encapsulation_table_nof
} dnx_data_srv6_encapsulation_table_e;






shr_error_e dnx_data_srv6_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


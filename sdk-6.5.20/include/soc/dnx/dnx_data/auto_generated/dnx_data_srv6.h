

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SRV6_H_

#define _DNX_DATA_SRV6_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_srv6.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_srv6_init(
    int unit);







typedef enum
{

    
    _dnx_data_srv6_termination_feature_nof
} dnx_data_srv6_termination_feature_e;



typedef int(
    *dnx_data_srv6_termination_feature_get_f) (
    int unit,
    dnx_data_srv6_termination_feature_e feature);


typedef uint32(
    *dnx_data_srv6_termination_min_nof_terminated_sids_psp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_psp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_min_nof_terminated_sids_usp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_usp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_srv6_termination_feature_get_f feature_get;
    
    dnx_data_srv6_termination_min_nof_terminated_sids_psp_get_f min_nof_terminated_sids_psp_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_psp_get_f max_nof_terminated_sids_psp_get;
    
    dnx_data_srv6_termination_min_nof_terminated_sids_usp_get_f min_nof_terminated_sids_usp_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_usp_get_f max_nof_terminated_sids_usp_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get_f max_nof_terminated_sids_usp_1pass_get;
} dnx_data_if_srv6_termination_t;







typedef enum
{

    
    _dnx_data_srv6_encapsulation_feature_nof
} dnx_data_srv6_encapsulation_feature_e;



typedef int(
    *dnx_data_srv6_encapsulation_feature_get_f) (
    int unit,
    dnx_data_srv6_encapsulation_feature_e feature);


typedef uint32(
    *dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_srv6_encapsulation_feature_get_f feature_get;
    
    dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_get_f max_nof_encap_sids_main_pass_get;
    
    dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_get_f nof_encap_sids_extended_pass_get;
} dnx_data_if_srv6_encapsulation_t;





typedef struct
{
    
    dnx_data_if_srv6_termination_t termination;
    
    dnx_data_if_srv6_encapsulation_t encapsulation;
} dnx_data_if_srv6_t;




extern dnx_data_if_srv6_t dnx_data_srv6;


#endif 



/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_SRV6_H_

#define _DNX_DATA_SRV6_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_srv6.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_srv6_init(
    int unit);







typedef enum
{
    dnx_data_srv6_general_jr2_compatible,
    dnx_data_srv6_general_srv6_gsid_prefix_64b_enable,
    dnx_data_srv6_general_srv6_usid_prefix_48b_enable,

    
    _dnx_data_srv6_general_feature_nof
} dnx_data_srv6_general_feature_e;



typedef int(
    *dnx_data_srv6_general_feature_get_f) (
    int unit,
    dnx_data_srv6_general_feature_e feature);



typedef struct
{
    
    dnx_data_srv6_general_feature_get_f feature_get;
} dnx_data_if_srv6_general_t;







typedef enum
{
    dnx_data_srv6_termination_srv6_psp_enable,

    
    _dnx_data_srv6_termination_feature_nof
} dnx_data_srv6_termination_feature_e;



typedef int(
    *dnx_data_srv6_termination_feature_get_f) (
    int unit,
    dnx_data_srv6_termination_feature_e feature);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_psp_extended_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_psp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_usp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_terminated_sids_usd_1pass_get_f) (
    int unit);


typedef uint32(
    *dnx_data_srv6_termination_max_nof_locators_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_srv6_termination_feature_get_f feature_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_psp_extended_get_f max_nof_terminated_sids_psp_extended_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_psp_get_f max_nof_terminated_sids_psp_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_usp_get_f max_nof_terminated_sids_usp_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_get_f max_nof_terminated_sids_usp_1pass_get;
    
    dnx_data_srv6_termination_max_nof_terminated_sids_usd_1pass_get_f max_nof_terminated_sids_usd_1pass_get;
    
    dnx_data_srv6_termination_max_nof_locators_get_f max_nof_locators_get;
} dnx_data_if_srv6_termination_t;







typedef enum
{
    dnx_data_srv6_encapsulation_is_t_insert_support,
    dnx_data_srv6_encapsulation_srv6_reduce_enable,

    
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







typedef enum
{

    
    _dnx_data_srv6_endpoint_feature_nof
} dnx_data_srv6_endpoint_feature_e;



typedef int(
    *dnx_data_srv6_endpoint_feature_get_f) (
    int unit,
    dnx_data_srv6_endpoint_feature_e feature);



typedef struct
{
    
    dnx_data_srv6_endpoint_feature_get_f feature_get;
} dnx_data_if_srv6_endpoint_t;





typedef struct
{
    
    dnx_data_if_srv6_general_t general;
    
    dnx_data_if_srv6_termination_t termination;
    
    dnx_data_if_srv6_encapsulation_t encapsulation;
    
    dnx_data_if_srv6_endpoint_t endpoint;
} dnx_data_if_srv6_t;




extern dnx_data_if_srv6_t dnx_data_srv6;


#endif 


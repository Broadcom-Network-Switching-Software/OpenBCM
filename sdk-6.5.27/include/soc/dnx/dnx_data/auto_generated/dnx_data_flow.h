
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_FLOW_H_

#define _DNX_DATA_FLOW_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_flow.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_flow_init(
    int unit);







typedef enum
{
    dnx_data_flow_general_is_tests_supported,
    dnx_data_flow_general_is_egress_virtual_supported,
    dnx_data_flow_general_is_inlif_legacy_profile_logic,
    dnx_data_flow_general_prp_profile_supported,
    dnx_data_flow_general_is_flow_enabled_for_legacy_applications,

    
    _dnx_data_flow_general_feature_nof
} dnx_data_flow_general_feature_e;



typedef int(
    *dnx_data_flow_general_feature_get_f) (
    int unit,
    dnx_data_flow_general_feature_e feature);


typedef uint32(
    *dnx_data_flow_general_match_per_gport_sw_nof_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_flow_general_match_entry_info_max_key_size_in_words_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_flow_general_feature_get_f feature_get;
    
    dnx_data_flow_general_match_per_gport_sw_nof_entries_get_f match_per_gport_sw_nof_entries_get;
    
    dnx_data_flow_general_match_entry_info_max_key_size_in_words_get_f match_entry_info_max_key_size_in_words_get;
} dnx_data_if_flow_general_t;







typedef enum
{

    
    _dnx_data_flow_srv6_feature_nof
} dnx_data_flow_srv6_feature_e;



typedef int(
    *dnx_data_flow_srv6_feature_get_f) (
    int unit,
    dnx_data_flow_srv6_feature_e feature);


typedef uint32(
    *dnx_data_flow_srv6_use_flow_lif_init_get_f) (
    int unit);


typedef uint32(
    *dnx_data_flow_srv6_use_flow_lif_term_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_flow_srv6_feature_get_f feature_get;
    
    dnx_data_flow_srv6_use_flow_lif_init_get_f use_flow_lif_init_get;
    
    dnx_data_flow_srv6_use_flow_lif_term_get_f use_flow_lif_term_get;
} dnx_data_if_flow_srv6_t;







typedef enum
{

    
    _dnx_data_flow_twamp_feature_nof
} dnx_data_flow_twamp_feature_e;



typedef int(
    *dnx_data_flow_twamp_feature_get_f) (
    int unit,
    dnx_data_flow_twamp_feature_e feature);


typedef uint32(
    *dnx_data_flow_twamp_twamp_reflector_supported_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_flow_twamp_feature_get_f feature_get;
    
    dnx_data_flow_twamp_twamp_reflector_supported_get_f twamp_reflector_supported_get;
} dnx_data_if_flow_twamp_t;





typedef struct
{
    
    dnx_data_if_flow_general_t general;
    
    dnx_data_if_flow_srv6_t srv6;
    
    dnx_data_if_flow_twamp_t twamp;
} dnx_data_if_flow_t;




extern dnx_data_if_flow_t dnx_data_flow;


#endif 




/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_TDM_H_

#define _DNX_DATA_TDM_H_

#include <sal/limits.h>
#include <soc/property.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_tdm.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



shr_error_e dnx_data_if_tdm_init(
    int unit);







typedef enum
{
    
    dnx_data_tdm_params_is_supported,

    
    _dnx_data_tdm_params_feature_nof
} dnx_data_tdm_params_feature_e;



typedef int(
    *dnx_data_tdm_params_feature_get_f) (
    int unit,
    dnx_data_tdm_params_feature_e feature);


typedef uint32(
    *dnx_data_tdm_params_context_map_base_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_context_map_base_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_sid_min_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_sid_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_nof_stream_ids_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_global_sid_offset_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_global_sid_offset_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_sid_offset_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_sid_offset_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_sid_offset_nof_in_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_context_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_nof_contexts_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_pkt_size_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_pkt_size_lower_limit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_pkt_size_upper_limit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_link_mask_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_link_mask_nof_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_tdm_tc_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_tdm_dp_get_f) (
    int unit);


typedef uint32(
    *dnx_data_tdm_params_mode_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_tdm_params_feature_get_f feature_get;
    
    dnx_data_tdm_params_context_map_base_size_get_f context_map_base_size_get;
    
    dnx_data_tdm_params_context_map_base_nof_get_f context_map_base_nof_get;
    
    dnx_data_tdm_params_sid_min_size_get_f sid_min_size_get;
    
    dnx_data_tdm_params_sid_size_get_f sid_size_get;
    
    dnx_data_tdm_params_nof_stream_ids_get_f nof_stream_ids_get;
    
    dnx_data_tdm_params_global_sid_offset_size_get_f global_sid_offset_size_get;
    
    dnx_data_tdm_params_global_sid_offset_nof_get_f global_sid_offset_nof_get;
    
    dnx_data_tdm_params_sid_offset_size_get_f sid_offset_size_get;
    
    dnx_data_tdm_params_sid_offset_nof_get_f sid_offset_nof_get;
    
    dnx_data_tdm_params_sid_offset_nof_in_bits_get_f sid_offset_nof_in_bits_get;
    
    dnx_data_tdm_params_context_size_get_f context_size_get;
    
    dnx_data_tdm_params_nof_contexts_get_f nof_contexts_get;
    
    dnx_data_tdm_params_pkt_size_size_get_f pkt_size_size_get;
    
    dnx_data_tdm_params_pkt_size_lower_limit_get_f pkt_size_lower_limit_get;
    
    dnx_data_tdm_params_pkt_size_upper_limit_get_f pkt_size_upper_limit_get;
    
    dnx_data_tdm_params_link_mask_size_get_f link_mask_size_get;
    
    dnx_data_tdm_params_link_mask_nof_get_f link_mask_nof_get;
    
    dnx_data_tdm_params_tdm_tc_get_f tdm_tc_get;
    
    dnx_data_tdm_params_tdm_dp_get_f tdm_dp_get;
    
    dnx_data_tdm_params_mode_get_f mode_get;
} dnx_data_if_tdm_params_t;





typedef struct
{
    
    dnx_data_if_tdm_params_t params;
} dnx_data_if_tdm_t;




extern dnx_data_if_tdm_t dnx_data_tdm;


#endif 


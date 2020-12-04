

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_TDM_H_

#define _DNX_DATA_INTERNAL_TDM_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_tdm_submodule_params,

    
    _dnx_data_tdm_submodule_nof
} dnx_data_tdm_submodule_e;








int dnx_data_tdm_params_feature_get(
    int unit,
    dnx_data_tdm_params_feature_e feature);



typedef enum
{
    dnx_data_tdm_params_define_context_map_base_size,
    dnx_data_tdm_params_define_context_map_base_nof,
    dnx_data_tdm_params_define_sid_min_size,
    dnx_data_tdm_params_define_sid_size,
    dnx_data_tdm_params_define_nof_stream_ids,
    dnx_data_tdm_params_define_global_sid_offset_size,
    dnx_data_tdm_params_define_global_sid_offset_nof,
    dnx_data_tdm_params_define_sid_offset_size,
    dnx_data_tdm_params_define_sid_offset_nof,
    dnx_data_tdm_params_define_sid_offset_nof_in_bits,
    dnx_data_tdm_params_define_context_size,
    dnx_data_tdm_params_define_nof_contexts,
    dnx_data_tdm_params_define_pkt_size_size,
    dnx_data_tdm_params_define_pkt_size_lower_limit,
    dnx_data_tdm_params_define_pkt_size_upper_limit,
    dnx_data_tdm_params_define_link_mask_size,
    dnx_data_tdm_params_define_link_mask_nof,
    dnx_data_tdm_params_define_tdm_tc,
    dnx_data_tdm_params_define_tdm_dp,
    dnx_data_tdm_params_define_mode,

    
    _dnx_data_tdm_params_define_nof
} dnx_data_tdm_params_define_e;



uint32 dnx_data_tdm_params_context_map_base_size_get(
    int unit);


uint32 dnx_data_tdm_params_context_map_base_nof_get(
    int unit);


uint32 dnx_data_tdm_params_sid_min_size_get(
    int unit);


uint32 dnx_data_tdm_params_sid_size_get(
    int unit);


uint32 dnx_data_tdm_params_nof_stream_ids_get(
    int unit);


uint32 dnx_data_tdm_params_global_sid_offset_size_get(
    int unit);


uint32 dnx_data_tdm_params_global_sid_offset_nof_get(
    int unit);


uint32 dnx_data_tdm_params_sid_offset_size_get(
    int unit);


uint32 dnx_data_tdm_params_sid_offset_nof_get(
    int unit);


uint32 dnx_data_tdm_params_sid_offset_nof_in_bits_get(
    int unit);


uint32 dnx_data_tdm_params_context_size_get(
    int unit);


uint32 dnx_data_tdm_params_nof_contexts_get(
    int unit);


uint32 dnx_data_tdm_params_pkt_size_size_get(
    int unit);


uint32 dnx_data_tdm_params_pkt_size_lower_limit_get(
    int unit);


uint32 dnx_data_tdm_params_pkt_size_upper_limit_get(
    int unit);


uint32 dnx_data_tdm_params_link_mask_size_get(
    int unit);


uint32 dnx_data_tdm_params_link_mask_nof_get(
    int unit);


uint32 dnx_data_tdm_params_tdm_tc_get(
    int unit);


uint32 dnx_data_tdm_params_tdm_dp_get(
    int unit);


uint32 dnx_data_tdm_params_mode_get(
    int unit);



typedef enum
{

    
    _dnx_data_tdm_params_table_nof
} dnx_data_tdm_params_table_e;






shr_error_e dnx_data_tdm_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


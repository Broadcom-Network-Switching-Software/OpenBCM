
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DEBUG_H_

#define _DNX_DATA_INTERNAL_DEBUG_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_debug_submodule_mem,
    dnx_data_debug_submodule_feature,
    dnx_data_debug_submodule_kleap,
    dnx_data_debug_submodule_general,
    dnx_data_debug_submodule_nif,

    
    _dnx_data_debug_submodule_nof
} dnx_data_debug_submodule_e;








int dnx_data_debug_mem_feature_get(
    int unit,
    dnx_data_debug_mem_feature_e feature);



typedef enum
{

    
    _dnx_data_debug_mem_define_nof
} dnx_data_debug_mem_define_e;




typedef enum
{
    dnx_data_debug_mem_table_params,
    dnx_data_debug_mem_table_block_map,
    dnx_data_debug_mem_table_mem_array,

    
    _dnx_data_debug_mem_table_nof
} dnx_data_debug_mem_table_e;



const dnx_data_debug_mem_params_t * dnx_data_debug_mem_params_get(
    int unit,
    int block);


const dnx_data_debug_mem_block_map_t * dnx_data_debug_mem_block_map_get(
    int unit,
    int block);


const dnx_data_debug_mem_mem_array_t * dnx_data_debug_mem_mem_array_get(
    int unit,
    int block);



shr_error_e dnx_data_debug_mem_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_debug_mem_block_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);


shr_error_e dnx_data_debug_mem_mem_array_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_debug_mem_params_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_debug_mem_block_map_info_get(
    int unit);


const dnxc_data_table_info_t * dnx_data_debug_mem_mem_array_info_get(
    int unit);






int dnx_data_debug_feature_feature_get(
    int unit,
    dnx_data_debug_feature_feature_e feature);



typedef enum
{

    
    _dnx_data_debug_feature_define_nof
} dnx_data_debug_feature_define_e;




typedef enum
{

    
    _dnx_data_debug_feature_table_nof
} dnx_data_debug_feature_table_e;









int dnx_data_debug_kleap_feature_get(
    int unit,
    dnx_data_debug_kleap_feature_e feature);



typedef enum
{
    dnx_data_debug_kleap_define_fwd1_gen_data_size,
    dnx_data_debug_kleap_define_fwd2_gen_data_size,

    
    _dnx_data_debug_kleap_define_nof
} dnx_data_debug_kleap_define_e;



uint32 dnx_data_debug_kleap_fwd1_gen_data_size_get(
    int unit);


uint32 dnx_data_debug_kleap_fwd2_gen_data_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_debug_kleap_table_nof
} dnx_data_debug_kleap_table_e;









int dnx_data_debug_general_feature_get(
    int unit,
    dnx_data_debug_general_feature_e feature);



typedef enum
{
    dnx_data_debug_general_define_global_visibility,
    dnx_data_debug_general_define_has_etpp_visibility_enabler,
    dnx_data_debug_general_define_has_periodic_visibility,
    dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold,
    dnx_data_debug_general_define_egress_port_mc_visibility,
    dnx_data_debug_general_define_pmf_debug_signals,
    dnx_data_debug_general_define_fer_debug_signals,
    dnx_data_debug_general_define_ees_data_device_debug_signals,
    dnx_data_debug_general_define_lbp_debug_signals_register_based,
    dnx_data_debug_general_define_signal_version,
    dnx_data_debug_general_define_mem_array_index,

    
    _dnx_data_debug_general_define_nof
} dnx_data_debug_general_define_e;



uint32 dnx_data_debug_general_global_visibility_get(
    int unit);


uint32 dnx_data_debug_general_has_etpp_visibility_enabler_get(
    int unit);


uint32 dnx_data_debug_general_has_periodic_visibility_get(
    int unit);


uint32 dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get(
    int unit);


uint32 dnx_data_debug_general_egress_port_mc_visibility_get(
    int unit);


uint32 dnx_data_debug_general_pmf_debug_signals_get(
    int unit);


uint32 dnx_data_debug_general_fer_debug_signals_get(
    int unit);


uint32 dnx_data_debug_general_ees_data_device_debug_signals_get(
    int unit);


uint32 dnx_data_debug_general_lbp_debug_signals_register_based_get(
    int unit);


uint32 dnx_data_debug_general_signal_version_get(
    int unit);


uint32 dnx_data_debug_general_mem_array_index_get(
    int unit);



typedef enum
{

    
    _dnx_data_debug_general_table_nof
} dnx_data_debug_general_table_e;









int dnx_data_debug_nif_feature_get(
    int unit,
    dnx_data_debug_nif_feature_e feature);



typedef enum
{
    dnx_data_debug_nif_define_extension_header,

    
    _dnx_data_debug_nif_define_nof
} dnx_data_debug_nif_define_e;



uint32 dnx_data_debug_nif_extension_header_get(
    int unit);



typedef enum
{

    
    _dnx_data_debug_nif_table_nof
} dnx_data_debug_nif_table_e;






shr_error_e dnx_data_debug_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


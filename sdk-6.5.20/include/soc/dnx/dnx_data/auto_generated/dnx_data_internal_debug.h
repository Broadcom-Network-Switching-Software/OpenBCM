

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_DEBUG_H_

#define _DNX_DATA_INTERNAL_DEBUG_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_debug.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_debug_submodule_mem,
    dnx_data_debug_submodule_feature,
    dnx_data_debug_submodule_kleap,
    dnx_data_debug_submodule_general,

    
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

    
    _dnx_data_debug_mem_table_nof
} dnx_data_debug_mem_table_e;



const dnx_data_debug_mem_params_t * dnx_data_debug_mem_params_get(
    int unit,
    int block);



shr_error_e dnx_data_debug_mem_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_debug_mem_params_info_get(
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
    dnx_data_debug_general_define_etpp_eop_and_sop_dec_above_threshold,
    dnx_data_debug_general_define_egress_port_mc_visibility,
    dnx_data_debug_general_define_mdb_debug_signals,
    dnx_data_debug_general_define_mem_array_index,

    
    _dnx_data_debug_general_define_nof
} dnx_data_debug_general_define_e;



uint32 dnx_data_debug_general_global_visibility_get(
    int unit);


uint32 dnx_data_debug_general_etpp_eop_and_sop_dec_above_threshold_get(
    int unit);


uint32 dnx_data_debug_general_egress_port_mc_visibility_get(
    int unit);


uint32 dnx_data_debug_general_mdb_debug_signals_get(
    int unit);


uint32 dnx_data_debug_general_mem_array_index_get(
    int unit);



typedef enum
{

    
    _dnx_data_debug_general_table_nof
} dnx_data_debug_general_table_e;






shr_error_e dnx_data_debug_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


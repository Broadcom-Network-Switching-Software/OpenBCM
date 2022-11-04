
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_FLOW_H_

#define _DNX_DATA_INTERNAL_FLOW_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_flow.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_flow_submodule_general,
    dnx_data_flow_submodule_srv6,
    dnx_data_flow_submodule_twamp,

    
    _dnx_data_flow_submodule_nof
} dnx_data_flow_submodule_e;








int dnx_data_flow_general_feature_get(
    int unit,
    dnx_data_flow_general_feature_e feature);



typedef enum
{
    dnx_data_flow_general_define_match_per_gport_sw_nof_entries,
    dnx_data_flow_general_define_match_entry_info_max_key_size_in_words,

    
    _dnx_data_flow_general_define_nof
} dnx_data_flow_general_define_e;



uint32 dnx_data_flow_general_match_per_gport_sw_nof_entries_get(
    int unit);


uint32 dnx_data_flow_general_match_entry_info_max_key_size_in_words_get(
    int unit);



typedef enum
{

    
    _dnx_data_flow_general_table_nof
} dnx_data_flow_general_table_e;









int dnx_data_flow_srv6_feature_get(
    int unit,
    dnx_data_flow_srv6_feature_e feature);



typedef enum
{
    dnx_data_flow_srv6_define_use_flow_lif_init,
    dnx_data_flow_srv6_define_use_flow_lif_term,

    
    _dnx_data_flow_srv6_define_nof
} dnx_data_flow_srv6_define_e;



uint32 dnx_data_flow_srv6_use_flow_lif_init_get(
    int unit);


uint32 dnx_data_flow_srv6_use_flow_lif_term_get(
    int unit);



typedef enum
{

    
    _dnx_data_flow_srv6_table_nof
} dnx_data_flow_srv6_table_e;









int dnx_data_flow_twamp_feature_get(
    int unit,
    dnx_data_flow_twamp_feature_e feature);



typedef enum
{
    dnx_data_flow_twamp_define_twamp_reflector_supported,

    
    _dnx_data_flow_twamp_define_nof
} dnx_data_flow_twamp_define_e;



uint32 dnx_data_flow_twamp_twamp_reflector_supported_get(
    int unit);



typedef enum
{

    
    _dnx_data_flow_twamp_table_nof
} dnx_data_flow_twamp_table_e;






shr_error_e dnx_data_flow_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


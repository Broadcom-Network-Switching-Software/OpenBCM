

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_INGR_REASSEMBLY_H_

#define _DNX_DATA_INTERNAL_INGR_REASSEMBLY_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_reassembly.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



typedef enum
{
    dnx_data_ingr_reassembly_submodule_context,
    dnx_data_ingr_reassembly_submodule_ilkn,
    dnx_data_ingr_reassembly_submodule_rcy,
    dnx_data_ingr_reassembly_submodule_priority,
    dnx_data_ingr_reassembly_submodule_dbal,

    
    _dnx_data_ingr_reassembly_submodule_nof
} dnx_data_ingr_reassembly_submodule_e;








int dnx_data_ingr_reassembly_context_feature_get(
    int unit,
    dnx_data_ingr_reassembly_context_feature_e feature);



typedef enum
{
    dnx_data_ingr_reassembly_context_define_nof_contexts,
    dnx_data_ingr_reassembly_context_define_invalid_context,

    
    _dnx_data_ingr_reassembly_context_define_nof
} dnx_data_ingr_reassembly_context_define_e;



uint32 dnx_data_ingr_reassembly_context_nof_contexts_get(
    int unit);


uint32 dnx_data_ingr_reassembly_context_invalid_context_get(
    int unit);



typedef enum
{
    dnx_data_ingr_reassembly_context_table_context_map,

    
    _dnx_data_ingr_reassembly_context_table_nof
} dnx_data_ingr_reassembly_context_table_e;



const dnx_data_ingr_reassembly_context_context_map_t * dnx_data_ingr_reassembly_context_context_map_get(
    int unit,
    int port_type);



shr_error_e dnx_data_ingr_reassembly_context_context_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_ingr_reassembly_context_context_map_info_get(
    int unit);






int dnx_data_ingr_reassembly_ilkn_feature_get(
    int unit,
    dnx_data_ingr_reassembly_ilkn_feature_e feature);



typedef enum
{

    
    _dnx_data_ingr_reassembly_ilkn_define_nof
} dnx_data_ingr_reassembly_ilkn_define_e;




typedef enum
{

    
    _dnx_data_ingr_reassembly_ilkn_table_nof
} dnx_data_ingr_reassembly_ilkn_table_e;









int dnx_data_ingr_reassembly_rcy_feature_get(
    int unit,
    dnx_data_ingr_reassembly_rcy_feature_e feature);



typedef enum
{
    dnx_data_ingr_reassembly_rcy_define_nof_interfaces,

    
    _dnx_data_ingr_reassembly_rcy_define_nof
} dnx_data_ingr_reassembly_rcy_define_e;



uint32 dnx_data_ingr_reassembly_rcy_nof_interfaces_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_reassembly_rcy_table_nof
} dnx_data_ingr_reassembly_rcy_table_e;









int dnx_data_ingr_reassembly_priority_feature_get(
    int unit,
    dnx_data_ingr_reassembly_priority_feature_e feature);



typedef enum
{
    dnx_data_ingr_reassembly_priority_define_nif_eth_priorities_nof,
    dnx_data_ingr_reassembly_priority_define_mirror_priorities_nof,
    dnx_data_ingr_reassembly_priority_define_rcy_priorities_nof,
    dnx_data_ingr_reassembly_priority_define_priorities_nof,
    dnx_data_ingr_reassembly_priority_define_cgm_priorities_nof,

    
    _dnx_data_ingr_reassembly_priority_define_nof
} dnx_data_ingr_reassembly_priority_define_e;



uint32 dnx_data_ingr_reassembly_priority_nif_eth_priorities_nof_get(
    int unit);


uint32 dnx_data_ingr_reassembly_priority_mirror_priorities_nof_get(
    int unit);


uint32 dnx_data_ingr_reassembly_priority_rcy_priorities_nof_get(
    int unit);


uint32 dnx_data_ingr_reassembly_priority_priorities_nof_get(
    int unit);


uint32 dnx_data_ingr_reassembly_priority_cgm_priorities_nof_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_reassembly_priority_table_nof
} dnx_data_ingr_reassembly_priority_table_e;









int dnx_data_ingr_reassembly_dbal_feature_get(
    int unit,
    dnx_data_ingr_reassembly_dbal_feature_e feature);



typedef enum
{
    dnx_data_ingr_reassembly_dbal_define_reassembly_context_bits,
    dnx_data_ingr_reassembly_dbal_define_interface_bits,
    dnx_data_ingr_reassembly_dbal_define_interface_max,
    dnx_data_ingr_reassembly_dbal_define_context_map_base_address_bits,
    dnx_data_ingr_reassembly_dbal_define_context_map_base_address_max,
    dnx_data_ingr_reassembly_dbal_define_context_map_base_offest_ext_max,
    dnx_data_ingr_reassembly_dbal_define_interleaved_interface_max,
    dnx_data_ingr_reassembly_dbal_define_interleaved_interface_bits,

    
    _dnx_data_ingr_reassembly_dbal_define_nof
} dnx_data_ingr_reassembly_dbal_define_e;



uint32 dnx_data_ingr_reassembly_dbal_reassembly_context_bits_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_interface_bits_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_interface_max_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_context_map_base_address_bits_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_context_map_base_address_max_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_context_map_base_offest_ext_max_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_interleaved_interface_max_get(
    int unit);


uint32 dnx_data_ingr_reassembly_dbal_interleaved_interface_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_ingr_reassembly_dbal_table_nof
} dnx_data_ingr_reassembly_dbal_table_e;






shr_error_e dnx_data_ingr_reassembly_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


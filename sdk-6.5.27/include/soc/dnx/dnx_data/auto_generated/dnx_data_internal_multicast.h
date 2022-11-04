
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_MULTICAST_H_

#define _DNX_DATA_INTERNAL_MULTICAST_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_multicast.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_multicast_submodule_general,
    dnx_data_multicast_submodule_params,
    dnx_data_multicast_submodule_mcdb_hash_table,

    
    _dnx_data_multicast_submodule_nof
} dnx_data_multicast_submodule_e;








int dnx_data_multicast_general_feature_get(
    int unit,
    dnx_data_multicast_general_feature_e feature);



typedef enum
{

    
    _dnx_data_multicast_general_define_nof
} dnx_data_multicast_general_define_e;




typedef enum
{

    
    _dnx_data_multicast_general_table_nof
} dnx_data_multicast_general_table_e;









int dnx_data_multicast_params_feature_get(
    int unit,
    dnx_data_multicast_params_feature_e feature);



typedef enum
{
    dnx_data_multicast_params_define_max_ing_mc_groups,
    dnx_data_multicast_params_define_max_egr_mc_groups,
    dnx_data_multicast_params_define_nof_ing_mc_bitmaps,
    dnx_data_multicast_params_define_nof_egr_mc_bitmaps,
    dnx_data_multicast_params_define_mcdb_allocation_method,
    dnx_data_multicast_params_define_nof_mcdb_entries_bits,
    dnx_data_multicast_params_define_ingr_dest_bits,
    dnx_data_multicast_params_define_max_static_element_id,
    dnx_data_multicast_params_define_mcdb_block_size,
    dnx_data_multicast_params_define_nof_mcdb_entries,
    dnx_data_multicast_params_define_max_nof_ingress_bmp_dest,

    
    _dnx_data_multicast_params_define_nof
} dnx_data_multicast_params_define_e;



int dnx_data_multicast_params_max_ing_mc_groups_get(
    int unit);


int dnx_data_multicast_params_max_egr_mc_groups_get(
    int unit);


uint32 dnx_data_multicast_params_nof_ing_mc_bitmaps_get(
    int unit);


uint32 dnx_data_multicast_params_nof_egr_mc_bitmaps_get(
    int unit);


uint32 dnx_data_multicast_params_mcdb_allocation_method_get(
    int unit);


uint32 dnx_data_multicast_params_nof_mcdb_entries_bits_get(
    int unit);


uint32 dnx_data_multicast_params_ingr_dest_bits_get(
    int unit);


uint32 dnx_data_multicast_params_max_static_element_id_get(
    int unit);


uint32 dnx_data_multicast_params_mcdb_block_size_get(
    int unit);


uint32 dnx_data_multicast_params_nof_mcdb_entries_get(
    int unit);


uint32 dnx_data_multicast_params_max_nof_ingress_bmp_dest_get(
    int unit);



typedef enum
{
    dnx_data_multicast_params_table_olp_ing_multicast_id,

    
    _dnx_data_multicast_params_table_nof
} dnx_data_multicast_params_table_e;



const dnx_data_multicast_params_olp_ing_multicast_id_t * dnx_data_multicast_params_olp_ing_multicast_id_get(
    int unit,
    int cores_group);



shr_error_e dnx_data_multicast_params_olp_ing_multicast_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index);



const dnxc_data_table_info_t * dnx_data_multicast_params_olp_ing_multicast_id_info_get(
    int unit);






int dnx_data_multicast_mcdb_hash_table_feature_get(
    int unit,
    dnx_data_multicast_mcdb_hash_table_feature_e feature);



typedef enum
{
    dnx_data_multicast_mcdb_hash_table_define_max_key_size_in_words,
    dnx_data_multicast_mcdb_hash_table_define_destination_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_destination_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_reference_num_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_reference_num_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_destination_type_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_destination_type_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_info_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_info_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_info_type_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_info_type_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_group_id_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_group_id_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_is_ingress_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_is_ingress_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_core_id_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_core_id_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_destination2_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_destination2_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_destination2_type_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_destination2_type_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_valid_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_same_if_dspa_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_valid_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_same_if_outlif_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_forward_group_valid_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_forward_group_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_forward_group_nof_bits,
    dnx_data_multicast_mcdb_hash_table_define_is_static_start_bit,
    dnx_data_multicast_mcdb_hash_table_define_is_static_nof_bits,

    
    _dnx_data_multicast_mcdb_hash_table_define_nof
} dnx_data_multicast_mcdb_hash_table_define_e;



uint32 dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_info_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_info_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_info_type_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_info_type_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_group_id_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_core_id_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination2_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination2_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination2_type_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_destination2_type_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_dspa_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_dspa_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_outlif_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_same_if_outlif_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_forward_group_valid_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_forward_group_valid_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_forward_group_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_forward_group_nof_bits_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_is_static_start_bit_get(
    int unit);


uint32 dnx_data_multicast_mcdb_hash_table_is_static_nof_bits_get(
    int unit);



typedef enum
{

    
    _dnx_data_multicast_mcdb_hash_table_table_nof
} dnx_data_multicast_mcdb_hash_table_table_e;






shr_error_e dnx_data_multicast_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


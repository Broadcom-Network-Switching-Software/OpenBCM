
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MULTICAST_H_

#define _DNX_DATA_MULTICAST_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_multicast.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_multicast_init(
    int unit);







typedef enum
{
    dnx_data_multicast_general_is_used,
    dnx_data_multicast_general_power_down_supported,

    
    _dnx_data_multicast_general_feature_nof
} dnx_data_multicast_general_feature_e;



typedef int(
    *dnx_data_multicast_general_feature_get_f) (
    int unit,
    dnx_data_multicast_general_feature_e feature);



typedef struct
{
    
    dnx_data_multicast_general_feature_get_f feature_get;
} dnx_data_if_multicast_general_t;






typedef struct
{
    uint32 multicast_id;
} dnx_data_multicast_params_olp_ing_multicast_id_t;



typedef enum
{
    dnx_data_multicast_params_mcdb_formats_v2,
    dnx_data_multicast_params_mc_pruning_actions_not_supported,

    
    _dnx_data_multicast_params_feature_nof
} dnx_data_multicast_params_feature_e;



typedef int(
    *dnx_data_multicast_params_feature_get_f) (
    int unit,
    dnx_data_multicast_params_feature_e feature);


typedef int(
    *dnx_data_multicast_params_max_ing_mc_groups_get_f) (
    int unit);


typedef int(
    *dnx_data_multicast_params_max_egr_mc_groups_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_ing_mc_bitmaps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_egr_mc_bitmaps_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_mcdb_allocation_method_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_mcdb_entries_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_ingr_dest_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_max_static_element_id_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_mcdb_block_size_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_nof_mcdb_entries_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_params_max_nof_ingress_bmp_dest_get_f) (
    int unit);


typedef const dnx_data_multicast_params_olp_ing_multicast_id_t *(
    *dnx_data_multicast_params_olp_ing_multicast_id_get_f) (
    int unit,
    int cores_group);



typedef struct
{
    
    dnx_data_multicast_params_feature_get_f feature_get;
    
    dnx_data_multicast_params_max_ing_mc_groups_get_f max_ing_mc_groups_get;
    
    dnx_data_multicast_params_max_egr_mc_groups_get_f max_egr_mc_groups_get;
    
    dnx_data_multicast_params_nof_ing_mc_bitmaps_get_f nof_ing_mc_bitmaps_get;
    
    dnx_data_multicast_params_nof_egr_mc_bitmaps_get_f nof_egr_mc_bitmaps_get;
    
    dnx_data_multicast_params_mcdb_allocation_method_get_f mcdb_allocation_method_get;
    
    dnx_data_multicast_params_nof_mcdb_entries_bits_get_f nof_mcdb_entries_bits_get;
    
    dnx_data_multicast_params_ingr_dest_bits_get_f ingr_dest_bits_get;
    
    dnx_data_multicast_params_max_static_element_id_get_f max_static_element_id_get;
    
    dnx_data_multicast_params_mcdb_block_size_get_f mcdb_block_size_get;
    
    dnx_data_multicast_params_nof_mcdb_entries_get_f nof_mcdb_entries_get;
    
    dnx_data_multicast_params_max_nof_ingress_bmp_dest_get_f max_nof_ingress_bmp_dest_get;
    
    dnx_data_multicast_params_olp_ing_multicast_id_get_f olp_ing_multicast_id_get;
    
    dnxc_data_table_info_get_f olp_ing_multicast_id_info_get;
} dnx_data_if_multicast_params_t;







typedef enum
{

    
    _dnx_data_multicast_mcdb_hash_table_feature_nof
} dnx_data_multicast_mcdb_hash_table_feature_e;



typedef int(
    *dnx_data_multicast_mcdb_hash_table_feature_get_f) (
    int unit,
    dnx_data_multicast_mcdb_hash_table_feature_e feature);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_info_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_info_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_info_type_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_info_type_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_group_id_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_core_id_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination2_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination2_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination2_type_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_destination2_type_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_dspa_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_dspa_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_outlif_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_same_if_outlif_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_forward_group_valid_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_forward_group_valid_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_forward_group_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_forward_group_nof_bits_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_is_static_start_bit_get_f) (
    int unit);


typedef uint32(
    *dnx_data_multicast_mcdb_hash_table_is_static_nof_bits_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_multicast_mcdb_hash_table_feature_get_f feature_get;
    
    dnx_data_multicast_mcdb_hash_table_max_key_size_in_words_get_f max_key_size_in_words_get;
    
    dnx_data_multicast_mcdb_hash_table_destination_start_bit_get_f destination_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_destination_nof_bits_get_f destination_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_reference_num_start_bit_get_f reference_num_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_reference_num_nof_bits_get_f reference_num_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_destination_type_start_bit_get_f destination_type_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_destination_type_nof_bits_get_f destination_type_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_info_start_bit_get_f info_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_info_nof_bits_get_f info_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_info_type_start_bit_get_f info_type_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_info_type_nof_bits_get_f info_type_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_group_id_start_bit_get_f group_id_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_group_id_nof_bits_get_f group_id_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_is_ingress_start_bit_get_f is_ingress_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_is_ingress_nof_bits_get_f is_ingress_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_core_id_start_bit_get_f core_id_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_core_id_nof_bits_get_f core_id_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_destination2_start_bit_get_f destination2_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_destination2_nof_bits_get_f destination2_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_destination2_type_start_bit_get_f destination2_type_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_destination2_type_nof_bits_get_f destination2_type_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_start_bit_get_f same_if_dspa_valid_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_dspa_valid_nof_bits_get_f same_if_dspa_valid_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_dspa_start_bit_get_f same_if_dspa_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_dspa_nof_bits_get_f same_if_dspa_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_start_bit_get_f same_if_outlif_valid_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_outlif_valid_nof_bits_get_f same_if_outlif_valid_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_outlif_start_bit_get_f same_if_outlif_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_same_if_outlif_nof_bits_get_f same_if_outlif_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_forward_group_valid_start_bit_get_f forward_group_valid_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_forward_group_valid_nof_bits_get_f forward_group_valid_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_forward_group_start_bit_get_f forward_group_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_forward_group_nof_bits_get_f forward_group_nof_bits_get;
    
    dnx_data_multicast_mcdb_hash_table_is_static_start_bit_get_f is_static_start_bit_get;
    
    dnx_data_multicast_mcdb_hash_table_is_static_nof_bits_get_f is_static_nof_bits_get;
} dnx_data_if_multicast_mcdb_hash_table_t;





typedef struct
{
    
    dnx_data_if_multicast_general_t general;
    
    dnx_data_if_multicast_params_t params;
    
    dnx_data_if_multicast_mcdb_hash_table_t mcdb_hash_table;
} dnx_data_if_multicast_t;




extern dnx_data_if_multicast_t dnx_data_multicast;


#endif 


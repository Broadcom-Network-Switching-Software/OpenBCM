
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INFRA_TESTS_H_

#define _DNX_DATA_INFRA_TESTS_H_

#include <sal/limits.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/property.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnxc/dnxc_data/dnxc_data_utils.h>
#include <soc/dnxc/dnxc_data/dnxc_data_mgmt.h>
#include <bcm_int/dnx/algo/res_mngr/resource_tag_bitmap.h>
#include <bcm_int/dnx/init/init_mem.h>
#include <soc/dnx/dnx_data/dnx_data_verify.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_infra_tests.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



shr_error_e dnx_data_if_infra_tests_init(
    int unit);







typedef enum
{
    dnx_data_infra_tests_res_mngr_example_init_enable,

    
    _dnx_data_infra_tests_res_mngr_example_feature_nof
} dnx_data_infra_tests_res_mngr_example_feature_e;



typedef int(
    *dnx_data_infra_tests_res_mngr_example_feature_get_f) (
    int unit,
    dnx_data_infra_tests_res_mngr_example_feature_e feature);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_nof_tags_levels_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_grain_size_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_grain_size_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_max_tag_value_1_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_max_tag_value_2_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_nof_elements_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_first_element_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_create_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_extra_arg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_advanced_algorithm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_res_mngr_example_data_per_entry_size_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_infra_tests_res_mngr_example_feature_get_f feature_get;
    
    dnx_data_infra_tests_res_mngr_example_nof_tags_levels_get_f nof_tags_levels_get;
    
    dnx_data_infra_tests_res_mngr_example_grain_size_1_get_f grain_size_1_get;
    
    dnx_data_infra_tests_res_mngr_example_grain_size_2_get_f grain_size_2_get;
    
    dnx_data_infra_tests_res_mngr_example_max_tag_value_1_get_f max_tag_value_1_get;
    
    dnx_data_infra_tests_res_mngr_example_max_tag_value_2_get_f max_tag_value_2_get;
    
    dnx_data_infra_tests_res_mngr_example_nof_elements_get_f nof_elements_get;
    
    dnx_data_infra_tests_res_mngr_example_first_element_get_f first_element_get;
    
    dnx_data_infra_tests_res_mngr_example_create_flags_get_f create_flags_get;
    
    dnx_data_infra_tests_res_mngr_example_extra_arg_get_f extra_arg_get;
    
    dnx_data_infra_tests_res_mngr_example_advanced_algorithm_get_f advanced_algorithm_get;
    
    dnx_data_infra_tests_res_mngr_example_data_per_entry_size_get_f data_per_entry_size_get;
} dnx_data_if_infra_tests_res_mngr_example_t;







typedef enum
{
    dnx_data_infra_tests_temp_mngr_example_init_enable,

    
    _dnx_data_infra_tests_temp_mngr_example_feature_nof
} dnx_data_infra_tests_temp_mngr_example_feature_e;



typedef int(
    *dnx_data_infra_tests_temp_mngr_example_feature_get_f) (
    int unit,
    dnx_data_infra_tests_temp_mngr_example_feature_e feature);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_create_flags_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_first_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_nof_profiles_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_max_references_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_default_profile_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_extra_arg_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_element_8_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_element_16_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_element_32_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_temp_mngr_example_element_int_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_infra_tests_temp_mngr_example_feature_get_f feature_get;
    
    dnx_data_infra_tests_temp_mngr_example_create_flags_get_f create_flags_get;
    
    dnx_data_infra_tests_temp_mngr_example_first_profile_get_f first_profile_get;
    
    dnx_data_infra_tests_temp_mngr_example_nof_profiles_get_f nof_profiles_get;
    
    dnx_data_infra_tests_temp_mngr_example_max_references_get_f max_references_get;
    
    dnx_data_infra_tests_temp_mngr_example_default_profile_get_f default_profile_get;
    
    dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_get_f advanced_algorithm_get;
    
    dnx_data_infra_tests_temp_mngr_example_extra_arg_get_f extra_arg_get;
    
    dnx_data_infra_tests_temp_mngr_example_element_8_get_f element_8_get;
    
    dnx_data_infra_tests_temp_mngr_example_element_16_get_f element_16_get;
    
    dnx_data_infra_tests_temp_mngr_example_element_32_get_f element_32_get;
    
    dnx_data_infra_tests_temp_mngr_example_element_int_get_f element_int_get;
} dnx_data_if_infra_tests_temp_mngr_example_t;







typedef enum
{
    dnx_data_infra_tests_linked_list_example_init_enable,

    
    _dnx_data_infra_tests_linked_list_example_feature_nof
} dnx_data_infra_tests_linked_list_example_feature_e;



typedef int(
    *dnx_data_infra_tests_linked_list_example_feature_get_f) (
    int unit,
    dnx_data_infra_tests_linked_list_example_feature_e feature);



typedef struct
{
    
    dnx_data_infra_tests_linked_list_example_feature_get_f feature_get;
} dnx_data_if_infra_tests_linked_list_example_t;







typedef enum
{
    dnx_data_infra_tests_sw_state_init_enable,

    
    _dnx_data_infra_tests_sw_state_feature_nof
} dnx_data_infra_tests_sw_state_feature_e;



typedef int(
    *dnx_data_infra_tests_sw_state_feature_get_f) (
    int unit,
    dnx_data_infra_tests_sw_state_feature_e feature);


typedef uint32(
    *dnx_data_infra_tests_sw_state_warmboot_backoff_rate_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_sw_state_htb_max_nof_elements_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_sw_state_insert_nof_pairs_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_sw_state_htb_rh_hashing_algorithm_get_f) (
    int unit);


typedef uint32(
    *dnx_data_infra_tests_sw_state_htb_rh_create_flags_get_f) (
    int unit);



typedef struct
{
    
    dnx_data_infra_tests_sw_state_feature_get_f feature_get;
    
    dnx_data_infra_tests_sw_state_warmboot_backoff_rate_get_f warmboot_backoff_rate_get;
    
    dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_get_f nof_guaranteed_reboots_get;
    
    dnx_data_infra_tests_sw_state_htb_max_nof_elements_get_f htb_max_nof_elements_get;
    
    dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_get_f htb_max_nof_elements_index_get;
    
    dnx_data_infra_tests_sw_state_insert_nof_pairs_get_f insert_nof_pairs_get;
    
    dnx_data_infra_tests_sw_state_htb_rh_hashing_algorithm_get_f htb_rh_hashing_algorithm_get;
    
    dnx_data_infra_tests_sw_state_htb_rh_create_flags_get_f htb_rh_create_flags_get;
} dnx_data_if_infra_tests_sw_state_t;





typedef struct
{
    
    dnx_data_if_infra_tests_res_mngr_example_t res_mngr_example;
    
    dnx_data_if_infra_tests_temp_mngr_example_t temp_mngr_example;
    
    dnx_data_if_infra_tests_linked_list_example_t linked_list_example;
    
    dnx_data_if_infra_tests_sw_state_t sw_state;
} dnx_data_if_infra_tests_t;




extern dnx_data_if_infra_tests_t dnx_data_infra_tests;


#endif 



/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_INTERNAL_INFRA_TESTS_H_

#define _DNX_DATA_INTERNAL_INFRA_TESTS_H_

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_infra_tests.h>



#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif



typedef enum
{
    dnx_data_infra_tests_submodule_res_mngr_example,
    dnx_data_infra_tests_submodule_temp_mngr_example,
    dnx_data_infra_tests_submodule_linked_list_example,
    dnx_data_infra_tests_submodule_sw_state,

    
    _dnx_data_infra_tests_submodule_nof
} dnx_data_infra_tests_submodule_e;








int dnx_data_infra_tests_res_mngr_example_feature_get(
    int unit,
    dnx_data_infra_tests_res_mngr_example_feature_e feature);



typedef enum
{
    dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels,
    dnx_data_infra_tests_res_mngr_example_define_grain_size_1,
    dnx_data_infra_tests_res_mngr_example_define_grain_size_2,
    dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1,
    dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2,
    dnx_data_infra_tests_res_mngr_example_define_nof_elements,
    dnx_data_infra_tests_res_mngr_example_define_first_element,
    dnx_data_infra_tests_res_mngr_example_define_create_flags,
    dnx_data_infra_tests_res_mngr_example_define_extra_arg,
    dnx_data_infra_tests_res_mngr_example_define_advanced_algorithm,
    dnx_data_infra_tests_res_mngr_example_define_data_per_entry_size,

    
    _dnx_data_infra_tests_res_mngr_example_define_nof
} dnx_data_infra_tests_res_mngr_example_define_e;



uint32 dnx_data_infra_tests_res_mngr_example_nof_tags_levels_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_grain_size_1_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_grain_size_2_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_max_tag_value_1_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_max_tag_value_2_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_nof_elements_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_first_element_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_create_flags_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_extra_arg_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_advanced_algorithm_get(
    int unit);


uint32 dnx_data_infra_tests_res_mngr_example_data_per_entry_size_get(
    int unit);



typedef enum
{

    
    _dnx_data_infra_tests_res_mngr_example_table_nof
} dnx_data_infra_tests_res_mngr_example_table_e;









int dnx_data_infra_tests_temp_mngr_example_feature_get(
    int unit,
    dnx_data_infra_tests_temp_mngr_example_feature_e feature);



typedef enum
{
    dnx_data_infra_tests_temp_mngr_example_define_create_flags,
    dnx_data_infra_tests_temp_mngr_example_define_first_profile,
    dnx_data_infra_tests_temp_mngr_example_define_nof_profiles,
    dnx_data_infra_tests_temp_mngr_example_define_max_references,
    dnx_data_infra_tests_temp_mngr_example_define_default_profile,
    dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm,
    dnx_data_infra_tests_temp_mngr_example_define_extra_arg,
    dnx_data_infra_tests_temp_mngr_example_define_element_8,
    dnx_data_infra_tests_temp_mngr_example_define_element_16,
    dnx_data_infra_tests_temp_mngr_example_define_element_32,
    dnx_data_infra_tests_temp_mngr_example_define_element_int,

    
    _dnx_data_infra_tests_temp_mngr_example_define_nof
} dnx_data_infra_tests_temp_mngr_example_define_e;



uint32 dnx_data_infra_tests_temp_mngr_example_create_flags_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_first_profile_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_nof_profiles_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_max_references_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_default_profile_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_extra_arg_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_element_8_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_element_16_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_element_32_get(
    int unit);


uint32 dnx_data_infra_tests_temp_mngr_example_element_int_get(
    int unit);



typedef enum
{

    
    _dnx_data_infra_tests_temp_mngr_example_table_nof
} dnx_data_infra_tests_temp_mngr_example_table_e;









int dnx_data_infra_tests_linked_list_example_feature_get(
    int unit,
    dnx_data_infra_tests_linked_list_example_feature_e feature);



typedef enum
{

    
    _dnx_data_infra_tests_linked_list_example_define_nof
} dnx_data_infra_tests_linked_list_example_define_e;




typedef enum
{

    
    _dnx_data_infra_tests_linked_list_example_table_nof
} dnx_data_infra_tests_linked_list_example_table_e;









int dnx_data_infra_tests_sw_state_feature_get(
    int unit,
    dnx_data_infra_tests_sw_state_feature_e feature);



typedef enum
{
    dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate,
    dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots,
    dnx_data_infra_tests_sw_state_define_htb_max_nof_elements,
    dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index,
    dnx_data_infra_tests_sw_state_define_insert_nof_pairs,
    dnx_data_infra_tests_sw_state_define_htb_rh_hashing_algorithm,
    dnx_data_infra_tests_sw_state_define_htb_rh_create_flags,

    
    _dnx_data_infra_tests_sw_state_define_nof
} dnx_data_infra_tests_sw_state_define_e;



uint32 dnx_data_infra_tests_sw_state_warmboot_backoff_rate_get(
    int unit);


uint32 dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_get(
    int unit);


uint32 dnx_data_infra_tests_sw_state_htb_max_nof_elements_get(
    int unit);


uint32 dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_get(
    int unit);


uint32 dnx_data_infra_tests_sw_state_insert_nof_pairs_get(
    int unit);


uint32 dnx_data_infra_tests_sw_state_htb_rh_hashing_algorithm_get(
    int unit);


uint32 dnx_data_infra_tests_sw_state_htb_rh_create_flags_get(
    int unit);



typedef enum
{

    
    _dnx_data_infra_tests_sw_state_table_nof
} dnx_data_infra_tests_sw_state_table_e;






shr_error_e dnx_data_infra_tests_init(
    int unit,
    dnxc_data_module_t *module_data);



#endif 


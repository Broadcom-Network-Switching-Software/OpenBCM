
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INIT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_infra_tests.h>




extern shr_error_e jr2_a0_data_infra_tests_attach(
    int unit);




static shr_error_e
dnx_data_infra_tests_res_mngr_example_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "res_mngr_example";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_infra_tests_res_mngr_example_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data infra_tests res_mngr_example features");

    submodule_data->features[dnx_data_infra_tests_res_mngr_example_init_enable].name = "init_enable";
    submodule_data->features[dnx_data_infra_tests_res_mngr_example_init_enable].doc = "";
    submodule_data->features[dnx_data_infra_tests_res_mngr_example_init_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_infra_tests_res_mngr_example_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data infra_tests res_mngr_example defines");

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels].name = "nof_tags_levels";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_grain_size_1].name = "grain_size_1";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_grain_size_1].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_grain_size_1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_grain_size_2].name = "grain_size_2";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_grain_size_2].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_grain_size_2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1].name = "max_tag_value_1";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2].name = "max_tag_value_2";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_nof_elements].name = "nof_elements";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_nof_elements].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_nof_elements].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_first_element].name = "first_element";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_first_element].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_first_element].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_create_flags].name = "create_flags";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_create_flags].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_create_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_extra_arg].name = "extra_arg";
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_extra_arg].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_res_mngr_example_define_extra_arg].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_infra_tests_res_mngr_example_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data infra_tests res_mngr_example tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_infra_tests_res_mngr_example_feature_get(
    int unit,
    dnx_data_infra_tests_res_mngr_example_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, feature);
}


uint32
dnx_data_infra_tests_res_mngr_example_nof_tags_levels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels);
}

uint32
dnx_data_infra_tests_res_mngr_example_grain_size_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_grain_size_1);
}

uint32
dnx_data_infra_tests_res_mngr_example_grain_size_2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_grain_size_2);
}

uint32
dnx_data_infra_tests_res_mngr_example_max_tag_value_1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1);
}

uint32
dnx_data_infra_tests_res_mngr_example_max_tag_value_2_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2);
}

uint32
dnx_data_infra_tests_res_mngr_example_nof_elements_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_nof_elements);
}

uint32
dnx_data_infra_tests_res_mngr_example_first_element_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_first_element);
}

uint32
dnx_data_infra_tests_res_mngr_example_create_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_create_flags);
}

uint32
dnx_data_infra_tests_res_mngr_example_extra_arg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_res_mngr_example, dnx_data_infra_tests_res_mngr_example_define_extra_arg);
}










static shr_error_e
dnx_data_infra_tests_temp_mngr_example_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "temp_mngr_example";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_infra_tests_temp_mngr_example_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data infra_tests temp_mngr_example features");

    submodule_data->features[dnx_data_infra_tests_temp_mngr_example_init_enable].name = "init_enable";
    submodule_data->features[dnx_data_infra_tests_temp_mngr_example_init_enable].doc = "";
    submodule_data->features[dnx_data_infra_tests_temp_mngr_example_init_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_infra_tests_temp_mngr_example_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data infra_tests temp_mngr_example defines");

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_create_flags].name = "create_flags";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_create_flags].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_create_flags].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_first_profile].name = "first_profile";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_first_profile].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_first_profile].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_nof_profiles].name = "nof_profiles";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_nof_profiles].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_nof_profiles].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_max_references].name = "max_references";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_max_references].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_max_references].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_default_profile].name = "default_profile";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_default_profile].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_default_profile].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm].name = "advanced_algorithm";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_extra_arg].name = "extra_arg";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_extra_arg].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_extra_arg].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_8].name = "element_8";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_8].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_8].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_16].name = "element_16";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_16].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_16].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_32].name = "element_32";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_32].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_32].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_int].name = "element_int";
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_int].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_temp_mngr_example_define_element_int].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_infra_tests_temp_mngr_example_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data infra_tests temp_mngr_example tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_infra_tests_temp_mngr_example_feature_get(
    int unit,
    dnx_data_infra_tests_temp_mngr_example_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, feature);
}


uint32
dnx_data_infra_tests_temp_mngr_example_create_flags_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_create_flags);
}

uint32
dnx_data_infra_tests_temp_mngr_example_first_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_first_profile);
}

uint32
dnx_data_infra_tests_temp_mngr_example_nof_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_nof_profiles);
}

uint32
dnx_data_infra_tests_temp_mngr_example_max_references_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_max_references);
}

uint32
dnx_data_infra_tests_temp_mngr_example_default_profile_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_default_profile);
}

uint32
dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm);
}

uint32
dnx_data_infra_tests_temp_mngr_example_extra_arg_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_extra_arg);
}

uint32
dnx_data_infra_tests_temp_mngr_example_element_8_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_element_8);
}

uint32
dnx_data_infra_tests_temp_mngr_example_element_16_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_element_16);
}

uint32
dnx_data_infra_tests_temp_mngr_example_element_32_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_element_32);
}

uint32
dnx_data_infra_tests_temp_mngr_example_element_int_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_temp_mngr_example, dnx_data_infra_tests_temp_mngr_example_define_element_int);
}










static shr_error_e
dnx_data_infra_tests_linked_list_example_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "linked_list_example";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_infra_tests_linked_list_example_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data infra_tests linked_list_example features");

    submodule_data->features[dnx_data_infra_tests_linked_list_example_init_enable].name = "init_enable";
    submodule_data->features[dnx_data_infra_tests_linked_list_example_init_enable].doc = "";
    submodule_data->features[dnx_data_infra_tests_linked_list_example_init_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_infra_tests_linked_list_example_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data infra_tests linked_list_example defines");

    
    submodule_data->nof_tables = _dnx_data_infra_tests_linked_list_example_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data infra_tests linked_list_example tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_infra_tests_linked_list_example_feature_get(
    int unit,
    dnx_data_infra_tests_linked_list_example_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_linked_list_example, feature);
}











static shr_error_e
dnx_data_infra_tests_sw_state_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sw_state";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_infra_tests_sw_state_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data infra_tests sw_state features");

    submodule_data->features[dnx_data_infra_tests_sw_state_init_enable].name = "init_enable";
    submodule_data->features[dnx_data_infra_tests_sw_state_init_enable].doc = "";
    submodule_data->features[dnx_data_infra_tests_sw_state_init_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_infra_tests_sw_state_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data infra_tests sw_state defines");

    submodule_data->defines[dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate].name = "warmboot_backoff_rate";
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots].name = "nof_guaranteed_reboots";
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_sw_state_define_htb_max_nof_elements].name = "htb_max_nof_elements";
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_htb_max_nof_elements].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_htb_max_nof_elements].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index].name = "htb_max_nof_elements_index";
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_infra_tests_sw_state_define_insert_nof_pairs].name = "insert_nof_pairs";
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_insert_nof_pairs].doc = "";
    
    submodule_data->defines[dnx_data_infra_tests_sw_state_define_insert_nof_pairs].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_infra_tests_sw_state_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data infra_tests sw_state tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_infra_tests_sw_state_feature_get(
    int unit,
    dnx_data_infra_tests_sw_state_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_sw_state, feature);
}


uint32
dnx_data_infra_tests_sw_state_warmboot_backoff_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_sw_state, dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate);
}

uint32
dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_sw_state, dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots);
}

uint32
dnx_data_infra_tests_sw_state_htb_max_nof_elements_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_sw_state, dnx_data_infra_tests_sw_state_define_htb_max_nof_elements);
}

uint32
dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_sw_state, dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index);
}

uint32
dnx_data_infra_tests_sw_state_insert_nof_pairs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_infra_tests, dnx_data_infra_tests_submodule_sw_state, dnx_data_infra_tests_sw_state_define_insert_nof_pairs);
}







shr_error_e
dnx_data_infra_tests_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "infra_tests";
    module_data->nof_submodules = _dnx_data_infra_tests_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data infra_tests submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_infra_tests_res_mngr_example_init(unit, &module_data->submodules[dnx_data_infra_tests_submodule_res_mngr_example]));
    SHR_IF_ERR_EXIT(dnx_data_infra_tests_temp_mngr_example_init(unit, &module_data->submodules[dnx_data_infra_tests_submodule_temp_mngr_example]));
    SHR_IF_ERR_EXIT(dnx_data_infra_tests_linked_list_example_init(unit, &module_data->submodules[dnx_data_infra_tests_submodule_linked_list_example]));
    SHR_IF_ERR_EXIT(dnx_data_infra_tests_sw_state_init(unit, &module_data->submodules[dnx_data_infra_tests_submodule_sw_state]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_infra_tests_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


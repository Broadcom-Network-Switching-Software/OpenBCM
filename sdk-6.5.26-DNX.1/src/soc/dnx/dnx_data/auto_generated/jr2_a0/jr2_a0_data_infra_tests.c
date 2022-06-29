
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
#include <bcm_int/dnx/spb/spb.h>
#include <bcm_int/dnx/init/init.h>







static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_init_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int feature_index = dnx_data_infra_tests_res_mngr_example_init_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_RES_MNGR_TEST;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "init_enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_nof_tags_levels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "nof_tags_levels";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_grain_size_1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_grain_size_1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "grain_size_1";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_grain_size_2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_grain_size_2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "grain_size_2";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_max_tag_value_1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "max_tag_value_1";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_max_tag_value_2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "max_tag_value_2";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_nof_elements_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_nof_elements;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "nof_elements";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_first_element_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_first_element;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "first_element";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_create_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_create_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "create_flags";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_extra_arg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_extra_arg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "extra_arg";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_advanced_algorithm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_advanced_algorithm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "advanced_algorithm";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_res_mngr_example_data_per_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    int define_index = dnx_data_infra_tests_res_mngr_example_define_data_per_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "res_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "data_per_entry_size";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_init_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int feature_index = dnx_data_infra_tests_temp_mngr_example_init_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_TEMP_MNGR_TEST;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "init_enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_create_flags_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_create_flags;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "create_flags";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_first_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_first_profile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "first_profile";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_nof_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_nof_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "nof_profiles";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_max_references_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_max_references;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "max_references";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_default_profile_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_default_profile;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "default_profile";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "advanced_algorithm";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_extra_arg_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_extra_arg;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "extra_arg";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_8_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_element_8;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFF;

    
    define->data = 0xFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "element_8";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_16_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_element_16;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1122;

    
    define->data = 0x1122;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "element_16";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_32_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_element_32;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x00112233;

    
    define->data = 0x00112233;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "element_32";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_int_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    int define_index = dnx_data_infra_tests_temp_mngr_example_define_element_int;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFFFF;

    
    define->data = 0xFFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "temp_mngr_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "element_int";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_infra_tests_linked_list_example_init_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_linked_list_example;
    int feature_index = dnx_data_infra_tests_linked_list_example_init_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_SW_STATE_TEST;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "linked_list_init_enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_init_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int feature_index = dnx_data_infra_tests_sw_state_init_enable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_SW_STATE_TEST;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "init_enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_warmboot_backoff_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int define_index = dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int define_index = dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5;

    
    define->data = 5;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_htb_max_nof_elements_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int define_index = dnx_data_infra_tests_sw_state_define_htb_max_nof_elements;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "sw_state_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "htb_max_nof_elements";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int define_index = dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "sw_state_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "htb_max_nof_elements_index";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_insert_nof_pairs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int define_index = dnx_data_infra_tests_sw_state_define_insert_nof_pairs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "sw_state_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "insert_nof_pairs";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_infra_tests_sw_state_htb_hashing_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = dnx_data_infra_tests_submodule_sw_state;
    int define_index = dnx_data_infra_tests_sw_state_define_htb_hashing;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "sw_state_test";
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "htb_hashing";
    define->property.range_min = 0;
    define->property.range_max = 0xFFFFFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);
    define->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_infra_tests_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_infra_tests;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_infra_tests_submodule_res_mngr_example;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_infra_tests_res_mngr_example_define_nof_tags_levels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_nof_tags_levels_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_grain_size_1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_grain_size_1_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_grain_size_2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_grain_size_2_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_max_tag_value_1;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_max_tag_value_1_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_max_tag_value_2;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_max_tag_value_2_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_nof_elements;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_nof_elements_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_first_element;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_first_element_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_create_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_create_flags_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_extra_arg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_extra_arg_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_advanced_algorithm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_advanced_algorithm_set;
    data_index = dnx_data_infra_tests_res_mngr_example_define_data_per_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_data_per_entry_size_set;

    
    data_index = dnx_data_infra_tests_res_mngr_example_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_init_enable_set;

    
    
    submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_infra_tests_temp_mngr_example_define_create_flags;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_create_flags_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_first_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_first_profile_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_nof_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_nof_profiles_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_max_references;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_max_references_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_default_profile;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_default_profile_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_advanced_algorithm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_advanced_algorithm_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_extra_arg;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_extra_arg_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_element_8;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_8_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_element_16;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_16_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_element_32;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_32_set;
    data_index = dnx_data_infra_tests_temp_mngr_example_define_element_int;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_element_int_set;

    
    data_index = dnx_data_infra_tests_temp_mngr_example_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_init_enable_set;

    
    
    submodule_index = dnx_data_infra_tests_submodule_linked_list_example;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_infra_tests_linked_list_example_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_linked_list_example_init_enable_set;

    
    
    submodule_index = dnx_data_infra_tests_submodule_sw_state;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_warmboot_backoff_rate_set;
    data_index = dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_set;
    data_index = dnx_data_infra_tests_sw_state_define_htb_max_nof_elements;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_htb_max_nof_elements_set;
    data_index = dnx_data_infra_tests_sw_state_define_htb_max_nof_elements_index;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_htb_max_nof_elements_index_set;
    data_index = dnx_data_infra_tests_sw_state_define_insert_nof_pairs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_insert_nof_pairs_set;
    data_index = dnx_data_infra_tests_sw_state_define_htb_hashing;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_htb_hashing_set;

    
    data_index = dnx_data_infra_tests_sw_state_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_sw_state_init_enable_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


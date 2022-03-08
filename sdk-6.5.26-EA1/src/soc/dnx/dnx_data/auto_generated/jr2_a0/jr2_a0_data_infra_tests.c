
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
        "Specify if example resource manager example tests should be init. Disabled in default.>\n"
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
        "Specify the number of tags levels in the example resource manager. In default equals 1.\n"
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
        "Specify the 1st grain size in the example resource manager. In default equals 0.\n"
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
        "Specify the 2nd grain size in the example resource manager. In default equals 0.\n"
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
        "Specify the 1st max tag value in the example resource manager. In default equals 0.\n"
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
        "Specify the 2nd max tag value in the example resource manager. In default equals 0.\n"
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
        "Specify the number of elements in example resource manager. In default equals 25.\n"
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
        "Specify the 1st max tag value in the example resource manager. In default equals 0.\n"
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
        "Specify the creation flags of example resource manager. In default equals 0.\n"
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
        "Specify the example resource manager has extra argument. In default equals 0.\n"
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
        "Specify if example template manager should be init. Disabled in default.>\n"
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
        "Specify if example should be init. Disabled in default.>\n"
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

    
    data_index = dnx_data_infra_tests_res_mngr_example_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_res_mngr_example_init_enable_set;

    
    
    submodule_index = dnx_data_infra_tests_submodule_temp_mngr_example;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_infra_tests_temp_mngr_example_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_temp_mngr_example_init_enable_set;

    
    
    submodule_index = dnx_data_infra_tests_submodule_sw_state;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_infra_tests_sw_state_define_warmboot_backoff_rate;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_warmboot_backoff_rate_set;
    data_index = dnx_data_infra_tests_sw_state_define_nof_guaranteed_reboots;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_infra_tests_sw_state_nof_guaranteed_reboots_set;

    
    data_index = dnx_data_infra_tests_sw_state_init_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_infra_tests_sw_state_init_enable_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


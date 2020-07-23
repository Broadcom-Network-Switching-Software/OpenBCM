

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MPLS

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_mpls.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_term_1_or_2_labels_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_mpls_term_1_or_2_labels;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_ioam_d_flag_stamping_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_mpls_ioam_d_flag_stamping;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_cbts_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_mpls_cbts;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_stack_termination_indication_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_stack_termination_indication;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_evpn_with_fl_cw_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_evpn_with_fl_cw;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "Enable or disable Flow-Label and Flow-Label + Control-Word in EVPN.\n"
        "custom_feature_enable_fl_cw_in_evpn=0/1.\n"
        "The feature is enabled by defeault.\n"
        "This is a temporary SOC property for 6.5.20 release only.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "enable_fl_cw_in_evpn";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);
    feature->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_mpls_general_nof_mpls_termination_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_nof_mpls_termination_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 8;

    
    define->data = 8;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_nof_mpls_push_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_nof_mpls_push_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_nof_bits_mpls_label_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_nof_bits_mpls_label;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_log_nof_mpls_range_elements_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_log_nof_mpls_range_elements;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_nof_mpls_range_elements_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_nof_mpls_range_elements;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1 << dnx_data_mpls.general.log_nof_mpls_range_elements_get(unit);

    
    define->data = 1 << dnx_data_mpls.general.log_nof_mpls_range_elements_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_special_label_encap_handling_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_mpls_special_label_encap_handling;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_control_word_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_mpls_control_word_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1;

    
    define->data = 1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_second_stage_parser_handling_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_mpls_second_stage_parser_handling;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_control_word_flag_bits_lsb_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_mpls_control_word_flag_bits_lsb;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_fhei_mps_type_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_fhei_mps_type_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_mpls_speculative_learning_handling_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_mpls_speculative_learning_handling;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_mpls_general_nof_php_gport_lif_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_nof_php_gport_lif;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_HEADERS_MODE;
    define->property.doc = NULL;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "0";
    define->property.mapping[0].val = 128;
    define->property.mapping[1].name = "1";
    define->property.mapping[1].val = 0;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_mpls_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_mpls;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_mpls_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_mpls_general_define_nof_mpls_termination_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_nof_mpls_termination_profiles_set;
    data_index = dnx_data_mpls_general_define_nof_mpls_push_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_nof_mpls_push_profiles_set;
    data_index = dnx_data_mpls_general_define_nof_bits_mpls_label;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_nof_bits_mpls_label_set;
    data_index = dnx_data_mpls_general_define_log_nof_mpls_range_elements;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_log_nof_mpls_range_elements_set;
    data_index = dnx_data_mpls_general_define_nof_mpls_range_elements;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_nof_mpls_range_elements_set;
    data_index = dnx_data_mpls_general_define_mpls_special_label_encap_handling;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_mpls_special_label_encap_handling_set;
    data_index = dnx_data_mpls_general_define_mpls_control_word_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_mpls_control_word_supported_set;
    data_index = dnx_data_mpls_general_define_mpls_second_stage_parser_handling;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_mpls_second_stage_parser_handling_set;
    data_index = dnx_data_mpls_general_define_mpls_control_word_flag_bits_lsb;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_mpls_control_word_flag_bits_lsb_set;
    data_index = dnx_data_mpls_general_define_fhei_mps_type_nof_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_fhei_mps_type_nof_bits_set;
    data_index = dnx_data_mpls_general_define_mpls_speculative_learning_handling;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_mpls_speculative_learning_handling_set;
    data_index = dnx_data_mpls_general_define_nof_php_gport_lif;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_mpls_general_nof_php_gport_lif_set;

    
    data_index = dnx_data_mpls_general_mpls_term_1_or_2_labels;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mpls_general_mpls_term_1_or_2_labels_set;
    data_index = dnx_data_mpls_general_mpls_ioam_d_flag_stamping;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mpls_general_mpls_ioam_d_flag_stamping_set;
    data_index = dnx_data_mpls_general_mpls_cbts;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mpls_general_mpls_cbts_set;
    data_index = dnx_data_mpls_general_stack_termination_indication;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mpls_general_stack_termination_indication_set;
    data_index = dnx_data_mpls_general_evpn_with_fl_cw;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_mpls_general_evpn_with_fl_cw_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE




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
j2p_a0_dnx_data_mpls_general_mpls_term_1_or_2_labels_set(
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
j2p_a0_dnx_data_mpls_general_mpls_cbts_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_mpls_cbts;
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
j2p_a0_dnx_data_mpls_general_stack_termination_indication_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int feature_index = dnx_data_mpls_general_stack_termination_indication;
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
j2p_a0_dnx_data_mpls_general_mpls_special_label_encap_handling_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_mpls_special_label_encap_handling;
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
j2p_a0_dnx_data_mpls_general_mpls_second_stage_parser_handling_set(
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
j2p_a0_dnx_data_mpls_general_fhei_mps_type_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_mpls;
    int submodule_index = dnx_data_mpls_submodule_general;
    int define_index = dnx_data_mpls_general_define_fhei_mps_type_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_mpls_attach(
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

    
    data_index = dnx_data_mpls_general_define_mpls_special_label_encap_handling;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_mpls_general_mpls_special_label_encap_handling_set;
    data_index = dnx_data_mpls_general_define_mpls_second_stage_parser_handling;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_mpls_general_mpls_second_stage_parser_handling_set;
    data_index = dnx_data_mpls_general_define_fhei_mps_type_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_mpls_general_fhei_mps_type_nof_bits_set;

    
    data_index = dnx_data_mpls_general_mpls_term_1_or_2_labels;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_mpls_general_mpls_term_1_or_2_labels_set;
    data_index = dnx_data_mpls_general_mpls_cbts;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_mpls_general_mpls_cbts_set;
    data_index = dnx_data_mpls_general_stack_termination_indication;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_mpls_general_stack_termination_indication_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


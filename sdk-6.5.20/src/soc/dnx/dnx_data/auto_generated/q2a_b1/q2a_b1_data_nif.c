

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
q2a_b1_dnx_data_nif_global_ilkn_credit_loss_handling_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_ilkn_credit_loss_handling_mode;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "How to handle ILKN credit loss issue\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "ilkn_credit_loss_handling_mode";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);
    feature->flags |= (DNXC_DATA_F_PROPERTY_INTERNAL);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
q2a_b1_dnx_data_nif_flexe_disable_l1_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_disable_l1;
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
q2a_b1_dnx_data_nif_flexe_disable_oam_over_flexe_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_disable_oam_over_flexe;
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
q2a_b1_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
q2a_b1_data_nif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_nif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_nif_submodule_global;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_global_ilkn_credit_loss_handling_mode;
    feature = &submodule->features[data_index];
    feature->set = q2a_b1_dnx_data_nif_global_ilkn_credit_loss_handling_mode_set;

    
    
    submodule_index = dnx_data_nif_submodule_flexe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_b1_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set;

    
    data_index = dnx_data_nif_flexe_disable_l1;
    feature = &submodule->features[data_index];
    feature->set = q2a_b1_dnx_data_nif_flexe_disable_l1_set;
    data_index = dnx_data_nif_flexe_disable_oam_over_flexe;
    feature = &submodule->features[data_index];
    feature->set = q2a_b1_dnx_data_nif_flexe_disable_oam_over_flexe_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE




/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MIRROR

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>







static shr_error_e
j2p_a0_dnx_data_snif_general_original_destination_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_general;
    int feature_index = dnx_data_snif_general_original_destination_is_supported;
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
j2p_a0_dnx_data_snif_ingress_nof_mirror_on_drop_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_ingress;
    int define_index = dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles;
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
j2p_a0_dnx_data_snif_egress_probability_sample_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_snif;
    int submodule_index = dnx_data_snif_submodule_egress;
    int feature_index = dnx_data_snif_egress_probability_sample;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
j2p_a0_data_snif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_snif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_snif_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_snif_general_original_destination_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_snif_general_original_destination_is_supported_set;

    
    
    submodule_index = dnx_data_snif_submodule_ingress;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_snif_ingress_define_nof_mirror_on_drop_profiles;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_snif_ingress_nof_mirror_on_drop_profiles_set;

    

    
    
    submodule_index = dnx_data_snif_submodule_egress;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_snif_egress_probability_sample;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_snif_egress_probability_sample_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


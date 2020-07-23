

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INSTRU

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_instru.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
q2a_a0_dnx_data_instru_ifa_ingress_tod_feature_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ifa;
    int define_index = dnx_data_instru_ifa_define_ingress_tod_feature;
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
q2a_a0_dnx_data_instru_ifa_max_lenght_check_for_ifa2_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ifa;
    int define_index = dnx_data_instru_ifa_define_max_lenght_check_for_ifa2;
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
q2a_a0_dnx_data_instru_eventor_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int feature_index = dnx_data_instru_eventor_is_supported;
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
q2a_a0_dnx_data_instru_eventor_is_extra_words_copy_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_eventor;
    int define_index = dnx_data_instru_eventor_define_is_extra_words_copy;
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
q2a_a0_data_instru_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_instru;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_instru_submodule_ifa;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_ifa_define_ingress_tod_feature;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_instru_ifa_ingress_tod_feature_set;
    data_index = dnx_data_instru_ifa_define_max_lenght_check_for_ifa2;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_instru_ifa_max_lenght_check_for_ifa2_set;

    

    
    
    submodule_index = dnx_data_instru_submodule_eventor;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_eventor_define_is_extra_words_copy;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_instru_eventor_is_extra_words_copy_set;

    
    data_index = dnx_data_instru_eventor_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_instru_eventor_is_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


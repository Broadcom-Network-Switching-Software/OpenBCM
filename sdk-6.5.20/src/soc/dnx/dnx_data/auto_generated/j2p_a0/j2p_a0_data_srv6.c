

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SRV6

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_srv6.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
j2p_a0_dnx_data_srv6_termination_max_nof_terminated_sids_psp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_srv6;
    int submodule_index = dnx_data_srv6_submodule_termination;
    int define_index = dnx_data_srv6_termination_define_max_nof_terminated_sids_psp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 11;

    
    define->data = 11;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_srv6_termination_max_nof_terminated_sids_usp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_srv6;
    int submodule_index = dnx_data_srv6_submodule_termination;
    int define_index = dnx_data_srv6_termination_define_max_nof_terminated_sids_usp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_srv6;
    int submodule_index = dnx_data_srv6_submodule_termination;
    int define_index = dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_srv6;
    int submodule_index = dnx_data_srv6_submodule_encapsulation;
    int define_index = dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass;
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
j2p_a0_dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_srv6;
    int submodule_index = dnx_data_srv6_submodule_encapsulation;
    int define_index = dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass;
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
j2p_a0_data_srv6_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_srv6;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_srv6_submodule_termination;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_srv6_termination_define_max_nof_terminated_sids_psp;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_srv6_termination_max_nof_terminated_sids_psp_set;
    data_index = dnx_data_srv6_termination_define_max_nof_terminated_sids_usp;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_srv6_termination_max_nof_terminated_sids_usp_set;
    data_index = dnx_data_srv6_termination_define_max_nof_terminated_sids_usp_1pass;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_srv6_termination_max_nof_terminated_sids_usp_1pass_set;

    

    
    
    submodule_index = dnx_data_srv6_submodule_encapsulation;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_srv6_encapsulation_define_max_nof_encap_sids_main_pass;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_srv6_encapsulation_max_nof_encap_sids_main_pass_set;
    data_index = dnx_data_srv6_encapsulation_define_nof_encap_sids_extended_pass;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_srv6_encapsulation_nof_encap_sids_extended_pass_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


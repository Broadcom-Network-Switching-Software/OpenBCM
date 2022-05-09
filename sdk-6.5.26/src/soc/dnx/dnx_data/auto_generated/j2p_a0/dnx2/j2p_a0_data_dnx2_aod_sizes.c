
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dnx_data/auto_generated/dnx2/dnx_data_internal_dnx2_aod_sizes.h>








static shr_error_e
j2p_a0_dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dnx2_aod_sizes;
    int submodule_index = dnx_data_dnx2_aod_sizes_submodule_AOD;
    int define_index = dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "ETPP_TRAP_CONTEXT_PORT_PROFILE", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4;

    
    define->data = 4;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dnx2_aod_sizes;
    int submodule_index = dnx_data_dnx2_aod_sizes_submodule_AOD;
    int define_index = dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "TDM_TRAP_CANDIDATE_HEADER", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 152;

    
    define->data = 152;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_dnx2_aod_sizes;
    int submodule_index = dnx_data_dnx2_aod_sizes_submodule_AOD;
    int define_index = dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG;
    SHR_FUNC_INIT_VARS(unit);

    
    if (!soc_is(unit, DNX2_DEVICE))
    {
        dnxc_data_mgmt_inaccessible_data_set(unit, "TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG", "DNX2");
    }

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10;

    
    define->data = 10;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_dnx2_aod_sizes_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_dnx2_aod_sizes;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_dnx2_aod_sizes_submodule_AOD;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_dnx2_aod_sizes_AOD_define_ETPP_TRAP_CONTEXT_PORT_PROFILE;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dnx2_aod_sizes_AOD_ETPP_TRAP_CONTEXT_PORT_PROFILE_set;
    data_index = dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER_set;
    data_index = dnx_data_dnx2_aod_sizes_AOD_define_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_dnx2_aod_sizes_AOD_TDM_TRAP_CANDIDATE_HEADER___PP_DSP_AG_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


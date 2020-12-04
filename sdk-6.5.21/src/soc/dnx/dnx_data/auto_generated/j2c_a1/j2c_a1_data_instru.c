

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








static shr_error_e
j2c_a1_dnx_data_instru_ipt_telemetry_queue_occ_check_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_telemetry_queue_occ_check;
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
j2c_a1_dnx_data_instru_ipt_telemetry_queue_occ_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_instru;
    int submodule_index = dnx_data_instru_submodule_ipt;
    int define_index = dnx_data_instru_ipt_define_telemetry_queue_occ_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfff;

    
    define->data = 0xfff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a1_data_instru_attach(
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
    
    submodule_index = dnx_data_instru_submodule_ipt;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_instru_ipt_define_telemetry_queue_occ_check;
    define = &submodule->defines[data_index];
    define->set = j2c_a1_dnx_data_instru_ipt_telemetry_queue_occ_check_set;
    data_index = dnx_data_instru_ipt_define_telemetry_queue_occ_max;
    define = &submodule->defines[data_index];
    define->set = j2c_a1_dnx_data_instru_ipt_telemetry_queue_occ_max_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


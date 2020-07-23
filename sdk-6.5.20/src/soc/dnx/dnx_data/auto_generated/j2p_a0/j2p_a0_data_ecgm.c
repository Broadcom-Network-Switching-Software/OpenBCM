

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ecgm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
j2p_a0_dnx_data_ecgm_core_resources_total_pds_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_core_resources;
    int define_index = dnx_data_ecgm_core_resources_define_total_pds;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 76000;

    
    define->data = 76000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_ecgm_core_resources_total_pds_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_core_resources;
    int define_index = dnx_data_ecgm_core_resources_define_total_pds_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 17;

    
    define->data = 17;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_ecgm_core_resources_total_dbs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_core_resources;
    int define_index = dnx_data_ecgm_core_resources_define_total_dbs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32000;

    
    define->data = 32000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_core_resources;
    int define_index = dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000000;

    
    define->data = 5000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_ecgm_port_resources_max_nof_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_port_resources;
    int define_index = dnx_data_ecgm_port_resources_define_max_nof_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2p_a0_data_ecgm_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_ecgm_submodule_core_resources;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ecgm_core_resources_define_total_pds;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ecgm_core_resources_total_pds_set;
    data_index = dnx_data_ecgm_core_resources_define_total_pds_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ecgm_core_resources_total_pds_nof_bits_set;
    data_index = dnx_data_ecgm_core_resources_define_total_dbs;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ecgm_core_resources_total_dbs_set;
    data_index = dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_set;

    

    
    
    submodule_index = dnx_data_ecgm_submodule_port_resources;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ecgm_port_resources_define_max_nof_ports;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_ecgm_port_resources_max_nof_ports_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


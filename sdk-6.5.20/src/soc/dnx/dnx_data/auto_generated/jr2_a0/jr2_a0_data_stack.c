

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STK

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_stack.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>








static shr_error_e
jr2_a0_dnx_data_stack_general_nof_tm_domains_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_nof_tm_domains_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_dest_port_base_queue_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_dest_port_base_queue_min;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xffc0;

    
    define->data = 0xffc0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_dest_port_base_queue_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_dest_port_base_queue_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xffff;

    
    define->data = 0xffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min;
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
jr2_a0_dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 256;

    
    define->data = 256;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfffffff;

    
    define->data = 0xfffffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_lag_stack_fec_resolve_entry_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min;
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
jr2_a0_dnx_data_stack_general_lag_stack_fec_resolve_entry_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max;
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
jr2_a0_dnx_data_stack_general_lag_stack_fec_resolve_entry_all_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xfffffff;

    
    define->data = 0xfffffff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_lag_domain_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_domain_min;
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
jr2_a0_dnx_data_stack_general_lag_domain_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_lag_domain_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 64;

    
    define->data = 64;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_stack_general_trunk_entry_per_tmd_bit_num_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num;
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
jr2_a0_dnx_data_stack_general_stacking_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_stack;
    int submodule_index = dnx_data_stack_submodule_general;
    int define_index = dnx_data_stack_general_define_stacking_enable;
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
jr2_a0_data_stack_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_stack;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_stack_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_stack_general_define_nof_tm_domains_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_nof_tm_domains_max_set;
    data_index = dnx_data_stack_general_define_dest_port_base_queue_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_dest_port_base_queue_min_set;
    data_index = dnx_data_stack_general_define_dest_port_base_queue_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_dest_port_base_queue_max_set;
    data_index = dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_stack_trunk_resolve_entry_min_set;
    data_index = dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_stack_trunk_resolve_entry_max_set;
    data_index = dnx_data_stack_general_define_lag_stack_trunk_resolve_entry_all;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_stack_trunk_resolve_entry_all_set;
    data_index = dnx_data_stack_general_define_lag_stack_fec_resolve_entry_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_stack_fec_resolve_entry_min_set;
    data_index = dnx_data_stack_general_define_lag_stack_fec_resolve_entry_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_stack_fec_resolve_entry_max_set;
    data_index = dnx_data_stack_general_define_lag_stack_fec_resolve_entry_all;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_stack_fec_resolve_entry_all_set;
    data_index = dnx_data_stack_general_define_lag_domain_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_domain_min_set;
    data_index = dnx_data_stack_general_define_lag_domain_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_lag_domain_max_set;
    data_index = dnx_data_stack_general_define_trunk_entry_per_tmd_bit_num;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_trunk_entry_per_tmd_bit_num_set;
    data_index = dnx_data_stack_general_define_stacking_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_stack_general_stacking_enable_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


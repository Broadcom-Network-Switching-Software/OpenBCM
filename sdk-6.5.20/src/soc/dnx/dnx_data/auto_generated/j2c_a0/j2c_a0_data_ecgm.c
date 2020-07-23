

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
#include <bcm_int/dnx/cosq/egress/ecgm.h>








static shr_error_e
j2c_a0_dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_core_resources;
    int define_index = dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4000000;

    
    define->data = 4000000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_ecgm_port_resources_max_nof_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_port_resources;
    int define_index = dnx_data_ecgm_port_resources_define_max_nof_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 160;

    
    define->data = 160;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
j2c_a0_dnx_data_ecgm_info_dropped_reason_rqp_set(
    int unit)
{
    int index_index;
    dnx_data_ecgm_info_dropped_reason_rqp_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_ecgm;
    int submodule_index = dnx_data_ecgm_submodule_info;
    int table_index = dnx_data_ecgm_info_table_dropped_reason_rqp;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 11;
    table->info_get.key_size[0] = 11;

    
    table->values[0].default_val = "NULL";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_ecgm_info_dropped_reason_rqp_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_ecgm_info_table_dropped_reason_rqp");

    
    default_data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->reason = NULL;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->reason = "Ttl DBs thld violated";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->reason = "Ttl UC DBs pool thld violated";
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->reason = "UC pkt discarded, UC FIFO is full";
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->reason = "MC HP discarded, MC FIFO is full";
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->reason = "MC LP discarded, MC FIFO is full";
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->reason = "Ttl MC DBs pool thld violated";
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->reason = "Packet-DP not eligbl for shared DB";
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->reason = "SP DBs threshold violated";
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->reason = "DP mthd: MC-TC DB thld violated";
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->reason = "SP mthd: MC-TC to SP0 DB thld vilt";
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_ecgm_info_dropped_reason_rqp_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->reason = "SP mthd: MC-TC to SP1 DB thld vilt";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2c_a0_data_ecgm_attach(
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

    
    data_index = dnx_data_ecgm_core_resources_define_max_core_bandwidth_Mbps;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ecgm_core_resources_max_core_bandwidth_Mbps_set;

    

    
    
    submodule_index = dnx_data_ecgm_submodule_port_resources;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_ecgm_port_resources_define_max_nof_ports;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_ecgm_port_resources_max_nof_ports_set;

    

    
    
    submodule_index = dnx_data_ecgm_submodule_info;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_ecgm_info_table_dropped_reason_rqp;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_ecgm_info_dropped_reason_rqp_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


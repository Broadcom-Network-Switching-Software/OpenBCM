

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_system_red.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <bcm_int/dnx/cosq/ingress/ingress_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dram.h>









static shr_error_e
j2p_a0_dnx_data_system_red_info_resource_set(
    int unit)
{
    int type_index;
    dnx_data_system_red_info_resource_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int table_index = dnx_data_system_red_info_table_resource;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_system_red_info_resource_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_system_red_info_table_resource");

    
    default_data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->max = 0;
    default_data->hw_resolution_max = 0;
    default_data->hw_resolution_nof_bits = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_PDS)->max/dnx_data_ingr_congestion.info.nof_pds_in_pdb_get(unit);
        data->hw_resolution_max = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS)->max/dnx_data_ingr_congestion.info.threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 12;
    }
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max;
        data->hw_resolution_max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->hw_resolution_max;
        data->hw_resolution_nof_bits = 13;
    }
    if (DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS, 0);
        data->max = dnx_data_dram.buffers.nof_bdbs_get(unit)-1;
        data->hw_resolution_max = dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS)->max/dnx_data_ingr_congestion.info.threshold_granularity_get(unit);
        data->hw_resolution_nof_bits = 13;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_system_red_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_system_red;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_system_red_submodule_info;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_system_red_info_table_resource;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_system_red_info_resource_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


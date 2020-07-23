

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
jr2_a0_dnx_data_system_red_config_enable_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_config;
    int define_index = dnx_data_system_red_config_define_enable;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_SYSTEM_RED_ENABLE;
    define->property.doc = 
        "\n"
        "Enable System RED mechanism.\n"
        "When enabled, a system red-Q-size will be calculated and according to output port congestion level.\n"
        "when a packet is enqueued, a system red will be performed.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_enable;
    define->property.method_str = "enable";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_system_red_info_nof_red_q_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int define_index = dnx_data_system_red_info_define_nof_red_q_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16;

    
    define->data = 16;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_system_red_info_max_red_q_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int define_index = dnx_data_system_red_info_define_max_red_q_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_system_red.info.nof_red_q_size_get(unit)-1;

    
    define->data = dnx_data_system_red.info.nof_red_q_size_get(unit)-1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_system_red_info_max_ing_aging_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int define_index = dnx_data_system_red_info_define_max_ing_aging_period;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 268;

    
    define->data = 268;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_system_red_info_max_sch_aging_period_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int define_index = dnx_data_system_red_info_define_max_sch_aging_period;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2147;

    
    define->data = 2147;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_system_red_info_sch_aging_period_factor_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int define_index = dnx_data_system_red_info_define_sch_aging_period_factor;
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
jr2_a0_dnx_data_system_red_info_max_free_res_thr_range_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int define_index = dnx_data_system_red_info_define_max_free_res_thr_range;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_system_red_info_resource_set(
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
        data->hw_resolution_nof_bits = 11;
    }
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_resource_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS, 0);
        data->max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->max;
        data->hw_resolution_max = dnx_data_ingr_congestion.info.resource_get(unit, DNX_INGRESS_CONGESTION_RESOURCE_SRAM_BUFFERS)->hw_resolution_max;
        data->hw_resolution_nof_bits = 12;
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


static shr_error_e
jr2_a0_dnx_data_system_red_info_dbal_set(
    int unit)
{
    int type_index;
    dnx_data_system_red_info_dbal_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_system_red;
    int submodule_index = dnx_data_system_red_submodule_info;
    int table_index = dnx_data_system_red_info_table_dbal;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_system_red_info_dbal_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_system_red_info_table_dbal");

    
    default_data = (dnx_data_system_red_info_dbal_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->offset = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_system_red_info_dbal_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_dbal_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS, 0);
        data->offset = 0;
    }
    if (DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_dbal_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS, 0);
        data->offset = 3*(dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS)->hw_resolution_nof_bits+1);
    }
    if (DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS < table->keys[0].size)
    {
        data = (dnx_data_system_red_info_dbal_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_SYSTEM_RED_RESOURCE_DRAM_BDBS, 0);
        data->offset = 3*(dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_PDBS)->hw_resolution_nof_bits+1)+3*(dnx_data_system_red.info.resource_get(unit, DNX_SYSTEM_RED_RESOURCE_SRAM_BUFFERS)->hw_resolution_nof_bits+1);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_system_red_attach(
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
    
    submodule_index = dnx_data_system_red_submodule_config;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_system_red_config_define_enable;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_config_enable_set;

    

    
    
    submodule_index = dnx_data_system_red_submodule_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_system_red_info_define_nof_red_q_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_info_nof_red_q_size_set;
    data_index = dnx_data_system_red_info_define_max_red_q_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_info_max_red_q_size_set;
    data_index = dnx_data_system_red_info_define_max_ing_aging_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_info_max_ing_aging_period_set;
    data_index = dnx_data_system_red_info_define_max_sch_aging_period;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_info_max_sch_aging_period_set;
    data_index = dnx_data_system_red_info_define_sch_aging_period_factor;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_info_sch_aging_period_factor_set;
    data_index = dnx_data_system_red_info_define_max_free_res_thr_range;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_system_red_info_max_free_res_thr_range_set;

    

    
    data_index = dnx_data_system_red_info_table_resource;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_system_red_info_resource_set;
    data_index = dnx_data_system_red_info_table_dbal;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_system_red_info_dbal_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


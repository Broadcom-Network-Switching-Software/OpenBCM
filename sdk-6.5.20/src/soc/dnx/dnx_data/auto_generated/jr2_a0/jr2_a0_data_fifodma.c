

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TEMPLATEMNGR

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fifodma.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dnx_fifodma.h>








static shr_error_e
jr2_a0_dnx_data_fifodma_general_nof_fifodma_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fifodma;
    int submodule_index = dnx_data_fifodma_submodule_general;
    int define_index = dnx_data_fifodma_general_define_nof_fifodma_channels;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 12;

    
    define->data = 12;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_fifodma_general_min_nof_host_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fifodma;
    int submodule_index = dnx_data_fifodma_submodule_general;
    int define_index = dnx_data_fifodma_general_define_min_nof_host_entries;
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
jr2_a0_dnx_data_fifodma_general_max_nof_host_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fifodma;
    int submodule_index = dnx_data_fifodma_submodule_general;
    int define_index = dnx_data_fifodma_general_define_max_nof_host_entries;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 16384;

    
    define->data = 16384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_fifodma_general_fifodma_map_set(
    int unit)
{
    int fifodma_source_index;
    dnx_data_fifodma_general_fifodma_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fifodma;
    int submodule_index = dnx_data_fifodma_submodule_general;
    int table_index = dnx_data_fifodma_general_table_fifodma_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fifodma.general.nof_fifodma_channels_get(unit);
    table->info_get.key_size[0] = dnx_data_fifodma.general.nof_fifodma_channels_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fifodma_general_fifodma_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fifodma_general_table_fifodma_map");

    
    default_data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fifodma_channel = -1;
    
    for (fifodma_source_index = 0; fifodma_source_index < table->keys[0].size; fifodma_source_index++)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, fifodma_source_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (soc_dnx_fifodma_src_oam_event < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_oam_event, 0);
        data->fifodma_channel = 0;
    }
    if (soc_dnx_fifodma_src_oam_status < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_oam_status, 0);
        data->fifodma_channel = 1;
    }
    if (soc_dnx_fifodma_src_olp < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_olp, 0);
        data->fifodma_channel = 2;
    }
    if (soc_dnx_fifodma_src_crps0_dma0 < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_crps0_dma0, 0);
        data->fifodma_channel = 3;
    }
    if (soc_dnx_fifodma_src_crps0_dma1 < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_crps0_dma1, 0);
        data->fifodma_channel = 4;
    }
    if (soc_dnx_fifodma_src_crps1_dma0 < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_crps1_dma0, 0);
        data->fifodma_channel = 5;
    }
    if (soc_dnx_fifodma_src_crps1_dma1 < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_crps1_dma1, 0);
        data->fifodma_channel = 6;
    }
    if (soc_dnx_fifodma_src_event_fifo < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_event_fifo, 0);
        data->fifodma_channel = 7;
    }
    if (soc_dnx_fifodma_src_8_reserved < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_8_reserved, 0);
        data->fifodma_channel = 8;
    }
    if (soc_dnx_fifodma_src_9_reserved < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_9_reserved, 0);
        data->fifodma_channel = 9;
    }
    if (soc_dnx_fifodma_src_10_reserved < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_10_reserved, 0);
        data->fifodma_channel = 10;
    }
    if (soc_dnx_fifodma_src_11_reserved < table->keys[0].size)
    {
        data = (dnx_data_fifodma_general_fifodma_map_t *) dnxc_data_mgmt_table_data_get(unit, table, soc_dnx_fifodma_src_11_reserved, 0);
        data->fifodma_channel = 11;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_fifodma_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_fifodma;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_fifodma_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fifodma_general_define_nof_fifodma_channels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fifodma_general_nof_fifodma_channels_set;
    data_index = dnx_data_fifodma_general_define_min_nof_host_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fifodma_general_min_nof_host_entries_set;
    data_index = dnx_data_fifodma_general_define_max_nof_host_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_fifodma_general_max_nof_host_entries_set;

    

    
    data_index = dnx_data_fifodma_general_table_fifodma_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_fifodma_general_fifodma_map_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


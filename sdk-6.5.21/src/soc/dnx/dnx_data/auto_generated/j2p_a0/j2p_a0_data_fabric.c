

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_fabric.h>







static shr_error_e
j2p_a0_dnx_data_fabric_general_blocks_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_blocks_exist;
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
j2p_a0_dnx_data_fabric_general_clock_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_clock_power_down;
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
j2p_a0_dnx_data_fabric_general_is_jr1_in_system_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_is_jr1_in_system_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_general_bw_update_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_bw_update;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_general_clk_repeater_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_clk_repeater_disable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_general_is_symb_errors_per_frame_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_is_symb_errors_per_frame_supported;
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
j2p_a0_dnx_data_fabric_general_nof_lcplls_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int define_index = dnx_data_fabric_general_define_nof_lcplls;
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
j2p_a0_dnx_data_fabric_general_fmac_clock_khz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int define_index = dnx_data_fabric_general_define_fmac_clock_khz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 906250;

    
    define->data = 906250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_fabric_general_pll_phys_set(
    int unit)
{
    int lcpll_index;
    dnx_data_fabric_general_pll_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int table_index = dnx_data_fabric_general_table_pll_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_general_pll_phys_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_general_table_pll_phys");

    
    default_data = (dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min_phy_id = -1;
    default_data->max_phy_id = -1;
    
    for (lcpll_index = 0; lcpll_index < table->keys[0].size; lcpll_index++)
    {
        data = (dnx_data_fabric_general_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, lcpll_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_general_pm_properties_set(
    int unit)
{
    int pm_index_index;
    dnx_data_fabric_general_pm_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int table_index = dnx_data_fabric_general_table_pm_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_pms_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_pms_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_general_pm_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_general_table_pm_properties");

    
    default_data = (dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->special_pll_check = 0;
    default_data->nof_plls = 1;
    
    for (pm_index_index = 0; pm_index_index < table->keys[0].size; pm_index_index++)
    {
        data = (dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int feature_index = dnx_data_fabric_links_load_balancing_periodic_event_enabled;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_links_load_balancing_context_0_reserved_for_cpu_cells_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int feature_index = dnx_data_fabric_links_load_balancing_context_0_reserved_for_cpu_cells;
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
j2p_a0_dnx_data_fabric_links_is_mac_groups_load_balancing_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int feature_index = dnx_data_fabric_links_is_mac_groups_load_balancing_supported;
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
j2p_a0_dnx_data_fabric_links_nof_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_nof_links;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 192;

    
    define->data = 192;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_links_kr_fec_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_kr_fec_supported;
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
j2p_a0_dnx_data_fabric_links_general_set(
    int unit)
{
    dnx_data_fabric_links_general_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int table_index = dnx_data_fabric_links_table_general;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_links_general_t, (1 + 1  ), "data of dnx_data_fabric_links_table_general");

    
    default_data = (dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 6 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_links, 0, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 1, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 2, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 3, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 4, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 5, 0xffffffff);
    
    data = (dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_fabric_blocks_nof_instances_fdtl_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_instances_fdtl;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_blocks_nof_sbus_chains_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_sbus_chains;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set(
    int unit)
{
    int fsrd_id_index;
    dnx_data_fabric_blocks_fsrd_sbus_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int table_index = dnx_data_fabric_blocks_table_fsrd_sbus_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_blocks_fsrd_sbus_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_blocks_table_fsrd_sbus_chain");

    
    default_data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->chain = -1;
    default_data->index_in_chain = -1;
    
    for (fsrd_id_index = 0; fsrd_id_index < table->keys[0].size; fsrd_id_index++)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, fsrd_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->chain = 0;
        data->index_in_chain = 10;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain = 0;
        data->index_in_chain = 11;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain = 0;
        data->index_in_chain = 8;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain = 0;
        data->index_in_chain = 9;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain = 0;
        data->index_in_chain = 6;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->chain = 0;
        data->index_in_chain = 7;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->chain = 0;
        data->index_in_chain = 4;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->chain = 0;
        data->index_in_chain = 5;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->chain = 0;
        data->index_in_chain = 2;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->chain = 0;
        data->index_in_chain = 3;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->chain = 0;
        data->index_in_chain = 0;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->chain = 0;
        data->index_in_chain = 1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->chain = 1;
        data->index_in_chain = 10;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->chain = 1;
        data->index_in_chain = 11;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->chain = 1;
        data->index_in_chain = 8;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->chain = 1;
        data->index_in_chain = 9;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->chain = 1;
        data->index_in_chain = 6;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->chain = 1;
        data->index_in_chain = 7;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->chain = 1;
        data->index_in_chain = 4;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->chain = 1;
        data->index_in_chain = 5;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->chain = 1;
        data->index_in_chain = 2;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->chain = 1;
        data->index_in_chain = 3;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->chain = 1;
        data->index_in_chain = 0;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->chain = 1;
        data->index_in_chain = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set(
    int unit)
{
    int fmac_id_index;
    dnx_data_fabric_blocks_fmac_sbus_chain_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int table_index = dnx_data_fabric_blocks_table_fmac_sbus_chain;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fmac_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_blocks_fmac_sbus_chain_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_blocks_table_fmac_sbus_chain");

    
    default_data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->chain = -1;
    default_data->index_in_chain = -1;
    
    for (fmac_id_index = 0; fmac_id_index < table->keys[0].size; fmac_id_index++)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, fmac_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->chain = 0;
        data->index_in_chain = 21;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain = 0;
        data->index_in_chain = 20;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain = 0;
        data->index_in_chain = 23;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain = 0;
        data->index_in_chain = 22;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain = 0;
        data->index_in_chain = 16;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->chain = 0;
        data->index_in_chain = 17;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->chain = 0;
        data->index_in_chain = 18;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->chain = 0;
        data->index_in_chain = 19;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->chain = 0;
        data->index_in_chain = 12;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->chain = 0;
        data->index_in_chain = 13;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->chain = 0;
        data->index_in_chain = 14;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->chain = 0;
        data->index_in_chain = 15;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->chain = 0;
        data->index_in_chain = 9;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->chain = 0;
        data->index_in_chain = 8;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->chain = 0;
        data->index_in_chain = 11;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->chain = 0;
        data->index_in_chain = 10;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->chain = 0;
        data->index_in_chain = 5;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->chain = 0;
        data->index_in_chain = 4;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->chain = 0;
        data->index_in_chain = 7;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->chain = 0;
        data->index_in_chain = 6;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->chain = 0;
        data->index_in_chain = 1;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->chain = 0;
        data->index_in_chain = 0;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->chain = 0;
        data->index_in_chain = 3;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->chain = 0;
        data->index_in_chain = 2;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->chain = 1;
        data->index_in_chain = 21;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->chain = 1;
        data->index_in_chain = 20;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->chain = 1;
        data->index_in_chain = 23;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->chain = 1;
        data->index_in_chain = 22;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->chain = 1;
        data->index_in_chain = 16;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->chain = 1;
        data->index_in_chain = 17;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->chain = 1;
        data->index_in_chain = 18;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->chain = 1;
        data->index_in_chain = 19;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->chain = 1;
        data->index_in_chain = 12;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->chain = 1;
        data->index_in_chain = 13;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->chain = 1;
        data->index_in_chain = 14;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->chain = 1;
        data->index_in_chain = 15;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->chain = 1;
        data->index_in_chain = 9;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->chain = 1;
        data->index_in_chain = 8;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->chain = 1;
        data->index_in_chain = 11;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->chain = 1;
        data->index_in_chain = 10;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->chain = 1;
        data->index_in_chain = 5;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->chain = 1;
        data->index_in_chain = 4;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->chain = 1;
        data->index_in_chain = 7;
    }
    if (43 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 43, 0);
        data->chain = 1;
        data->index_in_chain = 6;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->chain = 1;
        data->index_in_chain = 1;
    }
    if (45 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 45, 0);
        data->chain = 1;
        data->index_in_chain = 0;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->chain = 1;
        data->index_in_chain = 3;
    }
    if (47 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 47, 0);
        data->chain = 1;
        data->index_in_chain = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set(
    int unit)
{
    int fsrd_index_index;
    dnx_data_fabric_blocks_fsrd_ref_clk_master_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int table_index = dnx_data_fabric_blocks_table_fsrd_ref_clk_master;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_blocks_fsrd_ref_clk_master_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_blocks_table_fsrd_ref_clk_master");

    
    default_data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->master_fsrd_index = -1;
    
    for (fsrd_index_index = 0; fsrd_index_index < table->keys[0].size; fsrd_index_index++)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, fsrd_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->master_fsrd_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->master_fsrd_index = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->master_fsrd_index = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->master_fsrd_index = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->master_fsrd_index = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->master_fsrd_index = 0;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->master_fsrd_index = 10;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->master_fsrd_index = 10;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->master_fsrd_index = 10;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->master_fsrd_index = 10;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->master_fsrd_index = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->master_fsrd_index = 10;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->master_fsrd_index = 12;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->master_fsrd_index = 12;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->master_fsrd_index = 12;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->master_fsrd_index = 12;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->master_fsrd_index = 12;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->master_fsrd_index = 12;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->master_fsrd_index = 22;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->master_fsrd_index = 22;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->master_fsrd_index = 22;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->master_fsrd_index = 22;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->master_fsrd_index = 22;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->master_fsrd_index = 22;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2p_a0_dnx_data_fabric_cell_use_max_fda_reassembly_timer_value_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int feature_index = dnx_data_fabric_cell_use_max_fda_reassembly_timer_value;
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
j2p_a0_dnx_data_fabric_cell_pcp_require_continous_on_local_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int feature_index = dnx_data_fabric_cell_pcp_require_continous_on_local;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_fabric_cell_sr_cell_nof_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_sr_cell_nof_instances;
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
j2p_a0_dnx_data_fabric_cell_ctrl_cells_init_fifo_empty_iterations_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_ctrl_cells_init_fifo_empty_iterations;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 36;

    
    define->data = 36;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cell_default_packing_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_default_packing_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING;

    
    define->data = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cell_pcp_force_no_packing_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_pcp_force_no_packing;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Indicates that device should use pcp mode in 'no packing' mode\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "pcp_force_no_packing";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_fabric_pipes_local_route_pipe_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int feature_index = dnx_data_fabric_pipes_local_route_pipe_supported;
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
j2p_a0_dnx_data_fabric_pipes_max_nof_pipes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_max_nof_pipes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2;

    
    define->data = 2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_fabric_mesh_topology_calendar_mode_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_topology;
    int feature_index = dnx_data_fabric_mesh_topology_calendar_mode_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_fabric_mesh_topology_mesh_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_topology;
    int define_index = dnx_data_fabric_mesh_topology_define_mesh_init;
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
j2p_a0_dnx_data_fabric_mesh_topology_mesh_config_1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_topology;
    int define_index = dnx_data_fabric_mesh_topology_define_mesh_config_1;
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
j2p_a0_dnx_data_fabric_cgm_egress_drop_low_prio_mc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_egress_drop_low_prio_mc;
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
j2p_a0_dnx_data_fabric_cgm_is_static_shaper_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_static_shaper_supported;
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
j2p_a0_dnx_data_fabric_cgm_is_adaptive_tfl_shaper_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_adaptive_tfl_shaper_supported;
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
j2p_a0_dnx_data_fabric_cgm_is_adaptive_mnol_shaper_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_adaptive_mnol_shaper_supported;
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
j2p_a0_dnx_data_fabric_cgm_is_adaptive_shaper_watermarks_limited_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_adaptive_shaper_watermarks_limited;
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
j2p_a0_dnx_data_fabric_cgm_static_shaper_fix_factor_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_static_shaper_fix_factor;
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
j2p_a0_dnx_data_fabric_cgm_is_global_cell_shaper_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_global_cell_shaper_supported;
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
j2p_a0_dnx_data_fabric_cgm_is_local_unicast_dynamic_wfq_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_local_unicast_dynamic_wfq_supported;
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
j2p_a0_dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32*64/66;

    
    define->data = 32*64/66;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cgm_cgm_adaptive_tfl_shaper_zone_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_cgm_adaptive_tfl_shaper_zone_nof;
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
j2p_a0_dnx_data_fabric_cgm_cgm_global_cell_shaper_max_rate_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_cgm_global_cell_shaper_max_rate;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 100000;

    
    define->data = 100000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cgm_rci_link_level_factor_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_rci_link_level_factor_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6;

    
    define->data = 6;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_max_threshold_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_max_threshold_value;
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
j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_high_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 120;

    
    define->data = 120;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_low_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_weight_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_high_weight;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 70;

    
    define->data = 70;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_weight_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_low_weight;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 30;

    
    define->data = 30;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2p_a0_dnx_data_fabric_cgm_access_map_set(
    int unit)
{
    int control_type_index;
    dnx_data_fabric_cgm_access_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int table_index = dnx_data_fabric_cgm_table_access_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = bcmFabricCgmNof;
    table->info_get.key_size[0] = bcmFabricCgmNof;

    
    table->values[0].default_val = "DBAL_NOF_TABLES";
    table->values[1].default_val = "DBAL_FIELD_EMPTY";
    table->values[2].default_val = "DBAL_FIELD_EMPTY";
    table->values[3].default_val = "DBAL_FIELD_EMPTY";
    table->values[4].default_val = "DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "Unknown control_type";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_cgm_access_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_cgm_table_access_map");

    
    default_data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dbal_table = DBAL_NOF_TABLES;
    default_data->dbal_key1 = DBAL_FIELD_EMPTY;
    default_data->dbal_key2 = DBAL_FIELD_EMPTY;
    default_data->dbal_result = DBAL_FIELD_EMPTY;
    default_data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
    default_data->user_range = 0;
    default_data->user_min = 0;
    default_data->user_max = 0;
    default_data->is_valid = 0;
    default_data->err_msg = "Unknown control_type";
    
    for (control_type_index = 0; control_type_index < table->keys[0].size; control_type_index++)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, control_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmFabricCgmRciLinkPipeLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLinkPipeLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE;
        data->dbal_key2 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LINK_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciEgressPipeLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciEgressPipeLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE;
        data->dbal_key2 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_EGRESS_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciTotalEgressPipeLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciTotalEgressPipeLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE;
        data->dbal_key2 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_TOTAL_EGRESS_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciLocalLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLocalLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_LOCAL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LOCAL_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciTotalLocalLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciTotalLocalLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_LOCAL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_TOTAL_LOCAL_RCI_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciLinkLevelFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLinkLevelFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_MAPPING;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LINK_RCI_FACTOR;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciEgressLevelFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciEgressLevelFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_MAPPING;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_EGRESS_RCI_SCORE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciHighSeverityMinLinksParam < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciHighSeverityMinLinksParam, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_HIGH_SEVERITY;
        data->dbal_result = DBAL_FIELD_MIN_NOF_LINKS;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciHighSeverityFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciHighSeverityFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_HIGH_SEVERITY;
        data->dbal_result = DBAL_FIELD_SCORE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciCoreLevelMappingTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciCoreLevelMappingTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_TO_LEVEL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_CORE_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmRciDeviceLevelMappingTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciDeviceLevelMappingTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_SCORE_TO_LEVEL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_DEVICE_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciLeakyBucketEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciLeakyBucketEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_LEAKY_BUCKET_FC_ENABLE;
        data->dbal_result = DBAL_FIELD_LEAKY_BUCKET_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_LEAKY_BUCKET_CONFIG;
        data->dbal_key1 = DBAL_FIELD_LEAKY_BUCKET_ID;
        data->dbal_result = DBAL_FIELD_MAX_VALUE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEAKY_BUCKET;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciLeakyBucketFCLeakyBucketTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_LEAKY_BUCKET_CONFIG;
        data->dbal_key1 = DBAL_FIELD_LEAKY_BUCKET_ID;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEAKY_BUCKET;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciBackoffEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciBackoffEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_FC_ENABLE;
        data->dbal_result = DBAL_FIELD_BACKOFF_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGciBackoffLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGciBackoffLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GCI_BACKOFF_CONGESTION_SEVERITY_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL_SEPARATE;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_LEVEL_SEPARATE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmSlowStartEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmSlowStartEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_SLOW_START_ENABLE;
        data->dbal_key1 = DBAL_FIELD_FMC_SHAPER_ID;
        data->dbal_result = DBAL_FIELD_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SHAPER;
        data->is_valid = 1;
    }
    if (bcmFabricCgmSlowStartRateShaperPhaseParam < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmSlowStartRateShaperPhaseParam, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_SLOW_START_CONFIG;
        data->dbal_key1 = DBAL_FIELD_FMC_SHAPER_ID;
        data->dbal_key2 = DBAL_FIELD_PHASE;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_FMC_SLOW_START_PHASE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmLlfcPipeTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmLlfcPipeTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_LLFC_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_PIPE_ID;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PIPE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmDropFabricClassTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmDropFabricClassTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_FIFO_DROP_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_CAST;
        data->dbal_key2 = DBAL_FIELD_FABRIC_PRIORITY;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_CAST_PRIORITY;
        data->is_valid = 1;
    }
    if (bcmFabricCgmDropMeshMcPriorityTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmDropMeshMcPriorityTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_MESH_MC_FIFO_DROP_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_FABRIC_PRIORITY;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_PRIORITY;
        data->is_valid = 1;
    }
    if (bcmFabricCgmEgressDropMcLowPrioEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDropMcLowPrioEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_LOW_PRIO_MC;
        data->dbal_result = DBAL_FIELD_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmEgressDropMcLowPrioTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDropMcLowPrioTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_LOW_PRIO_MC;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmEgressDropMcLowPrioSelect < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDropMcLowPrioSelect, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_LOW_PRIO_MC;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmFcIngressTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmFcIngressTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_FC_INGRESS_THRESHOLDS;
        data->dbal_result = DBAL_FIELD_LINK_FIFO_SIZE_THRESHOLD;
        data->is_valid = 1;
    }
    if (bcmFabricCgmEgressDynamicWfqTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDynamicWfqTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DYNAMIC_WFQ;
        data->dbal_result = DBAL_FIELD_THRESHOLD;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmLciShaperMode < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmLciShaperMode, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 0;
        data->user_max = 1;
        data->is_valid = 1;
    }
    if (bcmFabricCgmLciShaperStaticRateInGbps < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmLciShaperStaticRateInGbps, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_STATIC_SHAPER;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolEnable < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolEnable, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 0;
        data->user_max = 1;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolMaxFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolMaxFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 2048;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolMinFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolMinFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 2048;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolFifoThreshold < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolFifoThreshold, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 0;
        data->user_max = 1023;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolNofLinksThreshold < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolNofLinksThreshold, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_MNOL;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolDecDelta < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolDecDelta, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = -1024;
        data->user_max = 1023;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolIncDelta < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolIncDelta, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = -1024;
        data->user_max = 1023;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolFastReleaseEnable < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolFastReleaseEnable, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_FAST_RELEASE_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveMnolFastReleaseValue < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveMnolFastReleaseValue, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_MNOL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 2048;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillEnable < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillEnable, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 0;
        data->user_max = 1;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillMaxFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillMaxFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 2048;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillMinFactor < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillMinFactor, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 2048;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillFastReleaseEnable < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillFastReleaseEnable, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_CONTROL;
        data->dbal_result = DBAL_FIELD_FAST_RELEASE_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillThreshold < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillThreshold, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_ZONE;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_ZONE;
        data->user_range = 1;
        data->user_min = 0;
        data->user_max = 1023;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillFastReleaseValue < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillFastReleaseValue, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_FAST_RELEASE_VALUES;
        data->dbal_key1 = DBAL_FIELD_ZONE;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_ZONE;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 2048;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillIncDelta < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillIncDelta, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_INC_DELTA;
        data->dbal_key1 = DBAL_FIELD_ZONE;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_ZONE;
        data->user_range = 1;
        data->user_min = -1024;
        data->user_max = 1023;
        data->is_valid = 1;
    }
    if (bcmFabricCgmAdaptiveTotalFillDecDelta < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmAdaptiveTotalFillDecDelta, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_ADAPTIVE_SHAPER_TFL_DEC_DELTA;
        data->dbal_key1 = DBAL_FIELD_ZONE;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_ZONE;
        data->user_range = 1;
        data->user_min = -1024;
        data->user_max = 1023;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGlobalCellShaperEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGlobalCellShaperEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GLOBAL_CELL_SHAPER_CONTROL;
        data->dbal_result = DBAL_FIELD_ENABLE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGlobalCellShaperRate < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGlobalCellShaperRate, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GLOBAL_CELL_SHAPER_CONTROL;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->user_range = 1;
        data->user_min = 1;
        data->user_max = 100000;
        data->is_valid = 1;
    }
    if (bcmFabricCgmGlobalCellShaperCellSizeMode < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmGlobalCellShaperCellSizeMode, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_GLOBAL_CELL_SHAPER_CONTROL;
        data->dbal_result = DBAL_FIELD_CELL_MODE;
        data->is_valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2p_a0_dnx_data_fabric_dtqs_max_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dtqs;
    int define_index = dnx_data_fabric_dtqs_define_max_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1536;

    
    define->data = 1536;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2p_a0_dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value;
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
j2p_a0_dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_adaptive_shaper_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_adaptive_shaper_threshold_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_adaptive_shaper_factor_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_adaptive_shaper_factor_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_adaptive_shaper_mnol_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_adaptive_shaper_mnol_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 32;

    
    define->data = 32;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 65535;

    
    define->data = 65535;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_time_frame_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_global_cell_shaper_time_frame_bits_nof;
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
j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_time_frame_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_global_cell_shaper_time_frame_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2047;

    
    define->data = 2047;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_cnt_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_global_cell_shaper_cnt_bits_nof;
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
j2p_a0_dnx_data_fabric_transmit_eir_fc_leaky_backet_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int feature_index = dnx_data_fabric_transmit_eir_fc_leaky_backet;
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
j2p_a0_dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_set(
    int unit)
{
    int dqcq_priority_index;
    dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int table_index = dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_iqs.dqcq.nof_priorities_get(unit);
    table->info_get.key_size[0] = dnx_data_iqs.dqcq.nof_priorities_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select");

    
    default_data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->sram = 0;
    default_data->mixs = 0;
    default_data->mixd = 0;
    default_data->mix_mixs = 0;
    default_data->mix_mixd = 0;
    default_data->s2d = 0;
    
    for (dqcq_priority_index = 0; dqcq_priority_index < table->keys[0].size; dqcq_priority_index++)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, dqcq_priority_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->sram = 12;
        data->mixs = 12;
        data->mixd = 12;
        data->mix_mixs = 12;
        data->mix_mixd = 12;
        data->s2d = 12;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->sram = 11;
        data->mixs = 11;
        data->mixd = 11;
        data->mix_mixs = 11;
        data->mix_mixd = 11;
        data->s2d = 11;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->sram = 10;
        data->mixs = 10;
        data->mixd = 10;
        data->mix_mixs = 10;
        data->mix_mixd = 10;
        data->s2d = 10;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->sram = 9;
        data->mixs = 9;
        data->mixd = 9;
        data->mix_mixs = 9;
        data->mix_mixd = 9;
        data->s2d = 9;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->sram = 8;
        data->mixs = 8;
        data->mixd = 8;
        data->mix_mixs = 8;
        data->mix_mixd = 8;
        data->s2d = 8;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->sram = 7;
        data->mixs = 7;
        data->mixd = 7;
        data->mix_mixs = 7;
        data->mix_mixd = 7;
        data->s2d = 7;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->sram = 6;
        data->mixs = 6;
        data->mixd = 6;
        data->mix_mixs = 6;
        data->mix_mixd = 6;
        data->s2d = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->sram = 5;
        data->mixs = 5;
        data->mixd = 5;
        data->mix_mixs = 5;
        data->mix_mixd = 5;
        data->s2d = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_set(
    int unit)
{
    dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int table_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "3000";
    table->values[1].default_val = "10";
    table->values[2].default_val = "200";
    table->values[3].default_val = "5";
    table->values[4].default_val = "99";
    table->values[5].default_val = "0xa";
    table->values[6].default_val = "0x1";
    table->values[7].default_val = "0x3fff";
    table->values[8].default_val = "0x3ff6";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_transmit_eir_fc_leaky_bucket_t, (1 + 1  ), "data of dnx_data_fabric_transmit_table_eir_fc_leaky_bucket");

    
    default_data = (dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->almost_full_th = 3000;
    default_data->almost_empty_th = 10;
    default_data->bucket_size = 200;
    default_data->bucket_fc_threshold = 5;
    default_data->period = 99;
    default_data->tokens_delta0 = 0xa;
    default_data->tokens_delta1 = 0x1;
    default_data->tokens_delta2 = 0x3fff;
    default_data->tokens_delta3 = 0x3ff6;
    
    data = (dnx_data_fabric_transmit_eir_fc_leaky_bucket_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2p_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_set(
    int unit)
{
    dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int table_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "2";
    table->values[1].default_val = "1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "2";
    table->values[4].default_val = "2";
    table->values[5].default_val = "3";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t, (1 + 1  ), "data of dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select");

    
    default_data = (dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->fifo_in_between = 2;
    default_data->fifo_partially_almost_full = 1;
    default_data->fifo_almost_full = 0;
    default_data->fifo_partially_almost_empty = 2;
    default_data->fifo_not_stable = 2;
    default_data->fifo_almost_empty = 3;
    
    data = (dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2p_a0_data_fabric_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_fabric;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_fabric_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_general_define_nof_lcplls;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_general_nof_lcplls_set;
    data_index = dnx_data_fabric_general_define_fmac_clock_khz;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_general_fmac_clock_khz_set;

    
    data_index = dnx_data_fabric_general_blocks_exist;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_general_blocks_exist_set;
    data_index = dnx_data_fabric_general_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_general_clock_power_down_set;
    data_index = dnx_data_fabric_general_is_jr1_in_system_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_general_is_jr1_in_system_supported_set;
    data_index = dnx_data_fabric_general_bw_update;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_general_bw_update_set;
    data_index = dnx_data_fabric_general_clk_repeater_disable;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_general_clk_repeater_disable_set;
    data_index = dnx_data_fabric_general_is_symb_errors_per_frame_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_general_is_symb_errors_per_frame_supported_set;

    
    data_index = dnx_data_fabric_general_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_general_pll_phys_set;
    data_index = dnx_data_fabric_general_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_general_pm_properties_set;
    
    submodule_index = dnx_data_fabric_submodule_links;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_links_define_nof_links;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_links_nof_links_set;
    data_index = dnx_data_fabric_links_define_kr_fec_supported;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_links_kr_fec_supported_set;

    
    data_index = dnx_data_fabric_links_load_balancing_periodic_event_enabled;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set;
    data_index = dnx_data_fabric_links_load_balancing_context_0_reserved_for_cpu_cells;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_links_load_balancing_context_0_reserved_for_cpu_cells_set;
    data_index = dnx_data_fabric_links_is_mac_groups_load_balancing_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_links_is_mac_groups_load_balancing_supported_set;

    
    data_index = dnx_data_fabric_links_table_general;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_links_general_set;
    
    submodule_index = dnx_data_fabric_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_blocks_define_nof_instances_fdtl;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_blocks_nof_instances_fdtl_set;
    data_index = dnx_data_fabric_blocks_define_nof_sbus_chains;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_blocks_nof_sbus_chains_set;

    

    
    data_index = dnx_data_fabric_blocks_table_fsrd_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fmac_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fsrd_ref_clk_master;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set;
    
    submodule_index = dnx_data_fabric_submodule_cell;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_cell_define_sr_cell_nof_instances;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cell_sr_cell_nof_instances_set;
    data_index = dnx_data_fabric_cell_define_ctrl_cells_init_fifo_empty_iterations;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cell_ctrl_cells_init_fifo_empty_iterations_set;
    data_index = dnx_data_fabric_cell_define_default_packing_mode;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cell_default_packing_mode_set;
    data_index = dnx_data_fabric_cell_define_pcp_force_no_packing;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cell_pcp_force_no_packing_set;

    
    data_index = dnx_data_fabric_cell_use_max_fda_reassembly_timer_value;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cell_use_max_fda_reassembly_timer_value_set;
    data_index = dnx_data_fabric_cell_pcp_require_continous_on_local;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cell_pcp_require_continous_on_local_set;

    
    
    submodule_index = dnx_data_fabric_submodule_pipes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_pipes_define_max_nof_pipes;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_pipes_max_nof_pipes_set;

    
    data_index = dnx_data_fabric_pipes_local_route_pipe_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_pipes_local_route_pipe_supported_set;

    
    
    submodule_index = dnx_data_fabric_submodule_mesh_topology;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_mesh_topology_define_mesh_init;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_mesh_topology_mesh_init_set;
    data_index = dnx_data_fabric_mesh_topology_define_mesh_config_1;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_mesh_topology_mesh_config_1_set;

    
    data_index = dnx_data_fabric_mesh_topology_calendar_mode_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_mesh_topology_calendar_mode_supported_set;

    
    
    submodule_index = dnx_data_fabric_submodule_cgm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_set;
    data_index = dnx_data_fabric_cgm_define_cgm_adaptive_tfl_shaper_zone_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_cgm_adaptive_tfl_shaper_zone_nof_set;
    data_index = dnx_data_fabric_cgm_define_cgm_global_cell_shaper_max_rate;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_cgm_global_cell_shaper_max_rate_set;
    data_index = dnx_data_fabric_cgm_define_rci_link_level_factor_max_value;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_rci_link_level_factor_max_value_set;
    data_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_max_threshold_value;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_max_threshold_value_set;
    data_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_high_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_threshold_set;
    data_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_low_threshold;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_threshold_set;
    data_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_high_weight;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_high_weight_set;
    data_index = dnx_data_fabric_cgm_define_local_unicast_dynamic_wfq_low_weight;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_cgm_local_unicast_dynamic_wfq_low_weight_set;

    
    data_index = dnx_data_fabric_cgm_egress_drop_low_prio_mc;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_egress_drop_low_prio_mc_set;
    data_index = dnx_data_fabric_cgm_is_static_shaper_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_is_static_shaper_supported_set;
    data_index = dnx_data_fabric_cgm_is_adaptive_tfl_shaper_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_is_adaptive_tfl_shaper_supported_set;
    data_index = dnx_data_fabric_cgm_is_adaptive_mnol_shaper_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_is_adaptive_mnol_shaper_supported_set;
    data_index = dnx_data_fabric_cgm_is_adaptive_shaper_watermarks_limited;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_is_adaptive_shaper_watermarks_limited_set;
    data_index = dnx_data_fabric_cgm_static_shaper_fix_factor;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_static_shaper_fix_factor_set;
    data_index = dnx_data_fabric_cgm_is_global_cell_shaper_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_is_global_cell_shaper_supported_set;
    data_index = dnx_data_fabric_cgm_is_local_unicast_dynamic_wfq_supported;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_cgm_is_local_unicast_dynamic_wfq_supported_set;

    
    data_index = dnx_data_fabric_cgm_table_access_map;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_cgm_access_map_set;
    
    submodule_index = dnx_data_fabric_submodule_dtqs;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_dtqs_define_max_size;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dtqs_max_size_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_mesh_topology_init_config_2_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_mesh_topology_init_config_2_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_adaptive_shaper_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_adaptive_shaper_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_adaptive_shaper_factor_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_adaptive_shaper_factor_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_adaptive_shaper_mnol_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_adaptive_shaper_mnol_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_granularity;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_granularity_set;
    data_index = dnx_data_fabric_dbal_define_global_cell_shaper_allowed_cells_max_value;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_allowed_cells_max_value_set;
    data_index = dnx_data_fabric_dbal_define_global_cell_shaper_time_frame_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_time_frame_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_global_cell_shaper_time_frame_max_value;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_time_frame_max_value_set;
    data_index = dnx_data_fabric_dbal_define_global_cell_shaper_cnt_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2p_a0_dnx_data_fabric_dbal_global_cell_shaper_cnt_bits_nof_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_transmit;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_fabric_transmit_eir_fc_leaky_backet;
    feature = &submodule->features[data_index];
    feature->set = j2p_a0_dnx_data_fabric_transmit_eir_fc_leaky_backet_set;

    
    data_index = dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_set;
    data_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_set;
    data_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select;
    table = &submodule->tables[data_index];
    table->set = j2p_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


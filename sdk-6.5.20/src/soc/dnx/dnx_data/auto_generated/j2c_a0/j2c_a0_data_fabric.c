

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>







static shr_error_e
j2c_a0_dnx_data_fabric_general_clock_power_down_set(
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
j2c_a0_dnx_data_fabric_general_nof_lcplls_set(
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
j2c_a0_dnx_data_fabric_general_fmac_clock_khz_set(
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
j2c_a0_dnx_data_fabric_general_pll_phys_set(
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
j2c_a0_dnx_data_fabric_general_pm_properties_set(
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
    table->values[1].default_val = "2";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_general_pm_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_general_table_pm_properties");

    
    default_data = (dnx_data_fabric_general_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->special_pll_check = 0;
    default_data->nof_plls = 2;
    
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
j2c_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set(
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
j2c_a0_dnx_data_fabric_links_nof_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_nof_links;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 48;

    
    define->data = 48;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_fabric_links_general_set(
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

    
    table->values[0].default_val = "0xffffffff, 0xffff";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_links_general_t, (1 + 1  ), "data of dnx_data_fabric_links_table_general");

    
    default_data = (dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 2 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_links, 0, 0xffffffff);
    _SHR_PBMP_WORD_SET(default_data->supported_links, 1, 0xffff);
    
    data = (dnx_data_fabric_links_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_fabric_blocks_nof_sbus_chains_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_sbus_chains;
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
j2c_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set(
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
        data->index_in_chain = 5;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain = 0;
        data->index_in_chain = 4;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain = 0;
        data->index_in_chain = 3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain = 0;
        data->index_in_chain = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain = 0;
        data->index_in_chain = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->chain = 0;
        data->index_in_chain = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set(
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
        data->index_in_chain = 11;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain = 0;
        data->index_in_chain = 10;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain = 0;
        data->index_in_chain = 9;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain = 0;
        data->index_in_chain = 8;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain = 0;
        data->index_in_chain = 7;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->chain = 0;
        data->index_in_chain = 6;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->chain = 0;
        data->index_in_chain = 5;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->chain = 0;
        data->index_in_chain = 4;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->chain = 0;
        data->index_in_chain = 3;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->chain = 0;
        data->index_in_chain = 2;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->chain = 0;
        data->index_in_chain = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fmac_sbus_chain_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->chain = 0;
        data->index_in_chain = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set(
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
        data->master_fsrd_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->master_fsrd_index = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->master_fsrd_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_blocks_fsrd_ref_clk_master_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->master_fsrd_index = 4;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_fabric_cell_sr_cell_nof_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cell;
    int define_index = dnx_data_fabric_cell_define_sr_cell_nof_instances;
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
j2c_a0_dnx_data_fabric_pipes_ocb_wfq_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int feature_index = dnx_data_fabric_pipes_ocb_wfq;
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
j2c_a0_dnx_data_fabric_pipes_max_nof_subcontexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int define_index = dnx_data_fabric_pipes_define_max_nof_subcontexts;
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
j2c_a0_dnx_data_fabric_pipes_valid_map_config_set(
    int unit)
{
    int config_id_index;
    dnx_data_fabric_pipes_valid_map_config_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int table_index = dnx_data_fabric_pipes_table_valid_map_config;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "0, 0, 0, 0";
    table->values[2].default_val = "0, 0, 0, 0";
    table->values[3].default_val = "SINGLE PIPE";
    table->values[4].default_val = "soc_dnxc_fabric_pipe_map_single";
    table->values[5].default_val = "-1";
    table->values[6].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_pipes_valid_map_config_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_pipes_table_valid_map_config");

    
    default_data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_pipes = 1;
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->uc[0] = 0;
    default_data->uc[1] = 0;
    default_data->uc[2] = 0;
    default_data->uc[3] = 0;
    SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
    default_data->mc[0] = 0;
    default_data->mc[1] = 0;
    default_data->mc[2] = 0;
    default_data->mc[3] = 0;
    default_data->name = "SINGLE PIPE";
    default_data->type = soc_dnxc_fabric_pipe_map_single;
    default_data->min_hp_mc = -1;
    default_data->tdm_pipe = -1;
    
    for (config_id_index = 0; config_id_index < table->keys[0].size; config_id_index++)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, config_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_pipes = 1;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 0;
        data->mc[1] = 0;
        data->mc[2] = 0;
        data->mc[3] = 0;
        data->name = "SINGLE PIPE";
        data->type = soc_dnxc_fabric_pipe_map_single;
        data->tdm_pipe = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_pipes = 2;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 1;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,MC";
        data->type = soc_dnxc_fabric_pipe_map_dual_uc_mc;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 2;
        data->mc[2] = 2;
        data->mc[3] = 1;
        data->name = "UC,HP MC (3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 3;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 2;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,HP MC (2,3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 0;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 2;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 1;
        data->name = "UC,HP MC (1,2,3),LP MC";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
        data->min_hp_mc = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->nof_pipes = 2;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 1;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 0;
        data->mc[1] = 0;
        data->mc[2] = 0;
        data->mc[3] = 1;
        data->name = "Non-TDM,TDM";
        data->type = soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm;
        data->tdm_pipe = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_fabric_pipes_valid_map_config_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->nof_pipes = 3;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->uc[0] = 0;
        data->uc[1] = 0;
        data->uc[2] = 0;
        data->uc[3] = 2;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_FABRIC_CELL_NOF_PRIORITIES, _SHR_E_INTERNAL, "out of bound access to array")
        data->mc[0] = 1;
        data->mc[1] = 1;
        data->mc[2] = 1;
        data->mc[3] = 2;
        data->name = "UC,MC,TDM";
        data->type = soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm;
        data->tdm_pipe = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_pipes_dtq_subcontexts_set(
    int unit)
{
    int dtq_index;
    int is_ocb_only_index;
    dnx_data_fabric_pipes_dtq_subcontexts_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_pipes;
    int table_index = dnx_data_fabric_pipes_table_dtq_subcontexts;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_fabric.pipes.max_nof_contexts_get(unit);
    table->info_get.key_size[0] = dnx_data_fabric.pipes.max_nof_contexts_get(unit);
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_pipes_dtq_subcontexts_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_fabric_pipes_table_dtq_subcontexts");

    
    default_data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->index = 0;
    
    for (dtq_index = 0; dtq_index < table->keys[0].size; dtq_index++)
    {
        for (is_ocb_only_index = 0; is_ocb_only_index < table->keys[1].size; is_ocb_only_index++)
        {
            data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, dtq_index, is_ocb_only_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->index = 0;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->index = 1;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->index = 2;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->index = 3;
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->index = 4;
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_fabric_pipes_dtq_subcontexts_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->index = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_fabric_tdm_priority_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_tdm;
    int define_index = dnx_data_fabric_tdm_define_priority;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3;

    
    define->data = 3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_TDM_PRIORITY_MIN;
    define->property.doc = 
        "\n"
        "Mark a CGM fabric priority as TDM (should be globally configured over the system).\n"
        "'NONE' represents a system that does not support TDM and all the priorities can be used as regular priorities.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "3";
    define->property.mapping[0].val = 3;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "NONE";
    define->property.mapping[1].val = -1;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_fabric_cgm_egress_drop_low_prio_mc_set(
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
j2c_a0_dnx_data_fabric_cgm_egress_fifo_static_size_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_egress_fifo_static_size;
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
j2c_a0_dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int feature_index = dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_tdm.params.mode_get(unit)!=TDM_MODE_NONE;

    
    feature->data = dnx_data_tdm.params.mode_get(unit)!=TDM_MODE_NONE;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_cgm_is_static_shaper_supported_set(
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
j2c_a0_dnx_data_fabric_cgm_egress_drop_th_clocks_resolution_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution;
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
j2c_a0_dnx_data_fabric_cgm_egress_fifo_fc_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_cgm;
    int define_index = dnx_data_fabric_cgm_define_egress_fifo_fc_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 18;

    
    define->data = 18;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_set(
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
j2c_a0_dnx_data_fabric_cgm_access_map_set(
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
    table->values[6].default_val = "Unknown control_type";
    
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
    if (bcmFabricCgmRciLocalLevelTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmRciLocalLevelTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_RCI_LOCAL_THRESHOLDS;
        data->dbal_key1 = DBAL_FIELD_LEVEL;
        data->dbal_result = DBAL_FIELD_LOCAL_RCI_THRESHOLD;
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
    if (bcmFabricCgmEgressDropTdmLatencyEn < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDropTdmLatencyEn, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_TDM_LATENCY;
        data->dbal_result = DBAL_FIELD_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
        data->is_valid = 1;
    }
    if (bcmFabricCgmEgressDropTdmLatencyTh < table->keys[0].size)
    {
        data = (dnx_data_fabric_cgm_access_map_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmFabricCgmEgressDropTdmLatencyTh, 0);
        data->dbal_table = DBAL_TABLE_FABRIC_CGM_EGRESS_DROP_TDM_LATENCY;
        data->dbal_result = DBAL_FIELD_EMPTY;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
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
        data->dbal_result = DBAL_FIELD_ENABLE;
        data->index_type = DNX_FABRIC_CGM_CONTROL_INDEX_TYPE_NONE;
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_fabric_dbal_link_rci_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof;
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
j2c_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof;
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
j2c_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1024;

    
    define->data = 1024;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value;
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
j2c_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof;
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
j2c_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 768;

    
    define->data = 768;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value;
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
j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value;
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
j2c_a0_dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof;
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
j2c_a0_dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value;
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
j2c_a0_dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof;
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
j2c_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof;
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
j2c_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value;
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
j2c_a0_dnx_data_fabric_mesh_mc_nof_replication_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_mesh_mc;
    int define_index = dnx_data_fabric_mesh_mc_define_nof_replication;
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
j2c_a0_dnx_data_fabric_transmit_eir_fc_leaky_backet_set(
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
j2c_a0_dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_set(
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
j2c_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_set(
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
j2c_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_set(
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
j2c_a0_data_fabric_attach(
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
    define->set = j2c_a0_dnx_data_fabric_general_nof_lcplls_set;
    data_index = dnx_data_fabric_general_define_fmac_clock_khz;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_general_fmac_clock_khz_set;

    
    data_index = dnx_data_fabric_general_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_general_clock_power_down_set;

    
    data_index = dnx_data_fabric_general_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_general_pll_phys_set;
    data_index = dnx_data_fabric_general_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_general_pm_properties_set;
    
    submodule_index = dnx_data_fabric_submodule_links;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_links_define_nof_links;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_links_nof_links_set;

    
    data_index = dnx_data_fabric_links_load_balancing_periodic_event_enabled;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set;

    
    data_index = dnx_data_fabric_links_table_general;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_links_general_set;
    
    submodule_index = dnx_data_fabric_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_blocks_define_nof_sbus_chains;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_blocks_nof_sbus_chains_set;

    

    
    data_index = dnx_data_fabric_blocks_table_fsrd_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fmac_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fsrd_ref_clk_master;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set;
    
    submodule_index = dnx_data_fabric_submodule_cell;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_cell_define_sr_cell_nof_instances;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_cell_sr_cell_nof_instances_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_pipes;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_pipes_define_max_nof_subcontexts;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_pipes_max_nof_subcontexts_set;

    
    data_index = dnx_data_fabric_pipes_ocb_wfq;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_pipes_ocb_wfq_set;

    
    data_index = dnx_data_fabric_pipes_table_valid_map_config;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_pipes_valid_map_config_set;
    data_index = dnx_data_fabric_pipes_table_dtq_subcontexts;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_pipes_dtq_subcontexts_set;
    
    submodule_index = dnx_data_fabric_submodule_tdm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_tdm_define_priority;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_tdm_priority_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_cgm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_cgm_define_egress_drop_th_clocks_resolution;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_cgm_egress_drop_th_clocks_resolution_set;
    data_index = dnx_data_fabric_cgm_define_egress_fifo_fc_threshold;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_cgm_egress_fifo_fc_threshold_set;
    data_index = dnx_data_fabric_cgm_define_static_shaper_credit_resolution_bytes;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_cgm_static_shaper_credit_resolution_bytes_set;

    
    data_index = dnx_data_fabric_cgm_egress_drop_low_prio_mc;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_cgm_egress_drop_low_prio_mc_set;
    data_index = dnx_data_fabric_cgm_egress_fifo_static_size;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_cgm_egress_fifo_static_size_set;
    data_index = dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_cgm_is_egress_drop_tdm_latency_supported_set;
    data_index = dnx_data_fabric_cgm_is_static_shaper_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_cgm_is_static_shaper_supported_set;

    
    data_index = dnx_data_fabric_cgm_table_access_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_cgm_access_map_set;
    
    submodule_index = dnx_data_fabric_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_dbal_define_link_rci_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_link_rci_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_llfc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_llfc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_egress_fifo_drop_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_egress_fifo_drop_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_link_fifo_base_address_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_link_fifo_base_address_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_base_address_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_base_address_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_fifo_fc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_egress_fifo_fc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_egress_drop_mc_low_prio_th_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_egress_drop_mc_low_prio_th_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_cgm_ingress_fc_threshold_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_ingress_fc_threshold_max_value_set;
    data_index = dnx_data_fabric_dbal_define_cgm_static_shaper_calendar_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_cgm_static_shaper_calendar_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_mesh_mc;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_mesh_mc_define_nof_replication;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_fabric_mesh_mc_nof_replication_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_transmit;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_fabric_transmit_eir_fc_leaky_backet;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_fabric_transmit_eir_fc_leaky_backet_set;

    
    data_index = dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_set;
    data_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_set;
    data_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


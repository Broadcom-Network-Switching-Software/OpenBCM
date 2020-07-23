

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/dnxc_port.h>
#include <phymod/phymod.h>
#include <soc/dnx/intr/auto_generated/j2c/j2c_intr.h>







static shr_error_e
j2c_a0_dnx_data_nif_global_reassembler_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_reassembler;
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
j2c_a0_dnx_data_nif_global_clock_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_clock_power_down;
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
j2c_a0_dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down;
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
j2c_a0_dnx_data_nif_global_nof_lcplls_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_lcplls;
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
j2c_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 144;

    
    define->data = 144;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
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
j2c_a0_dnx_data_nif_global_reassembler_fifo_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_reassembler_fifo_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 90;

    
    define->data = 90;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_global_last_port_led_scan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_last_port_led_scan;
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
j2c_a0_dnx_data_nif_global_pll_phys_set(
    int unit)
{
    int lcpll_index;
    dnx_data_nif_global_pll_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int table_index = dnx_data_nif_global_table_pll_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.global.nof_lcplls_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.global.nof_lcplls_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_global_pll_phys_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_global_table_pll_phys");

    
    default_data = (dnx_data_nif_global_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->min_phy_id = 0;
    default_data->max_phy_id = 0;
    
    for (lcpll_index = 0; lcpll_index < table->keys[0].size; lcpll_index++)
    {
        data = (dnx_data_nif_global_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, lcpll_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->min_phy_id = 0;
        data->max_phy_id = 127;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set(
    int unit)
{
    int nif_interface_id_index;
    dnx_data_nif_global_nif_interface_id_to_unit_id_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int table_index = dnx_data_nif_global_table_nif_interface_id_to_unit_id;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_global_nif_interface_id_to_unit_id_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_global_table_nif_interface_id_to_unit_id");

    
    default_data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->unit_id = 0;
    default_data->internal_offset = 0;
    
    for (nif_interface_id_index = 0; nif_interface_id_index < table->keys[0].size; nif_interface_id_index++)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, nif_interface_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->unit_id = 0;
        data->internal_offset = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->unit_id = 0;
        data->internal_offset = 2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->unit_id = 0;
        data->internal_offset = 4;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->unit_id = 0;
        data->internal_offset = 6;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->unit_id = 1;
        data->internal_offset = 0;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->unit_id = 1;
        data->internal_offset = 2;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->unit_id = 1;
        data->internal_offset = 4;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->unit_id = 1;
        data->internal_offset = 6;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->unit_id = 2;
        data->internal_offset = 0;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->unit_id = 2;
        data->internal_offset = 2;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->unit_id = 2;
        data->internal_offset = 4;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->unit_id = 2;
        data->internal_offset = 6;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->unit_id = 3;
        data->internal_offset = 0;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->unit_id = 3;
        data->internal_offset = 2;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->unit_id = 3;
        data->internal_offset = 4;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->unit_id = 3;
        data->internal_offset = 6;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->unit_id = 4;
        data->internal_offset = 0;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->unit_id = 4;
        data->internal_offset = 2;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->unit_id = 4;
        data->internal_offset = 4;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->unit_id = 4;
        data->internal_offset = 6;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->unit_id = 4;
        data->internal_offset = 8;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->unit_id = 4;
        data->internal_offset = 10;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->unit_id = 4;
        data->internal_offset = 12;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->unit_id = 4;
        data->internal_offset = 14;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->unit_id = 5;
        data->internal_offset = 0;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->unit_id = 5;
        data->internal_offset = 2;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->unit_id = 5;
        data->internal_offset = 4;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->unit_id = 5;
        data->internal_offset = 6;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->unit_id = 5;
        data->internal_offset = 8;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->unit_id = 5;
        data->internal_offset = 10;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->unit_id = 5;
        data->internal_offset = 12;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->unit_id = 5;
        data->internal_offset = 14;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->unit_id = 6;
        data->internal_offset = 0;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->unit_id = 6;
        data->internal_offset = 2;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->unit_id = 6;
        data->internal_offset = 4;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->unit_id = 6;
        data->internal_offset = 6;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->unit_id = 6;
        data->internal_offset = 8;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->unit_id = 6;
        data->internal_offset = 10;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->unit_id = 6;
        data->internal_offset = 12;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->unit_id = 6;
        data->internal_offset = 14;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->unit_id = 7;
        data->internal_offset = 0;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->unit_id = 7;
        data->internal_offset = 2;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->unit_id = 7;
        data->internal_offset = 4;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->unit_id = 7;
        data->internal_offset = 6;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->unit_id = 7;
        data->internal_offset = 8;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->unit_id = 7;
        data->internal_offset = 10;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->unit_id = 7;
        data->internal_offset = 12;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->unit_id = 7;
        data->internal_offset = 14;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->unit_id = 8;
        data->internal_offset = 0;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->unit_id = 8;
        data->internal_offset = 2;
    }
    if (100 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->unit_id = 8;
        data->internal_offset = 4;
    }
    if (102 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->unit_id = 8;
        data->internal_offset = 6;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->unit_id = 8;
        data->internal_offset = 8;
    }
    if (106 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->unit_id = 8;
        data->internal_offset = 10;
    }
    if (108 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->unit_id = 8;
        data->internal_offset = 12;
    }
    if (110 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->unit_id = 8;
        data->internal_offset = 14;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->unit_id = 9;
        data->internal_offset = 0;
    }
    if (114 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->unit_id = 9;
        data->internal_offset = 2;
    }
    if (116 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->unit_id = 9;
        data->internal_offset = 4;
    }
    if (118 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->unit_id = 9;
        data->internal_offset = 6;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->unit_id = 9;
        data->internal_offset = 8;
    }
    if (122 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->unit_id = 9;
        data->internal_offset = 10;
    }
    if (124 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->unit_id = 9;
        data->internal_offset = 12;
    }
    if (126 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->unit_id = 9;
        data->internal_offset = 14;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->unit_id = 10;
        data->internal_offset = 0;
    }
    if (130 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 130, 0);
        data->unit_id = 11;
        data->internal_offset = 0;
    }
    if (132 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 132, 0);
        data->unit_id = 12;
        data->internal_offset = 0;
    }
    if (134 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, 0);
        data->unit_id = 13;
        data->internal_offset = 0;
    }
    if (136 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->unit_id = 14;
        data->internal_offset = 0;
    }
    if (138 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 138, 0);
        data->unit_id = 15;
        data->internal_offset = 0;
    }
    if (140 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 140, 0);
        data->unit_id = 16;
        data->internal_offset = 0;
    }
    if (142 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 142, 0);
        data->unit_id = 17;
        data->internal_offset = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_nif_phys_nof_phys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_nof_phys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 128;

    
    define->data = 128;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_phys_pm8x50_gen_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_pm8x50_gen;
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
j2c_a0_dnx_data_nif_phys_general_set(
    int unit)
{
    dnx_data_nif_phys_general_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_general;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_general_t, (1 + 1  ), "data of dnx_data_nif_phys_table_general");

    
    default_data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 4 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 0, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 1, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 2, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 3, 0xFFFFFFFF);
    
    data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_phys_core_phys_map_set(
    int unit)
{
    int core_index_index;
    dnx_data_nif_phys_core_phys_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_core_phys_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "0x0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_core_phys_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_core_phys_map");

    
    default_data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->phys, 0, 0x0);
    
    for (core_index_index = 0; core_index_index < table->keys[0].size; core_index_index++)
    {
        data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFFFFFFFF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFFFFFFFF);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0xFFFFFFFF);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0xFFFFFFFF);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_phys_vco_div_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_nif_phys_vco_div_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_vco_div;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_vco_div_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_vco_div");

    
    default_data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->vco_div = 0;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->vco_div = 20;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->vco_div = 20;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->vco_div = 20;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->vco_div = 20;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->vco_div = 40;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->vco_div = 40;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->vco_div = 40;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->vco_div = 40;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->vco_div = 40;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->vco_div = 40;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_nif_ilkn_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_supported;
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
j2c_a0_dnx_data_nif_ilkn_credits_selector_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_credits_selector;
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
j2c_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
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
j2c_a0_dnx_data_nif_ilkn_cdu_ilkn_selectors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_cdu_ilkn_selectors;
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
j2c_a0_dnx_data_nif_ilkn_clu_ilkn_selectors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_clu_ilkn_selectors;
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
j2c_a0_dnx_data_nif_ilkn_clu_ilkn_direction_selectors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_clu_ilkn_direction_selectors;
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
j2c_a0_dnx_data_nif_ilkn_support_burst_interleaving_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_burst_interleaving;
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
j2c_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
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
j2c_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_ilu_burst_max;
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
j2c_a0_dnx_data_nif_ilkn_is_fec_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_fec_supported;
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
j2c_a0_dnx_data_nif_ilkn_is_fec_bypass_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_fec_bypass_supported;
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
j2c_a0_dnx_data_nif_ilkn_fec_units_in_cdu_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_fec_units_in_cdu;
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
j2c_a0_dnx_data_nif_ilkn_ilu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilu_nof;
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
j2c_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
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
j2c_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
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
j2c_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
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
j2c_a0_dnx_data_nif_ilkn_lanes_max_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_max_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.lanes_max_nof_get(unit);

    
    define->data = dnx_data_nif.ilkn.lanes_max_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 24;

    
    define->data = 24;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
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
j2c_a0_dnx_data_nif_ilkn_segments_max_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_segments_max_nof;
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
j2c_a0_dnx_data_nif_ilkn_segments_half_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_segments_half_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.segments_max_nof_get(unit) /2;

    
    define->data = dnx_data_nif.ilkn.segments_max_nof_get(unit) /2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_pms_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_pms_nof;
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
j2c_a0_dnx_data_nif_ilkn_fmac_bus_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_fmac_bus_size;
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
j2c_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
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
j2c_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
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
j2c_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2048;

    
    define->data = 2048;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
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
j2c_a0_dnx_data_nif_ilkn_tx_hrf_credits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
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
j2c_a0_dnx_data_nif_ilkn_watermark_high_elk_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_high_elk;
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
j2c_a0_dnx_data_nif_ilkn_watermark_low_elk_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_low_elk;
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
j2c_a0_dnx_data_nif_ilkn_watermark_high_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_high_data;
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
j2c_a0_dnx_data_nif_ilkn_watermark_low_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_low_data;
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
j2c_a0_dnx_data_nif_ilkn_is_20G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_20G_speed_supported;
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
j2c_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_25G_speed_supported;
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
j2c_a0_dnx_data_nif_ilkn_is_28G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
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
j2c_a0_dnx_data_nif_ilkn_is_53G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_53G_speed_supported;
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
j2c_a0_dnx_data_nif_ilkn_is_nif_56G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_nif_56G_speed_supported;
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
j2c_a0_dnx_data_nif_ilkn_is_fabric_56G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_fabric_56G_speed_supported;
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
j2c_a0_dnx_data_nif_ilkn_pad_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_pad_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 60;

    
    define->data = 60;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_max_nof_ifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_max_nof_ifs;
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
j2c_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2C_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->data = J2C_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2C_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->data = J2C_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2c_a0_dnx_data_nif_ilkn_phys_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_phys_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_phys");

    
    default_data = (dnx_data_nif_ilkn_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->bitmap, 0, 0);
    default_data->lanes_array[0] = 0;
    default_data->array_order_valid = 0;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->property.name = "ilkn_lanes";
    table->property.doc =
        "\n"
        "ilkn_lanes_#ilkn_id#=#bitmap#\n"
        "The bitmap parameter is up to 96 bits.\n"
        "When a bit is set, the equivalent lane (PHY) is used for the corresponding port.\n"
        "The ilkn_id parameter is the ILKN interface ID in the range 0-1.\n"
        "\n"
    ;
    table->property.method = dnxc_data_property_method_custom;
    table->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_nif_ilkn_phys_read(unit, ilkn_id_index, data));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_supported_phys_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_supported_phys_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_supported_phys;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_phys_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_phys");

    
    default_data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_supported = 0;
    default_data->max_phys = 0;
    _SHR_PBMP_WORD_SET(default_data->nif_phys, 0, 0);
    _SHR_PBMP_WORD_SET(default_data->fabric_phys, 0, 0);
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0xffffff00);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x000000ff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0xffffff00);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x000000ff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0xff000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0xff000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0xffffff00);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0xffffff00);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00ffffff);
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00ffffff);
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0xffffff00);
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0xffffff00);
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0000ffff);
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0000ffff);
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0xffffff00);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0xffffff00);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 3, 0x00000000);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_ilkn_cores_set(
    int unit)
{
    int ilkn_core_id_index;
    dnx_data_nif_ilkn_ilkn_cores_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_ilkn_cores;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_ilkn_cores_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_ilkn_cores");

    
    default_data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_elk_supported = 0;
    default_data->is_data_supported = 0;
    default_data->fec_disable_by_bypass = 0;
    
    for (ilkn_core_id_index = 0; ilkn_core_id_index < table->keys[0].size; ilkn_core_id_index++)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_elk_supported = 1;
        data->is_data_supported = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->is_elk_supported = 1;
        data->is_data_supported = 1;
        data->fec_disable_by_bypass = 1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
        data->fec_disable_by_bypass = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_ilkn_cores_clup_facing_set(
    int unit)
{
    int ilkn_core_id_index;
    dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_ilkn_cores_clup_facing;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_ilkn_cores_clup_facing_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_ilkn_cores_clup_facing");

    
    default_data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->clu_unit = -1;
    
    for (ilkn_core_id_index = 0; ilkn_core_id_index < table->keys[0].size; ilkn_core_id_index++)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->clu_unit = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->clu_unit = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->clu_unit = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->clu_unit = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->clu_unit = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->clu_unit = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_supported_device_core_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_supported_device_core_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_supported_device_core;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_device_core_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_device_core");

    
    default_data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->core = 0;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_properties_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "256";
    table->values[1].default_val = "32";
    table->values[2].default_val = "64";
    table->values[3].default_val = "2048";
    table->values[4].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_properties");

    
    default_data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->burst_max = 256;
    default_data->burst_short = 32;
    default_data->burst_min = 64;
    default_data->metaframe_period = 2048;
    default_data->is_over_fabric = 0;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_ILKN_BURST_MAX;
    table->values[0].property.doc =
        "\n"
        "Configure ILKN BurstMax per ILKN id.\n"
        "in Jr2 the only supported value is 256.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range;
    table->values[0].property.method_str = "suffix_range";
    table->values[0].property.suffix = "";
    table->values[0].property.range_min = 256;
    table->values[0].property.range_max = 512;
    
    table->values[1].property.name = spn_ILKN_BURST_SHORT;
    table->values[1].property.doc =
        "\n"
        "Configure ILKN BurstShort per ILKN id.\n"
        "supported values: 32, 64... BurstMax / 2.\n"
        "default: 32.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_range;
    table->values[1].property.method_str = "suffix_range";
    table->values[1].property.suffix = "";
    table->values[1].property.range_min = 32;
    table->values[1].property.range_max = 128;
    
    table->values[2].property.name = spn_ILKN_BURST_MIN;
    table->values[2].property.doc =
        "\n"
        "Configure ILKN BurstMin per ILKN id.\n"
        "supported values: BurstShort... BurstMax / 2.\n"
        "default: burstShort.\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_suffix_range;
    table->values[2].property.method_str = "suffix_range";
    table->values[2].property.suffix = "";
    table->values[2].property.range_min = 64;
    table->values[2].property.range_max = 256;
    
    table->values[3].property.name = spn_ILKN_METAFRAME_SYNC_PERIOD;
    table->values[3].property.doc =
        "\n"
        "Configure ILKN Metaframe priod per ILKN id.\n"
        "supported values: 64 - 16K.\n"
        "default: 2048.\n"
        "\n"
    ;
    table->values[3].property.method = dnxc_data_property_method_suffix_range;
    table->values[3].property.method_str = "suffix_range";
    table->values[3].property.suffix = "";
    table->values[3].property.range_min = 64;
    table->values[3].property.range_max = 16384;
    
    table->values[4].property.name = "ilkn_use_fabric_links";
    table->values[4].property.doc =
        "\n"
        "Configure the ILKN IF to work over Fabric links instead of NIF.\n"
        "default = 0;\n"
        "\n"
    ;
    table->values[4].property.method = dnxc_data_property_method_custom;
    table->values[4].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, ilkn_id_index, &data->burst_max));
    }
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, ilkn_id_index, &data->burst_short));
    }
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, ilkn_id_index, &data->burst_min));
    }
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[3].property, ilkn_id_index, &data->metaframe_period));
    }
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_nif_ilkn_properties_is_over_fabric_read(unit, ilkn_id_index, &data->is_over_fabric));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_nif_pms_set(
    int unit)
{
    int pm_id_index;
    dnx_data_nif_ilkn_nif_pms_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_nif_pms;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);

    
    table->values[0].default_val = "portmodDispatchTypeCount";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "0xFF";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_nif_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_nif_pms");

    
    default_data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->first_phy = -1;
    default_data->nof_phys = -1;
    default_data->facing_core_index = 0xFF;
    
    for (pm_id_index = 0; pm_id_index < table->keys[0].size; pm_id_index++)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 0;
        data->nof_phys = 8;
        data->facing_core_index = 6;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 8;
        data->nof_phys = 8;
        data->facing_core_index = 6;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 16;
        data->nof_phys = 8;
        data->facing_core_index = 7;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 24;
        data->nof_phys = 8;
        data->facing_core_index = 7;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 32;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 36;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 40;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 44;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 48;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 52;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 56;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 60;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 64;
        data->nof_phys = 4;
        data->facing_core_index = 2;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 68;
        data->nof_phys = 4;
        data->facing_core_index = 2;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 72;
        data->nof_phys = 4;
        data->facing_core_index = 2;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 76;
        data->nof_phys = 4;
        data->facing_core_index = 2;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 80;
        data->nof_phys = 4;
        data->facing_core_index = 3;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 84;
        data->nof_phys = 4;
        data->facing_core_index = 3;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 88;
        data->nof_phys = 4;
        data->facing_core_index = 3;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 92;
        data->nof_phys = 4;
        data->facing_core_index = 3;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 96;
        data->nof_phys = 4;
        data->facing_core_index = 4;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 100;
        data->nof_phys = 4;
        data->facing_core_index = 4;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 104;
        data->nof_phys = 4;
        data->facing_core_index = 4;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 108;
        data->nof_phys = 4;
        data->facing_core_index = 4;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 112;
        data->nof_phys = 4;
        data->facing_core_index = 5;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 116;
        data->nof_phys = 4;
        data->facing_core_index = 5;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 120;
        data->nof_phys = 4;
        data->facing_core_index = 5;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 124;
        data->nof_phys = 4;
        data->facing_core_index = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_fabric_pms_set(
    int unit)
{
    int pm_id_index;
    dnx_data_nif_ilkn_fabric_pms_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_fabric_pms;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "portmodDispatchTypeCount";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_fabric_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_fabric_pms");

    
    default_data = (dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->first_phy = -1;
    default_data->nof_phys = -1;
    default_data->facing_core_index = -1;
    
    for (pm_id_index = 0; pm_id_index < table->keys[0].size; pm_id_index++)
    {
        data = (dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->first_phy = 0;
        data->nof_phys = 8;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->first_phy = 8;
        data->nof_phys = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_nif_lanes_map_set(
    int unit)
{
    int ilkn_core_index;
    int range_id_index;
    dnx_data_nif_ilkn_nif_lanes_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_nif_lanes_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_nif_ilkn_table_nif_lanes_map");

    
    default_data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->first_phy = -1;
    default_data->ilkn_lane_offset = -1;
    default_data->nof_lanes = -1;
    
    for (ilkn_core_index = 0; ilkn_core_index < table->keys[0].size; ilkn_core_index++)
    {
        for (range_id_index = 0; range_id_index < table->keys[1].size; range_id_index++)
        {
            data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_index, range_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->first_phy = 32;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->first_phy = 40;
        data->ilkn_lane_offset = 16;
        data->nof_lanes = 8;
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->first_phy = 48;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->first_phy = 56;
        data->ilkn_lane_offset = 16;
        data->nof_lanes = 8;
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->first_phy = 64;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->first_phy = 72;
        data->ilkn_lane_offset = 16;
        data->nof_lanes = 8;
    }
    if (3 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 1);
        data->first_phy = 80;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 16;
    }
    if (4 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->first_phy = 96;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (5 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->first_phy = 104;
        data->ilkn_lane_offset = 16;
        data->nof_lanes = 8;
    }
    if (5 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 1);
        data->first_phy = 112;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 16;
    }
    if (6 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->first_phy = 0;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (7 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->first_phy = 8;
        data->ilkn_lane_offset = 16;
        data->nof_lanes = 8;
    }
    if (7 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 1);
        data->first_phy = 16;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 16;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_fabric_lanes_map_set(
    int unit)
{
    int ilkn_core_index;
    int range_id_index;
    dnx_data_nif_ilkn_fabric_lanes_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_fabric_lanes_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;
    table->keys[1].size = 1;
    table->info_get.key_size[1] = 1;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_fabric_lanes_map_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_nif_ilkn_table_fabric_lanes_map");

    
    default_data = (dnx_data_nif_ilkn_fabric_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->first_phy = -1;
    default_data->ilkn_lane_offset = -1;
    default_data->nof_lanes = -1;
    
    for (ilkn_core_index = 0; ilkn_core_index < table->keys[0].size; ilkn_core_index++)
    {
        for (range_id_index = 0; range_id_index < table->keys[1].size; range_id_index++)
        {
            data = (dnx_data_nif_ilkn_fabric_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_index, range_id_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (6 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_fabric_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->first_phy = 0;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 16;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_ilkn_start_tx_threshold_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_start_tx_threshold_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_start_tx_threshold_table_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_start_tx_threshold_table");

    
    default_data = (dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->start_thr = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 640000;
        data->start_thr = 12;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_nif_eth_nif_scheduler_context_per_rmc_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_nif_scheduler_context_per_rmc_support;
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
j2c_a0_dnx_data_nif_eth_rmc_flush_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_rmc_flush_support;
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
j2c_a0_dnx_data_nif_eth_is_rmc_low_priority_needs_configuration_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_is_rmc_low_priority_needs_configuration;
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
j2c_a0_dnx_data_nif_eth_is_rx_port_mode_needs_configuration_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_is_rx_port_mode_needs_configuration;
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
j2c_a0_dnx_data_nif_eth_cdu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_nof;
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
j2c_a0_dnx_data_nif_eth_clu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_nof;
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
j2c_a0_dnx_data_nif_eth_cdum_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdum_nof;
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
j2c_a0_dnx_data_nif_eth_nof_pms_in_cdu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
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
j2c_a0_dnx_data_nif_eth_nof_pms_in_clu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_pms_in_clu;
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
j2c_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
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
j2c_a0_dnx_data_nif_eth_clu_memory_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_memory_entries_nof;
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
j2c_a0_dnx_data_nif_eth_priority_groups_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_priority_groups_nof;
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
j2c_a0_dnx_data_nif_eth_phy_map_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_phy_map_granularity;
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
j2c_a0_dnx_data_nif_eth_start_tx_threshold_table_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_eth_start_tx_threshold_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_start_tx_threshold_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_start_tx_threshold_table_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_start_tx_threshold_table");

    
    default_data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->start_thr = 0;
    default_data->start_thr_for_l1 = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 400000;
        data->start_thr = 12;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_eth_pm_properties_set(
    int unit)
{
    int pm_index_index;
    dnx_data_nif_eth_pm_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_pm_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);

    
    table->values[0].default_val = "0x0,0x0,0x0,0x0";
    table->values[1].default_val = "0x1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "portmodDispatchTypeCount";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_pm_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_pm_properties");

    
    default_data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 4 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->phys, 0, 0x0);
    _SHR_PBMP_WORD_SET(default_data->phys, 1, 0x0);
    _SHR_PBMP_WORD_SET(default_data->phys, 2, 0x0);
    _SHR_PBMP_WORD_SET(default_data->phys, 3, 0x0);
    default_data->tvco_pll_index = 0x1;
    default_data->phy_addr = 0;
    default_data->is_master = 0;
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->special_pll_check = 0;
    
    for (pm_index_index = 0; pm_index_index < table->keys[0].size; pm_index_index++)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x000000FF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 0;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x0000FF00);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 8;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00FF0000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 16;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFF000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 24;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000000F);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 0;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000000F0);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 4;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000F00);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 8;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000F000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 12;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000F0000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 16;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00F00000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 20;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0F000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 24;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xF0000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 28;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x0000000F);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 0;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x000000F0);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 4;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000F00);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 8;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x0000F000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 12;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x000F0000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 16;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00F00000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 20;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x0F000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 24;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0xF0000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000000);
        data->phy_addr = 28;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x0000000F);
        data->phy_addr = 0;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x000000F0);
        data->phy_addr = 4;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00000F00);
        data->phy_addr = 8;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x0000F000);
        data->phy_addr = 12;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x000F0000);
        data->phy_addr = 16;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x00F00000);
        data->phy_addr = 20;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0x0F000000);
        data->phy_addr = 24;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        #if 4 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 3, 0xF0000000);
        data->phy_addr = 28;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_eth_ethu_properties_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_nif_eth_ethu_properties_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_ethu_properties;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "imbDispatchTypeNone";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_ethu_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_ethu_properties");

    
    default_data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pms[0] = -1;
    default_data->type = imbDispatchTypeNone;
    default_data->type_index = -1;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->pms[0] = 0;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->pms[0] = 1;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->pms[0] = 2;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->pms[0] = 3;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 4;
        data->pms[1] = 5;
        data->pms[2] = 6;
        data->pms[3] = 7;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 8;
        data->pms[1] = 9;
        data->pms[2] = 10;
        data->pms[3] = 11;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 12;
        data->pms[1] = 13;
        data->pms[2] = 14;
        data->pms[3] = 15;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 16;
        data->pms[1] = 17;
        data->pms[2] = 18;
        data->pms[3] = 19;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 3;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 20;
        data->pms[1] = 21;
        data->pms[2] = 22;
        data->pms[3] = 23;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 4;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 24;
        data->pms[1] = 25;
        data->pms[2] = 26;
        data->pms[3] = 27;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 5;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_eth_phy_map_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_eth_phy_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_phy_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.phys.nof_phys_get(unit)/dnx_data_nif.eth.phy_map_granularity_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.phys.nof_phys_get(unit)/dnx_data_nif.eth.phy_map_granularity_get(unit);

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_phy_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_phy_map");

    
    default_data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->phy_index = -1;
    default_data->ethu_index = -1;
    default_data->pm_index = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->phy_index = 0;
        data->ethu_index = 0;
        data->pm_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->phy_index = 4;
        data->ethu_index = 0;
        data->pm_index = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->phy_index = 8;
        data->ethu_index = 1;
        data->pm_index = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->phy_index = 12;
        data->ethu_index = 1;
        data->pm_index = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->phy_index = 16;
        data->ethu_index = 2;
        data->pm_index = 2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->phy_index = 20;
        data->ethu_index = 2;
        data->pm_index = 2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->phy_index = 24;
        data->ethu_index = 3;
        data->pm_index = 3;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->phy_index = 28;
        data->ethu_index = 3;
        data->pm_index = 3;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->phy_index = 32;
        data->ethu_index = 4;
        data->pm_index = 4;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->phy_index = 36;
        data->ethu_index = 4;
        data->pm_index = 5;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->phy_index = 40;
        data->ethu_index = 4;
        data->pm_index = 6;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->phy_index = 44;
        data->ethu_index = 4;
        data->pm_index = 7;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->phy_index = 48;
        data->ethu_index = 5;
        data->pm_index = 8;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->phy_index = 52;
        data->ethu_index = 5;
        data->pm_index = 9;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->phy_index = 56;
        data->ethu_index = 5;
        data->pm_index = 10;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->phy_index = 60;
        data->ethu_index = 5;
        data->pm_index = 11;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->phy_index = 64;
        data->ethu_index = 6;
        data->pm_index = 12;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->phy_index = 68;
        data->ethu_index = 6;
        data->pm_index = 13;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->phy_index = 72;
        data->ethu_index = 6;
        data->pm_index = 14;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->phy_index = 76;
        data->ethu_index = 6;
        data->pm_index = 15;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->phy_index = 80;
        data->ethu_index = 7;
        data->pm_index = 16;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->phy_index = 84;
        data->ethu_index = 7;
        data->pm_index = 17;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->phy_index = 88;
        data->ethu_index = 7;
        data->pm_index = 18;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->phy_index = 92;
        data->ethu_index = 7;
        data->pm_index = 19;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->phy_index = 96;
        data->ethu_index = 8;
        data->pm_index = 20;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->phy_index = 100;
        data->ethu_index = 8;
        data->pm_index = 21;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->phy_index = 104;
        data->ethu_index = 8;
        data->pm_index = 22;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->phy_index = 108;
        data->ethu_index = 8;
        data->pm_index = 23;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->phy_index = 112;
        data->ethu_index = 9;
        data->pm_index = 24;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->phy_index = 116;
        data->ethu_index = 9;
        data->pm_index = 25;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->phy_index = 120;
        data->ethu_index = 9;
        data->pm_index = 26;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->phy_index = 124;
        data->ethu_index = 9;
        data->pm_index = 27;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2c_a0_dnx_data_nif_simulator_cdu_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_simulator;
    int define_index = dnx_data_nif_simulator_define_cdu_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodDispatchTypeTscbh;

    
    define->data = phymodDispatchTypeTscbh;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_simulator_clu_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_simulator;
    int define_index = dnx_data_nif_simulator_define_clu_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodDispatchTypeTscf16_gen3;

    
    define->data = phymodDispatchTypeTscf16_gen3;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
j2c_a0_dnx_data_nif_prd_nof_clu_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_clu_port_profiles;
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
j2c_a0_dnx_data_nif_portmod_pm_types_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_portmod;
    int define_index = dnx_data_nif_portmod_define_pm_types_nof;
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
j2c_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set(
    int unit)
{
    int index_index;
    dnx_data_nif_portmod_pm_types_and_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_portmod;
    int table_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_NIF_PORTMOD_PM_TYPES_NOF;
    table->info_get.key_size[0] = DNX_DATA_MAX_NIF_PORTMOD_PM_TYPES_NOF;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_portmod_pm_types_and_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_portmod_table_pm_types_and_interfaces");

    
    default_data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->type = 0;
    default_data->instances = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->type = portmodDispatchTypePm8x50;
        data->instances = dnx_data_nif.eth.cdu_nof_get(unit);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->type = portmodDispatchTypePm4x25;
        data->instances = dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_clu_get(unit);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->type = portmodDispatchTypePmOsILKN_50G;
        data->instances = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->type = portmodDispatchTypePm8x50_fabric;
        data->instances = dnx_data_fabric.blocks.nof_pms_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2c_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int feature_index = dnx_data_nif_scheduler_decrement_credit_on_read_enable;
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
j2c_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
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
j2c_a0_dnx_data_nif_dbal_clu_rx_rmc_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_clu_rx_rmc_entry_size;
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
j2c_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
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
j2c_a0_dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
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
j2c_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
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
j2c_a0_dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 14;

    
    define->data = 14;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 15;

    
    define->data = 15;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2c_a0_data_nif_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_nif;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_nif_submodule_global;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_global_define_nof_lcplls;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_global_nof_lcplls_set;
    data_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set;
    data_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set;
    data_index = dnx_data_nif_global_define_reassembler_fifo_threshold;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_global_reassembler_fifo_threshold_set;
    data_index = dnx_data_nif_global_define_last_port_led_scan;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_global_last_port_led_scan_set;

    
    data_index = dnx_data_nif_global_reassembler;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_global_reassembler_set;
    data_index = dnx_data_nif_global_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_global_clock_power_down_set;
    data_index = dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down_set;

    
    data_index = dnx_data_nif_global_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_global_pll_phys_set;
    data_index = dnx_data_nif_global_table_nif_interface_id_to_unit_id;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set;
    
    submodule_index = dnx_data_nif_submodule_phys;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_phys_define_nof_phys;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_phys_nof_phys_set;
    data_index = dnx_data_nif_phys_define_pm8x50_gen;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_phys_pm8x50_gen_set;

    

    
    data_index = dnx_data_nif_phys_table_general;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_phys_general_set;
    data_index = dnx_data_nif_phys_table_core_phys_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_phys_core_phys_map_set;
    data_index = dnx_data_nif_phys_table_vco_div;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_phys_vco_div_set;
    
    submodule_index = dnx_data_nif_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ilkn_define_ilu_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_ilu_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set;
    data_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_lanes_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set;
    data_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set;
    data_index = dnx_data_nif_ilkn_define_segments_max_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_segments_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_segments_half_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_segments_half_nof_set;
    data_index = dnx_data_nif_ilkn_define_pms_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_pms_nof_set;
    data_index = dnx_data_nif_ilkn_define_fmac_bus_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_fmac_bus_size_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_tx_hrf_credits_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_elk;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_watermark_high_elk_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_elk;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_watermark_low_elk_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_data;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_watermark_high_data_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_data;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_watermark_low_data_set;
    data_index = dnx_data_nif_ilkn_define_is_20G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_is_20G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_25G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_is_28G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_53G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_is_53G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_nif_56G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_is_nif_56G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_fabric_56G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_is_fabric_56G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_pad_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_pad_size_set;
    data_index = dnx_data_nif_ilkn_define_max_nof_ifs;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_max_nof_ifs_set;
    data_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set;

    
    data_index = dnx_data_nif_ilkn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_is_supported_set;
    data_index = dnx_data_nif_ilkn_credits_selector;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_credits_selector_set;
    data_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set;
    data_index = dnx_data_nif_ilkn_cdu_ilkn_selectors;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_cdu_ilkn_selectors_set;
    data_index = dnx_data_nif_ilkn_clu_ilkn_selectors;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_clu_ilkn_selectors_set;
    data_index = dnx_data_nif_ilkn_clu_ilkn_direction_selectors;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_clu_ilkn_direction_selectors_set;
    data_index = dnx_data_nif_ilkn_support_burst_interleaving;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_support_burst_interleaving_set;
    data_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set;
    data_index = dnx_data_nif_ilkn_support_ilu_burst_max;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set;
    data_index = dnx_data_nif_ilkn_is_fec_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_is_fec_supported_set;
    data_index = dnx_data_nif_ilkn_is_fec_bypass_supported;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_is_fec_bypass_supported_set;
    data_index = dnx_data_nif_ilkn_fec_units_in_cdu;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_ilkn_fec_units_in_cdu_set;

    
    data_index = dnx_data_nif_ilkn_table_phys;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_phys_set;
    data_index = dnx_data_nif_ilkn_table_supported_phys;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_supported_phys_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_ilkn_cores_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores_clup_facing;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_ilkn_cores_clup_facing_set;
    data_index = dnx_data_nif_ilkn_table_supported_device_core;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_supported_device_core_set;
    data_index = dnx_data_nif_ilkn_table_properties;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_properties_set;
    data_index = dnx_data_nif_ilkn_table_nif_pms;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_nif_pms_set;
    data_index = dnx_data_nif_ilkn_table_fabric_pms;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_fabric_pms_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_pms;
    table = &submodule->tables[data_index];
    table->set = NULL;
    data_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_nif_lanes_map_set;
    data_index = dnx_data_nif_ilkn_table_fabric_lanes_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_fabric_lanes_map_set;
    data_index = dnx_data_nif_ilkn_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set;
    
    submodule_index = dnx_data_nif_submodule_eth;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_eth_define_cdu_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_cdu_nof_set;
    data_index = dnx_data_nif_eth_define_clu_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_clu_nof_set;
    data_index = dnx_data_nif_eth_define_cdum_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_cdum_nof_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_nof_pms_in_cdu_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_clu;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_nof_pms_in_clu_set;
    data_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_clu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_clu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_priority_groups_nof_set;
    data_index = dnx_data_nif_eth_define_phy_map_granularity;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_eth_phy_map_granularity_set;

    
    data_index = dnx_data_nif_eth_nif_scheduler_context_per_rmc_support;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_eth_nif_scheduler_context_per_rmc_support_set;
    data_index = dnx_data_nif_eth_rmc_flush_support;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_eth_rmc_flush_support_set;
    data_index = dnx_data_nif_eth_is_rmc_low_priority_needs_configuration;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_eth_is_rmc_low_priority_needs_configuration_set;
    data_index = dnx_data_nif_eth_is_rx_port_mode_needs_configuration;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_eth_is_rx_port_mode_needs_configuration_set;

    
    data_index = dnx_data_nif_eth_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_eth_start_tx_threshold_table_set;
    data_index = dnx_data_nif_eth_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_eth_pm_properties_set;
    data_index = dnx_data_nif_eth_table_ethu_properties;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_eth_ethu_properties_set;
    data_index = dnx_data_nif_eth_table_phy_map;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_eth_phy_map_set;
    
    submodule_index = dnx_data_nif_submodule_simulator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_simulator_define_cdu_type;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_simulator_cdu_type_set;
    data_index = dnx_data_nif_simulator_define_clu_type;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_simulator_clu_type_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_prd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_prd_define_nof_clu_port_profiles;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_prd_nof_clu_port_profiles_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_portmod;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_portmod_define_pm_types_nof;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_portmod_pm_types_nof_set;

    

    
    data_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_scheduler;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_scheduler_decrement_credit_on_read_enable;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set;

    
    
    submodule_index = dnx_data_nif_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_clu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_clu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set;
    data_index = dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_set;
    data_index = dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


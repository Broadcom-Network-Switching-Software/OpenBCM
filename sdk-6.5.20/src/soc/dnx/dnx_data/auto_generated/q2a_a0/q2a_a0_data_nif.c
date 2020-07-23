

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
#include <bcm_int/dnx/port/port_flexe.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/dnxc_port.h>
#include <phymod/phymod.h>
#include <soc/dnx/intr/auto_generated/q2a/q2a_intr.h>
#include <bcm/switch.h>







static shr_error_e
q2a_a0_dnx_data_nif_global_clock_power_down_set(
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
q2a_a0_dnx_data_nif_global_tx_separate_clock_power_down_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_tx_separate_clock_power_down;
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
q2a_a0_dnx_data_nif_global_nof_lcplls_set(
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
q2a_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 138;

    
    define->data = 138;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set(
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
q2a_a0_dnx_data_nif_global_last_port_led_scan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_last_port_led_scan;
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
q2a_a0_dnx_data_nif_global_pll_phys_set(
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
        data->max_phy_id = 51;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set(
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
        data->unit_id = 2;
        data->internal_offset = 8;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->unit_id = 2;
        data->internal_offset = 10;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->unit_id = 3;
        data->internal_offset = 0;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->unit_id = 3;
        data->internal_offset = 2;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->unit_id = 3;
        data->internal_offset = 4;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->unit_id = 3;
        data->internal_offset = 6;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->unit_id = 3;
        data->internal_offset = 8;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->unit_id = 3;
        data->internal_offset = 10;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->unit_id = 4;
        data->internal_offset = 0;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->unit_id = 4;
        data->internal_offset = 2;
    }
    if (44 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 44, 0);
        data->unit_id = 4;
        data->internal_offset = 4;
    }
    if (46 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 46, 0);
        data->unit_id = 4;
        data->internal_offset = 6;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->unit_id = 4;
        data->internal_offset = 8;
    }
    if (50 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 50, 0);
        data->unit_id = 4;
        data->internal_offset = 10;
    }
    if (52 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 52, 0);
        data->unit_id = 5;
        data->internal_offset = 0;
    }
    if (54 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 54, 0);
        data->unit_id = 6;
        data->internal_offset = 0;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->unit_id = 7;
        data->internal_offset = 0;
    }
    if (58 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 58, 0);
        data->unit_id = 7;
        data->internal_offset = 2;
    }
    if (60 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 60, 0);
        data->unit_id = 7;
        data->internal_offset = 4;
    }
    if (62 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 62, 0);
        data->unit_id = 7;
        data->internal_offset = 6;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->unit_id = 7;
        data->internal_offset = 8;
    }
    if (66 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 66, 0);
        data->unit_id = 7;
        data->internal_offset = 10;
    }
    if (68 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 68, 0);
        data->unit_id = 7;
        data->internal_offset = 12;
    }
    if (70 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 70, 0);
        data->unit_id = 7;
        data->internal_offset = 14;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->unit_id = 7;
        data->internal_offset = 16;
    }
    if (74 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 74, 0);
        data->unit_id = 7;
        data->internal_offset = 18;
    }
    if (76 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 76, 0);
        data->unit_id = 7;
        data->internal_offset = 20;
    }
    if (78 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 78, 0);
        data->unit_id = 7;
        data->internal_offset = 22;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->unit_id = 7;
        data->internal_offset = 24;
    }
    if (82 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 82, 0);
        data->unit_id = 7;
        data->internal_offset = 26;
    }
    if (84 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 84, 0);
        data->unit_id = 7;
        data->internal_offset = 28;
    }
    if (86 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 86, 0);
        data->unit_id = 7;
        data->internal_offset = 30;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->unit_id = 7;
        data->internal_offset = 32;
    }
    if (90 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 90, 0);
        data->unit_id = 7;
        data->internal_offset = 34;
    }
    if (92 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 92, 0);
        data->unit_id = 7;
        data->internal_offset = 36;
    }
    if (94 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 94, 0);
        data->unit_id = 7;
        data->internal_offset = 38;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->unit_id = 7;
        data->internal_offset = 40;
    }
    if (98 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 98, 0);
        data->unit_id = 7;
        data->internal_offset = 42;
    }
    if (100 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 100, 0);
        data->unit_id = 7;
        data->internal_offset = 44;
    }
    if (102 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 102, 0);
        data->unit_id = 7;
        data->internal_offset = 46;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->unit_id = 7;
        data->internal_offset = 48;
    }
    if (106 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 106, 0);
        data->unit_id = 7;
        data->internal_offset = 50;
    }
    if (108 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 108, 0);
        data->unit_id = 7;
        data->internal_offset = 52;
    }
    if (110 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 110, 0);
        data->unit_id = 7;
        data->internal_offset = 54;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->unit_id = 7;
        data->internal_offset = 56;
    }
    if (114 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 114, 0);
        data->unit_id = 7;
        data->internal_offset = 58;
    }
    if (116 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 116, 0);
        data->unit_id = 7;
        data->internal_offset = 60;
    }
    if (118 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 118, 0);
        data->unit_id = 7;
        data->internal_offset = 62;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->unit_id = 7;
        data->internal_offset = 64;
    }
    if (122 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 122, 0);
        data->unit_id = 7;
        data->internal_offset = 66;
    }
    if (124 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 124, 0);
        data->unit_id = 7;
        data->internal_offset = 68;
    }
    if (126 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 126, 0);
        data->unit_id = 7;
        data->internal_offset = 70;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->unit_id = 7;
        data->internal_offset = 72;
    }
    if (130 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 130, 0);
        data->unit_id = 7;
        data->internal_offset = 74;
    }
    if (132 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 132, 0);
        data->unit_id = 7;
        data->internal_offset = 76;
    }
    if (134 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 134, 0);
        data->unit_id = 7;
        data->internal_offset = 78;
    }
    if (136 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->unit_id = 7;
        data->internal_offset = 80;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_nif_phys_nof_phys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_nof_phys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 52;

    
    define->data = 52;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_phys_pm8x50_gen_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_pm8x50_gen;
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
q2a_a0_dnx_data_nif_phys_general_set(
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

    
    table->values[0].default_val = "0xFFFFFFFF,0x000FFFFF";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_general_t, (1 + 1  ), "data of dnx_data_nif_phys_table_general");

    
    default_data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 2 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 0, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 1, 0x000FFFFF);
    
    data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_phys_core_phys_map_set(
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
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFFFFFFFF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000FFFFF);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_phys_vco_div_set(
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
        data->vco_div = 40;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->vco_div = 40;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->vco_div = 40;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_nif_ilkn_is_supported_set(
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
q2a_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set(
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
q2a_a0_dnx_data_nif_ilkn_clu_ilkn_selectors_set(
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
q2a_a0_dnx_data_nif_ilkn_clu_ilkn_direction_selectors_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_clu_ilkn_direction_selectors;
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
q2a_a0_dnx_data_nif_ilkn_support_burst_interleaving_set(
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
q2a_a0_dnx_data_nif_ilkn_support_xpmd_if_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_xpmd_if;
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
q2a_a0_dnx_data_nif_ilkn_support_aligner_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_aligner;
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
q2a_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
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
q2a_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_support_ilu_burst_max;
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
q2a_a0_dnx_data_nif_ilkn_is_fec_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_fec_supported;
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
q2a_a0_dnx_data_nif_ilkn_hrf_flush_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_hrf_flush_support;
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
q2a_a0_dnx_data_nif_ilkn_elk_require_core_reset_after_link_training_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_elk_require_core_reset_after_link_training;
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
q2a_a0_dnx_data_nif_ilkn_reset_core_after_link_training_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_reset_core_after_link_training;
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
q2a_a0_dnx_data_nif_ilkn_ilu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilu_nof;
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
q2a_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
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
q2a_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
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
q2a_a0_dnx_data_nif_ilkn_lanes_max_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set(
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
q2a_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set(
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
q2a_a0_dnx_data_nif_ilkn_segments_max_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_segments_max_nof;
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
q2a_a0_dnx_data_nif_ilkn_segments_half_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_pms_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_fmac_bus_size_set(
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
q2a_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set(
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
q2a_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set(
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
q2a_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set(
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
q2a_a0_dnx_data_nif_ilkn_tx_hrf_credits_set(
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
q2a_a0_dnx_data_nif_ilkn_watermark_high_elk_set(
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
q2a_a0_dnx_data_nif_ilkn_watermark_low_elk_set(
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
q2a_a0_dnx_data_nif_ilkn_watermark_high_data_set(
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
q2a_a0_dnx_data_nif_ilkn_watermark_low_data_set(
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
q2a_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set(
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
q2a_a0_dnx_data_nif_ilkn_is_28G_speed_supported_set(
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
q2a_a0_dnx_data_nif_ilkn_is_53G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_53G_speed_supported;
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
q2a_a0_dnx_data_nif_ilkn_pad_size_set(
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
q2a_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = Q2A_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->data = Q2A_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = Q2A_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->data = Q2A_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
q2a_a0_dnx_data_nif_ilkn_phys_set(
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

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
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
        "The bitmap parameter is up to 24 bits.\n"
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
q2a_a0_dnx_data_nif_ilkn_supported_phys_set(
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

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
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
        data->max_phys = 12;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x0ffffff0);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_supported = 1;
        data->max_phys = 12;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x0ffffff0);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->is_supported = 1;
        data->max_phys = 24;
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0xf0000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x000fffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->is_supported = 1;
        data->max_phys = 12;
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0xf0000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0x000fffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_ilkn_ilkn_cores_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
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
        data->is_elk_supported = 1;
        data->is_data_supported = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_elk_supported = 0;
        data->is_data_supported = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_ilkn_ilkn_cores_clup_facing_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_ilkn_supported_device_core_set(
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

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
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
q2a_a0_dnx_data_nif_ilkn_properties_set(
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

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "256";
    table->values[1].default_val = "64";
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
    default_data->burst_short = 64;
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
    table->values[0].property.range_max = 256;
    
    table->values[1].property.name = spn_ILKN_BURST_SHORT;
    table->values[1].property.doc =
        "\n"
        "Configure ILKN BurstShort per ILKN id.\n"
        "supported values: 64... BurstMax / 2.\n"
        "default: 64.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_range;
    table->values[1].property.method_str = "suffix_range";
    table->values[1].property.suffix = "";
    table->values[1].property.range_min = 64;
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
    table->values[2].property.range_max = 128;
    
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
q2a_a0_dnx_data_nif_ilkn_nif_pms_set(
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
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_nif_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_nif_pms");

    
    default_data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->first_phy = -1;
    default_data->nof_phys = -1;
    default_data->facing_core_index = -1;
    
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
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 8;
        data->nof_phys = 8;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 16;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 20;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 24;
        data->nof_phys = 4;
        data->facing_core_index = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 28;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 32;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 36;
        data->nof_phys = 4;
        data->facing_core_index = 1;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 40;
        data->nof_phys = 4;
        data->facing_core_index = -1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 44;
        data->nof_phys = 4;
        data->facing_core_index = -1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->first_phy = 48;
        data->nof_phys = 4;
        data->facing_core_index = -1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_ilkn_nif_lanes_map_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
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
        data->first_phy = 4;
        data->ilkn_lane_offset = 12;
        data->nof_lanes = 12;
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->first_phy = 16;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 12;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->first_phy = 28;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set(
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
        data->speed = 600000;
        data->start_thr = 13;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_nif_eth_nif_scheduler_context_per_rmc_support_set(
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
q2a_a0_dnx_data_nif_eth_rmc_flush_support_set(
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
q2a_a0_dnx_data_nif_eth_is_rmc_low_priority_needs_configuration_set(
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
q2a_a0_dnx_data_nif_eth_is_rx_port_mode_needs_configuration_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_is_rx_port_mode_needs_configuration;
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
q2a_a0_dnx_data_nif_eth_pcs_lane_map_reconfig_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_pcs_lane_map_reconfig_support;
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
q2a_a0_dnx_data_nif_eth_cdu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_nof;
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
q2a_a0_dnx_data_nif_eth_clu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_nof;
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
q2a_a0_dnx_data_nif_eth_cdum_nof_set(
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
q2a_a0_dnx_data_nif_eth_nof_pms_in_cdu_set(
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
q2a_a0_dnx_data_nif_eth_nof_pms_in_clu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_pms_in_clu;
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
q2a_a0_dnx_data_nif_eth_cdu_mac_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_mac_nof;
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
q2a_a0_dnx_data_nif_eth_mac_lanes_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_mac_lanes_nof;
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
q2a_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_eth_clu_memory_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_memory_entries_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 384;

    
    define->data = 384;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_eth_priority_groups_nof_set(
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
q2a_a0_dnx_data_nif_eth_phy_map_granularity_set(
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
q2a_a0_dnx_data_nif_eth_start_tx_threshold_table_set(
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

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
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
        data->speed = 1000;
        data->start_thr_for_l1 = 2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 10000;
        data->start_thr_for_l1 = 9;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 25000;
        data->start_thr_for_l1 = 14;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 50000;
        data->start_thr_for_l1 = 26;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 100000;
        data->start_thr_for_l1 = 48;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 200000;
        data->start_thr_for_l1 = 88;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 400000;
        data->start_thr = 13;
        data->start_thr_for_l1 = 168;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_eth_pm_properties_set(
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
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x000000FF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 0;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x0000FF00);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 8;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x000F0000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->phy_addr = 0;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00F00000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->phy_addr = 4;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x0F000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->phy_addr = 8;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xF0000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->phy_addr = 16;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000000F);
        data->phy_addr = 20;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000000F0);
        data->phy_addr = 24;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000F00);
        data->phy_addr = 0;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000F000);
        data->phy_addr = 4;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000F0000);
        data->phy_addr = 8;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm4x25;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_eth_ethu_properties_set(
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
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 2;
        data->pms[1] = 3;
        data->pms[2] = 4;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 5;
        data->pms[1] = 6;
        data->pms[2] = 7;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->pms[0] = 8;
        data->pms[1] = 9;
        data->pms[2] = 10;
        data->type = imbDispatchTypeImb_clu;
        data->type_index = 2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_eth_phy_map_set(
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
        data->pm_index = 3;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->phy_index = 24;
        data->ethu_index = 2;
        data->pm_index = 4;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->phy_index = 28;
        data->ethu_index = 3;
        data->pm_index = 5;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->phy_index = 32;
        data->ethu_index = 3;
        data->pm_index = 6;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->phy_index = 36;
        data->ethu_index = 3;
        data->pm_index = 7;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->phy_index = 40;
        data->ethu_index = 4;
        data->pm_index = 8;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->phy_index = 44;
        data->ethu_index = 4;
        data->pm_index = 9;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->phy_index = 48;
        data->ethu_index = 4;
        data->pm_index = 10;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_nif_simulator_cdu_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_simulator;
    int define_index = dnx_data_nif_simulator_define_cdu_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodDispatchTypeTscbh_flexe;

    
    define->data = phymodDispatchTypeTscbh_flexe;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_simulator_clu_type_set(
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
q2a_a0_dnx_data_nif_flexe_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_is_supported;
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
q2a_a0_dnx_data_nif_flexe_is_led_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_is_led_supported;
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
q2a_a0_dnx_data_nif_flexe_prd_is_bypassed_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_prd_is_bypassed;
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
q2a_a0_dnx_data_nif_flexe_double_slots_for_small_client_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_double_slots_for_small_client;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "1: Enable double slots for small clients.\n"
        "0: Disable double slots for small clients.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "flexe_double_slots";
    feature->property.label = DNXC_DATA_LABEL_FLEXE;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_double_slots_for_all_client_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_double_slots_for_all_client;
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
q2a_a0_dnx_data_nif_flexe_half_capability_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_half_capability;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "1: Enable half capability.\n"
        "0: Disable half capability.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "flexe_half_capability";
    feature->property.label = DNXC_DATA_LABEL_FLEXE;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_rmc_cal_per_priority_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_rmc_cal_per_priority;
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
q2a_a0_dnx_data_nif_flexe_avoid_bw_verification_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_avoid_bw_verification;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "1: Do not verify flexe total bandwidth.\n"
        "0: Do verify flexe total bandwidth.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "flexe_avoid_bw_verification";
    feature->property.label = DNXC_DATA_LABEL_FLEXE;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_pcs_port_enable_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_pcs_port_enable_support;
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
q2a_a0_dnx_data_nif_flexe_rx_valid_in_lf_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_rx_valid_in_lf;
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
q2a_a0_dnx_data_nif_flexe_l1_mismatch_rate_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_l1_mismatch_rate_support;
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
q2a_a0_dnx_data_nif_flexe_feu_ilkn_burst_size_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_feu_ilkn_burst_size_support;
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
q2a_a0_dnx_data_nif_flexe_disable_l1_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_disable_l1;
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
q2a_a0_dnx_data_nif_flexe_disable_oam_over_flexe_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_disable_oam_over_flexe;
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
q2a_a0_dnx_data_nif_flexe_feu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_feu_nof;
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
q2a_a0_dnx_data_nif_flexe_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.flexe.nof_flexe_clients_get(unit) + dnx_data_nif.flexe.nof_special_clients_get(unit);

    
    define->data = dnx_data_nif.flexe.nof_flexe_clients_get(unit) + dnx_data_nif.flexe.nof_special_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_flexe_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_special_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_special_clients;
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
q2a_a0_dnx_data_nif_flexe_nof_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_groups;
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
q2a_a0_dnx_data_nif_flexe_max_group_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_group_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFFFFE;

    
    define->data = 0xFFFFE;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_min_group_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_min_group_id;
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
q2a_a0_dnx_data_nif_flexe_nb_tx_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_tx_delay;
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
q2a_a0_dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25;

    
    define->data = 25;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_pcs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_pcs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.flexe.max_flexe_core_speed_get(unit)/dnx_data_nif.flexe.phy_speed_granularity_get(unit);

    
    define->data = dnx_data_nif.flexe.max_flexe_core_speed_get(unit)/dnx_data_nif.flexe.phy_speed_granularity_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_phy_speeds_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_phy_speeds;
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
q2a_a0_dnx_data_nif_flexe_phy_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_phy_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 50000;

    
    define->data = 50000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_max_flexe_core_speed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_flexe_core_speed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 400000;

    
    define->data = 400000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_flexe_core_clock_source_pm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_flexe_core_clock_source_pm;
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
q2a_a0_dnx_data_nif_flexe_nof_logical_fifos_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_logical_fifos;
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
q2a_a0_dnx_data_nif_flexe_sar_channel_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_sar_channel_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.flexe.nof_flexe_clients_get(unit);

    
    define->data = dnx_data_nif.flexe.nof_flexe_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_mac_channel_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_mac_channel_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.flexe.nof_flexe_clients_get(unit) + dnx_data_nif.flexe.sar_channel_base_get(unit);

    
    define->data = dnx_data_nif.flexe.nof_flexe_clients_get(unit) + dnx_data_nif.flexe.sar_channel_base_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_client_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_client_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 5000;

    
    define->data = 5000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_sb_rx_fifos_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_sb_rx_fifos;
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
q2a_a0_dnx_data_nif_flexe_oam_client_channel_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_oam_client_channel;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_ptp_client_channel_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_ptp_client_channel;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 81;

    
    define->data = 81;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_oam_tx_cal_slot_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_oam_tx_cal_slot;
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
q2a_a0_dnx_data_nif_flexe_ptp_tx_cal_slot_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_ptp_tx_cal_slot;
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
q2a_a0_dnx_data_nif_flexe_priority_groups_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_priority_groups_nof;
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
q2a_a0_dnx_data_nif_flexe_max_nof_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.flexe.max_flexe_core_speed_get(unit)/dnx_data_nif.flexe.client_speed_granularity_get(unit);

    
    define->data = dnx_data_nif.flexe.max_flexe_core_speed_get(unit)/dnx_data_nif.flexe.client_speed_granularity_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_min_client_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_min_client_id;
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
q2a_a0_dnx_data_nif_flexe_max_client_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_client_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFFFE;

    
    define->data = 0xFFFE;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_ptp_channel_max_bandwidth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_ptp_channel_max_bandwidth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 500;

    
    define->data = 500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_oam_channel_max_bandwidth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_oam_channel_max_bandwidth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 2500;

    
    define->data = 2500;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_mem_entries_per_slot_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_mem_entries_per_slot;
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
q2a_a0_dnx_data_nif_flexe_ptp_oam_fifo_entries_in_sb_rx_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_ptp_oam_fifo_entries_in_sb_rx;
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
q2a_a0_dnx_data_nif_flexe_nof_flexe_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_instances;
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
q2a_a0_dnx_data_nif_flexe_nof_flexe_lphys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_lphys;
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
q2a_a0_dnx_data_nif_flexe_nof_sar_timeslots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_sar_timeslots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 80;

    
    define->data = 80;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_mac_timeslots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_mac_timeslots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 81;

    
    define->data = 81;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_distributed_ilu_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_distributed_ilu_id;
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
q2a_a0_dnx_data_nif_flexe_max_nif_rate_centralized_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_nif_rate_centralized;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1220000;

    
    define->data = 1220000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_rmc_cal_nof_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_rmc_cal_nof_slots;
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
q2a_a0_dnx_data_nif_flexe_max_nif_rate_distributed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_nif_rate_distributed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 420000;

    
    define->data = 420000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_max_ilu_rate_distributed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_ilu_rate_distributed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 618744;

    
    define->data = 618744;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_average_ipg_for_l1_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_average_ipg_for_l1;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 88;

    
    define->data = 88;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_mac_tx_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_mac_tx_threshold;
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
q2a_a0_dnx_data_nif_flexe_flexe_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_flexe_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FLEXE_MODE_DISABLED;

    
    define->data = DNX_FLEXE_MODE_DISABLED;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FLEXE_DEVICE_MODE;
    define->property.doc = 
        "\n"
        "FlexE mode in the device.\n"
        "DISABLED: FlexE is disabled.\n"
        "CENTRALIZED: FlexE centralized mode.\n"
        "DISTRIBUTED: FlexE distributed mode.\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DISABLED";
    define->property.mapping[0].val = DNX_FLEXE_MODE_DISABLED;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "CENTRALIZED";
    define->property.mapping[1].val = DNX_FLEXE_MODE_CENTRALIZED;
    define->property.mapping[2].name = "DISTRIBUTED";
    define->property.mapping[2].val = DNX_FLEXE_MODE_DISTRIBUTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    define->property.label = DNXC_DATA_LABEL_FLEXE;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_nof_pms_per_feu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_pms_per_feu;
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
q2a_a0_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
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
q2a_a0_dnx_data_nif_flexe_phy_info_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_flexe_phy_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int table_index = dnx_data_nif_flexe_table_phy_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 4;
    table->info_get.key_size[0] = 4;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_flexe_phy_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_flexe_table_phy_info");

    
    default_data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->min_logical_phy_id = -1;
    default_data->max_logical_phy_id = -1;
    default_data->flexe_core_port[0] = -1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 50000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 126;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_PCS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flexe_core_port[0] = 7;
        data->flexe_core_port[1] = 6;
        data->flexe_core_port[2] = 5;
        data->flexe_core_port[3] = 4;
        data->flexe_core_port[4] = 3;
        data->flexe_core_port[5] = 2;
        data->flexe_core_port[6] = 1;
        data->flexe_core_port[7] = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 100000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 254;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_PCS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flexe_core_port[0] = 6;
        data->flexe_core_port[1] = 4;
        data->flexe_core_port[2] = 2;
        data->flexe_core_port[3] = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 200000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 126;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_PCS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flexe_core_port[0] = 4;
        data->flexe_core_port[1] = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 400000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 62;
        data->flexe_core_port[0] = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_start_tx_threshold_table_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_flexe_start_tx_threshold_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int table_index = dnx_data_nif_flexe_table_start_tx_threshold_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_flexe_start_tx_threshold_table_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_flexe_table_start_tx_threshold_table");

    
    default_data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->start_thr = 0;
    default_data->start_thr_for_l1 = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 5000;
        data->start_thr = 5;
        data->start_thr_for_l1 = 3;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 10000;
        data->start_thr = 9;
        data->start_thr_for_l1 = 13;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 25000;
        data->start_thr_for_l1 = 21;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 50000;
        data->start_thr = 13;
        data->start_thr_for_l1 = 21;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 100000;
        data->start_thr = 17;
        data->start_thr_for_l1 = 29;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 200000;
        data->start_thr = 25;
        data->start_thr_for_l1 = 45;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 400000;
        data->start_thr = 41;
        data->start_thr_for_l1 = 69;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_supported_pms_set(
    int unit)
{
    int feu_idx_index;
    dnx_data_nif_flexe_supported_pms_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int table_index = dnx_data_nif_flexe_table_supported_pms;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_flexe_supported_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_flexe_table_supported_pms");

    
    default_data = (dnx_data_nif_flexe_supported_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nif_pms[0] = -1;
    
    for (feu_idx_index = 0; feu_idx_index < table->keys[0].size; feu_idx_index++)
    {
        data = (dnx_data_nif_flexe_supported_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, feu_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_supported_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->nif_pms[0] = 0;
        data->nif_pms[1] = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_flexe_sar_cell_mode_to_size_set(
    int unit)
{
    int cell_mode_index;
    dnx_data_nif_flexe_sar_cell_mode_to_size_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int table_index = dnx_data_nif_flexe_table_sar_cell_mode_to_size;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_flexe_sar_cell_mode_to_size_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_flexe_table_sar_cell_mode_to_size");

    
    default_data = (dnx_data_nif_flexe_sar_cell_mode_to_size_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->packet_size = -1;
    
    for (cell_mode_index = 0; cell_mode_index < table->keys[0].size; cell_mode_index++)
    {
        data = (dnx_data_nif_flexe_sar_cell_mode_to_size_t *) dnxc_data_mgmt_table_data_get(unit, table, cell_mode_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmSwitchFlexeSarCellMode28x66b < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_sar_cell_mode_to_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmSwitchFlexeSarCellMode28x66b, 0);
        data->packet_size = 236;
    }
    if (bcmSwitchFlexeSarCellMode29x66b < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_sar_cell_mode_to_size_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmSwitchFlexeSarCellMode29x66b, 0);
        data->packet_size = 244;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int feature_index = dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration;
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
q2a_a0_dnx_data_nif_prd_cdu_rmc_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_cdu_rmc_threshold_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x3fff;

    
    define->data = 0x3fff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_nif_prd_nof_clu_port_profiles_set(
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
q2a_a0_dnx_data_nif_prd_nof_feu_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_feu_port_profiles;
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
q2a_a0_dnx_data_nif_portmod_pm_types_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_portmod;
    int define_index = dnx_data_nif_portmod_define_pm_types_nof;
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
q2a_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int feature_index = dnx_data_nif_scheduler_decrement_credit_on_read_enable;
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
q2a_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
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
q2a_a0_dnx_data_nif_dbal_clu_rx_rmc_entry_size_set(
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
q2a_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
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
q2a_a0_dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size;
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
q2a_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
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
q2a_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
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
q2a_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
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
q2a_a0_dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_set(
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
q2a_a0_dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size;
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
q2a_a0_dnx_data_nif_features_pm_mode_0122_invalid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_features;
    int feature_index = dnx_data_nif_features_pm_mode_0122_invalid;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
q2a_a0_data_nif_attach(
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
    define->set = q2a_a0_dnx_data_nif_global_nof_lcplls_set;
    data_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set;
    data_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set;
    data_index = dnx_data_nif_global_define_last_port_led_scan;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_global_last_port_led_scan_set;

    
    data_index = dnx_data_nif_global_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_global_clock_power_down_set;
    data_index = dnx_data_nif_global_tx_separate_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_global_tx_separate_clock_power_down_set;

    
    data_index = dnx_data_nif_global_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_global_pll_phys_set;
    data_index = dnx_data_nif_global_table_nif_interface_id_to_unit_id;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set;
    
    submodule_index = dnx_data_nif_submodule_phys;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_phys_define_nof_phys;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_phys_nof_phys_set;
    data_index = dnx_data_nif_phys_define_pm8x50_gen;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_phys_pm8x50_gen_set;

    

    
    data_index = dnx_data_nif_phys_table_general;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_phys_general_set;
    data_index = dnx_data_nif_phys_table_core_phys_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_phys_core_phys_map_set;
    data_index = dnx_data_nif_phys_table_vco_div;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_phys_vco_div_set;
    
    submodule_index = dnx_data_nif_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ilkn_define_ilu_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_ilu_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set;
    data_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_lanes_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set;
    data_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set;
    data_index = dnx_data_nif_ilkn_define_segments_max_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_segments_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_segments_half_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_segments_half_nof_set;
    data_index = dnx_data_nif_ilkn_define_pms_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_pms_nof_set;
    data_index = dnx_data_nif_ilkn_define_fmac_bus_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_fmac_bus_size_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_tx_hrf_credits_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_elk;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_watermark_high_elk_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_elk;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_watermark_low_elk_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_data;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_watermark_high_data_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_data;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_watermark_low_data_set;
    data_index = dnx_data_nif_ilkn_define_is_25G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_is_28G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_53G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_is_53G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_pad_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_pad_size_set;
    data_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set;

    
    data_index = dnx_data_nif_ilkn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_is_supported_set;
    data_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set;
    data_index = dnx_data_nif_ilkn_clu_ilkn_selectors;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_clu_ilkn_selectors_set;
    data_index = dnx_data_nif_ilkn_clu_ilkn_direction_selectors;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_clu_ilkn_direction_selectors_set;
    data_index = dnx_data_nif_ilkn_support_burst_interleaving;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_support_burst_interleaving_set;
    data_index = dnx_data_nif_ilkn_support_xpmd_if;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_support_xpmd_if_set;
    data_index = dnx_data_nif_ilkn_support_aligner;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_support_aligner_set;
    data_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set;
    data_index = dnx_data_nif_ilkn_support_ilu_burst_max;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set;
    data_index = dnx_data_nif_ilkn_is_fec_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_is_fec_supported_set;
    data_index = dnx_data_nif_ilkn_hrf_flush_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_hrf_flush_support_set;
    data_index = dnx_data_nif_ilkn_elk_require_core_reset_after_link_training;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_elk_require_core_reset_after_link_training_set;
    data_index = dnx_data_nif_ilkn_reset_core_after_link_training;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_ilkn_reset_core_after_link_training_set;

    
    data_index = dnx_data_nif_ilkn_table_phys;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_phys_set;
    data_index = dnx_data_nif_ilkn_table_supported_phys;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_supported_phys_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_ilkn_cores_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores_clup_facing;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_ilkn_cores_clup_facing_set;
    data_index = dnx_data_nif_ilkn_table_supported_device_core;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_supported_device_core_set;
    data_index = dnx_data_nif_ilkn_table_properties;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_properties_set;
    data_index = dnx_data_nif_ilkn_table_nif_pms;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_nif_pms_set;
    data_index = dnx_data_nif_ilkn_table_fabric_pms;
    table = &submodule->tables[data_index];
    table->set = NULL;
    data_index = dnx_data_nif_ilkn_table_ilkn_pms;
    table = &submodule->tables[data_index];
    table->set = NULL;
    data_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_nif_lanes_map_set;
    data_index = dnx_data_nif_ilkn_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set;
    
    submodule_index = dnx_data_nif_submodule_eth;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_eth_define_cdu_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_cdu_nof_set;
    data_index = dnx_data_nif_eth_define_clu_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_clu_nof_set;
    data_index = dnx_data_nif_eth_define_cdum_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_cdum_nof_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_nof_pms_in_cdu_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_clu;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_nof_pms_in_clu_set;
    data_index = dnx_data_nif_eth_define_cdu_mac_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_cdu_mac_nof_set;
    data_index = dnx_data_nif_eth_define_mac_lanes_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_mac_lanes_nof_set;
    data_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_clu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_clu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_priority_groups_nof_set;
    data_index = dnx_data_nif_eth_define_phy_map_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_eth_phy_map_granularity_set;

    
    data_index = dnx_data_nif_eth_nif_scheduler_context_per_rmc_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_eth_nif_scheduler_context_per_rmc_support_set;
    data_index = dnx_data_nif_eth_rmc_flush_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_eth_rmc_flush_support_set;
    data_index = dnx_data_nif_eth_is_rmc_low_priority_needs_configuration;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_eth_is_rmc_low_priority_needs_configuration_set;
    data_index = dnx_data_nif_eth_is_rx_port_mode_needs_configuration;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_eth_is_rx_port_mode_needs_configuration_set;
    data_index = dnx_data_nif_eth_pcs_lane_map_reconfig_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_eth_pcs_lane_map_reconfig_support_set;

    
    data_index = dnx_data_nif_eth_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_eth_start_tx_threshold_table_set;
    data_index = dnx_data_nif_eth_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_eth_pm_properties_set;
    data_index = dnx_data_nif_eth_table_ethu_properties;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_eth_ethu_properties_set;
    data_index = dnx_data_nif_eth_table_phy_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_eth_phy_map_set;
    
    submodule_index = dnx_data_nif_submodule_simulator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_simulator_define_cdu_type;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_simulator_cdu_type_set;
    data_index = dnx_data_nif_simulator_define_clu_type;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_simulator_clu_type_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_flexe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flexe_define_feu_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_feu_nof_set;
    data_index = dnx_data_nif_flexe_define_nof_clients;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_clients_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_clients;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_flexe_clients_set;
    data_index = dnx_data_nif_flexe_define_nof_special_clients;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_special_clients_set;
    data_index = dnx_data_nif_flexe_define_nof_groups;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_groups_set;
    data_index = dnx_data_nif_flexe_define_max_group_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_group_id_set;
    data_index = dnx_data_nif_flexe_define_min_group_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_min_group_id_set;
    data_index = dnx_data_nif_flexe_define_nb_tx_delay;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nb_tx_delay_set;
    data_index = dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_set;
    data_index = dnx_data_nif_flexe_define_nof_pcs;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_pcs_set;
    data_index = dnx_data_nif_flexe_define_nof_phy_speeds;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_phy_speeds_set;
    data_index = dnx_data_nif_flexe_define_phy_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_phy_speed_granularity_set;
    data_index = dnx_data_nif_flexe_define_max_flexe_core_speed;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_flexe_core_speed_set;
    data_index = dnx_data_nif_flexe_define_flexe_core_clock_source_pm;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_flexe_core_clock_source_pm_set;
    data_index = dnx_data_nif_flexe_define_nof_logical_fifos;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_logical_fifos_set;
    data_index = dnx_data_nif_flexe_define_sar_channel_base;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_sar_channel_base_set;
    data_index = dnx_data_nif_flexe_define_mac_channel_base;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_mac_channel_base_set;
    data_index = dnx_data_nif_flexe_define_client_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_client_speed_granularity_set;
    data_index = dnx_data_nif_flexe_define_nof_sb_rx_fifos;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_sb_rx_fifos_set;
    data_index = dnx_data_nif_flexe_define_oam_client_channel;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_oam_client_channel_set;
    data_index = dnx_data_nif_flexe_define_ptp_client_channel;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_ptp_client_channel_set;
    data_index = dnx_data_nif_flexe_define_oam_tx_cal_slot;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_oam_tx_cal_slot_set;
    data_index = dnx_data_nif_flexe_define_ptp_tx_cal_slot;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_ptp_tx_cal_slot_set;
    data_index = dnx_data_nif_flexe_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_priority_groups_nof_set;
    data_index = dnx_data_nif_flexe_define_max_nof_slots;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_nof_slots_set;
    data_index = dnx_data_nif_flexe_define_min_client_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_min_client_id_set;
    data_index = dnx_data_nif_flexe_define_max_client_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_client_id_set;
    data_index = dnx_data_nif_flexe_define_ptp_channel_max_bandwidth;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_ptp_channel_max_bandwidth_set;
    data_index = dnx_data_nif_flexe_define_oam_channel_max_bandwidth;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_oam_channel_max_bandwidth_set;
    data_index = dnx_data_nif_flexe_define_nof_mem_entries_per_slot;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_mem_entries_per_slot_set;
    data_index = dnx_data_nif_flexe_define_ptp_oam_fifo_entries_in_sb_rx;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_ptp_oam_fifo_entries_in_sb_rx_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_instances;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_flexe_instances_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_lphys;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_flexe_lphys_set;
    data_index = dnx_data_nif_flexe_define_nof_sar_timeslots;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_sar_timeslots_set;
    data_index = dnx_data_nif_flexe_define_nof_mac_timeslots;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_mac_timeslots_set;
    data_index = dnx_data_nif_flexe_define_distributed_ilu_id;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_distributed_ilu_id_set;
    data_index = dnx_data_nif_flexe_define_max_nif_rate_centralized;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_nif_rate_centralized_set;
    data_index = dnx_data_nif_flexe_define_rmc_cal_nof_slots;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_rmc_cal_nof_slots_set;
    data_index = dnx_data_nif_flexe_define_max_nif_rate_distributed;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_nif_rate_distributed_set;
    data_index = dnx_data_nif_flexe_define_max_ilu_rate_distributed;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_max_ilu_rate_distributed_set;
    data_index = dnx_data_nif_flexe_define_average_ipg_for_l1;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_average_ipg_for_l1_set;
    data_index = dnx_data_nif_flexe_define_mac_tx_threshold;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_mac_tx_threshold_set;
    data_index = dnx_data_nif_flexe_define_flexe_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_flexe_mode_set;
    data_index = dnx_data_nif_flexe_define_nof_pms_per_feu;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nof_pms_per_feu_set;
    data_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set;

    
    data_index = dnx_data_nif_flexe_is_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_is_supported_set;
    data_index = dnx_data_nif_flexe_is_led_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_is_led_supported_set;
    data_index = dnx_data_nif_flexe_prd_is_bypassed;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_prd_is_bypassed_set;
    data_index = dnx_data_nif_flexe_double_slots_for_small_client;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_double_slots_for_small_client_set;
    data_index = dnx_data_nif_flexe_double_slots_for_all_client;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_double_slots_for_all_client_set;
    data_index = dnx_data_nif_flexe_half_capability;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_half_capability_set;
    data_index = dnx_data_nif_flexe_rmc_cal_per_priority;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_rmc_cal_per_priority_set;
    data_index = dnx_data_nif_flexe_avoid_bw_verification;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_avoid_bw_verification_set;
    data_index = dnx_data_nif_flexe_pcs_port_enable_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_pcs_port_enable_support_set;
    data_index = dnx_data_nif_flexe_rx_valid_in_lf;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_rx_valid_in_lf_set;
    data_index = dnx_data_nif_flexe_l1_mismatch_rate_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_l1_mismatch_rate_support_set;
    data_index = dnx_data_nif_flexe_feu_ilkn_burst_size_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_feu_ilkn_burst_size_support_set;
    data_index = dnx_data_nif_flexe_disable_l1;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_disable_l1_set;
    data_index = dnx_data_nif_flexe_disable_oam_over_flexe;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_flexe_disable_oam_over_flexe_set;

    
    data_index = dnx_data_nif_flexe_table_phy_info;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_flexe_phy_info_set;
    data_index = dnx_data_nif_flexe_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_flexe_start_tx_threshold_table_set;
    data_index = dnx_data_nif_flexe_table_supported_pms;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_flexe_supported_pms_set;
    data_index = dnx_data_nif_flexe_table_sar_cell_mode_to_size;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_flexe_sar_cell_mode_to_size_set;
    
    submodule_index = dnx_data_nif_submodule_prd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_prd_define_cdu_rmc_threshold_max;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_prd_cdu_rmc_threshold_max_set;
    data_index = dnx_data_nif_prd_define_nof_clu_port_profiles;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_prd_nof_clu_port_profiles_set;
    data_index = dnx_data_nif_prd_define_nof_feu_port_profiles;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_prd_nof_feu_port_profiles_set;

    
    data_index = dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration_set;

    
    
    submodule_index = dnx_data_nif_submodule_portmod;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_portmod_define_pm_types_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_portmod_pm_types_nof_set;

    

    
    data_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_scheduler;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_scheduler_decrement_credit_on_read_enable;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set;

    
    
    submodule_index = dnx_data_nif_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_clu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_clu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set;
    data_index = dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_set;
    data_index = dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_features_pm_mode_0122_invalid;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_nif_features_pm_mode_0122_invalid_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


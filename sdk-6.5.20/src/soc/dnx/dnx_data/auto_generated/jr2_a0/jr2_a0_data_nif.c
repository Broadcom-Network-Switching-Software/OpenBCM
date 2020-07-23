

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
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnxc/dnxc_port.h>
#include <phymod/phymod.h>
#include <soc/dnx/intr/auto_generated/jr2/jr2_intr.h>








static shr_error_e
jr2_a0_dnx_data_nif_global_nof_lcplls_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_lcplls;
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
jr2_a0_dnx_data_nif_global_max_core_access_per_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_max_core_access_per_port;
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
jr2_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
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
jr2_a0_dnx_data_nif_global_last_port_led_scan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_last_port_led_scan;
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
jr2_a0_dnx_data_nif_global_start_tx_threshold_global_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_start_tx_threshold_global;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Start TX threshold controls how many 64B words will be accumulated in the MLF before starting to transmit\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_range;
    define->property.method_str = "suffix_range";
    define->property.suffix = "start_tx_threshold";
    define->property.range_min = 0;
    define->property.range_max = 0x3fff;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_nif_global_pll_phys_set(
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
        data->max_phy_id = 47;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_pll_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->min_phy_id = 48;
        data->max_phy_id = 95;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_nif_phys_nof_phys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_nof_phys;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_lanes_in_cdu_get(unit) + dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_lanes_in_clu_get(unit);

    
    define->data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_lanes_in_cdu_get(unit) + dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_lanes_in_clu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_phys_nof_phys_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_phys_pm8x50_gen_set(
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
jr2_a0_dnx_data_nif_phys_is_pam4_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_is_pam4_speed_supported;
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
jr2_a0_dnx_data_nif_phys_general_set(
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

    
    table->values[0].default_val = "0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_general_t, (1 + 1  ), "data of dnx_data_nif_phys_table_general");

    
    default_data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 3 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 0, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 1, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 2, 0xFFFFFFFF);
    
    data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_phys_polarity_set(
    int unit)
{
    int lane_index_index;
    dnx_data_nif_phys_polarity_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_polarity;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.phys.nof_phys_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.phys.nof_phys_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_polarity_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_polarity");

    
    default_data = (dnx_data_nif_phys_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->tx_polarity = 0;
    default_data->rx_polarity = 0;
    
    for (lane_index_index = 0; lane_index_index < table->keys[0].size; lane_index_index++)
    {
        data = (dnx_data_nif_phys_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PHY_TX_POLARITY_FLIP;
    table->values[0].property.doc =
        "\n"
        "Flips PHY TX polarity if enabled\n"
        "phy_tx_polarity_flip_#lane# = 0 / 1\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_enable;
    table->values[0].property.method_str = "suffix_enable";
    table->values[0].property.suffix = "phy";
    
    table->values[1].property.name = spn_PHY_RX_POLARITY_FLIP;
    table->values[1].property.doc =
        "\n"
        "Flips PHY RX polarity if enabled\n"
        "phy_tx_polarity_flip_#lane# = 0 / 1\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_enable;
    table->values[1].property.method_str = "suffix_enable";
    table->values[1].property.suffix = "phy";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (lane_index_index = 0; lane_index_index < table->keys[0].size; lane_index_index++)
    {
        data = (dnx_data_nif_phys_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, lane_index_index, &data->tx_polarity));
    }
    for (lane_index_index = 0; lane_index_index < table->keys[0].size; lane_index_index++)
    {
        data = (dnx_data_nif_phys_polarity_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, lane_index_index, &data->rx_polarity));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_phys_core_phys_map_set(
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
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFFFFFFFF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000FFFF);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFFFF0000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0xFFFFFFFF);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_phys_vco_div_set(
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
        data->vco_div = 20;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->vco_div = 20;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->vco_div = 20;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->vco_div = 20;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->vco_div = 20;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->vco_div = 20;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->vco_div = 20;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->vco_div = 20;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_nif_ilkn_is_supported_set(
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
jr2_a0_dnx_data_nif_ilkn_is_ilu_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_ilu_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.ilkn.ilu_nof_get(unit) > 0 ? TRUE : FALSE;

    
    feature->data = dnx_data_nif.ilkn.ilu_nof_get(unit) > 0 ? TRUE : FALSE;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
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
jr2_a0_dnx_data_nif_ilkn_is_fec_supported_set(
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
jr2_a0_dnx_data_nif_ilkn_ilu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilu_nof;
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
jr2_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set(
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
jr2_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
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
jr2_a0_dnx_data_nif_ilkn_ilkn_if_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_if_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    
    define->data = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit) * dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
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
jr2_a0_dnx_data_nif_ilkn_nof_lanes_per_fec_unit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit;
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
jr2_a0_dnx_data_nif_ilkn_lanes_max_nof_set(
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
jr2_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
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
jr2_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
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
jr2_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
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
jr2_a0_dnx_data_nif_ilkn_segments_max_nof_set(
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
jr2_a0_dnx_data_nif_ilkn_segments_half_nof_set(
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
jr2_a0_dnx_data_nif_ilkn_pms_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_pms_nof;
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
jr2_a0_dnx_data_nif_ilkn_fmac_bus_size_set(
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
jr2_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
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
jr2_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
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
jr2_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
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
jr2_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
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
jr2_a0_dnx_data_nif_ilkn_tx_hrf_credits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
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
jr2_a0_dnx_data_nif_ilkn_nof_rx_hrf_per_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_nof_rx_hrf_per_port;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit) ? (dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get(unit) / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)) : 0;

    
    define->data = dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit) ? (dnx_data_nif.ilkn.ilkn_rx_hrf_nof_get(unit) / dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit)) : 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_watermark_high_elk_set(
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
jr2_a0_dnx_data_nif_ilkn_watermark_low_elk_set(
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
jr2_a0_dnx_data_nif_ilkn_watermark_high_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_high_data;
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
jr2_a0_dnx_data_nif_ilkn_watermark_low_data_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_watermark_low_data;
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
jr2_a0_dnx_data_nif_ilkn_is_20G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_20G_speed_supported;
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
jr2_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_25G_speed_supported;
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
jr2_a0_dnx_data_nif_ilkn_is_27G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_27G_speed_supported;
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
jr2_a0_dnx_data_nif_ilkn_is_28G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
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
jr2_a0_dnx_data_nif_ilkn_is_53G_speed_supported_set(
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
jr2_a0_dnx_data_nif_ilkn_is_nif_56G_speed_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_is_nif_56G_speed_supported;
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
jr2_a0_dnx_data_nif_ilkn_is_fabric_56G_speed_supported_set(
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
jr2_a0_dnx_data_nif_ilkn_max_nof_ifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_max_nof_ifs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_max_nof_elk_ifs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_max_nof_elk_ifs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = -1;

    
    define->data = -1;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = JR2_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->data = JR2_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = JR2_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->data = JR2_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_nif_ilkn_phys_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
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
jr2_a0_dnx_data_nif_ilkn_supported_phys_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
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
        data->max_phys = 16;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x0000ffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0000ffff);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_supported = 1;
        data->max_phys = 16;
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 1, 0xffff0000);
        _SHR_PBMP_WORD_SET(data->nif_phys, 2, 0x00000000);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 1, 0xff000000);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 2, 0x000000ff);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_supported_interfaces_set(
    int unit)
{
    int index_index;
    dnx_data_nif_ilkn_supported_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_supported_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 39;
    table->info_get.key_size[0] = 39;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "portmodDispatchTypeCount";
    table->values[2].default_val = "bcmPortPhyFecInvalid";
    table->values[3].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_interfaces");

    
    default_data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->pm_dispatch_type = portmodDispatchTypeCount;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->is_valid = 1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10312;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_20G_speed_supported_get(unit) && dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_25G_speed_supported_get(unit) && dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_27G_speed_supported_get(unit) && dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm4x25;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_28G_speed_supported_get(unit) && dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 10312;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 10312;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_20G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_20G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_25G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_25G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_27G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_27G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_28G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_28G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->speed = 53125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.ilkn.is_53G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->speed = 56250;
        data->pm_dispatch_type = portmodDispatchTypePm8x50;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.ilkn.is_nif_56G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->speed = 10312;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->speed = 10312;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_20G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_20G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_25G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_25G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_27G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_27G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.is_28G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_28G_speed_supported_get(unit) && dnx_data_nif.ilkn.fec_units_per_core_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->speed = 53125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_53G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->speed = 56250;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->fec_type = bcmPortPhyFecRs544;
        data->is_valid = dnx_data_nif.ilkn.is_fabric_56G_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_ilkn_cores_set(
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

    
    table->values[0].default_val = "1";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_ilkn_cores_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_ilkn_cores");

    
    default_data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_elk_supported = 1;
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
        data->is_data_supported = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_elk_supported = 1;
        data->is_data_supported = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_ilkn_cores_clup_facing_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_supported_device_core_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_supported_device_core_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_supported_device_core");

    
    default_data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->core = -1;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->core = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->core = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_properties_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "256";
    table->values[1].default_val = "32";
    table->values[2].default_val = "32";
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
    default_data->burst_min = 32;
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
    table->values[2].property.range_min = 32;
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
jr2_a0_dnx_data_nif_ilkn_nif_pms_set(
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
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 48;
        data->nof_phys = 8;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->first_phy = 56;
        data->nof_phys = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_fabric_pms_set(
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

    
    table->keys[0].size = 14;
    table->info_get.key_size[0] = 14;

    
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
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->first_phy = 56;
        data->nof_phys = 8;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_fabric;
        data->first_phy = 64;
        data->nof_phys = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_ilkn_pms_set(
    int unit)
{
    int ilkn_id_index;
    dnx_data_nif_ilkn_ilkn_pms_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int table_index = dnx_data_nif_ilkn_table_ilkn_pms;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_ilkn_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_ilkn_pms");

    
    default_data = (dnx_data_nif_ilkn_ilkn_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nif_pm_ids[0] = -1;
    default_data->fabric_pm_ids[0] = -1;
    
    for (ilkn_id_index = 0; ilkn_id_index < table->keys[0].size; ilkn_id_index++)
    {
        data = (dnx_data_nif_ilkn_ilkn_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, ilkn_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ILKN_PMS_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->nif_pm_ids[0] = 0;
        data->nif_pm_ids[1] = 1;
        data->nif_pm_ids[2] = 2;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ILKN_PMS_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->fabric_pm_ids[0] = 0;
        data->fabric_pm_ids[1] = 1;
        data->fabric_pm_ids[2] = 2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_ilkn_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ILKN_PMS_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->nif_pm_ids[0] = 6;
        data->nif_pm_ids[1] = 7;
        data->nif_pm_ids[2] = 8;
        SHR_RANGE_VERIFY(3, 0, DNX_DATA_MAX_NIF_ILKN_PMS_NOF, _SHR_E_INTERNAL, "out of bound access to array")
        data->fabric_pm_ids[0] = 7;
        data->fabric_pm_ids[1] = 8;
        data->fabric_pm_ids[2] = 9;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_nif_lanes_map_set(
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
        data->first_phy = 0;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->first_phy = 48;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_ilkn_fabric_lanes_map_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;
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
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_fabric_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->first_phy = 0;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_nif_ilkn_fabric_lanes_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->first_phy = 56;
        data->ilkn_lane_offset = 0;
        data->nof_lanes = 24;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_nif_eth_nif_scheduler_context_per_rmc_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_nif_scheduler_context_per_rmc_support;
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
jr2_a0_dnx_data_nif_eth_is_clu_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_is_clu_supported;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;

    
    feature->data = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_is_rx_port_mode_needs_configuration_set(
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
jr2_a0_dnx_data_nif_eth_pcs_lane_map_reconfig_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_pcs_lane_map_reconfig_support;
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
jr2_a0_dnx_data_nif_eth_gear_shifter_exists_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_gear_shifter_exists;
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
jr2_a0_dnx_data_nif_eth_pm8x50_bw_update_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_pm8x50_bw_update;
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
jr2_a0_dnx_data_nif_eth_portmod_thread_disable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_portmod_thread_disable;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_PORTMOD_THREAD_DISABLE;
    feature->property.doc = 
        "\n"
        "1: Disable pm8x50 portmon thread.\n"
        "0: Enable pm8x50 portmon thread.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_enable;
    feature->property.method_str = "enable";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
jr2_a0_dnx_data_nif_eth_cdu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_nof;
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
jr2_a0_dnx_data_nif_eth_clu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_nof;
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
jr2_a0_dnx_data_nif_eth_cdum_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdum_nof;
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
jr2_a0_dnx_data_nif_eth_nof_pms_in_cdu_set(
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
jr2_a0_dnx_data_nif_eth_nof_pms_in_clu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_pms_in_clu;
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
jr2_a0_dnx_data_nif_eth_total_nof_ethu_pms_in_device_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_total_nof_ethu_pms_in_device;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_cdu_get(unit) + dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_clu_get(unit);

    
    define->data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_cdu_get(unit) + dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_clu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_nof_cdu_lanes_in_pm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_cdu_lanes_in_pm;
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
jr2_a0_dnx_data_nif_eth_nof_clu_lanes_in_pm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_clu_lanes_in_pm;
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
jr2_a0_dnx_data_nif_eth_nof_cdu_pms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_cdu_pms;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_cdu_get(unit);

    
    define->data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_cdu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_nof_clu_pms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_clu_pms;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_clu_get(unit);

    
    define->data = dnx_data_nif.eth.clu_nof_get(unit)*dnx_data_nif.eth.nof_pms_in_clu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_ethu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_ethu_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.cdu_nof_get(unit) + dnx_data_nif.eth.clu_nof_get(unit);

    
    define->data = dnx_data_nif.eth.cdu_nof_get(unit) + dnx_data_nif.eth.clu_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_ethu_nof_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_ethu_nof_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.ethu_nof_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->data = dnx_data_nif.eth.ethu_nof_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_cdu_nof_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_nof_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.cdu_nof_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->data = dnx_data_nif.eth.cdu_nof_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_clu_nof_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_nof_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.clu_nof_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->data = dnx_data_nif.eth.clu_nof_get(unit)/dnx_data_device.general.nof_cores_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_nof_lanes_in_cdu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_lanes_in_cdu;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.nof_pms_in_cdu_get(unit)*dnx_data_nif.eth.nof_cdu_lanes_in_pm_get(unit);

    
    define->data = dnx_data_nif.eth.nof_pms_in_cdu_get(unit)*dnx_data_nif.eth.nof_cdu_lanes_in_pm_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_nof_lanes_in_clu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_lanes_in_clu;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.nof_pms_in_clu_get(unit)*dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);

    
    define->data = dnx_data_nif.eth.nof_pms_in_clu_get(unit)*dnx_data_nif.eth.nof_clu_lanes_in_pm_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_max_nof_lanes_in_ethu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_max_nof_lanes_in_ethu;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = UTILEX_MAX(dnx_data_nif.eth.nof_lanes_in_cdu_get(unit), dnx_data_nif.eth.nof_lanes_in_clu_get(unit));

    
    define->data = UTILEX_MAX(dnx_data_nif.eth.nof_lanes_in_cdu_get(unit), dnx_data_nif.eth.nof_lanes_in_clu_get(unit));

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_total_nof_cdu_lanes_in_device_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->data = dnx_data_nif.eth.cdu_nof_get(unit)*dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_cdu_mac_mode_config_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_mac_mode_config_nof;
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
jr2_a0_dnx_data_nif_eth_mac_mode_config_lanes_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_mac_mode_config_lanes_nof;
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
jr2_a0_dnx_data_nif_eth_cdu_mac_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_mac_nof;
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
jr2_a0_dnx_data_nif_eth_mac_lanes_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_mac_lanes_nof;
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
jr2_a0_dnx_data_nif_eth_ethu_logical_fifo_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_ethu_logical_fifo_nof;
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
jr2_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set(
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
jr2_a0_dnx_data_nif_eth_clu_memory_entries_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_clu_memory_entries_nof;
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
jr2_a0_dnx_data_nif_eth_priority_group_nof_entries_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_priority_group_nof_entries_min;
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
jr2_a0_dnx_data_nif_eth_priority_groups_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_priority_groups_nof;
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
jr2_a0_dnx_data_nif_eth_pad_size_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_pad_size_min;
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
jr2_a0_dnx_data_nif_eth_pad_size_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_pad_size_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 96;

    
    define->data = 96;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_packet_size_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_packet_size_max;
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
jr2_a0_dnx_data_nif_eth_an_max_nof_abilities_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_an_max_nof_abilities;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 20;

    
    define->data = 20;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_phy_map_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_phy_map_granularity;
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
jr2_a0_dnx_data_nif_eth_is_400G_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_is_400G_supported;
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
jr2_a0_dnx_data_nif_eth_start_tx_threshold_table_set(
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

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
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
        data->speed = 100000;
        data->start_thr = 8;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 400000;
        data->start_thr = 10;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_pm_properties_set(
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

    
    table->values[0].default_val = "0x0";
    table->values[1].default_val = "0x1";
    table->values[2].default_val = "0";
    table->values[3].default_val = "1";
    table->values[4].default_val = "portmodDispatchTypeCount";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_pm_properties_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_pm_properties");

    
    default_data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->phys, 0, 0x0);
    default_data->tvco_pll_index = 0x1;
    default_data->phy_addr = 0;
    default_data->is_master = 1;
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
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x000000FF);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 0;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x0000FF00);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 8;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->special_pll_check = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00FF0000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 16;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFF000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 24;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000000FF);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 32;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000FF00);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 40;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00FF0000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 48;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFF000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 56;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x000000FF);
        data->tvco_pll_index = 1;
        data->phy_addr = 64;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x0000FF00);
        data->tvco_pll_index = 1;
        data->phy_addr = 72;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0x00FF0000);
        data->tvco_pll_index = 1;
        data->phy_addr = 80;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        #if 3 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 2, 0xFF000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 88;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_ethu_properties_set(
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
        data->pms[0] = 4;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->pms[0] = 5;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->pms[0] = 6;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->pms[0] = 7;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->pms[0] = 8;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 8;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->pms[0] = 9;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 9;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->pms[0] = 10;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->pms[0] = 11;
        data->type = imbDispatchTypeImb_cdu;
        data->type_index = 11;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_phy_map_set(
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
        data->phy_index = 8;
        data->ethu_index = 1;
        data->pm_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->phy_index = 16;
        data->ethu_index = 2;
        data->pm_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->phy_index = 24;
        data->ethu_index = 3;
        data->pm_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->phy_index = 32;
        data->ethu_index = 4;
        data->pm_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->phy_index = 40;
        data->ethu_index = 5;
        data->pm_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->phy_index = 48;
        data->ethu_index = 6;
        data->pm_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->phy_index = 56;
        data->ethu_index = 7;
        data->pm_index = 7;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->phy_index = 64;
        data->ethu_index = 8;
        data->pm_index = 8;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->phy_index = 72;
        data->ethu_index = 9;
        data->pm_index = 9;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->phy_index = 80;
        data->ethu_index = 10;
        data->pm_index = 10;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->phy_index = 88;
        data->ethu_index = 11;
        data->pm_index = 11;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_max_speed_set(
    int unit)
{
    int lane_num_index;
    dnx_data_nif_eth_max_speed_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_max_speed;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_max_speed_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_max_speed");

    
    default_data = (dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    
    for (lane_num_index = 0; lane_num_index < table->keys[0].size; lane_num_index++)
    {
        data = (dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, lane_num_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 50000;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 100000;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 200000;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 400000;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_supported_interfaces_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_eth_supported_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_supported_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 43;
    table->info_get.key_size[0] = 43;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "bcmPortPhyFecInvalid";
    table->values[3].default_val = "portmodDispatchTypeCount";
    table->values[4].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_supported_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_supported_interfaces");

    
    default_data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->nof_lanes = -1;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->is_valid = 1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 1000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 5000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 12000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 40000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm4x25;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->speed = 12000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->speed = 40000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRs272;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (28 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 28, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (29 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 29, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (30 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 30, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (31 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 31, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (33 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 33, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (34 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 34, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs272;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (35 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 35, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (36 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 36, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (37 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 37, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50;
    }
    if (38 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 38, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (39 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 39, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs544_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs272_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (41 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 41, 0);
        data->speed = 400000;
        data->nof_lanes = 8;
        data->fec_type = bcmPortPhyFecRs544_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.eth.is_400G_supported_get(unit) == 1 ? TRUE : FALSE;
    }
    if (42 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 42, 0);
        data->speed = 400000;
        data->nof_lanes = 8;
        data->fec_type = bcmPortPhyFecRs272_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE && dnx_data_nif.eth.is_400G_supported_get(unit) == 1 ? TRUE : FALSE ;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_eth_supported_an_abilities_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_eth_supported_an_abilities_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_supported_an_abilities;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 28;
    table->info_get.key_size[0] = 28;

    
    table->values[0].default_val = "bcmPortAnModeNone";
    table->values[1].default_val = "0";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "bcmPortPhyFecInvalid";
    table->values[4].default_val = "BCM_PORT_MEDIUM_NONE";
    table->values[5].default_val = "bcmPortPhyChannelAll";
    table->values[6].default_val = "1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_supported_an_abilities_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_supported_an_abilities");

    
    default_data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->an_mode = bcmPortAnModeNone;
    default_data->speed = 0;
    default_data->nof_lanes = -1;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->medium = BCM_PORT_MEDIUM_NONE;
    default_data->channel = bcmPortPhyChannelAll;
    default_data->is_valid = 1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->an_mode = bcmPortAnModeCL73;
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 40000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (23 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 23, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 40000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (25 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 25, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (26 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 26, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_BACKPLANE;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }
    if (27 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_an_abilities_t *) dnxc_data_mgmt_table_data_get(unit, table, 27, 0);
        data->an_mode = bcmPortAnModeCL73BAM;
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->medium = BCM_PORT_MEDIUM_COPPER;
        data->is_valid = dnx_data_nif.eth.clu_nof_get(unit) > 0 ? TRUE : FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_nif_simulator_cdu_type_set(
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
jr2_a0_dnx_data_nif_simulator_clu_type_set(
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
jr2_a0_dnx_data_nif_flexe_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_is_supported;
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
jr2_a0_dnx_data_nif_flexe_feu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_feu_nof;
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
jr2_a0_dnx_data_nif_flexe_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_clients;
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
jr2_a0_dnx_data_nif_flexe_nof_flexe_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_clients;
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
jr2_a0_dnx_data_nif_flexe_flexe_mode_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}







static shr_error_e
jr2_a0_dnx_data_nif_prd_nof_control_frames_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_control_frames;
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
jr2_a0_dnx_data_nif_prd_nof_ether_type_codes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_ether_type_codes;
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
jr2_a0_dnx_data_nif_prd_nof_tcam_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_tcam_entries;
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
jr2_a0_dnx_data_nif_prd_nof_mpls_special_labels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_mpls_special_labels;
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
jr2_a0_dnx_data_nif_prd_nof_priorities_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_priorities;
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
jr2_a0_dnx_data_nif_prd_custom_ether_type_code_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_custom_ether_type_code_min;
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
jr2_a0_dnx_data_nif_prd_custom_ether_type_code_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_custom_ether_type_code_max;
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
jr2_a0_dnx_data_nif_prd_ether_type_code_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xf;

    
    define->data = 0xf;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_ether_type_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_ether_type_max;
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
jr2_a0_dnx_data_nif_prd_rmc_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_rmc_threshold_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x7fff;

    
    define->data = 0x7fff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_cdu_rmc_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_cdu_rmc_threshold_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.prd.rmc_threshold_max_get(unit);

    
    define->data = dnx_data_nif.prd.rmc_threshold_max_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_hrf_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_hrf_threshold_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x1fff;

    
    define->data = 0x1fff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_mpls_special_label_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_mpls_special_label_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xf;

    
    define->data = 0xf;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_flex_key_offset_key_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_flex_key_offset_key_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x7f;

    
    define->data = 0x7f;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_flex_key_offset_result_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_flex_key_offset_result_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xff;

    
    define->data = 0xff;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_tpid_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_tpid_max;
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
jr2_a0_dnx_data_nif_prd_nof_clu_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_clu_port_profiles;
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
jr2_a0_dnx_data_nif_prd_nof_feu_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_feu_port_profiles;
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
jr2_a0_dnx_data_nif_prd_ether_type_set(
    int unit)
{
    int ether_type_code_index;
    dnx_data_nif_prd_ether_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int table_index = dnx_data_nif_prd_table_ether_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 16;
    table->info_get.key_size[0] = 16;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "no name";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_prd_ether_type_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_prd_table_ether_type");

    
    default_data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->ether_type_size = 0;
    default_data->ether_type_name = "no name";
    
    for (ether_type_code_index = 0; ether_type_code_index < table->keys[0].size; ether_type_code_index++)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, ether_type_code_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "No Match";
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 1";
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 2";
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 3";
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 4";
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 5";
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 6";
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Configurable 7";
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->ether_type_size = 6;
        data->ether_type_name = "TRILL";
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "Control Frame";
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->ether_type_size = 28;
        data->ether_type_name = "ARP";
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->ether_type_size = 4;
        data->ether_type_name = "CFM";
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->ether_type_size = 36;
        data->ether_type_name = "FCoE";
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->ether_type_size = 20;
        data->ether_type_name = "IPv4";
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->ether_type_size = 40;
        data->ether_type_name = "IPv6";
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->ether_type_size = 0;
        data->ether_type_name = "MPLS";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_prd_port_profile_map_set(
    int unit)
{
    int type_index;
    dnx_data_nif_prd_port_profile_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int table_index = dnx_data_nif_prd_table_port_profile_map;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = imbDispatchTypeCount;
    table->info_get.key_size[0] = imbDispatchTypeCount;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_prd_port_profile_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_prd_table_port_profile_map");

    
    default_data = (dnx_data_nif_prd_port_profile_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_supported = 0;
    default_data->nof_profiles = 0;
    
    for (type_index = 0; type_index < table->keys[0].size; type_index++)
    {
        data = (dnx_data_nif_prd_port_profile_map_t *) dnxc_data_mgmt_table_data_get(unit, table, type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (imbDispatchTypeImb_clu < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_port_profile_map_t *) dnxc_data_mgmt_table_data_get(unit, table, imbDispatchTypeImb_clu, 0);
        data->is_supported = TRUE;
        data->nof_profiles = dnx_data_nif.prd.nof_clu_port_profiles_get(unit);
    }
    if (imbDispatchTypeImb_feu < table->keys[0].size)
    {
        data = (dnx_data_nif_prd_port_profile_map_t *) dnxc_data_mgmt_table_data_get(unit, table, imbDispatchTypeImb_feu, 0);
        data->is_supported = TRUE;
        data->nof_profiles = dnx_data_nif.prd.nof_feu_port_profiles_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_nif_portmod_pm_types_nof_set(
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
jr2_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set(
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
        data->type = portmodDispatchTypePmOsILKN_50G;
        data->instances = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->type = portmodDispatchTypePm8x50_fabric;
        data->instances = dnx_data_fabric.blocks.nof_pms_get(unit);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
jr2_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set(
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
jr2_a0_dnx_data_nif_scheduler_rate_per_ethu_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int define_index = dnx_data_nif_scheduler_define_rate_per_ethu_bit;
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
jr2_a0_dnx_data_nif_scheduler_rate_per_ilu_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int define_index = dnx_data_nif_scheduler_define_rate_per_ilu_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 150000;

    
    define->data = 150000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int define_index = dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 200000/dnx_data_nif.scheduler.nof_rmc_bits_get(unit);

    
    define->data = 200000/dnx_data_nif.scheduler.nof_rmc_bits_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_scheduler_rate_per_clu_rmc_bit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int define_index = dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 25000;

    
    define->data = 25000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_nif_scheduler_nof_weight_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int define_index = dnx_data_nif_scheduler_define_nof_weight_bits;
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
jr2_a0_dnx_data_nif_scheduler_nof_rmc_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_scheduler;
    int define_index = dnx_data_nif_scheduler_define_nof_rmc_bits;
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
jr2_a0_dnx_data_nif_dbal_active_mac_configure_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int feature_index = dnx_data_nif_dbal_active_mac_configure;
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
jr2_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set(
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
jr2_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
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
jr2_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
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
jr2_a0_dnx_data_nif_dbal_tx_credits_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_tx_credits_bits;
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
jr2_a0_dnx_data_nif_features_pm_mode_0122_invalid_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_features;
    int feature_index = dnx_data_nif_features_pm_mode_0122_invalid;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 1;

    
    feature->data = 1;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}





shr_error_e
jr2_a0_data_nif_attach(
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
    define->set = jr2_a0_dnx_data_nif_global_nof_lcplls_set;
    data_index = dnx_data_nif_global_define_max_core_access_per_port;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_global_max_core_access_per_port_set;
    data_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set;
    data_index = dnx_data_nif_global_define_last_port_led_scan;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_global_last_port_led_scan_set;
    data_index = dnx_data_nif_global_define_start_tx_threshold_global;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_global_start_tx_threshold_global_set;

    

    
    data_index = dnx_data_nif_global_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_global_pll_phys_set;
    
    submodule_index = dnx_data_nif_submodule_phys;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_phys_define_nof_phys;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_phys_nof_phys_set;
    data_index = dnx_data_nif_phys_define_nof_phys_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_phys_nof_phys_per_core_set;
    data_index = dnx_data_nif_phys_define_pm8x50_gen;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_phys_pm8x50_gen_set;
    data_index = dnx_data_nif_phys_define_is_pam4_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_phys_is_pam4_speed_supported_set;

    

    
    data_index = dnx_data_nif_phys_table_general;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_phys_general_set;
    data_index = dnx_data_nif_phys_table_polarity;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_phys_polarity_set;
    data_index = dnx_data_nif_phys_table_core_phys_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_phys_core_phys_map_set;
    data_index = dnx_data_nif_phys_table_vco_div;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_phys_vco_div_set;
    
    submodule_index = dnx_data_nif_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ilkn_define_ilu_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilu_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_if_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilkn_if_nof_set;
    data_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set;
    data_index = dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_nof_lanes_per_fec_unit_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_lanes_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set;
    data_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set;
    data_index = dnx_data_nif_ilkn_define_segments_max_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_segments_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_segments_half_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_segments_half_nof_set;
    data_index = dnx_data_nif_ilkn_define_pms_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_pms_nof_set;
    data_index = dnx_data_nif_ilkn_define_fmac_bus_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_fmac_bus_size_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_tx_hrf_credits_set;
    data_index = dnx_data_nif_ilkn_define_nof_rx_hrf_per_port;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_nof_rx_hrf_per_port_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_elk;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_watermark_high_elk_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_elk;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_watermark_low_elk_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_data;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_watermark_high_data_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_data;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_watermark_low_data_set;
    data_index = dnx_data_nif_ilkn_define_is_20G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_20G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_25G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_27G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_27G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_28G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_28G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_53G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_53G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_nif_56G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_nif_56G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_is_fabric_56G_speed_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_is_fabric_56G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_define_max_nof_ifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_max_nof_ifs_set;
    data_index = dnx_data_nif_ilkn_define_max_nof_elk_ifs;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_max_nof_elk_ifs_set;
    data_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set;

    
    data_index = dnx_data_nif_ilkn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_ilkn_is_supported_set;
    data_index = dnx_data_nif_ilkn_is_ilu_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_ilkn_is_ilu_supported_set;
    data_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set;
    data_index = dnx_data_nif_ilkn_is_fec_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_ilkn_is_fec_supported_set;

    
    data_index = dnx_data_nif_ilkn_table_phys;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_phys_set;
    data_index = dnx_data_nif_ilkn_table_supported_phys;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_supported_phys_set;
    data_index = dnx_data_nif_ilkn_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_supported_interfaces_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_ilkn_cores_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores_clup_facing;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_ilkn_cores_clup_facing_set;
    data_index = dnx_data_nif_ilkn_table_supported_device_core;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_supported_device_core_set;
    data_index = dnx_data_nif_ilkn_table_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_properties_set;
    data_index = dnx_data_nif_ilkn_table_nif_pms;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_nif_pms_set;
    data_index = dnx_data_nif_ilkn_table_fabric_pms;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_fabric_pms_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_pms;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_ilkn_pms_set;
    data_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_nif_lanes_map_set;
    data_index = dnx_data_nif_ilkn_table_fabric_lanes_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_ilkn_fabric_lanes_map_set;
    
    submodule_index = dnx_data_nif_submodule_eth;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_eth_define_cdu_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_cdu_nof_set;
    data_index = dnx_data_nif_eth_define_clu_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_clu_nof_set;
    data_index = dnx_data_nif_eth_define_cdum_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_cdum_nof_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_pms_in_cdu_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_clu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_pms_in_clu_set;
    data_index = dnx_data_nif_eth_define_total_nof_ethu_pms_in_device;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_total_nof_ethu_pms_in_device_set;
    data_index = dnx_data_nif_eth_define_nof_cdu_lanes_in_pm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_cdu_lanes_in_pm_set;
    data_index = dnx_data_nif_eth_define_nof_clu_lanes_in_pm;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_clu_lanes_in_pm_set;
    data_index = dnx_data_nif_eth_define_nof_cdu_pms;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_cdu_pms_set;
    data_index = dnx_data_nif_eth_define_nof_clu_pms;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_clu_pms_set;
    data_index = dnx_data_nif_eth_define_ethu_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_ethu_nof_set;
    data_index = dnx_data_nif_eth_define_ethu_nof_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_ethu_nof_per_core_set;
    data_index = dnx_data_nif_eth_define_cdu_nof_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_cdu_nof_per_core_set;
    data_index = dnx_data_nif_eth_define_clu_nof_per_core;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_clu_nof_per_core_set;
    data_index = dnx_data_nif_eth_define_nof_lanes_in_cdu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_lanes_in_cdu_set;
    data_index = dnx_data_nif_eth_define_nof_lanes_in_clu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_nof_lanes_in_clu_set;
    data_index = dnx_data_nif_eth_define_max_nof_lanes_in_ethu;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_max_nof_lanes_in_ethu_set;
    data_index = dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_total_nof_cdu_lanes_in_device_set;
    data_index = dnx_data_nif_eth_define_cdu_mac_mode_config_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_cdu_mac_mode_config_nof_set;
    data_index = dnx_data_nif_eth_define_mac_mode_config_lanes_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_mac_mode_config_lanes_nof_set;
    data_index = dnx_data_nif_eth_define_cdu_mac_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_cdu_mac_nof_set;
    data_index = dnx_data_nif_eth_define_mac_lanes_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_mac_lanes_nof_set;
    data_index = dnx_data_nif_eth_define_ethu_logical_fifo_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_ethu_logical_fifo_nof_set;
    data_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_clu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_clu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_priority_group_nof_entries_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_priority_group_nof_entries_min_set;
    data_index = dnx_data_nif_eth_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_priority_groups_nof_set;
    data_index = dnx_data_nif_eth_define_pad_size_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_pad_size_min_set;
    data_index = dnx_data_nif_eth_define_pad_size_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_pad_size_max_set;
    data_index = dnx_data_nif_eth_define_packet_size_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_packet_size_max_set;
    data_index = dnx_data_nif_eth_define_an_max_nof_abilities;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_an_max_nof_abilities_set;
    data_index = dnx_data_nif_eth_define_phy_map_granularity;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_phy_map_granularity_set;
    data_index = dnx_data_nif_eth_define_is_400G_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_eth_is_400G_supported_set;

    
    data_index = dnx_data_nif_eth_nif_scheduler_context_per_rmc_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_nif_scheduler_context_per_rmc_support_set;
    data_index = dnx_data_nif_eth_is_clu_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_is_clu_supported_set;
    data_index = dnx_data_nif_eth_is_rx_port_mode_needs_configuration;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_is_rx_port_mode_needs_configuration_set;
    data_index = dnx_data_nif_eth_pcs_lane_map_reconfig_support;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_pcs_lane_map_reconfig_support_set;
    data_index = dnx_data_nif_eth_gear_shifter_exists;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_gear_shifter_exists_set;
    data_index = dnx_data_nif_eth_pm8x50_bw_update;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_pm8x50_bw_update_set;
    data_index = dnx_data_nif_eth_portmod_thread_disable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_eth_portmod_thread_disable_set;

    
    data_index = dnx_data_nif_eth_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_start_tx_threshold_table_set;
    data_index = dnx_data_nif_eth_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_pm_properties_set;
    data_index = dnx_data_nif_eth_table_ethu_properties;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_ethu_properties_set;
    data_index = dnx_data_nif_eth_table_phy_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_phy_map_set;
    data_index = dnx_data_nif_eth_table_max_speed;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_max_speed_set;
    data_index = dnx_data_nif_eth_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_supported_interfaces_set;
    data_index = dnx_data_nif_eth_table_supported_an_abilities;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_eth_supported_an_abilities_set;
    
    submodule_index = dnx_data_nif_submodule_simulator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_simulator_define_cdu_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_simulator_cdu_type_set;
    data_index = dnx_data_nif_simulator_define_clu_type;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_simulator_clu_type_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_flexe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flexe_define_feu_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_flexe_feu_nof_set;
    data_index = dnx_data_nif_flexe_define_nof_clients;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_flexe_nof_clients_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_clients;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_flexe_nof_flexe_clients_set;
    data_index = dnx_data_nif_flexe_define_flexe_mode;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_flexe_flexe_mode_set;

    
    data_index = dnx_data_nif_flexe_is_supported;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_flexe_is_supported_set;

    
    
    submodule_index = dnx_data_nif_submodule_prd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_prd_define_nof_control_frames;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_control_frames_set;
    data_index = dnx_data_nif_prd_define_nof_ether_type_codes;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_ether_type_codes_set;
    data_index = dnx_data_nif_prd_define_nof_tcam_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_tcam_entries_set;
    data_index = dnx_data_nif_prd_define_nof_mpls_special_labels;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_mpls_special_labels_set;
    data_index = dnx_data_nif_prd_define_nof_priorities;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_priorities_set;
    data_index = dnx_data_nif_prd_define_custom_ether_type_code_min;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_custom_ether_type_code_min_set;
    data_index = dnx_data_nif_prd_define_custom_ether_type_code_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_custom_ether_type_code_max_set;
    data_index = dnx_data_nif_prd_define_ether_type_code_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_ether_type_code_max_set;
    data_index = dnx_data_nif_prd_define_ether_type_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_ether_type_max_set;
    data_index = dnx_data_nif_prd_define_rmc_threshold_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_rmc_threshold_max_set;
    data_index = dnx_data_nif_prd_define_cdu_rmc_threshold_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_cdu_rmc_threshold_max_set;
    data_index = dnx_data_nif_prd_define_hrf_threshold_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_hrf_threshold_max_set;
    data_index = dnx_data_nif_prd_define_mpls_special_label_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_mpls_special_label_max_set;
    data_index = dnx_data_nif_prd_define_flex_key_offset_key_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_flex_key_offset_key_max_set;
    data_index = dnx_data_nif_prd_define_flex_key_offset_result_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_flex_key_offset_result_max_set;
    data_index = dnx_data_nif_prd_define_tpid_max;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_tpid_max_set;
    data_index = dnx_data_nif_prd_define_nof_clu_port_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_clu_port_profiles_set;
    data_index = dnx_data_nif_prd_define_nof_feu_port_profiles;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_prd_nof_feu_port_profiles_set;

    

    
    data_index = dnx_data_nif_prd_table_ether_type;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_prd_ether_type_set;
    data_index = dnx_data_nif_prd_table_port_profile_map;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_prd_port_profile_map_set;
    
    submodule_index = dnx_data_nif_submodule_portmod;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_portmod_define_pm_types_nof;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_portmod_pm_types_nof_set;

    

    
    data_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_scheduler;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_scheduler_define_rate_per_ethu_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_scheduler_rate_per_ethu_bit_set;
    data_index = dnx_data_nif_scheduler_define_rate_per_ilu_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_scheduler_rate_per_ilu_bit_set;
    data_index = dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_set;
    data_index = dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_scheduler_rate_per_clu_rmc_bit_set;
    data_index = dnx_data_nif_scheduler_define_nof_weight_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_scheduler_nof_weight_bits_set;
    data_index = dnx_data_nif_scheduler_define_nof_rmc_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_scheduler_nof_rmc_bits_set;

    
    data_index = dnx_data_nif_scheduler_decrement_credit_on_read_enable;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set;

    
    
    submodule_index = dnx_data_nif_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set;
    data_index = dnx_data_nif_dbal_define_tx_credits_bits;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_nif_dbal_tx_credits_bits_set;

    
    data_index = dnx_data_nif_dbal_active_mac_configure;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_dbal_active_mac_configure_set;

    
    
    submodule_index = dnx_data_nif_submodule_features;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_features_pm_mode_0122_invalid;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_nif_features_pm_mode_0122_invalid_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


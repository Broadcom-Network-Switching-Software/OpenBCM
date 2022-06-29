
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_nif.h>
#include <soc/dnx/intr/auto_generated/j2x/j2x_intr.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <include/bcm_int/dnx/port/port.h>







static shr_error_e
j2x_a0_dnx_data_nif_global_reassembler_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_reassembler;
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
j2x_a0_dnx_data_nif_global_clock_power_down_set(
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
j2x_a0_dnx_data_nif_global_is_shared_arch_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_is_shared_arch;
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
j2x_a0_dnx_data_nif_global_is_ilkn_after_flexe_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_is_ilkn_after_flexe;
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
j2x_a0_dnx_data_nif_global_support_xpmd_if_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_support_xpmd_if;
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
j2x_a0_dnx_data_nif_global_is_g_hao_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_is_g_hao_supported;
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
j2x_a0_dnx_data_nif_global_bh_2x2_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_bh_2x2_supported;
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
j2x_a0_dnx_data_nif_global_eth_use_framer_mgmt_bw_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int feature_index = dnx_data_nif_global_eth_use_framer_mgmt_bw;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "eth_use_framer_mgmt_bw";
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_global_nof_lcplls_set(
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
j2x_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 386;

    
    define->data = 386;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
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
j2x_a0_dnx_data_nif_global_nof_nif_units_per_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_nof_nif_units_per_core;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.eth.max_ethu_nof_per_core_get(unit) + 1 + dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

    
    define->data = dnx_data_nif.eth.max_ethu_nof_per_core_get(unit) + 1 + dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_last_port_led_scan_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_last_port_led_scan;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 56;

    
    define->data = 56;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_reassembler_fifo_threshold_set(
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
j2x_a0_dnx_data_nif_global_total_data_capacity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_total_data_capacity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1600000;

    
    define->data = 1600000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_single_mgmt_port_capacity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_single_mgmt_port_capacity;
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
j2x_a0_dnx_data_nif_global_ghao_speed_limit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_ghao_speed_limit;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1200000;

    
    define->data = 1200000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_l1_only_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_l1_only_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    define->data = DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_L1_ONLY_MODE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 3;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "DISABLED";
    define->property.mapping[0].val = DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;
    define->property.mapping[0].is_default = 1 ;
    define->property.mapping[1].name = "L1_ONLY_WITHOUT_ILKN";
    define->property.mapping[1].val = DNX_PORT_NIF_L1_ONLY_MODE_WITHOUT_ILKN;
    define->property.mapping[2].name = "L1_ONLY_WITH_ILKN";
    define->property.mapping[2].val = DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_otn_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_global;
    int define_index = dnx_data_nif_global_define_otn_supported;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_OTN_SUPPORTED;
    define->property.doc = 
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
j2x_a0_dnx_data_nif_global_pll_phys_set(
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
        data->max_phy_id = 63;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set(
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
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->unit_id = 1;
        data->internal_offset = 0;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->unit_id = 2;
        data->internal_offset = 0;
    }
    if (24 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 24, 0);
        data->unit_id = 3;
        data->internal_offset = 0;
    }
    if (32 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 32, 0);
        data->unit_id = 4;
        data->internal_offset = 0;
    }
    if (40 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 40, 0);
        data->unit_id = 5;
        data->internal_offset = 0;
    }
    if (48 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 48, 0);
        data->unit_id = 6;
        data->internal_offset = 0;
    }
    if (56 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 56, 0);
        data->unit_id = 7;
        data->internal_offset = 0;
    }
    if (64 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 64, 0);
        data->unit_id = 8;
        data->internal_offset = 0;
    }
    if (72 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 72, 0);
        data->unit_id = 8;
        data->internal_offset = 8;
    }
    if (80 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 80, 0);
        data->unit_id = 8;
        data->internal_offset = 16;
    }
    if (88 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 88, 0);
        data->unit_id = 8;
        data->internal_offset = 24;
    }
    if (96 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 96, 0);
        data->unit_id = 8;
        data->internal_offset = 32;
    }
    if (104 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 104, 0);
        data->unit_id = 8;
        data->internal_offset = 40;
    }
    if (112 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 112, 0);
        data->unit_id = 8;
        data->internal_offset = 48;
    }
    if (120 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 120, 0);
        data->unit_id = 8;
        data->internal_offset = 56;
    }
    if (128 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 128, 0);
        data->unit_id = 8;
        data->internal_offset = 64;
    }
    if (136 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 136, 0);
        data->unit_id = 8;
        data->internal_offset = 72;
    }
    if (144 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 144, 0);
        data->unit_id = 8;
        data->internal_offset = 80;
    }
    if (152 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 152, 0);
        data->unit_id = 8;
        data->internal_offset = 88;
    }
    if (160 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 160, 0);
        data->unit_id = 8;
        data->internal_offset = 96;
    }
    if (168 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 168, 0);
        data->unit_id = 8;
        data->internal_offset = 104;
    }
    if (176 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 176, 0);
        data->unit_id = 8;
        data->internal_offset = 112;
    }
    if (184 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 184, 0);
        data->unit_id = 8;
        data->internal_offset = 120;
    }
    if (192 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 192, 0);
        data->unit_id = 8;
        data->internal_offset = 128;
    }
    if (200 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 200, 0);
        data->unit_id = 8;
        data->internal_offset = 136;
    }
    if (208 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 208, 0);
        data->unit_id = 8;
        data->internal_offset = 144;
    }
    if (216 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 216, 0);
        data->unit_id = 8;
        data->internal_offset = 152;
    }
    if (224 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 224, 0);
        data->unit_id = 8;
        data->internal_offset = 160;
    }
    if (232 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 232, 0);
        data->unit_id = 8;
        data->internal_offset = 168;
    }
    if (240 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 240, 0);
        data->unit_id = 8;
        data->internal_offset = 176;
    }
    if (248 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 248, 0);
        data->unit_id = 8;
        data->internal_offset = 184;
    }
    if (256 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 256, 0);
        data->unit_id = 8;
        data->internal_offset = 192;
    }
    if (264 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 264, 0);
        data->unit_id = 8;
        data->internal_offset = 200;
    }
    if (272 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 272, 0);
        data->unit_id = 8;
        data->internal_offset = 208;
    }
    if (280 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 280, 0);
        data->unit_id = 8;
        data->internal_offset = 216;
    }
    if (288 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 288, 0);
        data->unit_id = 8;
        data->internal_offset = 224;
    }
    if (296 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 296, 0);
        data->unit_id = 8;
        data->internal_offset = 232;
    }
    if (304 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 304, 0);
        data->unit_id = 8;
        data->internal_offset = 240;
    }
    if (312 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 312, 0);
        data->unit_id = 8;
        data->internal_offset = 248;
    }
    if (320 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 320, 0);
        data->unit_id = 8;
        data->internal_offset = 256;
    }
    if (328 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 328, 0);
        data->unit_id = 8;
        data->internal_offset = 264;
    }
    if (336 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 336, 0);
        data->unit_id = 8;
        data->internal_offset = 272;
    }
    if (344 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 344, 0);
        data->unit_id = 8;
        data->internal_offset = 280;
    }
    if (352 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 352, 0);
        data->unit_id = 8;
        data->internal_offset = 288;
    }
    if (360 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 360, 0);
        data->unit_id = 8;
        data->internal_offset = 296;
    }
    if (368 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 368, 0);
        data->unit_id = 8;
        data->internal_offset = 304;
    }
    if (376 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 376, 0);
        data->unit_id = 8;
        data->internal_offset = 312;
    }
    if (384 < table->keys[0].size)
    {
        data = (dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_get(unit, table, 384, 0);
        data->unit_id = 9;
        data->internal_offset = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_phys_txpi_supported_in_pm8x50_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int feature_index = dnx_data_nif_phys_txpi_supported_in_pm8x50;
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
j2x_a0_dnx_data_nif_phys_nof_phys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_nof_phys;
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
j2x_a0_dnx_data_nif_phys_max_phys_in_core_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_max_phys_in_core;
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
j2x_a0_dnx_data_nif_phys_pm8x50_gen_set(
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
j2x_a0_dnx_data_nif_phys_is_pam4_speed_supported_set(
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
j2x_a0_dnx_data_nif_phys_txpi_sdm_scheme_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int define_index = dnx_data_nif_phys_define_txpi_sdm_scheme;
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
j2x_a0_dnx_data_nif_phys_general_set(
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

    
    table->values[0].default_val = "0xFFFFFFFF,0xFFFFFFFF";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_general_t, (1 + 1  ), "data of dnx_data_nif_phys_table_general");

    
    default_data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    #if 2 > _SHR_PBMP_WORD_MAX
    #error "out of bound access to array"
    #endif
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 0, 0xFFFFFFFF);
    _SHR_PBMP_WORD_SET(default_data->supported_phys, 1, 0xFFFFFFFF);
    
    data = (dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_core_phys_map_set(
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
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFFFFFFFF);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_vco_div_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_nof_phys_per_core_set(
    int unit)
{
    int core_index_index;
    dnx_data_nif_phys_nof_phys_per_core_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_nof_phys_per_core;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_nof_phys_per_core_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_nof_phys_per_core");

    
    default_data = (dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_phys = 0;
    default_data->first_phy = 0;
    
    for (core_index_index = 0; core_index_index < table->keys[0].size; core_index_index++)
    {
        data = (dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, core_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_phys = 64;
        data->first_phy = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_phys_txpi_sdm_div_set(
    int unit)
{
    int index_index;
    dnx_data_nif_phys_txpi_sdm_div_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_phys;
    int table_index = dnx_data_nif_phys_table_txpi_sdm_div;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_phys_txpi_sdm_div_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_phys_table_txpi_sdm_div");

    
    default_data = (dnx_data_nif_phys_txpi_sdm_div_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->val = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_phys_txpi_sdm_div_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_txpi_sdm_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10000;
        data->val = 32*256;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_phys_txpi_sdm_div_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 25000;
        data->val = 64*256;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_ilkn_is_supported_set(
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
j2x_a0_dnx_data_nif_ilkn_is_elk_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_elk_supported;
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
j2x_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set(
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
j2x_a0_dnx_data_nif_ilkn_frmr_ilkn_selector_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_frmr_ilkn_selector;
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
j2x_a0_dnx_data_nif_ilkn_support_burst_interleaving_set(
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
j2x_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set(
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
j2x_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set(
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
j2x_a0_dnx_data_nif_ilkn_is_fec_supported_set(
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
j2x_a0_dnx_data_nif_ilkn_legacy_lane_format_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_legacy_lane_format;
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
j2x_a0_dnx_data_nif_ilkn_is_20p6G_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_20p6G_speed_supported;
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
j2x_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_25G_speed_supported;
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
j2x_a0_dnx_data_nif_ilkn_is_28p1G_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int feature_index = dnx_data_nif_ilkn_is_28p1G_speed_supported;
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
j2x_a0_dnx_data_nif_ilkn_ilu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilu_nof;
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
j2x_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
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
j2x_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
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
j2x_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
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
j2x_a0_dnx_data_nif_ilkn_segments_max_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_segments_half_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_segments_half_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ilkn.segments_max_nof_get(unit)/2;

    
    define->data = dnx_data_nif.ilkn.segments_max_nof_get(unit)/2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_pms_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_fmac_bus_size_set(
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
j2x_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set(
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
j2x_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set(
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
j2x_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set(
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
j2x_a0_dnx_data_nif_ilkn_tx_hrf_credits_set(
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
j2x_a0_dnx_data_nif_ilkn_watermark_high_data_set(
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
j2x_a0_dnx_data_nif_ilkn_watermark_low_data_set(
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
j2x_a0_dnx_data_nif_ilkn_pad_size_set(
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
j2x_a0_dnx_data_nif_ilkn_burst_max_range_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_burst_max_range_max;
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
j2x_a0_dnx_data_nif_ilkn_burst_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_burst_min;
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
j2x_a0_dnx_data_nif_ilkn_burst_min_range_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_burst_min_range_max;
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
j2x_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2X_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->data = J2X_INT_ILE_ILKN_RX_PORT_0_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = J2X_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->data = J2X_INT_ILE_ILKN_RX_PORT_1_STATUS_CHANGE_INT;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_interleaved_error_drop_single_context_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ilkn;
    int define_index = dnx_data_nif_ilkn_define_interleaved_error_drop_single_context;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "interleaved_error_drop_single_context";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_ilkn_supported_phys_set(
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
        data->max_phys = 24;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->is_supported = 1;
        data->max_phys = 12;
        _SHR_PBMP_WORD_SET(data->nif_phys, 0, 0x00ffffff);
        _SHR_PBMP_WORD_SET(data->fabric_phys, 0, 0x0);
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_supported_interfaces_set(
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

    
    table->keys[0].size = 7;
    table->info_get.key_size[0] = 7;

    
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
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_10p3G_speed_supported) && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only);
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 12500;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_12p5G_speed_supported) && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only);
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 20625;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_20p6G_speed_supported) > 0 ? TRUE : FALSE;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 25000;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_25G_speed_supported) > 0 ? TRUE : FALSE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_25p7G_speed_supported) && !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only);
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 27343;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_27p3G_speed_supported) > 0 ? TRUE : FALSE;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 28125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->is_valid = !dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_ilkn_over_fabric_only) && dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_is_28p1G_speed_supported) > 0 ? TRUE : FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_ilkn_cores_set(
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

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_ilkn_cores_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_ilkn_cores");

    
    default_data = (dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_elk_supported = 0;
    default_data->is_data_supported = 0;
    default_data->fec_disable_by_bypass = 0;
    default_data->fec_dependency = 0;
    
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_supported_device_core_set(
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
j2x_a0_dnx_data_nif_ilkn_nif_pms_set(
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

    
    table->keys[0].size = 8;
    table->info_get.key_size[0] = 8;

    
    table->values[0].default_val = "portmodDispatchTypeCount";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "-1";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_ilkn_nif_pms_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_ilkn_table_nif_pms");

    
    default_data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->dispatch_type = portmodDispatchTypeCount;
    default_data->first_phy = -1;
    default_data->nof_phys = -1;
    default_data->facing_core_index = 0;
    
    for (pm_id_index = 0; pm_id_index < table->keys[0].size; pm_id_index++)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->first_phy = 0;
        data->nof_phys = 8;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->first_phy = 8;
        data->nof_phys = 8;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->first_phy = 16;
        data->nof_phys = 8;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_nif_lanes_map_set(
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

    
    table->keys[0].size = 1;
    table->info_get.key_size[0] = 1;
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set(
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
        data->start_thr = 16;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_eth_is_rmc_allocation_needed_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_is_rmc_allocation_needed;
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
j2x_a0_dnx_data_nif_eth_gear_shifter_exists_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int feature_index = dnx_data_nif_eth_gear_shifter_exists;
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
j2x_a0_dnx_data_nif_eth_cdu_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_cdu_nof;
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
j2x_a0_dnx_data_nif_eth_clu_nof_set(
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
j2x_a0_dnx_data_nif_eth_cdum_nof_set(
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
j2x_a0_dnx_data_nif_eth_nof_pms_in_cdu_set(
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
j2x_a0_dnx_data_nif_eth_nof_lanes_in_cdu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_nof_lanes_in_cdu;
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
j2x_a0_dnx_data_nif_eth_cdu_mac_nof_set(
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
j2x_a0_dnx_data_nif_eth_mac_lanes_nof_set(
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
j2x_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set(
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
j2x_a0_dnx_data_nif_eth_priority_group_nof_entries_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int define_index = dnx_data_nif_eth_define_priority_group_nof_entries_min;
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
j2x_a0_dnx_data_nif_eth_priority_groups_nof_set(
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
j2x_a0_dnx_data_nif_eth_phy_map_granularity_set(
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
j2x_a0_dnx_data_nif_eth_pm_properties_set(
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
    table->values[6].default_val = "1";
    table->values[7].default_val = "1";
    table->values[8].default_val = "0";
    
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
    default_data->flr_support = 1;
    default_data->ext_txpi_support = 1;
    default_data->link_recovery_support = 0;
    
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
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
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
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00FF0000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 16;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0xFF000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 24;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x000000FF);
        data->tvco_pll_index = 1;
        data->phy_addr = 32;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x0000FF00);
        data->tvco_pll_index = 1;
        data->phy_addr = 40;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0x00FF0000);
        data->tvco_pll_index = 1;
        data->phy_addr = 48;
        data->is_master = 1;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        #if 2 > _SHR_PBMP_WORD_MAX
        #error "out of bound access to array"
        #endif
        _SHR_PBMP_WORD_SET(data->phys, 0, 0x00000000);
        _SHR_PBMP_WORD_SET(data->phys, 1, 0xFF000000);
        data->tvco_pll_index = 1;
        data->phy_addr = 56;
        data->is_master = 0;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_ethu_properties_set(
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
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->pms[0] = 1;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->pms[0] = 2;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->pms[0] = 3;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->pms[0] = 4;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 4;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->pms[0] = 5;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 5;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->pms[0] = 6;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 6;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->pms[0] = 7;
        data->type = imbDispatchTypeImb_cdu_shr;
        data->type_index = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_phy_map_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_supported_interfaces_set(
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

    
    table->keys[0].size = 23;
    table->info_get.key_size[0] = 23;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "-1";
    table->values[2].default_val = "bcmPortPhyFecInvalid";
    table->values[3].default_val = "portmodDispatchTypeCount";
    table->values[4].default_val = "1";
    table->values[5].default_val = "1";
    table->values[6].default_val = "1";
    
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
    default_data->ptp_support = 1;
    default_data->autoneg_support = 1;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 10000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = FALSE;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = FALSE;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 25000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 40000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecBaseR;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->ptp_support = FALSE;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 50000;
        data->nof_lanes = 1;
        data->fec_type = bcmPortPhyFecRs272;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 50000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 100000;
        data->nof_lanes = 2;
        data->fec_type = bcmPortPhyFecRs272;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecNone;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRsFec;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (18 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 18, 0);
        data->speed = 100000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs544;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
    }
    if (19 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 19, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs544_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) > 0 ? TRUE : FALSE;
    }
    if (20 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 20, 0);
        data->speed = 200000;
        data->nof_lanes = 4;
        data->fec_type = bcmPortPhyFecRs272_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }
    if (21 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 21, 0);
        data->speed = 400000;
        data->nof_lanes = 8;
        data->fec_type = bcmPortPhyFecRs544_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.eth.is_400G_supported_get(unit) == 1 ? TRUE : FALSE;
    }
    if (22 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 22, 0);
        data->speed = 400000;
        data->nof_lanes = 8;
        data->fec_type = bcmPortPhyFecRs272_2xN;
        data->dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->is_valid = dnx_data_nif.phys.is_pam4_speed_supported_get(unit) && dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE && dnx_data_nif.eth.is_400G_supported_get(unit) == 1 ? TRUE : FALSE ;
        data->ptp_support = dnx_data_nif.phys.pm8x50_gen_get(unit) > 1 ? TRUE : FALSE;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_ethu_per_core_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_nif_eth_ethu_per_core_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_ethu_per_core;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_nif.eth.ethu_nof_get(unit);
    table->info_get.key_size[0] = dnx_data_nif.eth.ethu_nof_get(unit);

    
    table->values[0].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_ethu_per_core_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_ethu_per_core");

    
    default_data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->core_id = -1;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->core_id = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->core_id = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->core_id = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->core_id = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->core_id = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->core_id = 0;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->core_id = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->core_id = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_eth_nif_cores_ethus_set(
    int unit)
{
    int core_id_index;
    dnx_data_nif_eth_nif_cores_ethus_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_eth;
    int table_index = dnx_data_nif_eth_table_nif_cores_ethus;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_device.general.nof_cores_get(unit);
    table->info_get.key_size[0] = dnx_data_device.general.nof_cores_get(unit);

    
    table->values[0].default_val = "DATA(nif,eth,max_ethu_nof_per_core)";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_eth_nif_cores_ethus_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_eth_table_nif_cores_ethus");

    
    default_data = (dnx_data_nif_eth_nif_cores_ethus_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_ethus = dnx_data_nif.eth.max_ethu_nof_per_core_get(unit);
    default_data->first_ethu = -1;
    
    for (core_id_index = 0; core_id_index < table->keys[0].size; core_id_index++)
    {
        data = (dnx_data_nif_eth_nif_cores_ethus_t *) dnxc_data_mgmt_table_data_get(unit, table, core_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_eth_nif_cores_ethus_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_ethus = 8;
        data->first_ethu = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_simulator_cdu_type_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_simulator;
    int define_index = dnx_data_nif_simulator_define_cdu_type;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = phymodDispatchTypeTscbh_fe_gen2;

    
    define->data = phymodDispatchTypeTscbh_fe_gen2;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_framer_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_is_supported;
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
j2x_a0_dnx_data_nif_framer_disable_l1_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_disable_l1;
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
j2x_a0_dnx_data_nif_framer_is_clock_independent_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_is_clock_independent;
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
j2x_a0_dnx_data_nif_framer_l1_eth_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_l1_eth_supported;
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
j2x_a0_dnx_data_nif_framer_eth_use_flexe_group_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_eth_use_flexe_group;
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
j2x_a0_dnx_data_nif_framer_embeded_mac_in_busa_busc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_embeded_mac_in_busa_busc;
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
j2x_a0_dnx_data_nif_framer_power_down_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int feature_index = dnx_data_nif_framer_power_down_supported;
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
j2x_a0_dnx_data_nif_framer_nof_units_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_nof_units;
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
j2x_a0_dnx_data_nif_framer_b66switch_backup_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_b66switch_backup_channels;
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
j2x_a0_dnx_data_nif_framer_gen_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_gen;
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
j2x_a0_dnx_data_nif_framer_nof_mgmt_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_nof_mgmt_channels;
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
j2x_a0_dnx_data_nif_framer_clock_mhz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_clock_mhz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1152;

    
    define->data = 1152;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_framer_ptp_channel_max_bandwidth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_ptp_channel_max_bandwidth;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000;

    
    define->data = 10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_framer_oam_channel_max_bandwidth_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_oam_channel_max_bandwidth;
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
j2x_a0_dnx_data_nif_framer_nof_mac1_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_nof_mac1_channels;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_framer_nof_mac2_channels_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_nof_mac2_channels;
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
j2x_a0_dnx_data_nif_framer_nof_pms_per_framer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int define_index = dnx_data_nif_framer_define_nof_pms_per_framer;
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
j2x_a0_dnx_data_nif_framer_b66switch_set(
    int unit)
{
    int port_type_index;
    dnx_data_nif_framer_b66switch_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int table_index = dnx_data_nif_framer_table_b66switch;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_ALGO_PORT_TYPE_NOF;
    table->info_get.key_size[0] = DNX_ALGO_PORT_TYPE_NOF;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "-1";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_framer_b66switch_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_framer_table_b66switch");

    
    default_data = (dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block_id = -1;
    default_data->backup_block_id = -1;
    
    for (port_type_index = 0; port_type_index < table->keys[0].size; port_type_index++)
    {
        data = (dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_get(unit, table, port_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DNX_ALGO_PORT_TYPE_NIF_ETH < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_NIF_ETH, 0);
        data->block_id = 6;
        data->backup_block_id = 23;
    }
    if (DNX_ALGO_PORT_TYPE_FLEXE_CLIENT < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_FLEXE_CLIENT, 0);
        data->block_id = 6;
        data->backup_block_id = 23;
    }
    if (DNX_ALGO_PORT_TYPE_FRAMER_MAC < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_FRAMER_MAC, 0);
        data->block_id = 9;
        data->backup_block_id = 26;
    }
    if (DNX_ALGO_PORT_TYPE_FRAMER_SAR < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_get(unit, table, DNX_ALGO_PORT_TYPE_FRAMER_SAR, 0);
        data->block_id = 7;
        data->backup_block_id = 24;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_framer_mac_info_set(
    int unit)
{
    int index_index;
    dnx_data_nif_framer_mac_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int table_index = dnx_data_nif_framer_table_mac_info;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_framer_mac_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_framer_table_mac_info");

    
    default_data = (dnx_data_nif_framer_mac_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->block_id = -1;
    default_data->backup_block_id = -1;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_framer_mac_info_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_mac_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->block_id = 8;
        data->backup_block_id = 25;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_mac_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->block_id = 10;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_framer_mgmt_channel_info_set(
    int unit)
{
    int channel_index;
    dnx_data_nif_framer_mgmt_channel_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_framer;
    int table_index = dnx_data_nif_framer_table_mgmt_channel_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DNX_DATA_MAX_NIF_FRAMER_NOF_MGMT_CHANNELS;
    table->info_get.key_size[0] = DNX_DATA_MAX_NIF_FRAMER_NOF_MGMT_CHANNELS;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_framer_mgmt_channel_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_framer_table_mgmt_channel_info");

    
    default_data = (dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->valid = 0;
    default_data->is_ptp = 0;
    default_data->block_id = 0;
    
    for (channel_index = 0; channel_index < table->keys[0].size; channel_index++)
    {
        data = (dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_get(unit, table, channel_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->valid = 0;
        data->block_id = 11;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->valid = 0;
        data->block_id = 12;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->valid = 1;
        data->block_id = 13;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->valid = 1;
        data->is_ptp = 1;
        data->block_id = 14;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_mac_client_is_l1_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_is_l1_support;
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
j2x_a0_dnx_data_nif_mac_client_is_speed_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_is_speed_supported;
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
j2x_a0_dnx_data_nif_mac_client_is_tx_crcmode_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_is_tx_crcmode_supported;
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
j2x_a0_dnx_data_nif_mac_client_is_tx_pading_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_is_tx_pading_supported;
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
j2x_a0_dnx_data_nif_mac_client_is_pfc_pause_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_is_pfc_pause_supported;
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
j2x_a0_dnx_data_nif_mac_client_is_loopback_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_is_loopback_supported;
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
j2x_a0_dnx_data_nif_mac_client_sb_tx_fifo_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_sb_tx_fifo_supported;
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
j2x_a0_dnx_data_nif_mac_client_sb_fifo_size_configurable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_sb_fifo_size_configurable;
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
j2x_a0_dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported;
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
j2x_a0_dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported;
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
j2x_a0_dnx_data_nif_mac_client_enc_dec_configurable_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int feature_index = dnx_data_nif_mac_client_enc_dec_configurable;
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
j2x_a0_dnx_data_nif_mac_client_nof_normal_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_normal_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_nof_special_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_special_clients;
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
j2x_a0_dnx_data_nif_mac_client_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.mac_client.nof_normal_clients_get(unit) + dnx_data_nif.mac_client.nof_special_clients_get(unit);

    
    define->data = dnx_data_nif.mac_client.nof_normal_clients_get(unit) + dnx_data_nif.mac_client.nof_special_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_channel_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_channel_base;
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
j2x_a0_dnx_data_nif_mac_client_max_nof_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_max_nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1700;

    
    define->data = 1700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_priority_groups_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_priority_groups_nof;
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
j2x_a0_dnx_data_nif_mac_client_mac_tx_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_mac_tx_threshold;
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
j2x_a0_dnx_data_nif_mac_client_nof_mac_timeslots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_mac_timeslots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_tx_average_ipg_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_tx_average_ipg_max;
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
j2x_a0_dnx_data_nif_mac_client_tx_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_tx_threshold_max;
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
j2x_a0_dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 4095;

    
    define->data = 4095;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_sb_fifo_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_sb_fifo_granularity;
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
j2x_a0_dnx_data_nif_mac_client_sb_cal_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_sb_cal_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_nof_sb_fifos_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_sb_fifos;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 640;

    
    define->data = 640;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_nof_sb_cal_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_sb_cal_slots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo;
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
j2x_a0_dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo;
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
j2x_a0_dnx_data_nif_mac_client_tinymac_clock_mhz_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_tinymac_clock_mhz;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1700;

    
    define->data = 1700;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g;
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
j2x_a0_dnx_data_nif_mac_client_async_fifo_delay_cycles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_async_fifo_delay_cycles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 260;

    
    define->data = 260;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_mac_client_pipe_delay_cycles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int define_index = dnx_data_nif_mac_client_define_pipe_delay_cycles;
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
j2x_a0_dnx_data_nif_mac_client_speed_tx_property_table_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_mac_client_speed_tx_property_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_mac_client;
    int table_index = dnx_data_nif_mac_client_table_speed_tx_property_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 5;
    table->info_get.key_size[0] = 5;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_mac_client_speed_tx_property_table_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_mac_client_table_speed_tx_property_table");

    
    default_data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = 0;
    default_data->speed_group = 0;
    default_data->num_credit = 0;
    default_data->tx_threshold = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 0;
        data->speed_group = 0;
        data->num_credit = 0;
        data->tx_threshold = 3;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 30000;
        data->speed_group = 0;
        data->num_credit = 1;
        data->tx_threshold = 7;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 80000;
        data->speed_group = 0;
        data->num_credit = 2;
        data->tx_threshold = 7;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 200000;
        data->speed_group = 1;
        data->num_credit = 7;
        data->tx_threshold = 7;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 400000;
        data->speed_group = 1;
        data->num_credit = 15;
        data->tx_threshold = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_sar_client_connect_to_66bswitch_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_sar_client;
    int feature_index = dnx_data_nif_sar_client_connect_to_66bswitch;
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
j2x_a0_dnx_data_nif_sar_client_is_tdm_port_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_sar_client;
    int feature_index = dnx_data_nif_sar_client_is_tdm_port;
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
j2x_a0_dnx_data_nif_sar_client_per_channel_cell_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_sar_client;
    int feature_index = dnx_data_nif_sar_client_per_channel_cell_mode;
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
j2x_a0_dnx_data_nif_sar_client_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_sar_client;
    int define_index = dnx_data_nif_sar_client_define_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_sar_client_nof_sar_timeslots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_sar_client;
    int define_index = dnx_data_nif_sar_client_define_nof_sar_timeslots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_sar_client_cal_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_sar_client;
    int define_index = dnx_data_nif_sar_client_define_cal_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
j2x_a0_dnx_data_nif_flexe_is_led_supported_set(
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
j2x_a0_dnx_data_nif_flexe_half_capability_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_pcs_port_enable_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_pcs_port_enable_support;
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
j2x_a0_dnx_data_nif_flexe_rx_valid_in_lf_set(
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
j2x_a0_dnx_data_nif_flexe_disable_oam_over_flexe_set(
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
j2x_a0_dnx_data_nif_flexe_nb_rx_port_reset_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_nb_rx_port_reset_support;
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
j2x_a0_dnx_data_nif_flexe_toggle_tx_reset_in_nb_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_toggle_tx_reset_in_nb;
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
j2x_a0_dnx_data_nif_flexe_pcs_to_flexe_port_map_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_pcs_to_flexe_port_map_support;
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
j2x_a0_dnx_data_nif_flexe_nb_cal_slot_enable_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_nb_cal_slot_enable_support;
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
j2x_a0_dnx_data_nif_flexe_nb_cal_switch_enable_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_nb_cal_switch_enable_support;
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
j2x_a0_dnx_data_nif_flexe_nb_dynamic_async_fifo_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_nb_dynamic_async_fifo;
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
j2x_a0_dnx_data_nif_flexe_multi_device_mode_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_multi_device_mode_support;
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
j2x_a0_dnx_data_nif_flexe_core_port_need_alloc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_core_port_need_alloc;
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
j2x_a0_dnx_data_nif_flexe_multi_flexe_slot_mode_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_multi_flexe_slot_mode_support;
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
j2x_a0_dnx_data_nif_flexe_unaware_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_unaware_support;
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
j2x_a0_dnx_data_nif_flexe_nb_tx_cal_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_nb_tx_cal_support;
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
j2x_a0_dnx_data_nif_flexe_feu_nof_set(
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
j2x_a0_dnx_data_nif_flexe_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 320;

    
    define->data = 320;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_nof_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_groups;
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
j2x_a0_dnx_data_nif_flexe_max_group_id_set(
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
j2x_a0_dnx_data_nif_flexe_min_group_id_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_min_group_id;
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
j2x_a0_dnx_data_nif_flexe_nb_tx_delay_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_tx_delay;
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
j2x_a0_dnx_data_nif_flexe_nof_pcs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_pcs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.flexe.nof_flexe_instances_get(unit);

    
    define->data = dnx_data_nif.flexe.nof_flexe_instances_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_nof_phy_speeds_set(
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
j2x_a0_dnx_data_nif_flexe_phy_speed_granularity_set(
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
j2x_a0_dnx_data_nif_flexe_max_flexe_core_speed_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_flexe_core_speed;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1600000;

    
    define->data = 1600000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_max_nof_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_max_nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 960;

    
    define->data = 960;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_min_client_id_set(
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
j2x_a0_dnx_data_nif_flexe_max_client_id_set(
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
j2x_a0_dnx_data_nif_flexe_nof_flexe_instances_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_instances;
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
j2x_a0_dnx_data_nif_flexe_nof_flexe_lphys_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_lphys;
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
j2x_a0_dnx_data_nif_flexe_nof_flexe_core_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_flexe_core_ports;
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
j2x_a0_dnx_data_nif_flexe_nb_cal_nof_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_cal_nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 340;

    
    define->data = 340;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_nb_cal_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_cal_granularity;
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
j2x_a0_dnx_data_nif_flexe_non_5g_slot_bw_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_non_5g_slot_bw;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800000;

    
    define->data = 800000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_nb_cal_slot_nof_bits_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_cal_slot_nof_bits;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 7;

    
    define->data = 7;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set(
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
j2x_a0_dnx_data_nif_flexe_device_slot_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_device_slot_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FLEXE_SLOT_MODE_1P25G_AND_5G;

    
    define->data = DNX_FLEXE_SLOT_MODE_1P25G_AND_5G;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FLEXE_SLOT_MODE;
    define->property.doc = 
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 2;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "1G_AND_5G";
    define->property.mapping[0].val = DNX_FLEXE_SLOT_MODE_1G_AND_5G;
    define->property.mapping[1].name = "1P25G_AND_5G";
    define->property.mapping[1].val = DNX_FLEXE_SLOT_MODE_1P25G_AND_5G;
    define->property.mapping[1].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_flexe_phy_info_set(
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
    table->values[4].default_val = "-1";
    table->values[5].default_val = "-1";
    
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
    default_data->nof_instances = -1;
    default_data->instance_id[0] = -1;
    
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
        data->nof_instances = 1;
        SHR_RANGE_VERIFY(16, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES, _SHR_E_INTERNAL, "out of bound access to array")
        data->instance_id[0] = 0;
        data->instance_id[1] = 1;
        data->instance_id[2] = 2;
        data->instance_id[3] = 3;
        data->instance_id[4] = 4;
        data->instance_id[5] = 5;
        data->instance_id[6] = 6;
        data->instance_id[7] = 7;
        data->instance_id[8] = 8;
        data->instance_id[9] = 9;
        data->instance_id[10] = 10;
        data->instance_id[11] = 11;
        data->instance_id[12] = 12;
        data->instance_id[13] = 13;
        data->instance_id[14] = 14;
        data->instance_id[15] = 15;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 100000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 254;
        data->nof_instances = 1;
        SHR_RANGE_VERIFY(16, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES, _SHR_E_INTERNAL, "out of bound access to array")
        data->instance_id[0] = 0;
        data->instance_id[1] = 1;
        data->instance_id[2] = 2;
        data->instance_id[3] = 3;
        data->instance_id[4] = 4;
        data->instance_id[5] = 5;
        data->instance_id[6] = 6;
        data->instance_id[7] = 7;
        data->instance_id[8] = 8;
        data->instance_id[9] = 9;
        data->instance_id[10] = 10;
        data->instance_id[11] = 11;
        data->instance_id[12] = 12;
        data->instance_id[13] = 13;
        data->instance_id[14] = 14;
        data->instance_id[15] = 15;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 200000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 126;
        data->nof_instances = 2;
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES, _SHR_E_INTERNAL, "out of bound access to array")
        data->instance_id[0] = 0;
        data->instance_id[1] = 2;
        data->instance_id[2] = 4;
        data->instance_id[3] = 6;
        data->instance_id[4] = 8;
        data->instance_id[5] = 10;
        data->instance_id[6] = 12;
        data->instance_id[7] = 14;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 400000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 62;
        data->nof_instances = 4;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES, _SHR_E_INTERNAL, "out of bound access to array")
        data->instance_id[0] = 0;
        data->instance_id[1] = 4;
        data->instance_id[2] = 8;
        data->instance_id[3] = 12;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_supported_pms_set(
    int unit)
{
    int framer_idx_index;
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
    
    for (framer_idx_index = 0; framer_idx_index < table->keys[0].size; framer_idx_index++)
    {
        data = (dnx_data_nif_flexe_supported_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, framer_idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_supported_pms_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        SHR_RANGE_VERIFY(8, 0, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, _SHR_E_INTERNAL, "out of bound access to array")
        data->nif_pms[0] = 0;
        data->nif_pms[1] = 1;
        data->nif_pms[2] = 2;
        data->nif_pms[3] = 3;
        data->nif_pms[4] = 4;
        data->nif_pms[5] = 5;
        data->nif_pms[6] = 6;
        data->nif_pms[7] = 7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flexe_client_speed_granularity_set(
    int unit)
{
    int slot_mode_index;
    dnx_data_nif_flexe_client_speed_granularity_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int table_index = dnx_data_nif_flexe_table_client_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "5000";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_flexe_client_speed_granularity_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_flexe_table_client_speed_granularity");

    
    default_data = (dnx_data_nif_flexe_client_speed_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = 5000;
    
    for (slot_mode_index = 0; slot_mode_index < table->keys[0].size; slot_mode_index++)
    {
        data = (dnx_data_nif_flexe_client_speed_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, slot_mode_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (bcmPortFlexePhySlot5G < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_client_speed_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortFlexePhySlot5G, 0);
        data->val = 5000;
    }
    if (bcmPortFlexePhySlot1P25G < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_client_speed_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortFlexePhySlot1P25G, 0);
        data->val = 1250;
    }
    if (bcmPortFlexePhySlot1G < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_client_speed_granularity_t *) dnxc_data_mgmt_table_data_get(unit, table, bcmPortFlexePhySlot1G, 0);
        data->val = 1000;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_wbc_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_wbc;
    int feature_index = dnx_data_nif_wbc_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit) != DNX_PORT_NIF_L1_ONLY_MODE_WITHOUT_ILKN || dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit) != DNX_PORT_NIF_L1_ONLY_MODE_WITHOUT_ILKN || dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
j2x_a0_dnx_data_nif_prd_tcam_key_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_tcam_key_offset_size;
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
j2x_a0_dnx_data_nif_prd_nof_priorities_set(
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
j2x_a0_dnx_data_nif_prd_rmc_threshold_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_rmc_threshold_max;
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
j2x_a0_dnx_data_nif_prd_nof_framer_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_framer_port_profiles;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.prd.nof_ofr_port_profiles_get(unit);

    
    define->data = dnx_data_nif.prd.nof_ofr_port_profiles_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_prd_nof_ofr_port_profiles_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_ofr_port_profiles;
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
j2x_a0_dnx_data_nif_prd_nof_ofr_nif_interfaces_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_ofr_nif_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit) - dnx_data_nif.ilkn.ilu_nof_get(unit)*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    
    define->data = dnx_data_nif.global.nof_nif_interfaces_per_core_get(unit) - dnx_data_nif.ilkn.ilu_nof_get(unit)*dnx_data_nif.ilkn.ilkn_unit_if_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_prd_nof_prd_counters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_prd;
    int define_index = dnx_data_nif_prd_define_nof_prd_counters;
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
j2x_a0_dnx_data_nif_portmod_pm_types_nof_set(
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
j2x_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set(
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
        data->type = portmodDispatchTypePm8x50_flexe_gen2;
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
j2x_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set(
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
j2x_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set(
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
j2x_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
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
j2x_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set(
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
j2x_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set(
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
j2x_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set(
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
j2x_a0_dnx_data_nif_dbal_nof_rx_fifo_counters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_nof_rx_fifo_counters;
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
j2x_a0_dnx_data_nif_dbal_nof_tx_fifo_counters_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_dbal;
    int define_index = dnx_data_nif_dbal_define_nof_tx_fifo_counters;
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
j2x_a0_dnx_data_nif_arb_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int feature_index = dnx_data_nif_arb_is_supported;
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
j2x_a0_dnx_data_nif_arb_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int feature_index = dnx_data_nif_arb_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_rx_qpms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_rx_qpms;
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
j2x_a0_dnx_data_nif_arb_nof_rx_ppms_in_qpm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_rx_ppms_in_qpm;
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
j2x_a0_dnx_data_nif_arb_nof_lanes_in_qpm_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_lanes_in_qpm;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_map_destinations_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_map_destinations;
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
j2x_a0_dnx_data_nif_arb_max_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_max_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_nof_rx_sources_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_rx_sources;
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
j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_modes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_modes;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_rx_sources_get(unit) * dnx_data_nif.arb.nof_map_destinations_get(unit);

    
    define->data = dnx_data_nif.arb.nof_rx_sources_get(unit) * dnx_data_nif.arb.nof_map_destinations_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_qpm_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 340;

    
    define->data = 340;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_sch_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_cdpm_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_cdpm_calendar_length;
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
j2x_a0_dnx_data_nif_arb_tmac_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tmac_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->data = dnx_data_nif.arb.nof_rx_ppms_in_qpm_get(unit) * dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_clients;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.arb.nof_contexts_get(unit);

    
    define->data = dnx_data_nif.arb.nof_contexts_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_qpm_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_qpm_port_speed_granularity;
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
j2x_a0_dnx_data_nif_arb_rx_sch_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_sch_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_eth_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_eth_port_speed_granularity;
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
j2x_a0_dnx_data_nif_arb_tx_tmac_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_tmac_port_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_tmac_nof_sections_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_tmac_nof_sections;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 680;

    
    define->data = 680;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity;
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
j2x_a0_dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_entries_in_tmac_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_entries_in_tmac_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar;
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
j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar;
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
j2x_a0_dnx_data_nif_arb_min_port_speed_for_link_list_section_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_min_port_speed_for_link_list_section;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3000;

    
    define->data = 3000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_link_list_sections_denominator_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_link_list_sections_denominator;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 10000;

    
    define->data = 10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_high_start_tx_speed_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_high_start_tx_speed_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 800000;

    
    define->data = 800000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_low_start_tx_speed_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_low_start_tx_speed_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 3000;

    
    define->data = 3000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_high_start_tx_speed_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_high_start_tx_speed_value;
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
j2x_a0_dnx_data_nif_arb_middle_start_tx_speed_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_middle_start_tx_speed_value;
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
j2x_a0_dnx_data_nif_arb_low_start_tx_speed_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_low_start_tx_speed_value;
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
j2x_a0_dnx_data_nif_arb_max_value_for_tx_start_threshold_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_max_value_for_tx_start_threshold;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 255;

    
    define->data = 255;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_arb_rx_pm_default_min_pkt_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_arb;
    int define_index = dnx_data_nif_arb_define_rx_pm_default_min_pkt_size;
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
j2x_a0_dnx_data_nif_ofr_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int feature_index = dnx_data_nif_ofr_is_supported;
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
j2x_a0_dnx_data_nif_ofr_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int feature_index = dnx_data_nif_ofr_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN || dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit)  == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN || dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rmc_per_priority_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rmc_per_priority_group;
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
j2x_a0_dnx_data_nif_ofr_nof_total_rmcs_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_total_rmcs;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) * dnx_data_nif.eth.priority_groups_nof_get(unit);

    
    define->data = dnx_data_nif.ofr.nof_rmc_per_priority_group_get(unit) * dnx_data_nif.eth.priority_groups_nof_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rx_mem_sections_per_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group;
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
j2x_a0_dnx_data_nif_ofr_nof_rx_memory_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_memory_groups;
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
j2x_a0_dnx_data_nif_ofr_nof_rx_memory_sections_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_memory_sections;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.ofr.nof_rx_memory_groups_get(unit) * dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);

    
    define->data = dnx_data_nif.ofr.nof_rx_memory_groups_get(unit) * dnx_data_nif.ofr.nof_rx_mem_sections_per_group_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_rx_memory_entries_per_section_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section;
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
j2x_a0_dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity;
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
j2x_a0_dnx_data_nif_ofr_rx_sch_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_rx_sch_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_ofr_nof_bytes_per_memory_section_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_ofr;
    int define_index = dnx_data_nif_ofr_define_nof_bytes_per_memory_section;
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
j2x_a0_dnx_data_nif_oft_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int feature_index = dnx_data_nif_oft_is_supported;
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
j2x_a0_dnx_data_nif_oft_is_used_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int feature_index = dnx_data_nif_oft_is_used;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN;

    
    feature->data = dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_DISABLED || dnx_data_nif.global.l1_only_mode_get(unit) == DNX_PORT_NIF_L1_ONLY_MODE_WITH_ILKN;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
j2x_a0_dnx_data_nif_oft_calendar_length_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_calendar_length;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 6800;

    
    define->data = 6800;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_nof_sections_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_sections;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 640;

    
    define->data = 640;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_nof_internal_calendar_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_internal_calendar_entries;
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
j2x_a0_dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar;
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
j2x_a0_dnx_data_nif_oft_nof_contexts_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_nof_contexts;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    
    define->data = dnx_data_nif.phys.nof_phys_get(unit) + dnx_data_nif.mac_client.nof_clients_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_oft_port_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_port_speed_granularity;
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
j2x_a0_dnx_data_nif_oft_calendar_speed_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_oft;
    int define_index = dnx_data_nif_oft_define_calendar_speed_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 250;

    
    define->data = 250;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}








static shr_error_e
j2x_a0_dnx_data_nif_signal_quality_hw_based_solution_set(
    int unit)
{
    int pm_type_index;
    dnx_data_nif_signal_quality_hw_based_solution_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_signal_quality;
    int table_index = dnx_data_nif_signal_quality_table_hw_based_solution;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = portmodDispatchTypeCount;
    table->info_get.key_size[0] = portmodDispatchTypeCount;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_signal_quality_hw_based_solution_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_signal_quality_table_hw_based_solution");

    
    default_data = (dnx_data_nif_signal_quality_hw_based_solution_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->is_valid = 0;
    
    for (pm_type_index = 0; pm_type_index < table->keys[0].size; pm_type_index++)
    {
        data = (dnx_data_nif_signal_quality_hw_based_solution_t *) dnxc_data_mgmt_table_data_get(unit, table, pm_type_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (portmodDispatchTypePm8x50_flexe_gen2 < table->keys[0].size)
    {
        data = (dnx_data_nif_signal_quality_hw_based_solution_t *) dnxc_data_mgmt_table_data_get(unit, table, portmodDispatchTypePm8x50_flexe_gen2, 0);
        data->is_valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_otn_is_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_otn;
    int feature_index = dnx_data_nif_otn_is_supported;
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
j2x_a0_dnx_data_nif_otn_nof_units_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_otn;
    int define_index = dnx_data_nif_otn_define_nof_units;
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
j2x_a0_dnx_data_nif_otn_nof_lanes_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_otn;
    int define_index = dnx_data_nif_otn_define_nof_lanes;
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
j2x_a0_dnx_data_nif_otn_nof_groups_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_otn;
    int define_index = dnx_data_nif_otn_define_nof_groups;
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
j2x_a0_dnx_data_nif_otn_nof_lanes_per_group_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_otn;
    int define_index = dnx_data_nif_otn_define_nof_lanes_per_group;
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
j2x_a0_dnx_data_nif_otn_supported_interfaces_set(
    int unit)
{
    int index_index;
    dnx_data_nif_otn_supported_interfaces_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_otn;
    int table_index = dnx_data_nif_otn_table_supported_interfaces;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 18;
    table->info_get.key_size[0] = 18;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "portmodDispatchTypeCount";
    table->values[2].default_val = "bcmPortPhyFecInvalid";
    table->values[3].default_val = "0";
    table->values[4].default_val = "no name";
    table->values[5].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_otn_supported_interfaces_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_otn_table_supported_interfaces");

    
    default_data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->speed = -1;
    default_data->pm_dispatch_type = portmodDispatchTypeCount;
    default_data->fec_type = bcmPortPhyFecInvalid;
    default_data->nof_lanes = 0;
    default_data->otn_intf_name = "no name";
    default_data->is_valid = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->speed = 10709;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTU2";
        data->is_valid = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 11049;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTU1e";
        data->is_valid = 1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 11095;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTU2e";
        data->is_valid = 1;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->speed = 25781;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTU25u-RS";
        data->is_valid = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->speed = 26562;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTU25u-RS-KP4";
        data->is_valid = 1;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->speed = 26562;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 2;
        data->otn_intf_name = "OTL50u.2-RS";
        data->is_valid = 1;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->speed = 27952;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 4;
        data->otn_intf_name = "FOIC1.4-RS";
        data->is_valid = 1;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->speed = 27952;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTU25-RS";
        data->is_valid = 1;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->speed = 27952;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 2;
        data->otn_intf_name = "OTL50.2-RS";
        data->is_valid = 1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->speed = 27952;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 4;
        data->otn_intf_name = "OTL4.4";
        data->is_valid = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->speed = 28076;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 4;
        data->otn_intf_name = "OTLC.4";
        data->is_valid = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->speed = 53125;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTL50u.1-RS";
        data->is_valid = 1;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->speed = 55904;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 2;
        data->otn_intf_name = "FOIC1.2-RS";
        data->is_valid = 1;
    }
    if (13 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 13, 0);
        data->speed = 55904;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 4;
        data->otn_intf_name = "FOIC2.4-RS";
        data->is_valid = 1;
    }
    if (14 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 14, 0);
        data->speed = 55904;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 8;
        data->otn_intf_name = "FOIC4.8-RS";
        data->is_valid = 1;
    }
    if (15 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 15, 0);
        data->speed = 55904;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 1;
        data->otn_intf_name = "OTL50.1-RS";
        data->is_valid = 1;
    }
    if (16 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 16, 0);
        data->speed = 55904;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 2;
        data->otn_intf_name = "OTL4.2";
        data->is_valid = 1;
    }
    if (17 < table->keys[0].size)
    {
        data = (dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_get(unit, table, 17, 0);
        data->speed = 56152;
        data->pm_dispatch_type = portmodDispatchTypePm8x50_flexe_gen2;
        data->fec_type = bcmPortPhyFecNone;
        data->nof_lanes = 2;
        data->otn_intf_name = "OTLC.2";
        data->is_valid = 1;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
j2x_a0_dnx_data_nif_flr_is_advanced_flr_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flr;
    int feature_index = dnx_data_nif_flr_is_advanced_flr_supported;
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
j2x_a0_dnx_data_nif_flr_max_tick_unit_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flr;
    int define_index = dnx_data_nif_flr_define_max_tick_unit;
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
j2x_a0_dnx_data_nif_flr_max_tick_count_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flr;
    int define_index = dnx_data_nif_flr_define_max_tick_count;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 1023;

    
    define->data = 1023;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_nif_flr_timer_granularity_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flr;
    int define_index = dnx_data_nif_flr_define_timer_granularity;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 40;

    
    define->data = 40;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2x_a0_data_nif_attach(
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
    define->set = j2x_a0_dnx_data_nif_global_nof_lcplls_set;
    data_index = dnx_data_nif_global_define_nof_nif_interfaces_per_core;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nof_nif_interfaces_per_core_set;
    data_index = dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_set;
    data_index = dnx_data_nif_global_define_nof_nif_units_per_core;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_nof_nif_units_per_core_set;
    data_index = dnx_data_nif_global_define_last_port_led_scan;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_last_port_led_scan_set;
    data_index = dnx_data_nif_global_define_reassembler_fifo_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_reassembler_fifo_threshold_set;
    data_index = dnx_data_nif_global_define_total_data_capacity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_total_data_capacity_set;
    data_index = dnx_data_nif_global_define_single_mgmt_port_capacity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_single_mgmt_port_capacity_set;
    data_index = dnx_data_nif_global_define_ghao_speed_limit;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_ghao_speed_limit_set;
    data_index = dnx_data_nif_global_define_l1_only_mode;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_l1_only_mode_set;
    data_index = dnx_data_nif_global_define_otn_supported;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_global_otn_supported_set;

    
    data_index = dnx_data_nif_global_reassembler;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_reassembler_set;
    data_index = dnx_data_nif_global_clock_power_down;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_clock_power_down_set;
    data_index = dnx_data_nif_global_is_shared_arch;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_is_shared_arch_set;
    data_index = dnx_data_nif_global_is_ilkn_after_flexe;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_is_ilkn_after_flexe_set;
    data_index = dnx_data_nif_global_support_xpmd_if;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_support_xpmd_if_set;
    data_index = dnx_data_nif_global_is_g_hao_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_is_g_hao_supported_set;
    data_index = dnx_data_nif_global_bh_2x2_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_bh_2x2_supported_set;
    data_index = dnx_data_nif_global_eth_use_framer_mgmt_bw;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_global_eth_use_framer_mgmt_bw_set;

    
    data_index = dnx_data_nif_global_table_pll_phys;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_global_pll_phys_set;
    data_index = dnx_data_nif_global_table_nif_interface_id_to_unit_id;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_global_nif_interface_id_to_unit_id_set;
    
    submodule_index = dnx_data_nif_submodule_phys;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_phys_define_nof_phys;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_nof_phys_set;
    data_index = dnx_data_nif_phys_define_max_phys_in_core;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_max_phys_in_core_set;
    data_index = dnx_data_nif_phys_define_pm8x50_gen;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_pm8x50_gen_set;
    data_index = dnx_data_nif_phys_define_is_pam4_speed_supported;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_is_pam4_speed_supported_set;
    data_index = dnx_data_nif_phys_define_txpi_sdm_scheme;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_phys_txpi_sdm_scheme_set;

    
    data_index = dnx_data_nif_phys_txpi_supported_in_pm8x50;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_phys_txpi_supported_in_pm8x50_set;

    
    data_index = dnx_data_nif_phys_table_general;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_general_set;
    data_index = dnx_data_nif_phys_table_core_phys_map;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_core_phys_map_set;
    data_index = dnx_data_nif_phys_table_vco_div;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_vco_div_set;
    data_index = dnx_data_nif_phys_table_nof_phys_per_core;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_nof_phys_per_core_set;
    data_index = dnx_data_nif_phys_table_txpi_sdm_div;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_phys_txpi_sdm_div_set;
    
    submodule_index = dnx_data_nif_submodule_ilkn;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ilkn_define_ilu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilu_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_unit_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_unit_if_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_unit_if_nof_set;
    data_index = dnx_data_nif_ilkn_define_fec_units_per_core_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_fec_units_per_core_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_lanes_max_nof_using_fec;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_lanes_max_nof_using_fec_set;
    data_index = dnx_data_nif_ilkn_define_lanes_allowed_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_lanes_allowed_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_over_eth_pms_max_set;
    data_index = dnx_data_nif_ilkn_define_segments_max_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_segments_max_nof_set;
    data_index = dnx_data_nif_ilkn_define_segments_half_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_segments_half_nof_set;
    data_index = dnx_data_nif_ilkn_define_pms_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_pms_nof_set;
    data_index = dnx_data_nif_ilkn_define_fmac_bus_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_fmac_bus_size_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_rx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_ilkn_tx_hrf_nof_set;
    data_index = dnx_data_nif_ilkn_define_data_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_data_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tdm_rx_hrf_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_tdm_rx_hrf_size_set;
    data_index = dnx_data_nif_ilkn_define_tx_hrf_credits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_tx_hrf_credits_set;
    data_index = dnx_data_nif_ilkn_define_watermark_high_data;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_watermark_high_data_set;
    data_index = dnx_data_nif_ilkn_define_watermark_low_data;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_watermark_low_data_set;
    data_index = dnx_data_nif_ilkn_define_pad_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_pad_size_set;
    data_index = dnx_data_nif_ilkn_define_burst_max_range_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_burst_max_range_max_set;
    data_index = dnx_data_nif_ilkn_define_burst_min;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_burst_min_set;
    data_index = dnx_data_nif_ilkn_define_burst_min_range_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_burst_min_range_max_set;
    data_index = dnx_data_nif_ilkn_define_port_0_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_port_0_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_port_1_status_intr_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_port_1_status_intr_id_set;
    data_index = dnx_data_nif_ilkn_define_interleaved_error_drop_single_context;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ilkn_interleaved_error_drop_single_context_set;

    
    data_index = dnx_data_nif_ilkn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_supported_set;
    data_index = dnx_data_nif_ilkn_is_elk_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_elk_supported_set;
    data_index = dnx_data_nif_ilkn_over_fabric_per_core_selector;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_over_fabric_per_core_selector_set;
    data_index = dnx_data_nif_ilkn_frmr_ilkn_selector;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_frmr_ilkn_selector_set;
    data_index = dnx_data_nif_ilkn_support_burst_interleaving;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_support_burst_interleaving_set;
    data_index = dnx_data_nif_ilkn_scheduler_context_per_port_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_scheduler_context_per_port_support_set;
    data_index = dnx_data_nif_ilkn_support_ilu_burst_max;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_support_ilu_burst_max_set;
    data_index = dnx_data_nif_ilkn_is_fec_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_fec_supported_set;
    data_index = dnx_data_nif_ilkn_legacy_lane_format;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_legacy_lane_format_set;
    data_index = dnx_data_nif_ilkn_is_20p6G_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_20p6G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_is_25G_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_25G_speed_supported_set;
    data_index = dnx_data_nif_ilkn_is_28p1G_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ilkn_is_28p1G_speed_supported_set;

    
    data_index = dnx_data_nif_ilkn_table_supported_phys;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_supported_phys_set;
    data_index = dnx_data_nif_ilkn_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_supported_interfaces_set;
    data_index = dnx_data_nif_ilkn_table_ilkn_cores;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_ilkn_cores_set;
    data_index = dnx_data_nif_ilkn_table_supported_device_core;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_supported_device_core_set;
    data_index = dnx_data_nif_ilkn_table_nif_pms;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_nif_pms_set;
    data_index = dnx_data_nif_ilkn_table_fabric_pms;
    table = &submodule->tables[data_index];
    table->set = NULL;
    data_index = dnx_data_nif_ilkn_table_nif_lanes_map;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_nif_lanes_map_set;
    data_index = dnx_data_nif_ilkn_table_start_tx_threshold_table;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_ilkn_start_tx_threshold_table_set;
    
    submodule_index = dnx_data_nif_submodule_eth;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_eth_define_cdu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdu_nof_set;
    data_index = dnx_data_nif_eth_define_clu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_clu_nof_set;
    data_index = dnx_data_nif_eth_define_cdum_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdum_nof_set;
    data_index = dnx_data_nif_eth_define_nof_pms_in_cdu;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_nof_pms_in_cdu_set;
    data_index = dnx_data_nif_eth_define_nof_lanes_in_cdu;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_nof_lanes_in_cdu_set;
    data_index = dnx_data_nif_eth_define_cdu_mac_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdu_mac_nof_set;
    data_index = dnx_data_nif_eth_define_mac_lanes_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_mac_lanes_nof_set;
    data_index = dnx_data_nif_eth_define_cdu_memory_entries_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_cdu_memory_entries_nof_set;
    data_index = dnx_data_nif_eth_define_priority_group_nof_entries_min;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_priority_group_nof_entries_min_set;
    data_index = dnx_data_nif_eth_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_priority_groups_nof_set;
    data_index = dnx_data_nif_eth_define_phy_map_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_eth_phy_map_granularity_set;

    
    data_index = dnx_data_nif_eth_is_rmc_allocation_needed;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_eth_is_rmc_allocation_needed_set;
    data_index = dnx_data_nif_eth_gear_shifter_exists;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_eth_gear_shifter_exists_set;

    
    data_index = dnx_data_nif_eth_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_pm_properties_set;
    data_index = dnx_data_nif_eth_table_ethu_properties;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_ethu_properties_set;
    data_index = dnx_data_nif_eth_table_phy_map;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_phy_map_set;
    data_index = dnx_data_nif_eth_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_supported_interfaces_set;
    data_index = dnx_data_nif_eth_table_ethu_per_core;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_ethu_per_core_set;
    data_index = dnx_data_nif_eth_table_nif_cores_ethus;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_eth_nif_cores_ethus_set;
    
    submodule_index = dnx_data_nif_submodule_simulator;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_simulator_define_cdu_type;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_simulator_cdu_type_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_framer;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_framer_define_nof_units;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_nof_units_set;
    data_index = dnx_data_nif_framer_define_b66switch_backup_channels;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_b66switch_backup_channels_set;
    data_index = dnx_data_nif_framer_define_gen;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_gen_set;
    data_index = dnx_data_nif_framer_define_nof_mgmt_channels;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_nof_mgmt_channels_set;
    data_index = dnx_data_nif_framer_define_clock_mhz;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_clock_mhz_set;
    data_index = dnx_data_nif_framer_define_ptp_channel_max_bandwidth;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_ptp_channel_max_bandwidth_set;
    data_index = dnx_data_nif_framer_define_oam_channel_max_bandwidth;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_oam_channel_max_bandwidth_set;
    data_index = dnx_data_nif_framer_define_nof_mac1_channels;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_nof_mac1_channels_set;
    data_index = dnx_data_nif_framer_define_nof_mac2_channels;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_nof_mac2_channels_set;
    data_index = dnx_data_nif_framer_define_nof_pms_per_framer;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_framer_nof_pms_per_framer_set;

    
    data_index = dnx_data_nif_framer_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_is_supported_set;
    data_index = dnx_data_nif_framer_disable_l1;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_disable_l1_set;
    data_index = dnx_data_nif_framer_is_clock_independent;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_is_clock_independent_set;
    data_index = dnx_data_nif_framer_l1_eth_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_l1_eth_supported_set;
    data_index = dnx_data_nif_framer_eth_use_flexe_group;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_eth_use_flexe_group_set;
    data_index = dnx_data_nif_framer_embeded_mac_in_busa_busc;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_embeded_mac_in_busa_busc_set;
    data_index = dnx_data_nif_framer_power_down_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_framer_power_down_supported_set;

    
    data_index = dnx_data_nif_framer_table_b66switch;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_framer_b66switch_set;
    data_index = dnx_data_nif_framer_table_mac_info;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_framer_mac_info_set;
    data_index = dnx_data_nif_framer_table_mgmt_channel_info;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_framer_mgmt_channel_info_set;
    
    submodule_index = dnx_data_nif_submodule_mac_client;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_mac_client_define_nof_normal_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_normal_clients_set;
    data_index = dnx_data_nif_mac_client_define_nof_special_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_special_clients_set;
    data_index = dnx_data_nif_mac_client_define_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_clients_set;
    data_index = dnx_data_nif_mac_client_define_channel_base;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_channel_base_set;
    data_index = dnx_data_nif_mac_client_define_max_nof_slots;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_max_nof_slots_set;
    data_index = dnx_data_nif_mac_client_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_priority_groups_nof_set;
    data_index = dnx_data_nif_mac_client_define_mac_tx_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_mac_tx_threshold_set;
    data_index = dnx_data_nif_mac_client_define_nof_mac_timeslots;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_mac_timeslots_set;
    data_index = dnx_data_nif_mac_client_define_tx_average_ipg_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_tx_average_ipg_max_set;
    data_index = dnx_data_nif_mac_client_define_tx_threshold_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_tx_threshold_max_set;
    data_index = dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_set;
    data_index = dnx_data_nif_mac_client_define_sb_fifo_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_sb_fifo_granularity_set;
    data_index = dnx_data_nif_mac_client_define_sb_cal_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_sb_cal_granularity_set;
    data_index = dnx_data_nif_mac_client_define_nof_sb_fifos;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_sb_fifos_set;
    data_index = dnx_data_nif_mac_client_define_nof_sb_cal_slots;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_sb_cal_slots_set;
    data_index = dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_set;
    data_index = dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_set;
    data_index = dnx_data_nif_mac_client_define_tinymac_clock_mhz;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_tinymac_clock_mhz_set;
    data_index = dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_set;
    data_index = dnx_data_nif_mac_client_define_async_fifo_delay_cycles;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_async_fifo_delay_cycles_set;
    data_index = dnx_data_nif_mac_client_define_pipe_delay_cycles;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_mac_client_pipe_delay_cycles_set;

    
    data_index = dnx_data_nif_mac_client_is_l1_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_is_l1_support_set;
    data_index = dnx_data_nif_mac_client_is_speed_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_is_speed_supported_set;
    data_index = dnx_data_nif_mac_client_is_tx_crcmode_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_is_tx_crcmode_supported_set;
    data_index = dnx_data_nif_mac_client_is_tx_pading_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_is_tx_pading_supported_set;
    data_index = dnx_data_nif_mac_client_is_pfc_pause_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_is_pfc_pause_supported_set;
    data_index = dnx_data_nif_mac_client_is_loopback_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_is_loopback_supported_set;
    data_index = dnx_data_nif_mac_client_sb_tx_fifo_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_sb_tx_fifo_supported_set;
    data_index = dnx_data_nif_mac_client_sb_fifo_size_configurable;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_sb_fifo_size_configurable_set;
    data_index = dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported_set;
    data_index = dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported_set;
    data_index = dnx_data_nif_mac_client_enc_dec_configurable;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_mac_client_enc_dec_configurable_set;

    
    data_index = dnx_data_nif_mac_client_table_speed_tx_property_table;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_mac_client_speed_tx_property_table_set;
    
    submodule_index = dnx_data_nif_submodule_sar_client;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_sar_client_define_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_sar_client_nof_clients_set;
    data_index = dnx_data_nif_sar_client_define_nof_sar_timeslots;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_sar_client_nof_sar_timeslots_set;
    data_index = dnx_data_nif_sar_client_define_cal_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_sar_client_cal_granularity_set;

    
    data_index = dnx_data_nif_sar_client_connect_to_66bswitch;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_sar_client_connect_to_66bswitch_set;
    data_index = dnx_data_nif_sar_client_is_tdm_port;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_sar_client_is_tdm_port_set;
    data_index = dnx_data_nif_sar_client_per_channel_cell_mode;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_sar_client_per_channel_cell_mode_set;

    
    
    submodule_index = dnx_data_nif_submodule_flexe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flexe_define_feu_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_feu_nof_set;
    data_index = dnx_data_nif_flexe_define_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_clients_set;
    data_index = dnx_data_nif_flexe_define_nof_groups;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_groups_set;
    data_index = dnx_data_nif_flexe_define_max_group_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_max_group_id_set;
    data_index = dnx_data_nif_flexe_define_min_group_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_min_group_id_set;
    data_index = dnx_data_nif_flexe_define_nb_tx_delay;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nb_tx_delay_set;
    data_index = dnx_data_nif_flexe_define_nof_pcs;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_pcs_set;
    data_index = dnx_data_nif_flexe_define_nof_phy_speeds;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_phy_speeds_set;
    data_index = dnx_data_nif_flexe_define_phy_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_phy_speed_granularity_set;
    data_index = dnx_data_nif_flexe_define_max_flexe_core_speed;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_max_flexe_core_speed_set;
    data_index = dnx_data_nif_flexe_define_max_nof_slots;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_max_nof_slots_set;
    data_index = dnx_data_nif_flexe_define_min_client_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_min_client_id_set;
    data_index = dnx_data_nif_flexe_define_max_client_id;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_max_client_id_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_instances;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_flexe_instances_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_lphys;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_flexe_lphys_set;
    data_index = dnx_data_nif_flexe_define_nof_flexe_core_ports;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nof_flexe_core_ports_set;
    data_index = dnx_data_nif_flexe_define_nb_cal_nof_slots;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nb_cal_nof_slots_set;
    data_index = dnx_data_nif_flexe_define_nb_cal_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nb_cal_granularity_set;
    data_index = dnx_data_nif_flexe_define_non_5g_slot_bw;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_non_5g_slot_bw_set;
    data_index = dnx_data_nif_flexe_define_nb_cal_slot_nof_bits;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nb_cal_slot_nof_bits_set;
    data_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set;
    data_index = dnx_data_nif_flexe_define_device_slot_mode;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flexe_device_slot_mode_set;

    
    data_index = dnx_data_nif_flexe_is_led_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_is_led_supported_set;
    data_index = dnx_data_nif_flexe_half_capability;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_half_capability_set;
    data_index = dnx_data_nif_flexe_pcs_port_enable_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_pcs_port_enable_support_set;
    data_index = dnx_data_nif_flexe_rx_valid_in_lf;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_rx_valid_in_lf_set;
    data_index = dnx_data_nif_flexe_disable_oam_over_flexe;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_disable_oam_over_flexe_set;
    data_index = dnx_data_nif_flexe_nb_rx_port_reset_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_nb_rx_port_reset_support_set;
    data_index = dnx_data_nif_flexe_toggle_tx_reset_in_nb;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_toggle_tx_reset_in_nb_set;
    data_index = dnx_data_nif_flexe_pcs_to_flexe_port_map_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_pcs_to_flexe_port_map_support_set;
    data_index = dnx_data_nif_flexe_nb_cal_slot_enable_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_nb_cal_slot_enable_support_set;
    data_index = dnx_data_nif_flexe_nb_cal_switch_enable_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_nb_cal_switch_enable_support_set;
    data_index = dnx_data_nif_flexe_nb_dynamic_async_fifo;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_nb_dynamic_async_fifo_set;
    data_index = dnx_data_nif_flexe_multi_device_mode_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_multi_device_mode_support_set;
    data_index = dnx_data_nif_flexe_core_port_need_alloc;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_core_port_need_alloc_set;
    data_index = dnx_data_nif_flexe_multi_flexe_slot_mode_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_multi_flexe_slot_mode_support_set;
    data_index = dnx_data_nif_flexe_unaware_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_unaware_support_set;
    data_index = dnx_data_nif_flexe_nb_tx_cal_support;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flexe_nb_tx_cal_support_set;

    
    data_index = dnx_data_nif_flexe_table_phy_info;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_flexe_phy_info_set;
    data_index = dnx_data_nif_flexe_table_supported_pms;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_flexe_supported_pms_set;
    data_index = dnx_data_nif_flexe_table_client_speed_granularity;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_flexe_client_speed_granularity_set;
    
    submodule_index = dnx_data_nif_submodule_wbc;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_wbc_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_wbc_is_used_set;

    
    
    submodule_index = dnx_data_nif_submodule_prd;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_prd_define_tcam_key_offset_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_tcam_key_offset_size_set;
    data_index = dnx_data_nif_prd_define_nof_priorities;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_priorities_set;
    data_index = dnx_data_nif_prd_define_rmc_threshold_max;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_rmc_threshold_max_set;
    data_index = dnx_data_nif_prd_define_nof_framer_port_profiles;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_framer_port_profiles_set;
    data_index = dnx_data_nif_prd_define_nof_ofr_port_profiles;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_ofr_port_profiles_set;
    data_index = dnx_data_nif_prd_define_nof_ofr_nif_interfaces;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_ofr_nif_interfaces_set;
    data_index = dnx_data_nif_prd_define_nof_prd_counters;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_prd_nof_prd_counters_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_portmod;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_portmod_define_pm_types_nof;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_portmod_pm_types_nof_set;

    

    
    data_index = dnx_data_nif_portmod_table_pm_types_and_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_portmod_pm_types_and_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_scheduler;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_nif_scheduler_decrement_credit_on_read_enable;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_scheduler_decrement_credit_on_read_enable_set;

    
    
    submodule_index = dnx_data_nif_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_cdu_rx_rmc_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ilu_burst_min_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_ilu_burst_min_entry_size_set;
    data_index = dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_set;
    data_index = dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_set;
    data_index = dnx_data_nif_dbal_define_nof_rx_fifo_counters;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_nof_rx_fifo_counters_set;
    data_index = dnx_data_nif_dbal_define_nof_tx_fifo_counters;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_dbal_nof_tx_fifo_counters_set;

    

    
    
    submodule_index = dnx_data_nif_submodule_arb;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_arb_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_contexts_set;
    data_index = dnx_data_nif_arb_define_nof_rx_qpms;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_rx_qpms_set;
    data_index = dnx_data_nif_arb_define_nof_rx_ppms_in_qpm;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_rx_ppms_in_qpm_set;
    data_index = dnx_data_nif_arb_define_nof_lanes_in_qpm;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_lanes_in_qpm_set;
    data_index = dnx_data_nif_arb_define_nof_map_destinations;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_map_destinations_set;
    data_index = dnx_data_nif_arb_define_max_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_max_calendar_length_set;
    data_index = dnx_data_nif_arb_define_nof_rx_sources;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_rx_sources_set;
    data_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_modes;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_modes_set;
    data_index = dnx_data_nif_arb_define_rx_qpm_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_length_set;
    data_index = dnx_data_nif_arb_define_rx_sch_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_calendar_length_set;
    data_index = dnx_data_nif_arb_define_cdpm_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_cdpm_calendar_length_set;
    data_index = dnx_data_nif_arb_define_tmac_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tmac_calendar_length_set;
    data_index = dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_qpm_calendar_nof_clients_set;
    data_index = dnx_data_nif_arb_define_rx_sch_calendar_nof_clients;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_calendar_nof_clients_set;
    data_index = dnx_data_nif_arb_define_rx_qpm_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_qpm_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_rx_sch_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_sch_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_tx_eth_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_eth_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_tx_tmac_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_tmac_port_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_tx_tmac_nof_sections;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_tmac_nof_sections_set;
    data_index = dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_set;
    data_index = dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_entries_in_tmac_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_entries_in_tmac_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_set;
    data_index = dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_set;
    data_index = dnx_data_nif_arb_define_min_port_speed_for_link_list_section;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_min_port_speed_for_link_list_section_set;
    data_index = dnx_data_nif_arb_define_link_list_sections_denominator;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_link_list_sections_denominator_set;
    data_index = dnx_data_nif_arb_define_high_start_tx_speed_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_high_start_tx_speed_threshold_set;
    data_index = dnx_data_nif_arb_define_low_start_tx_speed_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_low_start_tx_speed_threshold_set;
    data_index = dnx_data_nif_arb_define_high_start_tx_speed_value;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_high_start_tx_speed_value_set;
    data_index = dnx_data_nif_arb_define_middle_start_tx_speed_value;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_middle_start_tx_speed_value_set;
    data_index = dnx_data_nif_arb_define_low_start_tx_speed_value;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_low_start_tx_speed_value_set;
    data_index = dnx_data_nif_arb_define_max_value_for_tx_start_threshold;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_max_value_for_tx_start_threshold_set;
    data_index = dnx_data_nif_arb_define_rx_pm_default_min_pkt_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_arb_rx_pm_default_min_pkt_size_set;

    
    data_index = dnx_data_nif_arb_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_arb_is_supported_set;
    data_index = dnx_data_nif_arb_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_arb_is_used_set;

    
    
    submodule_index = dnx_data_nif_submodule_ofr;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_ofr_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_contexts_set;
    data_index = dnx_data_nif_ofr_define_nof_rmc_per_priority_group;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rmc_per_priority_group_set;
    data_index = dnx_data_nif_ofr_define_nof_total_rmcs;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_total_rmcs_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_mem_sections_per_group_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_memory_groups;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_memory_groups_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_memory_sections;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_memory_sections_set;
    data_index = dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_rx_memory_entries_per_section_set;
    data_index = dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_set;
    data_index = dnx_data_nif_ofr_define_rx_sch_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_rx_sch_granularity_set;
    data_index = dnx_data_nif_ofr_define_nof_bytes_per_memory_section;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_ofr_nof_bytes_per_memory_section_set;

    
    data_index = dnx_data_nif_ofr_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ofr_is_supported_set;
    data_index = dnx_data_nif_ofr_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_ofr_is_used_set;

    
    
    submodule_index = dnx_data_nif_submodule_oft;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_oft_define_calendar_length;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_calendar_length_set;
    data_index = dnx_data_nif_oft_define_nof_sections;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_sections_set;
    data_index = dnx_data_nif_oft_define_nof_internal_calendar_entries;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_internal_calendar_entries_set;
    data_index = dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_set;
    data_index = dnx_data_nif_oft_define_nof_contexts;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_nof_contexts_set;
    data_index = dnx_data_nif_oft_define_port_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_port_speed_granularity_set;
    data_index = dnx_data_nif_oft_define_calendar_speed_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_oft_calendar_speed_granularity_set;

    
    data_index = dnx_data_nif_oft_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_oft_is_supported_set;
    data_index = dnx_data_nif_oft_is_used;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_oft_is_used_set;

    
    
    submodule_index = dnx_data_nif_submodule_signal_quality;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_nif_signal_quality_table_hw_based_solution;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_signal_quality_hw_based_solution_set;
    
    submodule_index = dnx_data_nif_submodule_otn;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_otn_define_nof_units;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_otn_nof_units_set;
    data_index = dnx_data_nif_otn_define_nof_lanes;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_otn_nof_lanes_set;
    data_index = dnx_data_nif_otn_define_nof_groups;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_otn_nof_groups_set;
    data_index = dnx_data_nif_otn_define_nof_lanes_per_group;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_otn_nof_lanes_per_group_set;

    
    data_index = dnx_data_nif_otn_is_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_otn_is_supported_set;

    
    data_index = dnx_data_nif_otn_table_supported_interfaces;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_nif_otn_supported_interfaces_set;
    
    submodule_index = dnx_data_nif_submodule_flr;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flr_define_max_tick_unit;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flr_max_tick_unit_set;
    data_index = dnx_data_nif_flr_define_max_tick_count;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flr_max_tick_count_set;
    data_index = dnx_data_nif_flr_define_timer_granularity;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_nif_flr_timer_granularity_set;

    
    data_index = dnx_data_nif_flr_is_advanced_flr_supported;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_nif_flr_is_advanced_flr_supported_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


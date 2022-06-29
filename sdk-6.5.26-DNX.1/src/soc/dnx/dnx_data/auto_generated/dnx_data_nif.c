
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




extern shr_error_e jr2_a0_data_nif_attach(
    int unit);


extern shr_error_e jr2_b0_data_nif_attach(
    int unit);


extern shr_error_e j2c_a0_data_nif_attach(
    int unit);


extern shr_error_e j2c_a1_data_nif_attach(
    int unit);


extern shr_error_e q2a_a0_data_nif_attach(
    int unit);


extern shr_error_e q2a_b0_data_nif_attach(
    int unit);


extern shr_error_e q2a_b1_data_nif_attach(
    int unit);


extern shr_error_e j2p_a0_data_nif_attach(
    int unit);


extern shr_error_e j2x_a0_data_nif_attach(
    int unit);




static shr_error_e
dnx_data_nif_global_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "global";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_global_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif global features");

    submodule_data->features[dnx_data_nif_global_not_implemented].name = "not_implemented";
    submodule_data->features[dnx_data_nif_global_not_implemented].doc = "";
    submodule_data->features[dnx_data_nif_global_not_implemented].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_reassembler].name = "reassembler";
    submodule_data->features[dnx_data_nif_global_reassembler].doc = "";
    submodule_data->features[dnx_data_nif_global_reassembler].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_clock_power_down].name = "clock_power_down";
    submodule_data->features[dnx_data_nif_global_clock_power_down].doc = "";
    submodule_data->features[dnx_data_nif_global_clock_power_down].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down].name = "ilkn_cdu_fec_separate_clock_power_down";
    submodule_data->features[dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down].doc = "";
    submodule_data->features[dnx_data_nif_global_ilkn_cdu_fec_separate_clock_power_down].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_tx_separate_clock_power_down].name = "tx_separate_clock_power_down";
    submodule_data->features[dnx_data_nif_global_tx_separate_clock_power_down].doc = "";
    submodule_data->features[dnx_data_nif_global_tx_separate_clock_power_down].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_ilkn_credit_loss_handling_mode].name = "ilkn_credit_loss_handling_mode";
    submodule_data->features[dnx_data_nif_global_ilkn_credit_loss_handling_mode].doc = "";
    submodule_data->features[dnx_data_nif_global_ilkn_credit_loss_handling_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_bh_refclk_termination_inverted].name = "bh_refclk_termination_inverted";
    submodule_data->features[dnx_data_nif_global_bh_refclk_termination_inverted].doc = "";
    submodule_data->features[dnx_data_nif_global_bh_refclk_termination_inverted].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_is_shared_arch].name = "is_shared_arch";
    submodule_data->features[dnx_data_nif_global_is_shared_arch].doc = "";
    submodule_data->features[dnx_data_nif_global_is_shared_arch].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_is_ilkn_after_flexe].name = "is_ilkn_after_flexe";
    submodule_data->features[dnx_data_nif_global_is_ilkn_after_flexe].doc = "";
    submodule_data->features[dnx_data_nif_global_is_ilkn_after_flexe].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_mgmt_port_supported].name = "mgmt_port_supported";
    submodule_data->features[dnx_data_nif_global_mgmt_port_supported].doc = "";
    submodule_data->features[dnx_data_nif_global_mgmt_port_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_support_xpmd_if].name = "support_xpmd_if";
    submodule_data->features[dnx_data_nif_global_support_xpmd_if].doc = "";
    submodule_data->features[dnx_data_nif_global_support_xpmd_if].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_is_g_hao_supported].name = "is_g_hao_supported";
    submodule_data->features[dnx_data_nif_global_is_g_hao_supported].doc = "";
    submodule_data->features[dnx_data_nif_global_is_g_hao_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_bh_2x2_supported].name = "bh_2x2_supported";
    submodule_data->features[dnx_data_nif_global_bh_2x2_supported].doc = "";
    submodule_data->features[dnx_data_nif_global_bh_2x2_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_global_eth_use_framer_mgmt_bw].name = "eth_use_framer_mgmt_bw";
    submodule_data->features[dnx_data_nif_global_eth_use_framer_mgmt_bw].doc = "";
    submodule_data->features[dnx_data_nif_global_eth_use_framer_mgmt_bw].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_global_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif global defines");

    submodule_data->defines[dnx_data_nif_global_define_nof_lcplls].name = "nof_lcplls";
    submodule_data->defines[dnx_data_nif_global_define_nof_lcplls].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_nof_lcplls].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_nof_nif_interfaces_per_core].name = "nof_nif_interfaces_per_core";
    submodule_data->defines[dnx_data_nif_global_define_nof_nif_interfaces_per_core].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_nof_nif_interfaces_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity].name = "nif_interface_id_to_unit_id_granularity";
    submodule_data->defines[dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_nof_nif_units_per_core].name = "nof_nif_units_per_core";
    submodule_data->defines[dnx_data_nif_global_define_nof_nif_units_per_core].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_nof_nif_units_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_last_port_led_scan].name = "last_port_led_scan";
    submodule_data->defines[dnx_data_nif_global_define_last_port_led_scan].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_last_port_led_scan].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_reassembler_fifo_threshold].name = "reassembler_fifo_threshold";
    submodule_data->defines[dnx_data_nif_global_define_reassembler_fifo_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_reassembler_fifo_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_total_data_capacity].name = "total_data_capacity";
    submodule_data->defines[dnx_data_nif_global_define_total_data_capacity].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_total_data_capacity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_single_mgmt_port_capacity].name = "single_mgmt_port_capacity";
    submodule_data->defines[dnx_data_nif_global_define_single_mgmt_port_capacity].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_single_mgmt_port_capacity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_max_nof_lanes_per_port].name = "max_nof_lanes_per_port";
    submodule_data->defines[dnx_data_nif_global_define_max_nof_lanes_per_port].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_max_nof_lanes_per_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_ghao_speed_limit].name = "ghao_speed_limit";
    submodule_data->defines[dnx_data_nif_global_define_ghao_speed_limit].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_ghao_speed_limit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_global_define_start_tx_threshold_global].name = "start_tx_threshold_global";
    submodule_data->defines[dnx_data_nif_global_define_start_tx_threshold_global].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_start_tx_threshold_global].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_global_define_l1_only_mode].name = "l1_only_mode";
    submodule_data->defines[dnx_data_nif_global_define_l1_only_mode].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_l1_only_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_global_define_otn_supported].name = "otn_supported";
    submodule_data->defines[dnx_data_nif_global_define_otn_supported].doc = "";
    
    submodule_data->defines[dnx_data_nif_global_define_otn_supported].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_nif_global_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif global tables");

    
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].name = "pll_phys";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].doc = "";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].size_of_values = sizeof(dnx_data_nif_global_pll_phys_t);
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].entry_get = dnx_data_nif_global_pll_phys_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].keys[0].name = "lcpll";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_global_table_pll_phys].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_global_table_pll_phys].nof_values, "_dnx_data_nif_global_table_pll_phys table values");
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[0].name = "min_phy_id";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_global_pll_phys_t, min_phy_id);
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[1].name = "max_phy_id";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_global_table_pll_phys].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_global_pll_phys_t, max_phy_id);

    
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].name = "nif_interface_id_to_unit_id";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].doc = "";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].size_of_values = sizeof(dnx_data_nif_global_nif_interface_id_to_unit_id_t);
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].entry_get = dnx_data_nif_global_nif_interface_id_to_unit_id_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].keys[0].name = "nif_interface_id";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].nof_values, "_dnx_data_nif_global_table_nif_interface_id_to_unit_id table values");
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[0].name = "unit_id";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_global_nif_interface_id_to_unit_id_t, unit_id);
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[1].name = "internal_offset";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_global_table_nif_interface_id_to_unit_id].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_global_nif_interface_id_to_unit_id_t, internal_offset);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_global_feature_get(
    int unit,
    dnx_data_nif_global_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, feature);
}


uint32
dnx_data_nif_global_nof_lcplls_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_nof_lcplls);
}

uint32
dnx_data_nif_global_nof_nif_interfaces_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_nof_nif_interfaces_per_core);
}

uint32
dnx_data_nif_global_nif_interface_id_to_unit_id_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_nif_interface_id_to_unit_id_granularity);
}

uint32
dnx_data_nif_global_nof_nif_units_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_nof_nif_units_per_core);
}

uint32
dnx_data_nif_global_last_port_led_scan_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_last_port_led_scan);
}

uint32
dnx_data_nif_global_reassembler_fifo_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_reassembler_fifo_threshold);
}

uint32
dnx_data_nif_global_total_data_capacity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_total_data_capacity);
}

uint32
dnx_data_nif_global_single_mgmt_port_capacity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_single_mgmt_port_capacity);
}

uint32
dnx_data_nif_global_max_nof_lanes_per_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_max_nof_lanes_per_port);
}

uint32
dnx_data_nif_global_ghao_speed_limit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_ghao_speed_limit);
}

uint32
dnx_data_nif_global_start_tx_threshold_global_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_start_tx_threshold_global);
}

uint32
dnx_data_nif_global_l1_only_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_l1_only_mode);
}

uint32
dnx_data_nif_global_otn_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_define_otn_supported);
}



const dnx_data_nif_global_pll_phys_t *
dnx_data_nif_global_pll_phys_get(
    int unit,
    int lcpll)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_table_pll_phys);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lcpll, 0);
    return (const dnx_data_nif_global_pll_phys_t *) data;

}

const dnx_data_nif_global_nif_interface_id_to_unit_id_t *
dnx_data_nif_global_nif_interface_id_to_unit_id_get(
    int unit,
    int nif_interface_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_table_nif_interface_id_to_unit_id);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, nif_interface_id, 0);
    return (const dnx_data_nif_global_nif_interface_id_to_unit_id_t *) data;

}


shr_error_e
dnx_data_nif_global_pll_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_global_pll_phys_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_table_pll_phys);
    data = (const dnx_data_nif_global_pll_phys_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min_phy_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_phy_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_global_nif_interface_id_to_unit_id_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_global_nif_interface_id_to_unit_id_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_table_nif_interface_id_to_unit_id);
    data = (const dnx_data_nif_global_nif_interface_id_to_unit_id_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->unit_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->internal_offset);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_global_pll_phys_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_table_pll_phys);

}

const dnxc_data_table_info_t *
dnx_data_nif_global_nif_interface_id_to_unit_id_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_global, dnx_data_nif_global_table_nif_interface_id_to_unit_id);

}






static shr_error_e
dnx_data_nif_phys_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "phys";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_phys_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif phys features");

    submodule_data->features[dnx_data_nif_phys_txpi_supported_in_pm8x50].name = "txpi_supported_in_pm8x50";
    submodule_data->features[dnx_data_nif_phys_txpi_supported_in_pm8x50].doc = "";
    submodule_data->features[dnx_data_nif_phys_txpi_supported_in_pm8x50].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_phys_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif phys defines");

    submodule_data->defines[dnx_data_nif_phys_define_nof_phys].name = "nof_phys";
    submodule_data->defines[dnx_data_nif_phys_define_nof_phys].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_nof_phys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_phys_define_max_phys_in_core].name = "max_phys_in_core";
    submodule_data->defines[dnx_data_nif_phys_define_max_phys_in_core].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_max_phys_in_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_phys_define_pm8x50_gen].name = "pm8x50_gen";
    submodule_data->defines[dnx_data_nif_phys_define_pm8x50_gen].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_pm8x50_gen].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_phys_define_is_pam4_speed_supported].name = "is_pam4_speed_supported";
    submodule_data->defines[dnx_data_nif_phys_define_is_pam4_speed_supported].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_is_pam4_speed_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_phys_define_first_mgmt_phy].name = "first_mgmt_phy";
    submodule_data->defines[dnx_data_nif_phys_define_first_mgmt_phy].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_first_mgmt_phy].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_phys_define_nof_mgmt_phys].name = "nof_mgmt_phys";
    submodule_data->defines[dnx_data_nif_phys_define_nof_mgmt_phys].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_nof_mgmt_phys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_phys_define_txpi_sdm_scheme].name = "txpi_sdm_scheme";
    submodule_data->defines[dnx_data_nif_phys_define_txpi_sdm_scheme].doc = "";
    
    submodule_data->defines[dnx_data_nif_phys_define_txpi_sdm_scheme].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_phys_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif phys tables");

    
    submodule_data->tables[dnx_data_nif_phys_table_general].name = "general";
    submodule_data->tables[dnx_data_nif_phys_table_general].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_general].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_phys_table_general].size_of_values = sizeof(dnx_data_nif_phys_general_t);
    submodule_data->tables[dnx_data_nif_phys_table_general].entry_get = dnx_data_nif_phys_general_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_phys_table_general].nof_keys = 0;

    
    submodule_data->tables[dnx_data_nif_phys_table_general].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_phys_table_general].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_phys_table_general].nof_values, "_dnx_data_nif_phys_table_general table values");
    submodule_data->tables[dnx_data_nif_phys_table_general].values[0].name = "supported_phys";
    submodule_data->tables[dnx_data_nif_phys_table_general].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_nif_phys_table_general].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_general].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_general_t, supported_phys);

    
    submodule_data->tables[dnx_data_nif_phys_table_polarity].name = "polarity";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_phys_table_polarity].size_of_values = sizeof(dnx_data_nif_phys_polarity_t);
    submodule_data->tables[dnx_data_nif_phys_table_polarity].entry_get = dnx_data_nif_phys_polarity_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_phys_table_polarity].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_phys_table_polarity].keys[0].name = "lane_index";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_phys_table_polarity].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_phys_table_polarity].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_phys_table_polarity].nof_values, "_dnx_data_nif_phys_table_polarity table values");
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[0].name = "tx_polarity";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_polarity_t, tx_polarity);
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[1].name = "rx_polarity";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_polarity].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_polarity_t, rx_polarity);

    
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].name = "core_phys_map";
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].size_of_values = sizeof(dnx_data_nif_phys_core_phys_map_t);
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].entry_get = dnx_data_nif_phys_core_phys_map_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].keys[0].name = "core_index";
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].nof_values, "_dnx_data_nif_phys_table_core_phys_map table values");
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].values[0].name = "phys";
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_core_phys_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_core_phys_map_t, phys);

    
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].name = "vco_div";
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].size_of_values = sizeof(dnx_data_nif_phys_vco_div_t);
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].entry_get = dnx_data_nif_phys_vco_div_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].keys[0].name = "ethu_index";
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_phys_table_vco_div].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_phys_table_vco_div].nof_values, "_dnx_data_nif_phys_table_vco_div table values");
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].values[0].name = "vco_div";
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_vco_div].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_vco_div_t, vco_div);

    
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].name = "nof_phys_per_core";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].size_of_values = sizeof(dnx_data_nif_phys_nof_phys_per_core_t);
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].entry_get = dnx_data_nif_phys_nof_phys_per_core_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].keys[0].name = "core_index";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].nof_values, "_dnx_data_nif_phys_table_nof_phys_per_core table values");
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[0].name = "nof_phys";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_nof_phys_per_core_t, nof_phys);
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[1].name = "first_phy";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_nof_phys_per_core].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_nof_phys_per_core_t, first_phy);

    
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].name = "txpi_sdm_div";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].size_of_values = sizeof(dnx_data_nif_phys_txpi_sdm_div_t);
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].entry_get = dnx_data_nif_phys_txpi_sdm_div_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].keys[0].name = "index";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].nof_values, "_dnx_data_nif_phys_table_txpi_sdm_div table values");
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_txpi_sdm_div_t, speed);
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[1].name = "val";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_phys_table_txpi_sdm_div].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_phys_txpi_sdm_div_t, val);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_phys_feature_get(
    int unit,
    dnx_data_nif_phys_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, feature);
}


uint32
dnx_data_nif_phys_nof_phys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_nof_phys);
}

uint32
dnx_data_nif_phys_max_phys_in_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_max_phys_in_core);
}

uint32
dnx_data_nif_phys_pm8x50_gen_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_pm8x50_gen);
}

uint32
dnx_data_nif_phys_is_pam4_speed_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_is_pam4_speed_supported);
}

uint32
dnx_data_nif_phys_first_mgmt_phy_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_first_mgmt_phy);
}

uint32
dnx_data_nif_phys_nof_mgmt_phys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_nof_mgmt_phys);
}

uint32
dnx_data_nif_phys_txpi_sdm_scheme_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_define_txpi_sdm_scheme);
}



const dnx_data_nif_phys_general_t *
dnx_data_nif_phys_general_get(
    int unit)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_general);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    return (const dnx_data_nif_phys_general_t *) data;

}

const dnx_data_nif_phys_polarity_t *
dnx_data_nif_phys_polarity_get(
    int unit,
    int lane_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_polarity);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane_index, 0);
    return (const dnx_data_nif_phys_polarity_t *) data;

}

const dnx_data_nif_phys_core_phys_map_t *
dnx_data_nif_phys_core_phys_map_get(
    int unit,
    int core_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_core_phys_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core_index, 0);
    return (const dnx_data_nif_phys_core_phys_map_t *) data;

}

const dnx_data_nif_phys_vco_div_t *
dnx_data_nif_phys_vco_div_get(
    int unit,
    int ethu_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_vco_div);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ethu_index, 0);
    return (const dnx_data_nif_phys_vco_div_t *) data;

}

const dnx_data_nif_phys_nof_phys_per_core_t *
dnx_data_nif_phys_nof_phys_per_core_get(
    int unit,
    int core_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_nof_phys_per_core);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core_index, 0);
    return (const dnx_data_nif_phys_nof_phys_per_core_t *) data;

}

const dnx_data_nif_phys_txpi_sdm_div_t *
dnx_data_nif_phys_txpi_sdm_div_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_txpi_sdm_div);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_nif_phys_txpi_sdm_div_t *) data;

}


shr_error_e
dnx_data_nif_phys_general_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_phys_general_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_general);
    data = (const dnx_data_nif_phys_general_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, 0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->supported_phys);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_phys_polarity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_phys_polarity_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_polarity);
    data = (const dnx_data_nif_phys_polarity_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_polarity);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->rx_polarity);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_phys_core_phys_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_phys_core_phys_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_core_phys_map);
    data = (const dnx_data_nif_phys_core_phys_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->phys);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_phys_vco_div_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_phys_vco_div_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_vco_div);
    data = (const dnx_data_nif_phys_vco_div_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->vco_div);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_phys_nof_phys_per_core_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_phys_nof_phys_per_core_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_nof_phys_per_core);
    data = (const dnx_data_nif_phys_nof_phys_per_core_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_phys);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_phy);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_phys_txpi_sdm_div_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_phys_txpi_sdm_div_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_txpi_sdm_div);
    data = (const dnx_data_nif_phys_txpi_sdm_div_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_phys_general_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_general);

}

const dnxc_data_table_info_t *
dnx_data_nif_phys_polarity_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_polarity);

}

const dnxc_data_table_info_t *
dnx_data_nif_phys_core_phys_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_core_phys_map);

}

const dnxc_data_table_info_t *
dnx_data_nif_phys_vco_div_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_vco_div);

}

const dnxc_data_table_info_t *
dnx_data_nif_phys_nof_phys_per_core_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_nof_phys_per_core);

}

const dnxc_data_table_info_t *
dnx_data_nif_phys_txpi_sdm_div_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_phys, dnx_data_nif_phys_table_txpi_sdm_div);

}






static shr_error_e
dnx_data_nif_ilkn_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ilkn";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_ilkn_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif ilkn features");

    submodule_data->features[dnx_data_nif_ilkn_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_ilu_supported].name = "is_ilu_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_ilu_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_ilu_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_elk_supported].name = "is_elk_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_elk_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_elk_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_credits_selector].name = "credits_selector";
    submodule_data->features[dnx_data_nif_ilkn_credits_selector].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_credits_selector].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_over_fabric_per_core_selector].name = "over_fabric_per_core_selector";
    submodule_data->features[dnx_data_nif_ilkn_over_fabric_per_core_selector].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_over_fabric_per_core_selector].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_cdu_ilkn_selectors].name = "cdu_ilkn_selectors";
    submodule_data->features[dnx_data_nif_ilkn_cdu_ilkn_selectors].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_cdu_ilkn_selectors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_clu_ilkn_selectors].name = "clu_ilkn_selectors";
    submodule_data->features[dnx_data_nif_ilkn_clu_ilkn_selectors].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_clu_ilkn_selectors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_clu_ilkn_direction_selectors].name = "clu_ilkn_direction_selectors";
    submodule_data->features[dnx_data_nif_ilkn_clu_ilkn_direction_selectors].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_clu_ilkn_direction_selectors].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_frmr_ilkn_selector].name = "frmr_ilkn_selector";
    submodule_data->features[dnx_data_nif_ilkn_frmr_ilkn_selector].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_frmr_ilkn_selector].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_support_burst_interleaving].name = "support_burst_interleaving";
    submodule_data->features[dnx_data_nif_ilkn_support_burst_interleaving].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_support_burst_interleaving].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_support_aligner].name = "support_aligner";
    submodule_data->features[dnx_data_nif_ilkn_support_aligner].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_support_aligner].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_scheduler_context_per_port_support].name = "scheduler_context_per_port_support";
    submodule_data->features[dnx_data_nif_ilkn_scheduler_context_per_port_support].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_scheduler_context_per_port_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_support_ilu_burst_max].name = "support_ilu_burst_max";
    submodule_data->features[dnx_data_nif_ilkn_support_ilu_burst_max].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_support_ilu_burst_max].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_fec_supported].name = "is_fec_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_fec_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_fec_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_fec_bypass_supported].name = "is_fec_bypass_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_fec_bypass_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_fec_bypass_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_fec_units_in_cdu].name = "fec_units_in_cdu";
    submodule_data->features[dnx_data_nif_ilkn_fec_units_in_cdu].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_fec_units_in_cdu].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_hrf_flush_support].name = "hrf_flush_support";
    submodule_data->features[dnx_data_nif_ilkn_hrf_flush_support].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_hrf_flush_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_elk_require_core_reset_after_link_training].name = "elk_require_core_reset_after_link_training";
    submodule_data->features[dnx_data_nif_ilkn_elk_require_core_reset_after_link_training].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_elk_require_core_reset_after_link_training].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_reset_core_after_link_training].name = "reset_core_after_link_training";
    submodule_data->features[dnx_data_nif_ilkn_reset_core_after_link_training].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_reset_core_after_link_training].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_ilkn_over_fabric_only].name = "ilkn_over_fabric_only";
    submodule_data->features[dnx_data_nif_ilkn_ilkn_over_fabric_only].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_ilkn_over_fabric_only].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_12lanes_mode].name = "12lanes_mode";
    submodule_data->features[dnx_data_nif_ilkn_12lanes_mode].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_12lanes_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_legacy_lane_format].name = "legacy_lane_format";
    submodule_data->features[dnx_data_nif_ilkn_legacy_lane_format].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_legacy_lane_format].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_nif_56G_speed_supported].name = "is_nif_56G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_nif_56G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_nif_56G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_fabric_56G_speed_supported].name = "is_fabric_56G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_fabric_56G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_fabric_56G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_10p3G_speed_supported].name = "is_10p3G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_10p3G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_10p3G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_12p5G_speed_supported].name = "is_12p5G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_12p5G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_12p5G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_20p6G_speed_supported].name = "is_20p6G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_20p6G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_20p6G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_25G_speed_supported].name = "is_25G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_25G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_25G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_25p7G_speed_supported].name = "is_25p7G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_25p7G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_25p7G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_27p3G_speed_supported].name = "is_27p3G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_27p3G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_27p3G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_28p1G_speed_supported].name = "is_28p1G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_28p1G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_28p1G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_53p1G_speed_supported].name = "is_53p1G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_53p1G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_53p1G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ilkn_is_56p2G_speed_supported].name = "is_56p2G_speed_supported";
    submodule_data->features[dnx_data_nif_ilkn_is_56p2G_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_ilkn_is_56p2G_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_ilkn_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif ilkn defines");

    submodule_data->defines[dnx_data_nif_ilkn_define_ilu_nof].name = "ilu_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_unit_nof].name = "ilkn_unit_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_unit_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_unit_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_unit_if_nof].name = "ilkn_unit_if_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_unit_if_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_unit_if_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_if_nof].name = "ilkn_if_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_if_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_if_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_fec_units_per_core_nof].name = "fec_units_per_core_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_fec_units_per_core_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_fec_units_per_core_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit].name = "nof_lanes_per_fec_unit";
    submodule_data->defines[dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_max_nof].name = "lanes_max_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_max_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_max_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_max_nof_using_fec].name = "lanes_max_nof_using_fec";
    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_max_nof_using_fec].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_max_nof_using_fec].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_allowed_nof].name = "lanes_allowed_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_allowed_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_lanes_allowed_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max].name = "ilkn_over_eth_pms_max";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_segments_max_nof].name = "segments_max_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_segments_max_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_segments_max_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_segments_half_nof].name = "segments_half_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_segments_half_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_segments_half_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_pms_nof].name = "pms_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_pms_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_pms_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_fmac_bus_size].name = "fmac_bus_size";
    submodule_data->defines[dnx_data_nif_ilkn_define_fmac_bus_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_fmac_bus_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof].name = "ilkn_rx_hrf_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof].name = "ilkn_tx_hrf_nof";
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_data_rx_hrf_size].name = "data_rx_hrf_size";
    submodule_data->defines[dnx_data_nif_ilkn_define_data_rx_hrf_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_data_rx_hrf_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_tdm_rx_hrf_size].name = "tdm_rx_hrf_size";
    submodule_data->defines[dnx_data_nif_ilkn_define_tdm_rx_hrf_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_tdm_rx_hrf_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_tx_hrf_credits].name = "tx_hrf_credits";
    submodule_data->defines[dnx_data_nif_ilkn_define_tx_hrf_credits].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_tx_hrf_credits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_nof_rx_hrf_per_port].name = "nof_rx_hrf_per_port";
    submodule_data->defines[dnx_data_nif_ilkn_define_nof_rx_hrf_per_port].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_nof_rx_hrf_per_port].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_high_elk].name = "watermark_high_elk";
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_high_elk].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_high_elk].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_low_elk].name = "watermark_low_elk";
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_low_elk].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_low_elk].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_high_data].name = "watermark_high_data";
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_high_data].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_high_data].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_low_data].name = "watermark_low_data";
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_low_data].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_watermark_low_data].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_pad_size].name = "pad_size";
    submodule_data->defines[dnx_data_nif_ilkn_define_pad_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_pad_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_burst_max_range_max].name = "burst_max_range_max";
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_max_range_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_max_range_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_burst_min].name = "burst_min";
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_min].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_burst_short].name = "burst_short";
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_short].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_short].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_burst_min_range_max].name = "burst_min_range_max";
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_min_range_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_burst_min_range_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ilkn_define_max_nof_ifs].name = "max_nof_ifs";
    submodule_data->defines[dnx_data_nif_ilkn_define_max_nof_ifs].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_max_nof_ifs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_ilkn_define_max_nof_elk_ifs].name = "max_nof_elk_ifs";
    submodule_data->defines[dnx_data_nif_ilkn_define_max_nof_elk_ifs].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_max_nof_elk_ifs].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_ilkn_define_port_0_status_intr_id].name = "port_0_status_intr_id";
    submodule_data->defines[dnx_data_nif_ilkn_define_port_0_status_intr_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_port_0_status_intr_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_ilkn_define_port_1_status_intr_id].name = "port_1_status_intr_id";
    submodule_data->defines[dnx_data_nif_ilkn_define_port_1_status_intr_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_port_1_status_intr_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_ilkn_define_link_stable_wait].name = "link_stable_wait";
    submodule_data->defines[dnx_data_nif_ilkn_define_link_stable_wait].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_link_stable_wait].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_ilkn_define_interleaved_error_drop_single_context].name = "interleaved_error_drop_single_context";
    submodule_data->defines[dnx_data_nif_ilkn_define_interleaved_error_drop_single_context].doc = "";
    
    submodule_data->defines[dnx_data_nif_ilkn_define_interleaved_error_drop_single_context].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_nif_ilkn_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif ilkn tables");

    
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].name = "phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].size_of_values = sizeof(dnx_data_nif_ilkn_phys_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].entry_get = dnx_data_nif_ilkn_phys_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_phys].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_phys].nof_values, "_dnx_data_nif_ilkn_table_phys table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[0].name = "bitmap";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_phys_t, bitmap);
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[1].name = "lanes_array";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[1].type = "uint32[DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF]";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_phys_t, lanes_array);
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[2].name = "array_order_valid";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[2].type = "uint8";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_phys].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_phys_t, array_order_valid);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].name = "supported_phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].size_of_values = sizeof(dnx_data_nif_ilkn_supported_phys_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].entry_get = dnx_data_nif_ilkn_supported_phys_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].nof_values, "_dnx_data_nif_ilkn_table_supported_phys table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[0].name = "is_supported";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_phys_t, is_supported);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[1].name = "max_phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_phys_t, max_phys);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[2].name = "nif_phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[2].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_phys_t, nif_phys);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[3].name = "fabric_phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[3].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_phys].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_phys_t, fabric_phys);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].name = "supported_interfaces";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].size_of_values = sizeof(dnx_data_nif_ilkn_supported_interfaces_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].entry_get = dnx_data_nif_ilkn_supported_interfaces_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].keys[0].name = "index";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].nof_values, "_dnx_data_nif_ilkn_table_supported_interfaces table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_interfaces_t, speed);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[1].name = "pm_dispatch_type";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[1].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_interfaces_t, pm_dispatch_type);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[2].name = "fec_type";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[2].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_interfaces_t, fec_type);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[3].name = "is_valid";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[3].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_interfaces].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_interfaces_t, is_valid);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].name = "ilkn_cores";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].size_of_values = sizeof(dnx_data_nif_ilkn_ilkn_cores_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].entry_get = dnx_data_nif_ilkn_ilkn_cores_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].keys[0].name = "ilkn_core_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].nof_values, "_dnx_data_nif_ilkn_table_ilkn_cores table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[0].name = "is_elk_supported";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_ilkn_cores_t, is_elk_supported);
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[1].name = "is_data_supported";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_ilkn_cores_t, is_data_supported);
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[2].name = "fec_disable_by_bypass";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_ilkn_cores_t, fec_disable_by_bypass);
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[3].name = "fec_dependency";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[3].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_ilkn_cores_t, fec_dependency);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].name = "ilkn_cores_clup_facing";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].size_of_values = sizeof(dnx_data_nif_ilkn_ilkn_cores_clup_facing_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].entry_get = dnx_data_nif_ilkn_ilkn_cores_clup_facing_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].keys[0].name = "ilkn_core_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].nof_values, "_dnx_data_nif_ilkn_table_ilkn_cores_clup_facing table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].values[0].name = "clu_unit";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_ilkn_cores_clup_facing].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_ilkn_cores_clup_facing_t, clu_unit);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].name = "supported_device_core";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].size_of_values = sizeof(dnx_data_nif_ilkn_supported_device_core_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].entry_get = dnx_data_nif_ilkn_supported_device_core_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].nof_values, "_dnx_data_nif_ilkn_table_supported_device_core table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].values[0].name = "core";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_supported_device_core].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_supported_device_core_t, core);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].name = "properties";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].size_of_values = sizeof(dnx_data_nif_ilkn_properties_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].entry_get = dnx_data_nif_ilkn_properties_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].nof_values = 5;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_properties].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_properties].nof_values, "_dnx_data_nif_ilkn_table_properties table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[0].name = "burst_short";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_properties_t, burst_short);
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[1].name = "burst_max";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_properties_t, burst_max);
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[2].name = "burst_min";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_properties_t, burst_min);
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[3].name = "metaframe_period";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[3].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_properties_t, metaframe_period);
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[4].name = "is_over_fabric";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[4].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[4].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_properties].values[4].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_properties_t, is_over_fabric);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].name = "nif_pms";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].size_of_values = sizeof(dnx_data_nif_ilkn_nif_pms_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].entry_get = dnx_data_nif_ilkn_nif_pms_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].keys[0].name = "pm_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].nof_values, "_dnx_data_nif_ilkn_table_nif_pms table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[0].name = "dispatch_type";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[0].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_pms_t, dispatch_type);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[1].name = "first_phy";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_pms_t, first_phy);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[2].name = "nof_phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_pms_t, nof_phys);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[3].name = "facing_core_index";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[3].type = "int";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_pms].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_pms_t, facing_core_index);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].name = "fabric_pms";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].size_of_values = sizeof(dnx_data_nif_ilkn_fabric_pms_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].entry_get = dnx_data_nif_ilkn_fabric_pms_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].keys[0].name = "pm_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].nof_values, "_dnx_data_nif_ilkn_table_fabric_pms table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[0].name = "dispatch_type";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[0].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_pms_t, dispatch_type);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[1].name = "first_phy";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_pms_t, first_phy);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[2].name = "nof_phys";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_pms_t, nof_phys);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[3].name = "facing_core_index";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[3].type = "int";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_pms].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_pms_t, facing_core_index);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].name = "nif_lanes_map";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].size_of_values = sizeof(dnx_data_nif_ilkn_nif_lanes_map_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].entry_get = dnx_data_nif_ilkn_nif_lanes_map_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].nof_keys = 2;
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].keys[0].name = "ilkn_core";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].keys[1].name = "range_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].nof_values, "_dnx_data_nif_ilkn_table_nif_lanes_map table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[0].name = "first_phy";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_lanes_map_t, first_phy);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[1].name = "ilkn_lane_offset";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_lanes_map_t, ilkn_lane_offset);
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[2].name = "nof_lanes";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_nif_lanes_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_nif_lanes_map_t, nof_lanes);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].name = "fabric_lanes_map";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].size_of_values = sizeof(dnx_data_nif_ilkn_fabric_lanes_map_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].entry_get = dnx_data_nif_ilkn_fabric_lanes_map_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].nof_keys = 2;
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].keys[0].name = "ilkn_core";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].keys[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].keys[1].name = "range_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].nof_values, "_dnx_data_nif_ilkn_table_fabric_lanes_map table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[0].name = "first_phy";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_lanes_map_t, first_phy);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[1].name = "ilkn_lane_offset";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_lanes_map_t, ilkn_lane_offset);
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[2].name = "nof_lanes";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_fabric_lanes_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_fabric_lanes_map_t, nof_lanes);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].name = "start_tx_threshold_table";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].size_of_values = sizeof(dnx_data_nif_ilkn_start_tx_threshold_table_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].entry_get = dnx_data_nif_ilkn_start_tx_threshold_table_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].nof_values, "_dnx_data_nif_ilkn_table_start_tx_threshold_table table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_start_tx_threshold_table_t, speed);
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[1].name = "start_thr";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_start_tx_threshold_table].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_start_tx_threshold_table_t, start_thr);

    
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].name = "connectivity_options";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].size_of_values = sizeof(dnx_data_nif_ilkn_connectivity_options_t);
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].entry_get = dnx_data_nif_ilkn_connectivity_options_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].nof_keys = 2;
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].keys[0].name = "ilkn_id";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].keys[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].keys[1].name = "connectivity_mode";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].nof_values, "_dnx_data_nif_ilkn_table_connectivity_options table values");
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[0].name = "first_phy";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_connectivity_options_t, first_phy);
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[1].name = "last_phy";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_ilkn_table_connectivity_options].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_ilkn_connectivity_options_t, last_phy);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_ilkn_feature_get(
    int unit,
    dnx_data_nif_ilkn_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, feature);
}


uint32
dnx_data_nif_ilkn_ilu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilu_nof);
}

uint32
dnx_data_nif_ilkn_ilkn_unit_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilkn_unit_nof);
}

uint32
dnx_data_nif_ilkn_ilkn_unit_if_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilkn_unit_if_nof);
}

uint32
dnx_data_nif_ilkn_ilkn_if_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilkn_if_nof);
}

uint32
dnx_data_nif_ilkn_fec_units_per_core_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_fec_units_per_core_nof);
}

uint32
dnx_data_nif_ilkn_nof_lanes_per_fec_unit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_nof_lanes_per_fec_unit);
}

uint32
dnx_data_nif_ilkn_lanes_max_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_lanes_max_nof);
}

uint32
dnx_data_nif_ilkn_lanes_max_nof_using_fec_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_lanes_max_nof_using_fec);
}

uint32
dnx_data_nif_ilkn_lanes_allowed_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_lanes_allowed_nof);
}

uint32
dnx_data_nif_ilkn_ilkn_over_eth_pms_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilkn_over_eth_pms_max);
}

uint32
dnx_data_nif_ilkn_segments_max_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_segments_max_nof);
}

uint32
dnx_data_nif_ilkn_segments_half_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_segments_half_nof);
}

uint32
dnx_data_nif_ilkn_pms_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_pms_nof);
}

uint32
dnx_data_nif_ilkn_fmac_bus_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_fmac_bus_size);
}

uint32
dnx_data_nif_ilkn_ilkn_rx_hrf_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilkn_rx_hrf_nof);
}

uint32
dnx_data_nif_ilkn_ilkn_tx_hrf_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_ilkn_tx_hrf_nof);
}

uint32
dnx_data_nif_ilkn_data_rx_hrf_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_data_rx_hrf_size);
}

uint32
dnx_data_nif_ilkn_tdm_rx_hrf_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_tdm_rx_hrf_size);
}

uint32
dnx_data_nif_ilkn_tx_hrf_credits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_tx_hrf_credits);
}

uint32
dnx_data_nif_ilkn_nof_rx_hrf_per_port_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_nof_rx_hrf_per_port);
}

uint32
dnx_data_nif_ilkn_watermark_high_elk_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_watermark_high_elk);
}

uint32
dnx_data_nif_ilkn_watermark_low_elk_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_watermark_low_elk);
}

uint32
dnx_data_nif_ilkn_watermark_high_data_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_watermark_high_data);
}

uint32
dnx_data_nif_ilkn_watermark_low_data_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_watermark_low_data);
}

uint32
dnx_data_nif_ilkn_pad_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_pad_size);
}

uint32
dnx_data_nif_ilkn_burst_max_range_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_burst_max_range_max);
}

uint32
dnx_data_nif_ilkn_burst_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_burst_min);
}

uint32
dnx_data_nif_ilkn_burst_short_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_burst_short);
}

uint32
dnx_data_nif_ilkn_burst_min_range_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_burst_min_range_max);
}

uint32
dnx_data_nif_ilkn_max_nof_ifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_max_nof_ifs);
}

uint32
dnx_data_nif_ilkn_max_nof_elk_ifs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_max_nof_elk_ifs);
}

uint32
dnx_data_nif_ilkn_port_0_status_intr_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_port_0_status_intr_id);
}

uint32
dnx_data_nif_ilkn_port_1_status_intr_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_port_1_status_intr_id);
}

uint32
dnx_data_nif_ilkn_link_stable_wait_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_link_stable_wait);
}

uint32
dnx_data_nif_ilkn_interleaved_error_drop_single_context_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_define_interleaved_error_drop_single_context);
}



const dnx_data_nif_ilkn_phys_t *
dnx_data_nif_ilkn_phys_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_phys);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_nif_ilkn_phys_t *) data;

}

const dnx_data_nif_ilkn_supported_phys_t *
dnx_data_nif_ilkn_supported_phys_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_phys);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_nif_ilkn_supported_phys_t *) data;

}

const dnx_data_nif_ilkn_supported_interfaces_t *
dnx_data_nif_ilkn_supported_interfaces_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_interfaces);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_nif_ilkn_supported_interfaces_t *) data;

}

const dnx_data_nif_ilkn_ilkn_cores_t *
dnx_data_nif_ilkn_ilkn_cores_get(
    int unit,
    int ilkn_core_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_ilkn_cores);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_id, 0);
    return (const dnx_data_nif_ilkn_ilkn_cores_t *) data;

}

const dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *
dnx_data_nif_ilkn_ilkn_cores_clup_facing_get(
    int unit,
    int ilkn_core_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_ilkn_cores_clup_facing);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_core_id, 0);
    return (const dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) data;

}

const dnx_data_nif_ilkn_supported_device_core_t *
dnx_data_nif_ilkn_supported_device_core_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_device_core);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_nif_ilkn_supported_device_core_t *) data;

}

const dnx_data_nif_ilkn_properties_t *
dnx_data_nif_ilkn_properties_get(
    int unit,
    int ilkn_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_properties);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, 0);
    return (const dnx_data_nif_ilkn_properties_t *) data;

}

const dnx_data_nif_ilkn_nif_pms_t *
dnx_data_nif_ilkn_nif_pms_get(
    int unit,
    int pm_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_nif_pms);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_id, 0);
    return (const dnx_data_nif_ilkn_nif_pms_t *) data;

}

const dnx_data_nif_ilkn_fabric_pms_t *
dnx_data_nif_ilkn_fabric_pms_get(
    int unit,
    int pm_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_fabric_pms);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_id, 0);
    return (const dnx_data_nif_ilkn_fabric_pms_t *) data;

}

const dnx_data_nif_ilkn_nif_lanes_map_t *
dnx_data_nif_ilkn_nif_lanes_map_get(
    int unit,
    int ilkn_core,
    int range_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_nif_lanes_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_core, range_id);
    return (const dnx_data_nif_ilkn_nif_lanes_map_t *) data;

}

const dnx_data_nif_ilkn_fabric_lanes_map_t *
dnx_data_nif_ilkn_fabric_lanes_map_get(
    int unit,
    int ilkn_core,
    int range_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_fabric_lanes_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_core, range_id);
    return (const dnx_data_nif_ilkn_fabric_lanes_map_t *) data;

}

const dnx_data_nif_ilkn_start_tx_threshold_table_t *
dnx_data_nif_ilkn_start_tx_threshold_table_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_start_tx_threshold_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_ilkn_start_tx_threshold_table_t *) data;

}

const dnx_data_nif_ilkn_connectivity_options_t *
dnx_data_nif_ilkn_connectivity_options_get(
    int unit,
    int ilkn_id,
    int connectivity_mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_connectivity_options);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ilkn_id, connectivity_mode);
    return (const dnx_data_nif_ilkn_connectivity_options_t *) data;

}


shr_error_e
dnx_data_nif_ilkn_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_phys_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_phys);
    data = (const dnx_data_nif_ilkn_phys_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->bitmap);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_NIF_ILKN_LANES_MAX_NOF, data->lanes_array);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->array_order_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_supported_phys_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_supported_phys_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_phys);
    data = (const dnx_data_nif_ilkn_supported_phys_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_supported);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_phys);
            break;
        case 2:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->nif_phys);
            break;
        case 3:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->fabric_phys);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_supported_interfaces_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_interfaces);
    data = (const dnx_data_nif_ilkn_supported_interfaces_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pm_dispatch_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_ilkn_cores_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_ilkn_cores_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_ilkn_cores);
    data = (const dnx_data_nif_ilkn_ilkn_cores_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_elk_supported);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_data_supported);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_disable_by_bypass);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_dependency);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_ilkn_cores_clup_facing_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_ilkn_cores_clup_facing);
    data = (const dnx_data_nif_ilkn_ilkn_cores_clup_facing_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->clu_unit);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_supported_device_core_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_supported_device_core_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_device_core);
    data = (const dnx_data_nif_ilkn_supported_device_core_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->core);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_properties_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_properties);
    data = (const dnx_data_nif_ilkn_properties_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->burst_short);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->burst_max);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->burst_min);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->metaframe_period);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_over_fabric);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_nif_pms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_nif_pms_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_nif_pms);
    data = (const dnx_data_nif_ilkn_nif_pms_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dispatch_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_phy);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_phys);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->facing_core_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_fabric_pms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_fabric_pms_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_fabric_pms);
    data = (const dnx_data_nif_ilkn_fabric_pms_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dispatch_type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_phy);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_phys);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->facing_core_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_nif_lanes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_nif_lanes_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_nif_lanes_map);
    data = (const dnx_data_nif_ilkn_nif_lanes_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_phy);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ilkn_lane_offset);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_fabric_lanes_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_fabric_lanes_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_fabric_lanes_map);
    data = (const dnx_data_nif_ilkn_fabric_lanes_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_phy);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ilkn_lane_offset);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_start_tx_threshold_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_start_tx_threshold_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_start_tx_threshold_table);
    data = (const dnx_data_nif_ilkn_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_thr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_ilkn_connectivity_options_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_ilkn_connectivity_options_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_connectivity_options);
    data = (const dnx_data_nif_ilkn_connectivity_options_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_phy);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->last_phy);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_ilkn_phys_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_phys);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_supported_phys_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_phys);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_supported_interfaces_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_interfaces);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_ilkn_cores_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_ilkn_cores);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_ilkn_cores_clup_facing_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_ilkn_cores_clup_facing);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_supported_device_core_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_supported_device_core);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_properties_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_properties);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_nif_pms_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_nif_pms);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_fabric_pms_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_fabric_pms);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_nif_lanes_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_nif_lanes_map);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_fabric_lanes_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_fabric_lanes_map);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_start_tx_threshold_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_start_tx_threshold_table);

}

const dnxc_data_table_info_t *
dnx_data_nif_ilkn_connectivity_options_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ilkn, dnx_data_nif_ilkn_table_connectivity_options);

}






static shr_error_e
dnx_data_nif_eth_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "eth";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_eth_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif eth features");

    submodule_data->features[dnx_data_nif_eth_nif_scheduler_context_per_rmc_support].name = "nif_scheduler_context_per_rmc_support";
    submodule_data->features[dnx_data_nif_eth_nif_scheduler_context_per_rmc_support].doc = "";
    submodule_data->features[dnx_data_nif_eth_nif_scheduler_context_per_rmc_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_is_rmc_allocation_needed].name = "is_rmc_allocation_needed";
    submodule_data->features[dnx_data_nif_eth_is_rmc_allocation_needed].doc = "";
    submodule_data->features[dnx_data_nif_eth_is_rmc_allocation_needed].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_rmc_flush_support].name = "rmc_flush_support";
    submodule_data->features[dnx_data_nif_eth_rmc_flush_support].doc = "";
    submodule_data->features[dnx_data_nif_eth_rmc_flush_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_is_rmc_low_priority_needs_configuration].name = "is_rmc_low_priority_needs_configuration";
    submodule_data->features[dnx_data_nif_eth_is_rmc_low_priority_needs_configuration].doc = "";
    submodule_data->features[dnx_data_nif_eth_is_rmc_low_priority_needs_configuration].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_is_clu_supported].name = "is_clu_supported";
    submodule_data->features[dnx_data_nif_eth_is_clu_supported].doc = "";
    submodule_data->features[dnx_data_nif_eth_is_clu_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_is_rx_port_mode_needs_configuration].name = "is_rx_port_mode_needs_configuration";
    submodule_data->features[dnx_data_nif_eth_is_rx_port_mode_needs_configuration].doc = "";
    submodule_data->features[dnx_data_nif_eth_is_rx_port_mode_needs_configuration].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_pcs_lane_map_reconfig_support].name = "pcs_lane_map_reconfig_support";
    submodule_data->features[dnx_data_nif_eth_pcs_lane_map_reconfig_support].doc = "";
    submodule_data->features[dnx_data_nif_eth_pcs_lane_map_reconfig_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_gear_shifter_exists].name = "gear_shifter_exists";
    submodule_data->features[dnx_data_nif_eth_gear_shifter_exists].doc = "";
    submodule_data->features[dnx_data_nif_eth_gear_shifter_exists].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_pm8x50_bw_update].name = "pm8x50_bw_update";
    submodule_data->features[dnx_data_nif_eth_pm8x50_bw_update].doc = "";
    submodule_data->features[dnx_data_nif_eth_pm8x50_bw_update].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_pm8x50_clk_repeater_disable].name = "pm8x50_clk_repeater_disable";
    submodule_data->features[dnx_data_nif_eth_pm8x50_clk_repeater_disable].doc = "";
    submodule_data->features[dnx_data_nif_eth_pm8x50_clk_repeater_disable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_eth_portmod_thread_disable].name = "portmod_thread_disable";
    submodule_data->features[dnx_data_nif_eth_portmod_thread_disable].doc = "";
    submodule_data->features[dnx_data_nif_eth_portmod_thread_disable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_eth_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif eth defines");

    submodule_data->defines[dnx_data_nif_eth_define_cdu_nof].name = "cdu_nof";
    submodule_data->defines[dnx_data_nif_eth_define_cdu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_cdu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_clu_nof].name = "clu_nof";
    submodule_data->defines[dnx_data_nif_eth_define_clu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_clu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nbu_nof].name = "nbu_nof";
    submodule_data->defines[dnx_data_nif_eth_define_nbu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nbu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_mgu_nof].name = "mgu_nof";
    submodule_data->defines[dnx_data_nif_eth_define_mgu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_mgu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_cdum_nof].name = "cdum_nof";
    submodule_data->defines[dnx_data_nif_eth_define_cdum_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_cdum_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_cdu].name = "nof_pms_in_cdu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_cdu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_cdu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_clu].name = "nof_pms_in_clu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_clu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_clu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_nbu].name = "nof_pms_in_nbu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_nbu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_nbu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_mgu].name = "nof_pms_in_mgu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_mgu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_pms_in_mgu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_total_nof_ethu_pms_in_device].name = "total_nof_ethu_pms_in_device";
    submodule_data->defines[dnx_data_nif_eth_define_total_nof_ethu_pms_in_device].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_total_nof_ethu_pms_in_device].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_cdu_lanes_in_pm].name = "nof_cdu_lanes_in_pm";
    submodule_data->defines[dnx_data_nif_eth_define_nof_cdu_lanes_in_pm].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_cdu_lanes_in_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_clu_lanes_in_pm].name = "nof_clu_lanes_in_pm";
    submodule_data->defines[dnx_data_nif_eth_define_nof_clu_lanes_in_pm].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_clu_lanes_in_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_cdu_pms].name = "nof_cdu_pms";
    submodule_data->defines[dnx_data_nif_eth_define_nof_cdu_pms].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_cdu_pms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_clu_pms].name = "nof_clu_pms";
    submodule_data->defines[dnx_data_nif_eth_define_nof_clu_pms].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_clu_pms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_nbu_lanes_in_pm].name = "nof_nbu_lanes_in_pm";
    submodule_data->defines[dnx_data_nif_eth_define_nof_nbu_lanes_in_pm].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_nbu_lanes_in_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_mgu_lanes_in_pm].name = "nof_mgu_lanes_in_pm";
    submodule_data->defines[dnx_data_nif_eth_define_nof_mgu_lanes_in_pm].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_mgu_lanes_in_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_nbu_pms].name = "nof_nbu_pms";
    submodule_data->defines[dnx_data_nif_eth_define_nof_nbu_pms].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_nbu_pms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_mgu_pms].name = "nof_mgu_pms";
    submodule_data->defines[dnx_data_nif_eth_define_nof_mgu_pms].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_mgu_pms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_ethu_nof].name = "ethu_nof";
    submodule_data->defines[dnx_data_nif_eth_define_ethu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_ethu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_cdu_nof_per_core].name = "cdu_nof_per_core";
    submodule_data->defines[dnx_data_nif_eth_define_cdu_nof_per_core].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_cdu_nof_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_clu_nof_per_core].name = "clu_nof_per_core";
    submodule_data->defines[dnx_data_nif_eth_define_clu_nof_per_core].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_clu_nof_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_max_ethu_nof_per_core].name = "max_ethu_nof_per_core";
    submodule_data->defines[dnx_data_nif_eth_define_max_ethu_nof_per_core].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_max_ethu_nof_per_core].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_cdu].name = "nof_lanes_in_cdu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_cdu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_cdu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_clu].name = "nof_lanes_in_clu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_clu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_clu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_nbu].name = "nof_lanes_in_nbu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_nbu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_nbu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_mgu].name = "nof_lanes_in_mgu";
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_mgu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_lanes_in_mgu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_max_nof_lanes_in_ethu].name = "max_nof_lanes_in_ethu";
    submodule_data->defines[dnx_data_nif_eth_define_max_nof_lanes_in_ethu].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_max_nof_lanes_in_ethu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device].name = "total_nof_cdu_lanes_in_device";
    submodule_data->defines[dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_total_nof_nbu_lanes_in_device].name = "total_nof_nbu_lanes_in_device";
    submodule_data->defines[dnx_data_nif_eth_define_total_nof_nbu_lanes_in_device].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_total_nof_nbu_lanes_in_device].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_cdu_mac_mode_config_nof].name = "cdu_mac_mode_config_nof";
    submodule_data->defines[dnx_data_nif_eth_define_cdu_mac_mode_config_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_cdu_mac_mode_config_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nbu_mac_mode_config_nof].name = "nbu_mac_mode_config_nof";
    submodule_data->defines[dnx_data_nif_eth_define_nbu_mac_mode_config_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nbu_mac_mode_config_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_mac_mode_config_lanes_nof].name = "mac_mode_config_lanes_nof";
    submodule_data->defines[dnx_data_nif_eth_define_mac_mode_config_lanes_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_mac_mode_config_lanes_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_cdu_mac_nof].name = "cdu_mac_nof";
    submodule_data->defines[dnx_data_nif_eth_define_cdu_mac_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_cdu_mac_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nbu_mac_nof].name = "nbu_mac_nof";
    submodule_data->defines[dnx_data_nif_eth_define_nbu_mac_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nbu_mac_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_mac_lanes_nof].name = "mac_lanes_nof";
    submodule_data->defines[dnx_data_nif_eth_define_mac_lanes_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_mac_lanes_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_ethu_logical_fifo_nof].name = "ethu_logical_fifo_nof";
    submodule_data->defines[dnx_data_nif_eth_define_ethu_logical_fifo_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_ethu_logical_fifo_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_cdu_memory_entries_nof].name = "cdu_memory_entries_nof";
    submodule_data->defines[dnx_data_nif_eth_define_cdu_memory_entries_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_cdu_memory_entries_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_clu_memory_entries_nof].name = "clu_memory_entries_nof";
    submodule_data->defines[dnx_data_nif_eth_define_clu_memory_entries_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_clu_memory_entries_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nbu_memory_entries_nof].name = "nbu_memory_entries_nof";
    submodule_data->defines[dnx_data_nif_eth_define_nbu_memory_entries_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nbu_memory_entries_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_mgu_memory_entries_nof].name = "mgu_memory_entries_nof";
    submodule_data->defines[dnx_data_nif_eth_define_mgu_memory_entries_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_mgu_memory_entries_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_priority_group_nof_entries_min].name = "priority_group_nof_entries_min";
    submodule_data->defines[dnx_data_nif_eth_define_priority_group_nof_entries_min].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_priority_group_nof_entries_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_priority_groups_nof].name = "priority_groups_nof";
    submodule_data->defines[dnx_data_nif_eth_define_priority_groups_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_priority_groups_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_nof_rmcs_per_priority_group].name = "nof_rmcs_per_priority_group";
    submodule_data->defines[dnx_data_nif_eth_define_nof_rmcs_per_priority_group].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_nof_rmcs_per_priority_group].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_pad_size_min].name = "pad_size_min";
    submodule_data->defines[dnx_data_nif_eth_define_pad_size_min].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_pad_size_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_pad_size_max].name = "pad_size_max";
    submodule_data->defines[dnx_data_nif_eth_define_pad_size_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_pad_size_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_packet_size_max].name = "packet_size_max";
    submodule_data->defines[dnx_data_nif_eth_define_packet_size_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_packet_size_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_an_max_nof_abilities].name = "an_max_nof_abilities";
    submodule_data->defines[dnx_data_nif_eth_define_an_max_nof_abilities].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_an_max_nof_abilities].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_phy_map_granularity].name = "phy_map_granularity";
    submodule_data->defines[dnx_data_nif_eth_define_phy_map_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_phy_map_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_vco_factor_for_53p125_rate].name = "vco_factor_for_53p125_rate";
    submodule_data->defines[dnx_data_nif_eth_define_vco_factor_for_53p125_rate].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_vco_factor_for_53p125_rate].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_vco_factor_for_51p5625_rate].name = "vco_factor_for_51p5625_rate";
    submodule_data->defines[dnx_data_nif_eth_define_vco_factor_for_51p5625_rate].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_vco_factor_for_51p5625_rate].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_is_400G_supported].name = "is_400G_supported";
    submodule_data->defines[dnx_data_nif_eth_define_is_400G_supported].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_is_400G_supported].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_eth_define_is_800G_supported].name = "is_800G_supported";
    submodule_data->defines[dnx_data_nif_eth_define_is_800G_supported].doc = "";
    
    submodule_data->defines[dnx_data_nif_eth_define_is_800G_supported].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_eth_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif eth tables");

    
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].name = "start_tx_threshold_table";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].size_of_values = sizeof(dnx_data_nif_eth_start_tx_threshold_table_t);
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].entry_get = dnx_data_nif_eth_start_tx_threshold_table_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].nof_values, "_dnx_data_nif_eth_table_start_tx_threshold_table table values");
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_start_tx_threshold_table_t, speed);
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[1].name = "start_thr";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_start_tx_threshold_table_t, start_thr);
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[2].name = "start_thr_for_l1";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_start_tx_threshold_table].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_start_tx_threshold_table_t, start_thr_for_l1);

    
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].name = "pm_properties";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].size_of_values = sizeof(dnx_data_nif_eth_pm_properties_t);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].entry_get = dnx_data_nif_eth_pm_properties_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].keys[0].name = "pm_index";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].nof_values = 9;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_pm_properties].nof_values, "_dnx_data_nif_eth_table_pm_properties table values");
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[0].name = "phys";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[0].type = "bcm_pbmp_t";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, phys);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[1].name = "tvco_pll_index";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, tvco_pll_index);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[2].name = "phy_addr";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[2].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, phy_addr);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[3].name = "is_master";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[3].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, is_master);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[4].name = "dispatch_type";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[4].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[4].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[4].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, dispatch_type);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[5].name = "special_pll_check";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[5].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[5].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[5].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, special_pll_check);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[6].name = "flr_support";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[6].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[6].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[6].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, flr_support);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[7].name = "ext_txpi_support";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[7].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[7].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[7].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, ext_txpi_support);
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[8].name = "link_recovery_support";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[8].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[8].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_pm_properties].values[8].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_pm_properties_t, link_recovery_support);

    
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].name = "ethu_properties";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].size_of_values = sizeof(dnx_data_nif_eth_ethu_properties_t);
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].entry_get = dnx_data_nif_eth_ethu_properties_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].keys[0].name = "ethu_index";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].nof_values, "_dnx_data_nif_eth_table_ethu_properties table values");
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[0].name = "pms";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[0].type = "int[DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE]";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_ethu_properties_t, pms);
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[1].name = "type";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[1].type = "imb_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_ethu_properties_t, type);
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[2].name = "type_index";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[2].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_properties].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_ethu_properties_t, type_index);

    
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].name = "phy_map";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].size_of_values = sizeof(dnx_data_nif_eth_phy_map_t);
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].entry_get = dnx_data_nif_eth_phy_map_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_phy_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_phy_map].nof_values, "_dnx_data_nif_eth_table_phy_map table values");
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[0].name = "phy_index";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_phy_map_t, phy_index);
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[1].name = "ethu_index";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_phy_map_t, ethu_index);
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[2].name = "pm_index";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[2].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_phy_map].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_phy_map_t, pm_index);

    
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].name = "is_nif_loopback_supported";
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].size_of_values = sizeof(dnx_data_nif_eth_is_nif_loopback_supported_t);
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].entry_get = dnx_data_nif_eth_is_nif_loopback_supported_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].keys[0].name = "type";
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].nof_values, "_dnx_data_nif_eth_table_is_nif_loopback_supported table values");
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].values[0].name = "is_supported";
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_is_nif_loopback_supported].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_is_nif_loopback_supported_t, is_supported);

    
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].name = "max_speed";
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].size_of_values = sizeof(dnx_data_nif_eth_max_speed_t);
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].entry_get = dnx_data_nif_eth_max_speed_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].keys[0].name = "lane_num";
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_max_speed].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_max_speed].nof_values, "_dnx_data_nif_eth_table_max_speed table values");
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_max_speed].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_max_speed_t, speed);

    
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].name = "supported_interfaces";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].size_of_values = sizeof(dnx_data_nif_eth_supported_interfaces_t);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].entry_get = dnx_data_nif_eth_supported_interfaces_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].nof_values, "_dnx_data_nif_eth_table_supported_interfaces table values");
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, speed);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[1].name = "nof_lanes";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, nof_lanes);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[2].name = "fec_type";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[2].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, fec_type);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[3].name = "dispatch_type";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[3].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, dispatch_type);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[4].name = "is_valid";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[4].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[4].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[4].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, is_valid);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[5].name = "ptp_support";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[5].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[5].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[5].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, ptp_support);
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[6].name = "autoneg_support";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[6].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[6].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_interfaces].values[6].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_interfaces_t, autoneg_support);

    
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].name = "ethu_per_core";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].size_of_values = sizeof(dnx_data_nif_eth_ethu_per_core_t);
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].entry_get = dnx_data_nif_eth_ethu_per_core_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].keys[0].name = "ethu_index";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].nof_values, "_dnx_data_nif_eth_table_ethu_per_core table values");
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].values[0].name = "core_id";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_ethu_per_core].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_ethu_per_core_t, core_id);

    
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].name = "nif_cores_ethus";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].size_of_values = sizeof(dnx_data_nif_eth_nif_cores_ethus_t);
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].entry_get = dnx_data_nif_eth_nif_cores_ethus_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].keys[0].name = "core_id";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].nof_values, "_dnx_data_nif_eth_table_nif_cores_ethus table values");
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[0].name = "nof_ethus";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_nif_cores_ethus_t, nof_ethus);
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[1].name = "first_ethu";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_nif_cores_ethus].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_nif_cores_ethus_t, first_ethu);

    
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].name = "supported_clu_an_abilities";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].size_of_values = sizeof(dnx_data_nif_eth_supported_clu_an_abilities_t);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].entry_get = dnx_data_nif_eth_supported_clu_an_abilities_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].nof_values, "_dnx_data_nif_eth_table_supported_clu_an_abilities table values");
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[0].name = "an_mode";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, an_mode);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[1].name = "speed";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, speed);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[2].name = "nof_lanes";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[2].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, nof_lanes);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[3].name = "fec_type";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[3].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, fec_type);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[4].name = "medium";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[4].type = "bcm_port_medium_t";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[4].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[4].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, medium);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[5].name = "channel";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[5].type = "bcm_port_phy_channel_t";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[5].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[5].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, channel);
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[6].name = "is_valid";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[6].type = "int";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[6].doc = "";
    submodule_data->tables[dnx_data_nif_eth_table_supported_clu_an_abilities].values[6].offset = UTILEX_OFFSETOF(dnx_data_nif_eth_supported_clu_an_abilities_t, is_valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_eth_feature_get(
    int unit,
    dnx_data_nif_eth_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, feature);
}


uint32
dnx_data_nif_eth_cdu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_cdu_nof);
}

uint32
dnx_data_nif_eth_clu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_clu_nof);
}

uint32
dnx_data_nif_eth_nbu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nbu_nof);
}

uint32
dnx_data_nif_eth_mgu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_mgu_nof);
}

uint32
dnx_data_nif_eth_cdum_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_cdum_nof);
}

uint32
dnx_data_nif_eth_nof_pms_in_cdu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_pms_in_cdu);
}

uint32
dnx_data_nif_eth_nof_pms_in_clu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_pms_in_clu);
}

uint32
dnx_data_nif_eth_nof_pms_in_nbu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_pms_in_nbu);
}

uint32
dnx_data_nif_eth_nof_pms_in_mgu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_pms_in_mgu);
}

uint32
dnx_data_nif_eth_total_nof_ethu_pms_in_device_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_total_nof_ethu_pms_in_device);
}

uint32
dnx_data_nif_eth_nof_cdu_lanes_in_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_cdu_lanes_in_pm);
}

uint32
dnx_data_nif_eth_nof_clu_lanes_in_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_clu_lanes_in_pm);
}

uint32
dnx_data_nif_eth_nof_cdu_pms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_cdu_pms);
}

uint32
dnx_data_nif_eth_nof_clu_pms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_clu_pms);
}

uint32
dnx_data_nif_eth_nof_nbu_lanes_in_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_nbu_lanes_in_pm);
}

uint32
dnx_data_nif_eth_nof_mgu_lanes_in_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_mgu_lanes_in_pm);
}

uint32
dnx_data_nif_eth_nof_nbu_pms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_nbu_pms);
}

uint32
dnx_data_nif_eth_nof_mgu_pms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_mgu_pms);
}

uint32
dnx_data_nif_eth_ethu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_ethu_nof);
}

uint32
dnx_data_nif_eth_cdu_nof_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_cdu_nof_per_core);
}

uint32
dnx_data_nif_eth_clu_nof_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_clu_nof_per_core);
}

uint32
dnx_data_nif_eth_max_ethu_nof_per_core_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_max_ethu_nof_per_core);
}

uint32
dnx_data_nif_eth_nof_lanes_in_cdu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_lanes_in_cdu);
}

uint32
dnx_data_nif_eth_nof_lanes_in_clu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_lanes_in_clu);
}

uint32
dnx_data_nif_eth_nof_lanes_in_nbu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_lanes_in_nbu);
}

uint32
dnx_data_nif_eth_nof_lanes_in_mgu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_lanes_in_mgu);
}

uint32
dnx_data_nif_eth_max_nof_lanes_in_ethu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_max_nof_lanes_in_ethu);
}

uint32
dnx_data_nif_eth_total_nof_cdu_lanes_in_device_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_total_nof_cdu_lanes_in_device);
}

uint32
dnx_data_nif_eth_total_nof_nbu_lanes_in_device_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_total_nof_nbu_lanes_in_device);
}

uint32
dnx_data_nif_eth_cdu_mac_mode_config_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_cdu_mac_mode_config_nof);
}

uint32
dnx_data_nif_eth_nbu_mac_mode_config_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nbu_mac_mode_config_nof);
}

uint32
dnx_data_nif_eth_mac_mode_config_lanes_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_mac_mode_config_lanes_nof);
}

uint32
dnx_data_nif_eth_cdu_mac_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_cdu_mac_nof);
}

uint32
dnx_data_nif_eth_nbu_mac_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nbu_mac_nof);
}

uint32
dnx_data_nif_eth_mac_lanes_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_mac_lanes_nof);
}

uint32
dnx_data_nif_eth_ethu_logical_fifo_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_ethu_logical_fifo_nof);
}

uint32
dnx_data_nif_eth_cdu_memory_entries_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_cdu_memory_entries_nof);
}

uint32
dnx_data_nif_eth_clu_memory_entries_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_clu_memory_entries_nof);
}

uint32
dnx_data_nif_eth_nbu_memory_entries_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nbu_memory_entries_nof);
}

uint32
dnx_data_nif_eth_mgu_memory_entries_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_mgu_memory_entries_nof);
}

uint32
dnx_data_nif_eth_priority_group_nof_entries_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_priority_group_nof_entries_min);
}

uint32
dnx_data_nif_eth_priority_groups_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_priority_groups_nof);
}

uint32
dnx_data_nif_eth_nof_rmcs_per_priority_group_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_nof_rmcs_per_priority_group);
}

uint32
dnx_data_nif_eth_pad_size_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_pad_size_min);
}

uint32
dnx_data_nif_eth_pad_size_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_pad_size_max);
}

uint32
dnx_data_nif_eth_packet_size_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_packet_size_max);
}

uint32
dnx_data_nif_eth_an_max_nof_abilities_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_an_max_nof_abilities);
}

uint32
dnx_data_nif_eth_phy_map_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_phy_map_granularity);
}

uint32
dnx_data_nif_eth_vco_factor_for_53p125_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_vco_factor_for_53p125_rate);
}

uint32
dnx_data_nif_eth_vco_factor_for_51p5625_rate_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_vco_factor_for_51p5625_rate);
}

uint32
dnx_data_nif_eth_is_400G_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_is_400G_supported);
}

uint32
dnx_data_nif_eth_is_800G_supported_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_define_is_800G_supported);
}



const dnx_data_nif_eth_start_tx_threshold_table_t *
dnx_data_nif_eth_start_tx_threshold_table_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_start_tx_threshold_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_eth_start_tx_threshold_table_t *) data;

}

const dnx_data_nif_eth_pm_properties_t *
dnx_data_nif_eth_pm_properties_get(
    int unit,
    int pm_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_pm_properties);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_index, 0);
    return (const dnx_data_nif_eth_pm_properties_t *) data;

}

const dnx_data_nif_eth_ethu_properties_t *
dnx_data_nif_eth_ethu_properties_get(
    int unit,
    int ethu_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_ethu_properties);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ethu_index, 0);
    return (const dnx_data_nif_eth_ethu_properties_t *) data;

}

const dnx_data_nif_eth_phy_map_t *
dnx_data_nif_eth_phy_map_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_phy_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_eth_phy_map_t *) data;

}

const dnx_data_nif_eth_is_nif_loopback_supported_t *
dnx_data_nif_eth_is_nif_loopback_supported_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_is_nif_loopback_supported);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_nif_eth_is_nif_loopback_supported_t *) data;

}

const dnx_data_nif_eth_max_speed_t *
dnx_data_nif_eth_max_speed_get(
    int unit,
    int lane_num)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_max_speed);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, lane_num, 0);
    return (const dnx_data_nif_eth_max_speed_t *) data;

}

const dnx_data_nif_eth_supported_interfaces_t *
dnx_data_nif_eth_supported_interfaces_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_supported_interfaces);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_eth_supported_interfaces_t *) data;

}

const dnx_data_nif_eth_ethu_per_core_t *
dnx_data_nif_eth_ethu_per_core_get(
    int unit,
    int ethu_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_ethu_per_core);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ethu_index, 0);
    return (const dnx_data_nif_eth_ethu_per_core_t *) data;

}

const dnx_data_nif_eth_nif_cores_ethus_t *
dnx_data_nif_eth_nif_cores_ethus_get(
    int unit,
    int core_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_nif_cores_ethus);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, core_id, 0);
    return (const dnx_data_nif_eth_nif_cores_ethus_t *) data;

}

const dnx_data_nif_eth_supported_clu_an_abilities_t *
dnx_data_nif_eth_supported_clu_an_abilities_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_supported_clu_an_abilities);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_eth_supported_clu_an_abilities_t *) data;

}


shr_error_e
dnx_data_nif_eth_start_tx_threshold_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_start_tx_threshold_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_start_tx_threshold_table);
    data = (const dnx_data_nif_eth_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_thr);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_thr_for_l1);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_pm_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_pm_properties_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_pm_properties);
    data = (const dnx_data_nif_eth_pm_properties_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_PBMP_STR(buffer, data->phys);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tvco_pll_index);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->phy_addr);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_master);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dispatch_type);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->special_pll_check);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->flr_support);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ext_txpi_support);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->link_recovery_support);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_ethu_properties_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_ethu_properties_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_ethu_properties);
    data = (const dnx_data_nif_eth_ethu_properties_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, data->pms);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_phy_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_phy_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_phy_map);
    data = (const dnx_data_nif_eth_phy_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->phy_index);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ethu_index);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pm_index);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_is_nif_loopback_supported_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_is_nif_loopback_supported_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_is_nif_loopback_supported);
    data = (const dnx_data_nif_eth_is_nif_loopback_supported_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_supported);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_max_speed_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_max_speed_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_max_speed);
    data = (const dnx_data_nif_eth_max_speed_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_supported_interfaces_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_supported_interfaces);
    data = (const dnx_data_nif_eth_supported_interfaces_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dispatch_type);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ptp_support);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->autoneg_support);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_ethu_per_core_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_ethu_per_core_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_ethu_per_core);
    data = (const dnx_data_nif_eth_ethu_per_core_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->core_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_nif_cores_ethus_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_nif_cores_ethus_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_nif_cores_ethus);
    data = (const dnx_data_nif_eth_nif_cores_ethus_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_ethus);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->first_ethu);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_eth_supported_clu_an_abilities_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_eth_supported_clu_an_abilities_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_supported_clu_an_abilities);
    data = (const dnx_data_nif_eth_supported_clu_an_abilities_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->an_mode);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->medium);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->channel);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_eth_start_tx_threshold_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_start_tx_threshold_table);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_pm_properties_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_pm_properties);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_ethu_properties_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_ethu_properties);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_phy_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_phy_map);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_is_nif_loopback_supported_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_is_nif_loopback_supported);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_max_speed_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_max_speed);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_supported_interfaces_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_supported_interfaces);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_ethu_per_core_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_ethu_per_core);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_nif_cores_ethus_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_nif_cores_ethus);

}

const dnxc_data_table_info_t *
dnx_data_nif_eth_supported_clu_an_abilities_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_eth, dnx_data_nif_eth_table_supported_clu_an_abilities);

}






static shr_error_e
dnx_data_nif_simulator_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "simulator";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_simulator_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif simulator features");

    
    submodule_data->nof_defines = _dnx_data_nif_simulator_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif simulator defines");

    submodule_data->defines[dnx_data_nif_simulator_define_cdu_type].name = "cdu_type";
    submodule_data->defines[dnx_data_nif_simulator_define_cdu_type].doc = "";
    
    submodule_data->defines[dnx_data_nif_simulator_define_cdu_type].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_simulator_define_clu_type].name = "clu_type";
    submodule_data->defines[dnx_data_nif_simulator_define_clu_type].doc = "";
    
    submodule_data->defines[dnx_data_nif_simulator_define_clu_type].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_simulator_define_nbu_type].name = "nbu_type";
    submodule_data->defines[dnx_data_nif_simulator_define_nbu_type].doc = "";
    
    submodule_data->defines[dnx_data_nif_simulator_define_nbu_type].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_simulator_define_mgu_type].name = "mgu_type";
    submodule_data->defines[dnx_data_nif_simulator_define_mgu_type].doc = "";
    
    submodule_data->defines[dnx_data_nif_simulator_define_mgu_type].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_nif_simulator_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif simulator tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_simulator_feature_get(
    int unit,
    dnx_data_nif_simulator_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_simulator, feature);
}


uint32
dnx_data_nif_simulator_cdu_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_simulator, dnx_data_nif_simulator_define_cdu_type);
}

uint32
dnx_data_nif_simulator_clu_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_simulator, dnx_data_nif_simulator_define_clu_type);
}

uint32
dnx_data_nif_simulator_nbu_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_simulator, dnx_data_nif_simulator_define_nbu_type);
}

uint32
dnx_data_nif_simulator_mgu_type_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_simulator, dnx_data_nif_simulator_define_mgu_type);
}










static shr_error_e
dnx_data_nif_framer_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "framer";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_framer_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif framer features");

    submodule_data->features[dnx_data_nif_framer_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_framer_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_framer_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_framer_disable_l1].name = "disable_l1";
    submodule_data->features[dnx_data_nif_framer_disable_l1].doc = "";
    submodule_data->features[dnx_data_nif_framer_disable_l1].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_framer_is_clock_independent].name = "is_clock_independent";
    submodule_data->features[dnx_data_nif_framer_is_clock_independent].doc = "";
    submodule_data->features[dnx_data_nif_framer_is_clock_independent].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_framer_l1_eth_supported].name = "l1_eth_supported";
    submodule_data->features[dnx_data_nif_framer_l1_eth_supported].doc = "";
    submodule_data->features[dnx_data_nif_framer_l1_eth_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_framer_eth_use_flexe_group].name = "eth_use_flexe_group";
    submodule_data->features[dnx_data_nif_framer_eth_use_flexe_group].doc = "";
    submodule_data->features[dnx_data_nif_framer_eth_use_flexe_group].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_framer_embeded_mac_in_busa_busc].name = "embeded_mac_in_busa_busc";
    submodule_data->features[dnx_data_nif_framer_embeded_mac_in_busa_busc].doc = "";
    submodule_data->features[dnx_data_nif_framer_embeded_mac_in_busa_busc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_framer_power_down_supported].name = "power_down_supported";
    submodule_data->features[dnx_data_nif_framer_power_down_supported].doc = "";
    submodule_data->features[dnx_data_nif_framer_power_down_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_framer_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif framer defines");

    submodule_data->defines[dnx_data_nif_framer_define_nof_units].name = "nof_units";
    submodule_data->defines[dnx_data_nif_framer_define_nof_units].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_nof_units].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_b66switch_backup_channels].name = "b66switch_backup_channels";
    submodule_data->defines[dnx_data_nif_framer_define_b66switch_backup_channels].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_b66switch_backup_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_gen].name = "gen";
    submodule_data->defines[dnx_data_nif_framer_define_gen].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_gen].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_nof_mgmt_channels].name = "nof_mgmt_channels";
    submodule_data->defines[dnx_data_nif_framer_define_nof_mgmt_channels].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_nof_mgmt_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_clock_mhz].name = "clock_mhz";
    submodule_data->defines[dnx_data_nif_framer_define_clock_mhz].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_clock_mhz].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_ptp_channel_max_bandwidth].name = "ptp_channel_max_bandwidth";
    submodule_data->defines[dnx_data_nif_framer_define_ptp_channel_max_bandwidth].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_ptp_channel_max_bandwidth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_oam_channel_max_bandwidth].name = "oam_channel_max_bandwidth";
    submodule_data->defines[dnx_data_nif_framer_define_oam_channel_max_bandwidth].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_oam_channel_max_bandwidth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_nof_mac1_channels].name = "nof_mac1_channels";
    submodule_data->defines[dnx_data_nif_framer_define_nof_mac1_channels].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_nof_mac1_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_nof_mac2_channels].name = "nof_mac2_channels";
    submodule_data->defines[dnx_data_nif_framer_define_nof_mac2_channels].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_nof_mac2_channels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_framer_define_nof_pms_per_framer].name = "nof_pms_per_framer";
    submodule_data->defines[dnx_data_nif_framer_define_nof_pms_per_framer].doc = "";
    
    submodule_data->defines[dnx_data_nif_framer_define_nof_pms_per_framer].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_nif_framer_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif framer tables");

    
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].name = "b66switch";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].size_of_values = sizeof(dnx_data_nif_framer_b66switch_t);
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].entry_get = dnx_data_nif_framer_b66switch_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].keys[0].name = "port_type";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_framer_table_b66switch].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_framer_table_b66switch].nof_values, "_dnx_data_nif_framer_table_b66switch table values");
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[0].name = "block_id";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_b66switch_t, block_id);
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[1].name = "backup_block_id";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_b66switch].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_b66switch_t, backup_block_id);

    
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].name = "mac_info";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].size_of_values = sizeof(dnx_data_nif_framer_mac_info_t);
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].entry_get = dnx_data_nif_framer_mac_info_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].keys[0].name = "index";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_framer_table_mac_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_framer_table_mac_info].nof_values, "_dnx_data_nif_framer_table_mac_info table values");
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[0].name = "block_id";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_mac_info_t, block_id);
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[1].name = "backup_block_id";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mac_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_mac_info_t, backup_block_id);

    
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].name = "mgmt_channel_info";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].size_of_values = sizeof(dnx_data_nif_framer_mgmt_channel_info_t);
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].entry_get = dnx_data_nif_framer_mgmt_channel_info_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].keys[0].name = "channel";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].nof_values, "_dnx_data_nif_framer_table_mgmt_channel_info table values");
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[0].name = "valid";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_mgmt_channel_info_t, valid);
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[1].name = "is_ptp";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_mgmt_channel_info_t, is_ptp);
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[2].name = "block_id";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[2].type = "int";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_framer_table_mgmt_channel_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_framer_mgmt_channel_info_t, block_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_framer_feature_get(
    int unit,
    dnx_data_nif_framer_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, feature);
}


uint32
dnx_data_nif_framer_nof_units_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_nof_units);
}

uint32
dnx_data_nif_framer_b66switch_backup_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_b66switch_backup_channels);
}

uint32
dnx_data_nif_framer_gen_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_gen);
}

uint32
dnx_data_nif_framer_nof_mgmt_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_nof_mgmt_channels);
}

uint32
dnx_data_nif_framer_clock_mhz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_clock_mhz);
}

uint32
dnx_data_nif_framer_ptp_channel_max_bandwidth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_ptp_channel_max_bandwidth);
}

uint32
dnx_data_nif_framer_oam_channel_max_bandwidth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_oam_channel_max_bandwidth);
}

uint32
dnx_data_nif_framer_nof_mac1_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_nof_mac1_channels);
}

uint32
dnx_data_nif_framer_nof_mac2_channels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_nof_mac2_channels);
}

uint32
dnx_data_nif_framer_nof_pms_per_framer_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_define_nof_pms_per_framer);
}



const dnx_data_nif_framer_b66switch_t *
dnx_data_nif_framer_b66switch_get(
    int unit,
    int port_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_b66switch);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, port_type, 0);
    return (const dnx_data_nif_framer_b66switch_t *) data;

}

const dnx_data_nif_framer_mac_info_t *
dnx_data_nif_framer_mac_info_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_mac_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_nif_framer_mac_info_t *) data;

}

const dnx_data_nif_framer_mgmt_channel_info_t *
dnx_data_nif_framer_mgmt_channel_info_get(
    int unit,
    int channel)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_mgmt_channel_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, channel, 0);
    return (const dnx_data_nif_framer_mgmt_channel_info_t *) data;

}


shr_error_e
dnx_data_nif_framer_b66switch_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_framer_b66switch_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_b66switch);
    data = (const dnx_data_nif_framer_b66switch_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->backup_block_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_framer_mac_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_framer_mac_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_mac_info);
    data = (const dnx_data_nif_framer_mac_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->backup_block_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_framer_mgmt_channel_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_framer_mgmt_channel_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_mgmt_channel_info);
    data = (const dnx_data_nif_framer_mgmt_channel_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_ptp);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->block_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_framer_b66switch_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_b66switch);

}

const dnxc_data_table_info_t *
dnx_data_nif_framer_mac_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_mac_info);

}

const dnxc_data_table_info_t *
dnx_data_nif_framer_mgmt_channel_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_framer, dnx_data_nif_framer_table_mgmt_channel_info);

}






static shr_error_e
dnx_data_nif_mac_client_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "mac_client";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_mac_client_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif mac_client features");

    submodule_data->features[dnx_data_nif_mac_client_prd_is_bypassed].name = "prd_is_bypassed";
    submodule_data->features[dnx_data_nif_mac_client_prd_is_bypassed].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_prd_is_bypassed].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_rmc_cal_per_priority].name = "rmc_cal_per_priority";
    submodule_data->features[dnx_data_nif_mac_client_rmc_cal_per_priority].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_rmc_cal_per_priority].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_double_slots_for_all_client].name = "double_slots_for_all_client";
    submodule_data->features[dnx_data_nif_mac_client_double_slots_for_all_client].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_double_slots_for_all_client].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_l1_mismatch_rate_support].name = "l1_mismatch_rate_support";
    submodule_data->features[dnx_data_nif_mac_client_l1_mismatch_rate_support].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_l1_mismatch_rate_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_double_slots_for_small_client].name = "double_slots_for_small_client";
    submodule_data->features[dnx_data_nif_mac_client_double_slots_for_small_client].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_double_slots_for_small_client].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_avoid_bw_verification].name = "avoid_bw_verification";
    submodule_data->features[dnx_data_nif_mac_client_avoid_bw_verification].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_avoid_bw_verification].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_disable_l1_mismatch_rate_checking].name = "disable_l1_mismatch_rate_checking";
    submodule_data->features[dnx_data_nif_mac_client_disable_l1_mismatch_rate_checking].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_disable_l1_mismatch_rate_checking].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_is_l1_support].name = "is_l1_support";
    submodule_data->features[dnx_data_nif_mac_client_is_l1_support].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_is_l1_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_is_speed_supported].name = "is_speed_supported";
    submodule_data->features[dnx_data_nif_mac_client_is_speed_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_is_speed_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_is_tx_crcmode_supported].name = "is_tx_crcmode_supported";
    submodule_data->features[dnx_data_nif_mac_client_is_tx_crcmode_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_is_tx_crcmode_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_is_tx_pading_supported].name = "is_tx_pading_supported";
    submodule_data->features[dnx_data_nif_mac_client_is_tx_pading_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_is_tx_pading_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_is_pfc_pause_supported].name = "is_pfc_pause_supported";
    submodule_data->features[dnx_data_nif_mac_client_is_pfc_pause_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_is_pfc_pause_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_is_loopback_supported].name = "is_loopback_supported";
    submodule_data->features[dnx_data_nif_mac_client_is_loopback_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_is_loopback_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_sb_tx_fifo_supported].name = "sb_tx_fifo_supported";
    submodule_data->features[dnx_data_nif_mac_client_sb_tx_fifo_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_sb_tx_fifo_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_sb_fifo_size_configurable].name = "sb_fifo_size_configurable";
    submodule_data->features[dnx_data_nif_mac_client_sb_fifo_size_configurable].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_sb_fifo_size_configurable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported].name = "sb_tx_cal_switch_en_supported";
    submodule_data->features[dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported].name = "sb_tx_credit_cnt_en_supported";
    submodule_data->features[dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_mac_client_enc_dec_configurable].name = "enc_dec_configurable";
    submodule_data->features[dnx_data_nif_mac_client_enc_dec_configurable].doc = "";
    submodule_data->features[dnx_data_nif_mac_client_enc_dec_configurable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_mac_client_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif mac_client defines");

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_normal_clients].name = "nof_normal_clients";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_normal_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_normal_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_special_clients].name = "nof_special_clients";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_special_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_special_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_clients].name = "nof_clients";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_channel_base].name = "channel_base";
    submodule_data->defines[dnx_data_nif_mac_client_define_channel_base].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_channel_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_max_nof_slots].name = "max_nof_slots";
    submodule_data->defines[dnx_data_nif_mac_client_define_max_nof_slots].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_max_nof_slots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_logical_fifos].name = "nof_logical_fifos";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_logical_fifos].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_logical_fifos].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_oam_client_channel].name = "oam_client_channel";
    submodule_data->defines[dnx_data_nif_mac_client_define_oam_client_channel].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_oam_client_channel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_client_channel].name = "ptp_client_channel";
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_client_channel].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_client_channel].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_oam_tx_cal_slot].name = "oam_tx_cal_slot";
    submodule_data->defines[dnx_data_nif_mac_client_define_oam_tx_cal_slot].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_oam_tx_cal_slot].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_tx_cal_slot].name = "ptp_tx_cal_slot";
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_tx_cal_slot].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_tx_cal_slot].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_priority_groups_nof].name = "priority_groups_nof";
    submodule_data->defines[dnx_data_nif_mac_client_define_priority_groups_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_priority_groups_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_channel_max_bandwidth].name = "ptp_channel_max_bandwidth";
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_channel_max_bandwidth].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_channel_max_bandwidth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_oam_channel_max_bandwidth].name = "oam_channel_max_bandwidth";
    submodule_data->defines[dnx_data_nif_mac_client_define_oam_channel_max_bandwidth].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_oam_channel_max_bandwidth].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_mem_entries_per_slot].name = "nof_mem_entries_per_slot";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_mem_entries_per_slot].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_mem_entries_per_slot].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_oam_fifo_entries_in_sb_rx].name = "ptp_oam_fifo_entries_in_sb_rx";
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_oam_fifo_entries_in_sb_rx].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_ptp_oam_fifo_entries_in_sb_rx].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_average_ipg_for_l1].name = "average_ipg_for_l1";
    submodule_data->defines[dnx_data_nif_mac_client_define_average_ipg_for_l1].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_average_ipg_for_l1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_mac_tx_threshold].name = "mac_tx_threshold";
    submodule_data->defines[dnx_data_nif_mac_client_define_mac_tx_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_mac_tx_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_rmc_cal_nof_slots].name = "rmc_cal_nof_slots";
    submodule_data->defines[dnx_data_nif_mac_client_define_rmc_cal_nof_slots].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_rmc_cal_nof_slots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_mac_timeslots].name = "nof_mac_timeslots";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_mac_timeslots].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_mac_timeslots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_tx_average_ipg_max].name = "tx_average_ipg_max";
    submodule_data->defines[dnx_data_nif_mac_client_define_tx_average_ipg_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_tx_average_ipg_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_tx_threshold_max].name = "tx_threshold_max";
    submodule_data->defines[dnx_data_nif_mac_client_define_tx_threshold_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_tx_threshold_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max].name = "tx_fifo_cell_cnt_max";
    submodule_data->defines[dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_sb_fifo_granularity].name = "sb_fifo_granularity";
    submodule_data->defines[dnx_data_nif_mac_client_define_sb_fifo_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_sb_fifo_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_sb_cal_granularity].name = "sb_cal_granularity";
    submodule_data->defines[dnx_data_nif_mac_client_define_sb_cal_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_sb_cal_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_sb_fifos].name = "nof_sb_fifos";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_sb_fifos].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_sb_fifos].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_sb_cal_slots].name = "nof_sb_cal_slots";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_sb_cal_slots].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_sb_cal_slots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo].name = "nof_units_per_sb_tx_fifo";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo].name = "nof_units_per_sb_rx_fifo";
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_tinymac_clock_mhz].name = "tinymac_clock_mhz";
    submodule_data->defines[dnx_data_nif_mac_client_define_tinymac_clock_mhz].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_tinymac_clock_mhz].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g].name = "sb_tx_init_credit_for_5g";
    submodule_data->defines[dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_async_fifo_delay_cycles].name = "async_fifo_delay_cycles";
    submodule_data->defines[dnx_data_nif_mac_client_define_async_fifo_delay_cycles].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_async_fifo_delay_cycles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_pipe_delay_cycles].name = "pipe_delay_cycles";
    submodule_data->defines[dnx_data_nif_mac_client_define_pipe_delay_cycles].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_pipe_delay_cycles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_mac_client_define_mac_pad_size].name = "mac_pad_size";
    submodule_data->defines[dnx_data_nif_mac_client_define_mac_pad_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_mac_client_define_mac_pad_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_mac_client_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif mac_client tables");

    
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].name = "start_tx_threshold_table";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].size_of_values = sizeof(dnx_data_nif_mac_client_start_tx_threshold_table_t);
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].entry_get = dnx_data_nif_mac_client_start_tx_threshold_table_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].nof_values, "_dnx_data_nif_mac_client_table_start_tx_threshold_table table values");
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_start_tx_threshold_table_t, speed);
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[1].name = "start_thr";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_start_tx_threshold_table_t, start_thr);
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[2].name = "start_thr_for_l1";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_start_tx_threshold_table].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_start_tx_threshold_table_t, start_thr_for_l1);

    
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].name = "l1_mismatch_rate_table";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].size_of_values = sizeof(dnx_data_nif_mac_client_l1_mismatch_rate_table_t);
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].entry_get = dnx_data_nif_mac_client_l1_mismatch_rate_table_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].nof_values, "_dnx_data_nif_mac_client_table_l1_mismatch_rate_table table values");
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[0].name = "eth_speed";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_l1_mismatch_rate_table_t, eth_speed);
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[1].name = "client_speed";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_l1_mismatch_rate_table_t, client_speed);
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[2].name = "tx_thr";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_l1_mismatch_rate_table].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_l1_mismatch_rate_table_t, tx_thr);

    
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].name = "speed_tx_property_table";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].size_of_values = sizeof(dnx_data_nif_mac_client_speed_tx_property_table_t);
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].entry_get = dnx_data_nif_mac_client_speed_tx_property_table_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].nof_values, "_dnx_data_nif_mac_client_table_speed_tx_property_table table values");
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_speed_tx_property_table_t, speed);
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[1].name = "speed_group";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_speed_tx_property_table_t, speed_group);
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[2].name = "num_credit";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[2].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_speed_tx_property_table_t, num_credit);
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[3].name = "tx_threshold";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[3].type = "uint32";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_mac_client_table_speed_tx_property_table].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_mac_client_speed_tx_property_table_t, tx_threshold);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_mac_client_feature_get(
    int unit,
    dnx_data_nif_mac_client_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, feature);
}


uint32
dnx_data_nif_mac_client_nof_normal_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_normal_clients);
}

uint32
dnx_data_nif_mac_client_nof_special_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_special_clients);
}

uint32
dnx_data_nif_mac_client_nof_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_clients);
}

uint32
dnx_data_nif_mac_client_channel_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_channel_base);
}

uint32
dnx_data_nif_mac_client_max_nof_slots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_max_nof_slots);
}

uint32
dnx_data_nif_mac_client_nof_logical_fifos_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_logical_fifos);
}

uint32
dnx_data_nif_mac_client_oam_client_channel_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_oam_client_channel);
}

uint32
dnx_data_nif_mac_client_ptp_client_channel_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_ptp_client_channel);
}

uint32
dnx_data_nif_mac_client_oam_tx_cal_slot_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_oam_tx_cal_slot);
}

uint32
dnx_data_nif_mac_client_ptp_tx_cal_slot_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_ptp_tx_cal_slot);
}

uint32
dnx_data_nif_mac_client_priority_groups_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_priority_groups_nof);
}

uint32
dnx_data_nif_mac_client_ptp_channel_max_bandwidth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_ptp_channel_max_bandwidth);
}

uint32
dnx_data_nif_mac_client_oam_channel_max_bandwidth_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_oam_channel_max_bandwidth);
}

uint32
dnx_data_nif_mac_client_nof_mem_entries_per_slot_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_mem_entries_per_slot);
}

uint32
dnx_data_nif_mac_client_ptp_oam_fifo_entries_in_sb_rx_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_ptp_oam_fifo_entries_in_sb_rx);
}

uint32
dnx_data_nif_mac_client_average_ipg_for_l1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_average_ipg_for_l1);
}

uint32
dnx_data_nif_mac_client_mac_tx_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_mac_tx_threshold);
}

uint32
dnx_data_nif_mac_client_rmc_cal_nof_slots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_rmc_cal_nof_slots);
}

uint32
dnx_data_nif_mac_client_nof_mac_timeslots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_mac_timeslots);
}

uint32
dnx_data_nif_mac_client_tx_average_ipg_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_tx_average_ipg_max);
}

uint32
dnx_data_nif_mac_client_tx_threshold_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_tx_threshold_max);
}

uint32
dnx_data_nif_mac_client_tx_fifo_cell_cnt_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_tx_fifo_cell_cnt_max);
}

uint32
dnx_data_nif_mac_client_sb_fifo_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_sb_fifo_granularity);
}

uint32
dnx_data_nif_mac_client_sb_cal_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_sb_cal_granularity);
}

uint32
dnx_data_nif_mac_client_nof_sb_fifos_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_sb_fifos);
}

uint32
dnx_data_nif_mac_client_nof_sb_cal_slots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_sb_cal_slots);
}

uint32
dnx_data_nif_mac_client_nof_units_per_sb_tx_fifo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_units_per_sb_tx_fifo);
}

uint32
dnx_data_nif_mac_client_nof_units_per_sb_rx_fifo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_nof_units_per_sb_rx_fifo);
}

uint32
dnx_data_nif_mac_client_tinymac_clock_mhz_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_tinymac_clock_mhz);
}

uint32
dnx_data_nif_mac_client_sb_tx_init_credit_for_5g_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_sb_tx_init_credit_for_5g);
}

uint32
dnx_data_nif_mac_client_async_fifo_delay_cycles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_async_fifo_delay_cycles);
}

uint32
dnx_data_nif_mac_client_pipe_delay_cycles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_pipe_delay_cycles);
}

uint32
dnx_data_nif_mac_client_mac_pad_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_define_mac_pad_size);
}



const dnx_data_nif_mac_client_start_tx_threshold_table_t *
dnx_data_nif_mac_client_start_tx_threshold_table_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_start_tx_threshold_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_mac_client_start_tx_threshold_table_t *) data;

}

const dnx_data_nif_mac_client_l1_mismatch_rate_table_t *
dnx_data_nif_mac_client_l1_mismatch_rate_table_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_l1_mismatch_rate_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_mac_client_l1_mismatch_rate_table_t *) data;

}

const dnx_data_nif_mac_client_speed_tx_property_table_t *
dnx_data_nif_mac_client_speed_tx_property_table_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_speed_tx_property_table);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_mac_client_speed_tx_property_table_t *) data;

}


shr_error_e
dnx_data_nif_mac_client_start_tx_threshold_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_mac_client_start_tx_threshold_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_start_tx_threshold_table);
    data = (const dnx_data_nif_mac_client_start_tx_threshold_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_thr);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_thr_for_l1);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_mac_client_l1_mismatch_rate_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_mac_client_l1_mismatch_rate_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_l1_mismatch_rate_table);
    data = (const dnx_data_nif_mac_client_l1_mismatch_rate_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->eth_speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->client_speed);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_thr);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_mac_client_speed_tx_property_table_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_mac_client_speed_tx_property_table_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_speed_tx_property_table);
    data = (const dnx_data_nif_mac_client_speed_tx_property_table_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed_group);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->num_credit);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->tx_threshold);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_mac_client_start_tx_threshold_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_start_tx_threshold_table);

}

const dnxc_data_table_info_t *
dnx_data_nif_mac_client_l1_mismatch_rate_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_l1_mismatch_rate_table);

}

const dnxc_data_table_info_t *
dnx_data_nif_mac_client_speed_tx_property_table_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_mac_client, dnx_data_nif_mac_client_table_speed_tx_property_table);

}






static shr_error_e
dnx_data_nif_sar_client_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "sar_client";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_sar_client_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif sar_client features");

    submodule_data->features[dnx_data_nif_sar_client_feu_ilkn_burst_size_support].name = "feu_ilkn_burst_size_support";
    submodule_data->features[dnx_data_nif_sar_client_feu_ilkn_burst_size_support].doc = "";
    submodule_data->features[dnx_data_nif_sar_client_feu_ilkn_burst_size_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_sar_client_connect_to_66bswitch].name = "connect_to_66bswitch";
    submodule_data->features[dnx_data_nif_sar_client_connect_to_66bswitch].doc = "";
    submodule_data->features[dnx_data_nif_sar_client_connect_to_66bswitch].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_sar_client_is_tdm_port].name = "is_tdm_port";
    submodule_data->features[dnx_data_nif_sar_client_is_tdm_port].doc = "";
    submodule_data->features[dnx_data_nif_sar_client_is_tdm_port].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_sar_client_per_channel_cell_mode].name = "per_channel_cell_mode";
    submodule_data->features[dnx_data_nif_sar_client_per_channel_cell_mode].doc = "";
    submodule_data->features[dnx_data_nif_sar_client_per_channel_cell_mode].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_sar_client_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif sar_client defines");

    submodule_data->defines[dnx_data_nif_sar_client_define_nof_clients].name = "nof_clients";
    submodule_data->defines[dnx_data_nif_sar_client_define_nof_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_sar_client_define_nof_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_sar_client_define_ilkn_burst_size_in_feu].name = "ilkn_burst_size_in_feu";
    submodule_data->defines[dnx_data_nif_sar_client_define_ilkn_burst_size_in_feu].doc = "";
    
    submodule_data->defines[dnx_data_nif_sar_client_define_ilkn_burst_size_in_feu].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_sar_client_define_nof_sar_timeslots].name = "nof_sar_timeslots";
    submodule_data->defines[dnx_data_nif_sar_client_define_nof_sar_timeslots].doc = "";
    
    submodule_data->defines[dnx_data_nif_sar_client_define_nof_sar_timeslots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_sar_client_define_channel_base].name = "channel_base";
    submodule_data->defines[dnx_data_nif_sar_client_define_channel_base].doc = "";
    
    submodule_data->defines[dnx_data_nif_sar_client_define_channel_base].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_sar_client_define_sar_jitter_tolerance_max_level].name = "sar_jitter_tolerance_max_level";
    submodule_data->defines[dnx_data_nif_sar_client_define_sar_jitter_tolerance_max_level].doc = "";
    
    submodule_data->defines[dnx_data_nif_sar_client_define_sar_jitter_tolerance_max_level].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_sar_client_define_cal_granularity].name = "cal_granularity";
    submodule_data->defines[dnx_data_nif_sar_client_define_cal_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_sar_client_define_cal_granularity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_sar_client_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif sar_client tables");

    
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].name = "sar_cell_mode_to_size";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].doc = "";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].size_of_values = sizeof(dnx_data_nif_sar_client_sar_cell_mode_to_size_t);
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].entry_get = dnx_data_nif_sar_client_sar_cell_mode_to_size_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].keys[0].name = "cell_mode";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].nof_values, "_dnx_data_nif_sar_client_table_sar_cell_mode_to_size table values");
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].values[0].name = "packet_size";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_cell_mode_to_size].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_sar_client_sar_cell_mode_to_size_t, packet_size);

    
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].name = "sar_jitter_fifo_levels";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].doc = "";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].size_of_values = sizeof(dnx_data_nif_sar_client_sar_jitter_fifo_levels_t);
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].entry_get = dnx_data_nif_sar_client_sar_jitter_fifo_levels_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].keys[0].name = "tolerance_level";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].nof_values, "_dnx_data_nif_sar_client_table_sar_jitter_fifo_levels table values");
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[0].name = "low_level";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_sar_client_sar_jitter_fifo_levels_t, low_level);
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[1].name = "high_level";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_sar_client_table_sar_jitter_fifo_levels].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_sar_client_sar_jitter_fifo_levels_t, high_level);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_sar_client_feature_get(
    int unit,
    dnx_data_nif_sar_client_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, feature);
}


uint32
dnx_data_nif_sar_client_nof_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_define_nof_clients);
}

uint32
dnx_data_nif_sar_client_ilkn_burst_size_in_feu_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_define_ilkn_burst_size_in_feu);
}

uint32
dnx_data_nif_sar_client_nof_sar_timeslots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_define_nof_sar_timeslots);
}

uint32
dnx_data_nif_sar_client_channel_base_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_define_channel_base);
}

uint32
dnx_data_nif_sar_client_sar_jitter_tolerance_max_level_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_define_sar_jitter_tolerance_max_level);
}

uint32
dnx_data_nif_sar_client_cal_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_define_cal_granularity);
}



const dnx_data_nif_sar_client_sar_cell_mode_to_size_t *
dnx_data_nif_sar_client_sar_cell_mode_to_size_get(
    int unit,
    int cell_mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_table_sar_cell_mode_to_size);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, cell_mode, 0);
    return (const dnx_data_nif_sar_client_sar_cell_mode_to_size_t *) data;

}

const dnx_data_nif_sar_client_sar_jitter_fifo_levels_t *
dnx_data_nif_sar_client_sar_jitter_fifo_levels_get(
    int unit,
    int tolerance_level)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_table_sar_jitter_fifo_levels);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, tolerance_level, 0);
    return (const dnx_data_nif_sar_client_sar_jitter_fifo_levels_t *) data;

}


shr_error_e
dnx_data_nif_sar_client_sar_cell_mode_to_size_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_sar_client_sar_cell_mode_to_size_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_table_sar_cell_mode_to_size);
    data = (const dnx_data_nif_sar_client_sar_cell_mode_to_size_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->packet_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_sar_client_sar_jitter_fifo_levels_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_sar_client_sar_jitter_fifo_levels_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_table_sar_jitter_fifo_levels);
    data = (const dnx_data_nif_sar_client_sar_jitter_fifo_levels_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->low_level);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->high_level);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_sar_client_sar_cell_mode_to_size_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_table_sar_cell_mode_to_size);

}

const dnxc_data_table_info_t *
dnx_data_nif_sar_client_sar_jitter_fifo_levels_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_sar_client, dnx_data_nif_sar_client_table_sar_jitter_fifo_levels);

}






static shr_error_e
dnx_data_nif_flexe_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "flexe";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_flexe_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif flexe features");

    submodule_data->features[dnx_data_nif_flexe_is_led_supported].name = "is_led_supported";
    submodule_data->features[dnx_data_nif_flexe_is_led_supported].doc = "";
    submodule_data->features[dnx_data_nif_flexe_is_led_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_half_capability].name = "half_capability";
    submodule_data->features[dnx_data_nif_flexe_half_capability].doc = "";
    submodule_data->features[dnx_data_nif_flexe_half_capability].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_pcs_port_enable_support].name = "pcs_port_enable_support";
    submodule_data->features[dnx_data_nif_flexe_pcs_port_enable_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_pcs_port_enable_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_rx_valid_in_lf].name = "rx_valid_in_lf";
    submodule_data->features[dnx_data_nif_flexe_rx_valid_in_lf].doc = "";
    submodule_data->features[dnx_data_nif_flexe_rx_valid_in_lf].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_disable_oam_over_flexe].name = "disable_oam_over_flexe";
    submodule_data->features[dnx_data_nif_flexe_disable_oam_over_flexe].doc = "";
    submodule_data->features[dnx_data_nif_flexe_disable_oam_over_flexe].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_nb_rx_port_reset_support].name = "nb_rx_port_reset_support";
    submodule_data->features[dnx_data_nif_flexe_nb_rx_port_reset_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_nb_rx_port_reset_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_toggle_tx_reset_in_nb].name = "toggle_tx_reset_in_nb";
    submodule_data->features[dnx_data_nif_flexe_toggle_tx_reset_in_nb].doc = "";
    submodule_data->features[dnx_data_nif_flexe_toggle_tx_reset_in_nb].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_pcs_to_flexe_port_map_support].name = "pcs_to_flexe_port_map_support";
    submodule_data->features[dnx_data_nif_flexe_pcs_to_flexe_port_map_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_pcs_to_flexe_port_map_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_nb_cal_slot_enable_support].name = "nb_cal_slot_enable_support";
    submodule_data->features[dnx_data_nif_flexe_nb_cal_slot_enable_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_nb_cal_slot_enable_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_nb_cal_switch_enable_support].name = "nb_cal_switch_enable_support";
    submodule_data->features[dnx_data_nif_flexe_nb_cal_switch_enable_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_nb_cal_switch_enable_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_nb_dynamic_async_fifo].name = "nb_dynamic_async_fifo";
    submodule_data->features[dnx_data_nif_flexe_nb_dynamic_async_fifo].doc = "";
    submodule_data->features[dnx_data_nif_flexe_nb_dynamic_async_fifo].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_multi_device_mode_support].name = "multi_device_mode_support";
    submodule_data->features[dnx_data_nif_flexe_multi_device_mode_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_multi_device_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_core_port_need_alloc].name = "core_port_need_alloc";
    submodule_data->features[dnx_data_nif_flexe_core_port_need_alloc].doc = "";
    submodule_data->features[dnx_data_nif_flexe_core_port_need_alloc].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_multi_flexe_slot_mode_support].name = "multi_flexe_slot_mode_support";
    submodule_data->features[dnx_data_nif_flexe_multi_flexe_slot_mode_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_multi_flexe_slot_mode_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_unaware_support].name = "unaware_support";
    submodule_data->features[dnx_data_nif_flexe_unaware_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_unaware_support].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_flexe_nb_tx_cal_support].name = "nb_tx_cal_support";
    submodule_data->features[dnx_data_nif_flexe_nb_tx_cal_support].doc = "";
    submodule_data->features[dnx_data_nif_flexe_nb_tx_cal_support].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_flexe_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif flexe defines");

    submodule_data->defines[dnx_data_nif_flexe_define_feu_nof].name = "feu_nof";
    submodule_data->defines[dnx_data_nif_flexe_define_feu_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_feu_nof].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_clients].name = "nof_clients";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_groups].name = "nof_groups";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_groups].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_group_id].name = "max_group_id";
    submodule_data->defines[dnx_data_nif_flexe_define_max_group_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_group_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_min_group_id].name = "min_group_id";
    submodule_data->defines[dnx_data_nif_flexe_define_min_group_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_min_group_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nb_tx_delay].name = "nb_tx_delay";
    submodule_data->defines[dnx_data_nif_flexe_define_nb_tx_delay].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nb_tx_delay].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read].name = "nb_tx_fifo_th_for_first_read";
    submodule_data->defines[dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_pcs].name = "nof_pcs";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_pcs].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_pcs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_phy_speeds].name = "nof_phy_speeds";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_phy_speeds].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_phy_speeds].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_phy_speed_granularity].name = "phy_speed_granularity";
    submodule_data->defines[dnx_data_nif_flexe_define_phy_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_phy_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_flexe_core_speed].name = "max_flexe_core_speed";
    submodule_data->defines[dnx_data_nif_flexe_define_max_flexe_core_speed].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_flexe_core_speed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_flexe_core_clock_source_pm].name = "flexe_core_clock_source_pm";
    submodule_data->defines[dnx_data_nif_flexe_define_flexe_core_clock_source_pm].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_flexe_core_clock_source_pm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_nof_slots].name = "max_nof_slots";
    submodule_data->defines[dnx_data_nif_flexe_define_max_nof_slots].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_nof_slots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_min_client_id].name = "min_client_id";
    submodule_data->defines[dnx_data_nif_flexe_define_min_client_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_min_client_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_client_id].name = "max_client_id";
    submodule_data->defines[dnx_data_nif_flexe_define_max_client_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_client_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_instances].name = "nof_flexe_instances";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_instances].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_instances].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_lphys].name = "nof_flexe_lphys";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_lphys].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_lphys].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_distributed_ilu_id].name = "distributed_ilu_id";
    submodule_data->defines[dnx_data_nif_flexe_define_distributed_ilu_id].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_distributed_ilu_id].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_nif_rate_centralized].name = "max_nif_rate_centralized";
    submodule_data->defines[dnx_data_nif_flexe_define_max_nif_rate_centralized].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_nif_rate_centralized].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_nif_rate_distributed].name = "max_nif_rate_distributed";
    submodule_data->defines[dnx_data_nif_flexe_define_max_nif_rate_distributed].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_nif_rate_distributed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_max_ilu_rate_distributed].name = "max_ilu_rate_distributed";
    submodule_data->defines[dnx_data_nif_flexe_define_max_ilu_rate_distributed].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_max_ilu_rate_distributed].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_core_ports].name = "nof_flexe_core_ports";
    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_core_ports].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nof_flexe_core_ports].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_nof_slots].name = "nb_cal_nof_slots";
    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_nof_slots].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_nof_slots].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_granularity].name = "nb_cal_granularity";
    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_non_5g_slot_bw].name = "non_5g_slot_bw";
    submodule_data->defines[dnx_data_nif_flexe_define_non_5g_slot_bw].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_non_5g_slot_bw].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_slot_nof_bits].name = "nb_cal_slot_nof_bits";
    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_slot_nof_bits].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nb_cal_slot_nof_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flexe_define_flexe_mode].name = "flexe_mode";
    submodule_data->defines[dnx_data_nif_flexe_define_flexe_mode].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_flexe_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode].name = "nb_tdm_slot_allocation_mode";
    submodule_data->defines[dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_flexe_define_device_slot_mode].name = "device_slot_mode";
    submodule_data->defines[dnx_data_nif_flexe_define_device_slot_mode].doc = "";
    
    submodule_data->defines[dnx_data_nif_flexe_define_device_slot_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_nif_flexe_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif flexe tables");

    
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].name = "phy_info";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].size_of_values = sizeof(dnx_data_nif_flexe_phy_info_t);
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].entry_get = dnx_data_nif_flexe_phy_info_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].keys[0].name = "idx";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_flexe_table_phy_info].nof_values, "_dnx_data_nif_flexe_table_phy_info table values");
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_phy_info_t, speed);
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[1].name = "min_logical_phy_id";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_phy_info_t, min_logical_phy_id);
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[2].name = "max_logical_phy_id";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[2].type = "int";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_phy_info_t, max_logical_phy_id);
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[3].name = "flexe_core_port";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[3].type = "int[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CORE_PORTS]";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_phy_info_t, flexe_core_port);
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[4].name = "nof_instances";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[4].type = "int";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[4].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[4].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_phy_info_t, nof_instances);
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[5].name = "instance_id";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[5].type = "int[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES]";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[5].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_phy_info].values[5].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_phy_info_t, instance_id);

    
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].name = "supported_pms";
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].size_of_values = sizeof(dnx_data_nif_flexe_supported_pms_t);
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].entry_get = dnx_data_nif_flexe_supported_pms_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].keys[0].name = "framer_idx";
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].nof_values, "_dnx_data_nif_flexe_table_supported_pms table values");
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].values[0].name = "nif_pms";
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].values[0].type = "int[DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE]";
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_supported_pms].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_supported_pms_t, nif_pms);

    
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].name = "client_speed_granularity";
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].size_of_values = sizeof(dnx_data_nif_flexe_client_speed_granularity_t);
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].entry_get = dnx_data_nif_flexe_client_speed_granularity_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].keys[0].name = "slot_mode";
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].nof_values, "_dnx_data_nif_flexe_table_client_speed_granularity table values");
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].values[0].name = "val";
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_flexe_table_client_speed_granularity].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_flexe_client_speed_granularity_t, val);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_flexe_feature_get(
    int unit,
    dnx_data_nif_flexe_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, feature);
}


uint32
dnx_data_nif_flexe_feu_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_feu_nof);
}

uint32
dnx_data_nif_flexe_nof_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_clients);
}

uint32
dnx_data_nif_flexe_nof_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_groups);
}

uint32
dnx_data_nif_flexe_max_group_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_group_id);
}

uint32
dnx_data_nif_flexe_min_group_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_min_group_id);
}

uint32
dnx_data_nif_flexe_nb_tx_delay_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nb_tx_delay);
}

uint32
dnx_data_nif_flexe_nb_tx_fifo_th_for_first_read_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nb_tx_fifo_th_for_first_read);
}

uint32
dnx_data_nif_flexe_nof_pcs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_pcs);
}

uint32
dnx_data_nif_flexe_nof_phy_speeds_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_phy_speeds);
}

uint32
dnx_data_nif_flexe_phy_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_phy_speed_granularity);
}

uint32
dnx_data_nif_flexe_max_flexe_core_speed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_flexe_core_speed);
}

uint32
dnx_data_nif_flexe_flexe_core_clock_source_pm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_flexe_core_clock_source_pm);
}

uint32
dnx_data_nif_flexe_max_nof_slots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_nof_slots);
}

uint32
dnx_data_nif_flexe_min_client_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_min_client_id);
}

uint32
dnx_data_nif_flexe_max_client_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_client_id);
}

uint32
dnx_data_nif_flexe_nof_flexe_instances_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_flexe_instances);
}

uint32
dnx_data_nif_flexe_nof_flexe_lphys_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_flexe_lphys);
}

uint32
dnx_data_nif_flexe_distributed_ilu_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_distributed_ilu_id);
}

uint32
dnx_data_nif_flexe_max_nif_rate_centralized_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_nif_rate_centralized);
}

uint32
dnx_data_nif_flexe_max_nif_rate_distributed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_nif_rate_distributed);
}

uint32
dnx_data_nif_flexe_max_ilu_rate_distributed_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_max_ilu_rate_distributed);
}

uint32
dnx_data_nif_flexe_nof_flexe_core_ports_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nof_flexe_core_ports);
}

uint32
dnx_data_nif_flexe_nb_cal_nof_slots_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nb_cal_nof_slots);
}

uint32
dnx_data_nif_flexe_nb_cal_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nb_cal_granularity);
}

uint32
dnx_data_nif_flexe_non_5g_slot_bw_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_non_5g_slot_bw);
}

uint32
dnx_data_nif_flexe_nb_cal_slot_nof_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nb_cal_slot_nof_bits);
}

uint32
dnx_data_nif_flexe_flexe_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_flexe_mode);
}

uint32
dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode);
}

uint32
dnx_data_nif_flexe_device_slot_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_define_device_slot_mode);
}



const dnx_data_nif_flexe_phy_info_t *
dnx_data_nif_flexe_phy_info_get(
    int unit,
    int idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_phy_info);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, idx, 0);
    return (const dnx_data_nif_flexe_phy_info_t *) data;

}

const dnx_data_nif_flexe_supported_pms_t *
dnx_data_nif_flexe_supported_pms_get(
    int unit,
    int framer_idx)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_supported_pms);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, framer_idx, 0);
    return (const dnx_data_nif_flexe_supported_pms_t *) data;

}

const dnx_data_nif_flexe_client_speed_granularity_t *
dnx_data_nif_flexe_client_speed_granularity_get(
    int unit,
    int slot_mode)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_client_speed_granularity);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, slot_mode, 0);
    return (const dnx_data_nif_flexe_client_speed_granularity_t *) data;

}


shr_error_e
dnx_data_nif_flexe_phy_info_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_flexe_phy_info_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_phy_info);
    data = (const dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->min_logical_phy_id);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->max_logical_phy_id);
            break;
        case 3:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CORE_PORTS, data->flexe_core_port);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_instances);
            break;
        case 5:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_INSTANCES, data->instance_id);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_flexe_supported_pms_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_flexe_supported_pms_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_supported_pms);
    data = (const dnx_data_nif_flexe_supported_pms_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_NIF_ETH_TOTAL_NOF_ETHU_PMS_IN_DEVICE, data->nif_pms);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_flexe_client_speed_granularity_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_flexe_client_speed_granularity_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_client_speed_granularity);
    data = (const dnx_data_nif_flexe_client_speed_granularity_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->val);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_flexe_phy_info_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_phy_info);

}

const dnxc_data_table_info_t *
dnx_data_nif_flexe_supported_pms_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_supported_pms);

}

const dnxc_data_table_info_t *
dnx_data_nif_flexe_client_speed_granularity_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flexe, dnx_data_nif_flexe_table_client_speed_granularity);

}






static shr_error_e
dnx_data_nif_wbc_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "wbc";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_wbc_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif wbc features");

    submodule_data->features[dnx_data_nif_wbc_is_used].name = "is_used";
    submodule_data->features[dnx_data_nif_wbc_is_used].doc = "";
    submodule_data->features[dnx_data_nif_wbc_is_used].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_wbc_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif wbc defines");

    
    submodule_data->nof_tables = _dnx_data_nif_wbc_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif wbc tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_wbc_feature_get(
    int unit,
    dnx_data_nif_wbc_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_wbc, feature);
}











static shr_error_e
dnx_data_nif_prd_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "prd";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_prd_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif prd features");

    submodule_data->features[dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration].name = "is_rx_rmc_threshold_need_configuration";
    submodule_data->features[dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration].doc = "";
    submodule_data->features[dnx_data_nif_prd_is_rx_rmc_threshold_need_configuration].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_prd_is_mask_parser_tdm_supported].name = "is_mask_parser_tdm_supported";
    submodule_data->features[dnx_data_nif_prd_is_mask_parser_tdm_supported].doc = "";
    submodule_data->features[dnx_data_nif_prd_is_mask_parser_tdm_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_prd_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif prd defines");

    submodule_data->defines[dnx_data_nif_prd_define_nof_control_frames].name = "nof_control_frames";
    submodule_data->defines[dnx_data_nif_prd_define_nof_control_frames].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_control_frames].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_ether_type_codes].name = "nof_ether_type_codes";
    submodule_data->defines[dnx_data_nif_prd_define_nof_ether_type_codes].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_ether_type_codes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_tcam_entries].name = "nof_tcam_entries";
    submodule_data->defines[dnx_data_nif_prd_define_nof_tcam_entries].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_tcam_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_tcam_key_offset_size].name = "tcam_key_offset_size";
    submodule_data->defines[dnx_data_nif_prd_define_tcam_key_offset_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_tcam_key_offset_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_mpls_special_labels].name = "nof_mpls_special_labels";
    submodule_data->defines[dnx_data_nif_prd_define_nof_mpls_special_labels].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_mpls_special_labels].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_priorities].name = "nof_priorities";
    submodule_data->defines[dnx_data_nif_prd_define_nof_priorities].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_priorities].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_custom_ether_type_code_min].name = "custom_ether_type_code_min";
    submodule_data->defines[dnx_data_nif_prd_define_custom_ether_type_code_min].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_custom_ether_type_code_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_custom_ether_type_code_max].name = "custom_ether_type_code_max";
    submodule_data->defines[dnx_data_nif_prd_define_custom_ether_type_code_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_custom_ether_type_code_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_ether_type_code_max].name = "ether_type_code_max";
    submodule_data->defines[dnx_data_nif_prd_define_ether_type_code_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_ether_type_code_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_ether_type_max].name = "ether_type_max";
    submodule_data->defines[dnx_data_nif_prd_define_ether_type_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_ether_type_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_rmc_threshold_max].name = "rmc_threshold_max";
    submodule_data->defines[dnx_data_nif_prd_define_rmc_threshold_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_rmc_threshold_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_cdu_rmc_threshold_max].name = "cdu_rmc_threshold_max";
    submodule_data->defines[dnx_data_nif_prd_define_cdu_rmc_threshold_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_cdu_rmc_threshold_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_hrf_threshold_max].name = "hrf_threshold_max";
    submodule_data->defines[dnx_data_nif_prd_define_hrf_threshold_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_hrf_threshold_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_mpls_special_label_max].name = "mpls_special_label_max";
    submodule_data->defines[dnx_data_nif_prd_define_mpls_special_label_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_mpls_special_label_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_flex_key_offset_key_max].name = "flex_key_offset_key_max";
    submodule_data->defines[dnx_data_nif_prd_define_flex_key_offset_key_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_flex_key_offset_key_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_flex_key_offset_result_max].name = "flex_key_offset_result_max";
    submodule_data->defines[dnx_data_nif_prd_define_flex_key_offset_result_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_flex_key_offset_result_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_tpid_max].name = "tpid_max";
    submodule_data->defines[dnx_data_nif_prd_define_tpid_max].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_tpid_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_clu_port_profiles].name = "nof_clu_port_profiles";
    submodule_data->defines[dnx_data_nif_prd_define_nof_clu_port_profiles].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_clu_port_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_framer_port_profiles].name = "nof_framer_port_profiles";
    submodule_data->defines[dnx_data_nif_prd_define_nof_framer_port_profiles].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_framer_port_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_ofr_port_profiles].name = "nof_ofr_port_profiles";
    submodule_data->defines[dnx_data_nif_prd_define_nof_ofr_port_profiles].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_ofr_port_profiles].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_rmc_fifo_2_threshold_resolution].name = "rmc_fifo_2_threshold_resolution";
    submodule_data->defines[dnx_data_nif_prd_define_rmc_fifo_2_threshold_resolution].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_rmc_fifo_2_threshold_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_ofr_nif_interfaces].name = "nof_ofr_nif_interfaces";
    submodule_data->defines[dnx_data_nif_prd_define_nof_ofr_nif_interfaces].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_ofr_nif_interfaces].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_prd_define_nof_prd_counters].name = "nof_prd_counters";
    submodule_data->defines[dnx_data_nif_prd_define_nof_prd_counters].doc = "";
    
    submodule_data->defines[dnx_data_nif_prd_define_nof_prd_counters].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_prd_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif prd tables");

    
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].name = "ether_type";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].doc = "";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].size_of_values = sizeof(dnx_data_nif_prd_ether_type_t);
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].entry_get = dnx_data_nif_prd_ether_type_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].keys[0].name = "ether_type_code";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_prd_table_ether_type].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_prd_table_ether_type].nof_values, "_dnx_data_nif_prd_table_ether_type table values");
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[0].name = "ether_type_name";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[0].type = "char *";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_prd_ether_type_t, ether_type_name);
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[1].name = "ether_type_size";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_prd_table_ether_type].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_prd_ether_type_t, ether_type_size);

    
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].name = "port_profile_map";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].doc = "";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].size_of_values = sizeof(dnx_data_nif_prd_port_profile_map_t);
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].entry_get = dnx_data_nif_prd_port_profile_map_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].keys[0].name = "type";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].nof_values, "_dnx_data_nif_prd_table_port_profile_map table values");
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[0].name = "is_supported";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_prd_port_profile_map_t, is_supported);
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[1].name = "nof_profiles";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[1].type = "uint32";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_prd_table_port_profile_map].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_prd_port_profile_map_t, nof_profiles);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_prd_feature_get(
    int unit,
    dnx_data_nif_prd_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, feature);
}


uint32
dnx_data_nif_prd_nof_control_frames_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_control_frames);
}

uint32
dnx_data_nif_prd_nof_ether_type_codes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_ether_type_codes);
}

uint32
dnx_data_nif_prd_nof_tcam_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_tcam_entries);
}

uint32
dnx_data_nif_prd_tcam_key_offset_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_tcam_key_offset_size);
}

uint32
dnx_data_nif_prd_nof_mpls_special_labels_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_mpls_special_labels);
}

uint32
dnx_data_nif_prd_nof_priorities_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_priorities);
}

uint32
dnx_data_nif_prd_custom_ether_type_code_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_custom_ether_type_code_min);
}

uint32
dnx_data_nif_prd_custom_ether_type_code_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_custom_ether_type_code_max);
}

uint32
dnx_data_nif_prd_ether_type_code_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_ether_type_code_max);
}

uint32
dnx_data_nif_prd_ether_type_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_ether_type_max);
}

uint32
dnx_data_nif_prd_rmc_threshold_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_rmc_threshold_max);
}

uint32
dnx_data_nif_prd_cdu_rmc_threshold_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_cdu_rmc_threshold_max);
}

uint32
dnx_data_nif_prd_hrf_threshold_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_hrf_threshold_max);
}

uint32
dnx_data_nif_prd_mpls_special_label_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_mpls_special_label_max);
}

uint32
dnx_data_nif_prd_flex_key_offset_key_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_flex_key_offset_key_max);
}

uint32
dnx_data_nif_prd_flex_key_offset_result_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_flex_key_offset_result_max);
}

uint32
dnx_data_nif_prd_tpid_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_tpid_max);
}

uint32
dnx_data_nif_prd_nof_clu_port_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_clu_port_profiles);
}

uint32
dnx_data_nif_prd_nof_framer_port_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_framer_port_profiles);
}

uint32
dnx_data_nif_prd_nof_ofr_port_profiles_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_ofr_port_profiles);
}

uint32
dnx_data_nif_prd_rmc_fifo_2_threshold_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_rmc_fifo_2_threshold_resolution);
}

uint32
dnx_data_nif_prd_nof_ofr_nif_interfaces_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_ofr_nif_interfaces);
}

uint32
dnx_data_nif_prd_nof_prd_counters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_define_nof_prd_counters);
}



const dnx_data_nif_prd_ether_type_t *
dnx_data_nif_prd_ether_type_get(
    int unit,
    int ether_type_code)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_table_ether_type);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ether_type_code, 0);
    return (const dnx_data_nif_prd_ether_type_t *) data;

}

const dnx_data_nif_prd_port_profile_map_t *
dnx_data_nif_prd_port_profile_map_get(
    int unit,
    int type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_table_port_profile_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, type, 0);
    return (const dnx_data_nif_prd_port_profile_map_t *) data;

}


shr_error_e
dnx_data_nif_prd_ether_type_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_prd_ether_type_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_table_ether_type);
    data = (const dnx_data_nif_prd_ether_type_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->ether_type_name == NULL ? "" : data->ether_type_name);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ether_type_size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_prd_port_profile_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_prd_port_profile_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_table_port_profile_map);
    data = (const dnx_data_nif_prd_port_profile_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_supported);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_profiles);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_prd_ether_type_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_table_ether_type);

}

const dnxc_data_table_info_t *
dnx_data_nif_prd_port_profile_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_prd, dnx_data_nif_prd_table_port_profile_map);

}






static shr_error_e
dnx_data_nif_portmod_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "portmod";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_portmod_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif portmod features");

    submodule_data->features[dnx_data_nif_portmod_not_implemented].name = "not_implemented";
    submodule_data->features[dnx_data_nif_portmod_not_implemented].doc = "";
    submodule_data->features[dnx_data_nif_portmod_not_implemented].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_portmod_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif portmod defines");

    submodule_data->defines[dnx_data_nif_portmod_define_pm_types_nof].name = "pm_types_nof";
    submodule_data->defines[dnx_data_nif_portmod_define_pm_types_nof].doc = "";
    
    submodule_data->defines[dnx_data_nif_portmod_define_pm_types_nof].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_portmod_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif portmod tables");

    
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].name = "pm_types_and_interfaces";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].doc = "";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].size_of_values = sizeof(dnx_data_nif_portmod_pm_types_and_interfaces_t);
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].entry_get = dnx_data_nif_portmod_pm_types_and_interfaces_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].keys[0].name = "index";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].nof_values, "_dnx_data_nif_portmod_table_pm_types_and_interfaces table values");
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[0].name = "type";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[0].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_portmod_pm_types_and_interfaces_t, type);
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[1].name = "instances";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[1].type = "int";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_portmod_table_pm_types_and_interfaces].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_portmod_pm_types_and_interfaces_t, instances);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_portmod_feature_get(
    int unit,
    dnx_data_nif_portmod_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_portmod, feature);
}


uint32
dnx_data_nif_portmod_pm_types_nof_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_portmod, dnx_data_nif_portmod_define_pm_types_nof);
}



const dnx_data_nif_portmod_pm_types_and_interfaces_t *
dnx_data_nif_portmod_pm_types_and_interfaces_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_portmod, dnx_data_nif_portmod_table_pm_types_and_interfaces);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_nif_portmod_pm_types_and_interfaces_t *) data;

}


shr_error_e
dnx_data_nif_portmod_pm_types_and_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_portmod_pm_types_and_interfaces_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_portmod, dnx_data_nif_portmod_table_pm_types_and_interfaces);
    data = (const dnx_data_nif_portmod_pm_types_and_interfaces_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->type);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->instances);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_portmod_pm_types_and_interfaces_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_portmod, dnx_data_nif_portmod_table_pm_types_and_interfaces);

}






static shr_error_e
dnx_data_nif_scheduler_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "scheduler";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_scheduler_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif scheduler features");

    submodule_data->features[dnx_data_nif_scheduler_decrement_credit_on_read_enable].name = "decrement_credit_on_read_enable";
    submodule_data->features[dnx_data_nif_scheduler_decrement_credit_on_read_enable].doc = "";
    submodule_data->features[dnx_data_nif_scheduler_decrement_credit_on_read_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_scheduler_dynamic_scheduling].name = "dynamic_scheduling";
    submodule_data->features[dnx_data_nif_scheduler_dynamic_scheduling].doc = "";
    submodule_data->features[dnx_data_nif_scheduler_dynamic_scheduling].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_scheduler_hardcoded_lanes_and_priority].name = "hardcoded_lanes_and_priority";
    submodule_data->features[dnx_data_nif_scheduler_hardcoded_lanes_and_priority].doc = "";
    submodule_data->features[dnx_data_nif_scheduler_hardcoded_lanes_and_priority].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_scheduler_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif scheduler defines");

    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_ethu_bit].name = "rate_per_ethu_bit";
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_ethu_bit].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_ethu_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_ilu_bit].name = "rate_per_ilu_bit";
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_ilu_bit].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_ilu_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit].name = "rate_per_cdu_rmc_bit";
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit].name = "rate_per_clu_rmc_bit";
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_nof_weight_bits].name = "nof_weight_bits";
    submodule_data->defines[dnx_data_nif_scheduler_define_nof_weight_bits].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_nof_weight_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_nof_rmc_bits].name = "nof_rmc_bits";
    submodule_data->defines[dnx_data_nif_scheduler_define_nof_rmc_bits].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_nof_rmc_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_dynamic_scheduling_rate_resolution].name = "dynamic_scheduling_rate_resolution";
    submodule_data->defines[dnx_data_nif_scheduler_define_dynamic_scheduling_rate_resolution].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_dynamic_scheduling_rate_resolution].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_rmc_map_table_nof_entries].name = "rmc_map_table_nof_entries";
    submodule_data->defines[dnx_data_nif_scheduler_define_rmc_map_table_nof_entries].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_rmc_map_table_nof_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_scheduler_define_rmc_map_table_entry_resolution].name = "rmc_map_table_entry_resolution";
    submodule_data->defines[dnx_data_nif_scheduler_define_rmc_map_table_entry_resolution].doc = "";
    
    submodule_data->defines[dnx_data_nif_scheduler_define_rmc_map_table_entry_resolution].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_scheduler_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif scheduler tables");

    
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].name = "rx_scheduler_rmc_entries_map";
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].doc = "";
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].size_of_values = sizeof(dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t);
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].entry_get = dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].keys[0].name = "rmc_id";
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].nof_values, "_dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map table values");
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].values[0].name = "start_slot";
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].values[0].type = "int";
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t, start_slot);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_scheduler_feature_get(
    int unit,
    dnx_data_nif_scheduler_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, feature);
}


uint32
dnx_data_nif_scheduler_rate_per_ethu_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_rate_per_ethu_bit);
}

uint32
dnx_data_nif_scheduler_rate_per_ilu_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_rate_per_ilu_bit);
}

uint32
dnx_data_nif_scheduler_rate_per_cdu_rmc_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_rate_per_cdu_rmc_bit);
}

uint32
dnx_data_nif_scheduler_rate_per_clu_rmc_bit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_rate_per_clu_rmc_bit);
}

uint32
dnx_data_nif_scheduler_nof_weight_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_nof_weight_bits);
}

uint32
dnx_data_nif_scheduler_nof_rmc_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_nof_rmc_bits);
}

uint32
dnx_data_nif_scheduler_dynamic_scheduling_rate_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_dynamic_scheduling_rate_resolution);
}

uint32
dnx_data_nif_scheduler_rmc_map_table_nof_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_rmc_map_table_nof_entries);
}

uint32
dnx_data_nif_scheduler_rmc_map_table_entry_resolution_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_define_rmc_map_table_entry_resolution);
}



const dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t *
dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_get(
    int unit,
    int rmc_id)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, rmc_id, 0);
    return (const dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t *) data;

}


shr_error_e
dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map);
    data = (const dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->start_slot);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_scheduler_rx_scheduler_rmc_entries_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_scheduler, dnx_data_nif_scheduler_table_rx_scheduler_rmc_entries_map);

}






static shr_error_e
dnx_data_nif_dbal_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "dbal";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_dbal_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif dbal features");

    submodule_data->features[dnx_data_nif_dbal_active_mac_configure].name = "active_mac_configure";
    submodule_data->features[dnx_data_nif_dbal_active_mac_configure].doc = "";
    submodule_data->features[dnx_data_nif_dbal_active_mac_configure].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_dbal_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif dbal defines");

    submodule_data->defines[dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size].name = "cdu_rx_rmc_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_clu_rx_rmc_entry_size].name = "clu_rx_rmc_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_clu_rx_rmc_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_clu_rx_rmc_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size].name = "cdu_tx_start_threshold_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size].name = "clu_tx_start_threshold_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_nbu_tx_start_threshold_entry_size].name = "nbu_tx_start_threshold_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_nbu_tx_start_threshold_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_nbu_tx_start_threshold_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_mgu_tx_start_threshold_entry_size].name = "mgu_tx_start_threshold_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_mgu_tx_start_threshold_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_mgu_tx_start_threshold_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_ilu_burst_min_entry_size].name = "ilu_burst_min_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_ilu_burst_min_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_ilu_burst_min_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size].name = "ethu_rx_rmc_counter_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size].name = "cdu_lane_fifo_level_and_occupancy_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size].name = "clu_lane_fifo_level_and_occupancy_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_nbu_lane_fifo_level_and_occupancy_entry_size].name = "nbu_lane_fifo_level_and_occupancy_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_nbu_lane_fifo_level_and_occupancy_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_nbu_lane_fifo_level_and_occupancy_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_mgu_lane_fifo_level_and_occupancy_entry_size].name = "mgu_lane_fifo_level_and_occupancy_entry_size";
    submodule_data->defines[dnx_data_nif_dbal_define_mgu_lane_fifo_level_and_occupancy_entry_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_mgu_lane_fifo_level_and_occupancy_entry_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size].name = "sch_cnt_dec_threshold_bit_size";
    submodule_data->defines[dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_tx_credits_bits].name = "tx_credits_bits";
    submodule_data->defines[dnx_data_nif_dbal_define_tx_credits_bits].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_tx_credits_bits].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_nof_rx_fifo_counters].name = "nof_rx_fifo_counters";
    submodule_data->defines[dnx_data_nif_dbal_define_nof_rx_fifo_counters].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_nof_rx_fifo_counters].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_dbal_define_nof_tx_fifo_counters].name = "nof_tx_fifo_counters";
    submodule_data->defines[dnx_data_nif_dbal_define_nof_tx_fifo_counters].doc = "";
    
    submodule_data->defines[dnx_data_nif_dbal_define_nof_tx_fifo_counters].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_dbal_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif dbal tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_dbal_feature_get(
    int unit,
    dnx_data_nif_dbal_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, feature);
}


uint32
dnx_data_nif_dbal_cdu_rx_rmc_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_cdu_rx_rmc_entry_size);
}

uint32
dnx_data_nif_dbal_clu_rx_rmc_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_clu_rx_rmc_entry_size);
}

uint32
dnx_data_nif_dbal_cdu_tx_start_threshold_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_cdu_tx_start_threshold_entry_size);
}

uint32
dnx_data_nif_dbal_clu_tx_start_threshold_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_clu_tx_start_threshold_entry_size);
}

uint32
dnx_data_nif_dbal_nbu_tx_start_threshold_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_nbu_tx_start_threshold_entry_size);
}

uint32
dnx_data_nif_dbal_mgu_tx_start_threshold_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_mgu_tx_start_threshold_entry_size);
}

uint32
dnx_data_nif_dbal_ilu_burst_min_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_ilu_burst_min_entry_size);
}

uint32
dnx_data_nif_dbal_ethu_rx_rmc_counter_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_ethu_rx_rmc_counter_entry_size);
}

uint32
dnx_data_nif_dbal_cdu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_cdu_lane_fifo_level_and_occupancy_entry_size);
}

uint32
dnx_data_nif_dbal_clu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_clu_lane_fifo_level_and_occupancy_entry_size);
}

uint32
dnx_data_nif_dbal_nbu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_nbu_lane_fifo_level_and_occupancy_entry_size);
}

uint32
dnx_data_nif_dbal_mgu_lane_fifo_level_and_occupancy_entry_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_mgu_lane_fifo_level_and_occupancy_entry_size);
}

uint32
dnx_data_nif_dbal_sch_cnt_dec_threshold_bit_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_sch_cnt_dec_threshold_bit_size);
}

uint32
dnx_data_nif_dbal_tx_credits_bits_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_tx_credits_bits);
}

uint32
dnx_data_nif_dbal_nof_rx_fifo_counters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_nof_rx_fifo_counters);
}

uint32
dnx_data_nif_dbal_nof_tx_fifo_counters_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_dbal, dnx_data_nif_dbal_define_nof_tx_fifo_counters);
}










static shr_error_e
dnx_data_nif_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "features";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif features features");

    submodule_data->features[dnx_data_nif_features_pm_mode_0122_invalid].name = "pm_mode_0122_invalid";
    submodule_data->features[dnx_data_nif_features_pm_mode_0122_invalid].doc = "";
    submodule_data->features[dnx_data_nif_features_pm_mode_0122_invalid].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif features defines");

    
    submodule_data->nof_tables = _dnx_data_nif_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_features_feature_get(
    int unit,
    dnx_data_nif_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_features, feature);
}











static shr_error_e
dnx_data_nif_arb_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "arb";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_arb_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif arb features");

    submodule_data->features[dnx_data_nif_arb_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_arb_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_arb_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_arb_is_used].name = "is_used";
    submodule_data->features[dnx_data_nif_arb_is_used].doc = "";
    submodule_data->features[dnx_data_nif_arb_is_used].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_arb_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif arb defines");

    submodule_data->defines[dnx_data_nif_arb_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_nif_arb_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_qpms].name = "nof_rx_qpms";
    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_qpms].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_qpms].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_ppms_in_qpm].name = "nof_rx_ppms_in_qpm";
    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_ppms_in_qpm].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_ppms_in_qpm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_lanes_in_qpm].name = "nof_lanes_in_qpm";
    submodule_data->defines[dnx_data_nif_arb_define_nof_lanes_in_qpm].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_lanes_in_qpm].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_map_destinations].name = "nof_map_destinations";
    submodule_data->defines[dnx_data_nif_arb_define_nof_map_destinations].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_map_destinations].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_max_calendar_length].name = "max_calendar_length";
    submodule_data->defines[dnx_data_nif_arb_define_max_calendar_length].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_max_calendar_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_sources].name = "nof_rx_sources";
    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_sources].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_rx_sources].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_nof_modes].name = "rx_sch_calendar_nof_modes";
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_nof_modes].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_nof_modes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_calendar_length].name = "rx_qpm_calendar_length";
    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_calendar_length].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_calendar_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_length].name = "rx_sch_calendar_length";
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_length].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_cdpm_calendar_length].name = "cdpm_calendar_length";
    submodule_data->defines[dnx_data_nif_arb_define_cdpm_calendar_length].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_cdpm_calendar_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_tmac_calendar_length].name = "tmac_calendar_length";
    submodule_data->defines[dnx_data_nif_arb_define_tmac_calendar_length].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_tmac_calendar_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients].name = "rx_qpm_calendar_nof_clients";
    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_nof_clients].name = "rx_sch_calendar_nof_clients";
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_nof_clients].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_calendar_nof_clients].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_port_speed_granularity].name = "rx_qpm_port_speed_granularity";
    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_port_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_qpm_port_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_port_speed_granularity].name = "rx_sch_port_speed_granularity";
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_port_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_sch_port_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_tx_eth_port_speed_granularity].name = "tx_eth_port_speed_granularity";
    submodule_data->defines[dnx_data_nif_arb_define_tx_eth_port_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_tx_eth_port_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_port_speed_granularity].name = "tx_tmac_port_speed_granularity";
    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_port_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_port_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_nof_sections].name = "tx_tmac_nof_sections";
    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_nof_sections].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_nof_sections].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity].name = "tx_tmac_link_list_speed_granularity";
    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar].name = "nof_entries_in_memory_row_for_rx_calendar";
    submodule_data->defines[dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar].name = "nof_bit_per_entry_in_rx_calendar";
    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_entries_in_tmac_calendar].name = "nof_entries_in_tmac_calendar";
    submodule_data->defines[dnx_data_nif_arb_define_nof_entries_in_tmac_calendar].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_entries_in_tmac_calendar].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar].name = "nof_bit_per_entry_in_tmac_calendar";
    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar].name = "nof_bit_per_entry_in_ppm_calendar";
    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_min_port_speed_for_link_list_section].name = "min_port_speed_for_link_list_section";
    submodule_data->defines[dnx_data_nif_arb_define_min_port_speed_for_link_list_section].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_min_port_speed_for_link_list_section].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_link_list_sections_denominator].name = "link_list_sections_denominator";
    submodule_data->defines[dnx_data_nif_arb_define_link_list_sections_denominator].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_link_list_sections_denominator].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_high_start_tx_speed_threshold].name = "high_start_tx_speed_threshold";
    submodule_data->defines[dnx_data_nif_arb_define_high_start_tx_speed_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_high_start_tx_speed_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_low_start_tx_speed_threshold].name = "low_start_tx_speed_threshold";
    submodule_data->defines[dnx_data_nif_arb_define_low_start_tx_speed_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_low_start_tx_speed_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_high_start_tx_speed_value].name = "high_start_tx_speed_value";
    submodule_data->defines[dnx_data_nif_arb_define_high_start_tx_speed_value].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_high_start_tx_speed_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_middle_start_tx_speed_value].name = "middle_start_tx_speed_value";
    submodule_data->defines[dnx_data_nif_arb_define_middle_start_tx_speed_value].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_middle_start_tx_speed_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_low_start_tx_speed_value].name = "low_start_tx_speed_value";
    submodule_data->defines[dnx_data_nif_arb_define_low_start_tx_speed_value].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_low_start_tx_speed_value].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_max_value_for_tx_start_threshold].name = "max_value_for_tx_start_threshold";
    submodule_data->defines[dnx_data_nif_arb_define_max_value_for_tx_start_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_max_value_for_tx_start_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_arb_define_rx_pm_default_min_pkt_size].name = "rx_pm_default_min_pkt_size";
    submodule_data->defines[dnx_data_nif_arb_define_rx_pm_default_min_pkt_size].doc = "";
    
    submodule_data->defines[dnx_data_nif_arb_define_rx_pm_default_min_pkt_size].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_arb_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif arb tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_arb_feature_get(
    int unit,
    dnx_data_nif_arb_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, feature);
}


uint32
dnx_data_nif_arb_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_contexts);
}

uint32
dnx_data_nif_arb_nof_rx_qpms_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_rx_qpms);
}

uint32
dnx_data_nif_arb_nof_rx_ppms_in_qpm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_rx_ppms_in_qpm);
}

uint32
dnx_data_nif_arb_nof_lanes_in_qpm_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_lanes_in_qpm);
}

uint32
dnx_data_nif_arb_nof_map_destinations_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_map_destinations);
}

uint32
dnx_data_nif_arb_max_calendar_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_max_calendar_length);
}

uint32
dnx_data_nif_arb_nof_rx_sources_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_rx_sources);
}

uint32
dnx_data_nif_arb_rx_sch_calendar_nof_modes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_sch_calendar_nof_modes);
}

uint32
dnx_data_nif_arb_rx_qpm_calendar_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_qpm_calendar_length);
}

uint32
dnx_data_nif_arb_rx_sch_calendar_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_sch_calendar_length);
}

uint32
dnx_data_nif_arb_cdpm_calendar_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_cdpm_calendar_length);
}

uint32
dnx_data_nif_arb_tmac_calendar_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_tmac_calendar_length);
}

uint32
dnx_data_nif_arb_rx_qpm_calendar_nof_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_qpm_calendar_nof_clients);
}

uint32
dnx_data_nif_arb_rx_sch_calendar_nof_clients_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_sch_calendar_nof_clients);
}

uint32
dnx_data_nif_arb_rx_qpm_port_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_qpm_port_speed_granularity);
}

uint32
dnx_data_nif_arb_rx_sch_port_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_sch_port_speed_granularity);
}

uint32
dnx_data_nif_arb_tx_eth_port_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_tx_eth_port_speed_granularity);
}

uint32
dnx_data_nif_arb_tx_tmac_port_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_tx_tmac_port_speed_granularity);
}

uint32
dnx_data_nif_arb_tx_tmac_nof_sections_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_tx_tmac_nof_sections);
}

uint32
dnx_data_nif_arb_tx_tmac_link_list_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_tx_tmac_link_list_speed_granularity);
}

uint32
dnx_data_nif_arb_nof_entries_in_memory_row_for_rx_calendar_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_entries_in_memory_row_for_rx_calendar);
}

uint32
dnx_data_nif_arb_nof_bit_per_entry_in_rx_calendar_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_bit_per_entry_in_rx_calendar);
}

uint32
dnx_data_nif_arb_nof_entries_in_tmac_calendar_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_entries_in_tmac_calendar);
}

uint32
dnx_data_nif_arb_nof_bit_per_entry_in_tmac_calendar_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_bit_per_entry_in_tmac_calendar);
}

uint32
dnx_data_nif_arb_nof_bit_per_entry_in_ppm_calendar_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_nof_bit_per_entry_in_ppm_calendar);
}

uint32
dnx_data_nif_arb_min_port_speed_for_link_list_section_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_min_port_speed_for_link_list_section);
}

uint32
dnx_data_nif_arb_link_list_sections_denominator_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_link_list_sections_denominator);
}

uint32
dnx_data_nif_arb_high_start_tx_speed_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_high_start_tx_speed_threshold);
}

uint32
dnx_data_nif_arb_low_start_tx_speed_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_low_start_tx_speed_threshold);
}

uint32
dnx_data_nif_arb_high_start_tx_speed_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_high_start_tx_speed_value);
}

uint32
dnx_data_nif_arb_middle_start_tx_speed_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_middle_start_tx_speed_value);
}

uint32
dnx_data_nif_arb_low_start_tx_speed_value_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_low_start_tx_speed_value);
}

uint32
dnx_data_nif_arb_max_value_for_tx_start_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_max_value_for_tx_start_threshold);
}

uint32
dnx_data_nif_arb_rx_pm_default_min_pkt_size_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_arb, dnx_data_nif_arb_define_rx_pm_default_min_pkt_size);
}










static shr_error_e
dnx_data_nif_ofr_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "ofr";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_ofr_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif ofr features");

    submodule_data->features[dnx_data_nif_ofr_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_ofr_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_ofr_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_ofr_is_used].name = "is_used";
    submodule_data->features[dnx_data_nif_ofr_is_used].doc = "";
    submodule_data->features[dnx_data_nif_ofr_is_used].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_ofr_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif ofr defines");

    submodule_data->defines[dnx_data_nif_ofr_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_rmc_per_priority_group].name = "nof_rmc_per_priority_group";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rmc_per_priority_group].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rmc_per_priority_group].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_total_rmcs].name = "nof_total_rmcs";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_total_rmcs].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_total_rmcs].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group].name = "nof_rx_mem_sections_per_group";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_groups].name = "nof_rx_memory_groups";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_groups].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_sections].name = "nof_rx_memory_sections";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_sections].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_sections].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section].name = "nof_rx_memory_entries_per_section";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity].name = "rx_memory_link_list_speed_granularity";
    submodule_data->defines[dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_rx_sch_granularity].name = "rx_sch_granularity";
    submodule_data->defines[dnx_data_nif_ofr_define_rx_sch_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_rx_sch_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_ofr_define_nof_bytes_per_memory_section].name = "nof_bytes_per_memory_section";
    submodule_data->defines[dnx_data_nif_ofr_define_nof_bytes_per_memory_section].doc = "";
    
    submodule_data->defines[dnx_data_nif_ofr_define_nof_bytes_per_memory_section].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_ofr_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif ofr tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_ofr_feature_get(
    int unit,
    dnx_data_nif_ofr_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, feature);
}


uint32
dnx_data_nif_ofr_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_contexts);
}

uint32
dnx_data_nif_ofr_nof_rmc_per_priority_group_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_rmc_per_priority_group);
}

uint32
dnx_data_nif_ofr_nof_total_rmcs_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_total_rmcs);
}

uint32
dnx_data_nif_ofr_nof_rx_mem_sections_per_group_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_rx_mem_sections_per_group);
}

uint32
dnx_data_nif_ofr_nof_rx_memory_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_rx_memory_groups);
}

uint32
dnx_data_nif_ofr_nof_rx_memory_sections_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_rx_memory_sections);
}

uint32
dnx_data_nif_ofr_nof_rx_memory_entries_per_section_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_rx_memory_entries_per_section);
}

uint32
dnx_data_nif_ofr_rx_memory_link_list_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_rx_memory_link_list_speed_granularity);
}

uint32
dnx_data_nif_ofr_rx_sch_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_rx_sch_granularity);
}

uint32
dnx_data_nif_ofr_nof_bytes_per_memory_section_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_ofr, dnx_data_nif_ofr_define_nof_bytes_per_memory_section);
}










static shr_error_e
dnx_data_nif_oft_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "oft";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_oft_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif oft features");

    submodule_data->features[dnx_data_nif_oft_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_oft_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_oft_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_oft_is_used].name = "is_used";
    submodule_data->features[dnx_data_nif_oft_is_used].doc = "";
    submodule_data->features[dnx_data_nif_oft_is_used].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_oft_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif oft defines");

    submodule_data->defines[dnx_data_nif_oft_define_calendar_length].name = "calendar_length";
    submodule_data->defines[dnx_data_nif_oft_define_calendar_length].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_calendar_length].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_oft_define_nof_sections].name = "nof_sections";
    submodule_data->defines[dnx_data_nif_oft_define_nof_sections].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_nof_sections].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_oft_define_nof_internal_calendar_entries].name = "nof_internal_calendar_entries";
    submodule_data->defines[dnx_data_nif_oft_define_nof_internal_calendar_entries].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_nof_internal_calendar_entries].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar].name = "nof_bit_per_internal_entry_in_calendar";
    submodule_data->defines[dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_oft_define_nof_contexts].name = "nof_contexts";
    submodule_data->defines[dnx_data_nif_oft_define_nof_contexts].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_nof_contexts].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_oft_define_port_speed_granularity].name = "port_speed_granularity";
    submodule_data->defines[dnx_data_nif_oft_define_port_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_port_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_oft_define_calendar_speed_granularity].name = "calendar_speed_granularity";
    submodule_data->defines[dnx_data_nif_oft_define_calendar_speed_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_oft_define_calendar_speed_granularity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_oft_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif oft tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_oft_feature_get(
    int unit,
    dnx_data_nif_oft_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, feature);
}


uint32
dnx_data_nif_oft_calendar_length_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_calendar_length);
}

uint32
dnx_data_nif_oft_nof_sections_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_nof_sections);
}

uint32
dnx_data_nif_oft_nof_internal_calendar_entries_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_nof_internal_calendar_entries);
}

uint32
dnx_data_nif_oft_nof_bit_per_internal_entry_in_calendar_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_nof_bit_per_internal_entry_in_calendar);
}

uint32
dnx_data_nif_oft_nof_contexts_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_nof_contexts);
}

uint32
dnx_data_nif_oft_port_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_port_speed_granularity);
}

uint32
dnx_data_nif_oft_calendar_speed_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_oft, dnx_data_nif_oft_define_calendar_speed_granularity);
}










static shr_error_e
dnx_data_nif_signal_quality_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "signal_quality";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_signal_quality_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif signal_quality features");

    submodule_data->features[dnx_data_nif_signal_quality_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_signal_quality_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_signal_quality_is_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_signal_quality_is_sw_supported].name = "is_sw_supported";
    submodule_data->features[dnx_data_nif_signal_quality_is_sw_supported].doc = "";
    submodule_data->features[dnx_data_nif_signal_quality_is_sw_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_signal_quality_is_ser_supported].name = "is_ser_supported";
    submodule_data->features[dnx_data_nif_signal_quality_is_ser_supported].doc = "";
    submodule_data->features[dnx_data_nif_signal_quality_is_ser_supported].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_nif_signal_quality_is_ber_supported].name = "is_ber_supported";
    submodule_data->features[dnx_data_nif_signal_quality_is_ber_supported].doc = "";
    submodule_data->features[dnx_data_nif_signal_quality_is_ber_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_signal_quality_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif signal_quality defines");

    submodule_data->defines[dnx_data_nif_signal_quality_define_is_debug_enabled].name = "is_debug_enabled";
    submodule_data->defines[dnx_data_nif_signal_quality_define_is_debug_enabled].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_is_debug_enabled].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_nif_signal_quality_define_max_degrade_threshold].name = "max_degrade_threshold";
    submodule_data->defines[dnx_data_nif_signal_quality_define_max_degrade_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_max_degrade_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_signal_quality_define_min_degrade_threshold].name = "min_degrade_threshold";
    submodule_data->defines[dnx_data_nif_signal_quality_define_min_degrade_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_min_degrade_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_signal_quality_define_max_fail_threshold].name = "max_fail_threshold";
    submodule_data->defines[dnx_data_nif_signal_quality_define_max_fail_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_max_fail_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_signal_quality_define_min_fail_threshold].name = "min_fail_threshold";
    submodule_data->defines[dnx_data_nif_signal_quality_define_min_fail_threshold].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_min_fail_threshold].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_signal_quality_define_max_interval].name = "max_interval";
    submodule_data->defines[dnx_data_nif_signal_quality_define_max_interval].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_max_interval].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_signal_quality_define_min_interval].name = "min_interval";
    submodule_data->defines[dnx_data_nif_signal_quality_define_min_interval].doc = "";
    
    submodule_data->defines[dnx_data_nif_signal_quality_define_min_interval].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_signal_quality_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif signal_quality tables");

    
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].name = "supported_fecs";
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].doc = "";
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].size_of_values = sizeof(dnx_data_nif_signal_quality_supported_fecs_t);
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].entry_get = dnx_data_nif_signal_quality_supported_fecs_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].keys[0].name = "fec_type";
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].nof_values, "_dnx_data_nif_signal_quality_table_supported_fecs table values");
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].values[0].name = "is_valid";
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_signal_quality_table_supported_fecs].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_signal_quality_supported_fecs_t, is_valid);

    
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].name = "hw_based_solution";
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].doc = "";
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].size_of_values = sizeof(dnx_data_nif_signal_quality_hw_based_solution_t);
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].entry_get = dnx_data_nif_signal_quality_hw_based_solution_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].keys[0].name = "pm_type";
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].nof_values, "_dnx_data_nif_signal_quality_table_hw_based_solution table values");
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].values[0].name = "is_valid";
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_signal_quality_table_hw_based_solution].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_signal_quality_hw_based_solution_t, is_valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_signal_quality_feature_get(
    int unit,
    dnx_data_nif_signal_quality_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, feature);
}


uint32
dnx_data_nif_signal_quality_is_debug_enabled_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_is_debug_enabled);
}

uint32
dnx_data_nif_signal_quality_max_degrade_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_max_degrade_threshold);
}

uint32
dnx_data_nif_signal_quality_min_degrade_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_min_degrade_threshold);
}

uint32
dnx_data_nif_signal_quality_max_fail_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_max_fail_threshold);
}

uint32
dnx_data_nif_signal_quality_min_fail_threshold_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_min_fail_threshold);
}

uint32
dnx_data_nif_signal_quality_max_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_max_interval);
}

uint32
dnx_data_nif_signal_quality_min_interval_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_define_min_interval);
}



const dnx_data_nif_signal_quality_supported_fecs_t *
dnx_data_nif_signal_quality_supported_fecs_get(
    int unit,
    int fec_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_table_supported_fecs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, fec_type, 0);
    return (const dnx_data_nif_signal_quality_supported_fecs_t *) data;

}

const dnx_data_nif_signal_quality_hw_based_solution_t *
dnx_data_nif_signal_quality_hw_based_solution_get(
    int unit,
    int pm_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_table_hw_based_solution);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pm_type, 0);
    return (const dnx_data_nif_signal_quality_hw_based_solution_t *) data;

}


shr_error_e
dnx_data_nif_signal_quality_supported_fecs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_signal_quality_supported_fecs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_table_supported_fecs);
    data = (const dnx_data_nif_signal_quality_supported_fecs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_nif_signal_quality_hw_based_solution_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_signal_quality_hw_based_solution_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_table_hw_based_solution);
    data = (const dnx_data_nif_signal_quality_hw_based_solution_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_signal_quality_supported_fecs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_table_supported_fecs);

}

const dnxc_data_table_info_t *
dnx_data_nif_signal_quality_hw_based_solution_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_signal_quality, dnx_data_nif_signal_quality_table_hw_based_solution);

}






static shr_error_e
dnx_data_nif_otn_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "otn";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_otn_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif otn features");

    submodule_data->features[dnx_data_nif_otn_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_nif_otn_is_supported].doc = "";
    submodule_data->features[dnx_data_nif_otn_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_otn_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif otn defines");

    submodule_data->defines[dnx_data_nif_otn_define_nof_units].name = "nof_units";
    submodule_data->defines[dnx_data_nif_otn_define_nof_units].doc = "";
    
    submodule_data->defines[dnx_data_nif_otn_define_nof_units].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_otn_define_nof_lanes].name = "nof_lanes";
    submodule_data->defines[dnx_data_nif_otn_define_nof_lanes].doc = "";
    
    submodule_data->defines[dnx_data_nif_otn_define_nof_lanes].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_otn_define_nof_groups].name = "nof_groups";
    submodule_data->defines[dnx_data_nif_otn_define_nof_groups].doc = "";
    
    submodule_data->defines[dnx_data_nif_otn_define_nof_groups].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_otn_define_nof_lanes_per_group].name = "nof_lanes_per_group";
    submodule_data->defines[dnx_data_nif_otn_define_nof_lanes_per_group].doc = "";
    
    submodule_data->defines[dnx_data_nif_otn_define_nof_lanes_per_group].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_otn_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif otn tables");

    
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].name = "supported_interfaces";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].size_of_values = sizeof(dnx_data_nif_otn_supported_interfaces_t);
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].entry_get = dnx_data_nif_otn_supported_interfaces_entry_str_get;

    
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].nof_keys = 1;
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].keys[0].name = "index";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].nof_values = 6;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values, dnxc_data_value_t, submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].nof_values, "_dnx_data_nif_otn_table_supported_interfaces table values");
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[0].name = "speed";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[0].type = "uint32";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[0].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[0].offset = UTILEX_OFFSETOF(dnx_data_nif_otn_supported_interfaces_t, speed);
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[1].name = "pm_dispatch_type";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[1].type = "portmod_dispatch_type_t";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[1].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[1].offset = UTILEX_OFFSETOF(dnx_data_nif_otn_supported_interfaces_t, pm_dispatch_type);
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[2].name = "fec_type";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[2].type = "bcm_port_phy_fec_t";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[2].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[2].offset = UTILEX_OFFSETOF(dnx_data_nif_otn_supported_interfaces_t, fec_type);
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[3].name = "nof_lanes";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[3].type = "int";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[3].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[3].offset = UTILEX_OFFSETOF(dnx_data_nif_otn_supported_interfaces_t, nof_lanes);
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[4].name = "otn_intf_name";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[4].type = "char *";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[4].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[4].offset = UTILEX_OFFSETOF(dnx_data_nif_otn_supported_interfaces_t, otn_intf_name);
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[5].name = "is_valid";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[5].type = "uint32";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[5].doc = "";
    submodule_data->tables[dnx_data_nif_otn_table_supported_interfaces].values[5].offset = UTILEX_OFFSETOF(dnx_data_nif_otn_supported_interfaces_t, is_valid);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_otn_feature_get(
    int unit,
    dnx_data_nif_otn_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, feature);
}


uint32
dnx_data_nif_otn_nof_units_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_define_nof_units);
}

uint32
dnx_data_nif_otn_nof_lanes_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_define_nof_lanes);
}

uint32
dnx_data_nif_otn_nof_groups_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_define_nof_groups);
}

uint32
dnx_data_nif_otn_nof_lanes_per_group_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_define_nof_lanes_per_group);
}



const dnx_data_nif_otn_supported_interfaces_t *
dnx_data_nif_otn_supported_interfaces_get(
    int unit,
    int index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_table_supported_interfaces);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, index, 0);
    return (const dnx_data_nif_otn_supported_interfaces_t *) data;

}


shr_error_e
dnx_data_nif_otn_supported_interfaces_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_nif_otn_supported_interfaces_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_table_supported_interfaces);
    data = (const dnx_data_nif_otn_supported_interfaces_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->speed);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pm_dispatch_type);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->fec_type);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_lanes);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->otn_intf_name == NULL ? "" : data->otn_intf_name);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->is_valid);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_nif_otn_supported_interfaces_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_otn, dnx_data_nif_otn_table_supported_interfaces);

}






static shr_error_e
dnx_data_nif_flr_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "flr";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_nif_flr_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data nif flr features");

    submodule_data->features[dnx_data_nif_flr_is_advanced_flr_supported].name = "is_advanced_flr_supported";
    submodule_data->features[dnx_data_nif_flr_is_advanced_flr_supported].doc = "";
    submodule_data->features[dnx_data_nif_flr_is_advanced_flr_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_nif_flr_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data nif flr defines");

    submodule_data->defines[dnx_data_nif_flr_define_max_tick_unit].name = "max_tick_unit";
    submodule_data->defines[dnx_data_nif_flr_define_max_tick_unit].doc = "";
    
    submodule_data->defines[dnx_data_nif_flr_define_max_tick_unit].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flr_define_max_tick_count].name = "max_tick_count";
    submodule_data->defines[dnx_data_nif_flr_define_max_tick_count].doc = "";
    
    submodule_data->defines[dnx_data_nif_flr_define_max_tick_count].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_nif_flr_define_timer_granularity].name = "timer_granularity";
    submodule_data->defines[dnx_data_nif_flr_define_timer_granularity].doc = "";
    
    submodule_data->defines[dnx_data_nif_flr_define_timer_granularity].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_nif_flr_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data nif flr tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_nif_flr_feature_get(
    int unit,
    dnx_data_nif_flr_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flr, feature);
}


uint32
dnx_data_nif_flr_max_tick_unit_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flr, dnx_data_nif_flr_define_max_tick_unit);
}

uint32
dnx_data_nif_flr_max_tick_count_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flr, dnx_data_nif_flr_define_max_tick_count);
}

uint32
dnx_data_nif_flr_timer_granularity_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_nif, dnx_data_nif_submodule_flr, dnx_data_nif_flr_define_timer_granularity);
}







shr_error_e
dnx_data_nif_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "nif";
    module_data->nof_submodules = _dnx_data_nif_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data nif submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_nif_global_init(unit, &module_data->submodules[dnx_data_nif_submodule_global]));
    SHR_IF_ERR_EXIT(dnx_data_nif_phys_init(unit, &module_data->submodules[dnx_data_nif_submodule_phys]));
    SHR_IF_ERR_EXIT(dnx_data_nif_ilkn_init(unit, &module_data->submodules[dnx_data_nif_submodule_ilkn]));
    SHR_IF_ERR_EXIT(dnx_data_nif_eth_init(unit, &module_data->submodules[dnx_data_nif_submodule_eth]));
    SHR_IF_ERR_EXIT(dnx_data_nif_simulator_init(unit, &module_data->submodules[dnx_data_nif_submodule_simulator]));
    SHR_IF_ERR_EXIT(dnx_data_nif_framer_init(unit, &module_data->submodules[dnx_data_nif_submodule_framer]));
    SHR_IF_ERR_EXIT(dnx_data_nif_mac_client_init(unit, &module_data->submodules[dnx_data_nif_submodule_mac_client]));
    SHR_IF_ERR_EXIT(dnx_data_nif_sar_client_init(unit, &module_data->submodules[dnx_data_nif_submodule_sar_client]));
    SHR_IF_ERR_EXIT(dnx_data_nif_flexe_init(unit, &module_data->submodules[dnx_data_nif_submodule_flexe]));
    SHR_IF_ERR_EXIT(dnx_data_nif_wbc_init(unit, &module_data->submodules[dnx_data_nif_submodule_wbc]));
    SHR_IF_ERR_EXIT(dnx_data_nif_prd_init(unit, &module_data->submodules[dnx_data_nif_submodule_prd]));
    SHR_IF_ERR_EXIT(dnx_data_nif_portmod_init(unit, &module_data->submodules[dnx_data_nif_submodule_portmod]));
    SHR_IF_ERR_EXIT(dnx_data_nif_scheduler_init(unit, &module_data->submodules[dnx_data_nif_submodule_scheduler]));
    SHR_IF_ERR_EXIT(dnx_data_nif_dbal_init(unit, &module_data->submodules[dnx_data_nif_submodule_dbal]));
    SHR_IF_ERR_EXIT(dnx_data_nif_features_init(unit, &module_data->submodules[dnx_data_nif_submodule_features]));
    SHR_IF_ERR_EXIT(dnx_data_nif_arb_init(unit, &module_data->submodules[dnx_data_nif_submodule_arb]));
    SHR_IF_ERR_EXIT(dnx_data_nif_ofr_init(unit, &module_data->submodules[dnx_data_nif_submodule_ofr]));
    SHR_IF_ERR_EXIT(dnx_data_nif_oft_init(unit, &module_data->submodules[dnx_data_nif_submodule_oft]));
    SHR_IF_ERR_EXIT(dnx_data_nif_signal_quality_init(unit, &module_data->submodules[dnx_data_nif_submodule_signal_quality]));
    SHR_IF_ERR_EXIT(dnx_data_nif_otn_init(unit, &module_data->submodules[dnx_data_nif_submodule_otn]));
    SHR_IF_ERR_EXIT(dnx_data_nif_flr_init(unit, &module_data->submodules[dnx_data_nif_submodule_flr]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a1_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(q2a_b1_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_nif_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_nif_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_nif_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


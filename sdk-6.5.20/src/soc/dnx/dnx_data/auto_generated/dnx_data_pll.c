

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PLL

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_pll.h>



extern shr_error_e jr2_a0_data_pll_attach(
    int unit);
extern shr_error_e j2c_a0_data_pll_attach(
    int unit);
extern shr_error_e q2a_a0_data_pll_attach(
    int unit);
extern shr_error_e j2p_a0_data_pll_attach(
    int unit);



static shr_error_e
dnx_data_pll_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "PLL general configurations";
    
    submodule_data->nof_features = _dnx_data_pll_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pll general features");

    submodule_data->features[dnx_data_pll_general_ts_freq_lock].name = "ts_freq_lock";
    submodule_data->features[dnx_data_pll_general_ts_freq_lock].doc = "IEEE1588 DPLL mode";
    submodule_data->features[dnx_data_pll_general_ts_freq_lock].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pll_general_bs_enable].name = "bs_enable";
    submodule_data->features[dnx_data_pll_general_bs_enable].doc = "Broadsync clock enable";
    submodule_data->features[dnx_data_pll_general_bs_enable].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pll_general_is_supported].name = "is_supported";
    submodule_data->features[dnx_data_pll_general_is_supported].doc = "Indicate if PLL is supported";
    submodule_data->features[dnx_data_pll_general_is_supported].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pll_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pll general defines");

    
    submodule_data->nof_tables = _dnx_data_pll_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pll general tables");

    
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].name = "nif_pll";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].doc = "NIF PLL reference and output frequencies";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].size_of_values = sizeof(dnx_data_pll_general_nif_pll_t);
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].entry_get = dnx_data_pll_general_nif_pll_entry_str_get;

    
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].nof_keys = 1;
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].keys[0].name = "pll_index";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].keys[0].doc = "Index of NIF PLL (0 or 1)";

    
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pll_general_table_nif_pll].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pll_general_table_nif_pll].nof_values, "_dnx_data_pll_general_table_nif_pll table values");
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[0].name = "in_freq";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[0].type = "int";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[0].doc = "Reference clock frequency for the NIF SerDeses";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[0].offset = UTILEX_OFFSETOF(dnx_data_pll_general_nif_pll_t, in_freq);
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[1].name = "out_freq";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[1].type = "int";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[1].doc = "Output clock frequency for the NIF SerDeses";
    submodule_data->tables[dnx_data_pll_general_table_nif_pll].values[1].offset = UTILEX_OFFSETOF(dnx_data_pll_general_nif_pll_t, out_freq);

    
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].name = "fabric_pll";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].doc = "Fabric PLL reference and output frequencies";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].size_of_values = sizeof(dnx_data_pll_general_fabric_pll_t);
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].entry_get = dnx_data_pll_general_fabric_pll_entry_str_get;

    
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].nof_keys = 1;
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].keys[0].name = "pll_index";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].keys[0].doc = "Index of Fabric PLL (0 or 1)";

    
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pll_general_table_fabric_pll].nof_values, "_dnx_data_pll_general_table_fabric_pll table values");
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[0].name = "in_freq";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[0].type = "int";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[0].doc = "Output clock frequency for the Fabric SerDeses";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[0].offset = UTILEX_OFFSETOF(dnx_data_pll_general_fabric_pll_t, in_freq);
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[1].name = "out_freq";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[1].type = "int";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[1].doc = "Output clock frequency for the Fabric SerDeses";
    submodule_data->tables[dnx_data_pll_general_table_fabric_pll].values[1].offset = UTILEX_OFFSETOF(dnx_data_pll_general_fabric_pll_t, out_freq);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pll_general_feature_get(
    int unit,
    dnx_data_pll_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, feature);
}




const dnx_data_pll_general_nif_pll_t *
dnx_data_pll_general_nif_pll_get(
    int unit,
    int pll_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, dnx_data_pll_general_table_nif_pll);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pll_index, 0);
    return (const dnx_data_pll_general_nif_pll_t *) data;

}

const dnx_data_pll_general_fabric_pll_t *
dnx_data_pll_general_fabric_pll_get(
    int unit,
    int pll_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, dnx_data_pll_general_table_fabric_pll);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pll_index, 0);
    return (const dnx_data_pll_general_fabric_pll_t *) data;

}


shr_error_e
dnx_data_pll_general_nif_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pll_general_nif_pll_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, dnx_data_pll_general_table_nif_pll);
    data = (const dnx_data_pll_general_nif_pll_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->in_freq);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->out_freq);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_pll_general_fabric_pll_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pll_general_fabric_pll_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, dnx_data_pll_general_table_fabric_pll);
    data = (const dnx_data_pll_general_fabric_pll_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->in_freq);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->out_freq);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pll_general_nif_pll_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, dnx_data_pll_general_table_nif_pll);

}

const dnxc_data_table_info_t *
dnx_data_pll_general_fabric_pll_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_general, dnx_data_pll_general_table_fabric_pll);

}






static shr_error_e
dnx_data_pll_pll1_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pll1";
    submodule_data->doc = "PLL1 configurations";
    
    submodule_data->nof_features = _dnx_data_pll_pll1_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pll pll1 features");

    submodule_data->features[dnx_data_pll_pll1_ts_phase_initial_hi_config].name = "ts_phase_initial_hi_config";
    submodule_data->features[dnx_data_pll_pll1_ts_phase_initial_hi_config].doc = "Indication if configuring initial phase values high 32 is supported";
    submodule_data->features[dnx_data_pll_pll1_ts_phase_initial_hi_config].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_pll_pll1_ts_refclk_source_internal].name = "ts_refclk_source_internal";
    submodule_data->features[dnx_data_pll_pll1_ts_refclk_source_internal].doc = "Internal or external (1 for internal) reference clock for TS PLL1";
    submodule_data->features[dnx_data_pll_pll1_ts_refclk_source_internal].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pll_pll1_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pll pll1 defines");

    submodule_data->defines[dnx_data_pll_pll1_define_nof_pll1].name = "nof_pll1";
    submodule_data->defines[dnx_data_pll_pll1_define_nof_pll1].doc = "Number of PLL1";
    
    submodule_data->defines[dnx_data_pll_pll1_define_nof_pll1].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll1_define_ts_phase_initial_lo].name = "ts_phase_initial_lo";
    submodule_data->defines[dnx_data_pll_pll1_define_ts_phase_initial_lo].doc = "Initial phase values low 32";
    
    submodule_data->defines[dnx_data_pll_pll1_define_ts_phase_initial_lo].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_ts_phase_initial_hi].name = "ts_phase_initial_hi";
    submodule_data->defines[dnx_data_pll_pll1_define_ts_phase_initial_hi].doc = "Initial phase values high 32";
    
    submodule_data->defines[dnx_data_pll_pll1_define_ts_phase_initial_hi].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_ts_nof_bits_hi].name = "ts_nof_bits_hi";
    submodule_data->defines[dnx_data_pll_pll1_define_ts_nof_bits_hi].doc = "Number of high bits for presenting phase values";
    
    submodule_data->defines[dnx_data_pll_pll1_define_ts_nof_bits_hi].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_ts_nof_bits_right_shift_ns].name = "ts_nof_bits_right_shift_ns";
    submodule_data->defines[dnx_data_pll_pll1_define_ts_nof_bits_right_shift_ns].doc = "Number of bits to right shift to get nanoseconds.";
    
    submodule_data->defines[dnx_data_pll_pll1_define_ts_nof_bits_right_shift_ns].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_ts_pll1_id].name = "ts_pll1_id";
    submodule_data->defines[dnx_data_pll_pll1_define_ts_pll1_id].doc = "Index of the TS PLL1, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll1_define_ts_pll1_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_bs_pll1_id].name = "bs_pll1_id";
    submodule_data->defines[dnx_data_pll_pll1_define_bs_pll1_id].doc = "Index of the BS PLL1, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll1_define_bs_pll1_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_fab_pll1_id].name = "fab_pll1_id";
    submodule_data->defines[dnx_data_pll_pll1_define_fab_pll1_id].doc = "Index of the FAB PLL1, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll1_define_fab_pll1_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_nif_pll1_id].name = "nif_pll1_id";
    submodule_data->defines[dnx_data_pll_pll1_define_nif_pll1_id].doc = "Index of the NIF PLL1, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll1_define_nif_pll1_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll1_define_flexe_pll1_id].name = "flexe_pll1_id";
    submodule_data->defines[dnx_data_pll_pll1_define_flexe_pll1_id].doc = "Index of the FLEXE PLL1, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll1_define_flexe_pll1_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_pll_pll1_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pll pll1 tables");

    
    submodule_data->tables[dnx_data_pll_pll1_table_config].name = "config";
    submodule_data->tables[dnx_data_pll_pll1_table_config].doc = "PLL1 configuration parameters";
    submodule_data->tables[dnx_data_pll_pll1_table_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pll_pll1_table_config].size_of_values = sizeof(dnx_data_pll_pll1_config_t);
    submodule_data->tables[dnx_data_pll_pll1_table_config].entry_get = dnx_data_pll_pll1_config_entry_str_get;

    
    submodule_data->tables[dnx_data_pll_pll1_table_config].nof_keys = 1;
    submodule_data->tables[dnx_data_pll_pll1_table_config].keys[0].name = "pll1_type";
    submodule_data->tables[dnx_data_pll_pll1_table_config].keys[0].doc = "Type of PLL1 according to DNX_PLL1_TYPE enumerator";

    
    submodule_data->tables[dnx_data_pll_pll1_table_config].nof_values = 13;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pll_pll1_table_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pll_pll1_table_config].nof_values, "_dnx_data_pll_pll1_table_config table values");
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[0].name = "pll1_id";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[0].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[0].doc = "Index of the PLL1, used for DBAL access";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, pll1_id);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[1].name = "valid";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[1].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[1].doc = "Indication if the type of PLL1 is supported for device";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, valid);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[2].name = "pdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[2].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[2].doc = "Pre divider of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[2].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, pdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[3].name = "ndiv_int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[3].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[3].doc = "N divider of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[3].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ndiv_int);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[4].name = "ch0_mdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[4].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[4].doc = "M-divider for channel 0 of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[4].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ch0_mdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[5].name = "ch1_mdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[5].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[5].doc = "M-divider for channel 1 of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[5].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ch1_mdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[6].name = "ch2_mdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[6].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[6].doc = "M-divider for channel 2 of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[6].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ch2_mdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[7].name = "ch3_mdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[7].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[7].doc = "M-divider for channel 3 of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[7].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ch3_mdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[8].name = "ch4_mdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[8].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[8].doc = "M-divider for channel 4 of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[8].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ch4_mdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[9].name = "ch5_mdiv";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[9].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[9].doc = "M-divider for channel 5 of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[9].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, ch5_mdiv);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[10].name = "output_cml_en";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[10].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[10].doc = "Enable output CML of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[10].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, output_cml_en);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[11].name = "refclk_source_sel";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[11].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[11].doc = "Reference clock source select";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[11].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, refclk_source_sel);
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[12].name = "refclk";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[12].type = "int";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[12].doc = "Reference clock of PLL1";
    submodule_data->tables[dnx_data_pll_pll1_table_config].values[12].offset = UTILEX_OFFSETOF(dnx_data_pll_pll1_config_t, refclk);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pll_pll1_feature_get(
    int unit,
    dnx_data_pll_pll1_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, feature);
}


uint32
dnx_data_pll_pll1_nof_pll1_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_nof_pll1);
}

uint32
dnx_data_pll_pll1_ts_phase_initial_lo_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_ts_phase_initial_lo);
}

uint32
dnx_data_pll_pll1_ts_phase_initial_hi_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_ts_phase_initial_hi);
}

uint32
dnx_data_pll_pll1_ts_nof_bits_hi_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_ts_nof_bits_hi);
}

uint32
dnx_data_pll_pll1_ts_nof_bits_right_shift_ns_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_ts_nof_bits_right_shift_ns);
}

uint32
dnx_data_pll_pll1_ts_pll1_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_ts_pll1_id);
}

uint32
dnx_data_pll_pll1_bs_pll1_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_bs_pll1_id);
}

uint32
dnx_data_pll_pll1_fab_pll1_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_fab_pll1_id);
}

uint32
dnx_data_pll_pll1_nif_pll1_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_nif_pll1_id);
}

uint32
dnx_data_pll_pll1_flexe_pll1_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_define_flexe_pll1_id);
}



const dnx_data_pll_pll1_config_t *
dnx_data_pll_pll1_config_get(
    int unit,
    int pll1_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_table_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pll1_type, 0);
    return (const dnx_data_pll_pll1_config_t *) data;

}


shr_error_e
dnx_data_pll_pll1_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pll_pll1_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_table_config);
    data = (const dnx_data_pll_pll1_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pll1_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pdiv);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ndiv_int);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch0_mdiv);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch1_mdiv);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch2_mdiv);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch3_mdiv);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch4_mdiv);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch5_mdiv);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->output_cml_en);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->refclk_source_sel);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->refclk);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pll_pll1_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll1, dnx_data_pll_pll1_table_config);

}






static shr_error_e
dnx_data_pll_pll3_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pll3";
    submodule_data->doc = "PLL3 configurations";
    
    submodule_data->nof_features = _dnx_data_pll_pll3_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pll pll3 features");

    
    submodule_data->nof_defines = _dnx_data_pll_pll3_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pll pll3 defines");

    submodule_data->defines[dnx_data_pll_pll3_define_nof_pll3].name = "nof_pll3";
    submodule_data->defines[dnx_data_pll_pll3_define_nof_pll3].doc = "Number of PLL3";
    
    submodule_data->defines[dnx_data_pll_pll3_define_nof_pll3].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_vco_clock].name = "vco_clock";
    submodule_data->defines[dnx_data_pll_pll3_define_vco_clock].doc = "Frequency of the Voltage Control Oscilator of PLL3. Used to calculate Pdiv, Ndiv, Mdiv and the Effective Ferquency";
    
    submodule_data->defines[dnx_data_pll_pll3_define_vco_clock].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_kp].name = "kp";
    submodule_data->defines[dnx_data_pll_pll3_define_kp].doc = "KP parameter for PLL3 configuration";
    
    submodule_data->defines[dnx_data_pll_pll3_define_kp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_ki].name = "ki";
    submodule_data->defines[dnx_data_pll_pll3_define_ki].doc = "Ki parameter for PLL3 configuration";
    
    submodule_data->defines[dnx_data_pll_pll3_define_ki].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_en_ctrl].name = "en_ctrl";
    submodule_data->defines[dnx_data_pll_pll3_define_en_ctrl].doc = "Enable control";
    
    submodule_data->defines[dnx_data_pll_pll3_define_en_ctrl].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_en_ctrl_byp].name = "en_ctrl_byp";
    submodule_data->defines[dnx_data_pll_pll3_define_en_ctrl_byp].doc = "Enable control for bypass mode";
    
    submodule_data->defines[dnx_data_pll_pll3_define_en_ctrl_byp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_route_ctr_byp].name = "route_ctr_byp";
    submodule_data->defines[dnx_data_pll_pll3_define_route_ctr_byp].doc = "Value of route control field when using bypass";
    
    submodule_data->defines[dnx_data_pll_pll3_define_route_ctr_byp].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_125].name = "ref_clock_125";
    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_125].doc = "Value for 125MHz reference clock";
    
    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_125].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_156_25].name = "ref_clock_156_25";
    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_156_25].doc = "Value for 156.25MHz reference clock.";
    
    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_156_25].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_312_5].name = "ref_clock_312_5";
    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_312_5].doc = "Value for 312.5MHz reference clock.";
    
    submodule_data->defines[dnx_data_pll_pll3_define_ref_clock_312_5].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll3_define_nif0_pll3_id].name = "nif0_pll3_id";
    submodule_data->defines[dnx_data_pll_pll3_define_nif0_pll3_id].doc = "Index of the NIF0 PLL3, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll3_define_nif0_pll3_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll3_define_nif1_pll3_id].name = "nif1_pll3_id";
    submodule_data->defines[dnx_data_pll_pll3_define_nif1_pll3_id].doc = "Index of the NIF1 PLL3, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll3_define_nif1_pll3_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll3_define_mas0_pll3_id].name = "mas0_pll3_id";
    submodule_data->defines[dnx_data_pll_pll3_define_mas0_pll3_id].doc = "Index of the MAS0 PLL3, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll3_define_mas0_pll3_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll3_define_mas1_pll3_id].name = "mas1_pll3_id";
    submodule_data->defines[dnx_data_pll_pll3_define_mas1_pll3_id].doc = "Index of the MAS1 PLL3, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll3_define_mas1_pll3_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_pll_pll3_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pll pll3 tables");

    
    submodule_data->tables[dnx_data_pll_pll3_table_config].name = "config";
    submodule_data->tables[dnx_data_pll_pll3_table_config].doc = "PLL3 configuration parameters";
    submodule_data->tables[dnx_data_pll_pll3_table_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pll_pll3_table_config].size_of_values = sizeof(dnx_data_pll_pll3_config_t);
    submodule_data->tables[dnx_data_pll_pll3_table_config].entry_get = dnx_data_pll_pll3_config_entry_str_get;

    
    submodule_data->tables[dnx_data_pll_pll3_table_config].nof_keys = 1;
    submodule_data->tables[dnx_data_pll_pll3_table_config].keys[0].name = "pll3_type";
    submodule_data->tables[dnx_data_pll_pll3_table_config].keys[0].doc = "Type of PLL1 according to DNX_PLL1_TYPE enumerator";

    
    submodule_data->tables[dnx_data_pll_pll3_table_config].nof_values = 7;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pll_pll3_table_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pll_pll3_table_config].nof_values, "_dnx_data_pll_pll3_table_config table values");
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[0].name = "pll3_id";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[0].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[0].doc = "Index of the PLL1, used for DBAL access";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, pll3_id);
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[1].name = "valid";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[1].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[1].doc = "Indication if the type of PLL3 is supported for device";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, valid);
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[2].name = "ch1_mdiv";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[2].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[2].doc = "M-divider for channel 1 of PLL3";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[2].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, ch1_mdiv);
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[3].name = "ch2_mdiv";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[3].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[3].doc = "M-divider for channel 2 of PLL3";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[3].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, ch2_mdiv);
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[4].name = "ch3_mdiv";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[4].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[4].doc = "M-divider for channel 3 of PLL3";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[4].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, ch3_mdiv);
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[5].name = "ch4_mdiv";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[5].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[5].doc = "M-divider for channel 4 of PLL3";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[5].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, ch4_mdiv);
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[6].name = "ch5_mdiv";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[6].type = "int";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[6].doc = "M-divider for channel 5 of PLL3";
    submodule_data->tables[dnx_data_pll_pll3_table_config].values[6].offset = UTILEX_OFFSETOF(dnx_data_pll_pll3_config_t, ch5_mdiv);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pll_pll3_feature_get(
    int unit,
    dnx_data_pll_pll3_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, feature);
}


uint32
dnx_data_pll_pll3_nof_pll3_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_nof_pll3);
}

uint32
dnx_data_pll_pll3_vco_clock_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_vco_clock);
}

uint32
dnx_data_pll_pll3_kp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_kp);
}

uint32
dnx_data_pll_pll3_ki_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_ki);
}

uint32
dnx_data_pll_pll3_en_ctrl_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_en_ctrl);
}

uint32
dnx_data_pll_pll3_en_ctrl_byp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_en_ctrl_byp);
}

uint32
dnx_data_pll_pll3_route_ctr_byp_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_route_ctr_byp);
}

uint32
dnx_data_pll_pll3_ref_clock_125_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_ref_clock_125);
}

uint32
dnx_data_pll_pll3_ref_clock_156_25_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_ref_clock_156_25);
}

uint32
dnx_data_pll_pll3_ref_clock_312_5_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_ref_clock_312_5);
}

uint32
dnx_data_pll_pll3_nif0_pll3_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_nif0_pll3_id);
}

uint32
dnx_data_pll_pll3_nif1_pll3_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_nif1_pll3_id);
}

uint32
dnx_data_pll_pll3_mas0_pll3_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_mas0_pll3_id);
}

uint32
dnx_data_pll_pll3_mas1_pll3_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_define_mas1_pll3_id);
}



const dnx_data_pll_pll3_config_t *
dnx_data_pll_pll3_config_get(
    int unit,
    int pll3_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_table_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pll3_type, 0);
    return (const dnx_data_pll_pll3_config_t *) data;

}


shr_error_e
dnx_data_pll_pll3_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pll_pll3_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_table_config);
    data = (const dnx_data_pll_pll3_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pll3_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch1_mdiv);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch2_mdiv);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch3_mdiv);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch4_mdiv);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch5_mdiv);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pll_pll3_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll3, dnx_data_pll_pll3_table_config);

}






static shr_error_e
dnx_data_pll_pll4_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "pll4";
    submodule_data->doc = "PLL4 configurations";
    
    submodule_data->nof_features = _dnx_data_pll_pll4_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pll pll4 features");

    
    submodule_data->nof_defines = _dnx_data_pll_pll4_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pll pll4 defines");

    submodule_data->defines[dnx_data_pll_pll4_define_nof_pll4].name = "nof_pll4";
    submodule_data->defines[dnx_data_pll_pll4_define_nof_pll4].doc = "Number of PLL4";
    
    submodule_data->defines[dnx_data_pll_pll4_define_nof_pll4].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_pll_pll4_define_ts_pll4_id].name = "ts_pll4_id";
    submodule_data->defines[dnx_data_pll_pll4_define_ts_pll4_id].doc = "Index of the TS PLL4, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll4_define_ts_pll4_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll4_define_bs_pll4_id].name = "bs_pll4_id";
    submodule_data->defines[dnx_data_pll_pll4_define_bs_pll4_id].doc = "Index of the BS PLL4, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll4_define_bs_pll4_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll4_define_nif_pll4_id].name = "nif_pll4_id";
    submodule_data->defines[dnx_data_pll_pll4_define_nif_pll4_id].doc = "Index of the NIF PLL4, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll4_define_nif_pll4_id].flags |= DNXC_DATA_F_NUMERIC;

    submodule_data->defines[dnx_data_pll_pll4_define_fab_pll4_id].name = "fab_pll4_id";
    submodule_data->defines[dnx_data_pll_pll4_define_fab_pll4_id].doc = "Index of the FSRD PLL4, used for DBAL access";
    
    submodule_data->defines[dnx_data_pll_pll4_define_fab_pll4_id].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_pll_pll4_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pll pll4 tables");

    
    submodule_data->tables[dnx_data_pll_pll4_table_config].name = "config";
    submodule_data->tables[dnx_data_pll_pll4_table_config].doc = "PLL4 configuration parameters";
    submodule_data->tables[dnx_data_pll_pll4_table_config].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_pll_pll4_table_config].size_of_values = sizeof(dnx_data_pll_pll4_config_t);
    submodule_data->tables[dnx_data_pll_pll4_table_config].entry_get = dnx_data_pll_pll4_config_entry_str_get;

    
    submodule_data->tables[dnx_data_pll_pll4_table_config].nof_keys = 1;
    submodule_data->tables[dnx_data_pll_pll4_table_config].keys[0].name = "pll4_type";
    submodule_data->tables[dnx_data_pll_pll4_table_config].keys[0].doc = "Type of PLL4 according to DNX_PLL4_TYPE enumerator";

    
    submodule_data->tables[dnx_data_pll_pll4_table_config].nof_values = 15;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_pll_pll4_table_config].values, dnxc_data_value_t, submodule_data->tables[dnx_data_pll_pll4_table_config].nof_values, "_dnx_data_pll_pll4_table_config table values");
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[0].name = "pll4_id";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[0].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[0].doc = "Index of the PLL4, used for DBAL access";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[0].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, pll4_id);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[1].name = "valid";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[1].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[1].doc = "Indication if the type of PLL3 is supported for device";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[1].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, valid);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[2].name = "ref_clk_src_sel";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[2].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[2].doc = "Reference clock source select of PLL4 - 1 from pad, 0 from TS";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[2].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ref_clk_src_sel);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[3].name = "ref_clk";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[3].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[3].doc = "Reference clock of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[3].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ref_clk);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[4].name = "dll_ref_pdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[4].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[4].doc = "Pre divider of Delay-locked loop of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[4].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, dll_ref_pdiv);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[5].name = "dll_post_en";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[5].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[5].doc = "Enable of Delay-locked loop of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[5].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, dll_post_en);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[6].name = "aux_post_en";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[6].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[6].doc = "Enable of Auxiliary Oscillator of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[6].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, aux_post_en);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[7].name = "pdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[7].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[7].doc = "Pre divider of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[7].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, pdiv);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[8].name = "ndiv_int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[8].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[8].doc = "N divider of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[8].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ndiv_int);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[9].name = "d2c_en";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[9].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[9].doc = "Enable differential reference clock to CMOS";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[9].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, d2c_en);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[10].name = "ch0_mdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[10].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[10].doc = "M-divider for channel 0 of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[10].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ch0_mdiv);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[11].name = "ch1_mdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[11].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[11].doc = "M-divider for channel 1 of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[11].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ch1_mdiv);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[12].name = "ch2_mdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[12].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[12].doc = "M-divider for channel 2 of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[12].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ch2_mdiv);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[13].name = "ch3_mdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[13].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[13].doc = "M-divider for channel 3 of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[13].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ch3_mdiv);
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[14].name = "ch6_mdiv";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[14].type = "int";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[14].doc = "M-divider for channel 6 of PLL4";
    submodule_data->tables[dnx_data_pll_pll4_table_config].values[14].offset = UTILEX_OFFSETOF(dnx_data_pll_pll4_config_t, ch6_mdiv);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pll_pll4_feature_get(
    int unit,
    dnx_data_pll_pll4_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, feature);
}


uint32
dnx_data_pll_pll4_nof_pll4_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_define_nof_pll4);
}

uint32
dnx_data_pll_pll4_ts_pll4_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_define_ts_pll4_id);
}

uint32
dnx_data_pll_pll4_bs_pll4_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_define_bs_pll4_id);
}

uint32
dnx_data_pll_pll4_nif_pll4_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_define_nif_pll4_id);
}

uint32
dnx_data_pll_pll4_fab_pll4_id_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_define_fab_pll4_id);
}



const dnx_data_pll_pll4_config_t *
dnx_data_pll_pll4_config_get(
    int unit,
    int pll4_type)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_table_config);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pll4_type, 0);
    return (const dnx_data_pll_pll4_config_t *) data;

}


shr_error_e
dnx_data_pll_pll4_config_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_pll_pll4_config_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_table_config);
    data = (const dnx_data_pll_pll4_config_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pll4_id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ref_clk_src_sel);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ref_clk);
            break;
        case 4:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dll_ref_pdiv);
            break;
        case 5:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dll_post_en);
            break;
        case 6:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->aux_post_en);
            break;
        case 7:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->pdiv);
            break;
        case 8:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ndiv_int);
            break;
        case 9:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->d2c_en);
            break;
        case 10:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch0_mdiv);
            break;
        case 11:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch1_mdiv);
            break;
        case 12:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch2_mdiv);
            break;
        case 13:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch3_mdiv);
            break;
        case 14:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->ch6_mdiv);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_pll_pll4_config_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_pll4, dnx_data_pll_pll4_table_config);

}






static shr_error_e
dnx_data_pll_synce_pll_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "synce_pll";
    submodule_data->doc = "synce PLL general configurations";
    
    submodule_data->nof_features = _dnx_data_pll_synce_pll_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data pll synce_pll features");

    submodule_data->features[dnx_data_pll_synce_pll_present].name = "present";
    submodule_data->features[dnx_data_pll_synce_pll_present].doc = "Marks presence of synce pll";
    submodule_data->features[dnx_data_pll_synce_pll_present].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_pll_synce_pll_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data pll synce_pll defines");

    
    submodule_data->nof_tables = _dnx_data_pll_synce_pll_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data pll synce_pll tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_pll_synce_pll_feature_get(
    int unit,
    dnx_data_pll_synce_pll_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_pll, dnx_data_pll_submodule_synce_pll, feature);
}








shr_error_e
dnx_data_pll_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "pll";
    module_data->nof_submodules = _dnx_data_pll_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data pll submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_pll_general_init(unit, &module_data->submodules[dnx_data_pll_submodule_general]));
    SHR_IF_ERR_EXIT(dnx_data_pll_pll1_init(unit, &module_data->submodules[dnx_data_pll_submodule_pll1]));
    SHR_IF_ERR_EXIT(dnx_data_pll_pll3_init(unit, &module_data->submodules[dnx_data_pll_submodule_pll3]));
    SHR_IF_ERR_EXIT(dnx_data_pll_pll4_init(unit, &module_data->submodules[dnx_data_pll_submodule_pll4]));
    SHR_IF_ERR_EXIT(dnx_data_pll_synce_pll_init(unit, &module_data->submodules[dnx_data_pll_submodule_synce_pll]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_pll_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_pll_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_pll_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


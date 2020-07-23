

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SYNCEDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_synce.h>



extern shr_error_e jr2_a0_data_synce_attach(
    int unit);
extern shr_error_e j2c_a0_data_synce_attach(
    int unit);
extern shr_error_e q2a_a0_data_synce_attach(
    int unit);
extern shr_error_e j2p_a0_data_synce_attach(
    int unit);



static shr_error_e
dnx_data_synce_general_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "general";
    submodule_data->doc = "Synchronous Ethernet general attributes shared by Nif and Fabric SyncE.";
    
    submodule_data->nof_features = _dnx_data_synce_general_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data synce general features");

    submodule_data->features[dnx_data_synce_general_synce_no_sdm_mode].name = "synce_no_sdm_mode";
    submodule_data->features[dnx_data_synce_general_synce_no_sdm_mode].doc = "Indicates SYNCE is NOT in SDM mode.";
    submodule_data->features[dnx_data_synce_general_synce_no_sdm_mode].flags |= DNXC_DATA_F_FEATURE;

    submodule_data->features[dnx_data_synce_general_synce_nmg_chain_select].name = "synce_nmg_chain_select";
    submodule_data->features[dnx_data_synce_general_synce_nmg_chain_select].doc = "NMG_SYNC_CHAIN_SEL is needed or not to be configured to select NMG chain 0 or 1";
    submodule_data->features[dnx_data_synce_general_synce_nmg_chain_select].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_synce_general_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data synce general defines");

    submodule_data->defines[dnx_data_synce_general_define_nof_plls].name = "nof_plls";
    submodule_data->defines[dnx_data_synce_general_define_nof_plls].doc = "Number of synce plls in the device.";
    
    submodule_data->defines[dnx_data_synce_general_define_nof_plls].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_nif_div_min].name = "nif_div_min";
    submodule_data->defines[dnx_data_synce_general_define_nif_div_min].doc = "Min value for the nif synce output clk.";
    
    submodule_data->defines[dnx_data_synce_general_define_nif_div_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_nif_div_max].name = "nif_div_max";
    submodule_data->defines[dnx_data_synce_general_define_nif_div_max].doc = "Max value for the nif synce output clk.";
    
    submodule_data->defines[dnx_data_synce_general_define_nif_div_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_fabric_div_min].name = "fabric_div_min";
    submodule_data->defines[dnx_data_synce_general_define_fabric_div_min].doc = "Min value for the fabric synce output clk.";
    
    submodule_data->defines[dnx_data_synce_general_define_fabric_div_min].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_fabric_div_max].name = "fabric_div_max";
    submodule_data->defines[dnx_data_synce_general_define_fabric_div_max].doc = "Max value for the fabric synce output clk.";
    
    submodule_data->defines[dnx_data_synce_general_define_fabric_div_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_nof_clk_blocks].name = "nof_clk_blocks";
    submodule_data->defines[dnx_data_synce_general_define_nof_clk_blocks].doc = "Max value for the nif synce clock blocks.";
    
    submodule_data->defines[dnx_data_synce_general_define_nof_clk_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_nof_fabric_clk_blocks].name = "nof_fabric_clk_blocks";
    submodule_data->defines[dnx_data_synce_general_define_nof_fabric_clk_blocks].doc = "Max value for the fabric synce clock blocks.";
    
    submodule_data->defines[dnx_data_synce_general_define_nof_fabric_clk_blocks].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_synce_general_define_output_clk_mode].name = "output_clk_mode";
    submodule_data->defines[dnx_data_synce_general_define_output_clk_mode].doc = "SyncE output clock mode.";
    
    submodule_data->defines[dnx_data_synce_general_define_output_clk_mode].flags |= DNXC_DATA_F_NUMERIC;

    
    submodule_data->nof_tables = _dnx_data_synce_general_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data synce general tables");

    
    submodule_data->tables[dnx_data_synce_general_table_cfg].name = "cfg";
    submodule_data->tables[dnx_data_synce_general_table_cfg].doc = "SyncE common configuration.";
    submodule_data->tables[dnx_data_synce_general_table_cfg].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_synce_general_table_cfg].size_of_values = sizeof(dnx_data_synce_general_cfg_t);
    submodule_data->tables[dnx_data_synce_general_table_cfg].entry_get = dnx_data_synce_general_cfg_entry_str_get;

    
    submodule_data->tables[dnx_data_synce_general_table_cfg].nof_keys = 1;
    submodule_data->tables[dnx_data_synce_general_table_cfg].keys[0].name = "synce_index";
    submodule_data->tables[dnx_data_synce_general_table_cfg].keys[0].doc = "Master or Slave SyncE.";

    
    submodule_data->tables[dnx_data_synce_general_table_cfg].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_synce_general_table_cfg].values, dnxc_data_value_t, submodule_data->tables[dnx_data_synce_general_table_cfg].nof_values, "_dnx_data_synce_general_table_cfg table values");
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[0].name = "source_clock_port";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[0].type = "bcm_port_t";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[0].doc = "Select the SyncE source port for SyncE PLL.";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[0].offset = UTILEX_OFFSETOF(dnx_data_synce_general_cfg_t, source_clock_port);
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[1].name = "squelch_enable";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[1].type = "uint32";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[1].doc = "Enable or Disable the SyncE auto-squelch.";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[1].offset = UTILEX_OFFSETOF(dnx_data_synce_general_cfg_t, squelch_enable);
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[2].name = "output_clock_sel";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[2].type = "uint32";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[2].doc = "SyncE output clock select.";
    submodule_data->tables[dnx_data_synce_general_table_cfg].values[2].offset = UTILEX_OFFSETOF(dnx_data_synce_general_cfg_t, output_clock_sel);

    
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].name = "nmg_chain_map";
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].doc = "ethu_id mapped to NMG chain related to SYNCE";
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].size_of_values = sizeof(dnx_data_synce_general_nmg_chain_map_t);
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].entry_get = dnx_data_synce_general_nmg_chain_map_entry_str_get;

    
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].nof_keys = 1;
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].keys[0].name = "ethu_index";
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].keys[0].doc = "ethu instance number.";

    
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].values, dnxc_data_value_t, submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].nof_values, "_dnx_data_synce_general_table_nmg_chain_map table values");
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].values[0].name = "chain_id";
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].values[0].type = "uint32";
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].values[0].doc = "NMG chain id";
    submodule_data->tables[dnx_data_synce_general_table_nmg_chain_map].values[0].offset = UTILEX_OFFSETOF(dnx_data_synce_general_nmg_chain_map_t, chain_id);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_synce_general_feature_get(
    int unit,
    dnx_data_synce_general_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, feature);
}


uint32
dnx_data_synce_general_nof_plls_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_nof_plls);
}

uint32
dnx_data_synce_general_nif_div_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_nif_div_min);
}

uint32
dnx_data_synce_general_nif_div_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_nif_div_max);
}

uint32
dnx_data_synce_general_fabric_div_min_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_fabric_div_min);
}

uint32
dnx_data_synce_general_fabric_div_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_fabric_div_max);
}

uint32
dnx_data_synce_general_nof_clk_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_nof_clk_blocks);
}

uint32
dnx_data_synce_general_nof_fabric_clk_blocks_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_nof_fabric_clk_blocks);
}

uint32
dnx_data_synce_general_output_clk_mode_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_define_output_clk_mode);
}



const dnx_data_synce_general_cfg_t *
dnx_data_synce_general_cfg_get(
    int unit,
    int synce_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_table_cfg);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, synce_index, 0);
    return (const dnx_data_synce_general_cfg_t *) data;

}

const dnx_data_synce_general_nmg_chain_map_t *
dnx_data_synce_general_nmg_chain_map_get(
    int unit,
    int ethu_index)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_table_nmg_chain_map);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, ethu_index, 0);
    return (const dnx_data_synce_general_nmg_chain_map_t *) data;

}


shr_error_e
dnx_data_synce_general_cfg_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_synce_general_cfg_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_table_cfg);
    data = (const dnx_data_synce_general_cfg_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->source_clock_port);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->squelch_enable);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->output_clock_sel);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_synce_general_nmg_chain_map_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_synce_general_nmg_chain_map_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_table_nmg_chain_map);
    data = (const dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->chain_id);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_synce_general_cfg_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_table_cfg);

}

const dnxc_data_table_info_t *
dnx_data_synce_general_nmg_chain_map_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_synce, dnx_data_synce_submodule_general, dnx_data_synce_general_table_nmg_chain_map);

}



shr_error_e
dnx_data_synce_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "synce";
    module_data->nof_submodules = _dnx_data_synce_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data synce submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_synce_general_init(unit, &module_data->submodules[dnx_data_synce_submodule_general]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_synce_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_synce_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_synce_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


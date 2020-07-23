

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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>







static shr_error_e
q2a_a0_dnx_data_synce_general_synce_no_sdm_mode_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int feature_index = dnx_data_synce_general_synce_no_sdm_mode;
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
q2a_a0_dnx_data_synce_general_synce_nmg_chain_select_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int feature_index = dnx_data_synce_general_synce_nmg_chain_select;
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
q2a_a0_dnx_data_synce_general_fabric_div_min_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int define_index = dnx_data_synce_general_define_fabric_div_min;
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
q2a_a0_dnx_data_synce_general_fabric_div_max_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int define_index = dnx_data_synce_general_define_fabric_div_max;
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
q2a_a0_dnx_data_synce_general_cfg_set(
    int unit)
{
    int synce_index_index;
    dnx_data_synce_general_cfg_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int table_index = dnx_data_synce_general_table_cfg;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 2;
    table->info_get.key_size[0] = 2;

    
    table->values[0].default_val = "-1";
    table->values[1].default_val = "1";
    table->values[2].default_val = "2";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_synce_general_cfg_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_synce_general_table_cfg");

    
    default_data = (dnx_data_synce_general_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->source_clock_port = -1;
    default_data->squelch_enable = 1;
    default_data->output_clock_sel = 2;
    
    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnx_data_synce_general_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SYNC_ETH_CLK_TO_PORT_ID_CLK;
    table->values[0].property.doc =
        "\n"
        "Specify the logical port number that will drive the recovered clock.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range_signed;
    table->values[0].property.method_str = "suffix_range_signed";
    table->values[0].property.suffix = "";
    table->values[0].property.range_min = 0;
    table->values[0].property.range_max = dnx_data_port.general.fabric_port_base_get(unit) + dnx_data_fabric.links.nof_links_get(unit);
    
    table->values[1].property.name = spn_SYNC_ETH_CLK_SQUELCH_ENABLE;
    table->values[1].property.doc =
        "\n"
        "Enable or Disable SyncE auto squelch mode.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_suffix_enable;
    table->values[1].property.method_str = "suffix_enable";
    table->values[1].property.suffix = "clk_";
    
    table->values[2].property.name = spn_SYNC_ETH_CLK_DIVIDER;
    table->values[2].property.doc =
        "\n"
        "Select the SyncE output clock\n"
        "For NIF synce, the available value is,\n"
        "2: 25Mhz.\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_suffix_range;
    table->values[2].property.method_str = "suffix_range";
    table->values[2].property.suffix = "clk";
    table->values[2].property.range_min = 2;
    table->values[2].property.range_max = 2;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnx_data_synce_general_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, synce_index_index, &data->source_clock_port));
    }
    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnx_data_synce_general_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, synce_index_index, &data->squelch_enable));
    }
    for (synce_index_index = 0; synce_index_index < table->keys[0].size; synce_index_index++)
    {
        data = (dnx_data_synce_general_cfg_t *) dnxc_data_mgmt_table_data_get(unit, table, synce_index_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, synce_index_index, &data->output_clock_sel));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_synce_general_nmg_chain_map_set(
    int unit)
{
    int ethu_index_index;
    dnx_data_synce_general_nmg_chain_map_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_synce;
    int submodule_index = dnx_data_synce_submodule_general;
    int table_index = dnx_data_synce_general_table_nmg_chain_map;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_synce_general_nmg_chain_map_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_synce_general_table_nmg_chain_map");

    
    default_data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->chain_id = 0;
    
    for (ethu_index_index = 0; ethu_index_index < table->keys[0].size; ethu_index_index++)
    {
        data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, ethu_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->chain_id = 1;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->chain_id = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->chain_id = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->chain_id = 0;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_synce_general_nmg_chain_map_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->chain_id = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_synce_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_synce;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_synce_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_synce_general_define_fabric_div_min;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_synce_general_fabric_div_min_set;
    data_index = dnx_data_synce_general_define_fabric_div_max;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_synce_general_fabric_div_max_set;

    
    data_index = dnx_data_synce_general_synce_no_sdm_mode;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_synce_general_synce_no_sdm_mode_set;
    data_index = dnx_data_synce_general_synce_nmg_chain_select;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_synce_general_synce_nmg_chain_select_set;

    
    data_index = dnx_data_synce_general_table_cfg;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_synce_general_cfg_set;
    data_index = dnx_data_synce_general_table_nmg_chain_map;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_synce_general_nmg_chain_map_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


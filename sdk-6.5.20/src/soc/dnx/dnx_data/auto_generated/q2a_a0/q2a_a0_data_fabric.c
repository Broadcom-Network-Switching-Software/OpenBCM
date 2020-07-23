

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
#include <include/bcm_int/dnx/fabric/fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_iqs.h>







static shr_error_e
q2a_a0_dnx_data_fabric_general_blocks_exist_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int feature_index = dnx_data_fabric_general_blocks_exist;
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
q2a_a0_dnx_data_fabric_general_connect_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_general;
    int define_index = dnx_data_fabric_general_define_connect_mode;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    define->data = DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_FABRIC_CONNECT_MODE;
    define->property.doc = 
        "\n"
        "Set the connect mode of the Fabric:\n"
        "SINGLE_FAP - stand-alone device.\n"
        "Default: SINGLE_FAP\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_direct_map;
    define->property.method_str = "direct_map";
    define->property.nof_mapping = 1;
    DNXC_DATA_ALLOC(define->property.mapping, dnxc_data_property_map_t, define->property.nof_mapping, "dnxc_data property mapping");

    define->property.mapping[0].name = "SINGLE_FAP";
    define->property.mapping[0].val = DNX_FABRIC_CONNECT_MODE_SINGLE_FAP;
    define->property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



static shr_error_e
q2a_a0_dnx_data_fabric_general_pm_properties_set(
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
q2a_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set(
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
q2a_a0_dnx_data_fabric_links_nof_links_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_links;
    int define_index = dnx_data_fabric_links_define_nof_links;
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
q2a_a0_dnx_data_fabric_blocks_nof_pms_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_blocks;
    int define_index = dnx_data_fabric_blocks_define_nof_pms;
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
q2a_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set(
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

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_fabric_tdm_priority_set(
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
q2a_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof;
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
q2a_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value;
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
q2a_a0_dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof;
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
q2a_a0_dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_dbal;
    int define_index = dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 13;

    
    define->data = 13;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
q2a_a0_dnx_data_fabric_transmit_eir_fc_leaky_backet_set(
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
q2a_a0_dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_set(
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
q2a_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_set(
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
q2a_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_set(
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


static shr_error_e
q2a_a0_dnx_data_fabric_transmit_dtq_fc_local_thresholds_set(
    int unit)
{
    int dtq_fc_index;
    dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_fabric;
    int submodule_index = dnx_data_fabric_submodule_transmit;
    int table_index = dnx_data_fabric_transmit_table_dtq_fc_local_thresholds;
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

    
    DNXC_DATA_ALLOC(table->data, dnx_data_fabric_transmit_dtq_fc_local_thresholds_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_fabric_transmit_table_dtq_fc_local_thresholds");

    
    default_data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->threshold = 0;
    
    for (dtq_fc_index = 0; dtq_fc_index < table->keys[0].size; dtq_fc_index++)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, dtq_fc_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->threshold = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->threshold = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->threshold = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_fabric_transmit_dtq_fc_local_thresholds_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->threshold = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_fabric_attach(
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

    
    data_index = dnx_data_fabric_general_define_connect_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_general_connect_mode_set;

    
    data_index = dnx_data_fabric_general_blocks_exist;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_fabric_general_blocks_exist_set;

    
    data_index = dnx_data_fabric_general_table_pm_properties;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_general_pm_properties_set;
    
    submodule_index = dnx_data_fabric_submodule_links;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_links_define_nof_links;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_links_nof_links_set;

    
    data_index = dnx_data_fabric_links_load_balancing_periodic_event_enabled;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_fabric_links_load_balancing_periodic_event_enabled_set;

    
    
    submodule_index = dnx_data_fabric_submodule_blocks;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_blocks_define_nof_pms;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_blocks_nof_pms_set;

    

    
    data_index = dnx_data_fabric_blocks_table_fsrd_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_blocks_fsrd_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fmac_sbus_chain;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_blocks_fmac_sbus_chain_set;
    data_index = dnx_data_fabric_blocks_table_fsrd_ref_clk_master;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_blocks_fsrd_ref_clk_master_set;
    
    submodule_index = dnx_data_fabric_submodule_tdm;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_tdm_define_priority;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_tdm_priority_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_dbal;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_bits_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_wfq_index_max_value;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_dbal_fabric_transmit_wfq_index_max_value_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_dtq_fc_thresholds_bits_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_dbal_fabric_transmit_dtq_fc_thresholds_bits_nof_set;
    data_index = dnx_data_fabric_dbal_define_fabric_transmit_pdq_size_bits_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_fabric_dbal_fabric_transmit_pdq_size_bits_nof_set;

    

    
    
    submodule_index = dnx_data_fabric_submodule_transmit;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_fabric_transmit_eir_fc_leaky_backet;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_fabric_transmit_eir_fc_leaky_backet_set;

    
    data_index = dnx_data_fabric_transmit_table_pdq_to_dqcq_fc_th_select;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_transmit_pdq_to_dqcq_fc_th_select_set;
    data_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_set;
    data_index = dnx_data_fabric_transmit_table_eir_fc_leaky_bucket_delta_select;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_transmit_eir_fc_leaky_bucket_delta_select_set;
    data_index = dnx_data_fabric_transmit_table_dtq_fc_local_thresholds;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_fabric_transmit_dtq_fc_local_thresholds_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


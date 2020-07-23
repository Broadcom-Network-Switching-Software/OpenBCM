

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







static shr_error_e
q2a_b0_dnx_data_nif_flexe_is_led_supported_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_is_led_supported;
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
q2a_b0_dnx_data_nif_flexe_prd_is_bypassed_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_prd_is_bypassed;
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
q2a_b0_dnx_data_nif_flexe_double_slots_for_small_client_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_double_slots_for_small_client;
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
q2a_b0_dnx_data_nif_flexe_double_slots_for_all_client_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_double_slots_for_all_client;
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
q2a_b0_dnx_data_nif_flexe_half_capability_set(
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
q2a_b0_dnx_data_nif_flexe_rmc_cal_per_priority_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_rmc_cal_per_priority;
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
q2a_b0_dnx_data_nif_flexe_pcs_port_enable_support_set(
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
q2a_b0_dnx_data_nif_flexe_rx_valid_in_lf_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_rx_valid_in_lf;
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
q2a_b0_dnx_data_nif_flexe_l1_mismatch_rate_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_l1_mismatch_rate_support;
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
q2a_b0_dnx_data_nif_flexe_disable_l1_mismatch_rate_checking_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_disable_l1_mismatch_rate_checking;
    SHR_FUNC_INIT_VARS(unit);

    feature = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].features[feature_index];
    
    feature->default_data = 0;

    
    feature->data = 0;

    
    feature->flags |= DNXC_DATA_F_SUPPORTED;
    
    feature->property.name = spn_CUSTOM_FEATURE;
    feature->property.doc = 
        "\n"
        "1: Do not check the L1 rate when configuring L1 switching between Busb and ETH.\n"
        "0: Check L1 rate when configuring L1 switching between Busb and ETH.\n"
        "\n"
    ;
    feature->property.method = dnxc_data_property_method_suffix_enable;
    feature->property.method_str = "suffix_enable";
    feature->property.suffix = "flexe_disable_l1_mismatch_rate_checking";
    feature->property.label = DNXC_DATA_LABEL_FLEXE;
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &feature->property, -1, &feature->data));
    
    feature->flags |= (DNXC_DATA_F_PROPERTY);


    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_feature_value_set(unit, feature));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_nif_flexe_feu_ilkn_burst_size_support_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int feature_index = dnx_data_nif_flexe_feu_ilkn_burst_size_support;
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
q2a_b0_dnx_data_nif_flexe_nof_sb_rx_fifos_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nof_sb_rx_fifos;
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
q2a_b0_dnx_data_nif_flexe_priority_groups_nof_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_priority_groups_nof;
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
q2a_b0_dnx_data_nif_flexe_rmc_cal_nof_slots_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_rmc_cal_nof_slots;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 82;

    
    define->data = 82;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_nif_flexe_ilkn_burst_size_in_feu_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_ilkn_burst_size_in_feu;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 244;

    
    define->data = 244;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_b0_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int define_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
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
q2a_b0_dnx_data_nif_flexe_phy_info_set(
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
        data->flexe_core_port[0] = 0;
        data->flexe_core_port[1] = 1;
        data->flexe_core_port[2] = 2;
        data->flexe_core_port[3] = 3;
        data->flexe_core_port[4] = 4;
        data->flexe_core_port[5] = 5;
        data->flexe_core_port[6] = 6;
        data->flexe_core_port[7] = 7;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->speed = 100000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 254;
        SHR_RANGE_VERIFY(4, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_PCS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flexe_core_port[0] = 0;
        data->flexe_core_port[1] = 2;
        data->flexe_core_port[2] = 4;
        data->flexe_core_port[3] = 6;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_phy_info_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->speed = 200000;
        data->min_logical_phy_id = 1;
        data->max_logical_phy_id = 126;
        SHR_RANGE_VERIFY(2, 0, DNX_DATA_MAX_NIF_FLEXE_NOF_PCS, _SHR_E_INTERNAL, "out of bound access to array")
        data->flexe_core_port[0] = 0;
        data->flexe_core_port[1] = 4;
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
q2a_b0_dnx_data_nif_flexe_l1_mismatch_rate_table_set(
    int unit)
{
    int idx_index;
    dnx_data_nif_flexe_l1_mismatch_rate_table_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_nif;
    int submodule_index = dnx_data_nif_submodule_flexe;
    int table_index = dnx_data_nif_flexe_table_l1_mismatch_rate_table;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 3;
    table->info_get.key_size[0] = 3;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_nif_flexe_l1_mismatch_rate_table_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_nif_flexe_table_l1_mismatch_rate_table");

    
    default_data = (dnx_data_nif_flexe_l1_mismatch_rate_table_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->eth_speed = 0;
    default_data->client_speed = 0;
    default_data->tx_thr = 0;
    
    for (idx_index = 0; idx_index < table->keys[0].size; idx_index++)
    {
        data = (dnx_data_nif_flexe_l1_mismatch_rate_table_t *) dnxc_data_mgmt_table_data_get(unit, table, idx_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_l1_mismatch_rate_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->eth_speed = 1000;
        data->client_speed = 5000;
        data->tx_thr = 120;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_l1_mismatch_rate_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->eth_speed = 10000;
        data->client_speed = 5000;
        data->tx_thr = 120;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_nif_flexe_l1_mismatch_rate_table_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->eth_speed = 25000;
        data->client_speed = 5000;
        data->tx_thr = 120;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_b0_data_nif_attach(
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
    
    submodule_index = dnx_data_nif_submodule_flexe;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_nif_flexe_define_nof_sb_rx_fifos;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_nif_flexe_nof_sb_rx_fifos_set;
    data_index = dnx_data_nif_flexe_define_priority_groups_nof;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_nif_flexe_priority_groups_nof_set;
    data_index = dnx_data_nif_flexe_define_rmc_cal_nof_slots;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_nif_flexe_rmc_cal_nof_slots_set;
    data_index = dnx_data_nif_flexe_define_ilkn_burst_size_in_feu;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_nif_flexe_ilkn_burst_size_in_feu_set;
    data_index = dnx_data_nif_flexe_define_nb_tdm_slot_allocation_mode;
    define = &submodule->defines[data_index];
    define->set = q2a_b0_dnx_data_nif_flexe_nb_tdm_slot_allocation_mode_set;

    
    data_index = dnx_data_nif_flexe_is_led_supported;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_is_led_supported_set;
    data_index = dnx_data_nif_flexe_prd_is_bypassed;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_prd_is_bypassed_set;
    data_index = dnx_data_nif_flexe_double_slots_for_small_client;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_double_slots_for_small_client_set;
    data_index = dnx_data_nif_flexe_double_slots_for_all_client;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_double_slots_for_all_client_set;
    data_index = dnx_data_nif_flexe_half_capability;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_half_capability_set;
    data_index = dnx_data_nif_flexe_rmc_cal_per_priority;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_rmc_cal_per_priority_set;
    data_index = dnx_data_nif_flexe_pcs_port_enable_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_pcs_port_enable_support_set;
    data_index = dnx_data_nif_flexe_rx_valid_in_lf;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_rx_valid_in_lf_set;
    data_index = dnx_data_nif_flexe_l1_mismatch_rate_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_l1_mismatch_rate_support_set;
    data_index = dnx_data_nif_flexe_disable_l1_mismatch_rate_checking;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_disable_l1_mismatch_rate_checking_set;
    data_index = dnx_data_nif_flexe_feu_ilkn_burst_size_support;
    feature = &submodule->features[data_index];
    feature->set = q2a_b0_dnx_data_nif_flexe_feu_ilkn_burst_size_support_set;

    
    data_index = dnx_data_nif_flexe_table_phy_info;
    table = &submodule->tables[data_index];
    table->set = q2a_b0_dnx_data_nif_flexe_phy_info_set;
    data_index = dnx_data_nif_flexe_table_l1_mismatch_rate_table;
    table = &submodule->tables[data_index];
    table->set = q2a_b0_dnx_data_nif_flexe_l1_mismatch_rate_table_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


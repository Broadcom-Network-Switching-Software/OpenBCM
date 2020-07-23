

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L2

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>












static shr_error_e
j2c_a0_dnx_data_l2_feature_age_out_and_refresh_profile_selection_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_out_and_refresh_profile_selection;
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
j2c_a0_dnx_data_l2_feature_age_machine_pause_after_flush_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_machine_pause_after_flush;
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
j2c_a0_dnx_data_l2_feature_age_state_not_updated_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_age_state_not_updated;
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
j2c_a0_dnx_data_l2_feature_eth_qual_is_mc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_eth_qual_is_mc;
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
j2c_a0_dnx_data_l2_feature_bc_same_as_unknown_mc_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_bc_same_as_unknown_mc;
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
j2c_a0_dnx_data_l2_feature_wrong_limit_interrupt_handling_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_wrong_limit_interrupt_handling;
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
j2c_a0_dnx_data_l2_feature_transplant_instead_of_refresh_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_transplant_instead_of_refresh;
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
j2c_a0_dnx_data_l2_feature_static_mac_age_out_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_static_mac_age_out;
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
j2c_a0_dnx_data_l2_feature_vmv_for_limit_in_wrong_location_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_vmv_for_limit_in_wrong_location;
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
j2c_a0_dnx_data_l2_feature_learn_limit_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_learn_limit;
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
j2c_a0_dnx_data_l2_feature_learn_events_wrong_command_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_learn_events_wrong_command;
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
j2c_a0_dnx_data_l2_feature_ignore_limit_check_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_feature;
    int feature_index = dnx_data_l2_feature_ignore_limit_check;
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
j2c_a0_dnx_data_l2_vlan_domain_nof_vlan_domains_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vlan_domain;
    int define_index = dnx_data_l2_vlan_domain_define_nof_vlan_domains;
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
j2c_a0_dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_vlan_domain;
    int define_index = dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain;
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
j2c_a0_dnx_data_l2_age_and_flush_machine_filter_rules_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_filter_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_filter_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_19000m";
    table->values[1].default_val = "ITEM_0_0f";
    table->values[2].default_val = "ITEM_8_264f";
    table->values[3].default_val = "ITEM_275_531f";
    table->values[4].default_val = "ITEM_265_267f";
    table->values[5].default_val = "ITEM_532_534f";
    table->values[6].default_val = "ITEM_2_7f";
    table->values[7].default_val = "ITEM_269_274f";
    table->values[8].default_val = "ITEM_1_1f";
    table->values[9].default_val = "ITEM_268_268f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_filter_rules_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_filter_rules");

    
    default_data = (dnx_data_l2_age_and_flush_machine_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_19000m;
    default_data->valid = ITEM_0_0f;
    default_data->entry_mask = ITEM_8_264f;
    default_data->entry_filter = ITEM_275_531f;
    default_data->src_mask = ITEM_265_267f;
    default_data->src_filter = ITEM_532_534f;
    default_data->appdb_id_mask = ITEM_2_7f;
    default_data->appdb_id_filter = ITEM_269_274f;
    default_data->accessed_mask = ITEM_1_1f;
    default_data->accessed_filter = ITEM_268_268f;
    
    data = (dnx_data_l2_age_and_flush_machine_filter_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_l2_age_and_flush_machine_data_rules_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_data_rules_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_data_rules;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_MEM_20000m";
    table->values[1].default_val = "ITEM_0_3f";
    table->values[2].default_val = "ITEM_4_99f";
    table->values[3].default_val = "ITEM_100_195f";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_data_rules_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_data_rules");

    
    default_data = (dnx_data_l2_age_and_flush_machine_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->table_name = MDB_MEM_20000m;
    default_data->command = ITEM_0_3f;
    default_data->payload_mask = ITEM_4_99f;
    default_data->payload = ITEM_100_195f;
    
    data = (dnx_data_l2_age_and_flush_machine_data_rules_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_l2_age_and_flush_machine_flush_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_flush_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_flush;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_REG_301Dr";
    table->values[1].default_val = "50";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_flush_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_flush");

    
    default_data = (dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->flush_pulse = MDB_REG_301Dr;
    default_data->traverse_thread_priority = 50;
    
    data = (dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);
    
    table->values[1].property.name = spn_L2_FLUSH_TRAVERSE_THREAD_PRIORITY;
    table->values[1].property.doc =
        "L2 flush match traverse non-blocking thread priority\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_range;
    table->values[1].property.method_str = "range";
    table->values[1].property.range_min = 0;
    table->values[1].property.range_max = 0xFFFF;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    data = (dnx_data_l2_age_and_flush_machine_flush_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, -1, &data->traverse_thread_priority));
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2c_a0_dnx_data_l2_age_and_flush_machine_age_set(
    int unit)
{
    dnx_data_l2_age_and_flush_machine_age_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_l2;
    int submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    int table_index = dnx_data_l2_age_and_flush_machine_table_age;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->values[0].default_val = "MDB_REG_185r";
    table->values[1].default_val = "ITEM_136_143f";
    table->values[2].default_val = "MDB_REG_301Cr";
    
    DNXC_DATA_ALLOC(table->data, dnx_data_l2_age_and_flush_machine_age_t, (1 + 1  ), "data of dnx_data_l2_age_and_flush_machine_table_age");

    
    default_data = (dnx_data_l2_age_and_flush_machine_age_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->age_config = MDB_REG_185r;
    default_data->disable_aging = ITEM_136_143f;
    default_data->scan_pulse = MDB_REG_301Cr;
    
    data = (dnx_data_l2_age_and_flush_machine_age_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
    sal_memcpy(data, default_data, table->size_of_values);

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2c_a0_data_l2_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_l2;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_l2_submodule_general;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_l2_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_l2_feature_age_out_and_refresh_profile_selection;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_age_out_and_refresh_profile_selection_set;
    data_index = dnx_data_l2_feature_age_machine_pause_after_flush;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_age_machine_pause_after_flush_set;
    data_index = dnx_data_l2_feature_age_state_not_updated;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_age_state_not_updated_set;
    data_index = dnx_data_l2_feature_eth_qual_is_mc;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_eth_qual_is_mc_set;
    data_index = dnx_data_l2_feature_bc_same_as_unknown_mc;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_bc_same_as_unknown_mc_set;
    data_index = dnx_data_l2_feature_wrong_limit_interrupt_handling;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_wrong_limit_interrupt_handling_set;
    data_index = dnx_data_l2_feature_transplant_instead_of_refresh;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_transplant_instead_of_refresh_set;
    data_index = dnx_data_l2_feature_static_mac_age_out;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_static_mac_age_out_set;
    data_index = dnx_data_l2_feature_vmv_for_limit_in_wrong_location;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_vmv_for_limit_in_wrong_location_set;
    data_index = dnx_data_l2_feature_learn_limit;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_learn_limit_set;
    data_index = dnx_data_l2_feature_learn_events_wrong_command;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_learn_events_wrong_command_set;
    data_index = dnx_data_l2_feature_ignore_limit_check;
    feature = &submodule->features[data_index];
    feature->set = j2c_a0_dnx_data_l2_feature_ignore_limit_check_set;

    
    
    submodule_index = dnx_data_l2_submodule_vsi;
    submodule = &module->submodules[submodule_index];

    

    

    
    
    submodule_index = dnx_data_l2_submodule_vlan_domain;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_l2_vlan_domain_define_nof_vlan_domains;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_l2_vlan_domain_nof_vlan_domains_set;
    data_index = dnx_data_l2_vlan_domain_define_nof_bits_next_layer_network_domain;
    define = &submodule->defines[data_index];
    define->set = j2c_a0_dnx_data_l2_vlan_domain_nof_bits_next_layer_network_domain_set;

    

    
    
    submodule_index = dnx_data_l2_submodule_age_and_flush_machine;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_l2_age_and_flush_machine_table_filter_rules;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_l2_age_and_flush_machine_filter_rules_set;
    data_index = dnx_data_l2_age_and_flush_machine_table_data_rules;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_l2_age_and_flush_machine_data_rules_set;
    data_index = dnx_data_l2_age_and_flush_machine_table_flush;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_l2_age_and_flush_machine_flush_set;
    data_index = dnx_data_l2_age_and_flush_machine_table_age;
    table = &submodule->tables[data_index];
    table->set = j2c_a0_dnx_data_l2_age_and_flush_machine_age_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


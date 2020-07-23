

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCH

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <bcm/switch.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>








static shr_error_e
jr2_a0_dnx_data_switch_load_balancing_nof_lb_clients_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_nof_lb_clients;
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
jr2_a0_dnx_data_switch_load_balancing_nof_lb_crc_sel_tcam_entries_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries;
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
jr2_a0_dnx_data_switch_load_balancing_nof_crc_functions_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_nof_crc_functions;
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
jr2_a0_dnx_data_switch_load_balancing_initial_reserved_label_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_initial_reserved_label;
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
jr2_a0_dnx_data_switch_load_balancing_initial_reserved_label_force_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_initial_reserved_label_force;
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
jr2_a0_dnx_data_switch_load_balancing_initial_reserved_next_label_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_initial_reserved_next_label;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFFFF;

    
    define->data = 0xFFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_switch_load_balancing_initial_reserved_next_label_valid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0xFFFF;

    
    define->data = 0xFFFF;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_switch_load_balancing_reserved_next_label_valid_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_reserved_next_label_valid;
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
jr2_a0_dnx_data_switch_load_balancing_num_valid_mpls_protocols_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_num_valid_mpls_protocols;
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
jr2_a0_dnx_data_switch_load_balancing_nof_layer_records_from_parser_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_nof_layer_records_from_parser;
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
jr2_a0_dnx_data_switch_load_balancing_nof_seeds_per_crc_function_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0x10000;

    
    define->data = 0x10000;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_switch_load_balancing_hashing_selection_per_layer_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_hashing_selection_per_layer;
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
jr2_a0_dnx_data_switch_load_balancing_mpls_split_stack_config_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_mpls_split_stack_config;
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
jr2_a0_dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int define_index = dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists;
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
jr2_a0_dnx_data_switch_load_balancing_lb_clients_set(
    int unit)
{
    int client_hw_id_index;
    dnx_data_switch_load_balancing_lb_clients_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int table_index = dnx_data_switch_load_balancing_table_lb_clients;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    table->info_get.key_size[0] = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);

    
    table->values[0].default_val = "SAL_UINT32_MAX";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_switch_load_balancing_lb_clients_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_switch_load_balancing_table_lb_clients");

    
    default_data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->client_logical_id = SAL_UINT32_MAX;
    
    for (client_hw_id_index = 0; client_hw_id_index < table->keys[0].size; client_hw_id_index++)
    {
        data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, client_hw_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->client_logical_id = bcmSwitchECMPHashConfig;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->client_logical_id = bcmSwitchECMPSecondHierHashConfig;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->client_logical_id = bcmSwitchECMPThirdHierHashConfig;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->client_logical_id = bcmSwitchTrunkHashConfig;
    }
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_clients_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->client_logical_id = bcmSwitchNwkHashConfig;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_switch_load_balancing_lb_client_crc_set(
    int unit)
{
    int client_hw_id_index;
    dnx_data_switch_load_balancing_lb_client_crc_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int table_index = dnx_data_switch_load_balancing_table_lb_client_crc;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
    table->info_get.key_size[0] = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);

    
    table->values[0].default_val = "SAL_UINT32_MAX";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_switch_load_balancing_lb_client_crc_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_switch_load_balancing_table_lb_client_crc");

    
    default_data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->crc_hash_func = SAL_UINT32_MAX;
    
    for (client_hw_id_index = 0; client_hw_id_index < table->keys[0].size; client_hw_id_index++)
    {
        data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, client_hw_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->crc_hash_func = BCM_HASH_CONFIG_CRC16_0x10039;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->crc_hash_func = BCM_HASH_CONFIG_CRC16_0x100d7;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->crc_hash_func = BCM_HASH_CONFIG_CRC16_0x1015d;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->crc_hash_func = BCM_HASH_CONFIG_CRC16_0x10939;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_client_crc_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->crc_hash_func = BCM_HASH_CONFIG_CRC16_0x109e7;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_switch_load_balancing_lb_field_enablers_set(
    int unit)
{
    int header_id_index;
    dnx_data_switch_load_balancing_lb_field_enablers_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_load_balancing;
    int table_index = dnx_data_switch_load_balancing_table_lb_field_enablers;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES;
    table->info_get.key_size[0] = DBAL_NOF_ENUM_HASH_FIELD_ENABLERS_HEADER_VALUES;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_switch_load_balancing_lb_field_enablers_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_switch_load_balancing_table_lb_field_enablers");

    
    default_data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->field_enablers_mask = 0;
    
    for (header_id_index = 0; header_id_index < table->keys[0].size; header_id_index++)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, header_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP4_OUTER < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP4_OUTER, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_PROTOCOL|BCM_HASH_FIELD_IPV4_ADDRESS;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP4_INNER < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP4_INNER, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_PROTOCOL|BCM_HASH_FIELD_IPV4_ADDRESS;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_L4 < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_L4, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_L4;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_ETH_OUTER < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_ETH_OUTER, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_ETHER_TYPE|BCM_HASH_FIELD_MAC_ADDRESS|BCM_HASH_FIELD_VLAN;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_ETH_INNER < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_ETH_INNER, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_ETHER_TYPE|BCM_HASH_FIELD_MAC_ADDRESS|BCM_HASH_FIELD_VLAN;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP6_OUTER < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP6_OUTER, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_IPV6_ADDRESS|BCM_HASH_FIELD_FLOW_LABEL|BCM_HASH_FIELD_NXT_HDR;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP6_INNER < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_IP6_INNER, 0);
        data->field_enablers_mask = BCM_HASH_FIELD_IPV6_ADDRESS|BCM_HASH_FIELD_FLOW_LABEL|BCM_HASH_FIELD_NXT_HDR;
    }
    if (DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_MPLS < table->keys[0].size)
    {
        data = (dnx_data_switch_load_balancing_lb_field_enablers_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_ENUM_FVAL_HASH_FIELD_ENABLERS_HEADER_MPLS, 0);
        data->field_enablers_mask = BCM_HASH_MPLS_ALL_LABELS;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_switch_svtag_supported_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_svtag;
    int define_index = dnx_data_switch_svtag_define_supported;
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
jr2_a0_dnx_data_switch_feature_mpls_labels_included_in_hash_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_feature;
    int feature_index = dnx_data_switch_feature_mpls_labels_included_in_hash;
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
jr2_a0_dnx_data_switch_feature_excluded_header_bits_from_hash_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_switch;
    int submodule_index = dnx_data_switch_submodule_feature;
    int feature_index = dnx_data_switch_feature_excluded_header_bits_from_hash;
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
jr2_a0_data_switch_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_switch;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_switch_submodule_load_balancing;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_switch_load_balancing_define_nof_lb_clients;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_nof_lb_clients_set;
    data_index = dnx_data_switch_load_balancing_define_nof_lb_crc_sel_tcam_entries;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_nof_lb_crc_sel_tcam_entries_set;
    data_index = dnx_data_switch_load_balancing_define_nof_crc_functions;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_nof_crc_functions_set;
    data_index = dnx_data_switch_load_balancing_define_initial_reserved_label;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_initial_reserved_label_set;
    data_index = dnx_data_switch_load_balancing_define_initial_reserved_label_force;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_initial_reserved_label_force_set;
    data_index = dnx_data_switch_load_balancing_define_initial_reserved_next_label;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_initial_reserved_next_label_set;
    data_index = dnx_data_switch_load_balancing_define_initial_reserved_next_label_valid;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_initial_reserved_next_label_valid_set;
    data_index = dnx_data_switch_load_balancing_define_reserved_next_label_valid;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_reserved_next_label_valid_set;
    data_index = dnx_data_switch_load_balancing_define_num_valid_mpls_protocols;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_num_valid_mpls_protocols_set;
    data_index = dnx_data_switch_load_balancing_define_nof_layer_records_from_parser;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_nof_layer_records_from_parser_set;
    data_index = dnx_data_switch_load_balancing_define_nof_seeds_per_crc_function;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_nof_seeds_per_crc_function_set;
    data_index = dnx_data_switch_load_balancing_define_hashing_selection_per_layer;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_hashing_selection_per_layer_set;
    data_index = dnx_data_switch_load_balancing_define_mpls_split_stack_config;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_mpls_split_stack_config_set;
    data_index = dnx_data_switch_load_balancing_define_mpls_cam_next_label_valid_field_exists;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_load_balancing_mpls_cam_next_label_valid_field_exists_set;

    

    
    data_index = dnx_data_switch_load_balancing_table_lb_clients;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_switch_load_balancing_lb_clients_set;
    data_index = dnx_data_switch_load_balancing_table_lb_client_crc;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_switch_load_balancing_lb_client_crc_set;
    data_index = dnx_data_switch_load_balancing_table_lb_field_enablers;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_switch_load_balancing_lb_field_enablers_set;
    
    submodule_index = dnx_data_switch_submodule_svtag;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_switch_svtag_define_supported;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_switch_svtag_supported_set;

    

    
    
    submodule_index = dnx_data_switch_submodule_feature;
    submodule = &module->submodules[submodule_index];

    

    
    data_index = dnx_data_switch_feature_mpls_labels_included_in_hash;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_switch_feature_mpls_labels_included_in_hash_set;
    data_index = dnx_data_switch_feature_excluded_header_bits_from_hash;
    feature = &submodule->features[data_index];
    feature->set = jr2_a0_dnx_data_switch_feature_excluded_header_bits_from_hash_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


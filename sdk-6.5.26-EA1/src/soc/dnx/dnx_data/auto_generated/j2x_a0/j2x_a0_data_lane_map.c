
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_lane_map.h>
#include <bcm/port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>









static shr_error_e
j2x_a0_dnx_data_lane_map_otn_logical_lanes_set(
    int unit)
{
    int port_index;
    dnx_data_lane_map_otn_logical_lanes_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = dnx_data_lane_map_submodule_otn;
    int table_index = dnx_data_lane_map_otn_table_logical_lanes;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_lane_map_otn_logical_lanes_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_lane_map_otn_table_logical_lanes");

    
    default_data = (dnx_data_lane_map_otn_logical_lanes_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    _SHR_PBMP_WORD_SET(default_data->val, 0, 0);
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_lane_map_otn_logical_lanes_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = "otn_lanes";
    table->values[0].property.doc =
        "\n"
        "Specify the front panel logical lane bitmap for OTN interface.\n"
        "otn_lanes<port> = #bitmap#\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_custom;
    table->values[0].property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_lane_map_otn_logical_lanes_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnx_data_property_lane_map_otn_logical_lanes_val_read(unit, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_lane_map_otn_logical_otn_lane_set(
    int unit)
{
    int port_index;
    dnx_data_lane_map_otn_logical_otn_lane_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = dnx_data_lane_map_submodule_otn;
    int table_index = dnx_data_lane_map_otn_table_logical_otn_lane;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "BCM_PORT_RESOURCE_DEFAULT_REQUEST";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_lane_map_otn_logical_otn_lane_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_lane_map_otn_table_logical_otn_lane");

    
    default_data = (dnx_data_lane_map_otn_logical_otn_lane_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->val = BCM_PORT_RESOURCE_DEFAULT_REQUEST;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_lane_map_otn_logical_otn_lane_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_PORT_INIT_LOGICAL_OTN_LANE;
    table->values[0].property.doc =
        "\n"
        "Specify the first framer logical lane for OTN interface.\n"
        "port_init_logical_otn_lane_<port> = #logical_otn_lane#\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_suffix_range_signed;
    table->values[0].property.method_str = "suffix_range_signed";
    table->values[0].property.suffix = "";
    table->values[0].property.range_min = -1;
    table->values[0].property.range_max = 63;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_lane_map_otn_logical_otn_lane_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->val));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
j2x_a0_data_lane_map_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_lane_map;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_lane_map_submodule_otn;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_lane_map_otn_table_logical_lanes;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_lane_map_otn_logical_lanes_set;
    data_index = dnx_data_lane_map_otn_table_logical_otn_lane;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_lane_map_otn_logical_otn_lane_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


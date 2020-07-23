

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_property.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>









static shr_error_e
q2a_a0_dnx_data_port_static_add_fabric_quad_info_set(
    int unit)
{
    int quad_index;
    dnx_data_port_static_add_fabric_quad_info_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_fabric_quad_info;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->flags |= DNXC_DATA_F_INIT_ONLY;

    
    table->keys[0].size = dnx_data_fabric.links.nof_links_get(unit)/4;
    table->info_get.key_size[0] = dnx_data_fabric.links.nof_links_get(unit)/4;

    
    table->values[0].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_fabric_quad_info_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_fabric_quad_info");

    
    default_data = (dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->quad_enable = 0;
    
    for (quad_index = 0; quad_index < table->keys[0].size; quad_index++)
    {
        data = (dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, quad_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_SERDES_QRTT_ACTIVE;
    table->values[0].property.doc =
        "\n"
        "For Backward compatability with other devices\n"
        "leave value of 0(disable) to be supported.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_direct_map;
    table->values[0].property.method_str = "port_direct_map";
    table->values[0].property.nof_mapping = 1;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_fabric_quad_info_t property mapping");

    table->values[0].property.mapping[0].name = "0";
    table->values[0].property.mapping[0].val = 0;
    table->values[0].property.mapping[0].is_default = 1 ;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (quad_index = 0; quad_index < table->keys[0].size; quad_index++)
    {
        data = (dnx_data_port_static_add_fabric_quad_info_t *) dnxc_data_mgmt_table_data_get(unit, table, quad_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, quad_index, &data->quad_enable));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_port_static_add_header_type_set(
    int unit)
{
    int port_index;
    dnx_data_port_static_add_header_type_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_static_add;
    int table_index = dnx_data_port_static_add_table_header_type;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = dnx_data_port.general.fabric_port_base_get(unit);
    table->info_get.key_size[0] = dnx_data_port.general.fabric_port_base_get(unit);

    
    table->values[0].default_val = "BCM_SWITCH_PORT_HEADER_TYPE_ETH";
    table->values[1].default_val = "BCM_SWITCH_PORT_HEADER_TYPE_ETH";
    table->values[2].default_val = "BCM_SWITCH_PORT_HEADER_TYPE_ETH";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_static_add_header_type_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_static_add_table_header_type");

    
    default_data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->header_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    default_data->header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    default_data->header_type = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    table->values[0].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[0].property.doc =
        "\n"
        "Header type per incoming port.\n"
        "\n"
    ;
    table->values[0].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[0].property.method_str = "port_suffix_direct_map";
    table->values[0].property.suffix = "in";
    table->values[0].property.nof_mapping = 10;
    DNXC_DATA_ALLOC(table->values[0].property.mapping, dnxc_data_property_map_t, table->values[0].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[0].property.mapping[0].name = "ETH";
    table->values[0].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[0].property.mapping[0].is_default = 1 ;
    table->values[0].property.mapping[1].name = "RAW";
    table->values[0].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[0].property.mapping[2].name = "INJECTED";
    table->values[0].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED;
    table->values[0].property.mapping[3].name = "INJECTED_2_PP";
    table->values[0].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP;
    table->values[0].property.mapping[4].name = "INJECTED_2";
    table->values[0].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2;
    table->values[0].property.mapping[5].name = "TM";
    table->values[0].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_TM;
    table->values[0].property.mapping[6].name = "MPLS_RAW";
    table->values[0].property.mapping[6].val = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
    table->values[0].property.mapping[7].name = "RCH_0";
    table->values[0].property.mapping[7].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_0;
    table->values[0].property.mapping[8].name = "RCH_1";
    table->values[0].property.mapping[8].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_1;
    table->values[0].property.mapping[9].name = "STACKING";
    table->values[0].property.mapping[9].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    
    table->values[1].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[1].property.doc =
        "\n"
        "Header type per outgoing port.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[1].property.method_str = "port_suffix_direct_map";
    table->values[1].property.suffix = "out";
    table->values[1].property.nof_mapping = 6;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[1].property.mapping[0].name = "ETH";
    table->values[1].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[1].property.mapping[0].is_default = 1 ;
    table->values[1].property.mapping[1].name = "RAW";
    table->values[1].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[1].property.mapping[2].name = "CPU";
    table->values[1].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_CPU;
    table->values[1].property.mapping[3].name = "ENCAP_EXTERNAL_CPU";
    table->values[1].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU;
    table->values[1].property.mapping[4].name = "INJECTED";
    table->values[1].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED;
    table->values[1].property.mapping[5].name = "STACKING";
    table->values[1].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    
    table->values[2].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[2].property.doc =
        "\n"
        "Header type per port.\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_port_direct_map;
    table->values[2].property.method_str = "port_direct_map";
    table->values[2].property.nof_mapping = 4;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[2].property.mapping[0].name = "ETH";
    table->values[2].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[2].property.mapping[0].is_default = 1 ;
    table->values[2].property.mapping[1].name = "RAW";
    table->values[2].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[2].property.mapping[2].name = "MPLS_RAW";
    table->values[2].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
    table->values[2].property.mapping[3].name = "STACKING";
    table->values[2].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));

    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[0].property, port_index, &data->header_type_in));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[1].property, port_index, &data->header_type_out));
    }
    for (port_index = 0; port_index < table->keys[0].size; port_index++)
    {
        data = (dnx_data_port_static_add_header_type_t *) dnxc_data_mgmt_table_data_get(unit, table, port_index, 0);
        SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &table->values[2].property, port_index, &data->header_type));
    }
    
    table->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}





static shr_error_e
q2a_a0_dnx_data_port_general_unrestricted_portmod_pll_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int feature_index = dnx_data_port_general_unrestricted_portmod_pll;
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
q2a_a0_dnx_data_port_general_ingress_vlan_domain_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ingress_vlan_domain_size;
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
q2a_a0_dnx_data_port_general_vlan_domain_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_vlan_domain_size;
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
q2a_a0_dnx_data_port_general_ffc_instruction_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ffc_instruction_size;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 21;

    
    define->data = 21;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_port_general_prt_ffc_width_start_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_ffc_width_start_offset;
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
q2a_a0_dnx_data_port_general_prt_ffc_start_input_offset_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_prt_ffc_start_input_offset;
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
q2a_a0_dnx_data_port_general_reserved_tm_port_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_reserved_tm_port;
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
q2a_a0_dnx_data_port_general_fabric_port_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_fabric_port_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = SOC_MAX_NUM_PORTS-dnx_data_fabric.links.nof_links_get(unit);

    
    define->data = SOC_MAX_NUM_PORTS-dnx_data_fabric.links.nof_links_get(unit);

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "fabric_logical_port_base";
    define->property.doc = 
        "\n"
        "Even so that Q2A doesn't have fabric\n"
        "data is left because it is used for other logical port calculations\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_custom;
    define->property.method_str = "custom";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnx_data_property_port_general_fabric_port_base_read(unit, (uint32 *) &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
q2a_a0_dnx_data_port_general_pp_port_bits_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_pp_port_bits_size;
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
q2a_a0_dnx_data_port_general_nof_port_bits_in_pp_bus_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_port_bits_in_pp_bus;
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
q2a_a0_dnx_data_port_general_prt_configuration_set(
    int unit)
{
    int index_index;
    dnx_data_port_general_prt_configuration_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int table_index = dnx_data_port_general_table_prt_configuration;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 13;
    table->info_get.key_size[0] = 13;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "0";
    table->values[8].default_val = "0";
    table->values[9].default_val = "0";
    table->values[10].default_val = "0";
    table->values[11].default_val = "0";
    table->values[12].default_val = "0";
    table->values[13].default_val = "0";
    table->values[14].default_val = "0";
    table->values[15].default_val = "0";
    table->values[16].default_val = "0";
    table->values[17].default_val = "0";
    table->values[18].default_val = "0";
    table->values[19].default_val = "0";
    table->values[20].default_val = "0";
    table->values[21].default_val = "0";
    table->values[22].default_val = "0";
    table->values[23].default_val = "0";
    table->values[24].default_val = "0";
    table->values[25].default_val = "0";
    table->values[26].default_val = "0";
    table->values[27].default_val = "0";
    table->values[28].default_val = "0";
    table->values[29].default_val = "0";
    table->values[30].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_port_general_prt_configuration_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_port_general_table_prt_configuration");

    
    default_data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->prt_recycle_profile = 0;
    default_data->port_termination_ptc_profile = 0;
    default_data->prt_qual_mode = 0;
    default_data->layer_offset = 0;
    default_data->context_select = 0;
    default_data->kbr_valid_bitmap_4 = 0;
    default_data->kbr_valid_bitmap_3 = 0;
    default_data->kbr_valid_bitmap_2 = 0;
    default_data->kbr_valid_bitmap_1 = 0;
    default_data->kbr_valid_bitmap_0 = 0;
    default_data->ffc_5_offset = 0;
    default_data->ffc_5_width = 0;
    default_data->ffc_5_input_offset = 0;
    default_data->ffc_4_offset = 0;
    default_data->ffc_4_width = 0;
    default_data->ffc_4_input_offset = 0;
    default_data->ffc_3_offset = 0;
    default_data->ffc_3_width = 0;
    default_data->ffc_3_input_offset = 0;
    default_data->ffc_2_offset = 0;
    default_data->ffc_2_width = 0;
    default_data->ffc_2_input_offset = 0;
    default_data->ffc_1_offset = 0;
    default_data->ffc_1_width = 0;
    default_data->ffc_1_input_offset = 0;
    default_data->ffc_0_offset = 0;
    default_data->ffc_0_width = 0;
    default_data->ffc_0_input_offset = 0;
    default_data->src_sys_port_prt_mode = 0;
    default_data->pp_port_en = 0;
    default_data->tcam_mode_full_en = 0;
    
    for (index_index = 0; index_index < table->keys[0].size; index_index++)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 2;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 0;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH1;
        data->prt_qual_mode = 1;
        data->layer_offset = 3;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 32;
        data->kbr_valid_bitmap_1 = 9;
        data->kbr_valid_bitmap_0 = 16;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 15;
        data->ffc_5_input_offset = 232;
        data->ffc_4_offset = 2;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 248;
        data->ffc_3_offset = 16;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 325;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 2;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 15;
        data->ffc_0_input_offset = 232;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 294;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 310;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 0;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 292;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 2;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE1;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 8;
        data->kbr_valid_bitmap_2 = 4;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 4;
        data->ffc_3_input_offset = 280;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 15;
        data->ffc_2_input_offset = 128;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 7;
        data->ffc_0_input_offset = 308;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 0;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_802_1BR_TYPE2;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ETHERNET;
        data->prt_qual_mode = 0;
        data->layer_offset = 8;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 310;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 0;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 32;
        data->kbr_valid_bitmap_2 = 16;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 4;
        data->ffc_5_input_offset = 320;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 15;
        data->ffc_4_input_offset = 200;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 15;
        data->ffc_0_input_offset = 200;
        data->src_sys_port_prt_mode = 2;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_COE;
        data->prt_qual_mode = 0;
        data->layer_offset = 4;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 11;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 14;
        data->ffc_3_width = 2;
        data->ffc_3_input_offset = 325;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 6;
        data->ffc_1_width = 7;
        data->ffc_1_input_offset = 310;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 5;
        data->ffc_0_input_offset = 224;
        data->src_sys_port_prt_mode = 1;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_PTCH2;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RCH;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 4;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 2;
        data->ffc_3_input_offset = 262;
        data->ffc_2_offset = 32;
        data->ffc_2_width = 7;
        data->ffc_2_input_offset = 224;
        data->ffc_1_offset = 32;
        data->ffc_1_width = 1;
        data->ffc_1_input_offset = 255;
        data->ffc_0_offset = 32;
        data->ffc_0_width = 4;
        data->ffc_0_input_offset = 252;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (9 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 9, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PTCH2_JR1;
        data->prt_qual_mode = 1;
        data->layer_offset = 2;
        data->context_select = 1;
        data->kbr_valid_bitmap_4 = 2;
        data->kbr_valid_bitmap_3 = 12;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 1;
        data->ffc_3_width = 1;
        data->ffc_3_input_offset = 325;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 255;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 3;
        data->ffc_1_input_offset = 252;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 240;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (10 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 10, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_ITMH;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 310;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 1;
    }
    if (11 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 11, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_NONE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_RAW;
        data->prt_qual_mode = 0;
        data->layer_offset = 0;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 1;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 0;
        data->ffc_2_width = 0;
        data->ffc_2_input_offset = 0;
        data->ffc_1_offset = 0;
        data->ffc_1_width = 0;
        data->ffc_1_input_offset = 0;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 9;
        data->ffc_0_input_offset = 310;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 0;
        data->tcam_mode_full_en = 0;
    }
    if (12 < table->keys[0].size)
    {
        data = (dnx_data_port_general_prt_configuration_t *) dnxc_data_mgmt_table_data_get(unit, table, 12, 0);
        data->prt_recycle_profile = DBAL_ENUM_FVAL_RECYCLE_CMD_PRT_PROFILE_COE;
        data->port_termination_ptc_profile = DBAL_ENUM_FVAL_PORT_TERMINATION_PTC_PROFILE_PON;
        data->prt_qual_mode = 0;
        data->layer_offset = 4;
        data->context_select = 0;
        data->kbr_valid_bitmap_4 = 0;
        data->kbr_valid_bitmap_3 = 0;
        data->kbr_valid_bitmap_2 = 0;
        data->kbr_valid_bitmap_1 = 7;
        data->ffc_5_offset = 0;
        data->ffc_5_width = 0;
        data->ffc_5_input_offset = 0;
        data->ffc_4_offset = 0;
        data->ffc_4_width = 0;
        data->ffc_4_input_offset = 0;
        data->ffc_3_offset = 0;
        data->ffc_3_width = 0;
        data->ffc_3_input_offset = 0;
        data->ffc_2_offset = 13;
        data->ffc_2_width = 3;
        data->ffc_2_input_offset = 294;
        data->ffc_1_offset = 3;
        data->ffc_1_width = 9;
        data->ffc_1_input_offset = 310;
        data->ffc_0_offset = 0;
        data->ffc_0_width = 2;
        data->ffc_0_input_offset = 236;
        data->src_sys_port_prt_mode = 0;
        data->pp_port_en = 1;
        data->tcam_mode_full_en = 0;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
q2a_a0_data_port_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_port;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_port_submodule_static_add;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_port_static_add_table_fabric_quad_info;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_port_static_add_fabric_quad_info_set;
    data_index = dnx_data_port_static_add_table_header_type;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_port_static_add_header_type_set;
    
    submodule_index = dnx_data_port_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_port_general_define_fabric_phys_offset;
    define = &submodule->defines[data_index];
    define->set = NULL;
    data_index = dnx_data_port_general_define_ingress_vlan_domain_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_ingress_vlan_domain_size_set;
    data_index = dnx_data_port_general_define_vlan_domain_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_vlan_domain_size_set;
    data_index = dnx_data_port_general_define_ffc_instruction_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_ffc_instruction_size_set;
    data_index = dnx_data_port_general_define_prt_ffc_width_start_offset;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_prt_ffc_width_start_offset_set;
    data_index = dnx_data_port_general_define_prt_ffc_start_input_offset;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_prt_ffc_start_input_offset_set;
    data_index = dnx_data_port_general_define_reserved_tm_port;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_reserved_tm_port_set;
    data_index = dnx_data_port_general_define_fabric_port_base;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_fabric_port_base_set;
    data_index = dnx_data_port_general_define_pp_port_bits_size;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_pp_port_bits_size_set;
    data_index = dnx_data_port_general_define_nof_port_bits_in_pp_bus;
    define = &submodule->defines[data_index];
    define->set = q2a_a0_dnx_data_port_general_nof_port_bits_in_pp_bus_set;

    
    data_index = dnx_data_port_general_unrestricted_portmod_pll;
    feature = &submodule->features[data_index];
    feature->set = q2a_a0_dnx_data_port_general_unrestricted_portmod_pll_set;

    
    data_index = dnx_data_port_general_table_prt_configuration;
    table = &submodule->tables[data_index];
    table->set = q2a_a0_dnx_data_port_general_prt_configuration_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


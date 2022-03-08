
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_port.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>









static shr_error_e
j2x_a0_dnx_data_port_static_add_header_type_set(
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
    table->values[0].property.nof_mapping = 13;
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
    table->values[0].property.mapping[4].name = "INJECTED_2_PP_JR1_MODE";
    table->values[0].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_PP_JR1_MODE;
    table->values[0].property.mapping[5].name = "INJECTED_2";
    table->values[0].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2;
    table->values[0].property.mapping[6].name = "INJECTED_2_JR1_MODE";
    table->values[0].property.mapping[6].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED_2_JR1_MODE;
    table->values[0].property.mapping[7].name = "TM";
    table->values[0].property.mapping[7].val = BCM_SWITCH_PORT_HEADER_TYPE_TM;
    table->values[0].property.mapping[8].name = "MPLS_RAW";
    table->values[0].property.mapping[8].val = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
    table->values[0].property.mapping[9].name = "RCH_0";
    table->values[0].property.mapping[9].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_0;
    table->values[0].property.mapping[10].name = "RCH_1";
    table->values[0].property.mapping[10].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_1;
    table->values[0].property.mapping[11].name = "RCH_SRV6_USP_PSP";
    table->values[0].property.mapping[11].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_SRV6_USP_PSP;
    table->values[0].property.mapping[12].name = "STACKING";
    table->values[0].property.mapping[12].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    
    table->values[1].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[1].property.doc =
        "\n"
        "Header type per outgoing port.\n"
        "\n"
    ;
    table->values[1].property.method = dnxc_data_property_method_port_suffix_direct_map;
    table->values[1].property.method_str = "port_suffix_direct_map";
    table->values[1].property.suffix = "out";
    table->values[1].property.nof_mapping = 10;
    DNXC_DATA_ALLOC(table->values[1].property.mapping, dnxc_data_property_map_t, table->values[1].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[1].property.mapping[0].name = "ETH";
    table->values[1].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[1].property.mapping[0].is_default = 1 ;
    table->values[1].property.mapping[1].name = "RAW";
    table->values[1].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[1].property.mapping[2].name = "RAW_WITH_ORIGINAL_SYSTEM_HEADERS";
    table->values[1].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW_WITH_ORIGINAL_SYSTEM_HEADERS;
    table->values[1].property.mapping[3].name = "CPU";
    table->values[1].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_CPU;
    table->values[1].property.mapping[4].name = "ENCAP_EXTERNAL_CPU";
    table->values[1].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU;
    table->values[1].property.mapping[5].name = "INJECTED";
    table->values[1].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_INJECTED;
    table->values[1].property.mapping[6].name = "STACKING";
    table->values[1].property.mapping[6].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    table->values[1].property.mapping[7].name = "TM";
    table->values[1].property.mapping[7].val = BCM_SWITCH_PORT_HEADER_TYPE_TM;
    table->values[1].property.mapping[8].name = "MPLS_RAW";
    table->values[1].property.mapping[8].val = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
    table->values[1].property.mapping[9].name = "RCH_SRV6_USP_PSP";
    table->values[1].property.mapping[9].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_SRV6_USP_PSP;
    
    table->values[2].property.name = spn_TM_PORT_HEADER_TYPE;
    table->values[2].property.doc =
        "\n"
        "Header type per port.\n"
        "\n"
    ;
    table->values[2].property.method = dnxc_data_property_method_port_direct_map;
    table->values[2].property.method_str = "port_direct_map";
    table->values[2].property.nof_mapping = 6;
    DNXC_DATA_ALLOC(table->values[2].property.mapping, dnxc_data_property_map_t, table->values[2].property.nof_mapping, "dnx_data_port_static_add_header_type_t property mapping");

    table->values[2].property.mapping[0].name = "ETH";
    table->values[2].property.mapping[0].val = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    table->values[2].property.mapping[0].is_default = 1 ;
    table->values[2].property.mapping[1].name = "RAW";
    table->values[2].property.mapping[1].val = BCM_SWITCH_PORT_HEADER_TYPE_RAW;
    table->values[2].property.mapping[2].name = "STACKING";
    table->values[2].property.mapping[2].val = BCM_SWITCH_PORT_HEADER_TYPE_STACKING;
    table->values[2].property.mapping[3].name = "TM";
    table->values[2].property.mapping[3].val = BCM_SWITCH_PORT_HEADER_TYPE_TM;
    table->values[2].property.mapping[4].name = "MPLS_RAW";
    table->values[2].property.mapping[4].val = BCM_SWITCH_PORT_HEADER_TYPE_MPLS_RAW;
    table->values[2].property.mapping[5].name = "RCH_SRV6_USP_PSP";
    table->values[2].property.mapping[5].val = BCM_SWITCH_PORT_HEADER_TYPE_RCH_SRV6_USP_PSP;

    
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
j2x_a0_dnx_data_port_general_unrestricted_portmod_pll_set(
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
j2x_a0_dnx_data_port_general_use_portmod_speed_when_not_enabled_set(
    int unit)
{
    dnxc_data_feature_t *feature;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int feature_index = dnx_data_port_general_use_portmod_speed_when_not_enabled;
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
j2x_a0_dnx_data_port_general_pp_dsp_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_pp_dsp_size;
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
j2x_a0_dnx_data_port_general_ingress_vlan_domain_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ingress_vlan_domain_size;
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
j2x_a0_dnx_data_port_general_vlan_domain_size_set(
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
j2x_a0_dnx_data_port_general_ffc_instruction_size_set(
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
j2x_a0_dnx_data_port_general_ffc_instruction_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ffc_instruction_offset_size;
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
j2x_a0_dnx_data_port_general_ffc_instruction_width_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ffc_instruction_width_size;
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
j2x_a0_dnx_data_port_general_ffc_instruction_input_offset_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_ffc_instruction_input_offset_size;
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
j2x_a0_dnx_data_port_general_prt_ffc_width_start_offset_set(
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
j2x_a0_dnx_data_port_general_prt_ffc_start_input_offset_set(
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
j2x_a0_dnx_data_port_general_nof_ptc_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_ptc;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_nof_out_tm_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_out_tm_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_nof_pp_dsp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_pp_dsp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_nof_pp_ports_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_pp_ports;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_reserved_pp_dsp_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_reserved_pp_dsp;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 510;

    
    define->data = 510;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_nof_vlan_membership_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_vlan_membership_if;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_max_vlan_membership_if_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_max_vlan_membership_if;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 511;

    
    define->data = 511;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
j2x_a0_dnx_data_port_general_fabric_port_base_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_fabric_port_base;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 512;

    
    define->data = 512;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = "fabric_logical_port_base";
    define->property.doc = 
        "\n"
        "By default fabric logical ports are 512..(512 + nof links)\n"
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
j2x_a0_dnx_data_port_general_pp_port_bits_size_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_pp_port_bits_size;
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
j2x_a0_dnx_data_port_general_nof_port_bits_in_pp_bus_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_port;
    int submodule_index = dnx_data_port_submodule_general;
    int define_index = dnx_data_port_general_define_nof_port_bits_in_pp_bus;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 9;

    
    define->data = 9;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));


exit:
    SHR_FUNC_EXIT;
}




shr_error_e
j2x_a0_data_port_attach(
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

    

    

    
    data_index = dnx_data_port_static_add_table_header_type;
    table = &submodule->tables[data_index];
    table->set = j2x_a0_dnx_data_port_static_add_header_type_set;
    
    submodule_index = dnx_data_port_submodule_general;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_port_general_define_pp_dsp_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_pp_dsp_size_set;
    data_index = dnx_data_port_general_define_ingress_vlan_domain_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_ingress_vlan_domain_size_set;
    data_index = dnx_data_port_general_define_vlan_domain_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_vlan_domain_size_set;
    data_index = dnx_data_port_general_define_ffc_instruction_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_ffc_instruction_size_set;
    data_index = dnx_data_port_general_define_ffc_instruction_offset_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_ffc_instruction_offset_size_set;
    data_index = dnx_data_port_general_define_ffc_instruction_width_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_ffc_instruction_width_size_set;
    data_index = dnx_data_port_general_define_ffc_instruction_input_offset_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_ffc_instruction_input_offset_size_set;
    data_index = dnx_data_port_general_define_prt_ffc_width_start_offset;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_prt_ffc_width_start_offset_set;
    data_index = dnx_data_port_general_define_prt_ffc_start_input_offset;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_prt_ffc_start_input_offset_set;
    data_index = dnx_data_port_general_define_nof_ptc;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_nof_ptc_set;
    data_index = dnx_data_port_general_define_nof_out_tm_ports;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_nof_out_tm_ports_set;
    data_index = dnx_data_port_general_define_nof_pp_dsp;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_nof_pp_dsp_set;
    data_index = dnx_data_port_general_define_nof_pp_ports;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_nof_pp_ports_set;
    data_index = dnx_data_port_general_define_reserved_pp_dsp;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_reserved_pp_dsp_set;
    data_index = dnx_data_port_general_define_nof_vlan_membership_if;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_nof_vlan_membership_if_set;
    data_index = dnx_data_port_general_define_max_vlan_membership_if;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_max_vlan_membership_if_set;
    data_index = dnx_data_port_general_define_fabric_port_base;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_fabric_port_base_set;
    data_index = dnx_data_port_general_define_pp_port_bits_size;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_pp_port_bits_size_set;
    data_index = dnx_data_port_general_define_nof_port_bits_in_pp_bus;
    define = &submodule->defines[data_index];
    define->set = j2x_a0_dnx_data_port_general_nof_port_bits_in_pp_bus_set;

    
    data_index = dnx_data_port_general_unrestricted_portmod_pll;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_port_general_unrestricted_portmod_pll_set;
    data_index = dnx_data_port_general_use_portmod_speed_when_not_enabled;
    feature = &submodule->features[data_index];
    feature->set = j2x_a0_dnx_data_port_general_use_portmod_speed_when_not_enabled_set;

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


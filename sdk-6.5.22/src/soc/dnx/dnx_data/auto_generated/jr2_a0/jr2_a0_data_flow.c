
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLOW

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_flow.h>









static shr_error_e
jr2_a0_dnx_data_flow_special_fields_name_mapping_key_map(
    int unit,
    int key0_val,
    int key1_val,
    int *key0_index,
    int *key1_index)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_val)
    {
        case DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER:
            *key0_index = 0;
            break;
        case DBAL_FIELD_C_VID_1:
            *key0_index = 1;
            break;
        case DBAL_FIELD_C_VID_2:
            *key0_index = 2;
            break;
        case DBAL_FIELD_MPLS_LABEL_2:
            *key0_index = 3;
            break;
        case DBAL_FIELD_IPV4_TUNNEL_TYPE:
            *key0_index = 4;
            break;
        case DBAL_FIELD_BITSTR:
            *key0_index = 5;
            break;
        case DBAL_FIELD_IPV4_SIP:
            *key0_index = 6;
            break;
        case DBAL_FIELD_VRF:
            *key0_index = 7;
            break;
        case DBAL_FIELD_VID_INNER_VLAN:
            *key0_index = 8;
            break;
        case DBAL_FIELD_PCP_DEI_OUTER_VLAN:
            *key0_index = 9;
            break;
        case DBAL_FIELD_MAPPED_PP_PORT:
            *key0_index = 10;
            break;
        case DBAL_FIELD_DST_IPV4_MASK:
            *key0_index = 11;
            break;
        case DBAL_FIELD_SRC_IPV6_MASK:
            *key0_index = 12;
            break;
        case DBAL_FIELD_IPV4_DIP:
            *key0_index = 13;
            break;
        case DBAL_FIELD_LAYER_TYPE:
            *key0_index = 14;
            break;
        case DBAL_FIELD_MPLS_LABEL:
            *key0_index = 15;
            break;
        case DBAL_FIELD_PP_PORT:
            *key0_index = 16;
            break;
        case DBAL_FIELD_C_VID:
            *key0_index = 17;
            break;
        case DBAL_FIELD_DST_IPV6_MASK:
            *key0_index = 18;
            break;
        case DBAL_FIELD_VLAN_EDIT_VID_1:
            *key0_index = 19;
            break;
        case DBAL_FIELD_VLAN_EDIT_PROFILE:
            *key0_index = 20;
            break;
        case DBAL_FIELD_S_VID_1:
            *key0_index = 21;
            break;
        case DBAL_FIELD_VID_OUTER_VLAN:
            *key0_index = 22;
            break;
        case DBAL_FIELD_S_VID_2:
            *key0_index = 23;
            break;
        case DBAL_FIELD_S_VID:
            *key0_index = 24;
            break;
        case DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE:
            *key0_index = 25;
            break;
        case DBAL_FIELD_SOURCE_ADDRESS:
            *key0_index = 26;
            break;
        case DBAL_FIELD_VLAN_EDIT_VID_2:
            *key0_index = 27;
            break;
        case DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN:
            *key0_index = 28;
            break;
        case DBAL_FIELD_VLAN_DOMAIN:
            *key0_index = 29;
            break;
        case DBAL_FIELD_SRC_IPV4_MASK:
            *key0_index = 30;
            break;
        case DBAL_FIELD_FODO_ASSIGNMENT_MODE:
            *key0_index = 31;
            break;
        default:
            *key0_index = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_special_fields_name_mapping_key_reverse_map(
    int unit,
    int key0_index,
    int key1_index,
    int *key0_val,
    int *key1_val)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (key0_index)
    {
        case 0:
            *key0_val = DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER;
            break;
        case 1:
            *key0_val = DBAL_FIELD_C_VID_1;
            break;
        case 2:
            *key0_val = DBAL_FIELD_C_VID_2;
            break;
        case 3:
            *key0_val = DBAL_FIELD_MPLS_LABEL_2;
            break;
        case 4:
            *key0_val = DBAL_FIELD_IPV4_TUNNEL_TYPE;
            break;
        case 5:
            *key0_val = DBAL_FIELD_BITSTR;
            break;
        case 6:
            *key0_val = DBAL_FIELD_IPV4_SIP;
            break;
        case 7:
            *key0_val = DBAL_FIELD_VRF;
            break;
        case 8:
            *key0_val = DBAL_FIELD_VID_INNER_VLAN;
            break;
        case 9:
            *key0_val = DBAL_FIELD_PCP_DEI_OUTER_VLAN;
            break;
        case 10:
            *key0_val = DBAL_FIELD_MAPPED_PP_PORT;
            break;
        case 11:
            *key0_val = DBAL_FIELD_DST_IPV4_MASK;
            break;
        case 12:
            *key0_val = DBAL_FIELD_SRC_IPV6_MASK;
            break;
        case 13:
            *key0_val = DBAL_FIELD_IPV4_DIP;
            break;
        case 14:
            *key0_val = DBAL_FIELD_LAYER_TYPE;
            break;
        case 15:
            *key0_val = DBAL_FIELD_MPLS_LABEL;
            break;
        case 16:
            *key0_val = DBAL_FIELD_PP_PORT;
            break;
        case 17:
            *key0_val = DBAL_FIELD_C_VID;
            break;
        case 18:
            *key0_val = DBAL_FIELD_DST_IPV6_MASK;
            break;
        case 19:
            *key0_val = DBAL_FIELD_VLAN_EDIT_VID_1;
            break;
        case 20:
            *key0_val = DBAL_FIELD_VLAN_EDIT_PROFILE;
            break;
        case 21:
            *key0_val = DBAL_FIELD_S_VID_1;
            break;
        case 22:
            *key0_val = DBAL_FIELD_VID_OUTER_VLAN;
            break;
        case 23:
            *key0_val = DBAL_FIELD_S_VID_2;
            break;
        case 24:
            *key0_val = DBAL_FIELD_S_VID;
            break;
        case 25:
            *key0_val = DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE;
            break;
        case 26:
            *key0_val = DBAL_FIELD_SOURCE_ADDRESS;
            break;
        case 27:
            *key0_val = DBAL_FIELD_VLAN_EDIT_VID_2;
            break;
        case 28:
            *key0_val = DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN;
            break;
        case 29:
            *key0_val = DBAL_FIELD_VLAN_DOMAIN;
            break;
        case 30:
            *key0_val = DBAL_FIELD_SRC_IPV4_MASK;
            break;
        case 31:
            *key0_val = DBAL_FIELD_FODO_ASSIGNMENT_MODE;
            break;
        default:
            *key0_val = -1;
            break;
    }

    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_special_fields_name_mapping_set(
    int unit)
{
    int field_id_index;
    dnx_data_flow_special_fields_name_mapping_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_special_fields;
    int table_index = dnx_data_flow_special_fields_table_name_mapping;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 32;
    table->info_get.key_size[0] = 32;

    
    table->values[0].default_val = "\0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_flow_special_fields_name_mapping_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_flow_special_fields_table_name_mapping");

    
    default_data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->field_name = "\0";
    
    for (field_id_index = 0; field_id_index < table->keys[0].size; field_id_index++)
    {
        data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, field_id_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    table->key_map = jr2_a0_dnx_data_flow_special_fields_name_mapping_key_map;
    table->key_map_reverse = jr2_a0_dnx_data_flow_special_fields_name_mapping_key_reverse_map;
    
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_TUNNEL_ENDPOINT_IDENTIFIER, 0);
    data->field_name = "TUNNEL_ENDPOINT_IDENTIFIER";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_NEXT_LAYER_NETWORK_DOMAIN, 0);
    data->field_name = "NEXT_LAYER_NETWORK_DOMAIN";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_MPLS_LABEL, 0);
    data->field_name = "MPLS_LABEL";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_MPLS_LABEL_2, 0);
    data->field_name = "MPLS_LABEL_2";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_IPV4_DIP, 0);
    data->field_name = "IPV4_DIP";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_IPV4_SIP, 0);
    data->field_name = "IPV4_SIP";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VRF, 0);
    data->field_name = "VRF";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_IPV4_TUNNEL_TYPE, 0);
    data->field_name = "IPV4_TUNNEL_TYPE";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VLAN_EDIT_PROFILE, 0);
    data->field_name = "VLAN_EDIT_PROFILE";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VLAN_EDIT_PCP_DEI_PROFILE, 0);
    data->field_name = "VLAN_EDIT_PCP_DEI_PROFILE";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VLAN_EDIT_VID_1, 0);
    data->field_name = "VLAN_EDIT_VID_1";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VLAN_EDIT_VID_2, 0);
    data->field_name = "VLAN_EDIT_VID_2";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_FODO_ASSIGNMENT_MODE, 0);
    data->field_name = "VSI_ASSIGNMENT_MODE";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_SRC_IPV4_MASK, 0);
    data->field_name = "IPV4_SIP_MASK";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_DST_IPV4_MASK, 0);
    data->field_name = "IPV4_DIP_MASK";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_SRC_IPV6_MASK, 0);
    data->field_name = "IPV6_SIP_MASK";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_DST_IPV6_MASK, 0);
    data->field_name = "IPV6_DIP_MASK";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_BITSTR, 0);
    data->field_name = "BITSTR";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_PP_PORT, 0);
    data->field_name = "PP_PORT";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VLAN_DOMAIN, 0);
    data->field_name = "VLAN_DOMAIN";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_S_VID, 0);
    data->field_name = "S_VID";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_C_VID, 0);
    data->field_name = "C_VID";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_S_VID_1, 0);
    data->field_name = "S_VID_1";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_S_VID_2, 0);
    data->field_name = "S_VID_2";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_C_VID_1, 0);
    data->field_name = "C_VID_1";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_C_VID_2, 0);
    data->field_name = "C_VID_2";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_MAPPED_PP_PORT, 0);
    data->field_name = "PORT";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VID_OUTER_VLAN, 0);
    data->field_name = "VID_OUTER_VLAN";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_VID_INNER_VLAN, 0);
    data->field_name = "VID_INNER_VLAN";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_PCP_DEI_OUTER_VLAN, 0);
    data->field_name = "PCP_DEI_OUTER_VLAN";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_LAYER_TYPE, 0);
    data->field_name = "LAYER_TYPE";
    data = (dnx_data_flow_special_fields_name_mapping_t *) dnxc_data_mgmt_table_data_get(unit, table, DBAL_FIELD_SOURCE_ADDRESS, 0);
    data->field_name = "SRC_MAC_ADDRESS";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}






static shr_error_e
jr2_a0_dnx_data_flow_ipv4_use_flow_lif_init_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_ipv4;
    int define_index = dnx_data_flow_ipv4_define_use_flow_lif_init;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Enable/Disable flow lif usage\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "use_flow_lif_tunnel_init";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_flow_ipv4_use_flow_lif_term_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_flow;
    int submodule_index = dnx_data_flow_submodule_ipv4;
    int define_index = dnx_data_flow_ipv4_define_use_flow_lif_term;
    SHR_FUNC_INIT_VARS(unit);

    define = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].defines[define_index];
    
    define->default_data = 0;

    
    define->data = 0;

    
    define->flags |= DNXC_DATA_F_SUPPORTED;
    
    define->property.name = spn_CUSTOM_FEATURE;
    define->property.doc = 
        "\n"
        "Enable/Disable flow lif usage\n"
        "Default: 0\n"
        "\n"
    ;
    define->property.method = dnxc_data_property_method_suffix_enable;
    define->property.method_str = "suffix_enable";
    define->property.suffix = "use_flow_lif_tunnel_term";

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_define_value_set(unit, define));

    SHR_IF_ERR_EXIT(dnxc_data_mgmt_property_read(unit, &define->property, -1, &define->data));
    
    define->flags |= (DNXC_DATA_F_PROPERTY);

exit:
    SHR_FUNC_EXIT;
}




shr_error_e
jr2_a0_data_flow_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_flow;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_flow_submodule_special_fields;
    submodule = &module->submodules[submodule_index];

    

    

    
    data_index = dnx_data_flow_special_fields_table_name_mapping;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_flow_special_fields_name_mapping_set;
    
    submodule_index = dnx_data_flow_submodule_ipv4;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_flow_ipv4_define_use_flow_lif_init;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_ipv4_use_flow_lif_init_set;
    data_index = dnx_data_flow_ipv4_define_use_flow_lif_term;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_flow_ipv4_use_flow_lif_term_set;

    

    

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE



/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_field_map.h>




extern shr_error_e jr2_a0_data_field_map_attach(
    int unit);


extern shr_error_e jr2_b0_data_field_map_attach(
    int unit);


extern shr_error_e j2c_a0_data_field_map_attach(
    int unit);


extern shr_error_e q2a_a0_data_field_map_attach(
    int unit);


extern shr_error_e j2p_a0_data_field_map_attach(
    int unit);


extern shr_error_e j2x_a0_data_field_map_attach(
    int unit);




static shr_error_e
dnx_data_field_map_action_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "action";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_map_action_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field_map action features");

    
    submodule_data->nof_defines = _dnx_data_field_map_action_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field_map action defines");

    
    submodule_data->nof_tables = _dnx_data_field_map_action_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field_map action tables");

    
    submodule_data->tables[dnx_data_field_map_action_table_params].name = "params";
    submodule_data->tables[dnx_data_field_map_action_table_params].doc = "";
    submodule_data->tables[dnx_data_field_map_action_table_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_action_table_params].size_of_values = sizeof(dnx_data_field_map_action_params_t);
    submodule_data->tables[dnx_data_field_map_action_table_params].entry_get = dnx_data_field_map_action_params_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_action_table_params].nof_keys = 2;
    submodule_data->tables[dnx_data_field_map_action_table_params].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_map_action_table_params].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_map_action_table_params].keys[1].name = "action";
    submodule_data->tables[dnx_data_field_map_action_table_params].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_map_action_table_params].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_action_table_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_action_table_params].nof_values, "_dnx_data_field_map_action_table_params table values");
    submodule_data->tables[dnx_data_field_map_action_table_params].values[0].name = "id";
    submodule_data->tables[dnx_data_field_map_action_table_params].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_action_table_params].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_action_table_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_action_params_t, id);
    submodule_data->tables[dnx_data_field_map_action_table_params].values[1].name = "size";
    submodule_data->tables[dnx_data_field_map_action_table_params].values[1].type = "int";
    submodule_data->tables[dnx_data_field_map_action_table_params].values[1].doc = "";
    submodule_data->tables[dnx_data_field_map_action_table_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_map_action_params_t, size);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_map_action_feature_get(
    int unit,
    dnx_data_field_map_action_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_action, feature);
}




const dnx_data_field_map_action_params_t *
dnx_data_field_map_action_params_get(
    int unit,
    int stage,
    int action)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_action, dnx_data_field_map_action_table_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, action);
    return (const dnx_data_field_map_action_params_t *) data;

}


shr_error_e
dnx_data_field_map_action_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_action_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_action, dnx_data_field_map_action_table_params);
    data = (const dnx_data_field_map_action_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->id);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_map_action_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_action, dnx_data_field_map_action_table_params);

}






static shr_error_e
dnx_data_field_map_qual_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "qual";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_map_qual_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field_map qual features");

    
    submodule_data->nof_defines = _dnx_data_field_map_qual_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field_map qual defines");

    
    submodule_data->nof_tables = _dnx_data_field_map_qual_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field_map qual tables");

    
    submodule_data->tables[dnx_data_field_map_qual_table_params].name = "params";
    submodule_data->tables[dnx_data_field_map_qual_table_params].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_params].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_qual_table_params].size_of_values = sizeof(dnx_data_field_map_qual_params_t);
    submodule_data->tables[dnx_data_field_map_qual_table_params].entry_get = dnx_data_field_map_qual_params_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_qual_table_params].nof_keys = 2;
    submodule_data->tables[dnx_data_field_map_qual_table_params].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_map_qual_table_params].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_params].keys[1].name = "qual";
    submodule_data->tables[dnx_data_field_map_qual_table_params].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_map_qual_table_params].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_qual_table_params].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_qual_table_params].nof_values, "_dnx_data_field_map_qual_table_params table values");
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[0].name = "offset";
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_params_t, offset);
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[1].name = "size";
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[1].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[1].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_params].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_params_t, size);

    
    submodule_data->tables[dnx_data_field_map_qual_table_cs].name = "cs";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_qual_table_cs].size_of_values = sizeof(dnx_data_field_map_qual_cs_t);
    submodule_data->tables[dnx_data_field_map_qual_table_cs].entry_get = dnx_data_field_map_qual_cs_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_qual_table_cs].nof_keys = 2;
    submodule_data->tables[dnx_data_field_map_qual_table_cs].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].keys[1].name = "qual";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_map_qual_table_cs].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_qual_table_cs].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_qual_table_cs].nof_values, "_dnx_data_field_map_qual_table_cs table values");
    submodule_data->tables[dnx_data_field_map_qual_table_cs].values[0].name = "dbal_field";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_cs].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_cs_t, dbal_field);

    
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].name = "special_metadata_qual";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].size_of_values = sizeof(dnx_data_field_map_qual_special_metadata_qual_t);
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].entry_get = dnx_data_field_map_qual_special_metadata_qual_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].nof_keys = 2;
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].keys[0].name = "stage";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].keys[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].keys[1].name = "qual";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].keys[1].doc = "";

    
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].nof_values = 4;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].nof_values, "_dnx_data_field_map_qual_table_special_metadata_qual table values");
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[0].name = "nof_parts";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[0].type = "uint8";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_special_metadata_qual_t, nof_parts);
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[1].name = "offsets";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[1].type = "uint16[DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS]";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[1].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_special_metadata_qual_t, offsets);
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[2].name = "sizes";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[2].type = "uint8[DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS]";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[2].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_special_metadata_qual_t, sizes);
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[3].name = "vw_name";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[3].type = "char *";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[3].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_special_metadata_qual].values[3].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_special_metadata_qual_t, vw_name);

    
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].name = "layer_record_info_ingress";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].size_of_values = sizeof(dnx_data_field_map_qual_layer_record_info_ingress_t);
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].entry_get = dnx_data_field_map_qual_layer_record_info_ingress_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].nof_keys = 1;
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].keys[0].name = "layer_record";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].nof_values, "_dnx_data_field_map_qual_table_layer_record_info_ingress table values");
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[0].name = "valid";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_layer_record_info_ingress_t, valid);
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[1].name = "size";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[1].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[1].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_layer_record_info_ingress_t, size);
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[2].name = "offset";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[2].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[2].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_ingress].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_layer_record_info_ingress_t, offset);

    
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].name = "layer_record_info_egress";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].size_of_values = sizeof(dnx_data_field_map_qual_layer_record_info_egress_t);
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].entry_get = dnx_data_field_map_qual_layer_record_info_egress_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].nof_keys = 1;
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].keys[0].name = "layer_record";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].nof_values = 3;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].nof_values, "_dnx_data_field_map_qual_table_layer_record_info_egress table values");
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[0].name = "valid";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_layer_record_info_egress_t, valid);
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[1].name = "size";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[1].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[1].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[1].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_layer_record_info_egress_t, size);
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[2].name = "offset";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[2].type = "int";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[2].doc = "";
    submodule_data->tables[dnx_data_field_map_qual_table_layer_record_info_egress].values[2].offset = UTILEX_OFFSETOF(dnx_data_field_map_qual_layer_record_info_egress_t, offset);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_map_qual_feature_get(
    int unit,
    dnx_data_field_map_qual_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, feature);
}




const dnx_data_field_map_qual_params_t *
dnx_data_field_map_qual_params_get(
    int unit,
    int stage,
    int qual)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_params);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, qual);
    return (const dnx_data_field_map_qual_params_t *) data;

}

const dnx_data_field_map_qual_cs_t *
dnx_data_field_map_qual_cs_get(
    int unit,
    int stage,
    int qual)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_cs);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, qual);
    return (const dnx_data_field_map_qual_cs_t *) data;

}

const dnx_data_field_map_qual_special_metadata_qual_t *
dnx_data_field_map_qual_special_metadata_qual_get(
    int unit,
    int stage,
    int qual)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_special_metadata_qual);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, qual);
    return (const dnx_data_field_map_qual_special_metadata_qual_t *) data;

}

const dnx_data_field_map_qual_layer_record_info_ingress_t *
dnx_data_field_map_qual_layer_record_info_ingress_get(
    int unit,
    int layer_record)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_layer_record_info_ingress);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, layer_record, 0);
    return (const dnx_data_field_map_qual_layer_record_info_ingress_t *) data;

}

const dnx_data_field_map_qual_layer_record_info_egress_t *
dnx_data_field_map_qual_layer_record_info_egress_get(
    int unit,
    int layer_record)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_layer_record_info_egress);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, layer_record, 0);
    return (const dnx_data_field_map_qual_layer_record_info_egress_t *) data;

}


shr_error_e
dnx_data_field_map_qual_params_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_qual_params_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_params);
    data = (const dnx_data_field_map_qual_params_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_map_qual_cs_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_qual_cs_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_cs);
    data = (const dnx_data_field_map_qual_cs_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_field);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_map_qual_special_metadata_qual_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_qual_special_metadata_qual_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_special_metadata_qual);
    data = (const dnx_data_field_map_qual_special_metadata_qual_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, key1);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->nof_parts);
            break;
        case 1:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, data->offsets);
            break;
        case 2:
            DNXC_DATA_MGMT_ARR_STR(buffer, DNX_DATA_MAX_FIELD_QUAL_SPECIAL_METADATA_QUAL_MAX_PARTS, data->sizes);
            break;
        case 3:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%s", data->vw_name == NULL ? "" : data->vw_name);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_map_qual_layer_record_info_ingress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_qual_layer_record_info_ingress_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_layer_record_info_ingress);
    data = (const dnx_data_field_map_qual_layer_record_info_ingress_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset);
            break;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dnx_data_field_map_qual_layer_record_info_egress_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_qual_layer_record_info_egress_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_layer_record_info_egress);
    data = (const dnx_data_field_map_qual_layer_record_info_egress_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->size);
            break;
        case 2:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->offset);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_map_qual_params_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_params);

}

const dnxc_data_table_info_t *
dnx_data_field_map_qual_cs_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_cs);

}

const dnxc_data_table_info_t *
dnx_data_field_map_qual_special_metadata_qual_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_special_metadata_qual);

}

const dnxc_data_table_info_t *
dnx_data_field_map_qual_layer_record_info_ingress_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_layer_record_info_ingress);

}

const dnxc_data_table_info_t *
dnx_data_field_map_qual_layer_record_info_egress_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_qual, dnx_data_field_map_qual_table_layer_record_info_egress);

}






static shr_error_e
dnx_data_field_map_tcam_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "tcam";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_map_tcam_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field_map tcam features");

    
    submodule_data->nof_defines = _dnx_data_field_map_tcam_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field_map tcam defines");

    
    submodule_data->nof_tables = _dnx_data_field_map_tcam_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field_map tcam tables");

    
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].name = "tcam_pp_app";
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].doc = "";
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].size_of_values = sizeof(dnx_data_field_map_tcam_tcam_pp_app_t);
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].entry_get = dnx_data_field_map_tcam_tcam_pp_app_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].nof_keys = 1;
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].keys[0].name = "pp_app";
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].nof_values, "_dnx_data_field_map_tcam_table_tcam_pp_app table values");
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].values[0].name = "dbal_table";
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_tcam_table_tcam_pp_app].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_tcam_tcam_pp_app_t, dbal_table);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_map_tcam_feature_get(
    int unit,
    dnx_data_field_map_tcam_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_tcam, feature);
}




const dnx_data_field_map_tcam_tcam_pp_app_t *
dnx_data_field_map_tcam_tcam_pp_app_get(
    int unit,
    int pp_app)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_tcam, dnx_data_field_map_tcam_table_tcam_pp_app);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, pp_app, 0);
    return (const dnx_data_field_map_tcam_tcam_pp_app_t *) data;

}


shr_error_e
dnx_data_field_map_tcam_tcam_pp_app_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_tcam_tcam_pp_app_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_tcam, dnx_data_field_map_tcam_table_tcam_pp_app);
    data = (const dnx_data_field_map_tcam_tcam_pp_app_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dbal_table);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_map_tcam_tcam_pp_app_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_tcam, dnx_data_field_map_tcam_table_tcam_pp_app);

}






static shr_error_e
dnx_data_field_map_bcm_to_dnx_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "bcm_to_dnx";
    submodule_data->doc = "";
    
    submodule_data->nof_features = _dnx_data_field_map_bcm_to_dnx_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data field_map bcm_to_dnx features");

    
    submodule_data->nof_defines = _dnx_data_field_map_bcm_to_dnx_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data field_map bcm_to_dnx defines");

    
    submodule_data->nof_tables = _dnx_data_field_map_bcm_to_dnx_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data field_map bcm_to_dnx tables");

    
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].name = "forward_context";
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].doc = "";
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].size_of_values = sizeof(dnx_data_field_map_bcm_to_dnx_forward_context_t);
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].entry_get = dnx_data_field_map_bcm_to_dnx_forward_context_entry_str_get;

    
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].nof_keys = 1;
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].keys[0].name = "bcm_data";
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].keys[0].doc = "";

    
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].nof_values = 1;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].values, dnxc_data_value_t, submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].nof_values, "_dnx_data_field_map_bcm_to_dnx_table_forward_context table values");
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].values[0].name = "dnx_data";
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].values[0].type = "int";
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].values[0].doc = "";
    submodule_data->tables[dnx_data_field_map_bcm_to_dnx_table_forward_context].values[0].offset = UTILEX_OFFSETOF(dnx_data_field_map_bcm_to_dnx_forward_context_t, dnx_data);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_field_map_bcm_to_dnx_feature_get(
    int unit,
    dnx_data_field_map_bcm_to_dnx_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_bcm_to_dnx, feature);
}




const dnx_data_field_map_bcm_to_dnx_forward_context_t *
dnx_data_field_map_bcm_to_dnx_forward_context_get(
    int unit,
    int bcm_data)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_bcm_to_dnx, dnx_data_field_map_bcm_to_dnx_table_forward_context);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, bcm_data, 0);
    return (const dnx_data_field_map_bcm_to_dnx_forward_context_t *) data;

}


shr_error_e
dnx_data_field_map_bcm_to_dnx_forward_context_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_field_map_bcm_to_dnx_forward_context_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_bcm_to_dnx, dnx_data_field_map_bcm_to_dnx_table_forward_context);
    data = (const dnx_data_field_map_bcm_to_dnx_forward_context_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->dnx_data);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_field_map_bcm_to_dnx_forward_context_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_field_map, dnx_data_field_map_submodule_bcm_to_dnx, dnx_data_field_map_bcm_to_dnx_table_forward_context);

}



shr_error_e
dnx_data_field_map_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "field_map";
    module_data->nof_submodules = _dnx_data_field_map_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data field_map submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_field_map_action_init(unit, &module_data->submodules[dnx_data_field_map_submodule_action]));
    SHR_IF_ERR_EXIT(dnx_data_field_map_qual_init(unit, &module_data->submodules[dnx_data_field_map_submodule_qual]));
    SHR_IF_ERR_EXIT(dnx_data_field_map_tcam_init(unit, &module_data->submodules[dnx_data_field_map_submodule_tcam]));
    SHR_IF_ERR_EXIT(dnx_data_field_map_bcm_to_dnx_init(unit, &module_data->submodules[dnx_data_field_map_submodule_bcm_to_dnx]));
    

    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(jr2_b0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2p_a2(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_field_map_attach(unit));
    }
    else


    if (dnxc_data_mgmt_is_j2x_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_field_map_attach(unit));
        SHR_IF_ERR_EXIT(j2x_a0_data_field_map_attach(unit));
    }
    else

    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


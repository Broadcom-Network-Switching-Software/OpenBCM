

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INGRCS

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_ingress_cs.h>



extern shr_error_e jr2_a0_data_ingress_cs_attach(
    int unit);
extern shr_error_e j2c_a0_data_ingress_cs_attach(
    int unit);
extern shr_error_e q2a_a0_data_ingress_cs_attach(
    int unit);
extern shr_error_e j2p_a0_data_ingress_cs_attach(
    int unit);



static shr_error_e
dnx_data_ingress_cs_properties_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "properties";
    submodule_data->doc = "Information about ingress CS TCAM, plus PRT TCAM and  ePMF CS TCAM.";
    
    submodule_data->nof_features = _dnx_data_ingress_cs_properties_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingress_cs properties features");

    
    submodule_data->nof_defines = _dnx_data_ingress_cs_properties_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingress_cs properties defines");

    submodule_data->defines[dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max].name = "cs_buffer_nof_bits_max";
    submodule_data->defines[dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max].doc = "Length in bits of the Context Selection TCAM buffer for qualifiers or masks, the maximum of all ingress CS TCAMs.";
    
    submodule_data->defines[dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max].flags |= DNXC_DATA_F_DEFINE;

    submodule_data->defines[dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix].name = "nof_bits_entry_size_prefix";
    submodule_data->defines[dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix].doc = "Number of bits used in the prefix indicating half key or full key. Expected to be used for each half key at the lsb for tables that support half entries.";
    
    submodule_data->defines[dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix].flags |= DNXC_DATA_F_DEFINE;

    
    submodule_data->nof_tables = _dnx_data_ingress_cs_properties_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingress_cs properties tables");

    
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].name = "per_stage";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].doc = "Per stage (CS DB) information, including whether the CS_TCAM supports half entries.";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].flags |= DNXC_DATA_F_TABLE;
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].size_of_values = sizeof(dnx_data_ingress_cs_properties_per_stage_t);
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].entry_get = dnx_data_ingress_cs_properties_per_stage_entry_str_get;

    
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].nof_keys = 1;
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].keys[0].name = "stage";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].keys[0].doc = "CS DB, which CS TCAM to provide information for.";

    
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].nof_values = 2;
    DNXC_DATA_ALLOC(submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values, dnxc_data_value_t, submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].nof_values, "_dnx_data_ingress_cs_properties_table_per_stage table values");
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[0].name = "valid";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[0].type = "int";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[0].doc = "If the stage has information in this table.";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[0].offset = UTILEX_OFFSETOF(dnx_data_ingress_cs_properties_per_stage_t, valid);
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[1].name = "supports_half_entries";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[1].type = "int";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[1].doc = "If the CS TCAM can use half entries.";
    submodule_data->tables[dnx_data_ingress_cs_properties_table_per_stage].values[1].offset = UTILEX_OFFSETOF(dnx_data_ingress_cs_properties_per_stage_t, supports_half_entries);


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingress_cs_properties_feature_get(
    int unit,
    dnx_data_ingress_cs_properties_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_properties, feature);
}


uint32
dnx_data_ingress_cs_properties_cs_buffer_nof_bits_max_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_properties, dnx_data_ingress_cs_properties_define_cs_buffer_nof_bits_max);
}

uint32
dnx_data_ingress_cs_properties_nof_bits_entry_size_prefix_get(
    int unit)
{
    return dnxc_data_mgmt_define_data_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_properties, dnx_data_ingress_cs_properties_define_nof_bits_entry_size_prefix);
}



const dnx_data_ingress_cs_properties_per_stage_t *
dnx_data_ingress_cs_properties_per_stage_get(
    int unit,
    int stage)
{
    char *data;
    dnxc_data_table_t *table;

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_properties, dnx_data_ingress_cs_properties_table_per_stage);
    
    data = dnxc_data_mgmt_table_data_get(unit, table, stage, 0);
    return (const dnx_data_ingress_cs_properties_per_stage_t *) data;

}


shr_error_e
dnx_data_ingress_cs_properties_per_stage_entry_str_get(
    int unit,
    char *buffer,
    int key0,
    int key1,
    int value_index)
{
    dnxc_data_table_t *table;
    const dnx_data_ingress_cs_properties_per_stage_t *data;
    SHR_FUNC_INIT_VARS(unit);

    
    table = dnxc_data_mgmt_table_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_properties, dnx_data_ingress_cs_properties_table_per_stage);
    data = (const dnx_data_ingress_cs_properties_per_stage_t *) dnxc_data_mgmt_table_data_diag_get(unit, table, key0, 0);
    switch (value_index)
    {
        case 0:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->valid);
            break;
        case 1:
            sal_snprintf(buffer, DNXC_DATA_MGMT_MAX_TABLE_VALUE_LENGTH, "%d", data->supports_half_entries);
            break;
    }

    SHR_FUNC_EXIT;
}


const dnxc_data_table_info_t *
dnx_data_ingress_cs_properties_per_stage_info_get(
    int unit)
{

    
    return dnxc_data_mgmt_table_info_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_properties, dnx_data_ingress_cs_properties_table_per_stage);

}






static shr_error_e
dnx_data_ingress_cs_features_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "features";
    submodule_data->doc = "Per devices features";
    
    submodule_data->nof_features = _dnx_data_ingress_cs_features_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingress_cs features features");

    submodule_data->features[dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write].name = "disable_ecc_fix_en_before_read_write";
    submodule_data->features[dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write].doc = "";
    submodule_data->features[dnx_data_ingress_cs_features_disable_ecc_fix_en_before_read_write].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingress_cs_features_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingress_cs features defines");

    
    submodule_data->nof_tables = _dnx_data_ingress_cs_features_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingress_cs features tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingress_cs_features_feature_get(
    int unit,
    dnx_data_ingress_cs_features_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_features, feature);
}











static shr_error_e
dnx_data_ingress_cs_parser_init(
    int unit,
    dnxc_data_submodule_t *submodule_data)
{
    SHR_FUNC_INIT_VARS(unit);

    submodule_data->name = "parser";
    submodule_data->doc = "Ingress parser data";
    
    submodule_data->nof_features = _dnx_data_ingress_cs_parser_feature_nof;
    DNXC_DATA_ALLOC(submodule_data->features, dnxc_data_feature_t,  submodule_data->nof_features, "_dnxc_data ingress_cs parser features");

    submodule_data->features[dnx_data_ingress_cs_parser_parsing_context_map_enable].name = "parsing_context_map_enable";
    submodule_data->features[dnx_data_ingress_cs_parser_parsing_context_map_enable].doc = "data path control valid flag";
    submodule_data->features[dnx_data_ingress_cs_parser_parsing_context_map_enable].flags |= DNXC_DATA_F_FEATURE;

    
    submodule_data->nof_defines = _dnx_data_ingress_cs_parser_define_nof;
    DNXC_DATA_ALLOC(submodule_data->defines, dnxc_data_define_t, submodule_data->nof_defines, "_dnxc_data ingress_cs parser defines");

    
    submodule_data->nof_tables = _dnx_data_ingress_cs_parser_table_nof;
    DNXC_DATA_ALLOC(submodule_data->tables, dnxc_data_table_t, submodule_data->nof_tables, "_dnxc_data ingress_cs parser tables");


exit:
    SHR_FUNC_EXIT;
}


int
dnx_data_ingress_cs_parser_feature_get(
    int unit,
    dnx_data_ingress_cs_parser_feature_e feature)
{
    return dnxc_data_mgmt_feature_data_get(unit, dnx_data_module_ingress_cs, dnx_data_ingress_cs_submodule_parser, feature);
}








shr_error_e
dnx_data_ingress_cs_init(
    int unit,
    dnxc_data_module_t *module_data)
{
    SHR_FUNC_INIT_VARS(unit);

    
    module_data->name = "ingress_cs";
    module_data->nof_submodules = _dnx_data_ingress_cs_submodule_nof;
    DNXC_DATA_ALLOC(module_data->submodules, dnxc_data_submodule_t, module_data->nof_submodules, "_dnxc_data ingress_cs submodules");

    
    SHR_IF_ERR_EXIT(dnx_data_ingress_cs_properties_init(unit, &module_data->submodules[dnx_data_ingress_cs_submodule_properties]));
    SHR_IF_ERR_EXIT(dnx_data_ingress_cs_features_init(unit, &module_data->submodules[dnx_data_ingress_cs_submodule_features]));
    SHR_IF_ERR_EXIT(dnx_data_ingress_cs_parser_init(unit, &module_data->submodules[dnx_data_ingress_cs_submodule_parser]));
    
    if (dnxc_data_mgmt_is_jr2_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_jr2_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2c_a1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
        SHR_IF_ERR_EXIT(j2c_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_q2a_b1(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
        SHR_IF_ERR_EXIT(q2a_a0_data_ingress_cs_attach(unit));
    }
    else
    if (dnxc_data_mgmt_is_j2p_a0(unit))
    {
        SHR_IF_ERR_EXIT(jr2_a0_data_ingress_cs_attach(unit));
        SHR_IF_ERR_EXIT(j2p_a0_data_ingress_cs_attach(unit));
    }
    else
    {
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


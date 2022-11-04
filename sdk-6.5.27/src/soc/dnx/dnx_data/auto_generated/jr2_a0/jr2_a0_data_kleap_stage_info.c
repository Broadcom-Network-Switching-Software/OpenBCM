
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

#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_kleap_stage_info.h>








static shr_error_e
jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_set(
    int unit)
{
    dnxc_data_define_t *define;
    int module_index = dnx_data_module_kleap_stage_info;
    int submodule_index = dnx_data_kleap_stage_info_submodule_kleap_stage_info;
    int define_index = dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages;
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
jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_set(
    int unit)
{
    int stage_index_index;
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_kleap_stage_info;
    int submodule_index = dnx_data_kleap_stage_info_submodule_kleap_stage_info;
    int table_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
    table->values[0].default_val = "DEFAULT_NAME";
    table->values[1].default_val = "0";
    table->values[2].default_val = "1";
    table->values[3].default_val = "0";
    table->values[4].default_val = "0";
    table->values[5].default_val = "0";
    table->values[6].default_val = "0";
    table->values[7].default_val = "DBAL_NOF_TABLES";
    table->values[8].default_val = "DBAL_NOF_TABLES";
    table->values[9].default_val = "DBAL_NOF_TABLES";
    table->values[10].default_val = "DBAL_NOF_TABLES";
    table->values[11].default_val = "DBAL_NOF_TABLES";
    table->values[12].default_val = "DBAL_FIELD_EMPTY";
    table->values[13].default_val = "DBAL_FIELD_EMPTY";
    table->values[14].default_val = "DBAL_FIELD_EMPTY";
    table->values[15].default_val = "DBAL_FIELD_EMPTY";
    table->values[16].default_val = "DBAL_FIELD_EMPTY";
    table->values[17].default_val = "DBAL_FIELD_EMPTY";
    table->values[18].default_val = "DBAL_FIELD_EMPTY";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage");

    
    default_data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->stage_name = "DEFAULT_NAME";
    default_data->type = 0;
    default_data->nof_sub_stages = 1;
    default_data->nof_kbrs = 0;
    default_data->nof_ffc = 0;
    default_data->nof_ffc_g = 0;
    default_data->nof_pd = 0;
    default_data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
    default_data->dbal_table_kbr_info = DBAL_NOF_TABLES;
    default_data->dbal_table_ffc_instruction = DBAL_NOF_TABLES;
    default_data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
    default_data->dbal_table_pd_info = DBAL_NOF_TABLES;
    default_data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_EMPTY;
    default_data->dbal_context_profile_field_per_stage = DBAL_FIELD_EMPTY;
    default_data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    default_data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    default_data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    default_data->dbal_header_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    default_data->dbal_record_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    
    for (stage_index_index = 0; stage_index_index < table->keys[0].size; stage_index_index++)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->stage_name = "VTT1";
        data->type = SINGLE_STAGE_TYPE;
        data->nof_sub_stages = 1;
        data->nof_kbrs = 6;
        data->nof_ffc = 10;
        data->nof_ffc_g = 1;
        data->nof_pd = 3;
        data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_VT1_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_VT1_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
        data->dbal_table_pd_info = DBAL_TABLE_KLEAP_VT1_PD_INFO;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_KBR_IDX;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_CONTEXT_PROFILE;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_VT1_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_VT1_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_VT1_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_VT1_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_VT1_FFC_LAYER_RECORD;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->stage_name = "VTT23";
        data->type = DOUBLE_STAGE_TYPE;
        data->nof_sub_stages = 2;
        data->nof_kbrs = 7;
        data->nof_ffc = 20;
        data->nof_ffc_g = 4;
        data->nof_pd = 6;
        data->dbal_table_resource_mapping = DBAL_TABLE_KLEAP_VT23_RESOURCES_MAPPING;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_VT23_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_VT23_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_TABLE_KLEAP_VT23_FFC_QUAD_IS_ACL;
        data->dbal_table_pd_info = DBAL_TABLE_KLEAP_VT23_PD_INFO;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_KBR_IDX;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_CONTEXT_PROFILE;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_VT23_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_VT23_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_VT23_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_VT23_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_VT23_FFC_LAYER_RECORD;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->stage_name = "VTT45";
        data->type = DOUBLE_STAGE_TYPE;
        data->nof_sub_stages = 2;
        data->nof_kbrs = 6;
        data->nof_ffc = 20;
        data->nof_ffc_g = 4;
        data->nof_pd = 6;
        data->dbal_table_resource_mapping = DBAL_TABLE_KLEAP_VT45_RESOURCES_MAPPING;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_VT45_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_VT45_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_TABLE_KLEAP_VT45_FFC_QUAD_IS_ACL;
        data->dbal_table_pd_info = DBAL_TABLE_KLEAP_VT45_PD_INFO;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_KBR_IDX;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_CONTEXT_PROFILE;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_VT45_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_VT45_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_VT45_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_VT45_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_VT45_FFC_LAYER_RECORD;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->stage_name = "FWD12";
        data->type = DOUBLE_STAGE_TYPE;
        data->nof_sub_stages = 2;
        data->nof_kbrs = 18;
        data->nof_ffc = 38;
        data->nof_ffc_g = 5;
        data->nof_pd = 6;
        data->dbal_table_resource_mapping = DBAL_TABLE_KLEAP_FWD12_RESOURCES_MAPPING;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_FWD12_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_FWD12_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_TABLE_KLEAP_FWD12_FFC_QUAD_IS_ACL;
        data->dbal_table_pd_info = DBAL_TABLE_KLEAP_FWD12_PD_INFO;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_FWD12_KBR_ID;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_CONTEXT_PROFILE;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_FWD12_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_FWD12_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_FWD12_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_FWD12_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_FWD12_FFC_LAYER_RECORD;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->stage_name = "IPMF1";
        data->type = SINGLE_STAGE_TYPE;
        data->nof_sub_stages = 1;
        data->nof_kbrs = 5;
        data->nof_ffc = 64;
        data->nof_ffc_g = 1;
        data->nof_pd = 0;
        data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_IPMF1_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_IPMF1_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
        data->dbal_table_pd_info = DBAL_NOF_TABLES;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_FIELD_KEY;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_FIELD_PMF_CTX_ID;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_LAYER_RECORD;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->stage_name = "IPMF1_INTIAL";
        data->type = SINGLE_STAGE_TYPE;
        data->nof_sub_stages = 1;
        data->nof_kbrs = 5;
        data->nof_ffc = 64;
        data->nof_ffc_g = 1;
        data->nof_pd = 0;
        data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_IPMF1_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
        data->dbal_table_pd_info = DBAL_NOF_TABLES;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_FIELD_KEY;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_FIELD_PMF_CTX_ID;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_IPMF1_FFC_LAYER_RECORD;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->stage_name = "IPMF2";
        data->type = SINGLE_STAGE_TYPE;
        data->nof_sub_stages = 1;
        data->nof_kbrs = 5;
        data->nof_ffc = 32;
        data->nof_ffc_g = 1;
        data->nof_pd = 0;
        data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_IPMF2_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_IPMF2_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
        data->dbal_table_pd_info = DBAL_NOF_TABLES;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_FIELD_KEY;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_FIELD_PMF_CTX_ID;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_IPMF2_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_IPMF2_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_EMPTY;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_EMPTY;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->stage_name = "IPMF3";
        data->type = SINGLE_STAGE_TYPE;
        data->nof_sub_stages = 1;
        data->nof_kbrs = 3;
        data->nof_ffc = 32;
        data->nof_ffc_g = 1;
        data->nof_pd = 0;
        data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_IPMF3_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_IPMF3_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
        data->dbal_table_pd_info = DBAL_NOF_TABLES;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_FIELD_KEY;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_FIELD_PMF_CTX_ID;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_IPMF3_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_IPMF3_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_EMPTY;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_EMPTY;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_EMPTY;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->stage_name = "EPMF";
        data->type = SINGLE_STAGE_TYPE;
        data->nof_sub_stages = 1;
        data->nof_kbrs = 3;
        data->nof_ffc = 20;
        data->nof_ffc_g = 1;
        data->nof_pd = 0;
        data->dbal_table_resource_mapping = DBAL_NOF_TABLES;
        data->dbal_table_kbr_info = DBAL_TABLE_KLEAP_E_PMF_KBR_INFO;
        data->dbal_table_ffc_instruction = DBAL_TABLE_KLEAP_E_PMF_FFC_INSTRUCTION;
        data->dbal_table_ffc_quad_is_acl = DBAL_NOF_TABLES;
        data->dbal_table_pd_info = DBAL_NOF_TABLES;
        data->dbal_kbr_idx_field_per_stage = DBAL_FIELD_FIELD_KEY;
        data->dbal_context_profile_field_per_stage = DBAL_FIELD_FIELD_PMF_CTX_ID;
        data->dbal_ffc_instruction_field_per_stage = DBAL_FIELD_EPMF_FFC_INSTRUCTION;
        data->dbal_litrally_instruction_field_per_stage = DBAL_FIELD_EPMF_FFC_LITERALLY;
        data->dbal_relative_header_instruction_field_per_stage = DBAL_FIELD_EPMF_FFC_RELATIVE_HEADER;
        data->dbal_header_instruction_field_per_stage = DBAL_FIELD_EPMF_FFC_HEADER;
        data->dbal_record_instruction_field_per_stage = DBAL_FIELD_EPMF_FFC_LAYER_RECORD;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_set(
    int unit)
{
    int stage_index_index;
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_kleap_stage_info;
    int submodule_index = dnx_data_kleap_stage_info_submodule_kleap_stage_info;
    int table_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "0";
    table->values[2].default_val = "0";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t, (1 * (table->keys[0].size) + 1  ), "data of dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr");

    
    default_data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->nof_kbrs = 0;
    default_data->kbr2physical[0] = 0;
    default_data->kbr2key_sig[0] = 0;
    
    for (stage_index_index = 0; stage_index_index < table->keys[0].size; stage_index_index++)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index_index, 0);
        sal_memcpy(data, default_data, table->size_of_values);
    }
    
    if (0 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->nof_kbrs = 6;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_ISEM_1;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_ISEM_1;
        data->kbr2physical[5] = DBAL_PHYSICAL_TABLE_SEXEM_1;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = SINGLE_IF;
        data->kbr2key_sig[3] = IF_0;
        data->kbr2key_sig[4] = IF_1;
        data->kbr2key_sig[5] = SINGLE_IF;
    }
    if (1 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->nof_kbrs = 7;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_ISEM_2;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_ISEM_2;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_SEXEM_1;
        data->kbr2physical[5] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[6] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = IF_0;
        data->kbr2key_sig[3] = IF_1;
        data->kbr2key_sig[4] = SINGLE_IF;
        data->kbr2key_sig[5] = IF_0;
        data->kbr2key_sig[6] = IF_1;
    }
    if (2 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->nof_kbrs = 6;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_ISEM_2;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_ISEM_2;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_SEXEM_1;
        data->kbr2physical[5] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = IF_0;
        data->kbr2key_sig[3] = IF_1;
        data->kbr2key_sig[4] = SINGLE_IF;
        data->kbr2key_sig[5] = SINGLE_IF;
    }
    if (3 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->nof_kbrs = 18;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_FLP_LARGE_TCAM;
        data->kbr2physical[5] = DBAL_PHYSICAL_TABLE_FLP_LARGE_TCAM;
        data->kbr2physical[6] = DBAL_PHYSICAL_TABLE_LEM;
        data->kbr2physical[7] = DBAL_PHYSICAL_TABLE_SEXEM_3;
        data->kbr2physical[8] = DBAL_PHYSICAL_TABLE_SEXEM_2;
        data->kbr2physical[9] = DBAL_PHYSICAL_TABLE_KAPS_1;
        data->kbr2physical[10] = DBAL_PHYSICAL_TABLE_KAPS_1;
        data->kbr2physical[11] = DBAL_PHYSICAL_TABLE_LEM;
        data->kbr2physical[12] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[13] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[14] = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->kbr2physical[15] = DBAL_PHYSICAL_TABLE_KAPS_2;
        data->kbr2physical[16] = DBAL_PHYSICAL_TABLE_FLP_LARGE_TCAM;
        data->kbr2physical[17] = DBAL_PHYSICAL_TABLE_FLP_LARGE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = IF_2;
        data->kbr2key_sig[3] = IF_3;
        data->kbr2key_sig[4] = IF_0;
        data->kbr2key_sig[5] = IF_1;
        data->kbr2key_sig[6] = IF_1;
        data->kbr2key_sig[7] = SINGLE_IF;
        data->kbr2key_sig[8] = SINGLE_IF;
        data->kbr2key_sig[9] = IF_0;
        data->kbr2key_sig[10] = IF_1;
        data->kbr2key_sig[11] = IF_0;
        data->kbr2key_sig[12] = IF_0;
        data->kbr2key_sig[13] = IF_1;
        data->kbr2key_sig[14] = IF_0;
        data->kbr2key_sig[15] = IF_1;
        data->kbr2key_sig[16] = IF_2;
        data->kbr2key_sig[17] = IF_3;
    }
    if (4 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->nof_kbrs = 5;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = INVALID_IF;
        data->kbr2key_sig[3] = INVALID_IF;
        data->kbr2key_sig[4] = INVALID_IF;
    }
    if (5 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->nof_kbrs = 5;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = INVALID_IF;
        data->kbr2key_sig[3] = INVALID_IF;
        data->kbr2key_sig[4] = INVALID_IF;
    }
    if (6 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->nof_kbrs = 5;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[3] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2physical[4] = DBAL_PHYSICAL_TABLE_NONE;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = INVALID_IF;
        data->kbr2key_sig[3] = INVALID_IF;
        data->kbr2key_sig[4] = INVALID_IF;
    }
    if (7 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->nof_kbrs = 3;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = INVALID_IF;
    }
    if (8 < table->keys[0].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->nof_kbrs = 3;
        data->kbr2physical[0] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[1] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2physical[2] = DBAL_PHYSICAL_TABLE_TCAM;
        data->kbr2key_sig[0] = INVALID_IF;
        data->kbr2key_sig[1] = INVALID_IF;
        data->kbr2key_sig[2] = INVALID_IF;
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_set(
    int unit)
{
    int stage_index_index;
    int pd_index_index;
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_kleap_stage_info;
    int submodule_index = dnx_data_kleap_stage_info_submodule_kleap_stage_info;
    int table_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;
    table->keys[1].size = 8;
    table->info_get.key_size[1] = 8;

    
    table->values[0].default_val = "0";
    table->values[1].default_val = "DEFAULT_NAME";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd");

    
    default_data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->pd_mapping_type = 0;
    default_data->pd_mapping_name = "DEFAULT_NAME";
    
    for (stage_index_index = 0; stage_index_index < table->keys[0].size; stage_index_index++)
    {
        for (pd_index_index = 0; pd_index_index < table->keys[1].size; pd_index_index++)
        {
            data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index_index, pd_index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT1_VT_PD";
    }
    if (0 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 1);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT1_FWD_PD";
    }
    if (0 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 2);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT1_FLOAT_PD";
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT2_TT_PD";
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT2_FWD_PD";
    }
    if (1 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 2);
        data->pd_mapping_type = SUB_STAGE_1_PD;
        data->pd_mapping_name = "VT3_TT_PD";
    }
    if (1 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 3);
        data->pd_mapping_type = SUB_STAGE_1_PD;
        data->pd_mapping_name = "VT3_FWD_PD";
    }
    if (1 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 4);
        data->pd_mapping_type = SHARED_PD_0;
        data->pd_mapping_name = "VT23_FLOAT_PD";
    }
    if (1 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 5);
        data->pd_mapping_type = SHARED_PD_1;
        data->pd_mapping_name = "VT23_FLOAT_PD";
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT4_TT_PD";
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "VT4_FWD_PD";
    }
    if (2 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 2);
        data->pd_mapping_type = SUB_STAGE_1_PD;
        data->pd_mapping_name = "VT5_TT_PD";
    }
    if (2 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 3);
        data->pd_mapping_type = SUB_STAGE_1_PD;
        data->pd_mapping_name = "VT5_FWD_PD";
    }
    if (2 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 4);
        data->pd_mapping_type = SHARED_PD_0;
        data->pd_mapping_name = "VT45_FLOAT_PD";
    }
    if (2 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 5);
        data->pd_mapping_type = SHARED_PD_1;
        data->pd_mapping_name = "VT45_FLOAT_PD";
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "FWD1_FWD_PD";
    }
    if (3 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 1);
        data->pd_mapping_type = SUB_STAGE_0_PD;
        data->pd_mapping_name = "FWD1_RPF_PD";
    }
    if (3 < table->keys[0].size && 2 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 2);
        data->pd_mapping_type = SUB_STAGE_1_PD;
        data->pd_mapping_name = "FWD2_FWD_PD";
    }
    if (3 < table->keys[0].size && 3 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 3);
        data->pd_mapping_type = SUB_STAGE_1_PD;
        data->pd_mapping_name = "FWD2_RPF_PD";
    }
    if (3 < table->keys[0].size && 4 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 4);
        data->pd_mapping_type = SHARED_PD_0;
        data->pd_mapping_name = "FWD12_FLOAT_PD";
    }
    if (3 < table->keys[0].size && 5 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 5);
        data->pd_mapping_type = SHARED_PD_1;
        data->pd_mapping_name = "FWD12_FLOAT_PD";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_set(
    int unit)
{
    int stage_index_index;
    int sub_stage_index_index;
    dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *data, *default_data;
    dnxc_data_table_t *table;
    int module_index = dnx_data_module_kleap_stage_info;
    int submodule_index = dnx_data_kleap_stage_info_submodule_kleap_stage_info;
    int table_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage;
    SHR_FUNC_INIT_VARS(unit);

    table = &_dnxc_data[unit].modules[module_index].submodules[submodule_index].tables[table_index];
    
    table->flags |= DNXC_DATA_F_SUPPORTED;

    
    table->keys[0].size = 9;
    table->info_get.key_size[0] = 9;
    table->keys[1].size = 2;
    table->info_get.key_size[1] = 2;

    
    table->values[0].default_val = "DEFAULT_NAME";
    table->values[1].default_val = "0";
    table->values[2].default_val = "DBAL_FIELD_TYPE_DEF_EMPTY";
    table->values[3].default_val = "DBAL_FIELD_TYPE_DEF_EMPTY";
    table->values[4].default_val = "DBAL_NOF_TABLES";
    table->values[5].default_val = "DEFAULT_NAME";
    
    if (table->keys[0].size == 0 || table->info_get.key_size[0] == 0)
    {
        SHR_EXIT();
    }
    if (table->keys[1].size == 0 || table->info_get.key_size[1] == 0)
    {
        SHR_EXIT();
    }

    
    DNXC_DATA_ALLOC(table->data, dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t, (1 * (table->keys[0].size) * (table->keys[1].size) + 1  ), "data of dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage");

    
    default_data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, -1, -1);
    default_data->kleap_sub_stage_name = "DEFAULT_NAME";
    default_data->stage_index_in_pipe = 0;
    default_data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_EMPTY;
    default_data->dbal_stage_context_id = DBAL_FIELD_TYPE_DEF_EMPTY;
    default_data->dbal_stage_context_properties = DBAL_NOF_TABLES;
    default_data->literally_signal_structure_name = "DEFAULT_NAME";
    
    for (stage_index_index = 0; stage_index_index < table->keys[0].size; stage_index_index++)
    {
        for (sub_stage_index_index = 0; sub_stage_index_index < table->keys[1].size; sub_stage_index_index++)
        {
            data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, stage_index_index, sub_stage_index_index);
            sal_memcpy(data, default_data, table->size_of_values);
        }
    }
    
    if (0 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 0, 0);
        data->kleap_sub_stage_name = "VTT1";
        data->stage_index_in_pipe = 1;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_VT1_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_VT1_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_VT1_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "VT1_GENERAL_DATA";
    }
    if (1 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 0);
        data->kleap_sub_stage_name = "VTT2";
        data->stage_index_in_pipe = 2;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_VT2_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_VT2_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_VT2_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "VT2_GENERAL_DATA";
    }
    if (1 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 1, 1);
        data->kleap_sub_stage_name = "VTT3";
        data->stage_index_in_pipe = 3;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_VT3_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_VT3_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_VT3_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "VT2_GENERAL_DATA";
    }
    if (2 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 0);
        data->kleap_sub_stage_name = "VTT4";
        data->stage_index_in_pipe = 4;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_VT4_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_VT4_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_VT4_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "VT2_GENERAL_DATA";
    }
    if (2 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 2, 1);
        data->kleap_sub_stage_name = "VTT5";
        data->stage_index_in_pipe = 5;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_VT5_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_VT5_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "VT2_GENERAL_DATA";
    }
    if (3 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 0);
        data->kleap_sub_stage_name = "FWD1";
        data->stage_index_in_pipe = 6;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_FWD1_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_FWD1_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_FWD1_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "FWD1_GENERAL_DATA";
    }
    if (3 < table->keys[0].size && 1 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 3, 1);
        data->kleap_sub_stage_name = "FWD2";
        data->stage_index_in_pipe = 7;
        data->dbal_context_enum = DBAL_FIELD_TYPE_DEF_FWD2_CONTEXT_ID;
        data->dbal_stage_context_id = DBAL_FIELD_FWD2_CONTEXT_ID;
        data->dbal_stage_context_properties = DBAL_TABLE_INGRESS_IRPP_FWD2_CONTEXT_PROPERTIES;
        data->literally_signal_structure_name = "FWD2_GENERAL_DATA";
    }
    if (4 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 4, 0);
        data->kleap_sub_stage_name = "IPMF1";
    }
    if (5 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 5, 0);
        data->kleap_sub_stage_name = "IPMF1_INITIAL";
    }
    if (6 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 6, 0);
        data->kleap_sub_stage_name = "IPMF2";
    }
    if (7 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 7, 0);
        data->kleap_sub_stage_name = "IPMF3";
    }
    if (8 < table->keys[0].size && 0 < table->keys[1].size)
    {
        data = (dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_t *) dnxc_data_mgmt_table_data_get(unit, table, 8, 0);
        data->kleap_sub_stage_name = "EPMF";
    }

    
    SHR_IF_ERR_EXIT(dnxc_data_dyn_loader_table_value_set(unit, table));


exit:
    SHR_FUNC_EXIT;
}



shr_error_e
jr2_a0_data_kleap_stage_info_attach(
    int unit)
{
    dnxc_data_module_t *module = NULL;
    dnxc_data_submodule_t *submodule = NULL;
    dnxc_data_define_t *define = NULL;
    dnxc_data_feature_t *feature = NULL;
    dnxc_data_table_t *table = NULL;
    int module_index = dnx_data_module_kleap_stage_info;
    int submodule_index = -1, data_index = -1;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_REFERENCE(define);
    COMPILER_REFERENCE(feature);
    COMPILER_REFERENCE(table);
    COMPILER_REFERENCE(submodule);
    COMPILER_REFERENCE(data_index);
    COMPILER_REFERENCE(submodule_index);
    module = &_dnxc_data[unit].modules[module_index];
    
    submodule_index = dnx_data_kleap_stage_info_submodule_kleap_stage_info;
    submodule = &module->submodules[submodule_index];

    
    data_index = dnx_data_kleap_stage_info_kleap_stage_info_define_nof_kleap_stages;
    define = &submodule->defines[data_index];
    define->set = jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_nof_kleap_stages_set;

    

    
    data_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_set;
    data_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_kbr;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_kbr_set;
    data_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_pd;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_pd_set;
    data_index = dnx_data_kleap_stage_info_kleap_stage_info_table_info_per_stage_per_sub_stage;
    table = &submodule->tables[data_index];
    table->set = jr2_a0_dnx_data_kleap_stage_info_kleap_stage_info_info_per_stage_per_sub_stage_set;

    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE


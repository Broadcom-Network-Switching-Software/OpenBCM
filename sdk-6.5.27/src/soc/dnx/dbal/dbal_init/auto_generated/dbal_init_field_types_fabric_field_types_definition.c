
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_pipe_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_PIPE_ID];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_PIPE_ID", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.pipes.max_nof_pipes_get(unit)-1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_cell_ot_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_CELL_OT_SIZE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_CELL_OT_SIZE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_CELL_OT_SIZE_20_BITS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_CELL_OT_SIZE_24_BITS, "24_BITS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_CELL_OT_SIZE_20_BITS, "20_BITS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_CELL_OT_SIZE_24_BITS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_CELL_OT_SIZE_20_BITS].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_cell_gt_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_CELL_GT_SIZE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_CELL_GT_SIZE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_32_BITS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_24_BITS, "24_BITS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_15_BITS, "15_BITS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_20_BITS, "20_BITS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_32_BITS, "32_BITS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_24_BITS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_15_BITS].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_20_BITS].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_FABRIC_CELL_GT_SIZE_32_BITS].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_context_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_CONTEXT_ID];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_CONTEXT_ID", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.pipes.max_nof_contexts_get(unit)-1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_subcontext_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_SUBCONTEXT_ID];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_SUBCONTEXT_ID", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.pipes.max_nof_subcontexts_get(unit)-1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_link_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_LINK_ID];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_LINK_ID", dnx_data_fabric.dbal.nof_links_bits_nof_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.links.max_link_id_get(unit);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_priority_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_PRIORITY];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_PRIORITY", dnx_data_fabric.dbal.priority_bits_nof_get(unit), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.cell.nof_priorities_get(unit)-1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_fc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_DTQ_FC];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_DTQ_FC", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_DTQ_FC_RDF + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_FC_SHAPER, "SHAPER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP, "LP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP, "HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_FC_RDF, "RDF");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_FC_SHAPER].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_FC_LP].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_FC_HP].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_FC_RDF].value_from_mapping = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_ocb_fc_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_DTQ_OCB_FC];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_DTQ_OCB_FC", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_DTQ_OCB_FC_PDQ + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_OCB_FC_SHAPER, "SHAPER");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_OCB_FC_PDQ, "PDQ");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_OCB_FC_SHAPER].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_OCB_FC_PDQ].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_DTQ_MODE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_DTQ_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_HPMC_LPMC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_SINGLE_QUEUE, "SINGLE_QUEUE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_MC, "UC_MC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_HPMC_LPMC, "UC_HPMC_LPMC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_SINGLE_QUEUE].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_MC].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_HPMC_LPMC].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_global_shaper_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_DTQ_GLOBAL_SHAPER];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_DTQ_GLOBAL_SHAPER", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ0 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_FMC_DEV3, "FMC_DEV3");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_FUC_DEV2, "FUC_DEV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_TOTAL_DEV1, "TOTAL_DEV1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ1, "EGQ1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ0, "EGQ0");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_FMC_DEV3].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_FUC_DEV2].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_TOTAL_DEV1].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ1].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_FABRIC_DTQ_GLOBAL_SHAPER_EGQ0].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_pdq_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_PDQ_CONTEXT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_PDQ_CONTEXT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_LP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_HP, "LOCAL_0_HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_LP, "LOCAL_0_LP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP, "LOCAL_1_HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP, "LOCAL_1_LP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP, "FUC_DEV1_HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_LP, "FUC_DEV1_LP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP, "DEV2_HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP, "DEV2_LP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_HP, "FMC_DEV3_HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_LP, "FMC_DEV3_LP");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_HP].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_LP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_LP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_HP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_LP].value_from_mapping = 7;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_HP].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_LP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP].value_from_mapping = 4;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_LP].value_from_mapping = 5;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP].value_from_mapping = 6;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP].value_from_mapping = 7;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_HP].value_from_mapping = 8;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_LP].value_from_mapping = 9;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_HP].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_0_LP].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_HP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_LOCAL_1_LP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_HP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FUC_DEV1_LP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_HP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_DEV2_LP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_HP].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_CONTEXT_FMC_DEV3_LP].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_pdq_ocb_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_PDQ_OCB_CONTEXT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_PDQ_OCB_CONTEXT", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FMC_DEV3 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_0, "LOCAL_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1, "LOCAL_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FUC_DEV1, "FUC_DEV1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2, "DEV2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FMC_DEV3, "FMC_DEV3");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_0].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FUC_DEV1].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FMC_DEV3].value_from_mapping = 3;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_0].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FUC_DEV1].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2].value_from_mapping = 3;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FMC_DEV3].value_from_mapping = 4;
        }
    }
    else if (DBAL_IS_Q2A_A0 || DBAL_IS_Q2A_B0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_0].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_LOCAL_1].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FUC_DEV1].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_DEV2].is_invalid_value_from_mapping = TRUE;
            enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_OCB_CONTEXT_FMC_DEV3].is_invalid_value_from_mapping = TRUE;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_pdq_s2d_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_PDQ_S2D_CONTEXT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_PDQ_S2D_CONTEXT", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_PDQ_S2D_CONTEXT_LP + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_S2D_CONTEXT_HP, "HP");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PDQ_S2D_CONTEXT_LP, "LP");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_S2D_CONTEXT_HP].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_PDQ_S2D_CONTEXT_LP].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_pcp_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_PCP_MODE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_PCP_MODE", 2, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING, "NO_PACKING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PCP_MODE_SIMPLE, "SIMPLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS, "CONTINUOUS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_PCP_MODE_SIMPLE].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS].value_from_mapping = 2;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_wfq_fifo_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_WFQ_FIFO];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_WFQ_FIFO", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_MESH_MC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_0, "FABRIC_PIPE_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_1, "FABRIC_PIPE_1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_2, "FABRIC_PIPE_2");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_LOCAL_UC, "LOCAL_UC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_MESH_MC, "MESH_MC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_0].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_1].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_FABRIC_PIPE_2].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_LOCAL_UC].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_FABRIC_WFQ_FIFO_MESH_MC].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_load_balancing_count_mode_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_LOAD_BALANCING_COUNT_MODE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_LOAD_BALANCING_COUNT_MODE", 1, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_CLOCKS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_TRANSACTIONS, "TRANSACTIONS");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_CLOCKS, "CLOCKS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_TRANSACTIONS].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_CLOCKS].value_from_mapping = 1;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_load_balancing_context_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_LOAD_BALANCING_CONTEXT];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_LOAD_BALANCING_CONTEXT", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.links.load_balancing_profile_id_max_get(unit);
    cur_field_type_param->min_value = dnx_data_fabric.links.load_balancing_profile_id_min_get(unit);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_force_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_FORCE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_FORCE", 3, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_1 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_FORCE_DISABLE, "DISABLE");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_FORCE_FABRIC, "FABRIC");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_0, "CORE_0");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_1, "CORE_1");
    }
    if (DBAL_IS_J2C_A0 || DBAL_IS_J2X_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_DISABLE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_FABRIC].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_0].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_1].is_invalid_value_from_mapping = TRUE;
        }
    }
    else if (DBAL_IS_JR2_A0 || DBAL_IS_JR2_B0 || DBAL_IS_J2P_A0)
    {
        dbal_init_field_type_default_restrict_values(cur_field_type_param);
        {
            dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_DISABLE].value_from_mapping = 0;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_FABRIC].value_from_mapping = 1;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_0].value_from_mapping = 2;
            enum_info[DBAL_ENUM_FVAL_FABRIC_FORCE_CORE_1].value_from_mapping = 4;
        }
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_fabric_field_types_definition_fabric_sr_cell_instance_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FABRIC_SR_CELL_INSTANCE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = NULL;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FABRIC_SR_CELL_INSTANCE", utilex_log2_round_up(dnx_data_fabric.cell.sr_cell_nof_instances_get(unit)*dnx_data_device.general.nof_cores_get(unit)), is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    cur_field_type_param->max_value = dnx_data_fabric.cell.sr_cell_nof_instances_get(unit)*dnx_data_device.general.nof_cores_get(unit)-1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_fabric_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_pipe_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_cell_ot_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_cell_gt_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_context_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_subcontext_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_link_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_priority_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_fc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_ocb_fc_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_dtq_global_shaper_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_pdq_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_pdq_ocb_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_pdq_s2d_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_pcp_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_wfq_fifo_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_load_balancing_count_mode_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_load_balancing_context_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_force_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_fabric_field_types_definition_fabric_sr_cell_instance_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}


/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_coe_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRTTCAM_KEY_VP_COE];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = "standard_1";
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRTTCAM_KEY_VP_COE", 14, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VID;
    cur_field_types_info->struct_field_info[0].length = 6;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PTC_ATTRIBUTE;
    cur_field_types_info->struct_field_info[1].length = 8;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_pon_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRTTCAM_KEY_VP_PON];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = "standard_1";
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRTTCAM_KEY_VP_PON", 13, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_PON_TAG_PON_SUBPORT_ID;
    cur_field_types_info->struct_field_info[0].length = 4;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[1].length = dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___280_get(unit);
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ZERO_PADDING;
    cur_field_types_info->struct_field_info[2].length = dnx_data_dnx2_mdb_app_db.fields.numeric_mdb_field___281_get(unit);
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_spa_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRTTCAM_KEY_VP_SPA];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = "standard_1";
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRTTCAM_KEY_VP_SPA", 16, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_types_info->nof_struct_fields = 1;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_SYSTEM_PORT_AGGREGATE;
    cur_field_types_info->struct_field_info[0].length = 16;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prt_kbr_info_half_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRT_KBR_INFO_HALF];
    int is_valid = TRUE;
    uint8 is_skip_field_type_init = FALSE;
    char * current_image = "standard_1";
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "PRT_KBR_INFO_HALF", 17, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_UINT32, 1, current_image, &is_skip_field_type_init));
    if (is_skip_field_type_init)
    {
        SHR_EXIT();
    }
    cur_field_type_param->nof_childs = 3;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "PRTTCAM_KEY_VP_COE");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "PRTTCAM_KEY_VP_PON");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 2, "PRTTCAM_KEY_VP_SPA");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 4;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[1].encode_param1 = 10;
    }
    {
        cur_field_type_param->childs[2].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[2].encode_param1 = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_coe_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_pon_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_spa_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prt_kbr_info_half_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}


/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_virtual_tcam_field_types_prttcam_key_vp_coe_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_PRTTCAM_KEY_VP_COE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PRTTCAM_KEY_VP_COE" , 14 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , TRUE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_VID;
    cur_field_types_info->struct_field_info[0].length = 6;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PTC_ATTRIBUTE;
    cur_field_types_info->struct_field_info[1].length = 8;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PRTTCAM_KEY_VP_PON" , 13 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , TRUE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_PON_TAG_PON_SUBPORT_ID;
    cur_field_types_info->struct_field_info[0].length = 3;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_PTC;
    cur_field_types_info->struct_field_info[1].length = dnx_data_mdb_app_db.fields.numeric_mdb_field___201_get(unit);
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_ZERO_PADDING;
    cur_field_types_info->struct_field_info[2].length = dnx_data_mdb_app_db.fields.numeric_mdb_field___202_get(unit);
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PRTTCAM_KEY_VP_SPA" , 16 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , TRUE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 1;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_SYSTEM_PORT_AGGREGATE;
    cur_field_types_info->struct_field_info[0].length = 16;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
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
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "PRT_KBR_INFO_HALF" , 17 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , TRUE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 3 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "PRTTCAM_KEY_VP_COE", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "PRTTCAM_KEY_VP_PON", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "PRTTCAM_KEY_VP_SPA", sizeof(cur_field_type_param->childs[2].name));
    cur_field_type_param->nof_childs = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 4;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 10;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
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


/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_kleap_stage_selector_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_KLEAP_STAGE_SELECTOR];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "KLEAP_STAGE_SELECTOR" , 1 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->enums, 0x0, 2 * sizeof(dbal_db_enum_info_struct_t));
    sal_strncpy(cur_field_type_param->enums[0].name_from_interface, "STAGE_A", sizeof(cur_field_type_param->enums[0].name_from_interface));
    sal_strncpy(cur_field_type_param->enums[1].name_from_interface, "STAGE_B", sizeof(cur_field_type_param->enums[1].name_from_interface));
    cur_field_type_param->nof_enum_vals = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    cur_field_type_param->enums[0].value_from_mapping = 0;
    cur_field_type_param->enums[1].value_from_mapping = 1;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_ffc_key_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FFC_KEY_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FFC_KEY_OFFSET" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ffc_field_size_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FFC_FIELD_SIZE];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FFC_FIELD_SIZE" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ffc_field_offset_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FFC_FIELD_OFFSET];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FFC_FIELD_OFFSET" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ffc_index_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FFC_INDEX];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FFC_INDEX" , 32 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD12_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD12_FFC_RELATIVE_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD12_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD12_FFC_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD12_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD12_FFC_LITERALLY" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD12_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD12_FFC_LAYER_RECORD" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 5;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 6;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD12_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD12_FFC_INSTRUCTION" , 29 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "FWD12_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "FWD12_FFC_HEADER", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "FWD12_FFC_LITERALLY", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "FWD12_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT1_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT1_FFC_RELATIVE_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT1_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT1_FFC_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT1_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT1_FFC_LITERALLY" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT1_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT1_FFC_LAYER_RECORD" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 5;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 6;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT1_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT1_FFC_INSTRUCTION" , 29 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "VT1_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "VT1_FFC_HEADER", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "VT1_FFC_LITERALLY", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "VT1_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT23_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT23_FFC_RELATIVE_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT23_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT23_FFC_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT23_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT23_FFC_LITERALLY" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT23_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT23_FFC_LAYER_RECORD" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 5;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 6;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT23_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT23_FFC_INSTRUCTION" , 29 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "VT23_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "VT23_FFC_HEADER", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "VT23_FFC_LITERALLY", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "VT23_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT45_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT45_FFC_RELATIVE_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT45_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT45_FFC_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT45_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT45_FFC_LITERALLY" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT45_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT45_FFC_LAYER_RECORD" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 5;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 6;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VT45_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "VT45_FFC_INSTRUCTION" , 29 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "VT45_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "VT45_FFC_HEADER", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "VT45_FFC_LITERALLY", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "VT45_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_FFC_RELATIVE_HEADER" , 28 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_FFC_HEADER" , 28 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_FFC_LITERALLY" , 26 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 13;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_FFC_LAYER_RECORD" , 28 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 6;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 6;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF1_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF1_FFC_INSTRUCTION" , 30 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "IPMF1_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "IPMF1_FFC_HEADER", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "IPMF1_FFC_LITERALLY", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "IPMF1_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_ipmf2_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF2_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF2_FFC_LITERALLY" , 24 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 11;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ipmf2_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF2_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF2_FFC_INSTRUCTION" , 24 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 1 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "IPMF2_FFC_LITERALLY", sizeof(cur_field_type_param->childs[0].name));
    cur_field_type_param->nof_childs = 1;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_ipmf3_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF3_FFC_LITERALLY" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_ipmf3_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF3_FFC_LAYER_RECORD" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 6;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 5;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_ipmf3_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_IPMF3_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "IPMF3_FFC_INSTRUCTION" , 29 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 2 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "IPMF3_FFC_LITERALLY", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "IPMF3_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[1].name));
    cur_field_type_param->nof_childs = 2;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_FFC_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 10;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 1;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_FFC_LITERALLY" , 25 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 12;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_FFC_LAYER_RECORD" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 10;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 1;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_FFC_RELATIVE_HEADER" , 27 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 4;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_KEY_OFFSET;
    cur_field_types_info->struct_field_info[0].length = 8;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[1].length = 5;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[2].length = 10;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[3].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[3].offset = 1;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_EPMF_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "EPMF_FFC_INSTRUCTION" , 29 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "EPMF_FFC_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "EPMF_FFC_LITERALLY", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "EPMF_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "EPMF_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_relative_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS_FFC_RELATIVE_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS_FFC_RELATIVE_HEADER" , 20 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[0].length = 5;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[1].length = 12;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[2].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_header_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS_FFC_HEADER];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS_FFC_HEADER" , 20 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[0].length = 5;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[1].length = 12;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[2].length = 3;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_literally_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS_FFC_LITERALLY];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS_FFC_LITERALLY" , 18 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 2;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[0].length = 5;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[1].length = 13;
    
    
    
    
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
_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_layer_record_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS_FFC_LAYER_RECORD];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS_FFC_LAYER_RECORD" , 20 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_STRUCTURE , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_types_info->nof_struct_fields = 3;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    cur_field_types_info->struct_field_info[0].length = 5;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_FFC_FIELD_OFFSET;
    cur_field_types_info->struct_field_info[1].length = 6;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_FFC_INDEX;
    cur_field_types_info->struct_field_info[2].length = 3;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    cur_field_types_info->struct_field_info[2].offset = 6;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_instruction_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_L4OPS_FFC_INSTRUCTION];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "L4OPS_FFC_INSTRUCTION" , 22 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_HEX , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->childs, 0x0, 4 * sizeof(dbal_db_child_field_info_struct_t));
    sal_strncpy(cur_field_type_param->childs[0].name, "L4OPS_FFC_RELATIVE_HEADER", sizeof(cur_field_type_param->childs[0].name));
    sal_strncpy(cur_field_type_param->childs[1].name, "L4OPS_FFC_HEADER", sizeof(cur_field_type_param->childs[1].name));
    sal_strncpy(cur_field_type_param->childs[2].name, "L4OPS_FFC_LITERALLY", sizeof(cur_field_type_param->childs[2].name));
    sal_strncpy(cur_field_type_param->childs[3].name, "L4OPS_FFC_LAYER_RECORD", sizeof(cur_field_type_param->childs[3].name));
    cur_field_type_param->nof_childs = 4;
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    {
        
        sal_strncpy(cur_field_type_param->childs[1].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[2].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[2].encode_param1 = 8;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[3].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    {
        
        sal_strncpy(cur_field_type_param->childs[0].encode_type, "PREFIX" , DBAL_MAX_SHORT_STRING_LENGTH);
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_kleap_field_types_definition_fwd12_kbr_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD12_KBR_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD12_KBR_ID" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_UINT32 , 0 , 1 , FALSE ));
    
    
    
    
    
    sal_memset(cur_field_type_param->define, 0x0, 4 * sizeof(dbal_db_defines_info_struct_t));
    sal_strncpy(cur_field_type_param->define[0].define_name, "KBP_0", sizeof(cur_field_type_param->define[0].define_name));
    cur_field_type_param->define[0].define_value = 4;
    sal_strncpy(cur_field_type_param->define[1].define_name, "KBP_1", sizeof(cur_field_type_param->define[1].define_name));
    cur_field_type_param->define[1].define_value = 5;
    sal_strncpy(cur_field_type_param->define[2].define_name, "KBP_2", sizeof(cur_field_type_param->define[2].define_name));
    cur_field_type_param->define[2].define_value = 16;
    sal_strncpy(cur_field_type_param->define[3].define_name, "KBP_3", sizeof(cur_field_type_param->define[3].define_name));
    cur_field_type_param->define[3].define_value = 17;
    cur_field_type_param->nof_defined_vals = 4;
    
    
    
    
    cur_field_type_param->max_value = 17;
    
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = DBAL_DB_INVALID;
    
    cur_field_type_param->nof_illegal_value = 0;
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_kleap_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_kleap_stage_selector_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ffc_key_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ffc_field_size_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ffc_field_offset_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ffc_index_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_fwd12_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt1_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt23_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_vt45_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf1_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf2_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf2_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf3_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf3_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_ipmf3_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_epmf_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_relative_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_header_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_literally_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_layer_record_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_l4ops_ffc_instruction_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_kleap_field_types_definition_fwd12_kbr_id_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}

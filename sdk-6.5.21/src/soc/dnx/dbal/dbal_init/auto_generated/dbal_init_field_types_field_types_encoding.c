
/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

shr_error_e
_dbal_init_field_types_field_types_encoding_destination_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DESTINATION_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DESTINATION_ENCODING" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_PORT_ID].name_from_interface, "PORT_ID", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_PORT_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_MC_ID].name_from_interface, "MC_ID", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_MC_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_FLOW_ID].name_from_interface, "FLOW_ID", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_FLOW_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_LAG_ID].name_from_interface, "LAG_ID", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_LAG_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_INGRESS_TRAP_DEST].name_from_interface, "INGRESS_TRAP_DEST", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_INGRESS_TRAP_DEST].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_FEC].name_from_interface, "FEC", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_FEC].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_PORT_ID].value_from_mapping = 24;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_MC_ID].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_FLOW_ID].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_LAG_ID].value_from_mapping = 25;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_INGRESS_TRAP_DEST].value_from_mapping = 15;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_ENCODING_FEC].value_from_mapping = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_field_types_encoding_destination_jr1_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_DESTINATION_JR1_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "DESTINATION_JR1_ENCODING" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 6;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_PORT_ID].name_from_interface, "PORT_ID", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_PORT_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_MC_ID_JR1].name_from_interface, "MC_ID_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_MC_ID_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FLOW_ID_JR1].name_from_interface, "FLOW_ID_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FLOW_ID_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_LAG_ID].name_from_interface, "LAG_ID", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_LAG_ID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FEC_JR1].name_from_interface, "FEC_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FEC_JR1].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_INGRESS_TRAP_DEST_JR1].name_from_interface, "INGRESS_TRAP_DEST_JR1", sizeof(enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_INGRESS_TRAP_DEST_JR1].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_PORT_ID].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_MC_ID_JR1].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FLOW_ID_JR1].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_LAG_ID].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FEC_JR1].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_INGRESS_TRAP_DEST_JR1].value_from_mapping = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_field_types_encoding_inlif_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_INLIF_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "INLIF_ENCODING" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_P2P].name_from_interface, "AC_P2P", sizeof(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_P2P].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_MP].name_from_interface, "AC_MP", sizeof(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_MP].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_P2P_LARGE].name_from_interface, "AC_P2P_LARGE", sizeof(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_P2P_LARGE].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_MP_LARGE].name_from_interface, "AC_MP_LARGE", sizeof(enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_MP_LARGE].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_P2P].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_MP].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_P2P_LARGE].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_INLIF_ENCODING_AC_MP_LARGE].value_from_mapping = 0;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_field_types_encoding_fwd_dest_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FWD_DEST_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FWD_DEST_ENCODING" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 8;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_INVALID].name_from_interface, "INVALID", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_INVALID].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_OUTLIF].name_from_interface, "OUTLIF", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_OUTLIF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_EEI].name_from_interface, "EEI", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_EEI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_OUTLIF_STAT].name_from_interface, "OUTLIF_STAT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_OUTLIF_STAT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_EEI_STAT].name_from_interface, "EEI_STAT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_EEI_STAT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DESTINATION].name_from_interface, "DESTINATION", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DESTINATION].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DESTINATION_STAT].name_from_interface, "DESTINATION_STAT", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DESTINATION_STAT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DOUBLE_OUTLIF].name_from_interface, "DOUBLE_OUTLIF", sizeof(enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DOUBLE_OUTLIF].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_INVALID].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_OUTLIF].value_from_mapping = 5;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_EEI].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_OUTLIF_STAT].value_from_mapping = 6;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_EEI_STAT].value_from_mapping = 4;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DESTINATION].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DESTINATION_STAT].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_FWD_DEST_ENCODING_DOUBLE_OUTLIF].value_from_mapping = 7;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_field_types_encoding_super_fec_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_SUPER_FEC_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "SUPER_FEC_ENCODING" , 5 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 4;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_NO_PROTECTION].name_from_interface, "NO_PROTECTION", sizeof(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_NO_PROTECTION].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_NO_PROTECTION_W_STAT].name_from_interface, "NO_PROTECTION_W_STAT", sizeof(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_NO_PROTECTION_W_STAT].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_W_PROTECTION].name_from_interface, "W_PROTECTION", sizeof(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_W_PROTECTION].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_W_PROTECTION_W_STAT].name_from_interface, "W_PROTECTION_W_STAT", sizeof(enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_W_PROTECTION_W_STAT].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_NO_PROTECTION].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_NO_PROTECTION_W_STAT].value_from_mapping = 1;
        
        enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_W_PROTECTION].value_from_mapping = 2;
        
        enum_info[DBAL_ENUM_FVAL_SUPER_FEC_ENCODING_W_PROTECTION_W_STAT].value_from_mapping = 3;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_field_types_encoding_fodo_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FODO_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_type_param, cur_field_types_info, "FODO_ENCODING" , 8 , is_valid , FALSE , DBAL_FIELD_PRINT_TYPE_ENUM , 0 , 1 , FALSE ));
    
    
    
    
    cur_field_type_param->nof_enum_vals = 3;
    
    sal_memset(cur_field_type_param->enums, 0x0, cur_field_type_param->nof_enum_vals * sizeof(dbal_db_enum_info_struct_t));
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_VSI].name_from_interface, "VSI", sizeof(enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_VSI].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_VRF].name_from_interface, "VRF", sizeof(enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_VRF].name_from_interface));
        sal_strncpy(enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_BIER_MPLS].name_from_interface, "BIER_MPLS", sizeof(enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_BIER_MPLS].name_from_interface));
    }
    
    
    
    
    cur_field_type_param->nof_illegal_value = 0;
    cur_field_type_param->min_value = DBAL_DB_INVALID;
    cur_field_type_param->max_value = DBAL_DB_INVALID;
    cur_field_type_param->const_value = DBAL_DB_INVALID;
    cur_field_type_param->default_val = 0;
    cur_field_type_param->default_val_valid = FALSE;
    
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        
        enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_VSI].value_from_mapping = 0;
        
        enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_VRF].value_from_mapping = 3;
        
        enum_info[DBAL_ENUM_FVAL_FODO_ENCODING_BIER_MPLS].value_from_mapping = 191;
    }
    
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_field_types_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_destination_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_destination_jr1_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_inlif_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_fwd_dest_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_super_fec_encoding_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_fodo_encoding_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}

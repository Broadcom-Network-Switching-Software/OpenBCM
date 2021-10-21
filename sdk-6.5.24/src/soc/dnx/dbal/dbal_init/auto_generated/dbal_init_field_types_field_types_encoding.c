
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DESTINATION_ENCODING", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DESTINATION_ENCODING_FEC + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_ENCODING_PORT_ID, "PORT_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_ENCODING_MC_ID, "MC_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_ENCODING_FLOW_ID, "FLOW_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_ENCODING_LAG_ID, "LAG_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_ENCODING_INGRESS_TRAP_DEST, "INGRESS_TRAP_DEST");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_ENCODING_FEC, "FEC");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "DESTINATION_JR1_ENCODING", 5, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_INGRESS_TRAP_DEST_JR1 + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_PORT_ID, "PORT_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_MC_ID_JR1, "MC_ID_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FLOW_ID_JR1, "FLOW_ID_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_LAG_ID, "LAG_ID");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_FEC_JR1, "FEC_JR1");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_DESTINATION_JR1_ENCODING_INGRESS_TRAP_DEST_JR1, "INGRESS_TRAP_DEST_JR1");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
_dbal_init_field_types_field_types_encoding_fodo_encoding_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FODO_ENCODING];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FODO_ENCODING", 8, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, FALSE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_FODO_ENCODING_BIER_MPLS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FODO_ENCODING_VSI, "VSI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FODO_ENCODING_VRF, "VRF");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_FODO_ENCODING_BIER_MPLS, "BIER_MPLS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
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
    SHR_IF_ERR_EXIT(_dbal_init_field_types_field_types_encoding_fodo_encoding_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}

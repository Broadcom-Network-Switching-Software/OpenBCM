
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_field_types_vretps___prefix_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VRETPS___PREFIX_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VRETPS___PREFIX_ID", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_TTL_MODEL_MPLS_FHEI + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_FORWARD_DOMAIN_VRF_ENCODING, "FORWARD_DOMAIN_VRF_ENCODING");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_TTL_IDX_MPLS_FHEI, "TTL_IDX_MPLS_FHEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_QOS_MODEL_MPLS_FHEI, "QOS_MODEL_MPLS_FHEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_QOS_IDX_MPLS_FHEI, "QOS_IDX_MPLS_FHEI");
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_TTL_MODEL_MPLS_FHEI, "TTL_MODEL_MPLS_FHEI");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_FORWARD_DOMAIN_VRF_ENCODING].value_from_mapping = 0;
        enum_info[DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_TTL_IDX_MPLS_FHEI].value_from_mapping = 1;
        enum_info[DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_QOS_MODEL_MPLS_FHEI].value_from_mapping = 2;
        enum_info[DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_QOS_IDX_MPLS_FHEI].value_from_mapping = 3;
        enum_info[DBAL_ENUM_FVAL_VRETPS___PREFIX_ID_TTL_MODEL_MPLS_FHEI].value_from_mapping = 4;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_field_types_vrinlif___prefix_id_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_VRINLIF___PREFIX_ID];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "VRINLIF___PREFIX_ID", 4, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ENUM, 1, TRUE));
    cur_field_type_param->nof_enum_vals = DBAL_ENUM_FVAL_VRINLIF___PREFIX_ID_VSI_P2P_MSBS + 1;
    DBAL_INIT_FIELD_TYPES_ENUMS_CLEAR(cur_field_type_param);
    {
        DBAL_INIT_FIELD_TYPES_ENUM_NAME_SET(cur_field_type_param, DBAL_ENUM_FVAL_VRINLIF___PREFIX_ID_VSI_P2P_MSBS, "VSI_P2P_MSBS");
    }
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        dbal_db_enum_info_struct_t * enum_info = cur_field_type_param->enums;
        enum_info[DBAL_ENUM_FVAL_VRINLIF___PREFIX_ID_VSI_P2P_MSBS].value_from_mapping = 0;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_arr_field_types_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_field_types_vretps___prefix_id_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_arr_field_types_vrinlif___prefix_id_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}

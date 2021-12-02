
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * 
 */

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_mc_rpf_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_MC_RPF];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_MC_RPF", 24, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MC_RPF_MODE;
    cur_field_types_info->struct_field_info[0].length = 3;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[1].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_eei_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_EEI];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_EEI", 45, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[0].length = 24;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[1].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_LIF0];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_LIF0", 43, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_0;
    cur_field_types_info->struct_field_info[0].length = 22;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[1].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_17bit_lif1_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_LIF0_17BIT_LIF1];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_LIF0_17BIT_LIF1", 60, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_1;
    cur_field_types_info->struct_field_info[0].length = 22;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_0;
    cur_field_types_info->struct_field_info[1].length = 17;
    cur_field_types_info->struct_field_info[1].arr_prefix_table_entry_value = dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF_get(unit);
    cur_field_types_info->struct_field_info[1].arr_prefix_table_entry_width = 24;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[2].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_htm_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_LIF0_HTM];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_LIF0_HTM", 62, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_HIERARCHICAL_TM_FLOW;
    cur_field_types_info->struct_field_info[0].length = 19;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_0;
    cur_field_types_info->struct_field_info[1].length = 22;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[2].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_lif1_mc_rpf_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_LIF0_LIF1_MC_RPF];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_LIF0_LIF1_MC_RPF", 66, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 4;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MC_RPF_MODE;
    cur_field_types_info->struct_field_info[0].length = 1;
    cur_field_types_info->struct_field_info[0].arr_prefix_table_entry_value = dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_MC_RPF_MODE_MC_RPF_MODE_EXPLICIT_get(unit);
    cur_field_types_info->struct_field_info[0].arr_prefix_table_entry_width = 24;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_1;
    cur_field_types_info->struct_field_info[1].length = 22;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_0;
    cur_field_types_info->struct_field_info[2].length = 22;
    cur_field_types_info->struct_field_info[3].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[3].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_mc_rpf_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_DESTINATION_LIF0_MC_RPF];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_DESTINATION_LIF0_MC_RPF", 46, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_MC_RPF_MODE;
    cur_field_types_info->struct_field_info[0].length = 3;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_0;
    cur_field_types_info->struct_field_info[1].length = 22;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[2].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_jr1_destination_eei_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_JR1_DESTINATION_EEI];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_JR1_DESTINATION_EEI", 45, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 2;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[0].length = 24;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[1].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_jr1_destination_eth_rif_arp_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_JR1_DESTINATION_ETH_RIF_ARP];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_JR1_DESTINATION_ETH_RIF_ARP", 59, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_STRUCTURE, 1, TRUE));
    cur_field_types_info->nof_struct_fields = 3;
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_alloc(unit, cur_field_types_info));
    cur_field_types_info->struct_field_info[0].struct_field_id = DBAL_FIELD_GLOBAL_OUT_LIF_0;
    cur_field_types_info->struct_field_info[0].length = 16;
    cur_field_types_info->struct_field_info[0].arr_prefix_table_entry_value = dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_OUTLIF_ETH_RIF_get(unit);
    cur_field_types_info->struct_field_info[0].arr_prefix_table_entry_width = 24;
    cur_field_types_info->struct_field_info[1].struct_field_id = DBAL_FIELD_EEI;
    cur_field_types_info->struct_field_info[1].length = 22;
    cur_field_types_info->struct_field_info[1].arr_prefix_table_entry_value = dnx_data_dnx2_arr.prefix.IRPP_FEC_ENTRY_FORMAT___prefix_EEI_W_OUTLIF_get(unit);
    cur_field_types_info->struct_field_info[1].arr_prefix_table_entry_width = 24;
    cur_field_types_info->struct_field_info[2].struct_field_id = DBAL_FIELD_DESTINATION;
    cur_field_types_info->struct_field_info[2].length = 21;
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_no_protection_no_stats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_ENTRY_NO_PROTECTION_NO_STATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_ENTRY_NO_PROTECTION_NO_STATS", 73, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, TRUE));
    cur_field_type_param->nof_childs = 9;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "FEC_DESTINATION_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "FEC_DESTINATION_EEI");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 2, "FEC_DESTINATION_LIF0");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 3, "FEC_DESTINATION_LIF0_17BIT_LIF1");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 4, "FEC_DESTINATION_LIF0_HTM");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 5, "FEC_DESTINATION_LIF0_LIF1_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 6, "FEC_DESTINATION_LIF0_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 7, "FEC_JR1_DESTINATION_EEI");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 8, "FEC_JR1_DESTINATION_ETH_RIF_ARP");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[1].encode_param1 = 33554432;
    }
    {
        cur_field_type_param->childs[2].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[2].encode_param1 = 704643072;
    }
    {
        cur_field_type_param->childs[3].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[3].encode_param1 = 5632;
    }
    {
        cur_field_type_param->childs[4].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[4].encode_param1 = 1248;
    }
    {
        cur_field_type_param->childs[5].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[5].encode_param1 = 8;
    }
    {
        cur_field_type_param->childs[6].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[6].encode_param1 = 100663296;
    }
    {
        cur_field_type_param->childs[7].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[7].encode_param1 = 12582912;
    }
    {
        cur_field_type_param->childs[8].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[8].encode_param1 = 10240;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_no_protection_w_2_stats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_ENTRY_NO_PROTECTION_W_2_STATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_ENTRY_NO_PROTECTION_W_2_STATS", 48, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, TRUE));
    cur_field_type_param->nof_childs = 4;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "FEC_DESTINATION_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "FEC_DESTINATION_EEI");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 2, "FEC_DESTINATION_LIF0");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 3, "FEC_DESTINATION_LIF0_MC_RPF");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[1].encode_param1 = 1;
    }
    {
        cur_field_type_param->childs[2].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[2].encode_param1 = 21;
    }
    {
        cur_field_type_param->childs[3].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[3].encode_param1 = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_w_protection_no_stats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_ENTRY_W_PROTECTION_NO_STATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_ENTRY_W_PROTECTION_NO_STATS", 64, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, TRUE));
    cur_field_type_param->nof_childs = 7;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "FEC_DESTINATION_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "FEC_DESTINATION_EEI");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 2, "FEC_DESTINATION_LIF0");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 3, "FEC_DESTINATION_LIF0_17BIT_LIF1");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 4, "FEC_DESTINATION_LIF0_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 5, "FEC_JR1_DESTINATION_EEI");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 6, "FEC_JR1_DESTINATION_ETH_RIF_ARP");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[1].encode_param1 = 65536;
    }
    {
        cur_field_type_param->childs[2].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[2].encode_param1 = 1376256;
    }
    {
        cur_field_type_param->childs[3].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[3].encode_param1 = 11;
    }
    {
        cur_field_type_param->childs[4].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[4].encode_param1 = 196608;
    }
    {
        cur_field_type_param->childs[5].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[5].encode_param1 = 24576;
    }
    {
        cur_field_type_param->childs[6].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[6].encode_param1 = 20;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_w_protection_w_1_stats_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    dbal_field_types_basic_info_t * cur_field_types_info = field_types_info[DBAL_FIELD_TYPE_DEF_FEC_ENTRY_W_PROTECTION_W_1_STATS];
    int is_valid = TRUE;
    SHR_FUNC_INIT_VARS(unit);
    dbal_init_field_type_db_struct_clear(unit, cur_field_type_param);
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_general_info_set(unit, cur_field_types_info, "FEC_ENTRY_W_PROTECTION_W_1_STATS", 51, is_valid, FALSE, DBAL_FIELD_PRINT_TYPE_ARRAY32, 1, TRUE));
    cur_field_type_param->nof_childs = 5;
    DBAL_INIT_FIELD_TYPES_CHILDS_CLEAR(cur_field_type_param);
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 0, "FEC_DESTINATION_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 1, "FEC_DESTINATION_EEI");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 2, "FEC_DESTINATION_LIF0");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 3, "FEC_DESTINATION_LIF0_MC_RPF");
    DBAL_INIT_FIELD_TYPES_CHILD_NAME_SET(cur_field_type_param, 4, "FEC_JR1_DESTINATION_EEI");
    dbal_init_field_type_default_restrict_values(cur_field_type_param);
    {
        cur_field_type_param->childs[0].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[0].encode_param1 = 0;
    }
    {
        cur_field_type_param->childs[1].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[1].encode_param1 = 8;
    }
    {
        cur_field_type_param->childs[2].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[2].encode_param1 = 168;
    }
    {
        cur_field_type_param->childs[3].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[3].encode_param1 = 24;
    }
    {
        cur_field_type_param->childs[4].encode_type = DBAL_VALUE_FIELD_ENCODE_PREFIX;
        cur_field_type_param->childs[4].encode_param1 = 3;
    }
    SHR_IF_ERR_EXIT(dbal_db_init_field_type_add(unit, cur_field_type_param, cur_field_types_info));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_init(
int unit,
field_type_db_struct_t * cur_field_type_param,
dbal_field_types_basic_info_t ** field_types_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_mc_rpf_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_eei_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_17bit_lif1_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_htm_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_lif1_mc_rpf_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_destination_lif0_mc_rpf_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_jr1_destination_eei_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_jr1_destination_eth_rif_arp_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_no_protection_no_stats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_no_protection_w_2_stats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_w_protection_no_stats_init(unit, cur_field_type_param, field_types_info));
    SHR_IF_ERR_EXIT(_dbal_init_field_types_auto_generated_standard_1_standard_1_fec_field_types_definition_fec_entry_w_protection_w_1_stats_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FUNC_EXIT;
}

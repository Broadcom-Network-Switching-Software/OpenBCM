
/*! \file diag_dnx_dbal.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_DBAL_H_INCLUDED
#define DIAG_DNX_DBAL_H_INCLUDED

/*************
 * INCLUDES  *
 */

/*************
 *  DEFINES  *
 */

/*************
 *  MACROES  *
 */

/** this Macro validate that the table was initialized before performing any action on the table */
#define DIAG_DBAL_TABLE_STATUS_VALIDATE(table_id)                                                                   \
{                                                                                                                   \
    dbal_table_status_e status;                                                                                     \
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &status));                              \
    {                                                                                                               \
        if ((status == DBAL_TABLE_NOT_INITIALIZED))                                                                 \
        {                                                                                                           \
            LOG_CLI((BSL_META("\tTABLE %s not initialized \n"), dbal_logical_table_to_string(unit, table_id)));     \
            SHR_EXIT();                                                                                             \
        }                                                                                                           \
    }                                                                                                               \
}

/*
 * helper macros for performing the dbal action according to the field type & prameters
 */
#define DBAL_ENTRY_COMMIT_KEY_FIELD_WITH_MASK(unit, entry_handle_id, field_id, sand_arg)      \
do                                                                                  \
{                                                                                   \
    dbal_field_types_defs_e field_type;                                             \
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));       \
                                                                                    \
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)                                   \
    {                                                                               \
      dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id,         \
                                          SH_SAND_ARG_MAC_DATA(sand_arg),           \
                                          SH_SAND_ARG_MAC_MASK(sand_arg));          \
    }                                                                               \
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) |                             \
             (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))                        \
    {                                                                               \
      dbal_entry_key_field_arr8_masked_set(unit, entry_handle_id, field_id,         \
                                          SH_SAND_ARG_IP6_DATA(sand_arg),           \
                                          SH_SAND_ARG_IP6_MASK(sand_arg));          \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      dbal_entry_key_field_arr32_masked_set(unit, entry_handle_id, field_id,        \
                                          SH_SAND_ARG_ARRAY_DATA(sand_arg),         \
                                          SH_SAND_ARG_ARRAY_MASK(sand_arg));        \
    }                                                                               \
}while(0)

#define DBAL_ENTRY_COMMIT_KEY_FIELD(unit, entry_handle_id, field_id, sand_arg)      \
do                                                                                  \
{                                                                                   \
    dbal_field_types_defs_e field_type;                                             \
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));       \
                                                                                    \
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)                                   \
    {                                                                               \
      dbal_entry_key_field_arr8_set(unit, entry_handle_id, field_id,                \
                                                   SH_SAND_ARG_MAC_DATA(sand_arg)); \
    }                                                                               \
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) |                             \
             (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))                        \
    {                                                                               \
      dbal_entry_key_field_arr8_set(unit, entry_handle_id, field_id,                \
                                                   SH_SAND_ARG_IP6_DATA(sand_arg)); \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      dbal_entry_key_field_arr32_set(unit, entry_handle_id, field_id,               \
                                                 SH_SAND_ARG_ARRAY_DATA(sand_arg)); \
    }                                                                               \
}while(0)

#define DBAL_ENTRY_COMMIT_VALUE_FIELD(unit, entry_handle_id, field_id, res_type, print_err, sand_arg)    \
do                                                                                  \
{                                                                                   \
    dbal_field_types_defs_e field_type;                                             \
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));       \
                                                                                    \
    if (field_type == DBAL_FIELD_TYPE_DEF_L2_MAC)                                   \
    {                                                                               \
      dbal_entry_value_field_arr8_set(unit, entry_handle_id, field_id, INST_SINGLE, \
                                                   SH_SAND_ARG_MAC_DATA(sand_arg)); \
    }                                                                               \
    else if ((field_type == DBAL_FIELD_TYPE_DEF_IPV6) |                             \
             (field_type == DBAL_FIELD_TYPE_DEF_IPV6_GROUP))                        \
    {                                                                               \
      dbal_entry_value_field_arr8_set(unit, entry_handle_id, field_id, INST_SINGLE, \
                                                   SH_SAND_ARG_IP6_DATA(sand_arg)); \
    }                                                                               \
    else                                                                            \
    {                                                                               \
      if(field_id == DBAL_FIELD_RESULT_TYPE)                                        \
      {                                                                             \
          int rv;                                                                   \
          rv = dbal_result_type_string_to_id(unit, table_id, SH_SAND_ARG_STR(sand_arg), &res_type);\
          if(rv)                                                                    \
          {                                                                         \
              if(print_err)                                                         \
              {                                                                     \
                  LOG_CLI((BSL_META("result type name not valid %s, please use the string represent the result type value"), SH_SAND_ARG_STR(sand_arg)));\
                  SHR_IF_ERR_EXIT(diag_dbal_entry_print_help_line(unit, table_id, res_type, cmd, FALSE));\
              }                                                                     \
              SHR_EXIT();                                                           \
          }                                                                         \
          dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE,\
                                                 res_type);                         \
      }                                                                             \
      else                                                                          \
      {                                                                             \
          dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,\
                                                 SH_SAND_ARG_ARRAY_DATA(sand_arg)); \
      }                                                                             \
    }                                                                               \
}while(0)

/*************
 * GLOBALS   *
 */
extern sh_sand_man_t sh_dnx_dbal_man;
extern sh_sand_cmd_t sh_dnx_dbal_cmds[];
extern sal_field_type_e dbal_field_type_to_sal_field_type[];
extern sh_sand_enum_t dbal_test_enum_table[];
extern sh_sand_cmd_t diag_dbal_table_cmds[];
extern sh_sand_cmd_t diag_dbal_signal_cmds[];
extern sh_sand_cmd_t diag_dbal_entry_cmds[];
extern dbal_field_types_defs_e dbal_field_id_to_type[DBAL_NOF_FIELDS];

/**
 * \brief check how many of table key fields has const indication set
 *   \param [in] unit
 *   \param [in] table_id  -  the table to look for\n
 *   \param [out] num_const - return the number of const fields in the key.
 */
shr_error_e dbal_tables_field_is_key_const_val_num_get(
    int unit,
    dbal_tables_e table_id,
    uint8 *num_const);

/**
 * \brief check if a given field is part of the table key fields
 * (directly or as child field)
 *   \param [in] unit
 *   \param [in] table_id -  the table to look for\n
 *   \param [in] field_id -  the field requested\n
 *   \param [out] is_key_const - indication if the key field has const indication set
 */
shr_error_e dbal_tables_field_is_key_const_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key_const);

shr_error_e dbal_builds_enum_sand_table(
    int unit,
    dbal_fields_e field_id);
/**
 * \brief
 *   Prints all the entries of a table with ID table_id.
 *   Printing format is compact - each entry in a single line.
 */
shr_error_e diag_dbal_table_entries_print(
    int unit,
    dbal_tables_e table_id,
    uint8 dump_only_key_fields,
    uint8 dump_defualt_entries,
    args_t * args,
    sh_sand_control_t * sand_control);

/** gives the next avaiable table, for generic cases use: diag_dbal_table_from_string */
cmd_result_t diag_dbal_table_string_to_next_table_id_get(
    int unit,
    char *substr_match,
    dbal_tables_e curr_table_id,
    dbal_tables_e * table_id,
    int *is_full_match);

shr_error_e diag_dbal_table_from_string(
    int unit,
    char *table_name,
    dbal_tables_e * table_id,
    sh_sand_control_t * sand_control);

shr_error_e diag_dnx_dbal_table_capacity_estimate(
    int unit,
    dbal_tables_e table_id,
    int result_type,
    int *estimated_nof_entries);

shr_error_e diag_dbal_entry_print_help_line(
    int unit,
    dbal_tables_e table_id,
    uint32 res_type,
    char *cmd,
    uint8 key_field_only);

/**
 * \brief
 *   Prints the entry of a table
 *   the key is pre defined in the handle and dbal enty get has
 *   been called before calling this function.
 */
shr_error_e diag_dbal_entry_print_entry(
    int unit,
    uint32 entry_handle_id);

#endif /* DIAG_DNX_DBAL_H_INCLUDED */

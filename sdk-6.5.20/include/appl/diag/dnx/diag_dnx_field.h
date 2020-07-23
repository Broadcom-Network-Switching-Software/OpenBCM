/**
 * \file diag_dnx_field.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_DNX_FIELD_H_INCLUDED
/* { */
#define DIAG_DNX_FIELD_H_INCLUDED

/*************
 * INCLUDES  *
 */
#include <soc/dnx/swstate/auto_generated/types/dnx_field_group_types.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_types.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field_actions.h>
/*************
 *  DEFINES  *
 */

#define DNX_FIELD_KEY_ALL        DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_KEYS*2

/*************
 *  MACROS  *
 */

/**
 * \brief
 *   Keyword for the field stage.
 *   Relevant ENUMs:
 *      - Field_stage_enum_table[]
 *      - Field_context_diag_stage_enum_table[]
 *      - Field_efes_diag_stage_enum_table[]
 *      - Field_group_diag_stage_enum_table[]
 *      - Field_key_diag_stage_enum_table[]
 *      - Field_last_diag_stage_enum_table[]
 *      - Field_port_diag_stage_enum_table[]
 *      - Field_qualifier_diag_stage_enum_table[]
 *      - Field_range_diag_stage_enum_table[]
 *      - Field_system_diag_stage_enum_table[]
 *      - Field_virtualwire_diag_stage_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_STAGE         "stage"
/**
 * \brief
 *   Keyword for the field context.
 */
#define DIAG_DNX_FIELD_OPTION_CONTEXT       "context"
/**
 * \brief
 *   Keyword for the field context.
 *   Relevant ENUM: Field_group_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_GROUP         "group"
/**
 * \brief
 *   Keyword for the field group type.
 *   Relevant ENUM: Group_type_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_GROUP_TYPE    "group_type"
/**
 * \brief
 *   Keyword for key.
 *   Relevant ENUMs:
 *      - Field_key_enum_table[]
 *      - Field_init_key_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_KEY           "key"
/**
 * \brief
 *   Keyword for the entry.
 *   Relevant ENUMs:
 *      - In case of ACE entry: Field_ace_entry_id_enum_table[]
 *      - Otherwise : It should be a specific entry id.
 */
#define DIAG_DNX_FIELD_OPTION_ENTRY         "entry"
/**
 * \brief
 *   Keyword for ACE format.
 *   Relevant ENUM:
 *      - Field_ace_format_id_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_FORMAT        "format"
/**
 * \brief
 *   Keyword for IDs.
 *   Relevant ENUMs:
 *      - Field_action_enum_table[]
 *      - Field_qualifier_enum_table[]
 *      - Field_range_type_enum_table[]
 *      - Field_fem_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_ID            "id"
/**
 * \brief
 *   Keyword for the program.
 *   Relevant ENUMs:
 *      - In case of FEM: Field_fem_program_enum_table_for_display[]
 */
#define DIAG_DNX_FIELD_OPTION_PROGRAM       "program"
/**
 * \brief
 *   Keyword for the EFES array.
 */
#define DIAG_DNX_FIELD_OPTION_ARRAY         "array"
/**
 * \brief
 *   Keyword for the EFES position within the EFES array.
 */
#define DIAG_DNX_FIELD_OPTION_POSITION      "position"
/**
 * \brief
 *   Keyword for the valid indication.
 *   Cane be '0'-FALSE or '1'-TRUE.
 */
#define DIAG_DNX_FIELD_OPTION_VALID         "valid"
/**
 * \brief
 *   Keyword for the bit map of active 'action's for which FEM info is to be displayed.
 *   Relevant ENUM:
 *      - Field_fem_active_actions_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_ACTIVE_ACTIONS  "active_actions"
/**
 * \brief
 *   Keyword for the 'secondary' field group for which FEM info is to be displayed
 *   Relevant ENUM:
 *      - Field_group_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_SEC_GROUP       "sec_group"
/**
 * \brief
 *   Keyword for the type.
 *   Relevant ENUMs:
 *      - Field_port_type_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_TYPE           "type"
/**
 * \brief
 *   Keyword for the type.
 *   Relevant ENUMs:
 *      - Field_range_type_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_INDEX           "index"
/**
 * \brief
 *   Keyword for the HANDLERs.
 *   Relevant ENUMs:
 *      - Field_tcam_handler_id_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_HANDLER           "handler"
/**
 * \brief
 *   Keyword for the BANKs.
 *   Relevant ENUMs:
 *      - Field_tcam_bank_id_enum_table[]
 */
#define DIAG_DNX_FIELD_OPTION_BANK             "bank"
/**
 * \brief
 *   Keyword for the external opcode.
 */
#define DIAG_DNX_FIELD_OPTION_OPCODE       "opcode"

/**
 * \brief
 *   Keyword for only one packet injection
 */
#define DIAG_DNX_FIELD_OPTION_ONE_PKT       "one_pkt"

/**
 * \brief
 *   Keyword for the VirtualWire Name.
 */
#define DIAG_DNX_FIELD_OPTION_VW_NAME       "vw_name"
/**
 * \brief
 *   Keyword for the Predefined qualifier/action Name.
 */
#define DIAG_DNX_FIELD_OPTION_PREDEFINED_NAME       "predefined"
/**
 * \brief
 *   Keyword for the Core.
 */
#define DIAG_DNX_FIELD_OPTION_CORE       "core"
/**
 * \brief
 *   Keyword for the Signal.
 */
#define DIAG_DNX_FIELD_OPTION_SIGNAL       "signal"
/**
 * \brief
 *   Keyword for the showing signals.
 */
#define DIAG_DNX_FIELD_OPTION_SHOW_SIG       "show_sig"
/**
 * \brief
 *   Keyword for the showing description.
 */
#define DIAG_DNX_FIELD_OPTION_SHOW_DESC       "show_desc"

/*************
 * GLOBALS   *
 */
extern sh_sand_cmd_t Sh_dnx_field_map_cmds[];

extern sh_sand_cmd_t Sh_dnx_field_fem_base_cmds[];

extern sh_sand_man_t Sh_dnx_field_man;
extern sh_sand_cmd_t Sh_dnx_field_cmds[];

/**
 * \brief
 * Enum for option requiring all 'fem_programs' values
 */
extern sh_sand_enum_t Field_fem_program_enum_table_for_display[];
/**
 * \brief
 * Enum for option requiring all 'pmf' and none 'pmf' stages
 */
extern sh_sand_enum_t Field_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'context' diagnostic.
 */
extern sh_sand_enum_t Field_context_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'efes' diagnostic.
 */
extern sh_sand_enum_t Field_efes_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'group' diagnostic.
 */
extern sh_sand_enum_t Field_group_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'key' diagnostic.
 */
extern sh_sand_enum_t Field_key_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'last' diagnostic.
 */
extern sh_sand_enum_t Field_last_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'port' diagnostic.
 */
extern sh_sand_enum_t Field_port_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'qualifier' diagnostic.
 */
extern sh_sand_enum_t Field_qualifier_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'action' diagnostic.
 */
extern sh_sand_enum_t Field_action_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'range' diagnostic.
 */
extern sh_sand_enum_t Field_range_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'system' diagnostic.
 */
extern sh_sand_enum_t Field_system_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' and none 'pmf' stages for 'virtualwire' diagnostic.
 */
extern sh_sand_enum_t Field_virtualwire_diag_stage_enum_table[];
/**
 * \brief
 * Enum for option requiring all 'pmf' qualifier classes
 */
extern sh_sand_enum_t Field_qual_class_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' field groups
 */
extern sh_sand_enum_t Field_group_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' ACE Format IDs
 */
extern sh_sand_enum_t Field_ace_format_id_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' ACE Entry IDs
 */
extern sh_sand_enum_t Field_ace_entry_id_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' TCAM Handler IDs
 */
extern sh_sand_enum_t Field_tcam_handler_id_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' TCAM Bank IDs
 */
extern sh_sand_enum_t Field_tcam_bank_id_enum_table[];
/**
* \brief
* Enum for option requiring 'pmf' field qualifiers
*/
extern sh_sand_enum_t Field_qualifier_enum_table[];
/**
* \brief
* Enum for option requiring 'pmf' field virtualwire qualifiers
*/
extern sh_sand_enum_t Field_qualifier_vw_enum_table[];
/**
* \brief
* Enum for option requiring 'pmf' field actions
*/
extern sh_sand_enum_t Field_action_enum_table[];
/**
* \brief
* Enum for option requiring 'pmf' field virtualwire actions
*/
extern sh_sand_enum_t Field_action_vw_enum_table[];
/**
 * \brief
 * Enum for option requiring 'pmf' FEMs
 */
extern sh_sand_enum_t Field_fem_enum_table[];
/**
 * \brief
 * Enum for option requiring all 'group types'
 */
extern sh_sand_enum_t Group_type_enum_table[];
/**
 * \brief
 * Enum for option requiring all 'port types'
 */
extern sh_sand_enum_t Field_port_type_enum_table[];
/**
 * \brief
 * Enum for option requiring 'active actions' for FEMs
 */
extern sh_sand_enum_t Field_fem_active_actions_enum_table[];
/**
 * \brief
 * Enum for option requiring all 'range types' for range diagnostic.
 */
extern sh_sand_enum_t Field_range_list_range_type_enum_table[];
/**
 * \brief
 * Enum for option requiring all 'range types' for range extended diagnostic.
 */
extern sh_sand_enum_t Field_range_extended_range_type_enum_table[];
/**
 * \brief
 * Enum for option requiring all 'range config types' for range extended diagnostic.
 */
extern sh_sand_enum_t Field_range_extended_range_config_type_enum_table[];

extern sh_sand_enum_t Field_level_enum_table[];
extern sh_sand_enum_t Field_offset_enum_table[];
extern sh_sand_enum_t Field_de_type_enum_table[];
extern sh_sand_enum_t field_qual_class_type_enum_table[];
extern sh_sand_enum_t Field_key_enum_table[];
extern sh_sand_enum_t Field_init_key_enum_table[];
extern sh_sand_enum_t Field_key_display_type_enum_table[];
extern sh_sand_enum_t Field_kbp_opcode_type_enum_table[];
/*
 * Diagnostics procedures common to diag_dnx_field and ctest_dnx_field
 * {
 */
/*
 * Utility procedures, related to FEMs, to be used for, say, display of FEMs (See ctest_dnx_field_action.c)
 * {
 */
/**
 * \brief
 *   Display SWSTATE contents for a specified FEM
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] test_text -
 *   Name of test to be added to header of table
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *   * If current severity is higher than 'info' then display nothing.
 * \see
 *   * appl_dnx_action_starter
 */
shr_error_e appl_dnx_action_swstate_fem_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    char *test_text,
    sh_sand_control_t * sand_control);
/**
 * \brief
 *    Display SWSTATE contents for a specified 'FEM and FEM program' pair
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] fem_id -
 *    Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *    Indicator for which 'FEM program' this operation is intended.
 * \param [in] num_skips_per_row -
 *    Print formatting instruction. Number of columns to skip before getting to the "map index"
 *    column. Since some users add columns before calling this procedure, this
 *    input enables internal control over these added columns. See field_fem_display_by_context_cb().
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] prt_ctr -
 *   Control information related PRT tables. Required for procedures which, for example, add
 *   cells to an existing table.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * appl_dnx_action_starter
 */
shr_error_e appl_dnx_action_swstate_fem_prg_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    unsigned int num_skips_per_row,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr);

/**
 * \brief
 *   Check HW contents of all 'conditions' for a specified 'FEM' and
 *   indicate whether there is any condition that is valid and, hence,
 *   points to a valid 'action'.
 *   Indicates, then, whether at least one action is 'valid' for this FEM.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [out] valid_action_found_p -
 *   Pointer to int. This procedure loads pointed memory by a non-zero value if
 *   at least one action is 'valid' for specified 'fem_id'.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Used to verify that there is at least one action to display for
 *      specified 'fem_id'. If there is none, then info on that 'fem_id'
 *      will, in most cases, not be displayed.
 * \see
 *   * field_fem_display_actions_cb
 */
shr_error_e appl_dnx_action_hw_fem_is_any_action_on_condition_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    int *valid_action_found_p);
/**
 * \brief
 *   Check HW contents of all 'conditions' for a specified 'FEM and FEM program' pair.
 *   Indicate whether at least one condition is 'valid'
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *   Indicator for which 'FEM program' this operation is intended.
 * \param [out] valid_condition_found_p -
 *   Pointer to int. This procedure loads pointed memory by a non-zero value if
 *   at least one of the 16 conditions is marked 'valid'.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * field_fem_display_conditions_cb
 */
shr_error_e appl_dnx_action_hw_fem_is_any_condition_valid(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    int *valid_condition_found_p);
/**
 * \brief
 *   Display HW contents of all info related to all 'actions' ('fem_map_index') on
 *   a specified 'fem_id'. This info is: 'action type', 'adder', all 'bit descriptors'
 *   and indication of the 'fem_programs' that point to this 'action index'.
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] num_skips_per_row -
 *   Print formatting instruction. Number of columns to skip before getting to the "action specific info"
 *   columns. Since some users add columns before calling this procedure, this input enables internal
 *   control over these added columns.
 * \param [in] non_empty_only -
 *    Flag. Boolean. If non-zero then display only actions which are pointed by at least one 'valid'
 *    'condition'. Otherwise, display HW contents regardless of 'valid' state.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] prt_ctr -
 *   Control information related PRT tables. Required for procedures which, for example, add
 *   cells to an existing table.
 * \param [out] is_any_action_user_defined_p -
 *   Indicates if any of the FEM actions is User Defined.
 *   In case yes will return TRUE, otherwise FALSE.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    n the standard case (non_empty_only = TRUE), the caller of this procedure is assumed
 *    to have verified that there is at least one action, on this 'fem_id' which is pointed
 *    by at least one 'condition' on one of the 'fem_programs' that are assigned to this 'fem_id'.
 * \see
 *   * field_fem_display_conditions_cb
 */
shr_error_e appl_dnx_action_hw_fem_all_action_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    unsigned int num_skips_per_row,
    int non_empty_only,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr,
    uint8 *is_any_action_user_defined_p);
/**
 * \brief
 *   Display HW contents of all 'conditions' for a specified 'FEM and FEM program' pair
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] fem_id -
 *   Indicator for which 'FEM id' this operation is intended.
 * \param [in] fem_program -
 *   Indicator for which 'FEM program' this operation is intended.
 * \param [in] num_skips_per_row -
 *   Print formatting instruction. Number of columns to skip before getting to the "fem_condition_entry"
 *   columns (action_valid, map_data and map_index). Since some users add columns before calling this
 *   procedure, this input enables internal control over these added columns.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] prt_ctr -
 *   Control information related PRT tables. Required for procedures which, for example, add
 *   cells to an existing table.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    None
 * \see
 *   * field_fem_display_conditions_cb
 */
shr_error_e appl_dnx_action_hw_fem_condition_display(
    int unit,
    dnx_field_fem_id_t fem_id,
    dnx_field_fem_program_t fem_program,
    unsigned int num_skips_per_row,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr);

/**
 * \brief
 *    Display 'fem context' HW table contents for a specified 'fem context' and 'fem id' range.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \param [in] context_id -
 *    Indicator for which 'context id' this operation is intended.
 * \param [in] non_empty_only -
 *    Flag. Boolean. If non-zero then display only contexts which have at least one 'active'
 *    fem. A fem is considered 'active' if the 'fem key select' field has a legal value.
 *    See 'dbal_enum_value_field_field_pmf_a_fem_key_select_e' and DNX_FIELD_INVALID_FEM_KEY_SELECT
 * \param [in] fem_id_lower -
 *    Indicator for the lowest 'FEM id' this operation is intended for.
 * \param [in] fem_id_upper -
 *    Indicator for the highest 'FEM id' this operation is intended for.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller.
 * \param [in] prt_ctr -
 *   Control information related PRT tables. Required for procedures which, for example, add
 *   cells to an existing table.
 * \param [out] context_was_loaded_p -
 *   This procedure loads pointed memory by a non-zero value if at least one fem was found to be 'active'
 *   for specified 'context_id'.
 * \return
 *    \retval Zero - On success
 *    \retval Error - Identifier as per shr_error_e
 * \remark
 *    * Note that no info is printed, at all, if both no 'active' FEM is found on specified 'context_id'
 *      and 'non_empty_only' is non-zero.
 * \see
 *   * field_fem_display_by_context_cb()
 */
shr_error_e appl_dnx_action_full_fem_context_display(
    int unit,
    dnx_field_context_t context_id,
    int non_empty_only,
    dnx_field_fem_id_t fem_id_lower,
    dnx_field_fem_id_t fem_id_upper,
    sh_sand_control_t * sand_control,
    prt_control_t * prt_ctr,
    int *context_was_loaded_p);
/*
 * }
 */
/*
 * }
 */
/* } */
#endif /* DIAG_DNX_FIELD_H_INCLUDED */

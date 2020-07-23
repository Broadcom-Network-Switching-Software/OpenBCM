/*! \file bcmltcmd_internal.h
 *
 * Definitions intended for bcmltcmd internal use only.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTCMD_INTERNAL_H
#define BCMLTCMD_INTERNAL_H

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcmlt/bcmlt.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

/*!
 * \brief Table access commands parameters.
 *
 * This structure stores the parameters setting of logical or physical table
 * access commands execution.
 */
typedef struct bcmltcmd_params_s {

    /*!
     * True if table access command operates in asynchronous mode.
     * Default is in synchronous mode.
     */
    int async;

    /*! Table entry commit priority. */
    bcmlt_priority_level_t priority;

    /*!
     * True if table access command commits in transaction mode.
     * Default is in entry mode.
     */
    int trans;

    /*! Table access command notify option, valid in asynchronous mode. */
    bcmlt_notif_option_t notify;

    /*!
     * Table access command transaction type, valid when commits in
     * transaction mode.
     */
    bcmlt_trans_type_t trans_type;

    /*! Display modes (HEX, DEC or HEX + DEC) for different field types. */
    bcma_bcmlt_display_mode_t disp_mode;

    /*! Run the table access command in verbose mode. */
    int verbose;

    /*!
     * Option to configure the continuous commands mode for transaction commit.
     * This option is used to support a large amount of entries committed as
     * a transaction via the CLI lt/pt commands that might be limited in the
     * CLI input buffer size.
     */
    int cmd_cont;

    /*!
     * True if use the current parameters as the default parameters.
     * Otherwise the current parameters would be a one time setting.
     */
    int save_cur_to_default;

} bcmltcmd_params_t;

/*!
 * \brief Table operation parse function.
 *
 * \param[in] args CLI arguments list.
 * \param[in] ti Transaction information (NULL in entry mode).
 * \param[in] ei Entry information to store the parsed result.
 *
 * \retval BCMA_CLI_CMD_OK on success, otherwise failure.
 */
typedef int (*bcmltcmd_table_op_parse_t)(bcma_cli_args_t *args,
                                         bcma_bcmlt_transaction_info_t *ti,
                                         bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Table operation commit function.
 *
 * \param[in] ti Transaction information (NULL in entry mode).
 * \param[in] ei Entry information.
 * \param[in] new_entry True if the entry is a new allocated one to commit.
 *
 * \retval 0 on success, otherwise failure.
 */
typedef int (*bcmltcmd_table_op_commit_t)(bcma_bcmlt_transaction_info_t *ti,
                                          bcma_bcmlt_entry_info_t *ei,
                                          bool new_entry);

typedef struct bcmltcmd_cb_s {

    /*! Function for parsing table operation. */
    bcmltcmd_table_op_parse_t table_op_parse;

    /*! Function for commit table operation. */
    bcmltcmd_table_op_commit_t table_op_commit;

} bcmltcmd_cb_t;

/*!
 * \brief Parse fields value and add fields to entry.
 *
 * Parse fields value from CLI input(if any) in format of
 * <field name>=<field value>, and add parsed fields result(if any) to
 * entry.
 *
 * \param[in] args CLI arguments list.
 * \param[in] ei Entry information.
 *
 * \retval 0 No errors, otherwise -1.
 */
extern int
bcmltcmd_parse_fields(bcma_cli_args_t *args, bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Parse table attributes strings.
 *
 * Parse table attributes from CLI input (if any) in format of
 * attr=<attr1>[,<attr2>,...], and add parsed attribute value to
 * entry.
 *
 * \param[in] args CLI arguments list.
 * \param[in] ei Entry information.
 *
 * \retval Attribute value for entry.
 */
extern uint32_t
bcmltcmd_parse_attr(bcma_cli_args_t *args);

/*!
 * \brief Parse sub-strings of fields to be set as environment variables.
 *
 * Parse string from CLI input (if any) in format of
 * EnvVar=<str_1>,[<str_2>,...] for LT LOOKUP operation to set fields
 * qualified for sub-string matching to environment variables.
 * The string <str_n> can be specified as str[a] or str[a-b] for matching
 * the indices range of array field.
 * If EnvVar string is specified, the string will be tokenized into
 * <str_1>, <str_2>... and stored in the entry information \c ei.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param[in,out] ei Entry information.
 *
 * \retval Always 0.
 */
extern int
bcmltcmd_parse_envvar(bcma_cli_t *cli, bcma_cli_args_t *args,
                      bcma_bcmlt_entry_info_t *ei);

/*!
 * \brief Parse entry operation options.
 *
 * Parse entry operation options from CLI input (if any) which is specified
 * after a dash (-). The parsed options are collected in buffer \c buf.
 *
 * \param[in] args CLI arguments list.
 * \param[in] buf_size Buffer size.
 * \param[out] buf Buffer to store the parsed options.
 *
 * \retval Pointer to the parsed result buffer. NULL if \c buf is NULL.
 */
extern char *
bcmltcmd_parse_options(bcma_cli_args_t *args, int buf_size, char *buf);

/*!
 * \brief Parse table access commands parameters.
 *
 * Parse fields value from CLI input(if any) in format of
 * <field name>=<field value>, and add parsed fields result(if any) to
 * entry.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param[in,out] cmd_param Table operation command parameters.
 *
 * \retval 0 No errors, otherwise -1
 */
extern int
bcmltcmd_parse_parameters(bcma_cli_t *cli, bcma_cli_args_t *args,
                          bcmltcmd_params_t *cmd_param);

/*!
 * \brief Match tables with the sub-string specified in arguments \c args.
 *
 * This function will parse all of the arguments \c args and will callback
 * to function \c cb for each table matched the specified \c args.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param[in] physical Identify physical table if true,
 *                     otherwise logical table.
 * \param [in] cb Callback function when \c search_name matches.
 * \param [in] cookie User data for callback function.
 */
extern int
bcmltcmd_tables_match(bcma_cli_t *cli, bcma_cli_args_t *args, bool physical,
                      bcma_bcmlt_table_info_f cb, void *cookie);

/*!
 * \brief List tables information.
 *
 * List Logical or physical tables information according from CLI input.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param[in] physical Identify physical table if true,
 *                     otherwise logical table.
 *
 * \retval Always 0.
 */
extern int
bcmltcmd_tables_list(bcma_cli_t *cli, bcma_cli_args_t *args, bool physical);

/*!
 * \brief Traverse logical tables and list the traversed result.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param [in] disp_mode CLI output display mode of field value.
 *
 * \retval Always 0.
 */
extern int
bcmltcmd_lt_traverse(bcma_cli_t *cli, bcma_cli_args_t *args,
                     bcma_bcmlt_display_mode_t disp_mode);

/*!
 * \brief Dump specified physical tables contenets.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param [in] disp_mode CLI output display mode of field value.
 *
 * \retval Always 0.
 */
extern int
bcmltcmd_pt_dump(bcma_cli_t *cli, bcma_cli_args_t *args,
                 bcma_bcmlt_display_mode_t disp_mode);

/*!
 * \brief Reset specified physical tables contenets to default.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 *
 * \retval Always 0.
 */
extern int
bcmltcmd_pt_reset(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Handle CLI input for entry commit mode.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param[in] params Table operation command parameters.
 * \param[in] cb Callback handlers for processing table operation command.
 * \param[in] physical Identify entry operation on logical or physical table.
 *
 * \regval BCMA_CLI_CMD_OK No errors, otherwise BCMA_CLI_CMD_USAGE or
 *            BCMA_CLI_CMD_FAIL.
 */
extern int
bcmltcmd_commit_by_entry(bcma_cli_t *cli, bcma_cli_args_t *args,
                         bcmltcmd_params_t *params, bcmltcmd_cb_t *cb,
                         bool physical);

/*!
 * \brief Handle CLI input for transaction commit mode.
 *
 * \param[in] cli CLI object.
 * \param[in] args CLI arguments list.
 * \param[in] params Table operation command parameters.
 * \param[in] cb Callback handlers for processing table operation command.
 * \param[in] physical Identify entry operation on logical or physical table.
 * \param[in,out] cont_ti Transaction information used in continuous mode.
 *
 * \regval BCMA_CLI_CMD_OK No errors, otherwise BCMA_CLI_CMD_USAGE or
 *            BCMA_CLI_CMD_FAIL.
 */
extern int
bcmltcmd_commit_by_transaction(bcma_cli_t *cli, bcma_cli_args_t *args,
                               bcmltcmd_params_t *params, bcmltcmd_cb_t *cb,
                               bool physical,
                               bcma_bcmlt_transaction_info_t **cont_ti);

/*!
 * \brief Format logical table operation code to string.
 *
 * \param[in] opc Logical table operation code.
 *
 * \return String of the specified logical table operation.
 */
extern char *
bcmltcmd_lt_opcode_format(bcmlt_opcode_t opc);

/*!
 * \brief Get logical table operation codes.
 *
 * \return Logical table operations structure.
 */
extern const bcma_cli_parse_enum_t *
bcmltcmd_lt_opcodes_get(void);

/*!
 * \brief Format physical table operation code to string.

 * \param[in] opc Physical table operation code.
 *
 * \return String of the specified physical table operation.
 */
extern char *
bcmltcmd_pt_opcode_format(bcmlt_pt_opcode_t opc);

/*!
 * \brief Get physical table operation codes.
 *
 * \return Physical table operations structure.
 */
extern const bcma_cli_parse_enum_t *
bcmltcmd_pt_opcodes_get(void);

/*!
 * \brief Get the support strings of entry attributes.
 *
 * \return Entry attributes enum structure.
 */
extern const bcma_cli_parse_enum_t *
bcmltcmd_ent_attr_enum_get(void);

/*!
 * \brief Format table operation code from bcmlt entry information.
 *
 * This function can retrieve the entry operation code and entry table type
 * (logical or physical) from bcmlt entry information.
 *
 * \param[in] bcmlt_ei Entry information returned from bcmlt module.
 * \param[out] opcode Logical or physical operation code.
 * \param[out] physical True if the entry is a physical table.
 *
 * \return String of logical or physical table operation.
 */
extern char *
bcmltcmd_opcode_format(bcmlt_entry_info_t *bcmlt_ei,
                       int *opcode, bool *physical);

/*!
 * Format asynchronous notification code to string.
 *
 * \param[in] ntf Notification code.
 *
 * \return String of the specified notification code.
 */
extern char *
bcmltcmd_notification_format(bcmlt_notif_option_t ntf);

/*!
 * Format priority level code to string.
 *
 * \param[in] priority Priority level.
 *
 * \return String of the specified priority level.
 */
extern char *
bcmltcmd_priority_format(bcmlt_priority_level_t priority);

/*!
 * Format transaction type code to string.
 *
 * \param[in] type Transaction type.
 *
 * \return String of the specified transaction type.
 */
extern char *
bcmltcmd_trans_type_format(bcmlt_trans_type_t type);

#endif /* BCMLTCMD_INTERNAL_H */

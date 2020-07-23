/*! \file bcma_bcmltcmd_pt.c
 *
 * Physical table access command in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmlt/bcma_bcmltcmd_pt.h>

#include "bcmltcmd_internal.h"

/* Default setting of the pt command parameters */
static bcmltcmd_params_t pt_def_params = {
    .async = 0,
    .priority = BCMLT_PRIORITY_NORMAL,
    .trans = 0,
    .notify = BCMLT_NOTIF_OPTION_HW,
    .trans_type = BCMLT_TRANS_TYPE_BATCH,
    .disp_mode.scalar = BCMA_BCMLT_DISP_MODE_HEX_DEC,
    .disp_mode.array = BCMA_BCMLT_DISP_MODE_HEX_DEC,
    .disp_mode.array_delim = ',',
    .disp_mode.field_delim = '\n',
    .verbose = 1,
    .cmd_cont = 0,
    .save_cur_to_default = 0
};

/* Current setting of the pt command parameters */
static bcmltcmd_params_t pt_cur_params;

/* Physical table operation codes */
static bcma_cli_parse_enum_t pt_cmds_enum[] = {
    { "set",    BCMLT_PT_OPCODE_SET       },
    { "get",    BCMLT_PT_OPCODE_GET       },
    { "modify", BCMLT_PT_OPCODE_MODIFY    },
    { "lookup", BCMLT_PT_OPCODE_LOOKUP    },
    { NULL,     BCMLT_PT_OPCODE_NOP       }
};

/* Transaction handle used when the command is in continuous mode */
static bcma_bcmlt_transaction_info_t *pt_cont_ti[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Get physical table operation code from CLI input.
 */
static bcmlt_pt_opcode_t
pt_opcode_parse(const char *str)
{
    bcmlt_pt_opcode_t opc;
    bcma_cli_parse_enum_t *e = &pt_cmds_enum[0];

    if (str == NULL) {
        return BCMLT_PT_OPCODE_NOP;
    }

    while (e->name != NULL) {
        if (sal_strcasecmp(e->name, str) == 0) {
            break;
        }
        e++;
    }

    opc = e->val;

    return opc;
}

static void
post_commit_display_entry(bcma_bcmlt_entry_info_t *ei, int entry_no,
                          bool show_entry)
{
    bcmlt_pt_opcode_t opc = ei->pt_opcode;
    bcma_bcmlt_display_mode_t disp_mode = pt_cur_params.disp_mode;

    /* Disply entry fields if operation is to get table */
    if (opc == BCMLT_PT_OPCODE_GET || opc == BCMLT_PT_OPCODE_LOOKUP) {
        /* Display entry information */
        if (show_entry) {
            cli_out("  #%d Entry: %s\n", entry_no, ei->name);
        }
        bcma_bcmlt_entry_list(ei, "    ", 0, disp_mode, NULL);
    }
}

/*
 * The function is called after a physical table operation has been committed.
 */
static int
pt_table_op_post_commit_entry(bcma_bcmlt_entry_info_t *ei)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    bcmlt_pt_opcode_t opc = ei->pt_opcode;
    bcma_bcmlt_commit_params_t *commit_params = &ei->commit_params;

    /* Check entry status after async entry commit. */
    if (commit_params->async) {
        rv = bcmlt_entry_info_get(ei->eh, &bcmlt_ei);
        if (SHR_SUCCESS(rv)) {
            rv = bcmlt_ei.status;
        }
        if (SHR_FAILURE(rv)) {
            cli_out("%spt async=1 %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_pt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return -1;
        }
    }

    post_commit_display_entry(ei, 0, false);

    return 0;
}

/*
 * The function is called after a transaction has been committed.
 */
static int
pt_table_op_post_commit_transaction(bcma_bcmlt_transaction_info_t *ti)
{
    int rv;
    uint32_t idx;
    bcmlt_transaction_info_t bcmlt_ti;
    bcma_bcmlt_entry_info_t *ei;
    int verbose = pt_cur_params.verbose;

    rv = bcmlt_transaction_info_get(ti->th, &bcmlt_ti);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get transaction info: %s (%d).\n"),
                   shr_errmsg(rv), rv));
        return -1;
    }

    if (verbose) {
        cli_out("Transaction commit in %s %s mode with %d entr%s:\n",
                bcmlt_ti.type == BCMLT_TRANS_TYPE_BATCH ? "batch" : "atomic",
                ti->commit_params.async ? "asynchronous" : "synchronous",
                bcmlt_ti.num_entries,
                bcmlt_ti.num_entries > 1 ? "ies" : "y");
    }

    for (idx = 0, ei = ti->ei_head; idx < bcmlt_ti.num_entries && ei;
         idx++, ei = ei->next) {
        bcmlt_entry_info_t bcmlt_ei;

        rv = bcmlt_transaction_entry_num_get(ti->th, idx, &bcmlt_ei);
        if (SHR_FAILURE(rv)) {
            cli_out("%sFailed to get the #%d entry info in transaction: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, idx + 1, BCMA_BCMLT_ERRMSG(rv));
            continue;
        }
        if (verbose) {
            cli_out("  #%3d Entry: %s %s\n", idx + 1, bcmlt_ei.table_name,
                    bcmltcmd_opcode_format(&bcmlt_ei, NULL, NULL));
        }
        /* Check entry status */
        rv = bcmlt_ei.status;
        if (SHR_FAILURE(rv)) {
            cli_out("%s%s: Invalid entry (#%d) status: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, bcmlt_ei.table_name,
                    idx + 1, BCMA_BCMLT_ERRMSG(rv));
            if (ti->type == BCMLT_TRANS_TYPE_BATCH) {
                continue;
            } else {
                /* All or none operation for atomic transaction type. */
                return -1;
            }
        }

        post_commit_display_entry(ei, idx + 1, !verbose);
    }

    /* Sanity check for the transaction contents in bcmlt and bcma/bcmlt */
    if (idx != bcmlt_ti.num_entries || ei != NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Number of entries in transaction is different "
                            "to the number of entry list. %s\n"),
                   ei ? "Entry is not NULL" : ""));
        return -1;
    }

    return 0;
}

/*
 * The function is called before an operation is committed.
 */
static void
pt_table_op_pre_commit_entry(bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    int idx;
    bcmlt_pt_opcode_t opc = ei->pt_opcode;
    shr_pb_t *pb;
    bcma_bcmlt_display_mode_t disp_mode;
    int verbose = pt_cur_params.verbose;

    disp_mode.scalar = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array_delim = ',';
    disp_mode.field_delim = ' ';

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    /*
     * Display what we're going to commit.
     * eg. physical table name, op code, field value parsed from CLI.
     */
    if (!verbose) {
        return;
    }
    pb = shr_pb_create();

    if (new_entry) {
        shr_pb_printf(pb, "Table %s:\n", ei->name);
    }
    shr_pb_printf(pb, "  %s ", bcmltcmd_pt_opcode_format(opc));
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        bcma_bcmlt_field_format(pb, NULL, field, disp_mode);
    }
    /* Replace the last field delimiter with newline */
    shr_pb_replace_last(pb, '\n');

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);
}

/*
 * The function is called before an operation is committed.
 */
static void
pt_table_op_pre_commit_transaction(bcma_bcmlt_transaction_info_t *ti,
                                   bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    if (ei) {
        pt_table_op_pre_commit_entry(ei, new_entry);
    }
}

/*
 * Callback function for asynchronous commit.
 */
static void
pt_async_commit_cb_entry(bcmlt_notif_option_t event, bcmlt_pt_opcode_t opcode,
                         bcmlt_entry_info_t  *entry_info, void *usr_data)
{
    bcma_bcmlt_entry_info_t *ei;

    if (!entry_info) {
        return;
    }

    ei = (bcma_bcmlt_entry_info_t *)usr_data;
    if (!ei) {
        return;
    }

    LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                (BSL_META("pt %s asynchronous commit notify-%s: %s event\n"),
                 entry_info->table_name,
                 bcmltcmd_notification_format(entry_info->notif_opt),
                 bcmltcmd_notification_format(event)));

    /*
     * Only perform the post operation to display the get or lookup result
     * when it is notified by the HW event which indicates the operation
     * is done.
     *
     * Multiple events will be sent when the notification option
     * is ALL. Check the notify event type to cleanup resources
     * for only one time.
     */
    switch (entry_info->notif_opt) {
    case BCMLT_NOTIF_OPTION_HW:
    case BCMLT_NOTIF_OPTION_ALL:
        if (event == BCMLT_NOTIF_OPTION_HW) {
            pt_table_op_post_commit_entry(ei);
            bcma_bcmlt_entry_info_destroy(ei);
        }
        break;
    default:
        bcma_bcmlt_entry_info_destroy(ei);
        break;
    }
}

/*
 * Callback function for asynchronous commit.
 */
static void
pt_async_commit_cb_transaction(bcmlt_notif_option_t event,
                               bcmlt_transaction_info_t  *trans_info,
                               void *usr_data)
{
    bcma_bcmlt_transaction_info_t *ti;

    if (!trans_info) {
        return;
    }

    ti = (bcma_bcmlt_transaction_info_t *)usr_data;
    if (!ti) {
        return;
    }

    LOG_VERBOSE(BSL_LS_APPL_BCMLT,
                (BSL_META("pt asynchronous transaction commit notify-%s: "
                          "%s event\n"),
                 bcmltcmd_notification_format(trans_info->notif_opt),
                 bcmltcmd_notification_format(event)));

    /*
     * Only perform the post operation to display the get or lookup result
     * when it is notified by the HW event which indicates the operation
     * is done.
     *
     * Multiple events will be sent when the notification option
     * is ALL. Check the notify event type to cleanup resources
     * for only one time.
     */
    switch (trans_info->notif_opt) {
    case BCMLT_NOTIF_OPTION_HW:
    case BCMLT_NOTIF_OPTION_ALL:
        if (event == BCMLT_NOTIF_OPTION_HW) {
            pt_table_op_post_commit_transaction(ti);
            bcma_bcmlt_transaction_info_destroy(ti);
        }
        break;
    default:
        bcma_bcmlt_transaction_info_destroy(ti);
        break;
    }
}

/*
 * Process for commit physical table operation by entry.
 */
static int
pt_table_op_commit_entry(bcma_bcmlt_transaction_info_t *ti,
                         bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    int rv;
    bcmlt_pt_opcode_t opc = ei->pt_opcode;
    bcma_bcmlt_commit_params_t *commit_params = &ei->commit_params;

    pt_table_op_pre_commit_entry(ei, new_entry);

    if (commit_params->async) {
        LOG_INFO(BSL_LS_APPL_BCMLT,
                 (BSL_META("pt %s %s: Entry commit in asynchronous mode, "
                           "notification-%s and priority-%s.\n"),
                  ei->name, bcmltcmd_pt_opcode_format(opc),
                  bcmltcmd_notification_format(commit_params->notify),
                  bcmltcmd_priority_format(commit_params->priority)));
        /* Commit physical table in asynchronous mode. */
        rv = bcmlt_pt_entry_commit_async(ei->eh, opc, ei,
                                         commit_params->notify,
                                         pt_async_commit_cb_entry,
                                         commit_params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%spt %s %s: bcmlt_pt_entry_commit_async failed: "
                    BCMA_BCMLT_ERRMSG_FMT"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_pt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (commit_params->notify == BCMLT_NOTIF_OPTION_NO_NOTIF) {
            bcma_bcmlt_entry_info_destroy(ei);
        }
    } else {
        /* Commit physical table in synchronous mode. */
        rv = bcmlt_pt_entry_commit(ei->eh, opc, commit_params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%spt %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_pt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return BCMA_CLI_CMD_FAIL;
        }

        /* Perform post-op only if entry is committed successfully. */
        if (pt_table_op_post_commit_entry(ei) != 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Process for commit physical table operation(s) by transaction.
 */
static int
pt_table_op_commit_transaction(bcma_bcmlt_transaction_info_t *ti,
                               bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    int rv;
    int ret = BCMA_CLI_CMD_OK;
    bcma_bcmlt_commit_params_t *commit_params = &ti->commit_params;

    pt_table_op_pre_commit_transaction(ti, ei, new_entry);

    /*
     * The entry will be added to the transaction if 'ei' is not NULL.
     * After all entries are added to the transaction, 'ei' will be
     * passed as NULL to commit the transaction process.
     */

    /* Add the entry to the transaction. */
    if (ei) {
        if (bcma_bcmlt_transaction_entry_add(ti, ei) < 0) {
            return BCMA_CLI_CMD_FAIL;
        }
        return BCMA_CLI_CMD_OK;
    }

    /* Commit the transaction. */
    if (commit_params->async) {
        LOG_INFO(BSL_LS_APPL_BCMLT,
                 (BSL_META("pt: Transaction commit in asynchronous mode, "
                           "notification-%s and priority-%s.\n"),
                  bcmltcmd_notification_format(commit_params->notify),
                  bcmltcmd_priority_format(commit_params->priority)));
        /* Commit the transaction in asynchronous mode. */
        rv = bcmlt_transaction_commit_async(ti->th, commit_params->notify, ti,
                                            pt_async_commit_cb_transaction,
                                            commit_params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%sbcmlt_transaction_commit_async failed: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (commit_params->notify == BCMLT_NOTIF_OPTION_NO_NOTIF) {
            bcma_bcmlt_transaction_info_destroy(ti);
        }
    } else {
        /* Commit the transaction in synchronous mode. */
        rv = bcmlt_transaction_commit(ti->th, commit_params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%sbcmlt_transaction_commit failed: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, BCMA_BCMLT_ERRMSG(rv));
            ret = BCMA_CLI_CMD_FAIL;
        }
        if (pt_table_op_post_commit_transaction(ti) != 0) {
            ret = BCMA_CLI_CMD_FAIL;
        }
    }

    return ret;
}

/*
 * This function is used to check whether the specified option
 * is valid for the given opcode. The valid option will be
 * stored in the entry information for further processing.
 */
static bool
pt_opcode_option_check(bcma_bcmlt_entry_info_t *ei,
                       bcmlt_pt_opcode_t opc, char opt)
{
    if (opt == 'd') {
        /*
         * OPT: Only show fields with non-default value.
         */
        if (opc == BCMLT_PT_OPCODE_GET || opc == BCMLT_PT_OPCODE_LOOKUP) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_FLD_DIFF;
            return true;
        }
    } else if (opt == 'n') {
        /*
         * OPT: Show all fields of the entry with non-default value.
         */
        if (opc == BCMLT_PT_OPCODE_GET || opc == BCMLT_PT_OPCODE_LOOKUP) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_ENT_DIFF;
            return true;
        }
    }

    return false;
}

/*
 * Parse physical table operation in syntax: <op> [<field>=<val> ...]
 */
static int
pt_table_op_parse(bcma_cli_args_t *args,
                  bcma_bcmlt_transaction_info_t *ti,
                  bcma_bcmlt_entry_info_t *ei)
{
    char *arg;
    char buf[16], *opts, opt;

    /* Get physical table operation code */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    ei->pt_opcode = pt_opcode_parse(arg);
    if (ei->pt_opcode == BCMLT_PT_OPCODE_NOP) {
        /* Not a valid op code, restore the argument */
        BCMA_CLI_ARG_PREV(args);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Check for operation options. */
    ei->opts = 0;
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        while ((opt = *opts++) != '\0') {
            if (!pt_opcode_option_check(ei, ei->pt_opcode, opt)) {
                cli_out("%sOption '%c' is not supported in %s "
                        "operation%s.\n", BCMA_CLI_CONFIG_ERROR_STR,
                        opt, bcmltcmd_pt_opcode_format(ei->pt_opcode),
                        ti ? " (commit in transaction)" : "");
                return BCMA_CLI_CMD_FAIL;
            }
        }
    }

    if (bcmltcmd_parse_fields(args, ei) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }

    return BCMA_CLI_CMD_OK;
}

/* pt sub-command handler for table operation */
static int
pt_command_table_op(bcma_cli_t *cli, bcma_cli_args_t *args,
                    bcmltcmd_params_t *params)
{
    bcmltcmd_cb_t cb;
    int unit = cli->cmd_unit;

    /* Same parse function for different commit modes */
    cb.table_op_parse = pt_table_op_parse;

    /* Commit by transaction */
    if (params->trans || pt_cont_ti[unit]) {
        cb.table_op_commit = pt_table_op_commit_transaction;
        return bcmltcmd_commit_by_transaction(cli, args, params, &cb, true,
                                              &pt_cont_ti[unit]);
    }

    /* Commit by entry */
    cb.table_op_commit = pt_table_op_commit_entry;
    return bcmltcmd_commit_by_entry(cli, args, params, &cb, true);
}

/* pt sub-command "list" handler */
static int
pt_command_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return bcmltcmd_tables_list(cli, args, true);
}

/* pt sub-command "dump" handler */
static int
pt_command_dump(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_bcmlt_display_mode_t disp_mode = pt_cur_params.disp_mode;

    return bcmltcmd_pt_dump(cli, args, disp_mode);
}

/* pt sub-command "reset" handler */
static int
pt_command_reset(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return bcmltcmd_pt_reset(cli, args);
}

char *
bcmltcmd_pt_opcode_format(bcmlt_pt_opcode_t opc)
{
    bcma_cli_parse_enum_t *e = &pt_cmds_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_pt_opcode_t)e->val == opc) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid OP>" : e->name;
}

const bcma_cli_parse_enum_t *
bcmltcmd_pt_opcodes_get(void)
{
    return pt_cmds_enum;
}

int
bcma_bcmltcmd_pt(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *arg;
    bcmltcmd_params_t *params = &pt_cur_params;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Use default parameters as the initial current parameters. */
    sal_memcpy(params, &pt_def_params, sizeof(*params));

    /* Check input arguments by CLI parse table */
    if (bcmltcmd_parse_parameters(cli, args, params) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }
    if (params->save_cur_to_default) {
        /* Restore the current setting to default. */
        sal_memcpy(&pt_def_params, params, sizeof(*params));
        /* Clear the save to default flag. */
        pt_def_params.save_cur_to_default = 0;
        /*
         * The Command continue flag always needs to be set explicitly
         * to avoid misuse.
         */
        pt_def_params.cmd_cont = 0;
    }

    /* Done for the default setting */
    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_OK;
    }

    arg = BCMA_CLI_ARG_CUR(args);
    if (sal_strcmp(arg, "list") == 0) {
        /* pt command for tables list */
        BCMA_CLI_ARG_NEXT(args);
        rv = pt_command_list(cli, args);
    } else if (sal_strcmp(arg, "dump") == 0) {
        /* pt command for tables traverse */
        BCMA_CLI_ARG_NEXT(args);
        rv = pt_command_dump(cli, args);
    } else if (sal_strcmp(arg, "reset") == 0) {
        /* pt command for tables reset */
        BCMA_CLI_ARG_NEXT(args);
        rv = pt_command_reset(cli, args);
    } else {
        /* pt command for tables operation */
        rv = pt_command_table_op(cli, args, params);
    }

    return rv;
}

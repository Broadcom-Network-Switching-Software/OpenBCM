/*! \file bcma_bcmltcmd_lt.c
 *
 * Logical table access command in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <shr/shr_error.h>

#include <sal/sal_alloc.h>
#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/cli/bcma_cli_parse.h>

#include <bcma/bcmlt/bcma_bcmltcmd_lt.h>

#include "bcmltcmd_internal.h"

/*
 * Cookie for the callback function traversed_entry_commit()
 * from bcma_bcmlt_table_traverse().
 */
typedef struct trvs_entry_commit_info_s {
    bcma_bcmlt_entry_info_t *ei;
    int opc;
    int rv;
    int cnt;
    bool qlfr;
    bool aborted;
} trvs_entry_commit_info_t;

static int
lt_table_op_commit_entry(bcma_bcmlt_transaction_info_t *ti,
                         bcma_bcmlt_entry_info_t *ei, bool new_entry);

/* Default setting of the lt command parameters */
static bcmltcmd_params_t lt_def_params = {
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

/* Current setting of the lt command parameters */
static bcmltcmd_params_t lt_cur_params;

/* Logical table operation codes */
static bcma_cli_parse_enum_t lt_cmds_enum[] = {
    { "delete",   BCMLT_OPCODE_DELETE   },
    { "insert",   BCMLT_OPCODE_INSERT   },
    { "update",   BCMLT_OPCODE_UPDATE   },
    { "lookup",   BCMLT_OPCODE_LOOKUP   },
    { "traverse", BCMLT_OPCODE_TRAVERSE },
    { NULL,       BCMLT_OPCODE_NOP      }
};

/* Transaction handle used when the command is in continuous mode */
static bcma_bcmlt_transaction_info_t *lt_cont_ti[BCMDRD_CONFIG_MAX_UNITS];

/*
 * Get logical table operation code from CLI input.
 */
static bcmlt_opcode_t
lt_opcode_parse(const char *str)
{
    bcmlt_opcode_t opc;
    bcma_cli_parse_enum_t *e = &lt_cmds_enum[0];

    if (str == NULL) {
        return BCMLT_OPCODE_NOP;
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

/*
 * This function is used to check whether the specified option
 * is valid for the given opcode. The valid option will be
 * stored in the entry information for further processing.
 */
static bool
lt_opcode_option_check(bcma_bcmlt_transaction_info_t *ti,
                       bcma_bcmlt_entry_info_t *ei,
                       bcmlt_opcode_t opc, char opt)
{
    bool transaction = (ti != NULL);

    if (opt == 'i') {
        /*
         * OPT: INSERT after entry not found UPDATE.
         * Support in update opcode that commit in entry mode only.
         */
        if (opc == BCMLT_OPCODE_UPDATE && !transaction) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_INS_UPDT;
            return true;
        }
    } else if (opt == 'l') {
        /*
         * OPT: Perform lookup operation on each traversed entry.
         * Support in traverse opcode.
         */
        if (opc == BCMLT_OPCODE_TRAVERSE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_TRVS_LKUP;
            return true;
        }
    } else if (opt == 'u') {
        /*
         * OPT: Perform update operation on each traversed entry.
         * Support in traverse opcode.
         */
        if (opc == BCMLT_OPCODE_TRAVERSE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_TRVS_UPD;
            return true;
        }
    } else if (opt == 'd') {
        /*
         * OPT: Perform delete operation on each traversed entry.
         * Support in traverse opcode.
         */
        if (opc == BCMLT_OPCODE_TRAVERSE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_TRVS_DEL;
            return true;
        }
    } else if (opt == 'a') {
        /*
         * OPT: Auto-fill unspecified key fields by default values.
         * Support in insert/lookup/update/delete opcodes.
         */
        if (opc == BCMLT_OPCODE_INSERT || opc == BCMLT_OPCODE_LOOKUP ||
            opc == BCMLT_OPCODE_UPDATE || opc == BCMLT_OPCODE_DELETE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_AUTO_KFS;
            return true;
        }
    } else if (opt == 'f') {
        /*
         * OPT: Force unspecified fields to reset to default vaules.
         * Support in update opcodes.
         */
        if (opc == BCMLT_OPCODE_UPDATE) {
            ei->opts |= BCMA_BCMLT_ENTRY_OP_OPT_FLD_RESET;
            return true;
        }
    }

    return false;
}

/*
 * This function sets environment variables for the specified fields after
 * a successful LOOKUP operation or INSERT with index allocation.
 */
static void
post_commit_entry_set_envvar(bcma_bcmlt_entry_info_t *ei, bool append)
{
    bcma_cli_tokens_t *ctoks = ei->ev_ctoks;
    int *array_idx_range = ei->ev_arrayinfo;
    int idx, fidx;
    bool all_fields = false;
    shr_pb_t *pb = NULL;
    bcma_bcmlt_field_t **keyfields = NULL;
    int fcnt = 0;

    /* No entry fields are interested to be set to environment variables */
    if (ctoks == NULL || array_idx_range == NULL) {
        return;
    }

    if (append && ei->num_key_fields > 1) {
        keyfields = sal_alloc(sizeof(bcma_bcmlt_field_t *) * ei->num_key_fields,
                              "bcmaBcmltTrvsEnvVar");
        if (keyfields == NULL) {
            return;
        }
        pb = shr_pb_create();
    }

    /*
     * Check for all fields returned from LOOKUP or TRAVERSE operation
     * to see if any field is needed to be set to environment variable
     */
    for (fidx = 0; fidx < ei->num_fields; fidx++) {
        bcma_bcmlt_field_t *field = &ei->fields[fidx];
        int start, end, aidx, nlen, tlen;

        /* Skip unused fields */
        if (field->name == NULL) {
            continue;
        }

        for (idx = 0; idx < ctoks->argc && !all_fields; idx++) {
            char *token = ctoks->argv[idx];

            if (sal_strcmp(token, "*") == 0) {
                all_fields = true;
                break;
            }
            start = array_idx_range[idx * 2];
            end = array_idx_range[idx * 2 + 1];
            if (start != -1 && end != -1) {
                /* The token is specified to match array field */
                if ((field->flags & BCMA_BCMLT_FIELD_F_ARRAY) == 0) {
                    continue;
                }
                nlen = sal_strlen(field->name);
                tlen = sal_strlen(token);
                /*
                 * Match for array field by array field index format
                 * token[a] or token[a-b]
                 */
                if (nlen >= tlen &&
                    sal_strcasecmp(field->name + nlen - tlen, token) == 0) {
                    for (aidx = start; aidx <= end; aidx++) {
                        bcma_bcmlt_entry_field_var_set(ei, field, aidx);
                    }
                }
                /*
                 * Check for next token since different indices might be set
                 * for the same array field
                 */
                continue;
            } else if (sal_strcasestr(field->name, token)) {
                if (append) {
                    /* Support appending for key fields only when traversing */
                    if (field->dflt.key) {
                        bcma_bcmlt_entry_field_var_append(ei, field);
                        if (ei->num_key_fields > 1) {
                            shr_pb_printf(pb, "%s%s",
                                          fcnt > 0 ? "__" : "", field->name);
                            keyfields[fcnt++] = field;
                        }
                    }
                } else {
                    bcma_bcmlt_entry_field_var_set(ei, field, -1);
                }
                break;
            }
        }
        if (all_fields) {
            if (append) {
                /* Support appending for key fields only when traversing */
                if (field->dflt.key) {
                    bcma_bcmlt_entry_field_var_append(ei, field);
                    if (ei->num_key_fields > 1) {
                        shr_pb_printf(pb, "%s%s",
                                      fcnt > 0 ? "__" : "", field->name);
                        keyfields[fcnt++] = field;
                    }
                }
            } else {
                bcma_bcmlt_entry_field_var_set(ei, field, -1);
            }
        }
    }

    if (keyfields) {
        if (fcnt > 1) {
            /* Create variable for multiple keys group */
            bcma_bcmlt_entry_field_var_append_group(ei, keyfields, fcnt,
                                                    shr_pb_str(pb));
        }
        sal_free(keyfields);
    }
    if (pb) {
        shr_pb_destroy(pb);
    }
}

/*
 * This function is used to display the entry fields values after a successful
 * LOOKUP operation or INSERT operation for index with allocate case.
 */
static void
post_commit_display_entry(bcma_bcmlt_entry_info_t *ei, int entry_no,
                          bool show_entry)
{
    bcmlt_opcode_t opc = ei->opcode;
    bcma_bcmlt_display_mode_t disp_mode = lt_cur_params.disp_mode;

    if (opc == BCMLT_OPCODE_LOOKUP) {
        /* Display entry information */
        if (show_entry) {
            cli_out("  #%d Entry: %s\n", entry_no, ei->name);
        }
        bcma_bcmlt_entry_list(ei, "    ", false, disp_mode, NULL);
        post_commit_entry_set_envvar(ei, false);
    } else if (opc == BCMLT_OPCODE_INSERT) {
        /* Check for index with allocate on insert operation */
        if (ei->num_commit_fields >= 0) {
            uint32_t fcnt = 0;

            if (SHR_SUCCESS(bcmlt_entry_field_count(ei->eh, &fcnt))) {
                if ((int)fcnt > ei->num_commit_fields) {
                    /* Presumably to be index with allocation */
                    if (show_entry) {
                        cli_out("  #%d Entry: %s\n", entry_no, ei->name);
                    }
                    bcma_bcmlt_entry_list(ei, "    ", true, disp_mode, NULL);
                    post_commit_entry_set_envvar(ei, false);
                }
            }
        }
    }
}

/*
 * The function is called after a logical table operation has been committed.
 */
static int
lt_table_op_post_commit_entry(bcma_bcmlt_entry_info_t *ei)
{
    int rv;
    bcmlt_entry_info_t bcmlt_ei;
    bcmlt_opcode_t opc = ei->opcode;
    bcma_bcmlt_commit_params_t *commit_params = &ei->commit_params;

    /* Check entry status after async entry commit. */
    if (commit_params->async) {
        rv = bcmlt_entry_info_get(ei->eh, &bcmlt_ei);
        if (SHR_SUCCESS(rv)) {
            rv = bcmlt_ei.status;
        }
        if (SHR_FAILURE(rv)) {
            cli_out("%slt async=1 %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_lt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
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
lt_table_op_post_commit_transaction(bcma_bcmlt_transaction_info_t *ti)
{
    int rv;
    uint32_t idx;
    bcmlt_transaction_info_t bcmlt_ti;
    bcma_bcmlt_entry_info_t *ei;
    int verbose = lt_cur_params.verbose;

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
lt_table_op_pre_commit_entry(bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    int idx;
    bcmlt_opcode_t opc = ei->opcode;
    uint32_t fcnt = 0;
    shr_pb_t *pb;
    bcma_bcmlt_display_mode_t disp_mode;
    int verbose = lt_cur_params.verbose;

    disp_mode.scalar = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array = BCMA_BCMLT_DISP_MODE_HEX;
    disp_mode.array_delim = ',';
    disp_mode.field_delim = ' ';

    /* Set entry attribute */
    if (ei->attr != 0) {
        bcmlt_entry_attrib_set(ei->eh, ei->attr);
    }

    /* Get number of fields to be committed. */
    ei->num_commit_fields = -1;
    if (SHR_SUCCESS(bcmlt_entry_field_count(ei->eh, &fcnt))) {
        ei->num_commit_fields = fcnt;
    }

    /* Unset all variables set through previous table operation if any */
    bcma_bcmlt_entry_field_vars_unset(ei);

    /*
     * Display what we're going to commit.
     * eg. logical table name, op code, field value parsed from CLI.
     */
    if (!verbose) {
        return;
    }
    pb = shr_pb_create();

    if (new_entry) {
        shr_pb_printf(pb, "Table %s:\n", ei->name);
    }
    shr_pb_printf(pb, "  %s ", bcmltcmd_lt_opcode_format(opc));
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
lt_table_op_pre_commit_transaction(bcma_bcmlt_transaction_info_t *ti,
                                   bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    if (ei) {
        lt_table_op_pre_commit_entry(ei, new_entry);
    }
}

/*
 * Callback function for asynchronous commit.
 */
static void
lt_async_commit_cb_entry(bcmlt_notif_option_t event, bcmlt_opcode_t opcode,
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
                (BSL_META("lt %s asynchronous commit notify-%s: %s event\n"),
                 entry_info->table_name,
                 bcmltcmd_notification_format(entry_info->notif_opt),
                 bcmltcmd_notification_format(event)));

    /*
     * Only perform the post operation to display the lookup result
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
            lt_table_op_post_commit_entry(ei);
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
lt_async_commit_cb_transaction(bcmlt_notif_option_t event,
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
                (BSL_META("lt asynchronous transaction commit notify-%s: "
                          "%s event\n"),
                 bcmltcmd_notification_format(trans_info->notif_opt),
                 bcmltcmd_notification_format(event)));

    /*
     * Only perform the post operation to display the lookup result
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
            lt_table_op_post_commit_transaction(ti);
            bcma_bcmlt_transaction_info_destroy(ti);
        }
        break;
    default:
        bcma_bcmlt_transaction_info_destroy(ti);
        break;
    }
}

/*
 * Callback function for bcma_bcmlt_table_traverse().
 * Process for commit logical table operation for the traversed entry.
 */
static int
traversed_entry_commit(void *cookie, bcma_bcmlt_entry_info_t *tei, int entry_no)
{
    trvs_entry_commit_info_t *teci = (trvs_entry_commit_info_t *)cookie;
    bcma_bcmlt_entry_info_t *ei = teci->ei;
    bcma_bcmlt_commit_params_t *commit_params = &ei->commit_params;
    bcma_bcmlt_display_mode_t disp_mode = lt_cur_params.disp_mode;
    int opc = teci->opc;
    int rv = 0, idx;

    if (bcma_cli_ctrlc_break()) {
        teci->aborted = true;
        return -1;
    }

    /* Skip others if errors occurred previously. */
    if (SHR_FAILURE(teci->rv)) {
        return -1;
    }

    /* Check if any traverse qualifier is specified */
    for (idx = 0; idx < ei->num_qlfr_fields; idx++) {
        bcma_bcmlt_field_t field = {0};
        bcma_bcmlt_field_t *qlfr_field = &ei->qlfr_fields[idx];
        bool qualified = true;

        teci->qlfr = true;

        rv += bcma_bcmlt_field_init(&field, &qlfr_field->dflt);
        rv += bcma_bcmlt_entry_field_get(tei, &field, false);
        if (rv == 0) {
            qualified = bcma_bcmlt_field_qualifier(&field, qlfr_field);
        }
        bcma_bcmlt_field_array_free(&field);

        if (rv < 0) {
            teci->rv = SHR_E_FAIL;
            return -1;
        }

        if (!qualified) {
            /* Skip this entry */
            return 0;
        }
    }

    rv = 0;

    /*
     * The entry returned from traverse will contain all the fields
     * associated with the LT. If this entry is used for an update
     * operation, any read-only fields will be committed as well and
     * cause an access error even if the read-only fields values
     * remain unchanged.
     *
     * To avoid such access error, only the key fields from the
     * traverse-returned entry are added to the entry containing the
     * user specified fields for the optional delete/lookup/update
     * operations upon traverse.
     */
    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcmlt_field_def_t *df = &ei->def_fields[idx];

        if (df->key) {
            bcma_bcmlt_field_t field = {0};

            rv += bcma_bcmlt_field_init(&field, df);
            rv += bcma_bcmlt_entry_field_get(tei, &field, false);
            rv += bcma_bcmlt_entry_field_add(ei, &field);
            bcma_bcmlt_field_array_free(&field);

            if (rv < 0) {
                teci->rv = SHR_E_FAIL;
                return -1;
            }
        }
    }

    /* Commit operation for the entry if the traverse option is specified. */
    if (opc != BCMLT_OPCODE_NOP) {
        rv = bcmlt_entry_commit(ei->eh, opc, commit_params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%s%s operation failed while traversing table %s: "
                    BCMA_BCMLT_ERRMSG_FMT"\n",
                    BCMA_CLI_CONFIG_ERROR_STR, bcmltcmd_lt_opcode_format(opc),
                    ei->name, BCMA_BCMLT_ERRMSG(rv));
            teci->rv = rv;
            return -1;
        }
    }

    teci->cnt++;

    /* Display traversed entry information. */
    if (teci->cnt > 1) {
        cli_out("\n");
    }
    rv = bcma_bcmlt_entry_list(ei, "    ",
                               opc == BCMLT_OPCODE_LOOKUP ? 0 : 1,
                               disp_mode, NULL);
    if (opc == BCMLT_OPCODE_LOOKUP || opc == BCMLT_OPCODE_NOP) {
        post_commit_entry_set_envvar(ei,
                                     opc == BCMLT_OPCODE_LOOKUP ? false : true);
    }
    return rv;
}

/*
 * LT traverse operation.
 */
static int
op_commit_traverse(bcma_bcmlt_entry_info_t *ei)
{
    int cnt, idx;
    trvs_entry_commit_info_t teci;
    bcmlt_opcode_t opcode = BCMLT_OPCODE_NOP;

    /* Return failure if any key field is specified for traverse. */
    for (idx = 0; idx < ei->num_fields; idx++) {
        bcma_bcmlt_field_t *field = &ei->fields[idx];

        if (field->flags & BCMA_BCMLT_FIELD_F_KEY) {
            cli_out("%sSet key field %s is not allowed in traversing "
                    "table %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    ei->fields[idx].name, ei->name);
            return BCMA_CLI_CMD_FAIL;
        }
    }

    sal_memset(&teci, 0, sizeof(teci));
    teci.ei = ei;
    if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_TRVS_LKUP) {
        opcode = BCMLT_OPCODE_LOOKUP;
    } else if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_TRVS_UPD) {
        opcode = BCMLT_OPCODE_UPDATE;
    } else if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_TRVS_DEL) {
        opcode = BCMLT_OPCODE_DELETE;
    }
    teci.opc = opcode;

    cnt = bcma_bcmlt_table_traverse(ei->unit, ei->name, ei->attr,
                                    ei->def_fields, ei->num_def_fields,
                                    traversed_entry_commit, &teci);

    if (SHR_FAILURE(teci.rv)) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (cnt > 0) {
        cli_out("  %d entr%s traversed", cnt, (cnt > 1) ? "ies" : "y");
        if (teci.qlfr) {
            cli_out(". %d entr%s matching",
                    teci.cnt, (teci.cnt > 1) ? "ies" : "y");
        }
        if (opcode == BCMLT_OPCODE_NOP) {
            cli_out(".\n");
        } else {
            cli_out(" for %s operation.\n", bcmltcmd_lt_opcode_format(opcode));
        }
        return teci.aborted ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
    }

    if (cnt < 0) {
        cli_out("    "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(cnt));
    }
    return BCMA_CLI_CMD_FAIL;
}

/*
 * Process for commit logical table operation by entry.
 */
static int
lt_table_op_commit_entry(bcma_bcmlt_transaction_info_t *ti,
                         bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    int rv;
    bcmlt_opcode_t opc = ei->opcode;
    bcma_bcmlt_commit_params_t *commit_params = &ei->commit_params;

    lt_table_op_pre_commit_entry(ei, new_entry);

    if (opc == BCMLT_OPCODE_TRAVERSE) {
        return op_commit_traverse(ei);
    }

    if (commit_params->async) {
        LOG_INFO(BSL_LS_APPL_BCMLT,
                 (BSL_META("lt %s %s: Entry commit in asynchronous mode, "
                           "notification-%s and priority-%s.\n"),
                  ei->name, bcmltcmd_lt_opcode_format(opc),
                  bcmltcmd_notification_format(commit_params->notify),
                  bcmltcmd_priority_format(commit_params->priority)));
        /* Commit logical table in asynchronous mode. */
        rv = bcmlt_entry_commit_async(ei->eh, opc, ei,
                                      commit_params->notify,
                                      lt_async_commit_cb_entry,
                                      commit_params->priority);
        if (SHR_FAILURE(rv)) {
            cli_out("%slt %s %s: bcmlt_entry_commit_async failed: "
                    BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_lt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return BCMA_CLI_CMD_FAIL;
        }

        if (commit_params->notify == BCMLT_NOTIF_OPTION_NO_NOTIF) {
            bcma_bcmlt_entry_info_destroy(ei);
        }
    } else {
        /* Commit logical table in synchronous mode. */
        rv = bcmlt_entry_commit(ei->eh, opc, commit_params->priority);
        if (SHR_FAILURE(rv)) {
            /*
             * If an UPDATE operation fails with NOT_FOUND, then we retry
             * the operation as an INSERT instead of forcing the user to
             * do this manually.
             */
            if (opc == BCMLT_OPCODE_UPDATE && rv == SHR_E_NOT_FOUND &&
                ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_INS_UPDT) {
                cli_out("    "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(rv));
                ei->opcode = BCMLT_OPCODE_INSERT;
                return lt_table_op_commit_entry(NULL, ei, false);
            }
            cli_out("%slt %s %s: "BCMA_BCMLT_ERRMSG_FMT".\n",
                    BCMA_CLI_CONFIG_ERROR_STR, ei->name,
                    bcmltcmd_lt_opcode_format(opc), BCMA_BCMLT_ERRMSG(rv));
            return BCMA_CLI_CMD_FAIL;
        }

        /* Perform post-op only if entry is committed successfully. */
        if (lt_table_op_post_commit_entry(ei) != 0) {
            return BCMA_CLI_CMD_FAIL;
        }
    }

    return BCMA_CLI_CMD_OK;
}

/*
 * Process for commit logical table operation(s) by transaction.
 */
static int
lt_table_op_commit_transaction(bcma_bcmlt_transaction_info_t *ti,
                               bcma_bcmlt_entry_info_t *ei, bool new_entry)
{
    int rv;
    int ret = BCMA_CLI_CMD_OK;
    bcma_bcmlt_commit_params_t *commit_params = &ti->commit_params;

    if (ei && ei->opcode == BCMLT_OPCODE_TRAVERSE) {
        cli_out("%sTraverse operation is not supported "
                "in transaction mode.\n", BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }

    lt_table_op_pre_commit_transaction(ti, ei, new_entry);

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
                 (BSL_META("lt: Transaction commit in asynchronous mode, "
                           "notification-%s and priority-%s.\n"),
                  bcmltcmd_notification_format(commit_params->notify),
                  bcmltcmd_priority_format(commit_params->priority)));
        /* Commit the transaction in asynchronous mode. */
        rv = bcmlt_transaction_commit_async(ti->th, commit_params->notify, ti,
                                            lt_async_commit_cb_transaction,
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
        if (lt_table_op_post_commit_transaction(ti) != 0) {
            ret = BCMA_CLI_CMD_FAIL;
        }
    }

    return ret;
}

/*
 * Parse logical table operation in syntax: <op> [<field>=<val> ...]
 */
static int
lt_table_op_parse(bcma_cli_args_t *args,
                  bcma_bcmlt_transaction_info_t *ti,
                  bcma_bcmlt_entry_info_t *ei)
{
    char *arg;
    char buf[16];
    char *opts;
    char opt;

    /* Get logical table operation code */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }
    ei->opcode = lt_opcode_parse(arg);
    if (ei->opcode == BCMLT_OPCODE_NOP) {
        /* Not a valid op code, restore the argument */
        BCMA_CLI_ARG_PREV(args);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Check for operation options. */
    ei->opts = 0;
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        while ((opt = *opts++) != '\0') {
            if (!lt_opcode_option_check(ti, ei, ei->opcode, opt)) {
                cli_out("%sOption '%c' is not supported in %s "
                        "operation%s.\n", BCMA_CLI_CONFIG_ERROR_STR,
                        opt, bcmltcmd_lt_opcode_format(ei->opcode),
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

/* lt sub-command handler for table operation */
static int
lt_command_table_op(bcma_cli_t *cli, bcma_cli_args_t *args,
                    bcmltcmd_params_t *params)
{
    bcmltcmd_cb_t cb;
    int unit = cli->cmd_unit;

    /* Same parse function for different commit modes */
    cb.table_op_parse = lt_table_op_parse;

    /* Commit by transaction */
    if (params->trans || lt_cont_ti[unit]) {
        cb.table_op_commit = lt_table_op_commit_transaction;
        return bcmltcmd_commit_by_transaction(cli, args, params, &cb, false,
                                              &lt_cont_ti[unit]);
    }

    /* Commit by entry */
    cb.table_op_commit = lt_table_op_commit_entry;
    return bcmltcmd_commit_by_entry(cli, args, params, &cb, false);
}

/* lt sub-command "traverse" handler */
static int
lt_command_traverse(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    bcma_bcmlt_display_mode_t disp_mode = lt_cur_params.disp_mode;

    return bcmltcmd_lt_traverse(cli, args, disp_mode);
}

/* lt sub-command "list" handler */
static int
lt_command_list(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    return bcmltcmd_tables_list(cli, args, false);
}

char *
bcmltcmd_lt_opcode_format(bcmlt_opcode_t opc)
{
    bcma_cli_parse_enum_t *e = &lt_cmds_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_opcode_t)e->val == opc) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid OP>" : e->name;
}

const bcma_cli_parse_enum_t *
bcmltcmd_lt_opcodes_get(void)
{
    return lt_cmds_enum;
}

int
bcma_bcmltcmd_lt(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv = BCMA_CLI_CMD_OK;
    const char *arg;
    bcmltcmd_params_t *params = &lt_cur_params;

    if (cli == NULL || args == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Use default parameters as the initial current parameters. */
    sal_memcpy(params, &lt_def_params, sizeof(*params));

    /* Check input arguments by CLI parse table */
    if (bcmltcmd_parse_parameters(cli, args, params) < 0) {
        return BCMA_CLI_CMD_FAIL;
    }
    if (params->save_cur_to_default) {
        /* Restore the current setting to default. */
        sal_memcpy(&lt_def_params, params, sizeof(*params));
        /* Clear the save to default flag. */
        lt_def_params.save_cur_to_default = 0;
        /*
         * The Command continue flag always needs to be set explicitly
         * to avoid misuse.
         */
        lt_def_params.cmd_cont = 0;
    }

    /* Done for the default setting */
    if (BCMA_CLI_ARG_CNT(args) == 0) {
        return BCMA_CLI_CMD_OK;
    }

    arg = BCMA_CLI_ARG_CUR(args);
    if (sal_strcmp(arg, "list") == 0) {
        /* lt command for tables list */
        BCMA_CLI_ARG_NEXT(args);
        rv = lt_command_list(cli, args);
    } else if (sal_strcmp(arg, "traverse") == 0) {
        /* lt command for tables traverse */
        BCMA_CLI_ARG_NEXT(args);
        rv = lt_command_traverse(cli, args);
    } else {
        /* lt command for tables operation */
        rv = lt_command_table_op(cli, args, params);
    }

    return rv;
}

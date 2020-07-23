/*! \file bcmltcmd.c
 *
 * Common functions for physical and logical table access commands in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>
#include <sal/sal_sleep.h>

#include <bcma/cli/bcma_cli.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcma/cli/bcma_cli_token.h>

#include <bcmlt/bcmlt.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

/* Information for CLI argument which is considered as fields input */
typedef struct field_arg_info_s {
    /* Pointer to the CLI argument */
    char *arg;
    /* True if the field is represented as a qualifier */
    bool qlfr;
} field_arg_info_t;

/* Transaction type */
static bcma_cli_parse_enum_t trans_type_enum[] = {
    { "Atomic", BCMLT_TRANS_TYPE_ATOMIC },
    { "Batch",  BCMLT_TRANS_TYPE_BATCH  },
    { NULL,     0                       }
};

/* Commit priority */
static bcma_cli_parse_enum_t commit_pri_enum[] = {
    { "Normal", BCMLT_PRIORITY_NORMAL },
    { "High",   BCMLT_PRIORITY_HIGH   },
    { NULL,     0                     }
};

/* Commit notify option */
static bcma_cli_parse_enum_t notify_opt_enum[] = {
    { "Commit",    BCMLT_NOTIF_OPTION_COMMIT   },
    { "Hw",        BCMLT_NOTIF_OPTION_HW       },
    { "All",       BCMLT_NOTIF_OPTION_ALL      },
    { "None",      BCMLT_NOTIF_OPTION_NO_NOTIF },
    { NULL,        0                           }
};

/* BCMLT entry attributes */
static bcma_cli_parse_enum_t ent_attr_enum[] = {
    { "CACHE_BYPASS",   BCMLT_ENT_ATTR_GET_FROM_HW       },
    { "FILTER_DFLT",    BCMLT_ENT_ATTR_FILTER_DEFAULTS   },
    { "LPM_LOOKUP",     BCMLT_ENT_ATTR_LPM_LOOKUP        },
    { "TRVS_SNAPSHOT",  BCMLT_ENT_ATTR_TRAVERSE_SNAPSHOT },
    { "TRVS_DONE",      BCMLT_ENT_ATTR_TRAVERSE_DONE     },
    { NULL,             0                                }
};

/* Field value display option */
static bcma_cli_parse_enum_t disp_mode_enum[] = {
    { "Hex",       BCMA_BCMLT_DISP_MODE_HEX     },
    { "Dec",       BCMA_BCMLT_DISP_MODE_DEC     },
    { "HexDec",    BCMA_BCMLT_DISP_MODE_HEX_DEC },
    { NULL,        0                            }
};

/* Field display delimiter */
static bcma_cli_parse_enum_t disp_delim_enum[] = {
    { "Comma",     ','  },
    { "Newline",   '\n' },
    { "Space",     ' '  },
    { NULL,        0    }
};

/* Set the specified commit parameters to bcma_bcmlt_commit_params_t */
static int
commit_params_set(bcma_bcmlt_commit_params_t *commit_params,
                  bcmltcmd_params_t *params)
{
    commit_params->async = params->async;
    commit_params->notify = params->notify;
    commit_params->priority = params->priority;

    return 0;
}

/*
 * Parse and do entry operation in format:
 * <table_name> <table_op> [<table_op> ...]
 */
static int
entry_op(bcma_cli_t *cli, bcma_cli_args_t *args,
         bcma_bcmlt_transaction_info_t *ti,
         bcma_bcmlt_commit_params_t *commit_params,
         bcmltcmd_cb_t *cb, bool physical)
{
    int rv;
    const char *table_name;
    uint32_t attr;
    int unit = cli->cmd_unit;
    bcma_bcmlt_entry_info_t *ei;

    /* Get entry table name */
    if ((table_name = BCMA_CLI_ARG_GET(args)) == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    /* Check entry attribute */
    attr = bcmltcmd_parse_attr(args);

    ei = bcma_bcmlt_entry_info_create(unit, table_name, physical, attr,
                                      commit_params, NULL, 0);
    if (ei == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    bcmltcmd_parse_envvar(cli, args, ei);

    rv = cb->table_op_parse(args, ti, ei);
    if (rv != BCMA_CLI_CMD_OK) {
        /*
         * At least one valid table operation should be there
         * for a table entry.
         */
       bcma_bcmlt_entry_info_destroy(ei);
       return rv;
    }

    rv = cb->table_op_commit(ti, ei, true);
    if (rv != BCMA_CLI_CMD_OK) {
        bcma_bcmlt_entry_info_destroy(ei);
        return rv;
    }

    /* Try if any other table operations for the entry */
    while (BCMA_CLI_ARG_CNT(args) > 0) {
        if (commit_params->async || ti) {
            /*
             * Each table operation needs a new entry for asynchronous mode,
             * and every table in transaction commit needs a new entry as well.
             * Pre-allocate a new entry for these two conditions to parse the
             * table operation.
             */
            ei = bcma_bcmlt_entry_info_create(unit, table_name,
                                              physical, attr, commit_params,
                                              NULL, 0);
            if (ei == NULL) {
                return BCMA_CLI_CMD_FAIL;
            }
        }

        rv = cb->table_op_parse(args, ti, ei);
        if (rv != BCMA_CLI_CMD_OK) {
            /* End of table operation(s) of entry */
            if (commit_params->async || ti) {
                /*
                 * Free the pre-allocated entry for asynchronous mode or
                 * transaction commit.
                 */
                bcma_bcmlt_entry_info_destroy(ei);
            }
            if (ti && rv == BCMA_CLI_CMD_FAIL) {
                return BCMA_CLI_CMD_FAIL;
            } else {
                break;
            }
        }

        rv = cb->table_op_commit(ti, ei, commit_params->async || ti);
        if (rv != BCMA_CLI_CMD_OK) {
            bcma_bcmlt_entry_info_destroy(ei);
            return rv;
        }
    }

    /*
     * Clean up entry info directly for synchronous mode for entry commit.
     * The entry info for asynchronous mode or transaction commit will be
     * cleaned up in commit callback function or scheduled cleanup or
     * transaction cleanup function.
     */
    if (!commit_params->async && !ti) {
        bcma_bcmlt_entry_info_destroy(ei);
    }

    return BCMA_CLI_CMD_OK;
}

static int
entry_fields_default_val_add(bcma_bcmlt_entry_info_t *ei, bool key_only)
{
    int rv = 0;
    int idx;

    for (idx = 0; idx < ei->num_def_fields; idx++) {
        bcma_bcmlt_field_t field = {0};
        bcmlt_field_def_t *df = &ei->def_fields[idx];

        if (key_only && !df->key) {
            continue;
        }

        rv += bcma_bcmlt_field_init(&field, df);
        rv += bcma_bcmlt_field_default_val_set(&field);
        rv += bcma_bcmlt_entry_field_add(ei, &field);
        bcma_bcmlt_field_array_free(&field);

        if (rv < 0) {
            return -1;
        }
    }

    return 0;
}

int
bcmltcmd_commit_by_entry(bcma_cli_t *cli, bcma_cli_args_t *args,
                         bcmltcmd_params_t *params, bcmltcmd_cb_t *cb,
                         bool physical)
{
    int rv;
    bcma_bcmlt_commit_params_t commit_params;

    if (!params || !cb || !cb->table_op_parse || !cb->table_op_commit) {
        return BCMA_CLI_CMD_FAIL;
    }

    commit_params_set(&commit_params, params);

    while (BCMA_CLI_ARG_CNT(args) > 0) {
        rv = entry_op(cli, args, NULL, &commit_params, cb, physical);
        if (rv != BCMA_CLI_CMD_OK) {
            return rv;
        }
    }

    return BCMA_CLI_CMD_OK;
}

int
bcmltcmd_commit_by_transaction(bcma_cli_t *cli, bcma_cli_args_t *args,
                               bcmltcmd_params_t *params, bcmltcmd_cb_t *cb,
                               bool physical,
                               bcma_bcmlt_transaction_info_t **cont_ti)
{
    int rv;
    int unit = cli->cmd_unit;
    bcma_bcmlt_transaction_info_t *ti;
    bcma_bcmlt_commit_params_t commit_params;

    if (!params || !cb || !cb->table_op_parse || !cb->table_op_commit) {
        return BCMA_CLI_CMD_FAIL;
    }

    commit_params_set(&commit_params, params);

    if (*cont_ti != NULL) {
        /*
         * Use the saved transaction information when the transaction command
         * is in continuous mode.
         */
        ti = *cont_ti;
    } else {
        ti = bcma_bcmlt_transaction_info_create(unit, params->trans_type,
                                                &commit_params);
    }
    /* Start of the continuous mode. */
    if (params->cmd_cont && *cont_ti == NULL) {
        *cont_ti = ti;
    }
    /* End of the continuous mode. */
    if (!params->cmd_cont && *cont_ti) {
        *cont_ti = NULL;
    }
    if (ti == NULL) {
        return BCMA_CLI_CMD_FAIL;
    }

    while (BCMA_CLI_ARG_CNT(args) > 0) {
        rv = entry_op(cli, args, ti, &commit_params, cb, physical);
        if (rv != BCMA_CLI_CMD_OK) {
            bcma_bcmlt_transaction_info_destroy(ti);
            /* Clear the continuous mode on error */
            *cont_ti = NULL;
            return rv;
        }
    }

    /* Not yet to commit the transaction when not end to the continuous mode. */
    if (*cont_ti) {
        return BCMA_CLI_CMD_OK;;
    }

    /* Commit the transaction. */
    rv = cb->table_op_commit(ti, NULL, false);
    if (rv < 0) {
        bcma_bcmlt_transaction_info_destroy(ti);
        return BCMA_CLI_CMD_FAIL;
    }

    /*
     * Free the transaction handle only when commit in synchronous mode.
     * If commit in asynchronous mode, the transaction handle will be freed
     * when asynchronous event notifies.
     */
    if (!params->async) {
        bcma_bcmlt_transaction_info_destroy(ti);
    }

    return BCMA_CLI_CMD_OK;
}

int
bcmltcmd_parse_fields(bcma_cli_args_t *args, bcma_bcmlt_entry_info_t *ei)
{
    char *arg;
    field_arg_info_t *fargs = NULL;
    int idx = 0, rv = 0;
    int num_args;

    if (!ei) {
        return -1;
    }

    num_args = BCMA_CLI_ARG_CNT(args);
    if (num_args > ei->num_def_fields) {
        /*
         * Enlarge the fields array since the number of arguments
         * is greater than the default fields array size.
         */
        if (bcma_bcmlt_entry_fields_create(ei, num_args) < 0) {
            return -1;
        }
    }
    if (num_args > 0) {
        fargs = sal_alloc(num_args * sizeof(*fargs), "bcmaBcmltFieldArgInfo");
        if (fargs == NULL) {
            cli_out("Fail to allocate array of field arguments.\n");
            return -1;
        }
        sal_memset(fargs, 0, num_args * sizeof(*fargs));
    }

    /* Clear field numbers */
    ei->num_fields = 0;
    ei->num_qlfr_fields = 0;

    /*
     * Get Field setting if any, in format:
     * <field_name>=<val> <field_name>=<val> ...
     */
    while (fargs && (arg = BCMA_CLI_ARG_GET(args)) != NULL) {
        char *ptr;
        int qlfr;
        /*
         * First check the qualifiers for traverse operation,
         * since some qualifiers contain the '=' character
         */
        qlfr = bcma_bcmlt_entry_field_parse_qualifier(ei, arg);
        if (qlfr == 0) {
            /* No qualifier is specified. Check for field assignment instead. */
            ptr = sal_strchr(arg, '=');
            if (ptr) {
                /* Simple sanity check for =<val> or <field>= */
                if (ptr == arg || *(ptr + 1) == '\0') {
                    cli_out("%sInvalid field assignment: %s\n",
                            BCMA_CLI_CONFIG_ERROR_STR, arg);
                    rv = -1;
                    break;
                }
            } else {
                /* Not a field assignment format, restore the argument */
                BCMA_CLI_ARG_PREV(args);
                break;
            }
        } else if (qlfr > 0) {
            /*
             * A valid qualifier is found. Check if the qualifier fields array
             * has been created.
             */
            if (ei->qlfr_fields == NULL) {
                if (bcma_bcmlt_entry_qlfr_fields_create(ei, num_args) < 0) {
                    rv = -1;
                    break;
                }
            }
        } else {
            cli_out("%sInvalid field qualifier assignment: %s\n",
                    BCMA_CLI_CONFIG_ERROR_STR, arg);
            rv = -1;
            break;
        }

        fargs[idx].arg = arg;
        fargs[idx].qlfr = (qlfr > 0);
        idx++;
    }

    if (rv < 0) {
        sal_free(fargs);
        return -1;
    }

    /* Parse fields value */
    for (idx = 0; idx < num_args; idx++) {
        bcma_bcmlt_field_t *field;
        field_arg_info_t *farg = fargs + idx;

        if (farg->arg == NULL) {
            break;
        }
        if (farg->qlfr) {
            field = &ei->qlfr_fields[ei->num_qlfr_fields++];
        } else {
            field = &ei->fields[ei->num_fields++];
        }
        if (bcma_bcmlt_entry_field_parse(ei, field, farg->arg) < 0) {
            rv = -1;
        }
    }

    if (fargs) {
        sal_free(fargs);
    }

    if (rv < 0) {
        return -1;
    }

    if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_FLD_RESET) {
        /* Force reset to default value option for fields. */
        if (entry_fields_default_val_add(ei, false) < 0) {
            return -1;
        }
    } else if (ei->opts & BCMA_BCMLT_ENTRY_OP_OPT_AUTO_KFS) {
        /*
         * Auto-fill option for key fields. This option is covered
         * when BCMA_BCMLT_ENTRY_OP_OPT_FLD_RESET is set.
         */
        if (entry_fields_default_val_add(ei, true) < 0) {
            return -1;
        }
    }

    /* Add fields value to entry */
    for (idx = 0; idx < ei->num_fields; idx++) {
        if (bcma_bcmlt_entry_field_add(ei, &ei->fields[idx]) < 0) {
            rv = -1;
        }
    }

    return (rv == 0) ? 0 : -1;
}

uint32_t
bcmltcmd_parse_attr(bcma_cli_args_t *args)
{
    uint32_t attr = 0;
    char *arg;
    char *cmp_str = "attr=";
    int cmp_strlen = sal_strlen(cmp_str);

    arg = BCMA_CLI_ARG_CUR(args);
    if (arg == NULL) {
        return 0;
    }

    if (sal_strncasecmp(arg, cmp_str, cmp_strlen) == 0) {
        const char *str;
        bcma_cli_tokens_t *tok;
        int idx;

        BCMA_CLI_ARG_NEXT(args);

        str = arg + cmp_strlen;
        if (*str == '\0') {
            return 0;
        }

        tok = sal_alloc(sizeof(*tok), "bcmaBcmltcmdTok");
        if (tok == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to allocate parsing buffer\n")));
            return 0;
        }
        if (bcma_cli_tokenize(str, tok, ",") < 0) {
            sal_free(tok);
            return 0;
        }

        /* Compare every attribute token to supported attributes */
        for (idx = 0; idx < tok->argc; idx++) {
            bcma_cli_parse_enum_t *e = ent_attr_enum;
            int match = 0;

            while (e->name != NULL) {
                if (sal_strcasecmp(e->name, tok->argv[idx]) == 0) {
                    attr |= e->val;
                    match = 1;
                    break;
                }
                e++;
            }

            /* Display error for unsupported attributes. */
            if (!match) {
                cli_out("%sInvalid table attribute: %s\n",
                        BCMA_CLI_CONFIG_ERROR_STR, tok->argv[idx]);
            }
        }
        sal_free(tok);
    }

    return attr;
}

int
bcmltcmd_parse_envvar(bcma_cli_t *cli, bcma_cli_args_t *args,
                      bcma_bcmlt_entry_info_t *ei)
{
    char *arg;
    char *ptr;
    int idx;
    bcma_cli_tokens_t *ctoks;
    int *array_idx_range;

    ei->ev_cookie = cli;

    arg = BCMA_CLI_ARG_CUR(args);
    if (arg == NULL) {
        return 0;
    }

    if ((ptr = sal_strchr(arg, '=')) == NULL || *(++ptr) == '\0') {
        return 0;
    }
    if (!bcma_cli_parse_cmp("EnvVar", arg, '=')) {
        return 0;
    }

    ctoks = sal_alloc(sizeof(*ctoks), "bcmaBcmltcmdEvctoks");
    if (ctoks == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to allocate parsing buffer for EnvVar\n")));
        return 0;
    }
    if (bcma_cli_tokenize(ptr, ctoks, ",") < 0 || ctoks->argc <= 0) {
        sal_free(ctoks);
        return 0;
    }

    /*
     * Loop through all match tokens to parse array field index format if any
     */
   array_idx_range = sal_alloc(2 * ctoks->argc * sizeof(int),
                               "bcmaBcmltcmdArrayIdxRange");
    if (array_idx_range == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to allocate array buffer for EnvVar\n")));
        sal_free(ctoks);
        return 0;
    }
    for (idx = 0; idx < ctoks->argc; idx++) {
        char *token = ctoks->argv[idx];
        int start = -1, end = -1;

        bcma_bcmlt_field_parse_array_index(token, &start, &end);
        array_idx_range[idx * 2] = start;
        array_idx_range[idx * 2 + 1] = end;
    }

    ei->ev_ctoks = ctoks;
    ei->ev_arrayinfo = array_idx_range;
    BCMA_CLI_ARG_NEXT(args);

    return 0;
}

char *
bcmltcmd_parse_options(bcma_cli_args_t *args, int buf_size, char *buf)
{
    char *arg;
    char opt;
    int cnt = 0;

    if (buf == NULL || buf_size <= 0) {
        return NULL;
    }

    sal_memset(buf, 0, buf_size);

    while ((arg = BCMA_CLI_ARG_CUR(args)) != NULL) {
        if (*arg != '-') {
            break;
        }
        BCMA_CLI_ARG_NEXT(args);
        arg++;
        while ((opt = *arg++) != '\0') {
            if (cnt >= buf_size) {
                break;
            }
            buf[cnt++] = opt;
        }
    }

    return buf;
}

int
bcmltcmd_parse_parameters(bcma_cli_t *cli, bcma_cli_args_t *args,
                          bcmltcmd_params_t *cmd_params)
{
    int rv = 0;
    bcma_cli_parse_table_t prstbl;
    bcma_bcmlt_field_display_mode_t scalar_disp, array_disp;
    int array_disp_delim, field_disp_delim;

    if (!cmd_params) {
        return -1;
    }

    scalar_disp = cmd_params->disp_mode.scalar;
    array_disp = cmd_params->disp_mode.array;
    array_disp_delim = cmd_params->disp_mode.array_delim;
    field_disp_delim = cmd_params->disp_mode.field_delim;

    /* Add entry/transaction variables to parse table for user modification */
    bcma_cli_parse_table_init(cli, &prstbl);
    bcma_cli_parse_table_add(&prstbl, "TRans", "int",
                             &cmd_params->trans, NULL);
    bcma_cli_parse_table_add(&prstbl, "TransType", "enum",
                             &cmd_params->trans_type, trans_type_enum);
    bcma_cli_parse_table_add(&prstbl, "Async", "int",
                             &cmd_params->async, NULL);
    bcma_cli_parse_table_add(&prstbl, "PRIority", "enum",
                             &cmd_params->priority, commit_pri_enum);
    bcma_cli_parse_table_add(&prstbl, "NoTiFy", "enum",
                             &cmd_params->notify, notify_opt_enum);
    bcma_cli_parse_table_add(&prstbl, "ScalarDisplayMode", "enum",
                             &scalar_disp, disp_mode_enum);
    bcma_cli_parse_table_add(&prstbl, "ArrayDisplayMode", "enum",
                             &array_disp, disp_mode_enum);
    bcma_cli_parse_table_add(&prstbl, "ArrayDelimiter", "enum",
                             &array_disp_delim, disp_delim_enum);
    bcma_cli_parse_table_add(&prstbl, "FieldDelimiter", "enum",
                             &field_disp_delim, disp_delim_enum);
    bcma_cli_parse_table_add(&prstbl, "Verbose", "int",
                             &cmd_params->verbose, NULL);
    bcma_cli_parse_table_add(&prstbl, "ContCmdMode", "int",
                             &cmd_params->cmd_cont, NULL);
    bcma_cli_parse_table_add(&prstbl, "DEFault", "int",
                             &cmd_params->save_cur_to_default, NULL);

    if (bcma_cli_parse_table_do_args(&prstbl, args) < 0) {
        cli_out("%s: Error: Invalid option: %s\n",
                BCMA_CLI_ARG_CMD(args), BCMA_CLI_ARG_CUR(args));
        rv = -1;
    }
    if (field_disp_delim != '\n' && array_disp_delim == '\n') {
        /* Reset array delimiter to comma to resolve the conflict. */
        array_disp_delim = ',';
    }
    cmd_params->disp_mode.scalar = scalar_disp;
    cmd_params->disp_mode.array = array_disp;
    cmd_params->disp_mode.array_delim = array_disp_delim;
    cmd_params->disp_mode.field_delim = field_disp_delim;
    bcma_cli_parse_table_done(&prstbl);

    return rv;
}

int
bcmltcmd_tables_match(bcma_cli_t *cli, bcma_cli_args_t *args, bool physical,
                      bcma_bcmlt_table_info_f cb, void *cookie)
{
    int rv = 0;
    char *arg, *arg_start;
    int unit = cli->cmd_unit;
    uint32_t search_scope = physical ? BCMA_BCMLT_SCOPE_F_PTBL :
                                       BCMA_BCMLT_SCOPE_F_LTBL;

    /* Match for all tables. */
    if ((arg = BCMA_CLI_ARG_GET(args)) == NULL) {
        rv += bcma_bcmlt_tables_search(unit, "*", BCMA_BCMLT_MATCH_SUBSTR,
                                       search_scope, cb, cookie);
        if (rv <= 0) {
            cli_out("No available %s tables.\n",
                    physical ? "physical" : "logical");
        }
        return 0;
    }

    /* Match for the specified table(s) */
    arg_start = arg;
    do {
        rv += bcma_bcmlt_tables_search(unit, arg, BCMA_BCMLT_MATCH_SUBSTR,
                                       search_scope, cb, cookie);
    } while ((arg = BCMA_CLI_ARG_GET(args)) != NULL);

    if (rv <= 0) {
        BCMA_CLI_ARG_PREV(args);
        if (arg_start[0] == '@' && arg_start == BCMA_CLI_ARG_GET(args)) {
            /* Can not find the exact match table. */
            cli_out("%sUnsupported %s table: %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    physical ? "physical" : "logical", arg_start + 1);
        } else {
            cli_out("No matching %s tables.\n",
                    physical ? "physical" : "logical");
        }
        BCMA_CLI_ARG_NEXT(args);
    }

    return 0;
}

const bcma_cli_parse_enum_t *
bcmltcmd_ent_attr_enum_get(void)
{
    return ent_attr_enum;
}

char *
bcmltcmd_opcode_format(bcmlt_entry_info_t *bcmlt_ei,
                       int *opcode, bool *physical)
{
    int rv;
    bcmlt_unified_opcode_t uopc;

    if (bcmlt_ei == NULL) {
        return NULL;
    }

    rv = bcmlt_entry_oper_get(bcmlt_ei->entry_hdl, &uopc);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to get table %s entry op code: "
                            "%s (%d).\n"),
                   bcmlt_ei->table_name, shr_errmsg(rv), rv));
        return NULL;
    }
    if (opcode) {
        *opcode = uopc.pt ? uopc.opcode.pt : uopc.opcode.lt;
    }
    if (physical) {
        *physical = uopc.pt;
    }

    if (uopc.pt) {
        return bcmltcmd_pt_opcode_format(uopc.opcode.pt);
    }
    return bcmltcmd_lt_opcode_format(uopc.opcode.lt);
}

char *
bcmltcmd_notification_format(bcmlt_notif_option_t ntf)
{
    bcma_cli_parse_enum_t *e = &notify_opt_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_notif_option_t)e->val == ntf) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid NOTIFY>" : e->name;
}

char *
bcmltcmd_priority_format(bcmlt_priority_level_t priority)
{
    bcma_cli_parse_enum_t *e = &commit_pri_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_priority_level_t)e->val == priority) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid PRIORITY>" : e->name;
}

char *
bcmltcmd_trans_type_format(bcmlt_trans_type_t type)
{
    bcma_cli_parse_enum_t *e = &trans_type_enum[0];

    while (e->name != NULL) {
        if ((bcmlt_trans_type_t)e->val == type) {
            break;
        }
        e++;
    }

    return (e->name == NULL) ? "<Invalid TRANS_TYPE>" : e->name;
}

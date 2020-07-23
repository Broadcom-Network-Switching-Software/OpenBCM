/*! \file bcmltcmd_lt_traverse.c
 *
 * "lt traverse" command in CLI.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <sal/sal_libc.h>

#include <bcma/cli/bcma_cli_ctrlc.h>

#include <bcma/bcmlt/bcma_bcmlt.h>

#include "bcmltcmd_internal.h"

/*
 * Cookie for bcma_bcmlt_tables_search() call-back function
 * table_traverse()
 */
typedef struct tbl_search_trvs_data_s {
    int unit;
    bool verbose;
    bool all_fields;
    bcma_bcmlt_display_mode_t disp_mode;
    uint32_t attr;
    bool aborted;
} tbl_search_trvs_data_t;

/*
 * Callback function of bcma_bcmlt_table_traverse().
 * List fields values of the traversed entry.
 */
static int
traversed_entry_list(void *cookie, bcma_bcmlt_entry_info_t *ei, int entry_no)
{
    tbl_search_trvs_data_t *td = (tbl_search_trvs_data_t *)cookie;
    bool verbose = td->verbose;
    bool all_fields = td->all_fields;
    bcma_bcmlt_display_mode_t disp_mode = td->disp_mode;

    if (bcma_cli_ctrlc_break()) {
        td->aborted = true;
        return -1;
    }

    if (entry_no == 0) {
        if (!verbose) {
            cli_out("Traverse table [%s]:\n", ei->name);
        }
    } else {
        cli_out("\n");
    }
    bcma_bcmlt_entry_list(ei, "    ", all_fields ? 0 : 1, disp_mode, NULL);

    return 0;
}

/*
 * Callback function of bcma_bcmlt_tables_search().
 * Traverse the matched table and list fields values of the traversed entry.
 */
static int
lt_traverse(void *cookie, const char *name,
               bcmlt_field_def_t *fields, uint32_t num_fields)
{
    tbl_search_trvs_data_t *td = (tbl_search_trvs_data_t *)cookie;
    int unit = td->unit;
    bool verbose = td->verbose;
    uint32_t attr = td->attr;
    int cnt;

    if (verbose) {
        cli_out("Traverse table [%s]:\n", name);
    }

    cnt = bcma_bcmlt_table_traverse(unit, name, attr, fields, num_fields,
                                    traversed_entry_list, td);

    if (cnt <= 0) {
        if (verbose) {
            /* No entries are found. */
            cli_out("  "BCMA_BCMLT_ERRMSG_FMT"\n", BCMA_BCMLT_ERRMSG(cnt));
        }
    } else {
        cli_out("  %d entr%s traversed.\n",
                cnt, (cnt > 1) ? "ies" : "y");
    }

    return td->aborted ? -1 : 0;
}

int
bcmltcmd_lt_traverse(bcma_cli_t *cli, bcma_cli_args_t *args,
                     bcma_bcmlt_display_mode_t disp_mode)
{
    int unit = cli->cmd_unit;
    char *opts;
    char buf[16], opt;
    bool verbose = false;
    bool all_fields = false;
    tbl_search_trvs_data_t td;
    uint32_t attr;

    /* Check for attributes. */
    attr = bcmltcmd_parse_attr(args);

    /* Check for traverse options. */
    if ((opts = bcmltcmd_parse_options(args, sizeof(buf), buf)) != NULL) {
        while ((opt = *opts++) != '\0') {
            if (opt == 'v') {
                /* Verbose mode. */
                verbose = true;
            } else if (opt == 'l') {
                /* List all fields instead of key fields only. */
                all_fields = true;
            } else {
                cli_out("%sOption '%c' is not supported.\n",
                        BCMA_CLI_CONFIG_ERROR_STR, opt);
                return -1;
            }
        }
    }

    sal_memset(&td, 0, sizeof(td));
    td.unit = unit;
    td.verbose = verbose;
    td.all_fields = all_fields;
    td.disp_mode = disp_mode;
    td.attr = attr;

    bcmltcmd_tables_match(cli, args, false, lt_traverse, &td);

    return td.aborted ? BCMA_CLI_CMD_INTR : BCMA_CLI_CMD_OK;
}

/*! \file bcma_bcmptmcmd_alpm.c
 *
 * CLI commands for ALPM Debug.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/sal_libc.h>
#include <sal/sal_time.h>
#include <shr/shr_error.h>
#include <bcma/cli/bcma_cli_ctrlc.h>
#include <bcma/io/bcma_io_file.h>
#include <bcma/cli/bcma_cli_parse.h>
#include <bcmlt/bcmlt.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmptm/bcmptm.h>
#include <bcmptm/bcmptm_rm_alpm_internal.h>
#include <bcma/bcmptm/bcma_bcmptmcmd_alpm.h>

typedef struct alpm_show_data_s {
    char *ofile;
    int unit;
    int mtop;
    int db;
    int verbose;
    int brief;
} alpm_show_data_t;

static void
format_time(sal_usecs_t t)
{
    sal_usecs_t sec;

    sec = t /1000000;
    cli_out("%"PRIu32".%06"PRIu32" sec", sec, t - sec * 1000000);
}

static int
alpm_show(alpm_show_data_t *data)
{
    int rv = SHR_E_NONE;
    bcma_io_file_handle_t ofh = NULL;
    shr_pb_t *pb = shr_pb_create();

    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create print buffer object.\n")));
        return -1;
    }

    bcmptm_rm_alpm_info_dump(data->unit, data->mtop, data->db, data->verbose, data->brief, pb);

    if (data->ofile) {
        /* Open the file to write the replay commands to. */
        ofh = bcma_io_file_open(data->ofile, "w");
        if (ofh == NULL) {
            LOG_ERROR(BSL_LS_APPL_BCMLT,
                      (BSL_META("Failed to open file %s to write to.\n"),
                       data->ofile));
            return SHR_E_FAIL;
        }
        cli_out("OutputFile=%s\n", data->ofile);
        bcma_io_file_puts(ofh, shr_pb_str(pb));
    } else {
        cli_out("%s", shr_pb_str(pb));
    }

    if (ofh) {
        bcma_io_file_close(ofh);
    }

    shr_pb_destroy(pb);

    return rv;
}

static int
alpm_show_ctrlc(void *data)
{
    int rv;

    rv = alpm_show(data);
    if (SHR_FAILURE(rv)) {
        cli_out("%sFailed to show alpm.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
        return BCMA_CLI_CMD_FAIL;
    }
    return BCMA_CLI_CMD_OK;
}

static int
alpm_show_config(int unit, int m)
{
    int rv = SHR_E_NONE;
    shr_pb_t *pb = shr_pb_create();

    if (pb == NULL) {
        LOG_ERROR(BSL_LS_APPL_BCMLT,
                  (BSL_META("Failed to create print buffer object.\n")));
        return -1;
    }

    bcmptm_rm_alpm_config_show(unit, m, pb);

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return rv;
}

static int
cmd_alpm_show(int unit, bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    char *ofile = NULL;
    const char *cmd;
    bcma_cli_parse_table_t pt;
    int verbose = 0, db = 0, brief = 0, m = 0;
    alpm_show_data_t data = {0};

    cmd = BCMA_CLI_ARG_CUR(args);
    if (cmd != NULL) {
        if (sal_strcasecmp("config", cmd) == 0) {
            BCMA_CLI_ARG_NEXT(args);

            bcma_cli_parse_table_init(cli, &pt);
            bcma_cli_parse_table_add(&pt, "Mtop", "int", &m, NULL);
            if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
                bcma_cli_parse_table_done(&pt);
                return BCMA_CLI_CMD_USAGE;
            }
            rv = alpm_show_config(unit, m);
            bcma_cli_parse_table_done(&pt);
            return rv < 0 ? BCMA_CLI_CMD_FAIL : BCMA_CLI_CMD_OK;
        }
    }

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Verbose", "bool", &verbose, NULL);
    bcma_cli_parse_table_add(&pt, "DataBase", "int", &db, NULL);
    bcma_cli_parse_table_add(&pt, "Mtop", "int", &m, NULL);
    bcma_cli_parse_table_add(&pt, "OutputFile", "str", &ofile, NULL);
    bcma_cli_parse_table_add(&pt, "Brief", "bool", &brief, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    data.unit = unit;
    data.mtop = m;
    data.db = db;
    data.verbose = verbose;
    data.ofile = ofile;
    data.brief = brief;

    rv = bcma_cli_ctrlc_exec(cli, alpm_show_ctrlc, &data, 1);
    if (rv == BCMA_CLI_CMD_INTR) {
        cli_out("%sAlpm show aborted.\n",
                BCMA_CLI_CONFIG_ERROR_STR);
    }
    bcma_cli_parse_table_done(&pt);

    return rv;
}

static int
cmd_alpm_recreate(int unit, bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv, m = 0;
    bcma_cli_parse_table_t pt;
    sal_usecs_t time_start = 0;
    sal_usecs_t time_end = 0;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "Mtop", "int", &m, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    time_start = sal_time_usecs();
    rv = bcmptm_rm_alpm_commit(unit, m, true);
    if (SHR_SUCCESS(rv)) {
        rv = bcmptm_rm_alpm_abort(unit, m, true);
    }
    time_end = sal_time_usecs();
    if (rv < 0) {
        cli_out("%sAlpm %d recreate failed.\n",
                BCMA_CLI_CONFIG_ERROR_STR, m);
        return BCMA_CLI_CMD_FAIL;
    } else {
        cli_out("Time elapsed: ");
        format_time(time_end - time_start);
        cli_out("\n");
    }

    bcma_cli_parse_table_done(&pt);

    return SHR_E_NONE;
}

static int
cmd_alpm_sanity(int unit, bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    bcma_cli_parse_table_t pt;
    sal_usecs_t time_start = 0;
    sal_usecs_t time_end = 0;
    int num_fails = 0;
    char *search_name = NULL;
    char *table_name;
    bcmltd_sid_t ltid;
    int m = 0;
/*
    bcmlt_table_attrib_t *table_attr;
    void *hdl;
*/

    bcma_cli_parse_table_init(cli, &pt);

    bcma_cli_parse_table_add(&pt, "LT", "str", &search_name, NULL);
    bcma_cli_parse_table_add(&pt, "Mtop", "int", &m, NULL);

    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    if (search_name) {
        rv = bcmlt_table_names_get_first(unit, BCMLT_TABLE_SEARCH_LT, &table_name);
        while (SHR_SUCCESS(rv)) {
            if (sal_strcasecmp(table_name, search_name) == 0) {
                break;
            }
            rv = bcmlt_table_names_get_next(unit, BCMLT_TABLE_SEARCH_LT, &table_name);
        }
        if (rv < 0) {
            cli_out("%sUnsupported logical table: %s\n", BCMA_CLI_CONFIG_ERROR_STR,
                    search_name);
            return BCMA_CLI_CMD_FAIL;
        }
        /* Find the table ID associated with the name */
        ltid = bcmlrd_table_name_to_idx(unit, table_name);
    } else {
        /* Indicate all ALPM LT sids. */
        ltid = BCMLTD_SID_INVALID;
    }

    time_start = sal_time_usecs();
    rv = bcmptm_rm_alpm_sanity(unit, m, ltid, &num_fails);
    time_end = sal_time_usecs();
    if (search_name && rv == SHR_E_NOT_FOUND) {
        cli_out("%sNot ALPM logical table: %s.\n",
                BCMA_CLI_CONFIG_ERROR_STR,
                search_name
                );
        return BCMA_CLI_CMD_FAIL;
    } else if (rv < 0) {
        cli_out("%sALPM sanity failed on [%d] routes.\n",
                BCMA_CLI_CONFIG_ERROR_STR,
                num_fails
                );
        return BCMA_CLI_CMD_FAIL;
    } else {
        cli_out("ALPM sanity passed. Time elapsed: ");
        format_time(time_end - time_start);
        cli_out("\n");
    }

    bcma_cli_parse_table_done(&pt);

    return SHR_E_NONE;
}

static int
cmd_alpm_dump(int unit, bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int rv;
    int db = 0;
    int m = 0;
    int l1_index = -1;
    int levels = 1;
    bcma_cli_parse_table_t pt;

    bcma_cli_parse_table_init(cli, &pt);
    bcma_cli_parse_table_add(&pt, "DataBase", "int", &db, NULL);
    bcma_cli_parse_table_add(&pt, "Mtop", "int", &m, NULL);
    bcma_cli_parse_table_add(&pt, "Index",    "int", &l1_index, NULL);
    bcma_cli_parse_table_add(&pt, "Levels",   "int", &levels, NULL);
    if (bcma_cli_parse_table_do_args(&pt, args) < 0) {
        bcma_cli_parse_table_done(&pt);
        return BCMA_CLI_CMD_USAGE;
    }

    /* Execute dump. */
    rv = bcmptm_rm_alpm_table_dump(unit, m, db, l1_index, levels);
    if (SHR_FAILURE(rv)) {
        rv = BCMA_CLI_CMD_FAIL;
    }

    return rv;
}

int
bcma_bcmptmcmd_alpm(bcma_cli_t *cli, bcma_cli_args_t *args)
{
    int unit;
    const char *cmd;

    unit = cli->cmd_unit;

    cmd = BCMA_CLI_ARG_GET(args);
    if (cmd == NULL) {
        return BCMA_CLI_CMD_USAGE;
    }

    if (sal_strcasecmp("show", cmd) == 0) {
        return cmd_alpm_show(unit, cli, args);
    } else if (sal_strcasecmp("dump", cmd) == 0) {
        return cmd_alpm_dump(unit, cli, args);
    } else if (sal_strcasecmp("sanity", cmd) == 0) {
        return cmd_alpm_sanity(unit, cli, args);
    } else if (sal_strcasecmp("recreate", cmd) == 0) {
        return cmd_alpm_recreate(unit, cli, args);
    }
    return BCMA_CLI_CMD_USAGE;
}


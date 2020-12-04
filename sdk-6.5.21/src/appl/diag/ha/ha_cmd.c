/*! \file ha_cmd.c
 *
 * CLI command to display HA memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>
#include <sal/appl/sal.h>

#include <appl/diag/ha/ha.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf_ltsw.h>

#include <shr/shr_error.h>

/*******************************************************************************
 * Local definitions
 */


/*******************************************************************************
 * Private functions
 */
static int
cmd_alloc_status(int unit, args_t *args)
{
    shr_pb_t *pb;
    const char *arg;
    bool gen_mem_inc = false;
    bool fmt_hdr = true;

    pb = shr_pb_create();

    arg = ARG_GET(args);
    if (arg == NULL) {
        /* Format the memory usage of all components */
        appl_ha_mem_usage(unit, gen_mem_inc, pb, fmt_hdr);
    } else {
        /* Format the memory usage of the specified component */
        do {
            appl_ha_comp_mem_usage(unit, arg, gen_mem_inc, pb, fmt_hdr);
            fmt_hdr = false;
        } while ((arg = ARG_GET(args)) != NULL);
    }

    cli_out("%s", shr_pb_str(pb));

    shr_pb_destroy(pb);

    return CMD_OK;
}

static int
cmd_alloc(int unit, args_t *args)
{
    const char *arg;

    arg = ARG_GET(args);
    if (arg == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "status") == 0) {
        return cmd_alloc_status(unit, args);
    }

    return CMD_USAGE;
}

static int
cmd_file(int unit, args_t *args)
{
    const char *arg;
    int rv;

    arg = ARG_GET(args);
    if (arg == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "info") == 0) {
        char buf[80];
        rv = appl_ha_file_name_get(unit, buf, sizeof(buf));
        if (SHR_SUCCESS(rv)) {
            cli_out("HA file for SDK: %s\n", buf);
        } else {
            cli_out("ERROR: Failed to get HA file name. (%d)\n", rv);
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}

static int
cmd_check(int unit, args_t *args)
{
    const char *arg;
    int rv;
    int ha_chk = 0;
    char *path = NULL;
    char *curr_path = ".";
    parse_table_t pt;

    if (ARG_CNT(args) == 0) {
        sysconf_ltsw_ha_check_get(&ha_chk, &path);
        cli_out("HA check to path <%s>: %s\n",
                path, ha_chk ? "on" : "off");
        return CMD_OK;
    }
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Path", PQ_STRING, curr_path, &path, 0);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("%s: Invalid option: %s\n", ARG_CMD(args), ARG_CUR(args));
        rv = CMD_USAGE;
        goto done;
    }

    if ((arg = ARG_GET(args)) == NULL) {
        arg = "on";        /* Default is to turn logging on */
    }

    if (sal_strcasecmp("on", arg) == 0) {
        if (sysconf_ltsw_ha_check_set(1, path) < 0) {
            rv = CMD_FAIL;
        } else {
            rv = CMD_OK;
        }
        goto done;
    }
    if (sal_strcasecmp("off", arg) == 0) {
        if (sysconf_ltsw_ha_check_set(0, path) < 0) {
            rv = CMD_FAIL;
        } else {
            rv = CMD_OK;
        }
        goto done;
    }

    rv = CMD_USAGE;
done:
    parse_arg_eq_done(&pt);
    return rv;
}

static int
cmd_compare(int unit, args_t *args)
{
    const char *arg;
    int rv;

    arg = ARG_GET(args);
    if (arg == NULL) {
        arg = sysconf_ltsw_ha_state_path();
        if (arg == NULL) {
            cli_out("HA check is off. Please turn it on first.\n");
            return CMD_FAIL;
        }
    }

    rv = appl_ha_unit_state_comp(unit, arg);
    if (SHR_FAILURE(rv)) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

static int
cmd_dump(int unit, args_t *args)
{
    const char *arg;
    int rv;

    arg = ARG_GET(args);
    if (arg == NULL) {
        arg = sysconf_ltsw_ha_state_path();
        if (arg == NULL) {
            cli_out("HA check is off. Please turn it on first.\n");
            return CMD_FAIL;
        }
    }

    rv = appl_ha_unit_state_dump(unit, arg);
    if (SHR_FAILURE(rv)) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*******************************************************************************
 * Public functions
 */

cmd_result_t
cmd_ha(int unit, args_t *args)
{
    const char *arg;

    arg = ARG_GET(args);
    if (arg == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(arg, "alloc") == 0) {
        return cmd_alloc(unit, args);
    } else if (sal_strcasecmp(arg, "file") == 0) {
        return cmd_file(unit, args);
    } else if (sal_strcasecmp(arg, "check") == 0) {
        return cmd_check(unit, args);
    } else if (sal_strcasecmp(arg, "compare") == 0) {
        return cmd_compare(unit, args);
    } else if (sal_strcasecmp(arg, "dump") == 0) {
        return cmd_dump(unit, args);
    }

    return CMD_USAGE;
}

/*! Help for CLI HA command. */
char cmd_ha_usage[] =
    "\n"
    "\tHA alloc status [<component> ...]\n"
    "\tHA file info\n"
    "\tHA check [Path=<ha path>] [on|off]\n"
    "\tHA compare <ha path>\n"
    "\tHA dump <ha path>\n"
    ;


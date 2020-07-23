/*! \file ha_cmd.c
 *
 * CLI command to display HA memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <sal/appl/sal.h>

#include <appl/diag/ltsw/ha.h>
#include <appl/diag/shell.h>
#include <appl/diag/sysconf_ltsw.h>

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <pwd.h>
#include <dirent.h>
#include <shr/shr_error.h>
#include <shr/shr_ha.h>
#include <shr/shr_pb.h>
#include <bcmdrd_config.h>

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
    bool gen_mem_inc = true;
    bool fmt_hdr = true;

    pb = shr_pb_create();

    arg = ARG_GET(args);
    if (arg == NULL) {
        /* Format the memory usage of all components */
        appl_ltsw_ha_mem_usage(unit, gen_mem_inc, pb, fmt_hdr);
    } else {
        /* Format the memory usage of the specified component */
        do {
            appl_ltsw_ha_comp_mem_usage(unit, arg, gen_mem_inc, pb, fmt_hdr);
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
        /* Display the HA file location */
        rv = appl_ltsw_ha_gen_file_name_get(buf, sizeof(buf));
        if (SHR_SUCCESS(rv)) {
            cli_out("HA generic file:   %s\n", buf);
        } else {
            cli_out("ERROR: Failed to get HA generic file name (%d).\n", rv);
        }
        rv = appl_ltsw_ha_file_name_get(unit, buf, sizeof(buf));
        if (SHR_SUCCESS(rv)) {
            cli_out("HA file for SDKLT: %s\n", buf);
        } else {
            cli_out("ERROR: Failed to get HA file name (%d) for SDKLT.\n", rv);
        }
        rv = appl_ltsw_ha_file_name_get(unit + BCMDRD_CONFIG_MAX_UNITS, buf,
                                        sizeof(buf));
        if (SHR_SUCCESS(rv)) {
            cli_out("HA file for SDK:   %s\n", buf);
        } else {
            cli_out("ERROR: Failed to get HA file name (%d) for SDK.\n", rv);
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}

static int
cmd_compare(int unit, args_t *args)
{
    const char *arg;
    int rv;

    arg = ARG_GET(args);
    if (arg == NULL) {
        arg = sysconf_ltsw_ha_state_path();
    }

    rv = appl_ltsw_ha_unit_state_comp(unit, arg);
    if (SHR_FAILURE(rv)) {
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*******************************************************************************
 * Public functions
 */

cmd_result_t
cmd_ltsw_ha(int unit, args_t *args)
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
    } else if (sal_strcasecmp(arg, "compare") == 0) {
        return cmd_compare(unit, args);
    }

    return CMD_USAGE;
}

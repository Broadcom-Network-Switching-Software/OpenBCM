/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:     trunk.c
 * Purpose:  BCMX trunking commands
 */

#include <sal/appl/io.h>

#include <bcm/trunk.h>

#include <bcm/error.h>
#include <bcmx/bcmx.h>
#include <bcmx/trunk.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include "bcmx.h"
#include <appl/diag/diag.h>

char bcmx_cmd_trunk_usage[] =
    "BCMX Trunk Usages:\n"
    "    trunk create <tid>               - Create a trunk\n"
    "    trunk destroy <tid>              - Destroy a trunk\n"
    "    trunk set <tid> <uport-list>     - Add ports to a trunk\n"
    "    trunk show <tid>                 - Display trunk information\n";

cmd_result_t
bcmx_cmd_trunk(int unit, args_t *args)
{
    char *subcmd, *ch;
    bcmx_trunk_add_info_t tai;
    int lplist_given, tid;
    int rv = BCM_E_NONE;

    if ((subcmd = ARG_GET(args)) == NULL) {
        sal_printf("%s: Subcommand required\n", ARG_CMD(args));
        return CMD_USAGE;
    }

    if ((ch = ARG_GET(args)) == NULL) {
        sal_printf("%s: trunk ID not specified\n", ARG_CMD(args));
        return CMD_FAIL;
    } else {
        tid = parse_integer(ch);
    }

    bcmx_trunk_add_info_t_init(&tai);

    if ((ch = ARG_GET(args)) == NULL) {
        lplist_given = 0;
    } else {
        if (bcmx_lplist_parse(&tai.ports, ch) < 0) {
            sal_printf("%s: could not parse plist: %s\n", ARG_CMD(args), ch);
            bcmx_trunk_add_info_t_free(&tai);
            return CMD_FAIL;
        }
	lplist_given = 1;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
	if (tid < 0) {
	    rv = bcmx_trunk_create(&tid);
	    if (rv >= 0) {
		sal_printf("%s: trunk %d created\n", ARG_CMD(args), tid);
	    }
	} else {
	    rv = bcmx_trunk_create_id(tid);
	}
        goto done;
    }

    if (sal_strcasecmp(subcmd, "destroy") == 0) {
        rv = bcmx_trunk_destroy(tid);
        goto done;
    }

    if (sal_strcasecmp(subcmd, "set") == 0) {
        if (!lplist_given) {
            sal_printf("%s: Bad port list or not specified\n", ARG_CMD(args));
            bcmx_trunk_add_info_t_free(&tai);
            return CMD_USAGE;
        }

	/* default lots of things, lets bcm trunk code choose */
        tai.psc = -1;
        tai.dlf_port = BCMX_LPORT_INVALID;
        tai.mc_port = BCMX_LPORT_INVALID;
        tai.ipmc_port = BCMX_LPORT_INVALID;

        rv = bcmx_trunk_set(tid, &tai);

        goto done;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        bcmx_lplist_free(&tai.ports);

        if ((rv = bcmx_trunk_get(tid, &tai)) == BCM_E_NONE) {
            bcmx_lport_t  lport;
            int           count;

            sal_printf("trunk %d psc=%d ", tid, tai.psc);
            sal_printf("dlf=%s ", bcmx_lport_to_uport_str(tai.dlf_port));
            sal_printf("mc=%s ", bcmx_lport_to_uport_str(tai.mc_port));
            sal_printf("ipmc=%s ", bcmx_lport_to_uport_str(tai.ipmc_port));

            sal_printf("uports=");
            BCMX_LPLIST_ITER(tai.ports, lport, count) {
                sal_printf("%s%s", count == 0 ? "" : ",",
                           bcmx_lport_to_uport_str(lport));
            }
            if (count == 0) {
                sal_printf("none\n");
            } else {
                sal_printf("\n");
            }
        }
        goto done;
    }

    sal_printf("%s: ERROR: unknown subcommand: %s\n", ARG_CMD(args), subcmd);
    bcmx_lplist_free(&tai.ports);
    return CMD_USAGE;
	
 done:
    if (rv < 0) {
        sal_printf("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
        bcmx_trunk_add_info_t_free(&tai);
	    return CMD_FAIL;
    }

    bcmx_trunk_add_info_t_free(&tai);

    return CMD_OK;
}

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:     stg.c
 * Purpose:  BCMX STG CLI
 */

#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <bcm/vlan.h>
#include <bcm/error.h>
#include <bcmx/bcmx.h>
#include <bcmx/stg.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include "bcmx.h"
#include <appl/diag/diag.h>

char bcmx_cmd_stg_usage[] =
    "BCMX STG Usages:\n"
    "    stg {create | destroy} <stg id>     - Create/destroy a STG\n"
    "    stg add <stg id> <vlan_id> [...]    - Add VLAN(s) to STG\n"
    "    stg remove <stg id> <vlan_id> [...] - Remove VLAN(s) from STG\n"
    "    stg stp <stg id> <uport-list>       - Show STP state of port\n"
    "    stg default <stg id>                - Set default STG\n"
    "    stg show                            - Display STG info\n"
    "NOTE: Don't attempt to add VLAN 1 to STG groups!\n";

#define _BAD_SID  -1

cmd_result_t
bcmx_cmd_stg(int unit, args_t *args)
{
    char *subcmd, *ch;
    bcm_stg_t sid;
    vlan_id_t vid;
    int stp_state;
    int rv = BCM_E_NONE;
    cmd_result_t cmd_rv = CMD_OK;

    if ((subcmd = ARG_GET(args)) == NULL) {
        sal_printf("Subcommand required\n");
        return CMD_USAGE;
    }

    /* Get STG id */
    if ((ch = ARG_GET(args)) == NULL) {
        sid = _BAD_SID;
    } else {
        sid = parse_integer(ch);
    }

    /* stg create <stg id> */
    if (sal_strcasecmp(subcmd, "create") == 0) {
        if (sid == _BAD_SID) {
            sal_printf("STG: stg ID not specified\n");
            return CMD_FAIL;
        }

        rv = bcmx_stg_create_id(sid);
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        /* stg destroy <stg id> */
        if (sid == _BAD_SID) {
            sal_printf("STG: stg ID not specified\n");
            return CMD_FAIL;
        }

        rv = bcmx_stg_destroy(sid);
    } else if (sal_strcasecmp(subcmd, "add") == 0) {
        /* stg add <stg id> <vlan_id> [...] */
        if (sid == _BAD_SID) {
            sal_printf("STG: stg ID not specified\n");
            return CMD_FAIL;
        }

        while ((ch = ARG_GET(args)) != NULL) {
            vid = parse_integer(ch);
            rv = bcmx_stg_vlan_add(sid, vid);
            if (rv < 0) {
                return CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(subcmd, "remove") == 0) {
        /* stg remove <stg id> <vlan_id> [...] */
        if (sid == _BAD_SID) {
            sal_printf("STG: stg ID not specified\n");
            return CMD_FAIL;
        }

        while ((ch = ARG_GET(args)) != NULL) {
            vid = parse_integer(ch);
            rv = bcmx_stg_vlan_remove(sid, vid);
            if (rv < 0) {
                return CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(subcmd, "stp") == 0) {
        bcmx_lplist_t ports;
        int count;
        bcmx_lport_t lport;

        /* stg stp <stg id> <uport-list> */
        if (sid == _BAD_SID) {
            sal_printf("STG: stg ID not specified\n");
            return CMD_USAGE;
        }

        if ((ch = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }

        bcmx_lplist_init(&ports, 0, 0);

        if (bcmx_lplist_parse(&ports, ch) < 0) {
            cli_out("%s: Error: could not parse portlist: %s\n",
                    ARG_CMD(args), ch);
        } else if (ports.lp_last < 0) {
            cli_out("No ports specified.\n");
        } else {
            BCMX_LPLIST_IDX_ITER(&ports, lport, count) {
                rv = bcmx_stg_stp_get(sid, lport, &stp_state);
                if (rv == BCM_E_NONE) {
                    sal_printf("port %s STP state for STG %d = %d\n",
                               bcmx_lport_to_uport_str(lport), sid, stp_state);
                } else {
                    sal_printf("Error getting STP state for port %s. %s\n",
                               bcmx_lport_to_uport_str(lport), bcm_errmsg(rv));
                }
            }
        }
        bcmx_lplist_free(&ports);
    } else if (sal_strcasecmp(subcmd, "default") == 0) {
        /* stg default <stg id> */
        if (sid == _BAD_SID) {
            sal_printf("STG: stg ID not specified\n");
            return CMD_USAGE;
        }

        rv = bcmx_stg_default_set(sid);
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        /* stg show */
        bcm_stg_t *stg_list;
        int count, i;

        /* show list of all STG */
        rv = bcmx_stg_list(&stg_list, &count);
        if (rv == 0) {
            sal_printf("Existing STGs:");
            for (i = 0; i < count; i++) {
                sal_printf(" %d", stg_list[i]);
            }
            sal_printf("\n");
            rv = bcmx_stg_list_destroy(stg_list, count);
            if (rv != BCM_E_NONE) {
                sal_printf("Error:  STG list destroy failed %d: %s\n",
                           rv, bcm_errmsg(rv));
                rv = BCM_E_NONE;
            }
        }
    }

    if (rv < 0) {
        sal_printf("Error: Command returned %s\n", bcm_errmsg(rv));
        cmd_rv = CMD_FAIL;
    }

    return cmd_rv;
}


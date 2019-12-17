/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:    mirror.c
 * Purpose: BCMX mirroring CLI commands
 */

#include <bcm/mirror.h>
#include <bcm/error.h>
#include <bcmx/bcmx.h>
#include <bcmx/mirror.h>

#include <sal/appl/io.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include "bcmx.h"
#include <appl/diag/diag.h>

char bcmx_cmd_mirror_usage[] =
    "Get/set mirroring configuration.  \nSyntax:\n"
    "  mirror                      -- Show the mirror mode and dest port\n"
    "  mirror [init | disable | enable <options>]\n"
    "                              -- Set the mirroring configuration\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "Enable Options:\n"                  
    "  Mode=[L2|L3|Off]              -- Set the mirror mode\n"
    "  DESTport=<user-port>          -- Set the mirror destination port\n"
    "  IngressPorts=<user-port-list> -- Enable/disable ingress mirroring\n"
    "  EgressPorts=<user-port-list>  -- Enable/disable egress mirroring\n"
#endif
    "\nNote:  Not all modes are supported in all systems\n";

cmd_result_t
bcmx_cmd_mirror(int unit, args_t *args)
{
    parse_table_t pt;
    int mode;
    int enable = 1;   /* Should ports listed have mirroring enabled or dis */
    char *iport;
    char *eport;
    char *dport;
    bcmx_lplist_t ports;
    bcmx_lport_t lport;
    int count;
    cmd_result_t cmd_rv = CMD_OK;
    int rv;
    char                *subcmd = NULL;
    int mirror_mode;
    bcmx_uport_t uport;

    static char *modeList[] = {"L2", "L3", "Off", NULL};

    if ((subcmd = ARG_GET(args)) == NULL) {
        int first;
        int val;

        bcmx_mirror_mode_get(&mirror_mode);
        bcmx_mirror_to_get(&lport);

        if (mirror_mode == BCM_MIRROR_DISABLE) {
            sal_printf("Mirroring is currently disabled.\n");
        } else {
            sal_printf("Current mirror mode is:  %s.\n",
                mirror_mode == BCM_MIRROR_L2 ? "L2" : "L3");
            if (!BCMX_LPORT_VALID(lport)) {
                sal_printf("Warning:  mirror to port is invalid\n");
            }
        }
        if (BCMX_LPORT_VALID(lport)) {
            sal_printf("Current mirror to uport is %s (lport 0x%x).\n",
                       bcmx_lport_to_uport_str(lport), lport);
        }
        first = TRUE;
        BCMX_FOREACH_LPORT(lport) {
            if (bcmx_mirror_ingress_get(lport, &val) == BCM_E_NONE) {
                if (val) {
                    if (first) {
                        sal_printf("    Ingress uport: %s",
                                   bcmx_lport_to_uport_str(lport));
                        first = FALSE;
                    } else {
                        sal_printf(" %s", bcmx_lport_to_uport_str(lport));
                    }
                }
            }
        }
        if (!first) {
            sal_printf("\n");
        }
        first = TRUE;
        BCMX_FOREACH_LPORT(lport) {
            if (bcmx_mirror_egress_get(lport, &val) == BCM_E_NONE) {
                if (val) {
                    if (first) {
                        sal_printf("    Egress uport: %s",
                                   bcmx_lport_to_uport_str(lport));
                        first = FALSE;
                    } else {
                        sal_printf(" %s", bcmx_lport_to_uport_str(lport));
                    }

                }
            }
        }
        if (!first) {
            sal_printf("\n");
        }

        return CMD_OK;
    }

    if (! sal_strcasecmp(subcmd, "init")) {
        sal_printf("Mirror init returns %d\n", bcmx_mirror_init());
        return CMD_OK;
    } else if (! sal_strcasecmp(subcmd, "disable")) {
        sal_printf("Mirror mode(disable) returns %d\n",
                   bcmx_mirror_mode_set(BCM_MIRROR_DISABLE));
    } else if (! sal_strcasecmp(subcmd, "enable")) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Mode", PQ_NO_EQ_OPT | PQ_MULTI,
                        0, &mode, modeList);
        parse_table_add(&pt, "DESTport", PQ_NO_EQ_OPT | PQ_STRING,
                        0, &dport, NULL);
        parse_table_add(&pt, "IngressPorts", PQ_NO_EQ_OPT | PQ_STRING,
                        0, &iport, NULL);
        parse_table_add(&pt, "EgressPorts", PQ_NO_EQ_OPT | PQ_STRING,
                        0, &eport, NULL);

        /*
         * Check for each operation indicated and carry out sequentially 
         * Be careful about memory leaks with lplist.  Each segment should
         * init and free the list consistently.
         */

        if (parse_arg_eq(args, &pt) < 0) {
            sal_printf("%s: Error: Unknown option: %s\n", ARG_CMD(args),
                       ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return(CMD_FAIL);
        }

        /* Set dest port first to make things happen right */

        if (pt.pt_entries[1].pq_type & PQ_PARSED) { /* Destination port given */
            uport = bcmx_uport_parse(dport, NULL);
            lport = bcmx_uport_to_lport(uport);
            if (lport == BCMX_NO_SUCH_LPORT) {
                sal_printf("%s: bad destination port given: %s\n",
                           ARG_CMD(args), dport);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            rv = bcmx_mirror_to_set(lport);
            if (rv < 0) {
                sal_printf("%s: bcmx_mirror_to_set failed, %d: %s\n",
                           ARG_CMD(args), rv, bcm_errmsg(rv));
                parse_arg_eq_done(&pt);
                return(CMD_FAIL);
            }
        }

        if (pt.pt_entries[2].pq_type & PQ_PARSED) { /* Ingress ports given */
            if (bcmx_lplist_init(&ports, 0, 0) < 0) {
                sal_printf("%s: Could not init port list\n",
                           ARG_CMD(args));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            if (bcmx_lplist_parse(&ports, iport) < 0) {
                sal_printf("%s: Could not parse user port list: %s\n",
                           ARG_CMD(args), iport);
                cmd_rv = CMD_FAIL;
            } else {
                BCMX_LPLIST_ITER(ports, lport, count) {
                    rv = bcmx_mirror_ingress_set(lport, enable);
                    if (rv < 0) {
                        sal_printf("%s: Could not %s ingress mirroring for %s\n",
                                   ARG_CMD(args),
                                   "enable", 
                                   bcmx_lport_to_uport_str(lport));
                        cmd_rv = CMD_FAIL;
                        break;
                    }
                }
            }

            bcmx_lplist_free(&ports);
        }

        if ((cmd_rv == CMD_OK) && (pt.pt_entries[3].pq_type & PQ_PARSED)) {
            /* Engress ports given */
            if (bcmx_lplist_init(&ports, 0, 0) < 0) {
                sal_printf("%s: Could not init port list\n",
                           ARG_CMD(args));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            if (bcmx_lplist_parse(&ports, eport) < 0) {
                sal_printf("Could not parse user port list: %s\n", eport);
                cmd_rv = CMD_FAIL;
            } else {
                BCMX_LPLIST_ITER(ports, lport, count) {
                    rv = bcmx_mirror_egress_set(lport, enable);
                    if (rv < 0) {
                        sal_printf("%s: Could not %s egress mirroring for %s\n",
                                   ARG_CMD(args),
                                   "enable",
                                   bcmx_lport_to_uport_str(lport));
                        cmd_rv = CMD_FAIL;
                        break;
                    }
                }
            }

            bcmx_lplist_free(&ports);
        }

        if (pt.pt_entries[0].pq_type & PQ_PARSED) { /* Mode given */
            switch (mode) {
            case 0: mode = BCM_MIRROR_L2; break;
            case 1: mode = BCM_MIRROR_L2_L3; break;
            case 2: mode = BCM_MIRROR_DISABLE; break;
            default:
                sal_printf("%s: Unknown mode, %d.\n", ARG_CMD(args), mode);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            rv = bcmx_mirror_mode_set(mode);
            if (rv < 0) {
                sal_printf("%s: bcmx_mirror_mode_set failed, %d: %s\n",
                           ARG_CMD(args), rv, bcm_errmsg(rv));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
        }

        parse_arg_eq_done(&pt);

    } else {
        sal_printf("Unknown mirror subcommand: %s\n", subcmd);
        return CMD_USAGE;
    }
    return cmd_rv;
}


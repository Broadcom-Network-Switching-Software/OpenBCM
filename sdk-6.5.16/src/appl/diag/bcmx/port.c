/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmxdiag/port.c
 * Purpose:     Implement bcmx port command
 * Requires:    
 */

#include <bcm/port.h>
#include <bcm/error.h>
#include <shared/bsl.h>
#include <bcmx/port.h>
#include <bcmx/bcmx.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include "bcmx.h"
#include <appl/diag/diag.h>

#define PCMD_DONE(rv) cmd_rv = (rv); goto done

char bcmx_cmd_port_usage[] =
    "    port [-b] <uport-list> [opts]    - List (briefly) port properties\n"
    "    port <uport-list> [opts]         - Set options indicated\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "    Options include:\n"
    "        AutoNeg[=on|off]                 LinkScan[=on|off|hw|sw]\n"
    "        SPeed[=10|100|1000]              FullDuplex[=true|false]\n"
    "        TxPAUse[=on|off]                 RxPAUse[=on|off]\n"
    "        STationADdr[=<macaddr>]          LeaRN[=<learnmode>]\n"
    "        DIScard[=none|untag|all]         VlanFilter[=on|off]\n"
    "        UTPRIrity[=<pri>]                UTVlan[=<vid>]\n"
    "        PortFilterMode[=<value>]         PHymaster[=Master|Slave|Auto]\n"
    "Speed, duplex, pause are the ADVERTISED or FORCED according to autoneg.\n"
    "Speed of zero indicates maximum speed.\n"
    "LinkScan enables automatic scanning for link changes.\n"
    "VlanFilter drops input packets that are not tagged with a valid VLAN\n"
    "Priority sets the priority for untagged packets.\n"
    "PortFilterMode takes a value 0/1/2 for mode A/B/C (see register ref).\n"
    "<vid> is valid VLAN identifier.  <pri> is a value from 0 to 7.\n"
    "<learnmode> flag: bit 0 --> enable learning.  \n"
    "    bit 1 --> forward to CPU.   bit 2 --> switch packet\n"
#endif
    ;

/* Note that 'unit' is left over from the bcm layer.  It should be ignored */
cmd_result_t
bcmx_cmd_port(int unit, args_t *args)
{
    char *ch;
    bcmx_lplist_t ports;
    bcm_port_info_t given;
    bcm_port_info_t info;
    bcm_port_info_t *info_p;
    bcm_port_info_t *port_info = NULL;
    int rv;
    int count;
    bcmx_lport_t lport;
    int brief = FALSE;
    parse_table_t pt;
    uint32 seen = 0;
#if  defined(BCM_ESW_SUPPORT)
    uint32 port_flags;
#endif /*  define (BCM_ESW_SUPPORT) */
    uint32 parsed = 0;
    char uport_str[BCMX_UPORT_STR_LENGTH_DEFAULT];
    bcmx_uport_t uport;
    cmd_result_t cmd_rv = CMD_OK;

    if ((ch = ARG_GET(args)) == NULL) {
        return(CMD_USAGE);
    }

    if (!strcmp("-b", ch)) {
        brief = TRUE;
        if ((ch = ARG_GET(args)) == NULL) {
            return(CMD_USAGE);
        }
    }

    bcmx_lplist_init(&ports, 0, 0);

    if (bcmx_lplist_parse(&ports, ch) < 0) {
        cli_out("%s: Error: could not parse portlist: %s\n",
                ARG_CMD(args), ch);
        PCMD_DONE(CMD_FAIL);
    }

    if (ports.lp_last < 0) {
        cli_out("No ports specified.\n");
        PCMD_DONE(CMD_FAIL);
    }

    if (ARG_CNT(args) == 0) {
        seen = BCM_PORT_ATTR_ALL_MASK & ~BCM_PORT_ATTR_RATE_MASK;
    } else { /* Some arguments given..... */
        /*
         * Parse the arguments.  Determine which ones were actually given.
         */
        parse_table_init(unit, &pt);

#if defined(BCM_ESW_SUPPORT)
        port_parse_setup(0, &pt, &given);
#endif
        
        if (parse_arg_eq(args, &pt) < 0) {
            parse_arg_eq_done(&pt);
            PCMD_DONE(CMD_USAGE);
        }

        if (ARG_CNT(args) > 0) {
            cli_out("%s: Unknown argument %s\n",
                    ARG_CMD(args), ARG_CUR(args));
            parse_arg_eq_done(&pt);
            PCMD_DONE(CMD_FAIL);
        }

        /*
         * Find out what parameters specified.  Record values specified.
         */
#if defined(BCM_ESW_SUPPORT)
        port_parse_mask_get(&pt, &seen, &parsed);
#endif
        parse_arg_eq_done(&pt);

    }

    if (seen && parsed) {
        cli_out("%s: Cannot get and set port properties in one command\n",
                ARG_CMD(args));
        PCMD_DONE(CMD_FAIL);
    } else if (seen) { /* Show selected information */
        bcm_port_config_t config;
        int bcm_unit, bcm_port;

        /****************************************************************
         * DISPLAY PORT INFO
 */
        if (brief) {
            cli_out("  Uport\n");
#if defined(BCM_ESW_SUPPORT)
           /* Coverity
            * The comment at the top of this function says,
            * ignore unit number. -1 is not supposed to be passed
            * to this function, since the function accesses an array
            * with the passed unit number. This function is also
            * called from many other places but with valid unit number.
            * I dont want to modify the functionality of the following f(x).
            * Since this call with -1 is from bcmx,
            * I am choosing to ignore this coverity error.
            */
            /* coverity[negative_returns] */
            brief_port_info_header(-1);
#endif
        } else {
            cli_out("Uport Status (* indicates PHY link up)\n");
        }

        BCMX_LPLIST_ITER(ports, lport, count) {
            uport = BCMX_LPORT_TO_UPORT(lport);
            bcmx_uport_to_str(uport, uport_str, BCMX_UPORT_STR_LENGTH_DEFAULT);
            bcmx_lport_to_unit_port(lport, &bcm_unit, &bcm_port);
            rv = bcm_port_config_get(bcm_unit, &config);

            if (BCM_FAILURE(rv)) {
                cli_out("%s: Could not get port config information: %s\n",
                        ARG_CMD(args), bcm_errmsg(rv));
                PCMD_DONE(CMD_FAIL);
            }

            if (BCM_PBMP_MEMBER(config.cpu, bcm_port)) {
                if (brief) {
                    cli_out("%7s  up     CPU port for unit %d\n",
                            uport_str, bcm_unit);
                } else {
                    cli_out(" *%s CPU port for unit %d\n",
                            uport_str, bcm_unit);
                }
                continue;
            }

#if defined(BCM_ESW_SUPPORT)
            port_flags = seen;
            port_info_init(-1, lport, &info, port_flags);
#endif /*  define (BCM_ESW_SUPPORT) */
            rv = bcmx_port_selective_get(lport, &info);
            if (BCM_FAILURE(rv)) {
                cli_out("%s: Could not get port %s information: %s\n",
                        ARG_CMD(args), uport_str, bcm_errmsg(rv));
                PCMD_DONE(CMD_FAIL);
            }

            if (brief) {
#if defined(BCM_ESW_SUPPORT)
#if defined(BCM_TOMAHAWK_SUPPORT)
                esw_brief_port_info(unit, lport, &info, port_flags);
#else
                brief_port_info(unit, lport, &info, port_flags);
#endif
#endif
            } else {
#if defined(BCM_ESW_SUPPORT)
                disp_port_info(bcm_unit,uport_str, bcm_port, &info, 0, port_flags);
#endif
           }            
        }
        PCMD_DONE(CMD_OK);
    }

    /****************************************************************
     *
     * SET PORT INFO
     *     Allocate all the info structures
     *     Get the info for all the ports listed
     *     Adjust the structures according to input
     *     Write all the data back at once
     *
     * After allocating ports, make sure to goto done on error.
 */

    if (parsed & BCM_PORT_ATTR_LINKSCAN_MASK) {
        /* Map ON --> S/W, OFF--> None */
        if (given.linkscan > 2) {
            given.linkscan -= 3;
        }
    }

    port_info = sal_alloc((ports.lp_last+1) * sizeof(bcm_port_info_t),
                          "bcmx_port");
    if (!port_info) {
        cli_out("Could not allocate enough memory for %d ports\n",
                (ports.lp_last+1));
        PCMD_DONE(CMD_FAIL);
    }

    /* Get and adjust the information for the selected ports */
    BCMX_LPLIST_ITER(ports, lport, count) {
        info_p = &port_info[count];
#if defined(BCM_ESW_SUPPORT)
        port_info_init(-1, lport, info_p, parsed);
#endif

        rv = bcmx_port_selective_get(lport, info_p);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: port info get for port %s returned %s\n",
                    ARG_CMD(args), bcmx_lport_to_uport_str(lport),
                    bcm_errmsg(rv));
            PCMD_DONE(CMD_FAIL);
        }
    }

    BCMX_LPLIST_ITER(ports, lport, count) {
        info_p = &port_info[count];
        /* Get max speed and abilities for the port */
        if ((rv = bcmx_port_speed_max(lport, &given.speed_max)) < 0) {
            cli_out("port parse: Could not get port %s max speed: %s\n",
                    bcmx_lport_to_uport_str(lport), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            continue;
        }
        if ((rv = bcmx_port_speed_max(lport, &given.speed)) < 0) {
            cli_out("port parse: Could not get port %s peed: %s\n",
                    bcmx_lport_to_uport_str(lport), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            continue;
        }
        if ((rv = bcmx_port_ability_get(lport, &given.ability)) < 0) {
            cli_out("port parse: Could not get port %s ability: %s\n",
                    bcmx_lport_to_uport_str(lport), bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            continue;
        }
#if defined(BCM_ESW_SUPPORT)
        rv = port_parse_port_info_set(parsed, &given, info_p);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error: Could not parse port %s info: %s\n",
                    ARG_CMD(args), bcmx_lport_to_uport_str(lport),
                    bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            continue;
        }
#endif
        
        /* If AN is on, do not set speed, duplex, pause */
        if (info_p->autoneg) {
            info_p->action_mask &= ~(BCM_PORT_ATTR_SPEED_MASK |
                  BCM_PORT_ATTR_DUPLEX_MASK | BCM_PORT_ATTR_PAUSE_MASK);
        }

        rv = bcmx_port_selective_set(lport, info_p);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: ERROR: port info get for port %s returned %s\n",
                    ARG_CMD(args), bcmx_lport_to_uport_str(lport),
                    bcm_errmsg(rv));
            cmd_rv = CMD_FAIL;
            break;
        }
    }

done:
    bcmx_lplist_free(&ports);
    if (port_info) {
        sal_free(port_info);
    }
    return cmd_rv;
}


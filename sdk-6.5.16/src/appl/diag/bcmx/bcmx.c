/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx.c
 * Purpose:     First cut at some bcmx commands
 * Requires:
 */

/* Keep dependencies calculated properly by putting this before ifdef. */

#include <sal/core/libc.h>

#include <appl/diag/shell.h>
#include <appl/diag/cmdlist.h>
#include <appl/diag/system.h>

#include <appl/diag/diag.h>

#if defined(INCLUDE_BCMX)

#include <bcm/init.h>

#include <bcmx/bcmx.h>
#include <bcmx/lport.h>
#include <bcmx/lplist.h>

#include "bcmx.h"

#define _PLURAL(x) (((x) == 1) ? "" : "s")


STATIC int
_bcmx_sys_info_lport_display(bcmx_lport_t lport, int lp_cnt)
{
    int         bcm_unit, modid;
    bcm_port_t  bcm_port, modport;
    uint32      flags;
    int         rv;

    rv = bcmx_lport_to_unit_port(lport, &bcm_unit, &bcm_port);
    if (BCM_SUCCESS(rv)) {
        rv = bcmx_lport_to_modid_port(lport, &modid, &modport);
    }

    if (lp_cnt == 0) {
        sal_printf("%6s%8s%-10s%10s.%4s%10s.%4s%8s%s\n",
                   "Uport", "", "Lport", "Unit", "Port",
                   "ModID","Port", "", "Flags");
    }

    if (BCM_SUCCESS(rv)) {
        flags = BCMX_LPORT_FLAGS(lport);

        sal_printf("%6s%8s0x%8.8x%10d.%-4d%10d.%-4d%8s%s%s%s%s%s%s%s\n",
                   bcmx_lport_to_uport_str(lport), "",
                   lport,
                   bcm_unit, bcm_port,
                   modid, modport, "",
                   flags & BCMX_PORT_F_STACK_INT ? "Int-Stk " : "",
                   flags & BCMX_PORT_F_STACK_EXT ? "Ext-Stk " : "",
                   flags & BCMX_PORT_F_CPU ? "CPU " : "",
                   flags & BCMX_PORT_F_HG ? "HG " : "",
                   flags & BCMX_PORT_F_FE ? "FE " : "",
                   flags & BCMX_PORT_F_GE ? "GE " : "",
                   flags & BCMX_PORT_F_XE ? "XE " : "");
    } else {
        /* Error obtaining information */
        sal_printf("%6s%8s0x%8.8x (%s)\n",
                   bcmx_lport_to_uport_str(lport), "",
                   lport,
                   bcm_errmsg(rv));
    }

    return rv;
}

/* Local functions */
STATIC cmd_result_t bcmx_lp_parse(int unit, args_t *args);
STATIC cmd_result_t bcmx_cmd_sys_info(int unit, args_t *args);
STATIC cmd_result_t bcmx_attach_unit(int unit, args_t *args);
STATIC cmd_result_t bcmx_detach_unit(int unit, args_t *args);

extern char         bcmx_test_pkt_usage[];

static char bcmx_cmd_sys_info_usage[] =
    "Usage:\n\t"
    "  sysinfo\n"
    "        - Show information about system\n";

STATIC cmd_result_t
bcmx_cmd_sys_info(int unit, args_t *args)
{
    int          lp_cnt;
    bcmx_lport_t lport;

    lp_cnt = 0;

    /* Iterate over 'lport' */
    BCMX_FOREACH_LPORT(lport) {
        _bcmx_sys_info_lport_display(lport, lp_cnt++);
    }
            
    if (bcmx_unit_count != 0 || lp_cnt != 0) {
        sal_printf("BCMX has %d unit%s registered with %d valid lports\n",
                   bcmx_unit_count, bcmx_unit_count == 1 ? "" : "s", lp_cnt);
    }

    return CMD_OK;
}

static char bcmx_lp_parse_usage[] = "Parameters:  <port list>\n"
    "    Parse a user port list and report\n"
    "    Syntax of port list is a comma separated list of:\n"
    "    [N[-M] | FE | GE | XE | ALL | *]\n";

STATIC cmd_result_t
bcmx_lp_parse(int unit, args_t *args)
{
    bcmx_lplist_t list;
    char *c;
    bcmx_lport_t lport;
    int count;
    cmd_result_t cmd_rv = CMD_OK;
    int lp_cnt;

    if ((c = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    bcmx_lplist_init(&list, 0, 0);

    if (bcmx_lplist_parse(&list, c)) {
        cmd_rv = CMD_FAIL;
    } else {

        sal_printf("List contains %d port%s; allocated %d\n",
                   list.lp_last + 1, (list.lp_last == 0) ? "" : "s",
                   list.lp_alloc);
        lp_cnt = 0;

        BCMX_LPLIST_ITER(list, lport, count) {
            _bcmx_sys_info_lport_display(lport, lp_cnt++);
        }

        if (lp_cnt != list.lp_last + 1) {
            sal_printf("WARNING:  Port count mismatch\n");
        }
        sal_printf("\n");
    }

    bcmx_lplist_free(&list);

    return cmd_rv;
}

/****************************************************************
 *
 * Mapping provided by application to get strings for user ports
 */


STATIC void
bcmx_uport_to_str_default(bcmx_uport_t uport, char *buf, int len)
{
    if (len < BCMX_UPORT_STR_LENGTH_MIN) {
        return;
    }

    sal_sprintf(buf, "%d", PTR_TO_INT(uport));
}

STATIC bcmx_uport_t
bcmx_uport_parse_default(char *buf, char **end)
{
    int port;

    port = strtoul(buf, end, 0);

    return (bcmx_uport_t) INT_TO_PTR(port);
}

bcmx_uport_to_str_f bcmx_uport_to_str = bcmx_uport_to_str_default;
bcmx_uport_parse_f bcmx_uport_parse = bcmx_uport_parse_default;


/*
 * Function:
 *     bcmx_lport_to_uport_str
 * Purpose:
 *     Returns the uport value in char string format for given lport.
 * Parameters:
 *     lport      - BCMX logical port
 * Returns:
 *     String uport
 * Notes:
 *     This routine MUST be called one at a time because of the static
 *     buffer (no re-entrant).
 */
char *
bcmx_lport_to_uport_str(bcmx_lport_t lport)
{
    bcmx_uport_t  uport;
    static char   _uport_str[BCMX_UPORT_STR_LENGTH_DEFAULT];

    if ((uport = BCMX_LPORT_TO_UPORT(lport)) != BCMX_UPORT_INVALID_DEFAULT) {
        bcmx_uport_to_str(uport, _uport_str, BCMX_UPORT_STR_LENGTH_DEFAULT);
    } else {
        sal_sprintf(_uport_str, "-1");
    }

    return _uport_str;
}


/*
 * Function:
 *     cmp_case_prefix
 * Purpose:
 *     Check if buf begins with pref, case-independent.
 * Returns:
 *     TRUE - yes, FALSE - no
 */

STATIC int
cmp_case_prefix(char *buf, char *pref)
{
    int  i;

    for (i = 0; pref[i] != 0; i++) {
        if (buf[i] != pref[i] && buf[i] != pref[i] - 32) {
            return FALSE;
        }
    }

    return TRUE;
}

/*
 * Function:
 *      bcmx_lplist_parse
 * Purpose:
 *      Parse a buffer specifying user ports
 * Parameters:
 *      list     - Pointer to list to store result
 *      buf      - Pointer to buffer holding string desc.
 * Returns:
 *      0 on success; -1 on error
 * Notes:
 *      list must be initialized already.
 *
 *      This uses local translation function and
 *      assumes 1 - N ports.
 *      Syntax of port list:
 *      A TERM is defined as:   [UP1[-UP2] | FE | GE | XE | ALL | *]
 *      UP1 and UP2 are user port refs.  Everything else above is literal.
 *      A PORTLIST is multiple terms separated by commas.
 *      No negation.
 *      A NULL buf is treated as an empty port list
 */

int
bcmx_lplist_parse(bcmx_lplist_t *list, char *buf)
{
    bcmx_lport_t  sport, eport;
    bcmx_uport_t  uport;

    if (list == NULL) {
        return -1;
    }

    if (buf == NULL) {
        return 0;
    }

    while (*buf) {
        if (cmp_case_prefix(buf, "fe")) {
            buf += 2;
            BCMX_FOREACH_LPORT(sport) {
                if (BCMX_LPORT_FLAGS(sport) & BCMX_PORT_F_FE) {
                    bcmx_lplist_add(list, sport);
                }
            }
        } else if (cmp_case_prefix(buf, "ge")) {
            buf += 2;
            BCMX_FOREACH_LPORT(sport) {
                if (BCMX_LPORT_FLAGS(sport) & BCMX_PORT_F_GE) {
                    bcmx_lplist_add(list, sport);
                }
            }
        } else if (cmp_case_prefix(buf, "xe")) {
            buf += 2;
            BCMX_FOREACH_LPORT(sport) {
                if (BCMX_LPORT_FLAGS(sport) & BCMX_PORT_F_XE) {
                    bcmx_lplist_add(list, sport);
                }
            }
        } else if (cmp_case_prefix(buf, "hg")) {
            buf += 2;
            BCMX_FOREACH_LPORT(sport) {
                if (BCMX_LPORT_FLAGS(sport) & BCMX_PORT_F_HG) {
                    bcmx_lplist_add(list, sport);
                }
            }
        } else if (cmp_case_prefix(buf, "cpu")) {
            buf += 3;
            BCMX_FOREACH_LPORT(sport) {
                if (BCMX_LPORT_FLAGS(sport) & BCMX_PORT_F_CPU) {
                    bcmx_lplist_add(list, sport);
                }
            }
        } else if (cmp_case_prefix(buf, "all") || cmp_case_prefix(buf, "*")) {
            if (*buf == '*') {
                buf += 1;
            } else {
                buf += 3;
            }
            BCMX_FOREACH_LPORT(sport) {
                bcmx_lplist_add(list, sport);
            }
        } else { /* Try to parse as user port */
            uport = bcmx_uport_parse(buf, &buf);
            sport = BCMX_UPORT_TO_LPORT(uport);
            if (sport == BCMX_NO_SUCH_LPORT) {
                return -1;
            }
            if (*buf == '-') {  /* uport range */
                buf += 1;
                uport = bcmx_uport_parse(buf, &buf);
                eport = BCMX_UPORT_TO_LPORT(uport);
                if (eport == BCMX_NO_SUCH_LPORT) {
                    return -1;
                }
                if (bcmx_lplist_range(list, sport, eport) < 0) {
                    return -1;
                }
            } else {  /* single uport */
                bcmx_lplist_add(list, sport);
            }
        }
        
        switch (*buf) {
        case ',':
            buf += 1;
            break;
        case 0:     /* end of string */
            return 0;
        default:    /* unexpected character */
            return -1;
        }
    }

    return 0;
}

static char bcmx_attach_unit_usage[] =
    "Usage:  attach [<unit> | * ]\n"
    "    Call the BCMX attach routine.  <unit> is the BCM (dispatch) unit\n"
    "    number; * means attach all connected BCM units\n";

STATIC cmd_result_t
bcmx_attach_unit(int unit, args_t *args)
{
    int rv;
    int bcm_unit;
    char *ch;

    if ((ch = ARG_GET(args)) == NULL) {
        sal_printf("Attach requires BCM unit number\n");
        return CMD_USAGE;
    }

    /* attach *: attach all local units */
    if (ch[0] == '*' && ch[1] == '\0') {
        int max_unit;
        bcm_attach_max(&max_unit);
        for (bcm_unit = 0; bcm_unit <= max_unit; bcm_unit++) {
            if (bcm_attach_check(bcm_unit) < 0) {
                continue;
            }
            rv = bcmx_device_attach(bcm_unit);
            if (rv < 0) {
                sal_printf("Error returned by BCMX attach %d: %s\n", rv,
                           bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        return CMD_OK;
    }

    bcm_unit = parse_integer(ch);

    rv = bcmx_device_attach(bcm_unit);
    if (rv < 0) {
        sal_printf("Error returned by BCMX attach %d: %s\n", rv,
                   bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}


static char bcmx_detach_unit_usage[] =
    "Usage:  detach <unit>\n"
    "    Call the BCMX detach routine.  <unit> is the BCM (dispatch) unit\n"
    "    number.\n";


STATIC cmd_result_t
bcmx_detach_unit(int unit, args_t *args)
{
    int rv;
    int bcm_unit;
    char *ch;

    if ((ch = ARG_GET(args)) == NULL) {
        sal_printf("Attach requires BCM unit number\n");
        return CMD_USAGE;
    }
    bcm_unit = parse_integer(ch);

    rv = bcmx_device_detach(bcm_unit);
    if (rv < 0) {
        sal_printf("Error returned by BCMX detach %d: %s\n", rv,
                   bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

cmd_t bcmx_cmd_list[] = {
/*    Command      |     Function       | Usage String         */
/*  ---------------+--------------------+----------------------*/
/*                 | Short Description                         */

    {"?",            sh_help_short,       sh_help_short_usage,
                     "Display list of commands"},
    {"??",           sh_help,             sh_help_usage,
                     "@help" },
#if defined(INCLUDE_ACL)
    {"ACL",          bcmx_cmd_acl,        bcmx_cmd_acl_usage,
                     "AccessControlLists"},
#endif /* INCLUDE_ACL */
    {"Attach",       bcmx_attach_unit,    bcmx_attach_unit_usage,
                     "Attach BCMX unit."},
    {"BCM",          cmd_mode,            shell_mode_usage,
                     "Set shell mode."},
    {"DETach",       bcmx_detach_unit,    bcmx_detach_unit_usage,
                     "Detach BCMX unit."},
#ifdef BCM_FIELD_SUPPORT
    {"FIELD",        bcmx_cmd_field,     bcmx_cmd_field_usage,
                     "Manage the Field Processor"},
#endif  /* BCM_FIELD_SUPPORT */
#ifdef  INCLUDE_L3
    {"IPMC",         bcmx_cmd_ipmc,       bcmx_cmd_ipmc_usage,
                     "Add/Delete IP Multicast Addresses"},
#endif  /* INCLUDE_L3 */
    {"L2",           bcmx_cmd_l2,         bcmx_cmd_l2_usage,
                     "Display or change L2 table information"},
#ifdef  INCLUDE_L3
    {"L3",           bcmx_cmd_l3,         bcmx_cmd_l3_usage,
                     "Display or change L3 configuration"},
#endif  /* INCLUDE_L3 */
    {"LINKscan",     bcmx_cmd_link,       bcmx_cmd_link_usage,
                     "Enable BCMX linkscan callback"},
    {"LPort",        bcmx_lp_parse,       bcmx_lp_parse_usage,
                     "Parse a user port or port list"},
    {"MCAST",        bcmx_cmd_mcast,       bcmx_cmd_mcast_usage,
                     "Add/Delete L2 Multicast Addresses"},
    {"MIRROR",       bcmx_cmd_mirror,     bcmx_cmd_mirror_usage,
                     "Display or set port mirroring properties"},
    {"PORT",         bcmx_cmd_port,       bcmx_cmd_port_usage,
                     "Display or set port properties"},
    {"RATE",         bcmx_cmd_rate,       bcmx_cmd_rate_usage,
                     "Rate limiting"},
    {"RX",           bcmx_cmd_rx,         bcmx_cmd_rx_usage,
                     "Start/stop simple RX."},
    {"STAT",         bcmx_cmd_stat,       bcmx_cmd_stat_usage,
                     "Display or clear snmp statistics"},
    {"STG",          bcmx_cmd_stg,        bcmx_cmd_stg_usage,
                     "Spanning Tree Group"},
    {"SysInfo",      bcmx_cmd_sys_info,   bcmx_cmd_sys_info_usage,
                     "Report system level information"},
    {"TRUNK",        bcmx_cmd_trunk,     bcmx_cmd_trunk_usage,
                     "Display or set trunking properties"},
    {"TX",           bcmx_cmd_tx,        bcmx_cmd_tx_usage,
                     "Send a packet (simple)."},
    {"VLAN",         bcmx_cmd_vlan,       bcmx_cmd_vlan_usage,
                     "Display or set VLAN membership"},
};

int bcmx_cmd_cnt = COUNTOF(bcmx_cmd_list);

#endif  /* INCLUDE_BCMX */

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:
 * Requires:
 */

#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <sal/core/thread.h>
#include <shared/bsl.h>
#include <bcmx/lplist.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/l2.h>

#include <bcm/l2.h>
#include <bcm/error.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#if defined(INCLUDE_BCMX)

#include "bcmx.h"

static int l2n_report = TRUE;
static int l2n_inserts = 0;
static int l2n_deletes = 0;
static int l2n_insert_calls = 0;
static int l2n_delete_calls = 0;
static int l2n_native_inserts = 0;
static int l2n_native_deletes = 0;
static int l2n_insert = FALSE;

STATIC void
bcmxdiag_l2_notify(bcmx_l2_addr_t *addr, int insert, void *cookie)
{
    char flags_str[200];
    int  flag_str_left;
    int count = 0;
    int i;
    int rv;

    if (insert) {
        l2n_inserts++;
    } else {
        l2n_deletes++;
    }
    if (addr->flags & BCM_L2_NATIVE) {
        if (insert) {
            l2n_native_inserts++;
        } else {
            l2n_native_deletes++;
        }
    }

    if (l2n_report) {
        cli_out("BCMX L2 %s: V=0x%03x MAC=0x%02x%02x%02x"
                "%02x%02x%02x %s uport %s (0x%x) unit %d\n",
                insert ? "ADD" : "DEL",
                addr->vid,
                addr->mac[0], addr->mac[1], addr->mac[2],
                addr->mac[3], addr->mac[4], addr->mac[5],
                insert ? "to" : "from", 
                bcmx_lport_to_uport_str(addr->lport),
                addr->lport, addr->bcm_unit);

        if (addr->flags != 0) {
            flags_str[0] = '\0';
            flag_str_left = sizeof(flags_str) - 1;
            for (i = 0; i < bcm_l2_flags_count; i++) {
                if ((1 << i) & addr->flags) {
                    if (flag_str_left > 0) {
                        strncat(flags_str, bcm_l2_flags_str[i], flag_str_left);
                        flag_str_left -= strlen(bcm_l2_flags_str[i]);
                    }
                    if (flag_str_left > 0) {
                        strncat(flags_str, " ", flag_str_left);
                        flag_str_left -= 1;
                    }
                    if (!(++count % 4)) {
                        if (flag_str_left > 0) {
                            strncat(flags_str, "\n            ",
                                    flag_str_left);
                            flag_str_left -= 13;
                        }
                    }
                }
            }
            cli_out("    Flags 0x%x:  %s\n", addr->flags, flags_str);
        }
    }

    if (l2n_insert) {  /* Propagate native inserts/deletes */
        if (addr->flags & BCM_L2_NATIVE) {
            if (insert) {
                ++l2n_insert_calls;
                rv = bcmx_l2_addr_add(addr, NULL);
                if (rv < 0) {
                    cli_out("ERROR L2 notify insert %d: %s\n", rv,
                            bcm_errmsg(rv));
                }
            } else {
                ++l2n_delete_calls;
                rv = bcmx_l2_addr_delete(addr->mac, addr->vid);
                if (rv < 0) {
                    cli_out("ERROR L2 notify delete %d: %s\n", rv,
                            bcm_errmsg(rv));
                }
            }
        }
    }
}


/*
 * Add an L2 address to the switch, remove an L2 address from the
 * switch, show addresses, and clear addresses on a per port basis.
 */

char bcmx_cmd_l2_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  l2 add | del | remove | notify | age | lookup\n"
#else
    "  l2 add [Ports=<user-port-list>] [MACaddress=<mac>] [Vlanid=<id>]\n"
    "         [Trunk=true|false] [TrunkGroupId=<id>] [STatic=true|false]\n"
    "         [SourceCosPriority=true|false] [DiscardSource=true|false]\n"
    "         [DiscardDest]=true|false] [L3=true|false]\n"
    "         [CPUmirror=true|false]\n"
    "        - Add incrementing L2 addresses associated with port(s)\n"
    "  l2 del [MACaddress=<mac>] [Count=<value>] [Vlanid=<id>]\n"
    "        - Delete incrementing L2 address(s)\n"
    "  l2 remove [Vlanid=<id>] [TrunkGroupId=<id>]\n"
    "        - Remove all L2 addresses with VlanID or TrunkGroupId\n"
    "  l2 notify [start | stop | reg | unreg | [+|-]report [+|-]insert]\n"
    "        - Start, stop, register or unregister L2 change notification\n"
    "          'report' means report learns as they happen\n"
    "          'insert' means call bcmx_l2_addr_add on notify callback\n"
    "  l2 age [<seconds>]\n"
    "        - Set L2 aging interval if <seconds> provided,\n"
    "          otherwise return current aging interval.\n"
    "  l2 lookup MACaddress=<mac> Vlanid=<id>\n"
    "        - Lookup an l2 entry\n"
#ifdef BCM_XGS3_SWITCH_SUPPORT
    "  l2 cache add CacheIndex=<index>\n"
    "         [MACaddress=<mac>] [MACaddressMask=<mac>]\n"
    "         [Vlanid=<id>] [VlanidMask=<mask>]\n"
    "         [Port=<uport>] [Trunk=true|false] [TrunkGroupId=<id>]\n"
    "         [SetPriority=true|false] [PRIOrity=<prio>]\n"
    "         [DiscardDest]=true|false] [CPUmirror=true|false]\n"
    "         [MIRror=true|false] [BPDU=true|false] [L3=true|false]\n"
    "        - Add L2 cache entry\n"
    "  l2 cache del CacheIndex=<index> [Count=<value>]\n"
    "        - Delete L2 cache entry\n"
    "  l2 cache clear\n"
    "        - Delete all L2 cache entries\n"
#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif
    ;

cmd_result_t
bcmx_cmd_l2(int unit, args_t *args)
{
    int                 idx;
    char                *subcmd = NULL;
    static int          initted = 0;
    static sal_mac_addr_t arg_macaddr;
    static int          arg_static=0, arg_trunk=0, arg_l3if=0,
                        arg_scp=0, arg_ds=0, arg_dd=0, arg_count=0,
                        arg_vlan=VLAN_ID_DEFAULT, arg_tgid=0, arg_cbit=0;
    int                 arg_vlan_temp = -1, arg_tgid_temp = -1,
                        arg_age_seconds = 0;
    char                *argage;
    int                 rv = CMD_OK;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcmx_l2_addr_t      l2addr;
    char                *plist_str;
    static bcmx_lplist_t       plist;
    int                 count;
    bcmx_lport_t        lport;

    /*
     * Initialize MAC address field for the user to the first real
     * address which does not conflict
     */
    if (! initted) {
        arg_macaddr[0] = 0;
        arg_macaddr[1] = 0;
        arg_macaddr[2] = 0;
        arg_macaddr[3] = 0;
        arg_macaddr[4] = 0;
        arg_macaddr[5] = 0x1;

        if (bcmx_lplist_init(&plist, 0, 0) < 0) {
            cli_out("%s: Error: could not init plist\n", ARG_CMD(args));
            return CMD_FAIL;
        }

        initted = 1;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (! sal_strcasecmp(subcmd, "add") || ! sal_strcasecmp(subcmd, "+")) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Ports",      PQ_STRING,
                    0, &plist_str, NULL);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                    0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    0, &arg_vlan,       NULL);
        parse_table_add(&pt, "Trunk",           PQ_DFL|PQ_BOOL,
                    0, &arg_trunk,      NULL);
        parse_table_add(&pt, "TrunkGroupID",    PQ_DFL|PQ_INT,
                    0, &arg_tgid,       NULL);
        parse_table_add(&pt, "STatic",          PQ_DFL|PQ_BOOL,
                    0, &arg_static,     NULL);
        parse_table_add(&pt, "SourceCosPriority",       PQ_DFL|PQ_BOOL,
                    0, &arg_scp,        NULL);
        parse_table_add(&pt, "DiscardSource",   PQ_DFL|PQ_BOOL,
                    0, &arg_ds, NULL);
        parse_table_add(&pt, "DiscardDest",PQ_DFL|PQ_BOOL,
                    0, &arg_dd, NULL);
        parse_table_add(&pt, "L3",                      PQ_DFL|PQ_BOOL,
                    0, &arg_l3if,       NULL);
        parse_table_add(&pt, "CPUmirror",       PQ_DFL|PQ_BOOL,
                    0, &arg_cbit,       NULL);


        if (!parse_arg_eq(args, &pt)) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        bcmx_lplist_clear(&plist);

        if (bcmx_lplist_parse(&plist, plist_str) < 0) {
            cli_out("%s: Error: could not parse portlist: %s\n",
                    ARG_CMD(args), plist_str);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);

        /*
         * If we are setting the range, the MAC address is incremented by
         * 1 for each port.
         */
        BCMX_LPLIST_ITER(plist, lport, count) {
            bcmx_l2_addr_t_init(&l2addr, arg_macaddr, arg_vlan);

            l2addr.tgid = arg_tgid;

            /* Configure flags for SDK call */
            if (arg_static)
                l2addr.flags |= BCM_L2_STATIC;
            if (arg_scp)
                l2addr.flags |= BCM_L2_COS_SRC_PRI;
            if (arg_dd)
                l2addr.flags |= BCM_L2_DISCARD_DST;
            if (arg_ds)
                l2addr.flags |= BCM_L2_DISCARD_SRC;
            if (arg_l3if)
                l2addr.flags |= BCM_L2_L3LOOKUP;
            if (arg_trunk)
                l2addr.flags |= BCM_L2_TRUNK_MEMBER;
            if (arg_cbit)
                l2addr.flags |= BCM_L2_COPY_TO_CPU;

            l2addr.lport = lport;
            
            bcmx_dump_l2_addr("ADD: ", &l2addr, NULL);

            if ((rv = bcmx_l2_addr_add(&l2addr, NULL))!= BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            /* Set up for next call */
            increment_macaddr(arg_macaddr, 1);
        }

        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "del") || ! sal_strcasecmp(subcmd, "-")) {

        arg_count = 1;  /* Default count */

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                        0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Count",   PQ_DFL|PQ_INT,
                        0, &arg_count, NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                        0, &arg_vlan,       NULL);

        if (!parseEndOk(args, &pt, &retCode))
            return retCode;

        for (idx = 0; idx < arg_count; idx++) {
            rv = bcmx_l2_addr_get(arg_macaddr, arg_vlan, &l2addr, NULL);

            if (rv < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            bcmx_dump_l2_addr("DEL: ", &l2addr, NULL);

            if ((rv = bcmx_l2_addr_delete(arg_macaddr, arg_vlan)) < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            increment_macaddr(arg_macaddr, 1);
        }

        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "remove") || ! sal_strcasecmp(subcmd, "rem")) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    (void *) -1, &arg_vlan_temp,       NULL);
        parse_table_add(&pt, "TrunkGroupID",    PQ_DFL|PQ_INT,
                    (void *) -1, &arg_tgid_temp,       NULL);
        if (!parseEndOk(args, &pt, &retCode))
            return retCode;

        if (arg_vlan_temp > 0) {
            rv = bcmx_l2_addr_delete_by_vlan(arg_vlan_temp, 0);

            if (rv < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        if (arg_tgid_temp > 0) {
            rv = bcmx_l2_addr_delete_by_trunk(arg_tgid_temp, 0);

            if (rv < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "age")) {

        if ((argage = ARG_GET(args)) == NULL) {
            rv = bcmx_l2_age_timer_get(&arg_age_seconds);

            if (rv < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            } else {
                cli_out("Current age timer setting: %d seconds\n",
                        arg_age_seconds);
            }
        } else {
            arg_age_seconds = sal_ctoi(argage, 0);
            rv = bcmx_l2_age_timer_set(arg_age_seconds);

            if (rv < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }

        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "lookup") || ! sal_strcasecmp(subcmd, "lkup")) {

        /*
         * Lookup an arl entry
         */
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                    0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    0, &arg_vlan,       NULL);
        if (!parseEndOk(args, &pt, &retCode))
            return retCode;

        rv = bcmx_l2_addr_get(arg_macaddr, arg_vlan, &l2addr, NULL);

        if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        } else { /* Found it.  Display. */
            lport = l2addr.lport;
            cli_out("Found on uport %s:", bcmx_lport_to_uport_str(lport));
            bcmx_dump_l2_addr("lookup", &l2addr, NULL);
            return CMD_OK;
        }
    }

    else if (! sal_strcasecmp(subcmd, "dump")) {
        cli_out("L2 dump not yet implemented.\n");
        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "notify")) {
        int rv = BCM_E_NONE;

        if ((subcmd = ARG_GET(args)) == NULL) {
            cli_out("L2 notify: Ins %d (tot %d). "
                    "Del %d (tot %d). Ins calls %d. Del calls %d\n",
                    l2n_native_inserts, l2n_inserts,
                    l2n_native_deletes, l2n_deletes,
                    l2n_insert_calls, l2n_delete_calls);
                         
            return CMD_OK;
        } else {
            while (subcmd) {
                if (!sal_strcasecmp(subcmd, "start")) {
                    rv = bcmx_l2_notify_start();
                    if (rv < 0) {
                        cli_out("Could not start BCMX L2 notify: %s",
                                bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                } else if (!sal_strcasecmp(subcmd, "stop")) {
                    rv = bcmx_l2_notify_stop();
                    if (rv < 0) {
                        cli_out("Could not stop BCMX L2 notify: %s",
                                bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                } else if (!sal_strcasecmp(subcmd, "reg")) {
                    rv = bcmx_l2_notify_register(bcmxdiag_l2_notify, NULL);
                    if (rv < 0) {
                        cli_out("Could not register BCMX L2 notify "
                                "handler: %s", bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                } else if (!sal_strcasecmp(subcmd, "unreg")) {
                    rv = bcmx_l2_notify_unregister(bcmxdiag_l2_notify, NULL);
                    if (rv < 0) {
                        cli_out("Could not register BCMX L2 notify "
                                "handler: %s", bcm_errmsg(rv));
                        return CMD_FAIL;
                    }
                } else if (!sal_strcasecmp(subcmd, "report")) {
                    l2n_report = !l2n_report;
                    cli_out("L2 notify report is %s\n", l2n_report ?
                            "on" : "off");
                } else if (!sal_strcasecmp(subcmd, "+report")) {
                    l2n_report = TRUE;
                } else if (!sal_strcasecmp(subcmd, "-report")) {
                    l2n_report = FALSE;
                } else if (!sal_strcasecmp(subcmd, "insert")) {
                    l2n_insert = !l2n_insert;
                    cli_out("L2 notify insert is %s\n", l2n_insert ?
                            "on" : "off");
                } else if (!sal_strcasecmp(subcmd, "+insert")) {
                    l2n_insert = TRUE;
                } else if (!sal_strcasecmp(subcmd, "-insert")) {
                    l2n_insert = FALSE;
                } else {
                    cli_out("Bad notify subcommand: %s\n", subcmd);
                    return CMD_USAGE;
                }
                subcmd = ARG_GET(args);
            }
        }

        return CMD_OK;
    }

#ifdef BCM_XGS3_SWITCH_SUPPORT
    else if (!sal_strcasecmp(subcmd, "cache") ||
             !sal_strcasecmp(subcmd, "c") ) {
        int cidx;
        char *cachecmd = NULL;
        char str[16];
        static sal_mac_addr_t arg_macaddr, arg_macaddr_mask;
        static int arg_setpri = 0, arg_bpdu = 0, arg_mirror = 0, arg_prio = -1;
        static int arg_lport = 0;
        int arg_vlan_mask, arg_cidx, arg_ccount, idx_max;
        int arg_lookup_class = 0;
        bcmx_l2_cache_addr_t l2caddr;
        char *port_str = NULL;

        if ((cachecmd = ARG_GET(args)) == NULL) {
            return CMD_USAGE;
        }

        /* Masks always default to full mask */
        arg_vlan_mask = BCM_L2_VID_MASK_ALL;
        sal_memset(arg_macaddr_mask, 0xff, sizeof (sal_mac_addr_t));

        arg_cidx = -1;
        arg_ccount = 1;

        if (!sal_strcasecmp(cachecmd, "add") ||
            !sal_strcasecmp(cachecmd, "+") ) {

            parse_table_init(unit, &pt);
            parse_table_add(&pt, "LookupClass",         PQ_DFL|PQ_INT,
                            0, &arg_lookup_class, NULL);
            parse_table_add(&pt, "CacheIndex",          PQ_DFL|PQ_INT,
                            0, &arg_cidx,       NULL);
            parse_table_add(&pt, "MACaddress",          PQ_DFL|PQ_MAC,
                            0, &arg_macaddr,    NULL);
            parse_table_add(&pt, "MACaddressMask",      PQ_DFL|PQ_MAC,
                            0, &arg_macaddr_mask, NULL);
            parse_table_add(&pt, "Vlanid",              PQ_DFL|PQ_HEX,
                            0, &arg_vlan,       NULL);
            parse_table_add(&pt, "VlanidMask",          PQ_DFL|PQ_HEX,
                            0, &arg_vlan_mask,  NULL);
            parse_table_add(&pt, "Port",                PQ_DFL|PQ_STRING,
                            0, &port_str,       NULL);
            parse_table_add(&pt, "Trunk",               PQ_DFL|PQ_BOOL,
                            0, &arg_trunk,      NULL);
            parse_table_add(&pt, "TrunkGroupID",        PQ_DFL|PQ_INT,
                            0, &arg_tgid,       NULL);
            parse_table_add(&pt, "SetPriority",         PQ_DFL|PQ_BOOL,
                            0, &arg_trunk,      NULL);
            parse_table_add(&pt, "PRIOrity",            PQ_DFL|PQ_INT,
                            0, &arg_tgid,       NULL);
            parse_table_add(&pt, "DiscardDest",         PQ_DFL|PQ_BOOL,
                            0, &arg_dd,         NULL);
            parse_table_add(&pt, "L3",                  PQ_DFL|PQ_BOOL,
                            0, &arg_l3if,       NULL);
            parse_table_add(&pt, "CPUmirror",           PQ_DFL|PQ_BOOL,
                            0, &arg_cbit,       NULL);
            parse_table_add(&pt, "MIRror",              PQ_DFL|PQ_BOOL,
                            0, &arg_mirror,     NULL);
            parse_table_add(&pt, "BPDU",                PQ_DFL|PQ_BOOL,
                            0, &arg_bpdu,       NULL);
            if (parse_arg_eq(args, &pt) < 0) {
                cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            if (port_str != NULL) {
                arg_lport = bcmx_uport_to_lport(bcmx_uport_parse(port_str,
                                                                 NULL));
                if (arg_lport == BCMX_NO_SUCH_LPORT) {
                    sal_printf("%s: Error: could not parse port: %s\n",
                               ARG_CMD(args), port_str);
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            }
            parse_arg_eq_done(&pt);    /* No longer needed */

            if (arg_cidx == -1) {
                cli_out("%s ERROR: no cache index specified\n", ARG_CMD(args));
                return CMD_FAIL;
            }
            if (arg_setpri && arg_prio == -1) {
                cli_out("%s ERROR: no priority specified\n", ARG_CMD(args));
                return CMD_FAIL;
            }
            if (!arg_setpri) {
                arg_prio = -1;
            }

            bcmx_l2_cache_addr_t_init(&l2caddr);

            l2caddr.vlan = arg_vlan;
            l2caddr.vlan_mask = arg_vlan_mask;

            ENET_COPY_MACADDR(arg_macaddr, l2caddr.mac);
            ENET_COPY_MACADDR(arg_macaddr_mask, l2caddr.mac_mask);

            l2caddr.lookup_class = arg_lookup_class;

            l2caddr.dest_lport = arg_lport;

            l2caddr.dest_trunk = arg_tgid;

            /* Configure flags for SDK call */
            if (arg_dd || arg_ds)
                l2caddr.flags |= BCM_L2_CACHE_DISCARD;
            if (arg_l3if)
                l2caddr.flags |= BCM_L2_CACHE_L3;
            if (arg_trunk)
                l2caddr.flags |= BCM_L2_CACHE_TRUNK;
            if (arg_cbit)
                l2caddr.flags |= BCM_L2_CACHE_CPU;
            if (arg_bpdu)
                l2caddr.flags |= BCM_L2_CACHE_BPDU;
            if (arg_mirror)
                l2caddr.flags |= BCM_L2_CACHE_MIRROR;

            bcmx_dump_l2_cache_addr("ADD CACHE: ", &l2caddr);

            if ((rv = bcmx_l2_cache_set(arg_cidx, &l2caddr, 
                                        &cidx)) != BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            if (cidx == -1) {
                cli_out(" => using index %d\n", cidx);
            }

            /* Set up for next call */
            increment_macaddr(arg_macaddr, 1);
            if (arg_cidx >= 0) {
                arg_cidx++;
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "del") ||
                 !sal_strcasecmp(cachecmd, "-")) {

            if (ARG_CNT(args)) {
                parse_table_init(unit, &pt);
                parse_table_add(&pt, "CacheIndex",      PQ_DFL|PQ_INT,
                                0, &arg_cidx,   NULL);
                parse_table_add(&pt, "Count",           PQ_DFL|PQ_INT,
                                0, &arg_ccount, NULL);
                if (!parseEndOk(args, &pt, &retCode)) {
                    return retCode;
                }
            }

            if (arg_cidx == -1) {
                cli_out("%s ERROR: no index specified\n", ARG_CMD(args));
                return CMD_FAIL;
            }

            for (idx = 0; idx < arg_ccount; idx++) {
                if ((rv = bcmx_l2_cache_get(arg_cidx, &l2caddr)) < 0) {
                    cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                    return CMD_FAIL;
                }

                bcmx_dump_l2_cache_addr("DEL CACHE: ", &l2caddr);

                if ((rv = bcmx_l2_cache_delete(arg_cidx)) != BCM_E_NONE) {
                    cli_out("%s ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                arg_cidx++;
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "show") ||
                 !sal_strcasecmp(cachecmd, "-d")) {

            if ((rv = bcmx_l2_cache_size_get(&idx_max)) < 0) {
                cli_out("%s: ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            for (idx = 0; idx <= idx_max; idx++) {
                if (bcmx_l2_cache_get(idx, &l2caddr) == BCM_E_NONE) {
                    sal_sprintf(str, " %4d : ", idx);
                    bcmx_dump_l2_cache_addr(str, &l2caddr);
                }
            }

            return CMD_OK;
        }

        else if (!sal_strcasecmp(cachecmd, "clear") ||
                 !sal_strcasecmp(cachecmd, "clr")) {

            if (ARG_CNT(args)) {
                return CMD_USAGE;
            }

            if ((rv = bcmx_l2_cache_delete_all()) != BCM_E_NONE) {
                cli_out("%s ERROR: %s\n", ARG_CMD(args), bcm_errmsg(rv));
                return CMD_FAIL;
            }

            return CMD_OK;
        }

        else {
            return CMD_USAGE;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return CMD_USAGE;
}


void
bcmx_dump_l2_addr(char *pfx, bcmx_l2_addr_t *l2addr, bcmx_lplist_t *port_block)
{
    bcmx_lport_t lport;
    int          count;

    
    cli_out("%smac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d uport=%s",
            pfx,
            l2addr->mac[0], l2addr->mac[1], l2addr->mac[2],
            l2addr->mac[3], l2addr->mac[4], l2addr->mac[5],
            l2addr->vid,
            bcmx_lport_to_uport_str(l2addr->lport));


    if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
        cli_out(" Trunk=%d Rtag=%d", l2addr->tgid, l2addr->trunk_sel);
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        cli_out(" Static");
    }

    if (l2addr->flags & BCM_L2_HIT) {
        cli_out(" Hit");
    }

    if (l2addr->cos_src != 0 || l2addr->cos_dst != 0) {
        cli_out(" COS(src=%d,dst=%d)", l2addr->cos_src, l2addr->cos_dst);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        cli_out(" SCP");
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        cli_out(" CPU");
    }

    if (l2addr->flags & BCM_L2_L3LOOKUP) {
        cli_out(" L3");
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        cli_out(" DiscardSrc");
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        cli_out(" DiscardDest");
    }

    if (l2addr->flags & BCM_L2_MCAST) {
        cli_out(" MCast=%d", l2addr->l2mc_group);
    }

    if (!BCMX_LPLIST_IS_EMPTY(port_block)) {
        BCMX_LPLIST_ITER(*(port_block), lport, count) {
            cli_out("    Uport blocked: %s\n", bcmx_lport_to_uport_str(lport));
            cli_out("\n");
        }
    }

    cli_out("\n");
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
void
bcmx_dump_l2_cache_addr(char *pfx, bcmx_l2_cache_addr_t *l2caddr)
{
    cli_out("%smac=%02x:%02x:%02x:%02x:%02x:%02x",
            pfx,
            l2caddr->mac[0], l2caddr->mac[1], l2caddr->mac[2],
            l2caddr->mac[3], l2caddr->mac[4], l2caddr->mac[5]);

    if (!ENET_MACADDR_BROADCAST(l2caddr->mac_mask)) {
        cli_out("/%02x:%02x:%02x:%02x:%02x:%02x", 
                l2caddr->mac_mask[0], l2caddr->mac_mask[1], l2caddr->mac_mask[2],
                l2caddr->mac_mask[3], l2caddr->mac_mask[4], l2caddr->mac_mask[5]);
    }

    cli_out(" vlan=%d", l2caddr->vlan);

    if (l2caddr->vlan_mask != BCM_L2_VID_MASK_ALL) {
        cli_out("/0x%03x", l2caddr->vlan_mask);
    }

    if (l2caddr->flags & BCM_L2_CACHE_TRUNK) {
        cli_out(" trunk=%d", l2caddr->dest_trunk);
    } else {
        cli_out(" uport=%s", bcmx_lport_to_uport_str(l2caddr->dest_lport));
    }

    if (l2caddr->flags & BCM_L2_CACHE_SETPRI) {
        cli_out(" prio=%d", l2caddr->prio);
    }

    if (l2caddr->flags & BCM_L2_CACHE_BPDU) {
        cli_out(" BPDU");
    }

    if ((l2caddr->flags & BCM_L2_CACHE_CPU) ||
        (l2caddr->dest_lport == BCMX_LPORT_LOCAL_CPU)) {
        cli_out(" CPU");
    }

    if (l2caddr->flags & BCM_L2_CACHE_L3) {
        cli_out(" L3");
    }

    if (l2caddr->flags & BCM_L2_CACHE_DISCARD) {
        cli_out(" Discard");
    }

    if (l2caddr->flags & BCM_L2_CACHE_MIRROR) {
        cli_out(" Mirror");
    }

    cli_out("\n");
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#endif

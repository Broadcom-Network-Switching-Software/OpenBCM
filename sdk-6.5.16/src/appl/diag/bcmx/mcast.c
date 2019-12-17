/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        mcast.c
 * Purpose:
 * Requires:
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/bsl.h>
#include <bcmx/mcast.h>

#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include "bcmx.h"

char bcmx_cmd_mcast_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  mcast init | add | del | padd | pdel | lookup\n"
    "  mcast test add | del\n"
#else
    "  mcast init\n"
    "         - Initialize BMCX mcast stuff\n"
    "  mcast add [Ports=<user-port-list>] [UntagPorts=<user-port-list>]\n"
    "         [MACaddress=<mac>] [Vlanid=<id>] [CosDst=<COS Value>]\n"
    "        - Add L2 Multicast address & associated with port(s)\n"
    "  mcast del [MACaddress=<mac>] [Vlanid=<id>]\n"
    "        - Delete the given L2 Multicast Address\n"
    "  mcast padd [Ports=<user-port-list>] [UntagPorts=<user-port-list>]\n"
    "          [MACaddress=<mac>] [Vlanid=<id>] [CosDst=<COS Value>]\n"
    "        - Add ports to an existing L2 Multicast address\n"
    "  mcast pdel [Ports=<user-port-list>] [MACaddress=<mac>] [Vlanid=<id>]\n"
    "        - Delete ports from an existing L2 Multicast Address\n"
    "  mcast lookup MACaddress=<mac> Vlanid=<id>\n"
    "        - Lookup an l2 Mcast Address entry\n"
    "  mcast test add|del Ports=<Max. number of ports in the system>\n"
    "        - add -> Adds 256 mc incremental addresses\n"
    "        - del -> Deletes 256 mc incremental addresses\n"
#endif
    ;

cmd_result_t
bcmx_cmd_mcast(int unit, args_t *args)
{
    char                *subcmd = NULL;
    static int          initted = 0;
    static sal_mac_addr_t arg_macaddr;
    static int          arg_cos_dst=0,
                        arg_vlan=VLAN_ID_DEFAULT;
    int                 rv = CMD_OK;
    int                 i;
    int                 arg_max_ports;
    int                 u_port_num;
    parse_table_t       pt;
    char                *plist_str;
    char                *puntag_list_str;
    bcmx_mcast_addr_t   mcxaddr;
    char                port_list[256];
    bcmx_lport_t        lport;
    int                 count;

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
        arg_macaddr[5] = 0;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (! sal_strcasecmp(subcmd, "init")) {
	rv = bcmx_mcast_init();

	if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
	}
        return CMD_OK;
    }
    else if (! sal_strcasecmp(subcmd, "add")) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Ports",      PQ_STRING,
                    0, &plist_str, NULL);
        parse_table_add(&pt, "UntagPorts",      PQ_STRING,
                    0, &puntag_list_str, NULL);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                    0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    0, &arg_vlan,       NULL);
        parse_table_add(&pt, "CosDst",       PQ_DFL|PQ_INT,
                    0, &arg_cos_dst,        NULL);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
	    parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

        bcmx_mcast_addr_t_init(&mcxaddr, arg_macaddr, arg_vlan);

        bcmx_lplist_clear(&mcxaddr.ports);

        if (bcmx_lplist_parse(&mcxaddr.ports, plist_str) < 0) {
            sal_printf("%s: Error: could not parse portlist: %s\n",
                         ARG_CMD(args), plist_str);
	    parse_arg_eq_done(&pt);
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_FAIL;
        }

        bcmx_lplist_clear(&mcxaddr.untag_ports);

        if (bcmx_lplist_parse(&mcxaddr.untag_ports, puntag_list_str) < 0) {
            sal_printf("%s: Error: could not parse untag portlist: %s\n",
                         ARG_CMD(args), puntag_list_str);
	    parse_arg_eq_done(&pt);
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_FAIL;
        }
	parse_arg_eq_done(&pt);

	rv = bcmx_mcast_addr_add(&mcxaddr);
	bcmx_mcast_addr_t_free(&mcxaddr);

	if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
	}
        return CMD_OK;
    }
    else if (! sal_strcasecmp(subcmd, "padd")) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Ports",      PQ_STRING,
                    0, &plist_str, NULL);
        parse_table_add(&pt, "UntagPorts",      PQ_STRING,
                    0, &puntag_list_str, NULL);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                    0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    0, &arg_vlan,       NULL);
        parse_table_add(&pt, "CosDst",       PQ_DFL|PQ_INT,
                    0, &arg_cos_dst,        NULL);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
	    parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

	bcmx_mcast_addr_t_init(&mcxaddr, arg_macaddr, arg_vlan);

        bcmx_lplist_clear(&mcxaddr.ports);

        if (bcmx_lplist_parse(&mcxaddr.ports, plist_str) < 0) {
            sal_printf("%s: Error: could not parse portlist: %s\n",
                         ARG_CMD(args), plist_str);
	    parse_arg_eq_done(&pt);
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_FAIL;
        }

        bcmx_lplist_clear(&mcxaddr.untag_ports);

        if (bcmx_lplist_parse(&mcxaddr.untag_ports, puntag_list_str) < 0) {
            sal_printf("%s: Error: could not parse untag portlist: %s\n",
                         ARG_CMD(args), puntag_list_str);
	    parse_arg_eq_done(&pt);
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_FAIL;
        }
	parse_arg_eq_done(&pt);

	rv = bcmx_mcast_port_add(&mcxaddr);
	bcmx_mcast_addr_t_free(&mcxaddr);

	if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
	}
        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "del")) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                    0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    0, &arg_vlan,       NULL);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
	    parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
	parse_arg_eq_done(&pt);

	rv = bcmx_mcast_addr_remove(arg_macaddr, arg_vlan);

	if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
	}

        return CMD_OK;
    }

    else if (! sal_strcasecmp(subcmd, "pdel")) {

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Ports",      PQ_STRING,
                    0, &plist_str, NULL);
        parse_table_add(&pt, "MACaddress",      PQ_DFL|PQ_MAC,
                    0, &arg_macaddr,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                    0, &arg_vlan,       NULL);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
	    parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

	bcmx_mcast_addr_t_init(&mcxaddr, arg_macaddr, arg_vlan);

        bcmx_lplist_clear(&mcxaddr.ports);

        if (bcmx_lplist_parse(&mcxaddr.ports, plist_str) < 0) {
            sal_printf("%s: Error: could not parse portlist: %s\n",
                         ARG_CMD(args), plist_str);
	    parse_arg_eq_done(&pt);
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_FAIL;
        }
	parse_arg_eq_done(&pt);

	rv = bcmx_mcast_port_remove(&mcxaddr);
	bcmx_mcast_addr_t_free(&mcxaddr);

	if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
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

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
	    parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
	parse_arg_eq_done(&pt);

	bcmx_mcast_addr_t_init(&mcxaddr, arg_macaddr, arg_vlan);

        rv = bcmx_mcast_port_get(arg_macaddr, arg_vlan, &mcxaddr);

        if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_FAIL;
        } else { /* Found it.  Display. */
	    cli_out("mcast addr %02x.%02x.%02x.%02x.%02x.%02x vlan %d COS %d\n",
                    mcxaddr.mac[0], mcxaddr.mac[1], mcxaddr.mac[2],
                    mcxaddr.mac[3], mcxaddr.mac[4], mcxaddr.mac[5],
                    mcxaddr.vid, mcxaddr.cos_dst);
	    cli_out("uports:");
	    BCMX_LPLIST_ITER(mcxaddr.ports, lport, count) {
            cli_out(" %s", bcmx_lport_to_uport_str(lport));
	    }
	    cli_out("\n");
	    cli_out("untag uports:");
	    BCMX_LPLIST_ITER(mcxaddr.untag_ports, lport, count) {
            cli_out(" %s", bcmx_lport_to_uport_str(lport));
	    }
	    cli_out("\n");
	    bcmx_mcast_addr_t_free(&mcxaddr);
            return CMD_OK;
        }
    }

    else if (! sal_strcasecmp(subcmd, "test")) {

	if ((subcmd = ARG_GET(args)) == NULL) {
	    return CMD_USAGE;
	}
	if (! sal_strcasecmp(subcmd, "add")) {

	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Ports",      PQ_DFL|PQ_INT,
			    0, &arg_max_ports,NULL);
	    if (parse_arg_eq(args, &pt) < 0) {
		cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
		parse_arg_eq_done(&pt);
		return CMD_FAIL;
	    }
	    parse_arg_eq_done(&pt);

	    arg_macaddr[1] = 1;

	    for (i=0; i<256; i++) {

		memset(port_list, 0, sizeof(port_list));
		u_port_num = i % arg_max_ports;
		sal_sprintf(port_list,"%d,%d,%d,%d",(u_port_num%arg_max_ports)+1,
			((u_port_num+1)%arg_max_ports)+1,
			((u_port_num+2)%arg_max_ports)+1,
			((u_port_num+3)%arg_max_ports)+1);

		arg_vlan = 2;

		bcmx_mcast_addr_t_init(&mcxaddr, arg_macaddr, arg_vlan);

		bcmx_lplist_clear(&mcxaddr.ports);
		bcmx_lplist_clear(&mcxaddr.untag_ports);

		if (bcmx_lplist_parse(&mcxaddr.ports, port_list) < 0) {
		    sal_printf("%s: Error: could not parse portlist: %s\n",
			       ARG_CMD(args), port_list);
		    bcmx_mcast_addr_t_free(&mcxaddr);
		    return CMD_FAIL;
		}

		rv = bcmx_mcast_addr_add(&mcxaddr);
		bcmx_mcast_addr_t_free(&mcxaddr);

		if (rv < 0) {
		    cli_out("ERROR: %s\n", bcm_errmsg(rv));
		    return CMD_FAIL;
		}

		increment_macaddr(arg_macaddr, 1);
	    }

	} else  if (! sal_strcasecmp(subcmd, "del")) {
	    parse_table_init(unit, &pt);
	    parse_table_add(&pt, "Ports",      PQ_DFL|PQ_INT,
			    0, &arg_max_ports,NULL);
	    if (parse_arg_eq(args, &pt) < 0) {
		cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
		parse_arg_eq_done(&pt);
		return CMD_FAIL;
	    }
	    parse_arg_eq_done(&pt);

	    arg_macaddr[1] = 1;

	    for (i=0; i<256; i++) {

		arg_vlan = 2;

		rv = bcmx_mcast_addr_remove(arg_macaddr, arg_vlan);

		if (rv < 0) {
		    cli_out("ERROR: %s for mac = %d\n", bcm_errmsg(rv),i);
		    return CMD_FAIL;
		}

		increment_macaddr(arg_macaddr, 1);
	    }
	}
	return CMD_OK;
    }

    return CMD_USAGE;
}

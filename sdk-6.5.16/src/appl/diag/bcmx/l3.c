/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:     l3.c
 * Purpose:  BCMX L3 commands
 */

#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#include <bcmx/bcmx.h>
#include <bcmx/l3.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#ifdef	INCLUDE_BCMX

#include "bcmx.h"
#include <appl/diag/diag.h>

#ifdef	INCLUDE_L3

STATIC int
_bcmx_l3_intf_show(bcmx_l3_intf_t *intf)
{
    char if_mac_str[SAL_MACADDR_STR_LEN];

    cli_out("%-5s %-5s %-5s %-4s %-18s %-4s %-4s %-5s %-5s\n",
            "INTF", "VRF", "Group", "VLAN", "Source Mac", "MTU", "TTL",
            "Tunnel", "InnerVlan");

    format_macaddr(if_mac_str, intf->l3a_mac_addr);
    cli_out("%-5d %-5d %-5d %-4d %-18s %-4d %-4d %-5d  %-5d\n",
            intf->l3a_intf_id, intf->l3a_vrf, intf->l3a_group, 
            intf->l3a_vid, if_mac_str, intf->l3a_mtu, intf->l3a_ttl, 
            intf->l3a_tunnel_idx, intf->l3a_inner_vlan);

    return BCM_E_NONE;
}


STATIC int
_bcmx_l3_ip_show(bcmx_l3_host_t *info)
{
    char ip_str[IP6ADDR_STR_LEN];
    char if_mac_str[SAL_MACADDR_STR_LEN];
    char *hit;
    char *trunk = " ";
    bcmx_lport_t port;

    if (info->l3a_flags & BCM_L3_IP6) {
        cli_out("%-4s %-42s %-18s %-4s  %-11s %6s  %3s\n",
                "VRF", "IP address", "Mac Address", "INTF",
                "LPORT", "CLASS", "HIT");
    } else {
        cli_out("%-4s %-16s %-18s %-4s  %-11s %6s  %3s\n",
                "VRF", "IP address", "Mac Address", "INTF",
                "LPORT", "CLASS", "HIT");
    }

    hit = (info->l3a_flags & BCM_L3_HIT) ? "y" : "n";
    
    format_macaddr(if_mac_str, info->l3a_nexthop_mac);
    if (BCM_GPORT_IS_SET(info->l3a_lport)) {
        if (BCM_GPORT_IS_TRUNK(info->l3a_lport)) {
            trunk = "t";
        }
        port = info->l3a_lport;

    } else {
        if (info->l3a_flags & BCM_L3_TGID) {
            trunk = "t" ; 
            port = info->l3a_trunk;
        } else {
            port = info->l3a_lport;
        }
    }
    
    if (info->l3a_flags & BCM_L3_IP6) {
        format_ip6addr(ip_str, info->l3a_ip6_addr);
        cli_out("%-4d %-42s %-18s %-4d  0x%8.8x%1s  %-6d  %-3s\n",
                info->l3a_vrf, ip_str, if_mac_str, info->l3a_intf,
                port, trunk, info->l3a_lookup_class, hit);
    } else {
        format_ipaddr(ip_str, info->l3a_ip_addr);
        cli_out("%-4d %-16s %-18s %-4d  0x%8.8x%1s  %-6d  %-3s\n",
                info->l3a_vrf, ip_str, if_mac_str, info->l3a_intf,
                port, trunk, info->l3a_lookup_class, hit);
    }

    return BCM_E_NONE;
}



char bcmx_cmd_l3_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "    l3 intf create | delete | show ...\n"
    "    l3 ip add | del | show\n"
    "    l3 defip add | del | show\n"
#else
    "    l3 intf create Mac=<mac> Vlan=<vid> [INtf=<n>]\n"
    "                                     - Create an L3 interface\n"
    "                 Interface index may be optionally specified\n"
    "    l3 intf delete <intf>            - Destroy an L3 interface\n"
    "    l3 intf show INtf=<n>            - Show L3 interface\n"

    "    l3 ip add IP=<ipaddr> Mac=<nexthop_mac> Port=<uport> INt=<intf>\n"
    "                                     - Add L3 cache entry\n"
    "    l3 ip del IP=<ipaddr>            - Remove L3 cache entry\n"
    "    l3 ip show IP=<ipaddr>           - Display L3 cache entry\n"

    "    l3 defip add IP=<ipaddr> MaSk=<mask>\n"
    "                 Mac=<nexthop_mac> Port=<uport> INt=<intf>\n"
    "                                     - Add L3 route\n"
    "    l3 defip del IP=<ipaddr> MaSk=<mask>\n"
    "                                     - Delete L3 route\n"
    "    l3 defip show                    - Show L3 routes\n"
#endif
    ;

cmd_result_t
bcmx_cmd_l3(int unit, args_t *args)
{
    parse_table_t pt;
    char *table, *subcmd, *ch;
    sal_mac_addr_t mac;
    int vid;	/* not a bcm_vlan_t due to parsing uint16 vs uint32 */
    int intf;
    int rv = BCM_E_NONE;
    cmd_result_t cmd_rv;

    if ((table = ARG_GET(args)) == NULL) {
        sal_printf("Table required\n");
        return CMD_USAGE;
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        sal_printf("Subcommand required\n");
        return CMD_USAGE;
    }

    /*
     * L3 interface table
     */
    if (sal_strcasecmp(table, "intf") == 0 ||
	sal_strcasecmp(table, "if") == 0 ||
	sal_strcasecmp(table, "interface") == 0) {

        /* l3 intf create Mac=<mac> Vlan=<vid> */
        if (sal_strcasecmp(subcmd, "create") == 0 ||
	    sal_strcasecmp(subcmd, "add") == 0) {
            bcmx_l3_intf_t  intrf;
            int intf_idx = -1;

            parse_table_init(0, &pt);
            parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, &mac, 0);
            parse_table_add(&pt, "Vlan", PQ_DFL | PQ_INT, 0, &vid, 0);
            parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, &intf_idx, 0);
            if (!parseEndOk(args, &pt, &cmd_rv)) {
                return cmd_rv;
            }

            bcmx_l3_intf_t_init(&intrf);
            sal_memcpy(intrf.l3a_mac_addr, mac, sizeof(bcm_mac_t));
            intrf.l3a_vid = vid;
            intrf.l3a_intf_id = intf_idx;
            intrf.l3a_flags |= BCM_L3_ADD_TO_ARL;
            intrf.l3a_flags   |= BCM_L3_WITH_ID;
            if (intf_idx < 0) { /* Not specified */
                rv = bcmx_l3_intf_create(&intrf);
                if (rv == BCM_E_NONE) {
                    intf_idx = rv;
                }
            } else {
                rv = bcmx_l3_intf_create(&intrf);
            }

            if (rv < 0) {
                sal_printf("ERROR: l3 interface create %sfailed: %s\n",
			   intf_idx < 0 ? "" : "with id ", bcm_errmsg(rv));
                return CMD_FAIL;
            }
	    sal_printf("l3 interface %d created\n", intf_idx);
	    return CMD_OK;
        }

        /* l3 intf delete <intf> */
        if (sal_strcasecmp(subcmd, "delete") == 0 ||
	    sal_strcasecmp(subcmd, "del") == 0 ||
	    sal_strcasecmp(subcmd, "destroy") == 0) {
            bcmx_l3_intf_t  intrf;
            if ((ch = ARG_GET(args)) == NULL) {
                return CMD_USAGE;
            }
            intf = parse_integer(ch);

            bcmx_l3_intf_t_init(&intrf);
            intrf.l3a_intf_id = intf;

            rv = bcmx_l3_intf_delete(&intrf);
            if (rv < 0) {
                sal_printf("ERROR: l3 interface destroy failed: %s\n",
			   bcm_errmsg(rv));
                return CMD_FAIL;
            }
	    return CMD_OK;
        }

        /* l3 intf show INtf=<intf> */
        if (sal_strcasecmp(subcmd, "show") == 0) {
            bcmx_l3_intf_t  intf;
            int             intf_id;

            parse_table_init(0, &pt);
            parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void*)&intf_id, 0);

            if (!parse_arg_eq(args, &pt)) {
                sal_printf("%s: Unknown options: %s\n",
                           ARG_CMD(args), ARG_CUR(args));
                parse_arg_eq_done(&pt);
                return(CMD_FAIL);
            }

            parse_arg_eq_done(&pt);   /* No longer needed */

            bcmx_l3_intf_t_init(&intf);
            intf.l3a_intf_id = intf_id;

            rv = bcmx_l3_intf_get(&intf);
            if (rv < 0) {
                sal_printf("BCMX L3 intf get failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            _bcmx_l3_intf_show(&intf);
            return CMD_OK;
        }

	sal_printf("%s: unknown %s subcommand %s\n",
		   ARG_CMD(args), table, subcmd);
	return CMD_USAGE;
    }

    /*
     * L3 IP cache table
     */
    if (sal_strcasecmp(table, "ip") == 0) {
        ip_addr_t ipaddr;
        char *lpstr;
        bcmx_uport_t uport;
        bcmx_lport_t lport;

        /* l3 ip add IP=<ipaddr> Mac=<nexthop_mac> Port=<uport> INt=<intf> */
        if (sal_strcasecmp(subcmd, "add") == 0 ||
            sal_strcasecmp(subcmd, "create") == 0) {
            bcmx_l3_host_t l3ip;
            int tid;

            parse_table_init(0, &pt);
            parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ipaddr, NULL);
            parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, &mac, NULL);
            parse_table_add(&pt, "Port", PQ_STRING, 0, &lpstr, NULL);
            parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&intf, NULL);
            tid = -1;
            parse_table_add(&pt, "Trunk", PQ_DFL | PQ_INT, 0, (void *)&tid, NULL);
            if (!parse_arg_eq(args, &pt)) {
                sal_printf("%s: Unknown options: %s\n",
                           ARG_CMD(args), ARG_CUR(args));
		parse_arg_eq_done(&pt);
                return(CMD_FAIL);
            }

	    uport = bcmx_uport_parse(lpstr, NULL);
	    lport = BCMX_UPORT_TO_LPORT(uport);
            if (lport == BCMX_NO_SUCH_LPORT) {
                sal_printf("%s: bad uport given: %s\n", ARG_CMD(args), lpstr);
		parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            
	    bcmx_l3_host_t_init(&l3ip);
            l3ip.l3a_ip_addr = ipaddr;
            l3ip.l3a_intf = intf;
            sal_memcpy(l3ip.l3a_nexthop_mac, mac, sizeof(mac));
            l3ip.l3a_lport = lport;
            l3ip.l3a_flags = 0;
            if (tid != -1) {
                l3ip.l3a_trunk = tid;
                l3ip.l3a_flags |= BCM_L3_TGID;
            }

            rv = bcmx_l3_host_add(&l3ip);
            if (rv < 0) {
                sal_printf("BCMX L3 add failed: %s\n", bcm_errmsg(rv));
		parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
	    parse_arg_eq_done(&pt);
	    return CMD_OK;
        }

        /* l3 ip del IP=<ipaddr> */
        if (sal_strcasecmp(subcmd, "delete") == 0 ||
	    sal_strcasecmp(subcmd, "del") == 0 ||
	    sal_strcasecmp(subcmd, "destroy") == 0) {
            bcmx_l3_host_t l3ip;
            parse_table_init(0, &pt);
            parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ipaddr, 0);
            if (!parseEndOk(args, &pt, &cmd_rv)) {
                return cmd_rv;
            }
            bcmx_l3_host_t_init(&l3ip);
            l3ip.l3a_ip_addr = ipaddr;
            l3ip.l3a_flags = 0;

            rv = bcmx_l3_host_delete(&l3ip);
            if (rv < 0) {
                sal_printf("BCMX L3 delete failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
	    return CMD_OK;
        }

        /* l3 ip show IP=<ipaddr> */
        if (sal_strcasecmp(subcmd, "show") == 0) {
            bcmx_l3_host_t l3ip;

            parse_table_init(0, &pt);
            parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ipaddr, NULL);

            if (!parse_arg_eq(args, &pt)) {
                sal_printf("%s: Unknown options: %s\n",
                           ARG_CMD(args), ARG_CUR(args));
                parse_arg_eq_done(&pt);
                return(CMD_FAIL);
            }

            parse_arg_eq_done(&pt);   /* No longer needed */

            bcmx_l3_host_t_init(&l3ip);
            l3ip.l3a_ip_addr = ipaddr;
            l3ip.l3a_flags = 0;

            rv = bcmx_l3_host_find(&l3ip);
            if (rv < 0) {
                sal_printf("BCMX L3 find failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            _bcmx_l3_ip_show(&l3ip);
            return CMD_OK;
        }

	sal_printf("%s: unknown %s subcommand %s\n",
		   ARG_CMD(args), table, subcmd);
	return CMD_USAGE;
    }

    /*
     * L3 DEFIP route table
     */
    if (sal_strcasecmp(table, "defip") == 0) {
        bcmx_l3_route_t l3route;
        ip_addr_t ipaddr, mask;
        char *lpstr;
        bcmx_lport_t lport;
        bcmx_uport_t uport;

        /*
         * l3 defip add IP=<ipaddr> MaSk=<mask>
         *                          Mac=<nexthop_mac> Port=<uport> INt=<intf>
         */
        if (sal_strcasecmp(subcmd, "add") == 0 ||
	    sal_strcasecmp(subcmd, "create") == 0) {
            parse_table_init(0, &pt);
            parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, &ipaddr, NULL);
            parse_table_add(&pt, "MaSk", PQ_DFL | PQ_IP, 0, &mask, NULL);
            parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, &mac, NULL);
            parse_table_add(&pt, "Port", PQ_STRING, 0, &lpstr, NULL);
            parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, &intf, NULL);
            if (!parse_arg_eq(args, &pt)) {
                sal_printf("%s: Unknown options: %s\n",
                           ARG_CMD(args), ARG_CUR(args));
		parse_arg_eq_done(&pt);
                return(CMD_FAIL);
            }

	    uport = bcmx_uport_parse(lpstr, NULL);
	    lport = BCMX_UPORT_TO_LPORT(uport);
            if (lport == BCMX_NO_SUCH_LPORT) {
                sal_printf("%s: bad uport given: %s\n", ARG_CMD(args), lpstr);
		parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

	    sal_memset(&l3route, 0, sizeof(l3route));
            l3route.l3a_subnet = ipaddr;
            l3route.l3a_ip_mask = mask;
            l3route.l3a_intf = intf;
            sal_memcpy(l3route.l3a_nexthop_mac, mac, sizeof(mac));
            l3route.l3a_lport = lport;
            l3route.l3a_flags = 0;

            rv = bcmx_l3_route_add(&l3route);
            if (rv < 0) {
                sal_printf("BCMX L3 defip add failed: %s\n", bcm_errmsg(rv));
		parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
	    parse_arg_eq_done(&pt);
	    return CMD_OK;
        }

        /* l3 defip del IP=<ipaddr> MaSk=<mask> */
        if (sal_strcasecmp(subcmd, "delete") == 0 ||
	    sal_strcasecmp(subcmd, "del") == 0 ||
	    sal_strcasecmp(subcmd, "destroy") == 0) {
            parse_table_init(0, &pt);
            parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, &ipaddr, NULL);
            parse_table_add(&pt, "MaSk", PQ_DFL | PQ_IP, 0, &mask, NULL);
            if (!parseEndOk(args, &pt, &cmd_rv)) {
                return cmd_rv;
            }

	    sal_memset(&l3route, 0, sizeof(l3route));
            l3route.l3a_subnet = ipaddr;
            l3route.l3a_ip_mask = mask;

            rv = bcmx_l3_route_delete(&l3route);
            if (rv < 0) {
                sal_printf("BCMX L3 defip delete failed: %s\n",
			   bcm_errmsg(rv));
                return CMD_FAIL;
            }
	    return CMD_OK;
        }

        /* l3 defip show */
        if (sal_strcasecmp(subcmd, "show") == 0) {
            sal_printf("To be implemented\n");
            return CMD_FAIL;
        }

	sal_printf("%s: unknown %s subcommand %s\n",
		   ARG_CMD(args), table, subcmd);
	return CMD_USAGE;
    }


    sal_printf("%s: unknown table %s\n", ARG_CMD(args), table);
    return CMD_USAGE;
}

#endif	/* INCLUDE_L3 */
#endif	/* INCLUDE_BCMX */

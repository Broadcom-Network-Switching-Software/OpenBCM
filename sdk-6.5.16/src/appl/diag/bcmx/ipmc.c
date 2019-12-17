/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ipmc.c
 * Purpose:
 * Requires:
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/bsl.h>
#include <bcm/l3.h>
#include <bcmx/ipmc.h>

#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>

#include "bcmx.h"

#ifdef  INCLUDE_L3

char bcmx_cmd_ipmc_usage[] =
    "Usages:\n"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  ipmc init | add | lookup\n"
    "  ipmc test add\n"
#else
    "  ipmc init\n"
    "         - Initialize BMCX mcast stuff\n"
    "  ipmc add [L2Ports=<user-port-list>] [UntagPorts=<user-port-list>]\n"
    "         [L3Ports=<user-port-list>] [SrcIP=<IP>] [McIP=<IP>]\n"
    "         [CosDst=<COS Value>] [Trunk=<True|False>] [PortTgid=<user-port>]\n"
    "         [Vlanid=<id>] [TTL=<ttl>] [CPUDrop=<True|False>]\n"
    "         [Replace=<True|False>]\n"
    "        - Add IP Multicast address & associated with port(s)\n"
    "  ipmc lookup [SrcIP=<IP>] [McIP=<IP>] [Vlanid=<id>]\n"
    "        - Lookup an IP Mcast Address entry\n"
    "  ipmc test add Ports=<Max. number of ports in the system>\n"
    "        - add -> Adds 256 ipmc incremental addresses\n"
    "  ipmc egr set [Port=<user-port>] [Mac=<macaddr>] [Vlan=<id>] \n"
    "                  [Untag=<True|False>] [Ttl_thresh=<ttl_threshold>]\n"
    "        - Set egress IP Multicast configuration regs for a port\n"
    "  ipmc v6 add [SrcIP=<IP>] [McIP=<IP>] [L2Ports=<user-port-list>]\n"
    "         [UntagPorts=<user-port-list>] [L3Ports=<user-port-list>]\n"
    "         [CosDst=<COS Value>] [Trunk=<True|False>] [PortTgid=<user-port>]\n"
    "         [Vlanid=<id>] [TTL=<ttl>] [CPUDrop=<True|False>]\n"
    "              - Add an IPv6 MCAST entry into ipmc table\n"
    "  ipmc v6 del [SrcIP=<IP>] [McIP=<IP>] [Vlanid=<id>]\n"
    "        - Delete IPv6 MCAST entry from ipmc table\n"
#endif
    ;

cmd_result_t
bcmx_cmd_ipmc(int unit, args_t *args)
{
    char                *subcmd = NULL;
    static int          initted = 0;
    static sal_mac_addr_t arg_macaddr;
    static int          arg_cos_dst = 0,
                        arg_vlan = VLAN_ID_DEFAULT;
    int                 arg_trunk = 0;
    int                 replace = 0;
    int                 arg_ttl = 0;
    uint32              arg_src_ip = 0;
    uint32              arg_mc_ip = 0;
    int                 rv = CMD_OK;
    int                 i, u_port_num;
    int                 arg_max_ports;
    parse_table_t       pt;
    char                *port_str = NULL;
    char                *l2_plist_str = NULL;
    char                *l3_plist_str = NULL;
    char                *puntag_list_str = NULL;
    bcmx_ipmc_addr_t    ipmcxaddr;
    char                port_list[256];
    bcmx_lplist_t       l2_port_list;     /* L2 Port logical port list*/
    bcmx_lplist_t       l3_port_list;     /* L3 logical port list */
    bcmx_lplist_t       l2_untag_list;
    bcmx_uport_t        uport;
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
        bcmx_lplist_init(&l2_port_list, 0, 0);
        bcmx_lplist_init(&l3_port_list, 0, 0);
        bcmx_lplist_init(&l2_untag_list, 0, 0);
    }

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (! sal_strcasecmp(subcmd, "init")) {
        rv = bcmx_ipmc_init();

        if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }

        return CMD_OK;
    }
    else if (! sal_strcasecmp(subcmd, "add")) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "L2Ports",      PQ_STRING,
                        0, &l2_plist_str, NULL);
        parse_table_add(&pt, "UntagPorts",      PQ_STRING,
                        0, &puntag_list_str, NULL);
        parse_table_add(&pt, "L3Ports",      PQ_STRING,
                        0, &l3_plist_str, NULL);
        parse_table_add(&pt, "SrcIP",      PQ_DFL|PQ_IP,
                        0, &arg_src_ip,NULL);
        parse_table_add(&pt, "McIP",      PQ_DFL|PQ_IP,
                        0, &arg_mc_ip,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_INT,
                        0, &arg_vlan,       NULL);
        parse_table_add(&pt, "CosDst",       PQ_DFL|PQ_INT,
                        0, &arg_cos_dst,        NULL);
        parse_table_add(&pt, "Trunk",      PQ_DFL|PQ_BOOL,
                        0, &arg_trunk,NULL);
        parse_table_add(&pt, "PortTgid",   PQ_DFL|PQ_STRING,
                        0, &port_str,       NULL);
        parse_table_add(&pt, "Replace",      PQ_DFL|PQ_BOOL,
                        0, &replace,NULL);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }

	    bcmx_ipmc_addr_t_init(&ipmcxaddr);
        ipmcxaddr.s_ip_addr = arg_src_ip;
        ipmcxaddr.mc_ip_addr = arg_mc_ip;
        ipmcxaddr.vid = arg_vlan;
        bcmx_lplist_clear(&ipmcxaddr.l2_ports);
        if (bcmx_lplist_parse(&ipmcxaddr.l2_ports, l2_plist_str) < 0) {
            sal_printf("%s: Error: could not parse l2 portlist: %s\n",
                       ARG_CMD(args), l2_plist_str);
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        bcmx_lplist_clear(&ipmcxaddr.l2_untag_ports);
        if (bcmx_lplist_parse(&ipmcxaddr.l2_untag_ports, puntag_list_str) < 0) {
            sal_printf("%s: Error: could not parse untag portlist: %s\n",
                       ARG_CMD(args), puntag_list_str);
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        bcmx_lplist_clear(&ipmcxaddr.l3_ports);
        if (bcmx_lplist_parse(&ipmcxaddr.l3_ports, l3_plist_str) < 0) {
            sal_printf("%s: Error: could not parse l3 portlist: %s\n",
                       ARG_CMD(args), l3_plist_str);
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        ipmcxaddr.ts = arg_trunk;

        if (arg_trunk == 0) { /* non trunk port */
            /* convert user port to logical port */
            if (port_str == NULL) {
                sal_printf("%s: Error: port is required if not a trunk port\n",
                           ARG_CMD(args));
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            uport = bcmx_uport_parse(port_str, NULL);
            lport = bcmx_uport_to_lport(uport);
            if (lport == BCMX_NO_SUCH_LPORT) {
                sal_printf("%s: Error: could not parse port: %s\n",
                           ARG_CMD(args), port_str);
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            ipmcxaddr.port_tgid = lport;
        }
        parse_arg_eq_done(&pt);

        ipmcxaddr.cos = arg_cos_dst;
        if (replace) {
            ipmcxaddr.flags |= BCM_IPMC_REPLACE;
        }
        rv = bcmx_ipmc_add(&ipmcxaddr);
        if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            return CMD_FAIL;
        }
        bcmx_ipmc_addr_t_free(&ipmcxaddr);
        return CMD_OK;
    }
    else if (! sal_strcasecmp(subcmd, "lookup") || ! sal_strcasecmp(subcmd, "lkup")) {
        
        /*
         * Lookup an arl entry
         */
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "SrcIP",      PQ_DFL|PQ_IP,
                        0, &arg_src_ip,NULL);
        parse_table_add(&pt, "McIP",      PQ_DFL|PQ_IP,
                        0, &arg_mc_ip,NULL);
        parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_HEX,
                        0, &arg_vlan,       NULL);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);

        bcmx_ipmc_addr_t_init(&ipmcxaddr);
        rv = bcmx_ipmc_find(&ipmcxaddr);

        if (rv < 0) {
            cli_out("ERROR: %s\n", bcm_errmsg(rv));
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            return CMD_FAIL;
        } else { /* Found it.  Display. */
            cli_out("SourceIP 0x%x MCastIP 0x%x Vlan %d COS %d\n",
                    ipmcxaddr.s_ip_addr, ipmcxaddr.mc_ip_addr,
                    ipmcxaddr.vid, ipmcxaddr.cos);
            cli_out("l2 uports:");
            BCMX_LPLIST_ITER(ipmcxaddr.l2_ports, lport, count) {
                cli_out(" %s", bcmx_lport_to_uport_str(lport));
            }
            cli_out("\n");
            cli_out("l2 untag uports:");
            BCMX_LPLIST_ITER(ipmcxaddr.l2_untag_ports, lport, count) {
                cli_out(" %s", bcmx_lport_to_uport_str(lport));
            }
            cli_out("\n");
            cli_out("l3 uports:");
            BCMX_LPLIST_ITER(ipmcxaddr.l3_ports, lport, count) {
                cli_out(" %s", bcmx_lport_to_uport_str(lport));
            }
            cli_out("\n");
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
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
            arg_mc_ip = 0xe1010100;
            arg_src_ip = 0xa010100;

            for (i=0; i<256; i++) {
                
                arg_vlan = 2;
                bcmx_ipmc_addr_t_init(&ipmcxaddr);
                ipmcxaddr.s_ip_addr = arg_src_ip;
                ipmcxaddr.mc_ip_addr = arg_mc_ip;
                ipmcxaddr.vid = arg_vlan;
                u_port_num = i % arg_max_ports;
                sal_sprintf(port_list,"%d,%d,%d,%d",(u_port_num%arg_max_ports)+1,
                        ((u_port_num+1)%arg_max_ports)+1,
                        ((u_port_num+2)%arg_max_ports)+1,
                        ((u_port_num+3)%arg_max_ports)+1);
                bcmx_lplist_clear(&ipmcxaddr.l2_ports);
                if (bcmx_lplist_parse(&ipmcxaddr.l2_ports,
                                      port_list) < 0) {
                    sal_printf("%s: Error: could not parse portlist: %s\n",
                               ARG_CMD(args), port_list);
                    bcmx_ipmc_addr_t_free(&ipmcxaddr);
                    return CMD_FAIL;
                }
                
                u_port_num = (i+10) % arg_max_ports;
                sal_sprintf(port_list,"%d,%d,%d,%d",(u_port_num%arg_max_ports)+1,
                        ((u_port_num+1)%arg_max_ports)+1,
                        ((u_port_num+2)%arg_max_ports)+1,
                        ((u_port_num+3)%arg_max_ports)+1);
                bcmx_lplist_clear(&ipmcxaddr.l3_ports);
                if (bcmx_lplist_parse(&ipmcxaddr.l3_ports,
                                      port_list) < 0) {
                    sal_printf("%s: Error: could not parse portlist: %s\n",
                               ARG_CMD(args), port_list);
                    bcmx_ipmc_addr_t_free(&ipmcxaddr);
                    return CMD_FAIL;
                }
                
                bcmx_lplist_clear(&ipmcxaddr.l2_untag_ports);
                
                ipmcxaddr.ts = 0;
                ipmcxaddr.port_tgid = (i % arg_max_ports)+1;
                ipmcxaddr.cos = arg_cos_dst;
                
                rv = bcmx_ipmc_add(&ipmcxaddr);
                if (rv < 0) {
                    cli_out("ERROR: %s\n", bcm_errmsg(rv));
                    bcmx_ipmc_addr_t_free(&ipmcxaddr);
                    return CMD_FAIL;
                }
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                arg_src_ip += 1;
                arg_mc_ip += 1;
            }
        }
        return CMD_OK;
    } else if (sal_strcasecmp(subcmd, "egr") == 0) {
        int arg_untag;
        
        if ((subcmd = ARG_GET(args)) == NULL) {
            return CMD_FAIL;
        }
        
        if (sal_strcasecmp(subcmd, "set")) {
            return CMD_FAIL;
        }
        
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Port", PQ_STRING, 0, &port_str, NULL);
        parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, &arg_macaddr, NULL);
        parse_table_add(&pt, "Vlan", PQ_DFL | PQ_INT, 0, &arg_vlan, 0);
        parse_table_add(&pt, "Untag", PQ_DFL | PQ_INT,
                        0, &arg_untag, NULL);
        parse_table_add(&pt, "Ttl_thresh", PQ_DFL | PQ_INT, 0,
                        &arg_ttl, NULL);
        if (!parse_arg_eq(args, &pt)) {
            sal_printf("%s: Unknown options: %s\n",
                       ARG_CMD(args), ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return(CMD_FAIL);
        }
        
        uport = bcmx_uport_parse(port_str, NULL);
        lport = BCMX_UPORT_TO_LPORT(uport);
        if (lport == BCMX_NO_SUCH_LPORT) {
            sal_printf("%s: bad port given: %s\n", ARG_CMD(args), port_str);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);
        rv = bcmx_ipmc_egress_port_set(lport, arg_macaddr, arg_untag,
                                       arg_vlan, arg_ttl);
        if (rv < 0) {
            cli_out("\nERROR %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    else if (sal_strcasecmp(subcmd, "v6") == 0) {
        bcm_ip6_t       ip6_addr, sip6_addr;
        
        if ((subcmd = ARG_GET(args)) == NULL) {
            return CMD_FAIL;
        }
        
        if (sal_strcasecmp(subcmd, "add") == 0) {
            
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "SrcIP", PQ_DFL|PQ_IP6, 0,
                            (void *)&sip6_addr, NULL);
            parse_table_add(&pt, "McIP", PQ_DFL|PQ_IP6, 0,
                            (void *)&ip6_addr, NULL);
            parse_table_add(&pt, "L3Ports", PQ_STRING, 0, &l3_plist_str, NULL);
            parse_table_add(&pt, "L2Ports", PQ_STRING, 0, &l2_plist_str, NULL);
            parse_table_add(&pt, "UntagPorts",      PQ_STRING,
                            0, &puntag_list_str, NULL);
            parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_INT,
                            0, &arg_vlan,       NULL);
            parse_table_add(&pt, "CosDst",       PQ_DFL|PQ_INT,
                            0, &arg_cos_dst,        NULL);
            parse_table_add(&pt, "Trunk",      PQ_DFL|PQ_BOOL,
                            0, &arg_trunk,NULL);
            parse_table_add(&pt, "PortTgid", PQ_INT, 0, &u_port_num, 0);
            
            if (parse_arg_eq(args, &pt) < 0) {
                cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            bcmx_ipmc_addr_t_init(&ipmcxaddr);
            
            sal_memcpy(ipmcxaddr.s_ip6_addr, sip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmcxaddr.mc_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            ipmcxaddr.vid = arg_vlan;
            
            bcmx_lplist_clear(&ipmcxaddr.l2_ports);
            if (bcmx_lplist_parse(&ipmcxaddr.l2_ports, l2_plist_str) < 0) {
                sal_printf("%s: Error: could not parse l2 portlist: %s\n",
                           ARG_CMD(args), l2_plist_str);
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            bcmx_lplist_clear(&ipmcxaddr.l2_untag_ports);
            if (bcmx_lplist_parse(&ipmcxaddr.l2_untag_ports, 
                                  puntag_list_str) < 0) {
                sal_printf("%s: Error: could not parse untag portlist: %s\n",
                           ARG_CMD(args), puntag_list_str);
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            bcmx_lplist_clear(&ipmcxaddr.l3_ports);
            if (bcmx_lplist_parse(&ipmcxaddr.l3_ports, l3_plist_str) < 0) {
                sal_printf("%s: Error: could not parse l3 portlist: %s\n",
                           ARG_CMD(args), l3_plist_str);
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            ipmcxaddr.ts = arg_trunk;
            
            if (arg_trunk == 0) { /* non trunk port */
                ipmcxaddr.port_tgid = u_port_num; 
            }
            parse_arg_eq_done(&pt);
            
            ipmcxaddr.cos = arg_cos_dst;
            ipmcxaddr.flags |= BCM_IPMC_IP6;
            rv = bcmx_ipmc_add(&ipmcxaddr);
            if (rv < 0) {
                cli_out("ERROR: %s\n", bcm_errmsg(rv));
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                return CMD_FAIL;
            }
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            return CMD_OK;
        } else if (sal_strcasecmp(subcmd, "del") == 0) {
            
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "SrcIP", PQ_DFL|PQ_IP6, 0,
                            (void *)&sip6_addr, NULL);
            parse_table_add(&pt, "McIP", PQ_DFL|PQ_IP6, 0,
                            (void *)&ip6_addr, NULL);
            parse_table_add(&pt, "Vlanid",          PQ_DFL|PQ_INT,
                            0, &arg_vlan,       NULL);

            if (parse_arg_eq(args, &pt) < 0) {
                cli_out("%s ERROR: parsing arguments\n", ARG_CMD(args));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            parse_arg_eq_done(&pt);

            bcmx_ipmc_addr_t_init(&ipmcxaddr);
               
            sal_memcpy(ipmcxaddr.s_ip6_addr, sip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmcxaddr.mc_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            ipmcxaddr.vid = arg_vlan;
            ipmcxaddr.flags |= BCM_IPMC_IP6;
            rv = bcmx_ipmc_remove(&ipmcxaddr);
            if (rv < 0) {
                cli_out("ERROR: %s\n", bcm_errmsg(rv));
                bcmx_ipmc_addr_t_free(&ipmcxaddr);
                return CMD_FAIL;
            }
            bcmx_ipmc_addr_t_free(&ipmcxaddr);
            return CMD_OK;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    
    else {
        return CMD_USAGE;
    }
    return CMD_FAIL;
}

#endif  /* INCLUDE_L3 */

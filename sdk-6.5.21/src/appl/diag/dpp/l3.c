/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/



#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>
#include <bcm/l3.h>
#include <bcm/debug.h>
#include <bcm_int/dpp/l3.h>


STATIC cmd_result_t _cmd_dpp_l3_intf(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_egr(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_host(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_route(int unit, args_t *args);
STATIC cmd_result_t _cmd_dpp_l3_ecmp(int unit, args_t *args);

#define CMD_L3_INTF_USAGE                                  \
    "    intf [add | update | delete | find | get ]\n"     \
    "         [SMAC=<mac>] [vid=<id>]\n"                   \
    "         [vrf=<vrf>] [Id=<ifid>]\n"                   \
    "         [ttl=<ttl>] [mtu=<mtu>]\n"                   

#define CMD_L3_EGR_USAGE                                    \
    "    egr  [add | update | delete | find | get ]\n"      \
    "         [Id=<ifid>] [DMAC=<mac>]\n"                   \
    "         [module=<modid>] [port=<port>]\n"             \
    "         [trunk=<tid>] [label=<label>]\n"              \
    "         [encap=<encap>] [fec=<fec>]\n"                    

#define CMD_L3_HOST_USAGE                                  \
    "    host  [add | delete | update | find ]\n"            \
    "          [vrf=<vrf>] ( ip=<ipaddr> ) | \n" \
    "          (v6 ip6=<ipaddr> )\n"          \
    "          [fec=<fec>]\n"

#define CMD_L3_ROUTE_USAGE                                  \
    "    route [add | delete | update | get ]\n"            \
    "          [vrf=<vrf>] ( ip=<ipaddr> mask=<mask>) | \n" \
    "          (v6 ip6=<ipaddr> ip6mask=<mask> )\n"          \
    "          [fec=<fec>]\n"

#define CMD_L3_ECMP_USAGE                                                           \
    "    ecmp [create | add | delete | destroy | get ]\n"                           \
    "          [size=<size>] [fec1=<fec1>] [fec2=<fec2>] ..[fecn=<fecn>] \n"       \
    "          [ecmp_id=<ecmp_id>]\n"
static cmd_t _cmd_dpp_l3_list[] = {
    {"intf",        _cmd_dpp_l3_intf,         "\n" CMD_L3_INTF_USAGE, NULL},
    {"egr",         _cmd_dpp_l3_egr,          "\n" CMD_L3_EGR_USAGE, NULL},
    {"host",        _cmd_dpp_l3_host,         "\n" CMD_L3_HOST_USAGE, NULL},
    {"route",       _cmd_dpp_l3_route,        "\n" CMD_L3_ROUTE_USAGE, NULL},
    {"ecmp",        _cmd_dpp_l3_ecmp,         "\n" CMD_L3_ECMP_USAGE, NULL}
};

char cmd_dpp_l3_usage[] =
    "\n"
    CMD_L3_INTF_USAGE         "\n"
    CMD_L3_EGR_USAGE          "\n"
    CMD_L3_HOST_USAGE         "\n"
    CMD_L3_ROUTE_USAGE        "\n"
    CMD_L3_ECMP_USAGE         "\n"
    ;
/*
 * Local global variables to remember last values in arguments.
 */
static sal_mac_addr_t   _l3_macaddr = {0x0, 0x0, 0x0, 0x0, 0x0, 0x1};
static int _l3_vid = VLAN_ID_DEFAULT;
static int _l3_modid = 0, _l3_label, _l3_encap, _l3_port, _l3_tgid = 0;
static int _l3_fec;
static int _l3_vrf = 0, _l3_ttl = 0,  _l3_ifid = 0, _l3_mtu = 0;

#define _DPP_L3_ECMP_MAX 8

STATIC cmd_result_t
_cmd_dpp_l3_intf(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_intf_t      bcm_intf;
    char	      *subcmd;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    _l3_vid   = 0;
    _l3_ifid  = 0;
    _l3_vrf   = 0;
    _l3_ttl   = 0;
    _l3_mtu   = 0;

    rv        = BCM_E_NONE;
    sal_memset(_l3_macaddr, 0, 6);
    
    parse_table_init(unit, &pt);
    bcm_l3_intf_t_init(&bcm_intf);
    if ((sal_strcasecmp(subcmd, "add") == 0) ||
        (sal_strcasecmp(subcmd, "update") == 0)) {

        /* Parse command option arguments */
        parse_table_add(&pt, "SMAC", PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "vid", PQ_DFL|PQ_HEX, 0, &_l3_vid, NULL);
        parse_table_add(&pt, "Id",  PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);
        parse_table_add(&pt, "vrf", PQ_DFL|PQ_INT, 0, &_l3_vrf, NULL);
        parse_table_add(&pt, "ttl", PQ_DFL|PQ_INT, 0, &_l3_ttl, NULL);
        parse_table_add(&pt, "mtu", PQ_DFL|PQ_INT, 0, &_l3_mtu, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }

        if (sal_strcasecmp(subcmd, "update") == 0) {
            bcm_intf.l3a_flags   |= BCM_L3_REPLACE;
        }
        
        if (_l3_ifid) {
            bcm_intf.l3a_flags   |= BCM_L3_WITH_ID;
            bcm_intf.l3a_intf_id  = _l3_ifid;
        }

        if (_l3_vid) {
            bcm_intf.l3a_vid      = _l3_vid;
        }

        if (_l3_vrf) {
            bcm_intf.l3a_vrf      = _l3_vrf;
        }

        bcm_intf.l3a_ttl          = 2;
        if (_l3_ttl) {
            bcm_intf.l3a_ttl      = _l3_ttl;
        }

        bcm_intf.l3a_mtu         = 9216;
        if (_l3_mtu) {
            bcm_intf.l3a_mtu     = _l3_mtu;
        }
        sal_memcpy(bcm_intf.l3a_mac_addr,_l3_macaddr, 6);
    
        rv = bcm_l3_intf_create(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            cli_out("Created if_id 0x%x\n", bcm_intf.l3a_intf_id);
        }
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        parse_table_add(&pt, "SMAC", PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "vid", PQ_DFL|PQ_HEX, 0, &_l3_vid, NULL);
        parse_table_add(&pt, "Id",  PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);
        parse_table_add(&pt, "vrf", PQ_DFL|PQ_INT, 0, &_l3_vrf, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        
        if (_l3_ifid) {
            bcm_intf.l3a_flags   |= BCM_L3_WITH_ID;
            bcm_intf.l3a_intf_id  = _l3_ifid;
        } else {
            bcm_intf.l3a_vid      = _l3_vid;
            sal_memcpy(bcm_intf.l3a_mac_addr,
                       _l3_macaddr, 6);            
        }
        bcm_intf.l3a_vrf      = _l3_vrf;

        rv = bcm_l3_intf_delete(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            if (_l3_ifid)
                cli_out("Deleted if_id 0x%x, %s\n",
                        bcm_intf.l3a_intf_id, bcm_errmsg(rv));
            else
                cli_out("Deleted vid=%d mac=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x %s\n",
                        bcm_intf.l3a_vid,
                        bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                        bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                        bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                        bcm_errmsg(rv));
        }
        
    } else if (sal_strcasecmp(subcmd, "find") == 0) {

        /* Parse command option arguments */
        parse_table_add(&pt, "SMAC", PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "vid", PQ_DFL|PQ_HEX, 0, &_l3_vid, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        
        sal_memcpy(bcm_intf.l3a_mac_addr,_l3_macaddr, 6);
        bcm_intf.l3a_vid      = _l3_vid;
        rv = bcm_l3_intf_find(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Id=0x%x, Vid=%d, vrf=%d, ttl=%d, mtu=%d\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_intf_id, bcm_intf.l3a_vid,
                    bcm_intf.l3a_vrf, bcm_intf.l3a_ttl, bcm_intf.l3a_mtu);
        } else {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Vid=%d bcm_l3_intf_find() failure: %s\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_vid, bcm_errmsg(rv));
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {  
        parse_table_add(&pt, "Id",  PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);

        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        
        bcm_intf.l3a_flags   |= BCM_L3_WITH_ID;
        bcm_intf.l3a_intf_id  = _l3_ifid;
        rv = bcm_l3_intf_get(unit, &bcm_intf);
        if (rv == BCM_E_NONE) {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Id=0x%x, Vid=%d, vrf=%d, ttl=%d, mtu=%d\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_intf_id, bcm_intf.l3a_vid,
                    bcm_intf.l3a_vrf, bcm_intf.l3a_ttl, bcm_intf.l3a_mtu);
        } else {
            cli_out("MacAddr=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "Vid=%d bcm_l3_intf_get() failure: %s\n",
                    bcm_intf.l3a_mac_addr[0],bcm_intf.l3a_mac_addr[1],
                    bcm_intf.l3a_mac_addr[2],bcm_intf.l3a_mac_addr[3],
                    bcm_intf.l3a_mac_addr[4],bcm_intf.l3a_mac_addr[5],
                    bcm_intf.l3a_vid, bcm_errmsg(rv));
        }
    } else {
        cli_out("%s ERROR: Invalid option %s\n", FUNCTION_NAME(), subcmd);
	parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}


STATIC cmd_result_t
_cmd_dpp_l3_egr(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_egress_t    bcm_egr;
    char	      *subcmd;
    bcm_if_t           fec_idx;
    uint32             flags;
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    flags          = 0;
    _l3_ifid       = 0;
    _l3_tgid       = 0;
    _l3_modid      = 0;
    _l3_port       = 0;
    _l3_encap        = 0;
    _l3_label      = 0;
    _l3_fec        = 0;
    rv             = BCM_E_NONE;
    sal_memset(_l3_macaddr, 0, 6);
    
    parse_table_init(unit, &pt);
    bcm_l3_egress_t_init(&bcm_egr);
    
    if ((sal_strcasecmp(subcmd, "add") == 0) ||
        (sal_strcasecmp(subcmd, "update") == 0)) {

        /* Parse command option arguments */
        parse_table_add(&pt, "Id",     PQ_DFL|PQ_HEX, 0, &_l3_ifid, NULL);
        parse_table_add(&pt, "DMAC",   PQ_DFL|PQ_MAC, 0, &_l3_macaddr, NULL);
        parse_table_add(&pt, "module", PQ_DFL|PQ_INT, 0, &_l3_modid, NULL);
        parse_table_add(&pt, "port",   PQ_DFL|PQ_INT, 0, &_l3_port, NULL);
        parse_table_add(&pt, "trunk",  PQ_DFL|PQ_INT, 0, &_l3_tgid, NULL);
        parse_table_add(&pt, "label",  PQ_DFL|PQ_HEX, 0, &_l3_label, NULL);
        parse_table_add(&pt, "encap",  PQ_DFL|PQ_HEX, 0, &_l3_encap, NULL);
        parse_table_add(&pt, "fec",    PQ_DFL|PQ_HEX, 0, &_l3_fec, NULL);
        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }

        if (sal_strcasecmp(subcmd, "update") == 0) {
            flags = bcm_egr.flags = BCM_L3_REPLACE;
        }
        
        bcm_egr.intf              = _l3_ifid;
        sal_memcpy(bcm_egr.mac_addr,_l3_macaddr, 6);
        bcm_egr.module            = _l3_modid;
        bcm_egr.port              = _l3_port;
        bcm_egr.trunk             = _l3_tgid;
        bcm_egr.mpls_label        = _l3_label;
        bcm_egr.encap_id          = _l3_encap;
        
        rv = bcm_l3_egress_create(unit, flags, &bcm_egr, &fec_idx);
        if (rv == BCM_E_NONE) {
            cli_out("Created fec 0x%x encap=0x%x\n", fec_idx, bcm_egr.encap_id);
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        parse_table_add(&pt, "fec",    PQ_DFL|PQ_HEX, 0, &_l3_fec, NULL);
        
        if (!parseEndOk(args, &pt, &retCode)) {
            return retCode;
        }
        fec_idx = (uint32)_l3_fec;
        rv = bcm_l3_egress_get (unit,
                                fec_idx,
                                &bcm_egr) ;
        if (rv == BCM_E_NONE) {
            cli_out("\n\tDMAC=0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x "
                    "\n\tInterfaceId=0x%x, module=%d, port=%d, trunk=%d "
                    "\n\tlabel=0x%x encap=0x%x\n",
                    bcm_egr.mac_addr[0],bcm_egr.mac_addr[1],
                    bcm_egr.mac_addr[2],bcm_egr.mac_addr[3],
                    bcm_egr.mac_addr[4],bcm_egr.mac_addr[5],
                    bcm_egr.intf, bcm_egr.module,
                    bcm_egr.port, bcm_egr.trunk, bcm_egr.mpls_label,
                    bcm_egr.encap_id);
        }
    } else {
        cli_out("%s ERROR: Invalid option %s\n", FUNCTION_NAME(), subcmd);
	parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
_cmd_dpp_l3_host(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_host_t      bcm_host;
    char              *subcmd;
    ip_addr_t          ipaddr;
    ip6_addr_t         ip6addr;
    int                v6 = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    _l3_fec        = 0;
    _l3_vrf        = 0;
    rv             = BCM_E_NONE;
    sal_memset(&ipaddr, 0, sizeof(ipaddr));
    sal_memset(&ip6addr, 0, sizeof(ip6_addr_t));

    parse_table_init(unit, &pt);
    bcm_l3_host_t_init(&bcm_host);

    parse_table_add(&pt, "ip",      PQ_DFL | PQ_IP,  0, &ipaddr, NULL);
    parse_table_add(&pt, "ip6",     PQ_DFL | PQ_IP6, 0, &ip6addr, NULL);
    parse_table_add(&pt, "vrf",    PQ_DFL | PQ_INT, 0, &_l3_vrf, NULL);
    parse_table_add(&pt, "fec",    PQ_DFL | PQ_HEX, 0, &_l3_fec, NULL);
    parse_table_add(&pt, "v6",  PQ_DFL|PQ_BOOL|PQ_NO_EQ_OPT, 0, &v6, NULL);

    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    if (v6) {
        bcm_host.l3a_flags |= BCM_L3_IP6;
    }

    bcm_host.l3a_vrf      = _l3_vrf;
    bcm_host.l3a_ip_addr  = ipaddr;
    sal_memcpy(&bcm_host.l3a_ip6_addr, ip6addr, sizeof(ip6_addr_t));

    if (sal_strcasecmp(subcmd, "add") == 0) {
        bcm_host.l3a_intf    = _l3_fec;
        rv = bcm_l3_host_add(unit, &bcm_host);

        cli_out("Host add returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "update") == 0) {
        bcm_host.l3a_intf    = _l3_fec;
        bcm_host.l3a_flags  |= BCM_L3_REPLACE;
        rv = bcm_l3_host_add(unit, &bcm_host);

        cli_out("Host update returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "find") == 0) {
        rv = bcm_l3_host_find(unit, &bcm_host);
        cli_out("Host find returned: %s\n", bcm_errmsg(rv));
        if (rv == BCM_E_NONE) {
            rv = _bcm_ppd_frwrd_host_print_info(unit, &bcm_host);
        }

    } else {
        cli_out("%s ERROR: Invalid option %s", FUNCTION_NAME(), subcmd);
        return CMD_FAIL;
    }

    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
_cmd_dpp_l3_route(int unit, args_t *args)
{
    cmd_result_t       retCode;
    int                rv;
    parse_table_t      pt;
    bcm_l3_route_t     bcm_route;
    char	      *subcmd;
    ip_addr_t          ipaddr, mask;
    ip6_addr_t         ip6addr, ip6mask;
    int                v6 = 0;
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }

    _l3_fec        = 0;
    _l3_vrf        = 0;
    rv             = BCM_E_NONE;
    sal_memset(&ipaddr, 0, sizeof(ipaddr));
    sal_memset(&mask, 0, sizeof(mask));
    sal_memset(&ip6addr, 0, sizeof(ip6_addr_t));
    sal_memset(&ip6mask, 0, sizeof(ip6_addr_t));
    
    parse_table_init(unit, &pt);
    bcm_l3_route_t_init(&bcm_route);

    parse_table_add(&pt, "ip",      PQ_DFL | PQ_IP,  0, &ipaddr, NULL);
    parse_table_add(&pt, "ip6",     PQ_DFL | PQ_IP6, 0, &ip6addr, NULL);
    parse_table_add(&pt, "mask",    PQ_DFL | PQ_IP,  0, &mask,   NULL);
    parse_table_add(&pt, "ip6mask", PQ_DFL | PQ_IP6, 0, &ip6mask,   NULL);
    parse_table_add(&pt, "vrf",    PQ_DFL | PQ_INT, 0, &_l3_vrf, NULL);
    parse_table_add(&pt, "fec",    PQ_DFL | PQ_HEX, 0, &_l3_fec, NULL);
    parse_table_add(&pt, "v6",  PQ_DFL|PQ_BOOL|PQ_NO_EQ_OPT, 0, &v6, NULL);
        
    if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

    if (v6) {
        bcm_route.l3a_flags |= BCM_L3_IP6;
    }
    
    bcm_route.l3a_vrf     = _l3_vrf;
    bcm_route.l3a_subnet  = ipaddr;
    bcm_route.l3a_ip_mask = mask;
    sal_memcpy(&bcm_route.l3a_ip6_net, ip6addr, sizeof(ip6_addr_t));
    sal_memcpy(&bcm_route.l3a_ip6_mask, ip6mask, sizeof(ip6_addr_t));
    
    if (sal_strcasecmp(subcmd, "add") == 0) {
        bcm_route.l3a_intf    = _l3_fec;
        rv = bcm_l3_route_add(unit, &bcm_route);
        
        cli_out("Route add returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "update") == 0) {
        bcm_route.l3a_intf    = _l3_fec;
        bcm_route.l3a_flags  |= BCM_L3_REPLACE;
        rv = bcm_l3_route_add(unit, &bcm_route);

        cli_out("Route update returned: %s\n", bcm_errmsg(rv));

    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        rv = bcm_l3_route_get(unit, &bcm_route);
        cli_out("Route get returned: %s\n", bcm_errmsg(rv));
        if (rv == BCM_E_NONE) {
            rv = _bcm_ppd_frwrd_route_print_info(unit, &bcm_route);
        }

    } else {
        cli_out("%s ERROR: Invalid option %s", FUNCTION_NAME(), subcmd);
        return CMD_FAIL;
    }
    
    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
_cmd_dpp_l3_ecmp(int unit, args_t *args)
{
    int                size, rv;
    parse_table_t      pt;
    uint32             i;
    bcm_if_t           fecs[_DPP_L3_ECMP_MAX], ecmp_id;
	bcm_l3_egress_ecmp_t         ecmp;
	char               *subcmd;
	cmd_result_t       retCode;

    ecmp_id = 0;
	size  = 0;
	rv = BCM_E_NONE;
	sal_memset(fecs, 0, sizeof(bcm_if_t) * _DPP_L3_ECMP_MAX);
	bcm_l3_egress_ecmp_t_init(&ecmp);
    
    if ((subcmd = ARG_GET(args)) == NULL) {
        cli_out("%s ERROR: empty cmd\n", FUNCTION_NAME());
        return CMD_FAIL;
    }
        
    parse_table_init(unit, &pt);
	parse_table_add(&pt, "size",    PQ_INT, 0, &size, NULL);
	parse_table_add(&pt, "fec1",  PQ_DFL| PQ_HEX, 0, &fecs[0], NULL);
	parse_table_add(&pt, "fec2",  PQ_DFL| PQ_HEX, 0, &fecs[1], NULL);
	parse_table_add(&pt, "fec3",  PQ_DFL| PQ_HEX, 0, &fecs[2], NULL);
	parse_table_add(&pt, "fec4",  PQ_DFL| PQ_HEX, 0, &fecs[3], NULL);
	parse_table_add(&pt, "fec5",  PQ_DFL| PQ_HEX, 0, &fecs[4], NULL);
	parse_table_add(&pt, "fec6",  PQ_DFL| PQ_HEX, 0, &fecs[5], NULL);
	parse_table_add(&pt, "fec7",  PQ_DFL| PQ_HEX, 0, &fecs[6], NULL);
	parse_table_add(&pt, "fec8",  PQ_DFL| PQ_HEX, 0, &fecs[7], NULL);
	parse_table_add(&pt, "ecmp_id", PQ_INT, 0, &ecmp_id, NULL);
	if (!parseEndOk(args, &pt, &retCode)) {
        return retCode;
    }

	if (size > _DPP_L3_ECMP_MAX ) {
		cli_out("%s ERROR: ECMP maximal suuported size is %d\n", FUNCTION_NAME(), _DPP_L3_ECMP_MAX);
        return CMD_FAIL;
	}
	
    if (sal_strcasecmp(subcmd, "create") == 0) {
        /*
         * Initial creation
         */
		if (size == 0) {
			cli_out("%s ERROR: ECMP size is mandatory\n", FUNCTION_NAME());
            return CMD_FAIL;
		}
		ecmp.max_paths = size;
		
		rv = bcm_l3_egress_ecmp_create(unit, &ecmp, size, fecs);
        if (rv == BCM_E_NONE) {
            cli_out("ECMP create success: 0x%x\n", ecmp.ecmp_intf);
        }

    } else if (sal_strcasecmp(subcmd, "add") == 0) {
        /*
         * add a member to the group
         */
        if (ecmp_id == 0) {
			cli_out("%s ERROR: ECMP id is mandatory\n", FUNCTION_NAME());
            return CMD_FAIL;
		}
		ecmp.ecmp_intf = ecmp_id;

		if (size != 1) {
			cli_out("%s ERROR: It's only possbile to add one FEC at a time\n", FUNCTION_NAME());
            return CMD_FAIL;
		}

        rv = bcm_l3_egress_ecmp_add(unit, &ecmp, fecs[0]);
        if (rv == BCM_E_NONE) {
            cli_out("ECMP add successfull\n");
        }

    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        /*
         * remove a member from the group
         */
        if (ecmp_id == 0) {
			cli_out("%s ERROR: ECMP id is mandatory\n", FUNCTION_NAME());
            return CMD_FAIL;
		}
		ecmp.ecmp_intf = ecmp_id;

		if (size != 1) {
			cli_out("%s ERROR: It's only possbile to delete one FEC at a time\n", FUNCTION_NAME());
            return CMD_FAIL;
		}

        rv = bcm_l3_egress_ecmp_delete(unit, &ecmp, fecs[0]);
        if (rv == BCM_E_NONE) {
            cli_out("ECMP delete successfull\n");
        }
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        /*
         * destroy the whole group
         */
        if (ecmp_id == 0) {
			cli_out("%s ERROR: ECMP id is mandatory\n", FUNCTION_NAME());
            return CMD_FAIL;
		}
		ecmp.ecmp_intf = ecmp_id;

        rv = bcm_l3_egress_ecmp_destroy(unit, &ecmp);
        if (rv == BCM_E_NONE) {
            cli_out("ECMP destroy successfull\n");
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
		/*
         * get the whole group
         */
        if (ecmp_id == 0) {
			cli_out("%s ERROR: ECMP id is mandatory\n", FUNCTION_NAME());
            return CMD_FAIL;
		}
		ecmp.ecmp_intf = ecmp_id;

        rv = bcm_l3_egress_ecmp_get(unit, &ecmp, _DPP_L3_ECMP_MAX, fecs, &size);
        if (rv == BCM_E_NONE) {
            cli_out("ECMP get successfull\n");
            for (i = 0; i < size; i++) {
                cli_out("fec[%d] = 0x%x\n", i, fecs[i]);
            }
        }
    } else {
        cli_out("%s ERROR: Invalid option %s", FUNCTION_NAME(), subcmd);
        return CMD_FAIL;
    }
    
    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}


cmd_result_t
cmd_dpp_l3(int unit, args_t *args)
{
    return subcommand_execute(unit, args, 
                              _cmd_dpp_l3_list, COUNTOF(_cmd_dpp_l3_list));
}


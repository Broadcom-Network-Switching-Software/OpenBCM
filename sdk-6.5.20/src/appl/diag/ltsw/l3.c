/*! \file l3.c
 *
 * LTSW L3 CLI commands.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#if defined(BCM_LTSW_SUPPORT)

#include <bsl/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/nat.h>
#include <bcm/ipmc.h>
#include <bcm/stack.h>

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_fib_int.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/feature.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <sal/sal_types.h>
/******************************************************************************
 * Local definitions
 */
/* The maximum count of member can be inputted by one CLI call. */
#define ECMP_MEMBER_MAX 32

#define ROUTE_PERF_VRF 100

typedef struct route_perf_data_s {
    int count; /* Number of times the functions is called. */
    int max;   /* Maximum call time. */
    int min;   /* Minimum call time. */
    int total; /* Total duration time. */
} route_perf_data_t;

static int egr_obj_id[BCM_MAX_NUM_UNITS];
static int egr_obj_id2[BCM_MAX_NUM_UNITS];

static int alpm_dmode[3][2] = {{-1, -1},{-1, -1},{-1, -1}};

static const char *alpm_route_grp_str[] = {
    "IPv4 (Private) ",
    "IPv4 (Global)  ",
    "IPv4 (Override)",
    "IPv6 (Private) ",
    "IPv6 (Global)  ",
    "IPv6 (Override)",
    NULL
};

static const char *alpm_route_comb_str[] = {
    "IPv4 (Prv+Glo) ",
    "IPv4 (Prv+Glo) ",
    "IPv4 (Override)",
    "IPv6 (Prv+Glo) ",
    "IPv6 (Prv+Glo) ",
    "IPv6 (Override)",
    NULL
};

/******************************************************************************
 * Private functions
 */

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init_print
 * Description:
 *      Internal function to print out tunnel initiator info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      interface - (IN) L3 egress interface.
 *      info      - (IN) Pointer to bcm_tunnel_initiator_t data structure.
 */
STATIC int
cmd_ltsw_l3_tunnel_init_print(int unit, bcm_if_t interface,
                               bcm_tunnel_initiator_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];

    cli_out("Tunnel initiator:\n");
    cli_out("\tUnit        = %d\n", unit);
    if (interface != 0) {
        cli_out("\tInterface   = %d\n", interface);
    }
    cli_out("\tTUNNEL_TYPE = %d\n", info->type);
    cli_out("\tTTL         = %d\n", info->ttl);
    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(info->type)) {
        format_ip6addr(ip_str, info->dip6);
        cli_out("\tDIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6);
        cli_out("\tSIP         = 0x%-s\n", ip_str);
    } else {
        cli_out("\tDIP         = 0x%08x\n", info->dip);
        cli_out("\tSIP         = 0x%08x\n", info->sip);
        if (info->flags & BCM_TUNNEL_INIT_USE_INNER_DF) {
            cli_out("\tCopy DF from inner header.\n");
        } else if (info->flags & BCM_TUNNEL_INIT_IPV4_SET_DF) {
            cli_out("\tForce DF to 1 for ipv4 payload.\n");
        }
        if (info->flags & BCM_TUNNEL_INIT_IPV6_SET_DF) {
            cli_out("\tForce DF to 1 for ipv6 payload.\n");
        }
    }
    cli_out("\tDSCP_ECN_SEL = 0x%x\n", info->dscp_ecn_sel);
    cli_out("\tDSCP         = 0x%x\n", info->dscp);
    cli_out("\tDSCP_ECN_MAP = 0x%x\n", info->dscp_ecn_map);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init_create
 * Description:
 *      Service routine used to create a tunnel initiator.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_init_create(int unit, args_t *a)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_gport_t       tnl_id = 0;
    bcm_tunnel_type_t type = 0;
    parse_table_t     pt;
    cmd_result_t      retCode;
    bcm_ip6_t         sip6_addr;
    bcm_ip6_t         ip6_addr;
    bcm_ip_t          sip_addr = 0;
    bcm_ip_t          ip_addr = 0;
    int               ip4_df_sel = 0;
    int               ip6_df_sel = 0;
    int               dscp_val = 0;
    int               dscp_ecn_sel = 0;
    int               dscp_ecn_map = 0;
    int               ttl = 0;
    int               rv;

    /* Stack variables initialization. */
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "TnlId", PQ_DFL|PQ_INT, 0, (void*)&tnl_id, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&type, 0);
    parse_table_add(&pt, "TTL",  PQ_DFL|PQ_INT, 0, (void*)&ttl, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "DIP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "SIP6", PQ_DFL | PQ_IP6, 0, (void *)&sip6_addr, 0);
    parse_table_add(&pt, "DSCPEcnSel", PQ_DFL|PQ_INT, 0, (void *)&dscp_ecn_sel, 0);
    parse_table_add(&pt, "DSCPV", PQ_DFL|PQ_INT, 0, (void *)&dscp_val, 0);
    parse_table_add(&pt, "DFSEL4", PQ_DFL|PQ_INT, 0, (void *)&ip4_df_sel, 0);
    parse_table_add(&pt, "DFSEL6", PQ_DFL|PQ_BOOL, 0, (void *)&ip6_df_sel, 0);
    parse_table_add(&pt, "DSCPEcnMap", PQ_DFL|PQ_INT, 0, (void *)&dscp_ecn_map, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_tunnel_initiator_t_init(&tunnel_init);

    /* Fill tunnel info. */
    tunnel_init.type = type;
    tunnel_init.ttl = ttl;

    if (ip4_df_sel) {
        tunnel_init.flags |= (ip4_df_sel > 1) ?
            BCM_TUNNEL_INIT_USE_INNER_DF : BCM_TUNNEL_INIT_IPV4_SET_DF;
    }
    if (ip6_df_sel) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
    }

    sal_memcpy(tunnel_init.dip6, ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(tunnel_init.sip6, sip6_addr, sizeof(bcm_ip6_t));
    tunnel_init.dip = ip_addr;
    tunnel_init.sip = sip_addr;
    tunnel_init.dscp_ecn_sel = dscp_ecn_sel;
    tunnel_init.dscp = dscp_val;
    tunnel_init.dscp_ecn_map = dscp_ecn_map;

    if (tnl_id != 0) {
        tunnel_init.tunnel_id = tnl_id;
        tunnel_init.flags |= BCM_TUNNEL_WITH_ID | BCM_TUNNEL_REPLACE;
    }

    if ((rv = bcm_tunnel_initiator_create(unit, NULL, &tunnel_init)) < 0) {
        cli_out("ERROR %s: creating tunnel initiator %s.\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("New tunnel initiator ID: 0x%x.\n", tunnel_init.tunnel_id);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init_get
 * Description:
 *      Service routine used to get & show tunnel initiator info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_init_get(int unit, args_t *a)
{
    bcm_tunnel_initiator_t tunnel_init;
    parse_table_t          pt;
    cmd_result_t           retCode;
    bcm_gport_t            tnl_id;
    int                    rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "TnlId", PQ_DFL|PQ_INT, 0, (void*)&tnl_id, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.tunnel_id = tnl_id;

    if ((rv = bcm_tunnel_initiator_get(unit, NULL, &tunnel_init)) < 0) {
        cli_out("ERROR %s: getting tunnel initiator tnl_id 0x%x %s\n",
                ARG_CMD(a), tnl_id, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cmd_ltsw_l3_tunnel_init_print(unit, 0, &tunnel_init);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3tunnel_init_destroy
 * Description:
 *      Service routine used to remove tunnel initiator from an interface.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_init_destroy(int unit, args_t *a)
{
    cmd_result_t           retCode;
    parse_table_t          pt;
    int                    rv;
    bcm_gport_t            tnl_id;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "TnlId", PQ_DFL|PQ_INT, 0, (void*)&tnl_id, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if ((rv = bcm_tunnel_initiator_destroy(unit, tnl_id)) < 0) {
        cli_out("ERROR %s: destroying tunnel initiator 0x%x %s\n",
                ARG_CMD(a), tnl_id, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init_set
 * Description:
 *      Service routine used to set the tunnel property for the given L3 interface..
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_init_set(int unit, args_t *a)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t     intf;
    bcm_tunnel_type_t type = 0;
    parse_table_t     pt;
    cmd_result_t      retCode;
    bcm_ip6_t         sip6_addr;
    bcm_ip6_t         ip6_addr;
    bcm_mac_t         mac;
    bcm_if_t          interface = 0;
    bcm_ip_t          sip_addr = 0;
    bcm_ip_t          ip_addr = 0;
    int               ip4_df_sel = 0;
    int               ip6_df_sel = 0;
    int               dscp_val = 0;
    int               dscp_sel = 0;
    int               dscp_map = 0;
    int               ttl = 0;
    int               rv;

    /* Stack variables initialization. */
    sal_memset(&intf, 0, sizeof(bcm_l3_intf_t));
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&type, 0);
    parse_table_add(&pt, "TTL",  PQ_DFL|PQ_INT, 0, (void*)&ttl, 0);
    parse_table_add(&pt, "Mac",  PQ_DFL|PQ_MAC, 0, (void *)mac, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "DIP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "SIP6", PQ_DFL | PQ_IP6, 0, (void *)&sip6_addr, 0);
    parse_table_add(&pt, "DSCPSel", PQ_DFL|PQ_INT, 0, (void *)&dscp_sel, 0);
    parse_table_add(&pt, "DSCPV", PQ_DFL|PQ_INT, 0, (void *)&dscp_val, 0);
    parse_table_add(&pt, "DFSEL4", PQ_DFL|PQ_INT, 0, (void *)&ip4_df_sel, 0);
    parse_table_add(&pt, "DFSEL6", PQ_DFL|PQ_BOOL, 0, (void *)&ip6_df_sel, 0);
    parse_table_add(&pt, "DSCPMap", PQ_DFL|PQ_INT, 0, (void *)&dscp_map, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_intf_t_init(&intf);
    bcm_tunnel_initiator_t_init(&tunnel_init);

    /* Fill tunnel info. */
    intf.l3a_intf_id = interface;
    tunnel_init.type = type;
    tunnel_init.ttl = ttl;

    sal_memcpy(tunnel_init.dmac, mac, sizeof(mac));

    if (ip4_df_sel) {
        tunnel_init.flags |= (ip4_df_sel > 1) ?
            BCM_TUNNEL_INIT_USE_INNER_DF : BCM_TUNNEL_INIT_IPV4_SET_DF;
    }
    if (ip6_df_sel) {
        tunnel_init.flags |= BCM_TUNNEL_INIT_IPV6_SET_DF;
    }

    sal_memcpy(tunnel_init.dip6, ip6_addr, sizeof(bcm_ip6_t));
    sal_memcpy(tunnel_init.sip6, sip6_addr, sizeof(bcm_ip6_t));
    tunnel_init.dip = ip_addr;
    tunnel_init.sip = sip_addr;
    tunnel_init.dscp_sel = dscp_sel;
    tunnel_init.dscp = dscp_val;
    tunnel_init.dscp_map = dscp_map;

    if ((rv = bcm_tunnel_initiator_set(unit, &intf, &tunnel_init)) < 0) {
        cli_out("ERROR %s: setting tunnel initiator for %d %s\n",
                ARG_CMD(a), interface, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init_show
 * Description:
 *      Service routine used to get & show tunnel initiator info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_init_show(int unit, args_t *a)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t          intf;
    parse_table_t          pt;
    cmd_result_t           retCode;
    bcm_if_t               interface;
    int                    rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_tunnel_initiator_t_init(&tunnel_init);

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = interface;

    if ((rv = bcm_tunnel_initiator_get(unit, &intf, &tunnel_init)) < 0) {
        cli_out("ERROR %s: getting tunnel initiator for %d %s\n",
                ARG_CMD(a), interface, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cmd_ltsw_l3_tunnel_init_print(unit, interface, &tunnel_init);
    return CMD_OK;
}


/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init_clear
 * Description:
 *      Service routine used to delete the tunnel association for the given L3 interface.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_init_clear(int unit, args_t *a)
{
    bcm_if_t               interface;
    cmd_result_t           retCode;
    bcm_l3_intf_t          intf;
    parse_table_t          pt;
    int                    rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL|PQ_INT, 0, (void*)&interface, 0);
    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = interface;

    if ((rv = bcm_tunnel_initiator_clear(unit, &intf)) < 0) {
        cli_out("ERROR %s: clearing tunnel initiator for %d %s\n",
                ARG_CMD(a), interface, bcm_errmsg(rv));
        return CMD_FAIL;
    }
    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_init
 * Description:
 *      Service routine used to manipulate tunnel initiator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_tunnel_init(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing l3_tunnel_init subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_init_create(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_init_get(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_init_destroy(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "set") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_init_set(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_init_show(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "clear") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_init_clear(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_term_print
 * Description:
 *      Internal function to print out tunnel terminator info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      info      - (IN) Pointer to bcm_tunnel_terminator_t data structure.
 */
STATIC int
cmd_ltsw_l3_tunnel_term_print (int unit, bcm_tunnel_terminator_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];

    cli_out("Tunnel terminator:\n");
    cli_out("\tUnit        = %d\n", unit);
    cli_out("\tTUNNEL_TYPE = %d\n", info->type);
    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(info->type)) {
        format_ip6addr(ip_str, info->dip6);
        cli_out("\tDIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->dip6_mask);
        cli_out("\tDIP MASK    = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6);
        cli_out("\tSIP         = 0x%-s\n", ip_str);
        format_ip6addr(ip_str, info->sip6_mask);
        cli_out("\tSIP MASK    = 0x%-s\n", ip_str);
    } else {
        cli_out("\tDIP         = 0x%08x\n", info->dip);
        cli_out("\tDIP_MASK    = 0x%08x\n", info->dip_mask);
        cli_out("\tSIP         = 0x%08x\n", info->sip);
        cli_out("\tSIP_MASK    = 0x%08x\n", info->sip_mask);
    }

    if (info->flags & BCM_TUNNEL_TERM_EM) {
        cli_out("\tEM          = TRUE\n");
    } else {
        cli_out("\tEM          = FALSE\n");
    }

    cli_out("\tVlan id     = 0x%08x\n", info->vlan);

    if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_DSCP) {
        cli_out("\tCopy DSCP from outer ip header.\n");
    }

    if (info->flags & BCM_TUNNEL_TERM_USE_OUTER_TTL) {
        cli_out("\tCopy TTL from outer ip header.\n");
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_term_add
 * Description:
 *      Service routine used to add tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_term_add(int unit, args_t *a)
{
    bcm_tunnel_terminator_t tunnel_term;
    cmd_result_t      retCode;
    parse_table_t     pt;
    int               rv;
    bcm_ip6_t         sip6;
    bcm_ip6_t         dip6;
    int               vlan = 0;
    int               dip = 0;
    int               sip = 0;
    int               sip_mask_len = 0;
    int               dip_mask_len = 0;
    int               tnl_type = 0;
    int               outer_ttl = 0;
    int               outer_dscp = 0;
    int               replace=0;
    int               em = 0;

    parse_table_init(unit, &pt);
    sal_memset(sip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dip6, 0, sizeof(bcm_ip6_t));

    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void*)&dip, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void*)&sip, 0);
    parse_table_add(&pt, "DIP6",  PQ_DFL|PQ_IP6, 0, (void*)&dip6, 0);
    parse_table_add(&pt, "SIP6",  PQ_DFL|PQ_IP6, 0, (void*)&sip6, 0);
    parse_table_add(&pt, "DipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&dip_mask_len, 0);
    parse_table_add(&pt, "SipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&sip_mask_len, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&tnl_type, 0);
    parse_table_add(&pt, "OuterDSCP", PQ_DFL|PQ_BOOL, 0,(void *)&outer_dscp, 0);
    parse_table_add(&pt, "OuterTTL", PQ_DFL|PQ_BOOL, 0, (void *)&outer_ttl, 0);
    parse_table_add(&pt, "VLanid", PQ_DFL|PQ_INT, 0, (void*)&vlan, 0);
    parse_table_add(&pt, "Replace", PQ_DFL|PQ_BOOL, 0, &replace, 0);
    parse_table_add(&pt, "EM", PQ_DFL|PQ_BOOL, 0, &em, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memset(&tunnel_term, 0, sizeof (tunnel_term));

    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(tnl_type)) {
        sal_memcpy(tunnel_term.sip6, sip6, sizeof(bcm_ip6_t));
        sal_memcpy(tunnel_term.dip6, dip6, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(tunnel_term.sip6_mask, sip_mask_len);
        bcm_ip6_mask_create(tunnel_term.dip6_mask, dip_mask_len);
    } else {
        tunnel_term.sip = sip;
        tunnel_term.dip = dip;
        tunnel_term.sip_mask = bcm_ip_mask_create(sip_mask_len);
        tunnel_term.dip_mask = bcm_ip_mask_create(dip_mask_len);
    }

    tunnel_term.type = tnl_type;

    if (outer_dscp) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }

    if (outer_ttl) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }

    if (replace) {
        tunnel_term.flags |= BCM_TUNNEL_REPLACE;
    }

    if (em) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_EM;
    }

    tunnel_term.vlan = (bcm_vlan_t)vlan;

    if ((rv = bcm_tunnel_terminator_add(unit, &tunnel_term)) < 0) {
        cli_out("ERROR %s: adding tunnel term %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_term_delete
 * Description:
 *      Service routine used to delete tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_term_delete(int unit, args_t *a)
{
    bcm_tunnel_terminator_t tunnel_term;
    cmd_result_t      retCode;
    parse_table_t     pt;
    int               rv;
    bcm_ip6_t         sip6;
    bcm_ip6_t         dip6;
    int               dip = 0;
    int               sip = 0;
    int               sip_mask_len = 0;
    int               dip_mask_len = 0;
    int               tnl_type = 0;
    int               em = 0;

    parse_table_init(unit, &pt);
    sal_memset(sip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dip6, 0, sizeof(bcm_ip6_t));

    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void*)&dip, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void*)&sip, 0);
    parse_table_add(&pt, "DIP6",  PQ_DFL|PQ_IP6, 0, (void*)&dip6, 0);
    parse_table_add(&pt, "SIP6",  PQ_DFL|PQ_IP6, 0, (void*)&sip6, 0);
    parse_table_add(&pt, "DipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&dip_mask_len, 0);
    parse_table_add(&pt, "SipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&sip_mask_len, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&tnl_type, 0);
    parse_table_add(&pt, "EM", PQ_DFL|PQ_BOOL, 0, &em, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memset(&tunnel_term, 0, sizeof (tunnel_term));


    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(tnl_type)) {
        sal_memcpy(tunnel_term.sip6, sip6, sizeof(bcm_ip6_t));
        sal_memcpy(tunnel_term.dip6, dip6, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(tunnel_term.sip6_mask, sip_mask_len);
        bcm_ip6_mask_create(tunnel_term.dip6_mask, dip_mask_len);
    } else {
        tunnel_term.sip = sip;
        tunnel_term.dip = dip;
        tunnel_term.sip_mask = bcm_ip_mask_create(sip_mask_len);
        tunnel_term.dip_mask = bcm_ip_mask_create(dip_mask_len);
    }

    tunnel_term.type = tnl_type;
    if (em) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_EM;
    }

    if ((rv = bcm_tunnel_terminator_delete(unit, &tunnel_term)) < 0) {
        cli_out("ERROR %s: adding tunnel term %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_term_get
 * Description:
 *      Service routine used to read tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
STATIC cmd_result_t
cmd_ltsw_l3_tunnel_term_get(int unit, args_t *a)
{
    bcm_tunnel_terminator_t tunnel_term;
    cmd_result_t      retCode;
    parse_table_t     pt;
    int               rv;
    bcm_ip6_t         sip6;
    bcm_ip6_t         dip6;
    int               vlan = 0;
    int               dip = 0;
    int               sip = 0;
    int               sip_mask_len = 0;
    int               dip_mask_len = 0;
    int               tnl_type = 0;
    int               em = 0;

    parse_table_init(unit, &pt);
    sal_memset(sip6, 0, sizeof(bcm_ip6_t));
    sal_memset(dip6, 0, sizeof(bcm_ip6_t));
    sal_memset(&tunnel_term, 0, sizeof(bcm_tunnel_terminator_t));

    parse_table_add(&pt, "DIP",  PQ_DFL|PQ_IP, 0, (void*)&dip, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL|PQ_IP, 0, (void*)&sip, 0);
    parse_table_add(&pt, "DIP6",  PQ_DFL|PQ_IP6, 0, (void*)&dip6, 0);
    parse_table_add(&pt, "SIP6",  PQ_DFL|PQ_IP6, 0, (void*)&sip6, 0);
    parse_table_add(&pt, "DipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&dip_mask_len, 0);
    parse_table_add(&pt, "SipMaskLen", PQ_DFL|PQ_INT, 0, (void*)&sip_mask_len, 0);
    parse_table_add(&pt, "TYpe", PQ_DFL|PQ_INT, 0, (void*)&tnl_type, 0);
    parse_table_add(&pt, "VLAN", PQ_DFL|PQ_INT, 0, (void*)&vlan, 0);
    parse_table_add(&pt, "EM", PQ_DFL|PQ_BOOL, 0, &em, 0);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    sal_memset(&tunnel_term, 0, sizeof (tunnel_term));


    if (BCMI_LTSW_TUNNEL_OUTER_HEADER_IPV6(tnl_type)) {
        sal_memcpy(tunnel_term.sip6, sip6, sizeof(bcm_ip6_t));
        sal_memcpy(tunnel_term.dip6, dip6, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(tunnel_term.sip6_mask, sip_mask_len);
        bcm_ip6_mask_create(tunnel_term.dip6_mask, dip_mask_len);
    } else {
        tunnel_term.sip = sip;
        tunnel_term.dip = dip;
        tunnel_term.sip_mask = bcm_ip_mask_create(sip_mask_len);
        tunnel_term.dip_mask = bcm_ip_mask_create(dip_mask_len);
    }

    tunnel_term.type = tnl_type;
    tunnel_term.vlan = (bcm_vlan_t)vlan;

    if (em) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_EM;
    }

    if ((rv = bcm_tunnel_terminator_get(unit, &tunnel_term)) < 0) {
        cli_out("ERROR %s: getting tunnel term %s\n",
                ARG_CMD(a), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cmd_ltsw_l3_tunnel_term_print(unit, &tunnel_term);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_tunnel_term
 * Description:
 *      Service routine used to manipulate tunnel terminator entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_tunnel_term(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing l3_tunnel_term subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_term_add(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_term_delete(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        if ((r = cmd_ltsw_l3_tunnel_term_get(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

/*!
 * \brief Print the ingress interface info.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [in] info Ingress interface info.
 *
 * \return none.
 */
static void
l3_cmd_ingress_print(int unit, int intf_id, bcm_l3_ingress_t *info)
{
    cli_out("%-5d 0x%-10x %-5d %-9d %-9d %-9d\n",
            intf_id, info->flags, info->vrf, info->urpf_mode,
            info->intf_class, info->qos_map_id);
    cli_out("\n");
}

/*!
 * \brief The callback of ingress interface traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [in] info Ingress interface info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_ingress_show_trav_cb(int unit, bcm_if_t intf_id,
                            bcm_l3_ingress_t *info, void *cookie)
{
    l3_cmd_ingress_print(unit, intf_id, info);
    return BCM_E_NONE;
}

/*!
 * \brief The callback of ingress interface traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Ingress interface ID.
 * \param [in] info Ingress interface info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_ingress_clear_trav_cb(int unit, bcm_if_t intf_id,
                             bcm_l3_ingress_t *info, void *cookie)
{
    return bcm_l3_ingress_destroy(unit, intf_id);
}

/*!
 * \brief Add a L3 ingress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ingress_add(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_ingress_t intf;
    int intf_id = BCM_IF_INVALID;
    int def_intf_id = BCM_IF_INVALID;
    int vrf = 0, urpf_mode = 0, intf_class = 0, qos_map_id = 0, dscp_trust = 0;
    int urpf_dft_rt_chk_en = 0, pim_bidir = 0;
    int rv;
    cmd_result_t retcode;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INTF", PQ_DFL | PQ_INT, (void *)&def_intf_id,
                    (void *)&intf_id, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "UrpfMode", PQ_DFL | PQ_INT, 0, (void *)&urpf_mode, 0);
    parse_table_add(&pt, "INtfClass", PQ_DFL | PQ_INT, 0,
                    (void *)&intf_class, 0);
    parse_table_add(&pt, "QOSmapID", PQ_DFL | PQ_INT, 0,
                    (void *)&qos_map_id, 0);
    parse_table_add(&pt, "DSCPTrust", PQ_DFL | PQ_BOOL, 0,
                    (void *)&dscp_trust, 0);
    parse_table_add(&pt, "UrpfDefaultRouteCheck", PQ_DFL | PQ_BOOL, 0,
                    (void *)&urpf_dft_rt_chk_en, 0);
    parse_table_add(&pt, "PIM_BIDIR", PQ_DFL | PQ_BOOL, 0,
                    (void *)&pim_bidir, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_l3_ingress_t_init(&intf);

    if (intf_id != BCM_IF_INVALID) {
        intf.flags |= BCM_L3_INGRESS_WITH_ID;
    }

    if (dscp_trust) {
        intf.flags |= BCM_L3_INGRESS_DSCP_TRUST;
    }

    if (urpf_dft_rt_chk_en) {
        intf.flags |= BCM_L3_INGRESS_URPF_DEFAULT_ROUTE_CHECK;
    }

    if (pim_bidir) {
        intf.flags |= BCM_L3_INGRESS_PIM_BIDIR;
    }

    intf.vrf = vrf;
    intf.urpf_mode = urpf_mode;
    intf.intf_class = intf_class;
    intf.qos_map_id = qos_map_id;

    rv = bcm_l3_ingress_create(unit, &intf, &intf_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding entry to L3 Ingress Interface table: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Delete a L3 ingress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ingress_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_if_t intf_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&intf_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (intf_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    rv = bcm_l3_ingress_destroy(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting ingress interface (%d): %s\n",
            ARG_CMD(arg), intf_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Get a L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ingress_get(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_ingress_t intf;
    bcm_if_t intf_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&intf_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (intf_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    bcm_l3_ingress_t_init(&intf);

    rv = bcm_l3_ingress_get(unit, intf_id, &intf);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error getting ingress interface (%d): %s\n",
            ARG_CMD(arg), intf_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("%-5s %-10s %-5s %-9s %-9s %-9s\n",
            "INTF", "Flag", "VRF", "URPF_MODE", "INTF_CLASS", "QOS_MAP_ID");
    cli_out("-----------------------------------------------------------\n");

    l3_cmd_ingress_print(unit, intf_id, &intf);

    return CMD_OK;
}

/*!
 * \brief Delete all L3 ingress interfaces.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ingress_clear(int unit, args_t *arg)
{
    int rv;

    rv = bcm_l3_ingress_traverse(unit, l3_cmd_ingress_clear_trav_cb, NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting all ingress interfaces: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Show L3 ingress interface entries.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ingress_show(int unit, args_t *arg)
{
    int rv;

    cli_out("%-5s %-10s %-5s %-9s %-9s %-9s\n",
            "INTF", "Flag", "VRF", "URPF_MODE", "INTF_CLASS", "QOS_MAP_ID");
    cli_out("-----------------------------------------------------------\n");

    rv = bcm_l3_ingress_traverse(unit, l3_cmd_ingress_show_trav_cb, NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error showing all ingress interfaces: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief L3 ingress interface cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ingress(int unit, args_t *arg)
{
    int rv;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        if ((rv = cmd_ltsw_l3_ingress_add(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        if ((rv = cmd_ltsw_l3_ingress_delete(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "get")) {
        if ((rv = cmd_ltsw_l3_ingress_get(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        if ((rv = cmd_ltsw_l3_ingress_clear(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "show")) {
        if ((rv = cmd_ltsw_l3_ingress_show(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*!
 * \brief Print the egress interface info.
 *
 * \param [in] unit Unit number.
 * \param [in] info Egress interface info.
 *
 * \return none.
 */
static void
l3_cmd_intf_print(int unit, bcm_l3_intf_t *info)
{
    char mac_str[SAL_MACADDR_STR_LEN];
    char str[20];

    format_macaddr(mac_str, info->l3a_mac_addr);
    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        if (!ltsw_feature(unit, LTSW_FT_L3_HIER)) {
            if (info->l3a_intf_flags & BCM_L3_INTF_UNDERLAY) {
                sal_strcpy(str, "UnderLay");
            } else {
                sal_strcpy(str, "OverLay");
            }
        } else {
            if (info->l3a_intf_flags & BCM_L3_INTF_OVERLAY) {
                sal_strcpy(str, "OverLay");
            } else {
                sal_strcpy(str, "UnderLay");
            }
        }
    } else {
        sal_strcpy(str, " ");
    }

    cli_out("%-5d %-5d %-18s %-9s\n",
            info->l3a_intf_id, info->l3a_vid, mac_str, str);
    cli_out("\n");
}

/*!
 * \brief Add a L3 egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_intf_add(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_intf_t intf;
    bcm_mac_t mac;
    bcm_if_t intf_id = BCM_IF_INVALID;
    int vid = 0, ul = 0;
    int rv;
    cmd_result_t retcode;

    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Vlan", PQ_DFL | PQ_INT, 0, &vid, 0);
    parse_table_add(&pt, "MAC", PQ_DFL | PQ_MAC, 0, mac, 0);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&intf_id, 0);
    parse_table_add(&pt, "UnderLay", PQ_DFL | PQ_BOOL, 0, (void *)&ul, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_l3_intf_t_init(&intf);

    sal_memcpy(intf.l3a_mac_addr, mac, sizeof(bcm_mac_t));
    intf.l3a_vid = (bcm_vlan_t)vid;
    if (intf_id != BCM_IF_INVALID) {
        intf.l3a_intf_id = intf_id;
        intf.l3a_flags |= BCM_L3_WITH_ID;
    }
    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_DISABLE_ADD_TO_ARL, 0)) {
        intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    }

    intf.l3a_intf_flags = ul ? BCM_L3_INTF_UNDERLAY : BCM_L3_INTF_OVERLAY;

    rv = bcm_l3_intf_create(unit, &intf);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding entry to L3 Egress Interface table: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Delete a L3 egress interface entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_intf_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_intf_t intf;
    bcm_if_t intf_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&intf_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (intf_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = intf_id;

    rv = bcm_l3_intf_delete(unit, &intf);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting egress interface (%d): %s\n",
            ARG_CMD(arg), intf_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Get a L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_intf_get(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_intf_t intf;
    bcm_if_t intf_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_DFL | PQ_INT, 0, (void *)&intf_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (intf_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = intf_id;

    rv = bcm_l3_intf_get(unit, &intf);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error getting egress interface (%d): %s\n",
            ARG_CMD(arg), intf_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("%-5s %-5s %-18s\n", "INTF", "VID", "MAC Address");
    cli_out("----------------------------------------------\n");
    l3_cmd_intf_print(unit, &intf);

    return CMD_OK;
}

/*!
 * \brief Delete all L3 egress interfaces.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_intf_clear(int unit, args_t *arg)
{
    int rv;

    rv = bcm_l3_intf_delete_all(unit);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting all egress interfaces: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Show L3 route entries.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_intf_show(int unit, args_t *arg)
{
    int i, rv;
    bcm_l3_info_t info;
    bcm_l3_intf_t intf;

    if ((rv = bcm_l3_info(unit, &info) < 0)) {
        cli_out("Error in L3 info access: %s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("%-5s %-5s %-18s\n", "INTF", "VID", "MAC Address");
    cli_out("----------------------------------------------\n");

    for (i = 1; i < info.l3info_max_intf; i++) {
        bcm_l3_intf_t_init(&intf);
        intf.l3a_intf_id = i;
        rv = bcm_l3_intf_get(unit, &intf);
        if (BCM_SUCCESS(rv)) {
            l3_cmd_intf_print(unit, &intf);
        } else {
            continue;
        }
    }

    return CMD_OK;
}

/*!
 * \brief L3 egress interface cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_intf(int unit, args_t *arg)
{
    int rv;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        if ((rv = cmd_ltsw_l3_intf_add(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        if ((rv = cmd_ltsw_l3_intf_delete(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "get")) {
        if ((rv = cmd_ltsw_l3_intf_get(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        if ((rv = cmd_ltsw_l3_intf_clear(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "show")) {
        if ((rv = cmd_ltsw_l3_intf_show(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*!
 * \brief Resolve the gport.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Gport number.
 * \param [in] port Port number.
 * \param [in] modid Module ID.
 * \param [in] tgid Trunk ID.
 * \param [in] port_type Port type string.
 *
 * \return none.
 */
static int
gport_resolve(int unit, bcm_gport_t gport, int *port, int *modid, int *tgid,
              char *port_type)
{
    int mod_in, port_in;

    if (BCM_GPORT_IS_BLACK_HOLE(gport)) {
        sal_strcpy(port_type, "BlackHole");
    } else if (BCM_GPORT_IS_TRUNK(gport)) {
        *tgid = BCM_GPORT_TRUNK_GET(gport);
        sal_strcpy(port_type, "trunk");
    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        *port = BCM_GPORT_LOCAL_GET(gport);
        BCM_IF_ERROR_RETURN
            (bcm_stk_my_modid_get(unit, modid));
    } else if (BCM_GPORT_IS_DEVPORT(gport)) {
        *port = BCM_GPORT_DEVPORT_PORT_GET(gport);
        BCM_IF_ERROR_RETURN
            (bcm_stk_my_modid_get(unit, modid));
    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        mod_in = BCM_GPORT_MODPORT_MODID_GET(gport);
        port_in = BCM_GPORT_MODPORT_PORT_GET(gport);
        BCM_IF_ERROR_RETURN
            (bcm_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, port_in,
                                modid, port));
    } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        *port = BCM_GPORT_MPLS_PORT_ID_GET(gport);
        BCM_IF_ERROR_RETURN
            (bcm_stk_my_modid_get(unit, modid));
        sal_strcpy(port_type, "mpls");
    } else if (BCM_GPORT_IS_FLOW_PORT(gport)) {
        *port = BCM_GPORT_FLOW_PORT_ID_GET(gport);
        BCM_IF_ERROR_RETURN
            (bcm_stk_my_modid_get(unit, modid));
        sal_strcpy(port_type, "flow");
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*!
 * \brief Print the egress object info.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress object ID.
 * \param [in] info Egress object info.
 *
 * \return none.
 */
static void
l3_cmd_egress_print(int unit, int intf_id, bcm_l3_egress_t *info)
{
    char mac_str[SAL_MACADDR_STR_LEN];
    int port, tgid, modid, rv;
    char port_type[16] = {0};

    port = -1;
    modid = -1;
    tgid = BCM_TRUNK_INVALID;

    format_macaddr(mac_str, info->mac_addr);
    if (BCM_GPORT_IS_SET(info->port)) {
        rv = gport_resolve(unit, info->port, &port, &modid, &tgid, port_type);
        if (BCM_FAILURE(rv)) {
            cli_out("Error: Invalid gport %d.\n", info->port);
        }
    } else {
        if (info->flags & BCM_L3_TGID) {
            sal_strcpy(port_type, "trunk");
        }
        port = info->port;
        modid = info->module;
        tgid = info->trunk;
    }

    if (ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        cli_out("%d %18s %6d %5d %5s %6d %5d %4s\n",
                        intf_id, mac_str, info->intf,
                        ((info->flags & BCM_L3_TGID) ? tgid : port), port_type,
                        modid, info->mtu,
                        (info->flags & BCM_L3_DST_DISCARD) ? "yes" : "no");

    } else {
        if (ltsw_feature(unit, LTSW_FT_L3_HIER)) {
            cli_out("%d %18s %6d %5d %5s %6d %5d %10s %4s\n",
                    intf_id, mac_str, info->intf,
                    ((info->flags & BCM_L3_TGID) ? tgid : port), port_type,
                    modid, info->mtu,
                    (sal_strcmp(port_type, "flow") == 0) ? "overlay" : "underlay",
                    (info->flags & BCM_L3_DST_DISCARD) ? "yes" : "no");
        } else {
            cli_out("%d %18s %6d %5d %5s %6d %5d %10s %4s\n",
                    intf_id, mac_str, info->intf,
                    ((info->flags & BCM_L3_TGID) ? tgid : port), port_type,
                    modid, info->mtu,
                    (info->flags2 & BCM_L3_FLAGS2_UNDERLAY) ? "underlay" : "overlay",
                    (info->flags & BCM_L3_DST_DISCARD) ? "yes" : "no");
        }
    }
    cli_out("\n");
}

/*!
 * \brief The callback of egress object traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress object ID.
 * \param [in] info Egress object info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_egress_show_trav_cb(int unit, bcm_if_t intf_id,
                            bcm_l3_egress_t *info, void *cookie)
{
    l3_cmd_egress_print(unit, intf_id, info);
    return BCM_E_NONE;
}

/*!
 * \brief The callback of egress object traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress object ID.
 * \param [in] info Egress object info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_egress_clear_trav_cb(int unit, bcm_if_t intf_id,
                             bcm_l3_egress_t *info, void *cookie)
{
    return bcm_l3_egress_destroy(unit, intf_id);
}

/*!
 * \brief Add an L3 egress object entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_egress_add(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_egress_t intf;
    bcm_mac_t mac;
    bcm_port_t port = 0;
    bcm_module_t modid = 0;
    bcm_trunk_t trunk = -1;
    int l2tocpu = 0, ul = 0;
    bcm_if_t l3oif = BCM_IF_INVALID;
    int egr_id = BCM_IF_INVALID;
    uint32 flags = 0;
    int rv;
    cmd_result_t retcode;

    sal_memset(mac, 0, sizeof(bcm_mac_t));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mac",  PQ_DFL | PQ_MAC, 0, (void *)mac, 0);
    parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, (void *)&port, 0);
    parse_table_add(&pt, "MOdule", PQ_DFL | PQ_INT, 0, (void *)&modid, 0);
    parse_table_add(&pt, "Trunk", PQ_DFL | PQ_INT, 0, (void *)&trunk, 0);
    parse_table_add(&pt, "L2tocpu", PQ_DFL | PQ_BOOL, 0, (void *)&l2tocpu, 0);
    parse_table_add(&pt, "INTF", PQ_DFL | PQ_INT, 0, (void *)&l3oif, 0);
    parse_table_add(&pt, "EgrId", PQ_DFL | PQ_INT, 0, (void *)&egr_id, 0);
    parse_table_add(&pt, "UnderLay", PQ_DFL | PQ_BOOL, 0, (void *)&ul, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_l3_egress_t_init(&intf);

    if (egr_id > BCM_IF_INVALID) {
        flags |= BCM_L3_WITH_ID;
    }

    intf.intf = l3oif;
    sal_memcpy(intf.mac_addr, mac, sizeof(bcm_mac_t));

    if (BCM_GPORT_IS_SET(port)) {
        intf.port = port;
    } else {
        intf.module = modid;
        if (trunk >= 0) {
            intf.flags |= BCM_L3_TGID;
            intf.trunk = trunk;
        } else {
            intf.port = port;
        }
    }

    if (l2tocpu) {
        intf.flags |= BCM_L3_L2TOCPU;
    }

    if (ul) {
        intf.flags2 |= BCM_L3_FLAGS2_UNDERLAY;
    }

    rv = bcm_l3_egress_create(unit, flags, &intf, &egr_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding L3 egress object: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Delete an L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_egress_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_if_t intf_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: EgrID=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrID", PQ_DFL | PQ_INT, 0, (void *)&intf_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (intf_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: EgrID=id\n");
        return (CMD_FAIL);
    }

    rv = bcm_l3_egress_destroy(unit, intf_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting L3 egress object (%d): %s\n",
            ARG_CMD(arg), intf_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Get an L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_egress_get(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_egress_t intf;
    bcm_if_t egr_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: EgrID=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrID", PQ_DFL | PQ_INT, 0, (void *)&egr_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (egr_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: Intf=id\n");
        return (CMD_FAIL);
    }

    bcm_l3_egress_t_init(&intf);

    rv = bcm_l3_egress_get(unit, egr_id, &intf);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error getting egress interface (%d): %s\n",
            ARG_CMD(arg), egr_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("%s %18s %6s %9s %6s %5s %10s %4s\n",
            "EgrID", " MAC Address", "Intf", "Port", "Module", "MTU",
            "Type", "Drop");
    cli_out("---------------------------------------------------------------\n");

    l3_cmd_egress_print(unit, egr_id, &intf);

    return CMD_OK;
}

/*!
 * \brief Delete all L3 ingress interfaces.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_egress_clear(int unit, args_t *arg)
{
    int rv;

    rv = bcm_l3_egress_traverse(unit, l3_cmd_egress_clear_trav_cb, NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting all L3 egress objects: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Show L3 egress object entries.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_egress_show(int unit, args_t *arg)
{
    int rv;

    cli_out("%s %18s %6s %9s %6s %5s %10s %4s\n",
            "EgrID", " MAC Address", "Intf", "Port", "Module", "MTU", "Type",
            "Drop");
    cli_out("---------------------------------------------------------------\n");

    rv = bcm_l3_egress_traverse(unit, l3_cmd_egress_show_trav_cb, NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error showing all egress objects: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief L3 egress object cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_egress(int unit, args_t *arg)
{
    int rv;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        if ((rv = cmd_ltsw_l3_egress_add(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        if ((rv = cmd_ltsw_l3_egress_delete(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "get")) {
        if ((rv = cmd_ltsw_l3_egress_get(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        if ((rv = cmd_ltsw_l3_egress_clear(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "show")) {
        if ((rv = cmd_ltsw_l3_egress_show(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*!
 * \brief Print the ECMP object info.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress object ID.
 * \param [in] info Egress object info.
 *
 * \return none.
 */
static void
l3_cmd_ecmp_print(int unit, bcm_l3_egress_ecmp_t *info, int member_cnt,
                  bcm_l3_ecmp_member_t *member_arr)
{
    int idx;
    int ul = false;

    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        if (ltsw_feature(unit, LTSW_FT_L3_HIER)) {
            ul = (info->ecmp_group_flags & BCM_L3_ECMP_OVERLAY) ? false : true;
        } else {
            ul = (info->ecmp_group_flags & BCM_L3_ECMP_UNDERLAY) ? true : false;
        }
        cli_out("ECMP group %d(%s):\n\t", info->ecmp_intf,
                ul ? "UnderLay" : "OverLay");
    } else {
        cli_out("ECMP group %d:\n\t", info->ecmp_intf);
    }
    cli_out("Dynamic mode %d, Flags 0x%x, Max path %d\n\t",
            info->dynamic_mode, info->ecmp_group_flags, info->max_paths);
    cli_out("Interfaces: (member count %d)\n\t", member_cnt);
    for (idx = 0; idx < member_cnt; idx++) {
        cli_out("{");
        cli_out("%d", member_arr[idx].egress_if);
        if (member_arr[idx].egress_if_2) {
            cli_out(" %d", member_arr[idx].egress_if_2);
        }
        cli_out("} ");
        if (idx && (!(idx % 10))) {
            cli_out("\n");
        }
    }
    cli_out("\n");
}

/*!
 * \brief The callback of ECMP traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress object ID.
 * \param [in] info Egress object info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_ecmp_show_trav_cb(int unit, bcm_l3_egress_ecmp_t *info, int member_cnt,
                         bcm_l3_ecmp_member_t *member_arr, void *cookie)
{
    l3_cmd_ecmp_print(unit, info, member_cnt, member_arr);
    return BCM_E_NONE;
}

/*!
 * \brief The callback of ECMP traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Egress object ID.
 * \param [in] info Egress object info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_ecmp_clear_trav_cb(int unit, bcm_l3_egress_ecmp_t *info, int member_cnt,
                          bcm_l3_ecmp_member_t *member_arr, void *cookie)
{
    return bcm_l3_ecmp_destroy(unit, info->ecmp_intf);
}

/*!
 * \brief Add an L3 ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp_add(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_egress_ecmp_t ecmp;
    int grp_id = BCM_IF_INVALID;
    int max_paths = 0, dynamic_mode = 0, egr_if_cnt = 0, ul = 0;
    int ol_egr_if_arr[ECMP_MEMBER_MAX] = {0};
    int ul_egr_if_arr[ECMP_MEMBER_MAX] = {0};
    char ol_egr_if_str[ECMP_MEMBER_MAX][16];
    char ul_egr_if_str[ECMP_MEMBER_MAX][16];
    bcm_l3_ecmp_member_t member_arr[ECMP_MEMBER_MAX];
    int i, rv;
    uint32 options = 0;
    cmd_result_t retcode;

    sal_memset(member_arr, 0, sizeof(bcm_l3_ecmp_member_t) * ECMP_MEMBER_MAX);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "GRouPID",  PQ_DFL | PQ_INT, 0, (void *)&grp_id, 0);
    parse_table_add(&pt, "MaxPaths", PQ_DFL | PQ_INT, 0, (void *)&max_paths, 0);
    parse_table_add(&pt, "DynamicMode", PQ_DFL | PQ_INT, 0,
                    (void *)&dynamic_mode, 0);
    parse_table_add(&pt, "UnderLay", PQ_DFL | PQ_BOOL, 0, (void *)&ul, 0);
    for (i = 0; i < ECMP_MEMBER_MAX; i++) {
        sal_sprintf(ol_egr_if_str[i], "OL_INTF%d", i);
        parse_table_add(&pt, ol_egr_if_str[i], PQ_DFL | PQ_INT, 0,
                        (void *)&ol_egr_if_arr[i], 0);

        sal_sprintf(ul_egr_if_str[i], "UL_INTF%d", i);
        parse_table_add(&pt, ul_egr_if_str[i], PQ_DFL | PQ_INT, 0,
                        (void *)&ul_egr_if_arr[i], 0);
    }

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);

    if (grp_id > BCM_IF_INVALID) {
        options |= BCM_L3_ECMP_O_CREATE_WITH_ID;
        ecmp.ecmp_intf = grp_id;
    }

    if (!ul) {
        ecmp.ecmp_group_flags |= BCM_L3_ECMP_OVERLAY;
    } else {
        ecmp.ecmp_group_flags |= BCM_L3_ECMP_UNDERLAY;
    }

    ecmp.max_paths = max_paths;
    ecmp.dynamic_mode = dynamic_mode;

    for (i = 0; i < ECMP_MEMBER_MAX; i++) {
        if (!ol_egr_if_arr[i] && !ul_egr_if_arr[i]) {
            continue;
        }

        if (ol_egr_if_arr[i] > 0) {
            member_arr[egr_if_cnt].egress_if = ol_egr_if_arr[i];
        }
        if (ul_egr_if_arr[i] > 0) {
            member_arr[egr_if_cnt].egress_if_2 = ul_egr_if_arr[i];
        }
        egr_if_cnt++;
    }
    rv = bcm_l3_ecmp_create(unit, options, &ecmp, egr_if_cnt, member_arr);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding L3 ECMP group: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Delete an L3 ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_if_t grp_id = BCM_IF_INVALID;
    int rv;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: GRouPID=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "GRouPID", PQ_DFL | PQ_INT, 0, (void *)&grp_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (grp_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: GRouPID=id\n");
        return (CMD_FAIL);
    }

    rv = bcm_l3_ecmp_destroy(unit, grp_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting L3 ECMP group (%d): %s\n",
            ARG_CMD(arg), grp_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Get an L3 ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp_get(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_l3_egress_ecmp_t ecmp;
    bcm_if_t grp_id = BCM_IF_INVALID;
    int egr_if_cnt = 0;
    bcm_l3_ecmp_member_t *member_arr = NULL;
    int rv, sz;
    cmd_result_t retcode;

    if (!ARG_CNT(arg)) {
        cli_out("Expected command parameters: GRoupID=id\n");
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "GRouPID", PQ_DFL | PQ_INT, 0, (void *)&grp_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (grp_id == BCM_IF_INVALID) {
        cli_out("Expected command parameters: GRouPID=id\n");
        return (CMD_FAIL);
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.ecmp_intf = grp_id;
    rv = bcm_l3_ecmp_get(unit, &ecmp, 0, NULL, &egr_if_cnt);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error getting ECMP group (%d): %s\n",
            ARG_CMD(arg), grp_id, bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    if (egr_if_cnt) {
        sz = egr_if_cnt * sizeof(bcm_l3_ecmp_member_t);
        member_arr = sal_alloc(sz, "ecmp member");
        if (member_arr == NULL) {
            cli_out("%s: ERROR: %s\n", ARG_CMD(arg), bcm_errmsg(BCM_E_MEMORY));
            return (CMD_FAIL);
        }
        sal_memset(member_arr, 0, sz);
        rv = bcm_l3_ecmp_get(unit, &ecmp, egr_if_cnt, member_arr, &egr_if_cnt);
        if (BCM_FAILURE(rv)) {
            sal_free(member_arr);
            cli_out("%s: Error getting ECMP group (%d): %s\n",
                ARG_CMD(arg), grp_id, bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    }

    l3_cmd_ecmp_print(unit, &ecmp, egr_if_cnt, member_arr);
    if (!member_arr) {
        sal_free(member_arr);
    }

    return CMD_OK;
}

/*!
 * \brief Delete all L3 ECMP groups.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp_clear(int unit, args_t *arg)
{
    int rv;

    rv = bcm_l3_ecmp_traverse(unit, l3_cmd_ecmp_clear_trav_cb, NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting all L3 ECMP groups: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Show L3 ECMP groups.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp_show(int unit, args_t *arg)
{
    int rv;

    rv = bcm_l3_ecmp_traverse(unit, l3_cmd_ecmp_show_trav_cb, NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error showing all ECMP groups: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Add/delete an L3 ECMP member.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp_member_op(int unit, args_t *arg)
{
    char *subcmd;
    parse_table_t pt;
    int grp_id = BCM_IF_INVALID;
    int ol_egr_if = BCM_IF_INVALID;
    int ul_egr_if = BCM_IF_INVALID;
    bcm_l3_ecmp_member_t member;
    int insert = 0;
    int rv;
    cmd_result_t retcode;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    } else if(!sal_strcasecmp(subcmd, "insert")) {
        insert = true;
    } else if (!sal_strcasecmp(subcmd, "remove")) {
        insert = false;
    } else {
        cli_out("%s: Unsupported command to ECMP member.\n", ARG_CMD(arg));
        return (CMD_FAIL);
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "GRouPID",  PQ_DFL | PQ_INT, 0, (void *)&grp_id, 0);
    parse_table_add(&pt, "OL_INTF", PQ_DFL | PQ_INT, 0, (void *)&ol_egr_if, 0);
    parse_table_add(&pt, "UL_INTF", PQ_DFL | PQ_INT, 0, (void *)&ul_egr_if, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_l3_ecmp_member_t_init(&member);
    member.egress_if = ol_egr_if;
    member.egress_if_2 = ul_egr_if;

    if (insert) {
        rv = bcm_l3_ecmp_member_add(unit, grp_id, &member);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error adding member to ECMP groups: %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    } else {
        rv = bcm_l3_ecmp_member_delete(unit, grp_id, &member);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error deleting member from ECMP groups: %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    }

    return CMD_OK;
}

/*!
 * \brief L3 ECMP cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_ecmp(int unit, args_t *arg)
{
    int rv;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        if ((rv = cmd_ltsw_l3_ecmp_add(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        if ((rv = cmd_ltsw_l3_ecmp_delete(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "get")) {
        if ((rv = cmd_ltsw_l3_ecmp_get(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        if ((rv = cmd_ltsw_l3_ecmp_clear(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "show")) {
        if ((rv = cmd_ltsw_l3_ecmp_show(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "member")) {
        if ((rv = cmd_ltsw_l3_ecmp_member_op(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*!
 * \brief Print the host info.
 *
 * \param [in] unit Unit number.
 * \param [in] info host info.
 *
 * \return none.
 */
static void
l3_cmd_host_print(int unit, bcm_l3_host_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];

    if (info->l3a_flags & BCM_L3_IP6) {
        format_ip6addr(ip_str, info->l3a_ip6_addr);
        cli_out("%-4d %-19s %-4d %-4d", info->l3a_vrf, ip_str,
                info->l3a_intf, info->l3a_ul_intf);
    } else {
        format_ipaddr(ip_str, info->l3a_ip_addr);
        cli_out("%-4d %-19s %-4d %-4d", info->l3a_vrf, ip_str, info->l3a_intf,
                info->l3a_ul_intf);
    }
    cli_out("\n");
}

/*!
 * \brief The callback of host traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] index Entry index.
 * \param [in] info Host info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_host_trav_cb(int unit, int index, bcm_l3_host_t *info, void *cookie)
{
    cli_out("%-8d", index);
    l3_cmd_host_print(unit, info);
    return BCM_E_NONE;
}

/*!
 * \brief Add a L3 host entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_host_add(int unit, args_t *arg)
{
    bcm_ip_t ip_addr = 0;
    bcm_ip6_t ip6_addr;
    int vrf = 0, ol_intf = 0, ul_intf = 0, v6 = 0;
    bcm_l3_host_t host_info;
    parse_table_t pt;
    int rv, ecmp = 0;

    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "INTF", PQ_DFL | PQ_INT, 0, (void *)&ol_intf, 0);
    parse_table_add(&pt, "UL_INTF", PQ_DFL | PQ_INT, 0, (void *)&ul_intf, 0);
    parse_table_add(&pt, "ECMP",    PQ_DFL | PQ_BOOL, 0, (void *)&ecmp, 0);

    if (!ARG_CNT(arg)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (pt.pt_entries[1].pq_type & PQ_PARSED) {
        v6 = true;
    }

    parse_arg_eq_done(&pt);

    bcm_l3_host_t_init(&host_info);

    if (v6) {
        sal_memcpy(host_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
        host_info.l3a_flags |= BCM_L3_IP6;
    } else {
        host_info.l3a_ip_addr = ip_addr;
    }

    host_info.l3a_vrf = vrf;
    host_info.l3a_intf = ol_intf;
    host_info.l3a_ul_intf = ul_intf;
    if (ecmp) {
        host_info.l3a_flags |= BCM_L3_MULTIPATH;
    }

    rv = bcm_l3_host_add(unit, &host_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding host table: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Delete a L3 host entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_host_delete(int unit, args_t *arg)
{
    bcm_ip_t ip_addr = 0;
    bcm_ip6_t ip6_addr;
    int vrf = 0, v6 = 0;
    bcm_l3_host_t host_info;
    parse_table_t pt;
    int rv;

    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);

    if (!ARG_CNT(arg)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (pt.pt_entries[1].pq_type & PQ_PARSED) {
        v6 = true;
    }

    parse_arg_eq_done(&pt);

    bcm_l3_host_t_init(&host_info);

    if (v6) {
        sal_memcpy(host_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
        host_info.l3a_flags |= BCM_L3_IP6;
    } else {
        host_info.l3a_ip_addr = ip_addr;
    }

    host_info.l3a_vrf = vrf;

    rv = bcm_l3_host_delete(unit, &host_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting host table: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Get a L3 host entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_host_get(int unit, args_t *arg)
{
    bcm_ip_t ip_addr = 0;
    bcm_ip6_t ip6_addr;
    int vrf = 0, v6 = 0;
    bcm_l3_host_t host_info;
    parse_table_t pt;
    int rv;

    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);

    if (!ARG_CNT(arg)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (pt.pt_entries[1].pq_type & PQ_PARSED) {
        v6 = true;
    }

    parse_arg_eq_done(&pt);

    bcm_l3_host_t_init(&host_info);

    if (v6) {
        sal_memcpy(host_info.l3a_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
        host_info.l3a_flags |= BCM_L3_IP6;
    } else {
        host_info.l3a_ip_addr = ip_addr;
    }

    host_info.l3a_vrf = vrf;

    rv = bcm_l3_host_find(unit, &host_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error getting host entry: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("%3s %-19s %-4s %-4s\n", "VRF", "Net Addr", "INTF", "UL_INTF");
    cli_out("--------------------------------------------\n");
    l3_cmd_host_print(unit, &host_info);

    return CMD_OK;
}

/*!
 * \brief Delete all L3 host entries.
 *
 * Use option 'v6' to specify the host table to be cleared.
 * By default, only IPv4 host table will be cleared.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_host_clear(int unit, args_t *arg)
{
    int v6 = 0;
    bcm_l3_host_t host_info;
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;

    if (ARG_CNT(arg)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "V6", PQ_DFL | PQ_BOOL, 0, (void *)&v6, 0);
        if(!parseEndOk(arg, &pt, &retcode)) {
            return retcode;
        }

    }

    bcm_l3_host_t_init(&host_info);

    if (v6) {
        host_info.l3a_flags |= BCM_L3_IP6;
    }

    rv = bcm_l3_host_delete_all(unit, &host_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error clearing host entry: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Show L3 host entries.
 *
 * Use option 'v6' to specify the host table to be shown.
 * By default, only show entries in IPv4 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_host_show(int unit, args_t *arg)
{
    int v6 = 0;
    int start, end;
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    uint32 flags = 0;
    bcm_l3_info_t l3info;

    start = end = 0;

    if (ARG_CNT(arg)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "V6", PQ_DFL | PQ_BOOL, 0, (void *)&v6, 0);
        parse_table_add(&pt, "START", PQ_DFL | PQ_INT, 0, (void *)&start, 0);
        parse_table_add(&pt, "END", PQ_DFL | PQ_INT, 0, (void *)&end, 0);

        if(!parseEndOk(arg, &pt, &retcode)) {
            return retcode;
        }

        if (v6) {
            flags = BCM_L3_IP6;
        }
    }

    if (!end) {
        rv = bcm_l3_info(unit, &l3info);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 info accessing: %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        end = l3info.l3info_max_host;
    }

    cli_out("%8s %-19s %-4s %-4s\n", "VRF", "Net Addr", "INTF", "UL_INTF");
    cli_out("--------------------------------------------\n");
    rv = bcm_l3_host_traverse(unit, flags, start, end, l3_cmd_host_trav_cb,
                              NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error showing host entry: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief L3 host cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_host(int unit, args_t *arg)
{
    int rv;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        if ((rv = cmd_ltsw_l3_host_add(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        if ((rv = cmd_ltsw_l3_host_delete(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "get")) {
        if ((rv = cmd_ltsw_l3_host_get(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        if ((rv = cmd_ltsw_l3_host_clear(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "show")) {
        if ((rv = cmd_ltsw_l3_host_show(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*!
 * \brief Print the route info.
 *
 * \param [in] unit Unit number.
 * \param [in] info Route info.
 *
 * \return none.
 */
static void
l3_cmd_route_print(int unit, bcm_l3_route_t *info)
{
    char ip_str[IP6ADDR_STR_LEN + 3];
    char vrf_str[20];
    int masklen;

    if (info->l3a_vrf == BCM_L3_VRF_GLOBAL) {
        sal_strcpy(vrf_str, "Global");
    } else if (info->l3a_vrf == BCM_L3_VRF_OVERRIDE) {
        sal_strcpy(vrf_str, "Override");
    } else {
        sal_sprintf(vrf_str, "%d", info->l3a_vrf);
    }

    if (info->l3a_flags & BCM_L3_IP6) {
        format_ip6addr(ip_str, info->l3a_ip6_net);
        masklen = bcm_ip6_mask_length(info->l3a_ip6_mask);
        cli_out("%-8s %-15s/%d %-4d %-4d", vrf_str, ip_str, masklen,
                info->l3a_intf, info->l3a_ul_intf);
    } else {
        format_ipaddr_mask(ip_str, info->l3a_subnet, info->l3a_ip_mask);
        cli_out("%-8s %-19s %-4d %-4d", vrf_str, ip_str, info->l3a_intf,
                info->l3a_ul_intf);
    }
    cli_out("\n");
}

/*!
 * \brief The callback of route traverse.
 *
 * \param [in] unit Unit number.
 * \param [in] index Entry index.
 * \param [in] info Route info.
 * \param [in] cookie User data.
 *
 * \retval BCM_E_NONE No error.
 */
static int
l3_cmd_route_trav_cb(int unit, int index, bcm_l3_route_t *info, void *cookie)
{
    cli_out("%-8d", index);
    l3_cmd_route_print(unit, info);
    return BCM_E_NONE;
}

/*!
 * \brief Add a L3 route entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route_add(int unit, args_t *arg)
{
    bcm_ip_t ip_addr = 0;
    bcm_ip6_t ip6_addr;
    int masklen = 0, vrf = 0, ol_intf = 0, ul_intf = 0, v6 = 0, dst_discard = 0;
    bcm_l3_route_t route_info;
    parse_table_t pt;
    int rv;
    int replace = 0, ecmp = 0;

    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "MASKLEN", PQ_DFL | PQ_INT, 0, (void *)&masklen, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
    parse_table_add(&pt, "INTF", PQ_DFL | PQ_INT, 0, (void *)&ol_intf, 0);
    parse_table_add(&pt, "UL_INTF", PQ_DFL | PQ_INT, 0, (void *)&ul_intf, 0);
    parse_table_add(&pt, "DstDiscard", PQ_DFL | PQ_BOOL, 0,
                    (void *)&dst_discard, 0);
    parse_table_add(&pt, "Replace", PQ_DFL | PQ_BOOL, 0, (void *)&replace, 0);
    parse_table_add(&pt, "ECMP",    PQ_DFL | PQ_BOOL, 0, (void *)&ecmp, 0);

    if (!ARG_CNT(arg)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (pt.pt_entries[1].pq_type & PQ_PARSED) {
        v6 = true;
    }

    parse_arg_eq_done(&pt);

    bcm_l3_route_t_init(&route_info);

    if (replace) {
        route_info.l3a_flags |= BCM_L3_REPLACE;
    }

    if (v6) {
        sal_memcpy(route_info.l3a_ip6_net, ip6_addr, BCM_IP6_ADDRLEN);
        bcm_ip6_mask_create(route_info.l3a_ip6_mask, masklen);
        route_info.l3a_flags |= BCM_L3_IP6;
    } else {
        route_info.l3a_subnet = ip_addr;
        route_info.l3a_ip_mask = bcm_ip_mask_create(masklen);
    }

    route_info.l3a_vrf = vrf;
    route_info.l3a_intf = ol_intf;
    route_info.l3a_ul_intf = ul_intf;
    if (ecmp) {
        route_info.l3a_flags |= BCM_L3_MULTIPATH;
    }

    if (dst_discard) {
        route_info.l3a_flags |= BCM_L3_DST_DISCARD;
    }

    rv = bcm_l3_route_add(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding route table: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Delete a L3 route entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route_delete(int unit, args_t *arg)
{
    bcm_ip_t ip_addr = 0;
    bcm_ip6_t ip6_addr;
    int masklen = 0, vrf = 0, v6 = 0;
    bcm_l3_route_t route_info;
    parse_table_t pt;
    int rv;

    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "MASKLEN", PQ_DFL | PQ_INT, 0, (void *)&masklen, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);

    if (!ARG_CNT(arg)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (pt.pt_entries[1].pq_type & PQ_PARSED) {
        v6 = true;
    }

    parse_arg_eq_done(&pt);

    bcm_l3_route_t_init(&route_info);

    if (v6) {
        sal_memcpy(route_info.l3a_ip6_net, ip6_addr, BCM_IP6_ADDRLEN);
        bcm_ip6_mask_create(route_info.l3a_ip6_mask, masklen);
        route_info.l3a_flags |= BCM_L3_IP6;
    } else {
        route_info.l3a_subnet = ip_addr;
        route_info.l3a_ip_mask = bcm_ip_mask_create(masklen);
    }

    route_info.l3a_vrf = vrf;

    rv = bcm_l3_route_delete(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting route table: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Get a L3 route entry.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route_get(int unit, args_t *arg)
{
    bcm_ip_t ip_addr = 0;
    bcm_ip6_t ip6_addr;
    int masklen = 0, vrf = 0, v6 = 0;
    bcm_l3_route_t route_info;
    parse_table_t pt;
    int rv;

    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "IP6", PQ_DFL | PQ_IP6, 0, (void *)&ip6_addr, 0);
    parse_table_add(&pt, "MASKLEN", PQ_DFL | PQ_INT, 0, (void *)&masklen, 0);
    parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);

    if (!ARG_CNT(arg)) {  /* Display settings */
        cli_out("Current settings:\n");
        parse_eq_format(&pt);
        parse_arg_eq_done(&pt);
        return CMD_OK;
    }

    if (parse_arg_eq(arg, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (pt.pt_entries[1].pq_type & PQ_PARSED) {
        v6 = true;
    }

    parse_arg_eq_done(&pt);

    bcm_l3_route_t_init(&route_info);

    if (v6) {
        sal_memcpy(route_info.l3a_ip6_net, ip6_addr, BCM_IP6_ADDRLEN);
        bcm_ip6_mask_create(route_info.l3a_ip6_mask, masklen);
        route_info.l3a_flags |= BCM_L3_IP6;
    } else {
        route_info.l3a_subnet = ip_addr;
        route_info.l3a_ip_mask = bcm_ip_mask_create(masklen);
    }

    route_info.l3a_vrf = vrf;

    rv = bcm_l3_route_get(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error getting route entry: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("%3s %-19s %-4s %-4s\n", "VRF", "Net Addr", "INTF", "UL_INTF");
    cli_out("--------------------------------------------\n");
    l3_cmd_route_print(unit, &route_info);

    return CMD_OK;
}

/*!
 * \brief Delete all L3 route entries.
 *
 * Use option 'v6' to specify the route table to be cleared.
 * By default, only IPv4 route table will be cleared.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route_clear(int unit, args_t *arg)
{
    int v6 = 0;
    bcm_l3_route_t route_info;
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;

    if (ARG_CNT(arg)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "V6", PQ_DFL | PQ_BOOL, 0, (void *)&v6, 0);
        if(!parseEndOk(arg, &pt, &retcode)) {
            return retcode;
        }

    }

    bcm_l3_route_t_init(&route_info);

    if (v6) {
        route_info.l3a_flags |= BCM_L3_IP6;
    }

    rv = bcm_l3_route_delete_all(unit, &route_info);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error clearing route entry: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Show L3 route entries.
 *
 * Use option 'v6' to specify the route table to be shown.
 * By default, only show entries in IPv4 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route_show(int unit, args_t *arg)
{
    int v6 = 0;
    int start, end;
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    uint32 flags = 0;
    bcm_l3_info_t l3info;

    start = end = 0;

    if (ARG_CNT(arg)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "V6", PQ_DFL | PQ_BOOL, 0, (void *)&v6, 0);
        parse_table_add(&pt, "START", PQ_DFL | PQ_INT, 0, (void *)&start, 0);
        parse_table_add(&pt, "END", PQ_DFL | PQ_INT, 0, (void *)&end, 0);

        if(!parseEndOk(arg, &pt, &retcode)) {
            return retcode;
        }

        if (v6) {
            flags = BCM_L3_IP6;
        }
    }

    if (!end) {
        rv = bcm_l3_info(unit, &l3info);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error L3 info accessing: %s\n",
                    ARG_CMD(arg), bcm_errmsg(rv));
            return CMD_FAIL;
        }

        end = l3info.l3info_max_route;
    }

    cli_out("%7s %-8s %-18s %4s %4s\n", " ", "VRF", "Net Addr", "INTF", "UL_INTF");
    cli_out("--------------------------------------------\n");
    rv = bcm_l3_route_traverse(unit, flags, start, end, l3_cmd_route_trav_cb,
                               NULL);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error showing route entry: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

/*!
 * \brief Add a time measurement to a perf_data_t instance.
 *
 * \param [in] perf Performance data.
 * \param [in] duration Time measurement.
 *
 * \return none.
 */
static void
route_perf_data_add(route_perf_data_t *perf, int duration)
{
    if (perf->count == 0) {
        perf->count = 1;
        perf->min = duration;
        perf->max = duration;
        perf->total = duration;
    } else {
        perf->count++;
        if (duration > perf->max) {
            perf->max = duration;
        }
        if (duration < perf->min) {
            perf->min = duration;
        }
        perf->total += duration;
    }
}

/*!
 * \brief Print performance data.
 *
 * \param [in] perf Performance data.
 * \param [in] route_cnt Added route number.
 * \param [in] label Label.
 *
 * \return none.
 */
static void
route_perf_data_print(route_perf_data_t *perf, int route_cnt, const char *label)
{
    if( perf->count > 0) {
        /*int u_avg = perf->total / perf->count;*/
        uint32 temp;

        temp = route_cnt * perf->count;
        if (temp > 4294) {
            uint32 temp2 = 0;
            uint32 rem = 0, divn;
            while ((int)temp > 0) {
                divn = (temp > 2147) ? 2147 : temp;
                divn = divn * 1000000;
                temp2 += (divn + rem) / perf->total;
                rem = (divn + rem) % perf->total;
                temp -= 2147;
            }
            temp = temp2;
        } else {
            temp = temp * 1000000 / perf->total;
        }
        cli_out(" %7d ", temp);
    } else {
        cli_out("    -    ");
    }
}

/*!
 * \brief Preparation for route performance test.
 *
 * \param [in] unit Unit number.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_test_setup(int unit)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    bcm_l3_egress_t egr_obj;
    bcm_if_t l3_intf_id;
    bcm_mac_t smac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0xfe, 0x01};
    int vrf_arr[] = {ROUTE_PERF_VRF,
                     BCM_L3_VRF_GLOBAL,
                     BCM_L3_VRF_OVERRIDE};
    int i = 0;
    bcm_l3_vrf_route_data_mode_t dmode;

    /* Turn off back ground threads */
    sh_process_command(unit, "l2 learn off");
    sh_process_command(unit, "l2 age off");
    sh_process_command(unit, "linkscan off");
    sh_process_command(unit, "counter off");

    l3_intf_id = 1;

    /* L3 Interface */
    bcm_l3_intf_t_init(&l3_intf);
    sal_memcpy(l3_intf.l3a_mac_addr, smac, sizeof(smac));
    l3_intf.l3a_flags = BCM_L3_WITH_ID;
    l3_intf.l3a_intf_id = l3_intf_id;
    l3_intf.l3a_vid = 1;
    rv = bcm_l3_intf_create(unit, &l3_intf);
    if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
        cli_out("UT Route: Create L3 intf failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    /* L3 Egress */
    bcm_l3_egress_t_init(&egr_obj);
    egr_obj.intf = l3_intf_id;
    egr_obj.port = 1;
    sal_memcpy(egr_obj.mac_addr, dmac, sizeof(dmac));
    rv = bcm_l3_egress_create(unit, 0, &egr_obj, &egr_obj_id[unit]);
    if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
        cli_out("UT Route: Error creating egress object: %s\n", bcm_errmsg(rv));
        return rv;
    }

    bcm_l3_egress_t_init(&egr_obj);
    dmac[5]++;
    egr_obj.intf = l3_intf_id;
    egr_obj.port = 2;
    sal_memcpy(egr_obj.mac_addr, dmac, sizeof(dmac));
    rv = bcm_l3_egress_create(unit, 0, &egr_obj, &egr_obj_id2[unit]);
    if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
        cli_out("UT Route: Error creating egress object: %s\n", bcm_errmsg(rv));
        return rv;
    }

    for (i = 0; i < COUNTOF(vrf_arr); i ++) {
        rv = bcm_l3_vrf_route_data_mode_get(unit, vrf_arr[i], 0, &dmode);
        if (BCM_SUCCESS(rv)) {
            alpm_dmode[i][0] = dmode;
        }
    }

    for (i = 0; i < COUNTOF(vrf_arr); i ++) {
        rv = bcm_l3_vrf_route_data_mode_get(unit, vrf_arr[i], BCM_L3_IP6,
                                            &dmode);
        if (BCM_SUCCESS(rv)) {
            alpm_dmode[i][1] = dmode;
        }
    }

    return BCM_E_NONE;
}

/*!
 * \brief Operate IPv4 route table through LT interfaces.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation code.
 * \param [in] vrf VRF.
 * \param [in] ip IP address.
 * \param [in] ip_mask IP mask.
 * \param [in] intf Egress object ID.
 * \param [in] alpm_dmode alpm data mode array.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_v4_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint32 ip,
    uint32 ip_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    int rv;
    rv = bcmi_l3_route_perf_v4_lt_op(unit, op, vrf,
                                     ip, ip_mask, intf, alpm_dmode);
    return rv;
}

/*!
 * \brief Operate IPv4 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation code.
 * \param [in] use_api Use BCM API.
 * \param [in] set_idx Index of set.
 * \param [in] rpd Pointer to performance data.
 * \param [in] vrf Vrf value.
 * \param [out] route_cnt Added route number.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_v4_op_test(int unit, bcmi_ltsw_l3_route_perf_opcode_t op, int use_api,
                      int set_idx, route_perf_data_t *rpd, int vrf, int *route_cnt)
{
    int rv = BCM_E_NONE;
    int start, end, duration = 0;
    bcm_l3_route_t route_info;
    uint32_t ip, start_ip = 0x01010101;
    uint32 ip_mask = 0xffffffff;
    int loop = 4096;
    int loop_ip = 1024;
    int i, j, intf;
    uint32 prfx[4] = {22, 23, 24, 20};
    uint32 mask[4] = {0xfffffc00, 0xfffffe00, 0xffffff00, 0xfffff000};
    uint32 ipv4[4] = {0x02020202, 0x29020202, 0x61020202, 0x8C010101};
    uint32 base_ip;
    int (*op_func[BCMI_LTSW_RP_OPCODE_NUM])(int, bcm_l3_route_t *) = {
        bcm_l3_route_add,
        bcm_l3_route_add,
        bcm_l3_route_get,
        bcm_l3_route_delete
    };

    *route_cnt = 0;

    bcm_l3_route_t_init(&route_info);

    start = sal_time_usecs();
    ip = start_ip + set_idx * loop;

    for (i = 0; i < loop; i++) {
        if (use_api) {
            route_info.l3a_vrf = vrf;
            route_info.l3a_subnet = ip;
            route_info.l3a_ip_mask= ip_mask;
            route_info.l3a_intf = egr_obj_id[unit];
            if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                route_info.l3a_flags = BCM_L3_REPLACE;
                route_info.l3a_intf = egr_obj_id2[unit];
            }

            rv = op_func[op](unit, &route_info);
        } else {
            if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                intf = egr_obj_id2[unit] - 100000;/* _SHR_L3_EGRESS_IDX_MIN */
            } else {
                intf = egr_obj_id[unit] - 100000;
            }
            rv = route_perf_v4_lt_op(unit, op, vrf, ip, ip_mask,
                                     intf, alpm_dmode);
        }
        if (BCM_FAILURE(rv)) {
            end = sal_time_usecs();
            duration = end - start;
            goto exit;
        } else {
            *route_cnt += 1;
        }

        ip++;
    }

    end = sal_time_usecs();
    duration = end - start;

    for (j = 0; j < 4; j++) {
        start = sal_time_usecs();
        for (i = 0; i < loop_ip; i++) {
            base_ip = ((loop_ip + i + loop_ip * set_idx) << (32 - prfx[j]));
            ip = base_ip + ipv4[j];
            ip_mask = mask[j];

            if (use_api) {
                route_info.l3a_vrf = vrf;
                route_info.l3a_subnet = ip;
                route_info.l3a_ip_mask= ip_mask;
                route_info.l3a_intf = egr_obj_id[unit];
                if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                    route_info.l3a_flags = BCM_L3_REPLACE;
                    route_info.l3a_intf = egr_obj_id2[unit];
                }

                rv = op_func[op](unit, &route_info);
            } else {
                if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                    intf = egr_obj_id2[unit] - 100000;
                } else {
                    intf = egr_obj_id[unit] - 100000;
                }
                rv = route_perf_v4_lt_op(unit, op, vrf, ip, ip_mask,
                                         intf, alpm_dmode);
            }
            if (BCM_FAILURE(rv)) {
                end = sal_time_usecs();
                duration += (end - start);
                goto exit;
            } else {
                *route_cnt += 1;
            }
        }
        end = sal_time_usecs();
        duration += (end - start);
    }

exit:
    route_perf_data_add(&rpd[set_idx], duration);
    return rv;
}

/*!
 * \brief Clean up resource allocated for route performance test.
 *
 * \param [in] unit Unit number.
 *
 * \retval None.
 */
static void
route_perf_test_clean(int unit)
{
    int rv;
    bcm_l3_intf_t intf;

    rv = bcm_l3_egress_destroy(unit, egr_obj_id2[unit]);
    if (BCM_FAILURE(rv) && (rv != BCM_E_BUSY) && (rv != BCM_E_NOT_FOUND)) {
        cli_out("UT Route: Error deleting egress obj2: %s\n", bcm_errmsg(rv));
    }

    rv = bcm_l3_egress_destroy(unit, egr_obj_id[unit]);
    if (BCM_FAILURE(rv) && (rv != BCM_E_BUSY) && (rv != BCM_E_NOT_FOUND)) {
        cli_out("UT Route: Error deleting egress obj: %s\n", bcm_errmsg(rv));
    }

    bcm_l3_intf_t_init(&intf);
    intf.l3a_intf_id = 1;
    rv = bcm_l3_intf_delete(unit, &intf);
    if (BCM_FAILURE(rv) && (rv != BCM_E_BUSY) && (rv != BCM_E_NOT_FOUND)) {
        cli_out("UT Route: Error deleting interface: %s\n", bcm_errmsg(rv));
    }

    /* Turn on back ground threads */
    sh_process_command(unit, "l2 learn on");
    sh_process_command(unit, "l2 age on");
    sh_process_command(unit, "linkscan on");
    sh_process_command(unit, "counter on");

    return;
}

/*!
 * \brief Performance test on IPv4 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] ops Bitmap of operation code.
 * \param [in] loops Number of runs.
 * \param [in] nsets Number of sets.
 * \param [in] use_api Use BCM API.
 * \param [in] vrf Vrf value.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_v4_test(int unit, int ops, int loops, int nsets, int use_api, int vrf)
{
    int run, i, sz, op;
    int route_add = 0, set_size = 0, route_cnt = 0, tot_route = 0;
    route_perf_data_t *rpd[BCMI_LTSW_RP_OPCODE_NUM], *ptr;
    char *data, *pd = NULL;
    int rv = BCM_E_NONE;
    bcm_l3_route_t route_info;

    sz = sizeof(route_perf_data_t) * nsets;

    data = sal_alloc((sz * BCMI_LTSW_RP_OPCODE_NUM), "bcmRoutePerfData");
    if (!data) {
        return BCM_E_MEMORY;
    }
    sal_memset(data, 0, (sz * BCMI_LTSW_RP_OPCODE_NUM));

    for (i = 0; i < BCMI_LTSW_RP_OPCODE_NUM; i++) {
        rpd[i] = (route_perf_data_t *)(data + i * sz);
    }

    for (run = 0; run < loops; run++) {
        if (ops & (1 << BCMI_LTSW_RP_OPCODE_ADD)) {
            /* Add default route */
            if (use_api) {
                bcm_l3_route_t_init(&route_info);

                route_info.l3a_vrf = vrf;
                route_info.l3a_subnet = 0;
                route_info.l3a_ip_mask= 0;
                route_info.l3a_intf = egr_obj_id[unit];

                rv = bcm_l3_route_add(unit, &route_info);
            } else {
                rv = route_perf_v4_lt_op(unit, BCMI_LTSW_RP_OPCODE_ADD, vrf,
                                         0, 0, egr_obj_id[unit] - 100000,
                                         alpm_dmode);
            }

            if (BCM_FAILURE(rv) && (rv != BCM_E_EXISTS)) {
                cli_out("Failed to add IPv4 default route (rv = %d)\n", rv);
                goto exit;
            }
        }

        route_add = 0;
        for (op = 0; op < BCMI_LTSW_RP_OPCODE_NUM; op++) {
            if (!(ops & (1 << op))) {
                continue;
            }
            for (i = 0; i < nsets; i++) {
                rv = route_perf_v4_op_test(unit, op, use_api, i,
                                           rpd[op], vrf, &route_cnt);
                if (op == BCMI_LTSW_RP_OPCODE_ADD) {
                    route_add += route_cnt;
                    if (i == 0) {
                        set_size = route_cnt;
                    }
                }
                if (BCM_FAILURE(rv)) {
                    if (op == BCMI_LTSW_RP_OPCODE_ADD) {
                        nsets = i + 1;
                    }
                    break;
                }
            }
        }

        if (ops & (1 << BCMI_LTSW_RP_OPCODE_DEL)) {
            /* Delete default route */
            if (use_api) {
                rv = bcm_l3_route_delete(unit, &route_info);
            } else {
                rv = route_perf_v4_lt_op(unit, BCMI_LTSW_RP_OPCODE_DEL,
                                         vrf, 0, 0, 0, alpm_dmode);
            }

            if (BCM_FAILURE(rv)) {
                cli_out("Failed to delete IPv4 default route (rv = %d)\n", rv);
                goto exit;
            }
        }

        /* For test purpose, flush heap memory by allocate 16MB and free,
         * in order to trigger garbage defragmentation before next run. */
        pd = sal_alloc(1024*1024*16, "alpm_mem_flush");
        if (pd) {
            sal_free(pd);
            pd = NULL;
        }
    }

    tot_route = route_add + 1; /* include the default route. */
    cli_out("IPv4 (%s) performance test: Added routes = %d\n",
            (vrf == BCM_L3_VRF_GLOBAL ? "Global" :
            (vrf == BCM_L3_VRF_OVERRIDE ? "Override" : "Private")),
            tot_route);
    cli_out(" Set RouteCnt TotRoute  RPS:Add  Update   Lookup   Delete \n");

    tot_route = 0;
    for (i = 0; i < nsets; i++) {
        if ((tot_route + set_size) > route_add) {
            route_cnt = route_add - tot_route;
        } else {
            route_cnt = set_size;
        }
        tot_route += route_cnt;

        cli_out("%3d  %7d  %7d ", i, route_cnt, tot_route);

        for (op = 0; op < BCMI_LTSW_RP_OPCODE_NUM; op++) {
            ptr = rpd[op];
            route_perf_data_print(&ptr[i], route_cnt, "");
        }
        cli_out("\n");
    }
    cli_out("\n");

exit:
    if (data) {
        sal_free(data);;
    }
    return rv;
}

/*!
 * \brief Operate IPv6 route table through LT interfaces.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation code.
 * \param [in] vrf VRF.
 * \param [in] v6 IP address.
 * \param [in] v6_mask IP mask.
 * \param [in] intf Egress object ID.
 * \param [in] alpm_dmode alpm data mode array.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    int rv;
    rv = bcmi_l3_route_perf_v6_lt_op(unit, op, vrf,
                                     v6, v6_mask, intf, alpm_dmode);
    return rv;
}

/*!
 * \brief Operate IPv6 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] op Operation code.
 * \param [in] use_api Use BCM API.
 * \param [in] set_idx Index of set.
 * \param [in] rpd Pointer to performance data.
 * \param [in] vrf Vrf value.
 * \param [out] route_cnt Added route number.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_v6_op_test(int unit, bcmi_ltsw_l3_route_perf_opcode_t op, int use_api,
                      int set_idx, route_perf_data_t *rpd, int vrf, int *route_cnt)
{
    int rv = BCM_E_NONE;
    int start, end, duration = 0;
    bcm_l3_route_t route_info;
    uint64_t ip[2], start_ip[2] = {0x0101010101010101, 0x0101010101010101};
    uint64_t ip_mask[2] = {0xffffffffffffffff, 0xffffffffffffffff};
    int loop = 4096;
    int loop_ip = 1024;
    int i, j, intf, pos;
    uint32 prfx[4] = {118, 119, 120, 116};
    uint64_t mask[4][2] = {{0xfffffffffffffc00, 0xffffffffffffffff},
                           {0xfffffffffffffe00, 0xffffffffffffffff},
                           {0xffffffffffffff00, 0xffffffffffffffff},
                           {0xfffffffffffff000, 0xffffffffffffffff}
                          };
    uint64_t ipv6[4][2] = {{0x0202020202020202, 0x0202020202020202},
                           {0x0202020202020202, 0x2902020202020202},
                           {0x0202020202020202, 0x6102020202020202},
                           {0x0202020202020202, 0x8c02020202020202},
                          };
    int (*op_func[BCMI_LTSW_RP_OPCODE_NUM])(int, bcm_l3_route_t *) = {
        bcm_l3_route_add,
        bcm_l3_route_add,
        bcm_l3_route_get,
        bcm_l3_route_delete
    };

    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_IPV6_LPM_128B_ENABLE, 1)) {
        pos = 1;
        start_ip[0] = 0;
        ip_mask[0] = 0;
        for (i = 0; i < 4; i++) {
            prfx[i] = 64 - (i+1)*2;
            mask[i][1] &= ~((1 << ((i+1)*2)) - 1);
            mask[i][0] = 0;
        }
    } else {
        pos = 0;
    }

    ip[1] = start_ip[1];
    ip[0] = start_ip[0];
    ip[pos] += set_idx * loop;

    *route_cnt = 0;

    bcm_l3_route_t_init(&route_info);

    start = sal_time_usecs();

    for (i = 0; i < loop; i++) {
        if (use_api) {
            route_info.l3a_flags = BCM_L3_IP6;
            route_info.l3a_vrf = vrf;
            bcmi_ltsw_util_uint64_to_ip6(&(route_info.l3a_ip6_net), ip);
            bcmi_ltsw_util_uint64_to_ip6(&(route_info.l3a_ip6_mask), ip_mask);
            route_info.l3a_intf = egr_obj_id[unit];
            if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                route_info.l3a_flags |= BCM_L3_REPLACE;
                route_info.l3a_intf = egr_obj_id2[unit];
            }

            rv = op_func[op](unit, &route_info);
        } else {
            if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                intf = egr_obj_id2[unit] - 100000;/* _SHR_L3_EGRESS_IDX_MIN */
            } else {
                intf = egr_obj_id[unit] - 100000;
            }
            rv = route_perf_v6_lt_op(unit, op, vrf, ip, ip_mask,
                                     intf, alpm_dmode);
        }
        if (BCM_FAILURE(rv)) {
            end = sal_time_usecs();
            duration = end - start;
            goto exit;
        } else {
            *route_cnt += 1;
        }

        ip[pos]++;
    }

    end = sal_time_usecs();
    duration = end - start;

    for (j = 0; j < 4; j++) {
        start = sal_time_usecs();
        for (i = 0; i < loop_ip; i++) {
            ip[1] = ipv6[j][1];
            ip_mask[1] = mask[j][1];
            ip[0] = ipv6[j][0];
            ip_mask[0] = mask[j][0];

            ip[pos] += ((loop_ip + i + loop_ip * set_idx) << (128 - prfx[j]));

            if (use_api) {
                route_info.l3a_flags = BCM_L3_IP6;
                route_info.l3a_vrf = vrf;
                bcmi_ltsw_util_uint64_to_ip6(&(route_info.l3a_ip6_net), ip);
                bcmi_ltsw_util_uint64_to_ip6(&(route_info.l3a_ip6_mask), ip_mask);
                route_info.l3a_intf = egr_obj_id[unit];
                if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                    route_info.l3a_flags |= BCM_L3_REPLACE;
                    route_info.l3a_intf = egr_obj_id2[unit];
                }

                rv = op_func[op](unit, &route_info);
            } else {
                if (op == BCMI_LTSW_RP_OPCODE_UPD) {
                    intf = egr_obj_id2[unit] - 100000;
                } else {
                    intf = egr_obj_id[unit] - 100000;
                }
                rv = route_perf_v6_lt_op(unit, op, vrf, ip, ip_mask,
                                         intf, alpm_dmode);
            }
            if (BCM_FAILURE(rv)) {
                end = sal_time_usecs();
                duration += (end - start);
                goto exit;
            } else {
                *route_cnt += 1;
            }
        }
        end = sal_time_usecs();
        duration += (end - start);
    }

exit:
    route_perf_data_add(&rpd[set_idx], duration);
    return rv;
}


/*!
 * \brief Performance test on IPv6 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] ops Bitmap of operation code.
 * \param [in] loops Number of runs.
 * \param [in] nsets Number of sets.
 * \param [in] use_api Use BCM API.
 * \param [in] vrf Vrf value.
 *
 * \retval BCM_E_NONE No error.
 */
static int
route_perf_v6_test(int unit, int ops, int loops, int nsets, int use_api, int vrf)
{
    int run, i, sz, op;
    int route_add = 0, set_size = 0, route_cnt = 0, tot_route = 0;
    route_perf_data_t *rpd[BCMI_LTSW_RP_OPCODE_NUM], *ptr;
    char *data = NULL, *pd = NULL;
    int rv = BCM_E_NONE;
    bcm_l3_route_t route_info;
    uint64_t v6_zero[2] = {0, 0};

    sz = sizeof(route_perf_data_t) * nsets;

    data = sal_alloc((sz * BCMI_LTSW_RP_OPCODE_NUM), "bcmRoutePerfData");
    if (!data) {
        return BCM_E_MEMORY;
    }
    sal_memset(data, 0, (sz * BCMI_LTSW_RP_OPCODE_NUM));

    for (i = 0; i < BCMI_LTSW_RP_OPCODE_NUM; i++) {
        rpd[i] = (route_perf_data_t *)(data + i * sz);
    }

    for (run = 0; run < loops; run++) {
        if (ops & (1 << BCMI_LTSW_RP_OPCODE_ADD)) {
            /* Add default route */
            if (use_api) {
                bcm_l3_route_t_init(&route_info);

                route_info.l3a_flags = BCM_L3_IP6;
                route_info.l3a_vrf = vrf;
                route_info.l3a_intf = egr_obj_id[unit];

                rv = bcm_l3_route_add(unit, &route_info);
            } else {
                rv = route_perf_v6_lt_op(unit, BCMI_LTSW_RP_OPCODE_ADD, vrf,
                                         v6_zero, v6_zero, egr_obj_id[unit] - 100000,
                                         alpm_dmode);
            }

            if (BCM_FAILURE(rv)) {
                cli_out("Failed to add IPv6 default route (rv = %d)\n", rv);
                goto exit;
            }
        }

        route_add = 0;
        for (op = 0; op < BCMI_LTSW_RP_OPCODE_NUM; op++) {
            if (!(ops & (1 << op))) {
                continue;
            }
            for (i = 0; i < nsets; i++) {
                rv = route_perf_v6_op_test(unit, op, use_api, i,
                                           rpd[op], vrf, &route_cnt);
                if (op == BCMI_LTSW_RP_OPCODE_ADD) {
                    route_add += route_cnt;
                    if (i == 0) {
                        set_size = route_cnt;
                    }
                }
                if (BCM_FAILURE(rv)) {
                    if (op == BCMI_LTSW_RP_OPCODE_ADD) {
                        nsets = i + 1;
                    }
                    break;
                }
            }
        }

        if (ops & (1 << BCMI_LTSW_RP_OPCODE_DEL)) {
            /* Delete default route */
            if (use_api) {
                rv = bcm_l3_route_delete(unit, &route_info);
            } else {
                rv = route_perf_v6_lt_op(unit, BCMI_LTSW_RP_OPCODE_DEL,
                                         vrf, v6_zero, v6_zero, 0, alpm_dmode);
            }

            if (BCM_FAILURE(rv)) {
                cli_out("Failed to delete IPv6 default route (rv = %d)\n", rv);
                goto exit;
            }
        }

        /* For test purpose, flush heap memory by allocate 16MB and free,
         * in order to trigger garbage defragmentation before next run. */
        pd = sal_alloc(1024*1024*16, "alpm_mem_flush");
        if (pd) {
            sal_free(pd);
            pd = NULL;
        }
    }

    tot_route = route_add + 1; /* include the default route. */
    cli_out("IPv6 (%s) performance test: Added routes = %d\n",
            (vrf == BCM_L3_VRF_GLOBAL ? "Global" :
            (vrf == BCM_L3_VRF_OVERRIDE ? "Override" : "Private")),
            tot_route);
    cli_out(" Set RouteCnt TotRoute  RPS:Add  Update   Lookup   Delete \n");

    tot_route = 0;
    for (i = 0; i < nsets; i++) {
        if ((tot_route + set_size) > route_add) {
            route_cnt = route_add - tot_route;
        } else {
            route_cnt = set_size;
        }
        tot_route += route_cnt;

        cli_out("%3d  %7d  %7d ", i, route_cnt, tot_route);

        for (op = 0; op < BCMI_LTSW_RP_OPCODE_NUM; op++) {
            ptr = rpd[op];
            route_perf_data_print(&ptr[i], route_cnt, "");
        }
        cli_out("\n");
    }
    cli_out("\n");

exit:
    if (data) {
        sal_free(data);;
    }
    return rv;
}

/*!
 * \brief Route performance test.
 *
 * \param [in] unit Unit number.
 * \param [in] ip_type IP type.
 * \param [in] ops Bitmap of operation code.
 * \param [in] loops Number of runs.
 * \param [in] nsets Number of sets.
 * \param [in] use_api Use BCM API.
 * \param [in] vrf_bmp Bitmap of VRF.
 *
 * \retval BCM_E_NONE No error.
 */
static int
ltsw_l3_route_perf(int unit, int ip_type, int ops, int loops, int nsets,
                   int use_api, int vrf_bmp)
{
    int nonset, rv = BCM_E_NONE;
    int vrf_arr[] = {ROUTE_PERF_VRF,
                     BCM_L3_VRF_GLOBAL,
                     BCM_L3_VRF_OVERRIDE};
    int i = 0;

    /* OP_ADD is must */
    if ((ops & (1 << BCMI_LTSW_RP_OPCODE_ADD)) == 0) {
        return SHR_E_PARAM;
    }

    /* Force loops=1 if not OP_DEL */
    if ((ops & (1 << BCMI_LTSW_RP_OPCODE_DEL)) == 0) {
        loops = 1;
    }

    rv = route_perf_test_setup(unit);
    if (BCM_FAILURE(rv)) {
        goto exit;
    }

    nonset = (nsets == -1);

    if ((ip_type == 1) || (ip_type == 3)) { /* IPv4 first */
        if (nonset) {
            nsets = 130;
        }

        for (i = 0; i < COUNTOF(vrf_arr); i ++) {
            if (!(vrf_bmp & (1 << i))) {
                continue;
            }
            rv = route_perf_v4_test(unit, ops, loops, nsets, use_api, vrf_arr[i]);
            if (BCM_FAILURE(rv) && rv != BCM_E_RESOURCE) {
                goto exit;
            }
        }
    }

    if (ip_type != 1) { /* IPv6 */
        if (nonset) {
            nsets = 300;
        }

        for (i = 0; i < COUNTOF(vrf_arr); i ++) {
            if (!(vrf_bmp & (1 << i))) {
                continue;
            }
            rv = route_perf_v6_test(unit, ops, loops, nsets, use_api, vrf_arr[i]);
            if (BCM_FAILURE(rv) && rv != BCM_E_RESOURCE) {
                goto exit;
            }
        }
    }

    if (ip_type == 4) { /* then IPv4 */
        if (nonset) {
            nsets = 130;
        }

        for (i = 0; i < COUNTOF(vrf_arr); i ++) {
            if (!(vrf_bmp & (1 << i))) {
                continue;
            }
            rv = route_perf_v4_test(unit, ops, loops, nsets, use_api, vrf_arr[i]);
            if (BCM_FAILURE(rv) && rv != BCM_E_RESOURCE) {
                goto exit;
            }
        }
    }

exit:
    route_perf_test_clean(unit);

    return BCM_E_NONE;
}

/*!
 * \brief Route performance test.
 *
 * \param [in] unit           Unit number.
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route_perf(int unit, args_t *arg)
{
    int ip_type, loops, nsets, op_bitmap, vrf_bmp, use_api, rv;
    cmd_result_t cmd_rv = CMD_OK;
    parse_table_t pt;
    int alpm_temp, alpm_mode;

    ip_type = 1;
    loops = 3;
    op_bitmap = 0xf;
    use_api = 1;
    nsets = -1;

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);
    if ((alpm_temp == 1) || (alpm_temp == 3) || (alpm_temp == 5) ||
        (alpm_temp == 7) || (alpm_temp == 9) || (alpm_temp == 11)) {
        /* Combined. */
        alpm_mode = 0;
        vrf_bmp = 0x1;
    } else if ((alpm_temp == 2) || (alpm_temp == 4) || (alpm_temp == 6) ||
               (alpm_temp == 10)) {
        /*
         * In Parallel mode, the database (both TCAM and SRAM) is partitioned
         * up front into global and VRF specific regions. Run perf test on both
         * global and VRF by default.
         */
        alpm_mode = 1;
        vrf_bmp = 0x3;
    } else {
        cli_out("Invalide ALPM template: %d\n", alpm_temp);
        return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "v4v6", PQ_DFL | PQ_INT, 0, (void *)&ip_type, 0);
    parse_table_add(&pt, "loops", PQ_DFL | PQ_INT, 0, (void *)&loops, 0);
    parse_table_add(&pt, "nsets", PQ_DFL | PQ_INT, 0, (void *)&nsets, 0);
    parse_table_add(&pt, "UseApi", PQ_DFL | PQ_INT, 0, (void *)&use_api, 0);
    parse_table_add(&pt, "OPs", PQ_DFL | PQ_INT, 0, (void *)&op_bitmap, 0);
    parse_table_add(&pt, "VRFs", PQ_DFL | PQ_INT, 0, (void *)&vrf_bmp, 0);

    if (!parseEndOk(arg, &pt, &cmd_rv)) {
        return cmd_rv;
    }

    cli_out("Test runs: %d\n", loops);
    cli_out("ALPM mode %s\n",
            alpm_mode == 1 ? "Parallel" : "Combined");

    rv = ltsw_l3_route_perf(unit, ip_type, op_bitmap, loops, nsets, use_api, vrf_bmp);
    if (BCM_FAILURE(rv)) {
        cli_out("L3 Route performance test failed: %s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*!
 * \brief L3 route cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_l3_route(int unit, args_t *arg)
{
    int rv;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "add")) {
        if ((rv = cmd_ltsw_l3_route_add(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "delete")) {
        if ((rv = cmd_ltsw_l3_route_delete(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "get")) {
        if ((rv = cmd_ltsw_l3_route_get(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "clear")) {
        if ((rv = cmd_ltsw_l3_route_clear(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "show")) {
        if ((rv = cmd_ltsw_l3_route_show(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else if (!sal_strcasecmp(subcmd, "perf")) {
        if ((rv = cmd_ltsw_l3_route_perf(unit, arg)) < 0) {
            return CMD_FAIL;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_egress_print
 * Description:
 *      Internal function to print out nat egress info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      info      - (IN) Pointer to bcm_l3_nat_egress_t data structure.
 */
static void
cmd_ltsw_l3_nat_egress_print(int unit, bcm_l3_nat_egress_t *info)
{
    char sip_str[IP6ADDR_STR_LEN];
    char dip_str[IP6ADDR_STR_LEN];

    format_ipaddr(sip_str,info->sip_addr);
    format_ipaddr(dip_str,info->dip_addr);
    cli_out("%-5d %-7d %-7d %-16s %-16s %-6d %-6d\n",
            info->flags, info->snat_id, info->dnat_id, sip_str, dip_str,
            info->src_port, info->dst_port);
}

/*
 * Function:
 *    nat_egress_traverse_cb
 * Description:
 *    Internal function to print out nat egress info.
 * Parameters:
 *    unit      - (IN) Device number.
 *    index     - (IN) Egress NAT index.
 *    nat_info  - (IN) Egress NAT structure.
 *    data      - (IN) Egress NAT user data.
 */
static int
nat_egress_traverse_cb(int unit, int index,
                       bcm_l3_nat_egress_t *nat_info, void *data)
{
    cmd_ltsw_l3_nat_egress_print(unit, nat_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_egress_add
 * Description:
 *      Service routine used to create a nat egress entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_egress_add(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    int napt = 0;
    int snat = 0;
    int dnat = 0;
    int snat_id = 0;
    int dnat_id = 0;
    bcm_ip_t sip_addr = 0;
    bcm_ip_t dip_addr = 0;
    int src_port = 0;
    int dst_port = 0;
    bcm_l3_nat_egress_t nat_egress;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
    parse_table_add(&pt, "SNAT", PQ_DFL|PQ_INT, 0, (void*)&snat, 0);
    parse_table_add(&pt, "DNAT", PQ_DFL|PQ_INT, 0, (void*)&dnat, 0);
    parse_table_add(&pt, "SNAT_ID", PQ_DFL|PQ_INT, 0, (void*)&snat_id, 0);
    parse_table_add(&pt, "DNAT_ID", PQ_DFL|PQ_INT, 0, (void*)&dnat_id, 0);
    parse_table_add(&pt, "DIP",  PQ_DFL | PQ_IP, 0, (void *)&dip_addr, 0);
    parse_table_add(&pt, "SIP",  PQ_DFL | PQ_IP, 0, (void *)&sip_addr, 0);
    parse_table_add(&pt, "SRCPORT", PQ_DFL | PQ_INT, 0, (void *)&src_port, 0);
    parse_table_add(&pt, "DSTPORT", PQ_DFL | PQ_INT, 0, (void *)&dst_port, 0);
    if (!parseEndOk(arg, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_nat_egress_t_init(&nat_egress);

    nat_egress.flags |= BCM_L3_NAT_EGRESS_WITH_ID;
    if (napt) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_NAPT;
    }
    if (snat) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_SNAT;
    }
    if (dnat) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_DNAT;
    }

    nat_egress.snat_id = snat_id;
    nat_egress.dnat_id = dnat_id;
    nat_egress.sip_addr = sip_addr;
    nat_egress.src_port = src_port;
    nat_egress.dip_addr = dip_addr;
    nat_egress.dst_port = dst_port;


    if ((rv = bcm_l3_nat_egress_add(unit, &nat_egress)) < 0) {
        cli_out("ERROR %s: creating nat egress %s.\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("New NAT egress: snat_id:%d, dnat_id:%d.\n",
            nat_egress.snat_id, nat_egress.dnat_id);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_egress_get
 * Description:
 *      Service routine used to get & show l3 nat egress info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_egress_get(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    int napt = 0;
    int snat = 0;
    int dnat = 0;
    int snat_id = 0;
    int dnat_id = 0;
    bcm_l3_nat_egress_t nat_egress;
    int rv;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
    parse_table_add(&pt, "SNAT", PQ_DFL|PQ_INT, 0, (void*)&snat, 0);
    parse_table_add(&pt, "DNAT", PQ_DFL|PQ_INT, 0, (void*)&dnat, 0);
    parse_table_add(&pt, "SNAT_ID", PQ_DFL|PQ_INT, 0, (void*)&snat_id, 0);
    parse_table_add(&pt, "DNAT_ID", PQ_DFL|PQ_INT, 0, (void*)&dnat_id, 0);
    if (!parseEndOk(arg, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_nat_egress_t_init(&nat_egress);

    if (napt) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_NAPT;
    }
    if (snat) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_SNAT;
    }
    if (dnat) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_DNAT;
    }

    nat_egress.snat_id = snat_id;
    nat_egress.dnat_id = dnat_id;

    if ((rv = bcm_l3_nat_egress_get(unit, &nat_egress)) < 0) {
        cli_out("ERROR %s: getting nat egress snat_id:%d, dnat_id:%d %s\n",
                ARG_CMD(arg), nat_egress.snat_id, nat_egress.dnat_id,
                bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("%-7s %-7s %-7s %-16s %-16s %-6s %-7s\n",
            "FLAGS", "SNAT_ID", "DNAT_ID", "SIP", "DIP", "SPORT", "DPORT");
    cmd_ltsw_l3_nat_egress_print(unit, &nat_egress);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_egress_destroy
 * Description:
 *      Service routine used to destroy nat egress entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      a         - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_egress_destroy(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    int napt = 0;
    int snat = 0;
    int dnat = 0;
    int snat_id = 0;
    int dnat_id = 0;
    bcm_l3_nat_egress_t nat_egress;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
    parse_table_add(&pt, "SNAT", PQ_DFL|PQ_INT, 0, (void*)&snat, 0);
    parse_table_add(&pt, "DNAT", PQ_DFL|PQ_INT, 0, (void*)&dnat, 0);
    parse_table_add(&pt, "SNAT_ID", PQ_DFL|PQ_INT, 0, (void*)&snat_id, 0);
    parse_table_add(&pt, "DNAT_ID", PQ_DFL|PQ_INT, 0, (void*)&dnat_id, 0);
    if (!parseEndOk(arg, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_nat_egress_t_init(&nat_egress);

    if (napt) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_NAPT;
    }
    if (snat) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_SNAT;
    }
    if (dnat) {
        nat_egress.flags |= BCM_L3_NAT_EGRESS_DNAT;
    }

    nat_egress.snat_id = snat_id;
    nat_egress.dnat_id = dnat_id;

    if ((rv = bcm_l3_nat_egress_destroy(unit, &nat_egress)) < 0) {
        cli_out("ERROR %s: destroying nat egress snat_id:%d, dnat_id:%d %s\n",
                ARG_CMD(arg), nat_egress.snat_id, nat_egress.dnat_id,
                bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_egress_show
 * Description:
 *     Service routine used to show nat egress info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static int
cmd_ltsw_l3_nat_egress_show(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    uint32 flags = 0;
    int napt = 0;
    int snat = 0;
    int dnat = 0;
    int rv;

    if (ARG_CNT(arg)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
        parse_table_add(&pt, "SNAT", PQ_DFL|PQ_INT, 0, (void*)&snat, 0);
        parse_table_add(&pt, "DNAT", PQ_DFL|PQ_INT, 0, (void*)&dnat, 0);
        if (!parseEndOk(arg, &pt, &retCode)) {
            return retCode;
        }
    }

    if (napt) {
        flags |= BCM_L3_NAT_EGRESS_NAPT;
    }
    if (snat) {
        flags |= BCM_L3_NAT_EGRESS_SNAT;
    }
    if (dnat) {
        flags |= BCM_L3_NAT_EGRESS_DNAT;
    }

    cli_out("%-5s %-7s %-7s %-16s %-16s %-6s %-7s\n",
            "FLAGS", "SNAT_ID", "DNAT_ID", "SIP", "DIP", "SPORT", "DPORT");

    rv = bcm_l3_nat_egress_traverse(unit, flags, 0, 0,
                                    nat_egress_traverse_cb, NULL);
    if(rv < 0) {
        cli_out("Failed in egress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_egress
 * Description:
 *      Service routine used to manipulate l3 nat egress entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_egress(int unit, args_t *arg)
{
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing l3_nat_egress subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        return cmd_ltsw_l3_nat_egress_add(unit, arg);
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        return cmd_ltsw_l3_nat_egress_get(unit, arg);
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        return cmd_ltsw_l3_nat_egress_destroy(unit, arg);
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        return cmd_ltsw_l3_nat_egress_show(unit, arg);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_ingress_print
 * Description:
 *      Internal function to print out nat ingress info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      info      - (IN) Pointer to bcm_l3_nat_ingress_t data structure.
 */
static void
cmd_ltsw_l3_nat_ingress_print(int unit, bcm_l3_nat_ingress_t *info)
{
    char ip_str[IP6ADDR_STR_LEN];

    format_ipaddr(ip_str,info->ip_addr);
    cli_out("%-5d %-16s %-5d %-6d %-5d %-5d %-7d %-8d\n",
            info->flags, ip_str, info->vrf, info->nat_id,
            info->ip_proto, info->l4_port, info->nexthop, info->class_id);
}

/*
 * Function:
 *    nat_ingress_traverse_cb
 * Description:
 *    Internal function to print out nat ingress info.
 * Parameters:
 *    unit     - (IN) Device number.
 *    index    - (IN) Ingress NAT index.
 *    nat_info - (IN) Ingress NAT structure.
 *    data     - (IN) Ingress NAT user data.
 */
static int
nat_ingress_traverse_cb(int unit, int index,
                        bcm_l3_nat_ingress_t *nat_info, void *data)
{
    cmd_ltsw_l3_nat_ingress_print(unit, nat_info);
    return BCM_E_NONE;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_ingress_add
 * Description:
 *      Service routine used to create a nat ingress entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_ingress_add(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    int napt = 0;
    int dst_discard = 0;
    bcm_ip_t ip_addr = 0;
    int vrf = 0;
    int proto = 0;
    int l4_port = 0;
    int nat_id = 0;
    int nexthop = 0;
    int realm_id = 0;
    int class_id = 0;
    bcm_l3_nat_ingress_t nat_ingress;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
    parse_table_add(&pt, "DST_DISCARD", PQ_DFL|PQ_INT, 0,(void*)&dst_discard, 0);
    parse_table_add(&pt, "IP", PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0, (void*)&vrf, 0);
    parse_table_add(&pt, "PROTO", PQ_DFL | PQ_INT, 0, (void *)&proto, 0);
    parse_table_add(&pt, "L4_PORT", PQ_DFL | PQ_INT, 0, (void *)&l4_port, 0);
    parse_table_add(&pt, "NAT_ID", PQ_DFL|PQ_INT, 0, (void*)&nat_id, 0);
    parse_table_add(&pt, "NEXTHOP", PQ_DFL|PQ_INT, 0, (void*)&nexthop, 0);
    parse_table_add(&pt, "REALM_ID", PQ_DFL|PQ_INT, 0, (void*)&realm_id, 0);
    parse_table_add(&pt, "CLASS_ID", PQ_DFL|PQ_INT, 0, (void*)&class_id, 0);
    if (!parseEndOk(arg, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_nat_ingress_t_init(&nat_ingress);

    if (napt) {
        nat_ingress.flags |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
    }
    if (dst_discard) {
        nat_ingress.flags |= BCM_L3_NAT_INGRESS_DST_DISCARD;
    }

    nat_ingress.ip_addr  = ip_addr;
    nat_ingress.vrf      = vrf;
    nat_ingress.ip_proto = proto;
    nat_ingress.l4_port  = l4_port;
    nat_ingress.nat_id   = nat_id;
    nat_ingress.nexthop  = nexthop;
    nat_ingress.realm_id = realm_id;
    nat_ingress.class_id = class_id;

    if ((rv = bcm_l3_nat_ingress_add(unit, &nat_ingress)) < 0) {
        cli_out("ERROR %s: creating nat ingress %s.\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_ingress_get
 * Description:
 *      Service routine used to get & show l3 nat ingress info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_ingress_get(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    int napt = 0;
    bcm_ip_t ip_addr = 0;
    int vrf = 0;
    int proto = 0;
    int l4_port = 0;
    bcm_l3_nat_ingress_t nat_ingress;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
    parse_table_add(&pt, "IP",  PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0, (void*)&vrf, 0);
    parse_table_add(&pt, "PROTO", PQ_DFL | PQ_INT, 0, (void *)&proto, 0);
    parse_table_add(&pt, "L4_PORT", PQ_DFL | PQ_INT, 0, (void *)&l4_port, 0);
    if (!parseEndOk(arg, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_nat_ingress_t_init(&nat_ingress);

    if (napt) {
        nat_ingress.flags |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
    }

    nat_ingress.ip_addr  = ip_addr;
    nat_ingress.vrf      = vrf;
    nat_ingress.ip_proto = proto;
    nat_ingress.l4_port  = l4_port;

    if ((rv = bcm_l3_nat_ingress_find(unit, &nat_ingress)) < 0) {
        cli_out("ERROR %s: getting nat ingress %s.\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    cli_out("%-5s %-16s %-5s %-6s %-5s %-7s %-7s %-8s\n",
            "FLAGS", "IP", "VRF", "NAT_ID",
            "PROTO", "L4_PORT", "NEXTHOP", "CLASS_ID");

    cmd_ltsw_l3_nat_ingress_print(unit, &nat_ingress);

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_ingress_delete
 * Description:
 *      Service routine used to delete nat ingress entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_ingress_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    int napt = 0;
    bcm_ip_t ip_addr = 0;
    int vrf = 0;
    int proto = 0;
    int l4_port = 0;
    bcm_l3_nat_ingress_t nat_ingress;
    int rv;

    if (ARG_CNT(arg) == 0) {
        return CMD_USAGE;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
    parse_table_add(&pt, "IP",  PQ_DFL | PQ_IP, 0, (void *)&ip_addr, 0);
    parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0, (void*)&vrf, 0);
    parse_table_add(&pt, "PROTO", PQ_DFL | PQ_INT, 0, (void *)&proto, 0);
    parse_table_add(&pt, "L4_PORT", PQ_DFL | PQ_INT, 0, (void *)&l4_port, 0);
    if (!parseEndOk(arg, &pt, &retCode)) {
        return retCode;
    }

    /* Initialize data structures. */
    bcm_l3_nat_ingress_t_init(&nat_ingress);

    if (napt) {
        nat_ingress.flags |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
    }

    nat_ingress.ip_addr  = ip_addr;
    nat_ingress.vrf      = vrf;
    nat_ingress.ip_proto = proto;
    nat_ingress.l4_port  = l4_port;

    if ((rv = bcm_l3_nat_ingress_delete(unit, &nat_ingress)) < 0) {
        cli_out("ERROR %s: destroying nat ingress %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_ingress_show
 * Description:
 *     Service routine used to show nat ingress info.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static int
cmd_ltsw_l3_nat_ingress_show(int unit, args_t *arg)
{
    parse_table_t pt;
    cmd_result_t retCode;
    uint32 flags = 0;
    int napt = 0;
    int dst_discard = 0;
    int rv;

    if (ARG_CNT(arg)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "NAPT", PQ_DFL|PQ_INT, 0, (void *)&napt, 0);
        parse_table_add(&pt, "DST_DISCARD", PQ_DFL|PQ_INT, 0,
                        (void*)&dst_discard, 0);
        if (!parseEndOk(arg, &pt, &retCode)) {
            return retCode;
        }
    }

    if (napt) {
        flags |= BCM_L3_NAT_INGRESS_TYPE_NAPT;
    }
    if (dst_discard) {
        flags |= BCM_L3_NAT_INGRESS_DST_DISCARD;
    }

    cli_out("%-5s %-16s %-5s %-6s %-5s %-7s %-7s %-8s\n",
            "FLAGS", "IP", "VRF", "NAT_ID",
            "PROTO", "L4_PORT", "NEXTHOP", "CLASS_ID");

    rv = bcm_l3_nat_ingress_traverse(unit, flags, 0, 0,
                                     nat_ingress_traverse_cb, NULL);
    if(rv < 0) {
        cli_out("Failed in egress_traverse: %s\n", bcm_errmsg(rv));
        return rv;
    }

    return CMD_OK;
}

/*
 * Function:
 *      cmd_ltsw_l3_nat_ingress
 * Description:
 *      Service routine used to manipulate l3 nat ingress entry.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_l3_nat_ingress(int unit, args_t *arg)
{
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing l3_nat_ingress subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        return cmd_ltsw_l3_nat_ingress_add(unit, arg);
    } else if (sal_strcasecmp(subcmd, "get") == 0) {
        return cmd_ltsw_l3_nat_ingress_get(unit, arg);
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        return cmd_ltsw_l3_nat_ingress_delete(unit, arg);
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        return cmd_ltsw_l3_nat_ingress_show(unit, arg);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*
 * Function:
 *   cmd_ipmc_entry_print
 * Description:
 *   Internal function to print out ipmc entry info.
 * Parameters:
 *      unit   - (IN) - device number.
 *      info   - (IN) - Pointer to bcm_ipmc_addr_t data structure.
 *      cookie - (IN) - user cookie
 */
static int
cmd_ipmc_entry_print(int unit, bcm_ipmc_addr_t *info, void *cookie)
{
    char s_ip_str[IP6ADDR_STR_LEN];
    char mc_ip_str[IP6ADDR_STR_LEN];
    int masklen;

    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    /* Print IPMC entry. */
    if (info->flags & BCM_IPMC_IP6) {
        format_ip6addr(s_ip_str, info->s_ip6_addr);
        format_ip6addr(mc_ip_str, info->mc_ip6_addr);
        masklen = bcm_ip6_mask_length(info->mc_ip6_mask);
        cli_out("SRC IP ADDRESS : %s\n", s_ip_str);
        cli_out("MC  IP ADDRESS : %s\n", mc_ip_str);
        cli_out("MC IP MASK LEN : %d\n", masklen);
        cli_out("VLAN           : %d\n", info->vid);
        cli_out("VRF            : %d\n", info->vrf);
        cli_out("GROUP          : 0x%x\n", info->group);
        cli_out("GROUP_L2       : 0x%x\n", info->group_l2);
        cli_out("CLASSS         : %d\n", info->lookup_class);
        cli_out("HIT            : %s\n",
                (info->flags & BCM_IPMC_HIT) ? "Y" :"N");
        cli_out("Ingress L3_IIF : %d\n", info->ing_intf);
        cli_out("Expected L3_IIF: %d\n", info->l3a_intf);
    } else {
        format_ipaddr(s_ip_str, info->s_ip_addr);
        format_ipaddr(mc_ip_str, info->mc_ip_addr);
        masklen = bcm_ip_mask_length(info->mc_ip_mask);
        cli_out("SRC IP ADDRESS : %s\n", s_ip_str);
        cli_out("MC  IP ADDRESS : %s\n", mc_ip_str);
        cli_out("MC IP MASK LEN : %d\n", masklen);
        cli_out("VLAN           : %d\n", info->vid);
        cli_out("VRF            : %d\n", info->vrf);
        cli_out("GROUP          : 0x%x\n", info->group);
        cli_out("GROUP_L2       : 0x%x\n", info->group_l2);
        cli_out("CLASSS         : %d\n", info->lookup_class);
        cli_out("HIT            : %s\n",
                (info->flags & BCM_IPMC_HIT) ? "Y" :"N");
        cli_out("Ingress L3_IIF : %d\n", info->ing_intf);
        cli_out("Expected L3_IIF: %d\n", info->l3a_intf);
    }
    return (BCM_E_NONE);
}

/*!
 * \brief Print ALPM usage.
 *
 * \param [in] unit Unit number.
 * \param [in] res ALPM resources.
 *
 * \retval None.
 */
static void
alpm_res_usage_print(int unit, bcm_l3_alpm_resource_t *res)
{
    int grp, i, tot, usage[BCM_L3_ALPM_LEVELS] = {0};

    cli_out("- Current ALPM route count & resource usage\n");
    cli_out("\t\t Routes L1_usage L2_usage L3_usage L1_used:total L2_used:total L3_used:total\n");
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {
        /* calculate level's usage = cnt_used / cnt_total */
        for (i = 0; i < BCM_L3_ALPM_LEVELS; i++) {
            tot = res[grp].lvl_usage[i].cnt_total;
            if (tot) {
                /* usage in 10000% */
                usage[i] = (res[grp].lvl_usage[i].cnt_used * 10000L) / tot;
            }
        }

        cli_out("%s:%7d  %3d.%02d  %3d.%02d  %3d.%02d %6d:%6d  %6d:%6d  %6d:%6d\n",
                alpm_route_grp_str[grp],
                res[grp].route_cnt,
                usage[0]/100, usage[0]%100,
                usage[1]/100, usage[1]%100,
                usage[2]/100, usage[2]%100,
                res[grp].lvl_usage[0].cnt_used, res[grp].lvl_usage[0].cnt_total,
                res[grp].lvl_usage[1].cnt_used, res[grp].lvl_usage[1].cnt_total,
                res[grp].lvl_usage[2].cnt_used, res[grp].lvl_usage[2].cnt_total);
    }
}

/*!
 * \brief Show ALPM usage.
 *
 * \param [in] unit Unit number.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_alpm_res_usage_show(int unit)
{
    bcm_l3_alpm_resource_t res[bcmL3RouteGroupCounter];
    int grp, rv;

    sal_memset(res, 0, sizeof(bcm_l3_alpm_resource_t) * bcmL3RouteGroupCounter);

    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {
        rv = bcm_l3_alpm_resource_get(unit, grp, &res[grp]);
        if (BCM_FAILURE(rv)) {
            cli_out("Error showing ALPM usage on route group %d: %s\n",
                    grp, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    alpm_res_usage_print(unit, res);

    return CMD_OK;
}

/*!
 * \brief ALPM resource usage cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_alpm_res_usage(int unit, args_t *arg)
{
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_alpm_res_usage_show(unit);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/*!
 * \brief Show ALPM projection.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_alpm_projection_show(int unit, args_t *arg)
{
    char *subcmd;
    int grp, i, rv, proj_route;
    int v4v6_ratio, max_lvl, max_usage, alpm_temp, mix_route;
    bcm_l3_alpm_resource_t res[bcmL3RouteGroupCounter];
    bool alpm_mode_combined;

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);
    if ((alpm_temp == 1) || (alpm_temp == 3) || (alpm_temp == 5) ||
        (alpm_temp == 7) || (alpm_temp == 9) || (alpm_temp == 11)) {
        alpm_mode_combined = true;
    } else if ((alpm_temp == 2) || (alpm_temp == 4) || (alpm_temp == 6) ||
               (alpm_temp == 10)) {
        alpm_mode_combined = false;
    } else {
        cli_out("No resource for ALPM (l3_alpm_template %d).\n", alpm_temp);
        return CMD_FAIL;
    }

    subcmd = ARG_GET(arg);
    if (subcmd != NULL) {
        v4v6_ratio = parse_integer(subcmd);
    } else {
        /* Default to uni-dimensional. */
        v4v6_ratio = 0;
    }

    sal_memset(res, 0, sizeof(bcm_l3_alpm_resource_t) * bcmL3RouteGroupCounter);

    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {
        rv = bcm_l3_alpm_resource_get(unit, grp, &res[grp]);
        if (BCM_FAILURE(rv)) {
            cli_out("Error showing ALPM usage on route group %d: %s\n",
                    grp, bcm_errmsg(rv));
            return CMD_FAIL;
        }
    }

    alpm_res_usage_print(unit, res);

    /* Show route projection based on current route trend */
    cli_out("\n- Route projection based on current route trend\n");
    i = 0;
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {

        if (alpm_mode_combined) {
            if (grp == bcmL3RouteGroupGlobalV4 ||
                grp == bcmL3RouteGroupGlobalV6) {
                continue; /* skip Global grp in Combined mode */
            }
        }

        rv = bcmi_ltsw_l3_route_grp_projection(unit, grp, &proj_route,
                                               &max_lvl, &max_usage);
        if (BCM_FAILURE(rv)) {
            continue; /* projection unavailable */
        }

        cli_out("%s: Prj:%7d (max L%d_usage:%3d%%)\n",
                alpm_mode_combined?
                alpm_route_comb_str[grp] : alpm_route_grp_str[grp],
                proj_route, max_lvl, max_usage);
        i++;
    }
    if (i == 0) {
        cli_out("      <none>\n");
    }

    /* Show route projection based on test DB result interpolation */
    cli_out("\n- Route projection based on test DB result interpolation and configuration\n");
    if (v4v6_ratio) {
        cli_out("  (using mixed V4/V6 ratio: %d%%)\n",v4v6_ratio);
    } else {
        cli_out("  (uni-dimensional projection)\n");
    }
    for (grp = 0; grp < bcmL3RouteGroupCounter; grp++) {

        if (alpm_mode_combined) {
            if (grp == bcmL3RouteGroupGlobalV4 ||
                grp == bcmL3RouteGroupGlobalV6) {
                continue; /* skip Global grp in Combined mode */
            }
        }

        rv = bcmi_ltsw_l3_route_grp_testdb(unit, grp, v4v6_ratio, &proj_route,
                                           &mix_route, &max_lvl, &max_usage);

        if (BCM_FAILURE(rv)) {
            continue; /* default unavailable */
        }

        if (v4v6_ratio) {
            cli_out("%s: Max:%7d Mix:%7d (max L%d_usage:%3d%%)\n",
                    alpm_mode_combined ?
                    alpm_route_comb_str[grp] : alpm_route_grp_str[grp],
                    proj_route, mix_route, max_lvl, max_usage);
        } else {
            cli_out("%s: Max:%7d (max L%d_usage:%3d%%)\n",
                    alpm_mode_combined ?
                    alpm_route_comb_str[grp] : alpm_route_grp_str[grp],
                    proj_route, max_lvl, max_usage);
        }
    }
    cli_out("\n");

    return CMD_OK;
}

/*!
 * \brief ALPM projection cmd handler.
 *
 * \param [in] unit Unit number.
 * \param [in] arg Arg structure for parsing arguments.
 *
 * \retval CMD_OK Command completed successfully.
 * \retval CMD_FAIL Command failed.
 */
static cmd_result_t
cmd_ltsw_alpm_projection(int unit, args_t *arg)
{
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "show")) {
        return cmd_ltsw_alpm_projection_show(unit, arg);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief L3 cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_l3(int unit, args_t *arg)
{
    char *subcmd = NULL;
    int rv;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "init")) {
        if ((rv = bcm_l3_init(unit)) < 0) {
            cli_out("%s: error initializing: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        }
        return CMD_OK;
    } else if (!sal_strcasecmp(subcmd, "detach")) {
        if ((rv = bcm_l3_cleanup(unit)) < 0) {
            cli_out("%s: error detaching: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        }
        return CMD_OK;
    } else if (!sal_strcasecmp(subcmd, "tunnel_init")) {
        return cmd_ltsw_l3_tunnel_init(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "tunnel_term")) {
        return cmd_ltsw_l3_tunnel_term(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "ing_intf")) {
        return cmd_ltsw_l3_ingress(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "intf")) {
        return cmd_ltsw_l3_intf(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "egress")) {
        return cmd_ltsw_l3_egress(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "ecmp")) {
        return cmd_ltsw_l3_ecmp(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "host")) {
        return cmd_ltsw_l3_host(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "route")) {
        return cmd_ltsw_l3_route(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "nat_egress")) {
        return cmd_ltsw_l3_nat_egress(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "nat_ingress")) {
        return cmd_ltsw_l3_nat_ingress(unit, arg);
    } else {
        cli_out("Unsupported command. \n");
        return CMD_OK;
    }

    return CMD_OK;
}

cmd_result_t
cmd_ltsw_ipmc(int unit, args_t *a)
{
    char *table, *subcmd_s, *argstr;
    int subcmd = 0;
    parse_table_t pt;
    cmd_result_t retCode;
    bcm_mac_t mac;
    vlan_id_t vid;
    int vlan = 0;
    int r, i;
    int port = 0, ttl_th = 1, untag = 1;
    char mac_str[SAL_MACADDR_STR_LEN];
    bcm_ipmc_counters_t counters;
    bcm_ip_t s_ip_addr = 0, mc_ip_addr = 0;
    bcm_ipmc_addr_t ipmc_data;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    int valid = 1, cos = 0, ts = 0, nocheck = 0, replace = 0, group = 0, group_l2 = 0;
    int lookup_class = 0;
    int cfg_enable = 1, cfg_check_src_port = 1, cfg_check_src_ip = 1;
    int vrf = 0;
    bcm_ip6_t ip6_addr, sip6_addr;
    int mc_ip_mask_len = 0;
    int mc_ip6_mask_len = 0;
    bcm_if_t l3_iif = BCM_IF_INVALID;
    int hit = 0;

#define IPMC_ADD 1
#define IPMC_DEL 2
#define IPMC_CLEAR 3
#define IPMC_SHOW 4
#define IPMC_SET 5

    sal_memset(mac, 0x10, 6);
    sal_memset(&ipmc_data, 0, sizeof(ipmc_data));
    sal_memset(ip6_addr, 0, sizeof(bcm_ip6_t));
    sal_memset(sip6_addr, 0, sizeof(bcm_ip6_t));

    /* Check valid device to operation on ...*/
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((table = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(table, "init") == 0) {
        if ((r = bcm_ipmc_init(unit)) < 0) {
            cli_out("%s: error initializing: %s\n", ARG_CMD(a), bcm_errmsg(r));
        }
        return CMD_OK;
    } else if (sal_strcasecmp(table, "detach") == 0) {
        if ((r = bcm_ipmc_detach(unit)) < 0) {
            cli_out("%s: error detaching: %s\n", ARG_CMD(a), bcm_errmsg(r));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "config") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Enable", PQ_DFL|PQ_BOOL, 0,
                (void *)&cfg_enable, 0);
        parse_table_add(&pt, "CheckSrcPort", PQ_DFL|PQ_BOOL, 0,
                (void *)&cfg_check_src_port, 0);
        parse_table_add(&pt, "CheckSrcIp", PQ_DFL|PQ_BOOL, 0,
                (void *)&cfg_check_src_ip, 0);
        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Error: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        i = CMD_OK;
        if (pt.pt_entries[0].pq_type & PQ_PARSED) {
            r = bcm_ipmc_enable(unit, cfg_enable);
            if (r < 0) {
                cli_out("%s: Error: Enable failed: %s\n",
                         ARG_CMD(a), bcm_errmsg(r));
                i = CMD_FAIL;
            }
        }
        if (pt.pt_entries[1].pq_type & PQ_PARSED) {
            /* bcm_ipmc_source_port_check is deprecated */
            r = BCM_E_UNAVAIL;
            if (r < 0) {
                cli_out("%s: Error: check_source_port failed: %s\n",
                                ARG_CMD(a), bcm_errmsg(r));
                i = CMD_FAIL;
            }
        }
        if (pt.pt_entries[2].pq_type & PQ_PARSED) {
            /* bcm_ipmc_source_ip_search is deprecated */
            r = BCM_E_UNAVAIL;
            if (r < 0) {
                cli_out("%s: Error: check_source_ip failed: %s\n",
                                ARG_CMD(a), bcm_errmsg(r));
                i = CMD_FAIL;
            }
        }
        parse_arg_eq_done(&pt);
        return (i);
    }

    if ((subcmd_s = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd_s, "add") == 0) {
        subcmd = IPMC_ADD;
    } else if (sal_strcasecmp(subcmd_s, "del") == 0) {
        subcmd = IPMC_DEL;
    } else if (sal_strcasecmp(subcmd_s, "clear") == 0) {
        subcmd = IPMC_CLEAR;
    } else if (sal_strcasecmp(subcmd_s, "show") == 0) {
        subcmd = IPMC_SHOW;
    } else if (sal_strcasecmp(subcmd_s, "set") == 0) {
        subcmd = IPMC_SET;
    } else {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(table, "table") == 0) {
        switch (subcmd) {
        case IPMC_ADD:
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP, 0,
                    (void *)&s_ip_addr, 0);
            parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP, 0,
                    (void *)&mc_ip_addr, 0);
            parse_table_add(&pt, "Mc_IPMaskLen", PQ_DFL|PQ_INT, 0,
                    (void*)&mc_ip_mask_len, 0);
            parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, (void *)&cos, 0);
            parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0,
                    (void *)&vrf, 0);
            parse_table_add(&pt, "Valid", PQ_DFL|PQ_BOOL, 0,
                    (void *)&valid, 0);
            parse_table_add(&pt, "src_Port", PQ_INT, 0, (void *)&port, 0);
            parse_table_add(&pt, "TS", PQ_INT, 0, (void *)&ts, 0);
            parse_table_add(&pt, "NoCHECK", PQ_DFL|PQ_BOOL, 0,
                    (void *)&nocheck, 0);
            parse_table_add(&pt, "Replace", PQ_DFL|PQ_BOOL, 0,
                    (void *)&replace, 0);
            parse_table_add(&pt, "LookupClass", PQ_DFL|PQ_INT, 0,
                                (void *)&lookup_class, 0);
            parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0,
                                (void *)&group, 0);
            parse_table_add(&pt, "GroupL2", PQ_DFL|PQ_INT, 0,
                                (void *)&group_l2, 0);
            parse_table_add(&pt, "IngIntf", PQ_DFL|PQ_INT, 0,
                                (void *)&l3_iif, 0);
            parse_table_add(&pt, "Hit", PQ_DFL|PQ_BOOL, 0,
                                (void *)&hit, 0);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Error: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            bcm_ipmc_addr_t_init(&ipmc_data);
            if (pt.pt_entries[3].pq_type & PQ_PARSED) {
                ipmc_data.flags |= BCM_IPMC_SETPRI;
            }
            parse_arg_eq_done(&pt);
            vid = vlan;
            ipmc_data.s_ip_addr = s_ip_addr;
            ipmc_data.mc_ip_addr = mc_ip_addr;
            ipmc_data.mc_ip_mask = bcm_ip_mask_create(mc_ip_mask_len);
            ipmc_data.vid = vid;
            ipmc_data.vrf = vrf;
            ipmc_data.cos = cos;
            ipmc_data.ts = ts;
            ipmc_data.port_tgid = port;
            ipmc_data.v = valid;
            ipmc_data.ing_intf = l3_iif;
            if (nocheck) {
                ipmc_data.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
            }
            if (replace) {
                ipmc_data.flags |= BCM_IPMC_REPLACE;
            }
            if (hit) {
                ipmc_data.flags |= BCM_IPMC_HIT_ENABLE;
            }
            ipmc_data.lookup_class = lookup_class;
            if (group) {
                ipmc_data.group = group;
            }
            if (group_l2) {
                ipmc_data.group_l2 = group_l2;
                ipmc_data.flags |= BCM_IPMC_L2;
            }
            r = bcm_ipmc_add(unit, &ipmc_data);
            if (r < 0) {
                cli_out("%s: Error Add to ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
        case IPMC_DEL:
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP, 0,
                    (void *)&s_ip_addr, 0);
            parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP, 0,
                    (void *)&mc_ip_addr, 0);
            parse_table_add(&pt, "Mc_IPMaskLen", PQ_DFL|PQ_INT, 0,
                    (void*)&mc_ip_mask_len, 0);
            parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "VRF", PQ_DFL|PQ_INT, 0,
                    (void *)&vrf, 0);
            parse_table_add(&pt, "IngIntf", PQ_DFL|PQ_INT, 0,
                    (void *)&l3_iif, 0);
            if (!parseEndOk( a, &pt, &retCode))
            return retCode;
            bcm_ipmc_addr_t_init(&ipmc_data);
            ipmc_data.s_ip_addr = s_ip_addr;
            ipmc_data.mc_ip_addr = mc_ip_addr;
            ipmc_data.mc_ip_mask = bcm_ip_mask_create(mc_ip_mask_len);
            ipmc_data.vid = vlan;
            ipmc_data.vrf = vrf;
            ipmc_data.ing_intf = l3_iif;
            r = bcm_ipmc_remove(unit, &ipmc_data);
            if (r < 0) {
                cli_out("%s: Error delete from ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
        case IPMC_CLEAR:
            r = bcm_ipmc_remove_all(unit);
            if (r < 0) {
                cli_out("%s: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            return (CMD_OK);
        case IPMC_SHOW:
            if (ARG_CNT(a)) {
                r = BCM_E_UNAVAIL;
                return CMD_OK;
            }
            bcm_ipmc_traverse(unit, 0, cmd_ipmc_entry_print, NULL);
            return (CMD_OK);
        default:
            return CMD_USAGE;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "egr") == 0) {
        switch (subcmd) {
        case IPMC_SET:
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_PORT, 0, (void *)&port, 0);
            parse_table_add(&pt, "MAC", PQ_DFL|PQ_MAC, 0, (void *)mac, 0);
            parse_table_add(&pt, "Vlan", PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "Untag", PQ_DFL|PQ_BOOL, 0,
                            (void *)&untag, 0);
            parse_table_add(&pt, "Ttl_thresh", PQ_DFL|PQ_INT, 0,
                            (void *)&ttl_th, 0);
            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }
            vid = vlan;
            r = bcm_ipmc_egress_port_set(unit, port, mac, untag, vid, ttl_th);
            if (r < 0) {
                cli_out("%s: Egress IP Multicast Configuration registers: %s\n",
                        ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
        case IPMC_SHOW:
            if ((argstr = ARG_GET(a)) == NULL) {
                pbmp = pcfg.e;
            } else {
                if (parse_bcm_port(unit, argstr, &port) < 0) {
                    cli_out("%s: Invalid port string: %s\n",
                            ARG_CMD(a), argstr);
                    return CMD_FAIL;
                }
                if (!BCM_PBMP_MEMBER(pcfg.e, port)) {
                    cli_out("port %d is not a valid Ethernet port\n", port);
                    return CMD_FAIL;
                }
                BCM_PBMP_CLEAR(pbmp);
                BCM_PBMP_PORT_ADD(pbmp, port);
            }
            cli_out("Egress IP Multicast Configuration Register information\n");
            cli_out("Port      Mac Address    Vlan Untag TTL_THRESH\n");
            BCM_PBMP_ITER(pbmp, port) {
                r = bcm_ipmc_egress_port_get(unit, port, mac, &untag, &vid,
                                             &ttl_th);
                if (r == BCM_E_NONE) {
                    format_macaddr(mac_str, mac);
                    cli_out("%-4s  %-18s %4d   %s  %5d\n",
                            BCM_PORT_NAME(unit, port), mac_str,
                            vid, untag ? "Y" : "N", ttl_th);
                }
            }
            return CMD_OK;
        default:
            return CMD_USAGE;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(table, "counter") == 0) {
        if ((argstr  = ARG_GET(a)) == NULL) {
            pbmp = pcfg.e;
        } else {
            if (parse_bcm_port(unit, argstr, &port) < 0) {
                cli_out("%s: Invalid port string: %s\n", ARG_CMD(a),
                        argstr);
                return CMD_FAIL;
            }
            if (!BCM_PBMP_MEMBER(pcfg.e, port)) {
                cli_out("port %d is not a valid Ethernet port\n", port);
                return CMD_FAIL;
            }
            BCM_PBMP_CLEAR(pbmp);
            BCM_PBMP_PORT_ADD(pbmp, port);
        }
        cli_out("PORT      RMCA     TMCA     IMBP     IMRP    RIMDR    TIMDR\n");
        /* coverity[overrun-local] */
        BCM_PBMP_ITER(pbmp, port) {
            r = bcm_ipmc_counters_get(unit, port, &counters);
            if (r == BCM_E_NONE) {
                cli_out("%4d  %8d %8d %8d %8d %8d %8d\n",
                        port,
                        COMPILER_64_LO(counters.rmca),
                        COMPILER_64_LO(counters.tmca),
                        COMPILER_64_LO(counters.imbp),
                        COMPILER_64_LO(counters.imrp),
                        COMPILER_64_LO(counters.rimdr),
                        COMPILER_64_LO(counters.timdr));
                }
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(table, "ip6table") == 0) {
        switch (subcmd) {
        /* ipmc ip6table add */
        case IPMC_ADD:
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&sip6_addr, 0);
            parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&ip6_addr, 0);
            parse_table_add(&pt, "Mc_IPMaskLen", PQ_DFL|PQ_INT, 0,
                    (void*)&mc_ip6_mask_len, 0);
            parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "Valid", PQ_DFL|PQ_BOOL, 0,
                            (void *)&valid, 0);
            parse_table_add(&pt, "COS", PQ_DFL|PQ_INT, 0, (void *)&cos, 0);
            parse_table_add(&pt, "src_Port", PQ_INT, 0, (void *)&port, 0);
            parse_table_add(&pt, "TS", PQ_INT, 0, (void *)&ts, 0);
            parse_table_add(&pt, "NoCHECK", PQ_DFL|PQ_BOOL, 0,
                            (void *)&nocheck, 0);
            parse_table_add(&pt, "LookupClass", PQ_DFL|PQ_INT, 0,
                            (void *)&lookup_class, 0);
            parse_table_add(&pt, "Group", PQ_DFL|PQ_INT, 0,
                            (void *)&group, 0);
            parse_table_add(&pt, "GroupL2", PQ_DFL|PQ_INT, 0,
                            (void *)&group_l2, 0);
            parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
            parse_table_add(&pt, "IngIntf", PQ_DFL|PQ_INT, 0,
                            (void *)&l3_iif, 0);
            parse_table_add(&pt, "Hit", PQ_DFL|PQ_BOOL, 0,
                            (void *)&hit, 0);
            if (parse_arg_eq(a, &pt) < 0) {
                cli_out("%s: Error: Unknown option: %s\n",
                        ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            bcm_ipmc_addr_t_init(&ipmc_data);
            if (pt.pt_entries[4].pq_type & PQ_PARSED) {
                ipmc_data.flags |= BCM_IPMC_SETPRI;
            }
            parse_arg_eq_done(&pt);
            sal_memcpy(ipmc_data.mc_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmc_data.s_ip6_addr, sip6_addr, BCM_IP6_ADDRLEN);
            bcm_ip6_mask_create(ipmc_data.mc_ip6_mask, mc_ip6_mask_len);
            ipmc_data.cos = cos;
            ipmc_data.vid = vlan;
            ipmc_data.flags |= BCM_IPMC_IP6;
            ipmc_data.ts = ts;
            ipmc_data.port_tgid = port;
            ipmc_data.vrf = vrf;
            ipmc_data.v = valid;
            ipmc_data.ing_intf = l3_iif;
            if (nocheck) {
                ipmc_data.flags |= BCM_IPMC_SOURCE_PORT_NOCHECK;
            }
            if (hit) {
                ipmc_data.flags |= BCM_IPMC_HIT_ENABLE;
            }
            ipmc_data.lookup_class = lookup_class;
            if (group) {
                ipmc_data.group = group;
            }
            if (group_l2) {
                ipmc_data.group_l2 = group_l2;
                ipmc_data.flags |= BCM_IPMC_L2;
            }
            r = bcm_ipmc_add(unit, &ipmc_data);
            if (r < 0) {
                cli_out("%s: Error Add to ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
            break;
        case IPMC_DEL:
            /* ipmc ip6table del */
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Src_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&sip6_addr, 0);
            parse_table_add(&pt, "Mc_IP", PQ_DFL|PQ_IP6, 0,
                            (void *)&ip6_addr, 0);
            parse_table_add(&pt, "Mc_IPMaskLen", PQ_DFL|PQ_INT, 0,
                    (void*)&mc_ip6_mask_len, 0);
            parse_table_add(&pt, "VlanID", PQ_DFL|PQ_INT, 0, (void *)&vlan, 0);
            parse_table_add(&pt, "VRF", PQ_DFL | PQ_INT, 0, (void *)&vrf, 0);
            parse_table_add(&pt, "IngIntf", PQ_DFL|PQ_INT, 0,
                            (void *)&l3_iif, 0);
            if (!parseEndOk( a, &pt, &retCode)) {
                return retCode;
            }
            bcm_ipmc_addr_t_init(&ipmc_data);
            sal_memcpy(ipmc_data.mc_ip6_addr, ip6_addr, BCM_IP6_ADDRLEN);
            sal_memcpy(ipmc_data.s_ip6_addr, sip6_addr, BCM_IP6_ADDRLEN);
            bcm_ip6_mask_create(ipmc_data.mc_ip6_mask, mc_ip6_mask_len);
            ipmc_data.vid=vlan;
            ipmc_data.flags = BCM_IPMC_IP6;
            ipmc_data.vrf = vrf;
            ipmc_data.ing_intf = l3_iif;
            r = bcm_ipmc_remove(unit, &ipmc_data);
            if (r < 0) {
                cli_out("%s: Error delete from ipmc table %s\n", ARG_CMD(a),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            return CMD_OK;
        case IPMC_CLEAR:
            /* ipmc ip6table clear */
            r = bcm_ipmc_remove_all(unit);
            if (r < 0) {
                cli_out("%s: %s\n", ARG_CMD(a), bcm_errmsg(r));
                return CMD_FAIL;
            }
            return (CMD_OK);
        case IPMC_SHOW:
            if (ARG_CNT(a)) {
                r = BCM_E_UNAVAIL;
                return CMD_OK;
            }
            /* ipmc ip6table show */
            bcm_ipmc_traverse(unit, BCM_IPMC_IP6, cmd_ipmc_entry_print, NULL);
            return (CMD_OK);
        default:
            return CMD_USAGE;
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}

/*!
 * \brief ALPM cmd handler.
 *
 * \param [in] unit           Unit number
 * \param [in] arg            Arg structure for parsing arguments.
 *
 * \retval CMD_OK             Command completed successfully.
 * \retval CMD_FAIL           Command failed.
 */
cmd_result_t
cmd_ltsw_alpm(int unit, args_t *arg)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "usage")) {
        return cmd_ltsw_alpm_res_usage(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "projection")) {
        return cmd_ltsw_alpm_projection(unit, arg);
    } else {
        cli_out("Unsupported command. \n");
    }

    return CMD_OK;
}

#endif /* BCM_LTSW_SUPPORT */

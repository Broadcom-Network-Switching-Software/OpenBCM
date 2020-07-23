/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        knetctrl.c
 * Purpose:     Kernel network control
 * Requires:
 */

#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <appl/diag/system.h>
#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <bcm/error.h>
#include <bcm/knet.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <soc/knet.h>
#include "diag_dnx_knet.h"

#ifdef INCLUDE_KNET
char cmd_dnx_knet_usage[] = "Usages:\n"
    "  knetctrl netif create [Vlan=<vlan>] [Port=<port>] [RCPU=yes|no]\n"
    "        Create a virtual network interface.\n"
    "        Supported options:\n"
    "          [AddTag=yes|no]      - Add tag before sending to switch port\n"
    "          [KeepRxTag=yes|no]   - Keep tag of packets which are sent to this netif\n"
    "          [IFName=<str>]       - Optional network device name\n"
    "          [CBData=<val>]       - User data for knet rx cb\n"
    "  knetctrl netif destroy <id>\n"
    "        Destroy a virtual network interface.\n"
    "  knetctrl netif show\n"
    "        Show virtual network interfaces.\n"
    "  knetctrl filter create DestType=Null|NetIF|RxAPI [DestID=<id>] [options]\n"
    "        Create a packet filter.\n"
    "        DestID is required for DestType=NetIF only.\n"
    "        Supported options:\n"
    "          [DESCription=<str>]  - Optional filter description\n"
    "          [PRIOrity=<prio>]    - Filter match order priority (0 is highest)\n"
    "          [StripTag=yes|no]    - Strip VLAN tag before sending packet to NetIF\n"
    "          [Mirror=yes|no]      - Copy packet to RxAPI (if DestType=NetIF)\n"
    "          [DestProto=<proto>]  - Override default protocol (if DestType=NetIF)\n"
    "          [Vlan=<vlan>]        - Match specified VLAN ID\n"
    "          [IngPort=<port>]     - Match specified local ingress port\n"
    "          [SrcPort=<port>]     - Match specified module port\n"
    "          [SrcModid=<modid>]   - Match specified module ID\n"
    "          [TrapID=<id>]        - Match specified Trap ID\n"
    "          [Reason=<reason>]    - Match specified reason for copy to CPU\n"
    "          [PktOffset=<offet>]  - Match packet data starting at this offset\n"
    "          [PktByteX=<val>]     - Match packet data byte X (X=[0..7] supported)\n"
    "          [PktByte0=<val>]     - Match packet data byte 0 (example)\n"
    "          [CBData=<val>]       - User data for knet rx cb\n"
    "  knetctrl filter destroy <id>\n"
    "        Destroy a packet filter.\n" "  knetctrl filter show\n" "        Show all packet filters.\n";

/* Note:  See knet.h, BCM_KNET_NETIF_T_xxx */
STATIC char *dnx_knet_netif_type[] = {
    "unknown", "Vlan", "Port", "Meta", NULL
};

/* Note:  See knet.h, BCM_KNET_DEST_T_xxx */
STATIC char *dnx_knet_filter_dest_type[] = {
    "Null", "NetIF", "RxAPI", "CallBack", NULL
};

STATIC char *dnx_knet_reason_str[bcmRxReasonCount + 1] = BCM_RX_REASON_NAMES_INITIALIZER;

STATIC void
dnx_knet_show_netif(
    int unit,
    bcm_knet_netif_t * netif)
{
    char *type_str = "?";
    char *port_str = "n/a";

    switch (netif->type)
    {
        case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
        case BCM_KNET_NETIF_T_TX_META_DATA:
            type_str = dnx_knet_netif_type[netif->type];
            break;
        case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
            type_str = dnx_knet_netif_type[netif->type];
            port_str = BCM_PORT_NAME(unit, netif->port);
            break;
        default:
            break;
    }

    cli_out("Interface ID %d: name=%s type=%s vlan=%d port=%s",
            netif->id, netif->name, type_str, netif->vlan, port_str);

    if (netif->cb_user_data)
    {
        cli_out(" cbdata=%d", netif->cb_user_data);
    }
    if (netif->flags & BCM_KNET_NETIF_F_ADD_TAG)
    {
        cli_out(" addtag");
    }
    if (netif->flags & BCM_KNET_NETIF_F_RCPU_ENCAP)
    {
        cli_out(" rcpu");
    }
    if (netif->flags & BCM_KNET_NETIF_F_KEEP_RX_TAG)
    {
        cli_out(" keeprxtag");
    }
    cli_out("\n");
}

STATIC int
dnx_knet_trav_netif(
    int unit,
    bcm_knet_netif_t * netif,
    void *user_data)
{
    int *count = (int *) user_data;

    dnx_knet_show_netif(unit, netif);

    if (count)
    {
        (*count)++;
    }

    return BCM_E_NONE;
}

STATIC void
dnx_knet_show_filter(
    int unit,
    bcm_knet_filter_t * filter)
{
    char *dest_str = "?";
    char proto_str[16];
    int idx, edx;

    switch (filter->dest_type)
    {
        case BCM_KNET_DEST_T_NULL:
        case BCM_KNET_DEST_T_NETIF:
        case BCM_KNET_DEST_T_BCM_RX_API:
        case BCM_KNET_DEST_T_CALLBACK:
            dest_str = dnx_knet_filter_dest_type[filter->dest_type];
            break;
        default:
            break;
    }

    proto_str[0] = 0;
    if (filter->dest_proto)
    {
        sal_snprintf(proto_str, sizeof(proto_str), "[0x%04x]", filter->dest_proto);
    }
    cli_out("Filter ID %d: prio=%d dest=%s(%d)%s desc='%s'",
            filter->id, filter->priority, dest_str, filter->dest_id, proto_str, filter->desc);

    if (filter->cb_user_data)
    {
        cli_out(" cbdata=%d", filter->cb_user_data);
    }
    if (filter->mirror_type == BCM_KNET_DEST_T_BCM_RX_API)
    {
        cli_out(" mirror=rxapi");
    }
    else if (filter->mirror_type == BCM_KNET_DEST_T_NETIF)
    {
        proto_str[0] = 0;
        if (filter->mirror_proto)
        {
            sal_snprintf(proto_str, sizeof(proto_str), "[0x%04x]", filter->mirror_proto);
        }
        cli_out(" mirror=netif(%d)%s", filter->mirror_id, proto_str);
    }
    if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG)
    {
        cli_out(" striptag");
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_VLAN)
    {
        cli_out(" vlan=%d", filter->m_vlan);
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT)
    {
        cli_out(" ingport=%x", filter->m_ingport);
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODID)
    {
        cli_out(" srcmod=%d", filter->m_src_modid);
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_SRC_MODPORT)
    {
        cli_out(" srcport=%d", filter->m_src_modport);
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE)
    {
        cli_out(" trapid=%d", (filter->m_fp_rule >> 16) & 0xffff);
        cli_out(" trapqulifier=%d", filter->m_fp_rule & 0xffff);
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_REASON)
    {
        cli_out(" reason=");
        for (idx = 0; idx < bcmRxReasonCount; idx++)
        {
            if (BCM_RX_REASON_GET(filter->m_reason, idx))
            {
                cli_out("%s", dnx_knet_reason_str[idx]);
                break;
            }
        }
    }
    if (filter->match_flags & BCM_KNET_FILTER_M_RAW)
    {
        cli_out(" rawdata");
        for (idx = 0; idx < filter->raw_size; idx++)
        {
            if (filter->m_raw_mask[idx])
            {
                break;
            }
        }
        for (edx = filter->raw_size - 1; edx > idx; edx--)
        {
            if (filter->m_raw_mask[edx])
            {
                break;
            }
        }
        if (edx < idx)
        {
            /*
             * Entire mask is empty - should not happen
             */
            cli_out("?");
        }
        else
        {
            /*
             * Show offset of first valid byte
             */
            cli_out("[%d]", idx);
            /*
             * Dump data
             */
            for (; idx <= edx; idx++)
            {
                if (filter->m_raw_mask[idx])
                {
                    cli_out(":0x%02x", filter->m_raw_data[idx]);
                    if (filter->m_raw_mask[idx] != 0xff)
                    {
                        cli_out("/0x%02x", filter->m_raw_mask[idx]);
                    }
                }
                else
                {
                    cli_out(":-");
                }
            }
        }
    }
    cli_out("\n");
}

STATIC int
dnx_knet_trav_filter(
    int unit,
    bcm_knet_filter_t * filter,
    void *user_data)
{
    int *count = (int *) user_data;

    dnx_knet_show_filter(unit, filter);

    if (count)
    {
        (*count)++;
    }

    return BCM_E_NONE;
}

cmd_result_t
cmd_dnx_knet(
    int unit,
    args_t * args)
{
    parse_table_t pt;
    char *subcmd;
    int rv;
    int idx, pdx, count;
    int if_id;
    int if_type;
    int if_vlan;
    int if_port;
    int if_addtag;
    int if_rcpu;
    char *if_name;
    int pf_id;
    int pf_prio;
    int pf_dest_type;
    int pf_dest_id;
    int pf_striptag;
    int pf_mirror;
    int pf_mirror_id;
    int pf_vlan;
    int pf_ingport;
    int pf_src_modport;
    int pf_src_modid;
    int pf_reason;
    int pf_fp_rule;
    char *pf_desc;
    uint32 pkt_offset;
    int pkt_data[8];
    bcm_knet_netif_t netif;
    bcm_knet_filter_t filter;
    int pf_dest_proto;
    int pf_mirror_proto;
    int if_keeprxtag;
    int if_cb_user_data;
    int pf_cb_user_data;

    if ((subcmd = ARG_GET(args)) == NULL)
    {
        cli_out("Requires string argument\n");
        return CMD_USAGE;
    }
    if (sal_strcasecmp(subcmd, "netif") == 0)
    {
        if ((subcmd = ARG_GET(args)) == NULL)
        {
            cli_out("Requires additional string argument\n");
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd, "create") == 0)
        {
            if_type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
            if_vlan = 1;
            if_port = -1;
            if_addtag = 0;
            if_name = NULL;
            if_keeprxtag = 0;
            if_rcpu = 0;
            if_cb_user_data = 0;
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Type", PQ_DFL | PQ_MULTI, 0, &if_type, dnx_knet_netif_type);
            parse_table_add(&pt, "Vlan", PQ_DFL | PQ_INT, 0, &if_vlan, 0);
            parse_table_add(&pt, "Port", PQ_DFL | PQ_PORT, 0, &if_port, 0);
            parse_table_add(&pt, "AddTag", PQ_DFL | PQ_BOOL, 0, &if_addtag, 0);
            parse_table_add(&pt, "KeepRxTag", PQ_DFL | PQ_BOOL, 0, &if_keeprxtag, 0);
            parse_table_add(&pt, "RCPU", PQ_DFL | PQ_BOOL, 0, &if_rcpu, 0);
            parse_table_add(&pt, "IFName", PQ_DFL | PQ_STRING, 0, &if_name, 0);
            parse_table_add(&pt, "CBData", PQ_DFL | PQ_INT, 0, &if_cb_user_data, 0);
            if (parse_arg_eq(args, &pt) < 0)
            {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            bcm_knet_netif_t_init(&netif);
            if (if_name)
            {
                sal_strncpy(netif.name, if_name, sizeof(netif.name) - 1);
            }
            parse_arg_eq_done(&pt);
            netif.type = if_type;
            netif.vlan = if_vlan;
            netif.cb_user_data = if_cb_user_data;
            if (if_port >= 0)
            {
                netif.port = if_port;
            }
            if (if_addtag)
            {
                netif.flags |= BCM_KNET_NETIF_F_ADD_TAG;
            }
            if (if_keeprxtag)
            {
                netif.flags |= BCM_KNET_NETIF_F_KEEP_RX_TAG;
            }
            if (if_rcpu)
            {
                netif.flags |= BCM_KNET_NETIF_F_RCPU_ENCAP;
            }
            if ((rv = bcm_knet_netif_create(unit, &netif)) < 0)
            {
                cli_out("Error creating network interface: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            dnx_knet_show_netif(unit, &netif);
        }
        else if (sal_strcasecmp(subcmd, "destroy") == 0)
        {
            if (!ARG_CNT(args) || !isint(ARG_CUR(args)))
            {
                return (CMD_USAGE);
            }
            if_id = parse_integer(ARG_GET(args));
            if ((rv = bcm_knet_netif_destroy(unit, if_id)) < 0)
            {
                cli_out("Error destroying network interface: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        else if (sal_strcasecmp(subcmd, "show") == 0)
        {
            count = 0;
            if (bcm_knet_netif_traverse(unit, dnx_knet_trav_netif, &count) < 0)
            {
                return CMD_FAIL;
            }
            if (count == 0)
            {
                cli_out("<no network interfaces>\n");
            }
        }
        else
        {
            cli_out("Subcommand not found: %s\n", subcmd);
            return CMD_USAGE;
        }
    }
    else if (sal_strcasecmp(subcmd, "filter") == 0)
    {
        if ((subcmd = ARG_GET(args)) == NULL)
        {
            cli_out("Requires string argument\n");
            return CMD_USAGE;
        }
        if (sal_strcasecmp(subcmd, "create") == 0)
        {
            pf_dest_type = BCM_KNET_DEST_T_NULL;
            pf_dest_id = -1;
            pf_mirror = 0;
            pf_mirror_id = 0;
            pf_vlan = -1;
            pf_ingport = -1;
            pf_src_modport = -1;
            pf_src_modid = -1;
            pf_reason = bcmRxReasonInvalid;
            pf_fp_rule = -1;
            pkt_offset = 0;
            for (idx = 0; idx < COUNTOF(pkt_data); idx++)
            {
                pkt_data[idx] = -1;
            }
            pf_desc = NULL;
            pf_prio = 0;
            pf_striptag = 1;
            pf_dest_proto = 0;
            pf_mirror_proto = 0;
            pf_cb_user_data = 0;
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "DestType", PQ_DFL | PQ_MULTI, 0, &pf_dest_type, dnx_knet_filter_dest_type);
            parse_table_add(&pt, "DestID", PQ_DFL | PQ_INT, 0, &pf_dest_id, 0);
            parse_table_add(&pt, "PRIOrity", PQ_DFL | PQ_INT, 0, &pf_prio, 0);
            parse_table_add(&pt, "DESCription", PQ_DFL | PQ_STRING, 0, &pf_desc, 0);
            parse_table_add(&pt, "StripTag", PQ_DFL | PQ_BOOL, 0, &pf_striptag, 0);
            parse_table_add(&pt, "Mirror", PQ_DFL | PQ_BOOL, 0, &pf_mirror, 0);
            parse_table_add(&pt, "MirrorID", PQ_DFL | PQ_INT, 0, &pf_mirror_id, 0);
            parse_table_add(&pt, "Vlan", PQ_DFL | PQ_INT, 0, &pf_vlan, 0);
            parse_table_add(&pt, "IngPort", PQ_DFL | PQ_PORT, 0, &pf_ingport, 0);
            parse_table_add(&pt, "SrcPort", PQ_DFL | PQ_INT, 0, &pf_src_modport, 0);
            parse_table_add(&pt, "SrcModid", PQ_DFL | PQ_INT, 0, &pf_src_modid, 0);
            parse_table_add(&pt, "Reason", PQ_DFL | PQ_MULTI, 0, &pf_reason, dnx_knet_reason_str);
            parse_table_add(&pt, "TrapID", PQ_DFL | PQ_INT, 0, &pf_fp_rule, 0);
            parse_table_add(&pt, "PktOffset", PQ_DFL | PQ_HEX, 0, &pkt_offset, 0);
            parse_table_add(&pt, "PktByte0", PQ_DFL | PQ_INT, 0, &pkt_data[0], 0);
            parse_table_add(&pt, "PktByte1", PQ_DFL | PQ_INT, 0, &pkt_data[1], 0);
            parse_table_add(&pt, "PktByte2", PQ_DFL | PQ_INT, 0, &pkt_data[2], 0);
            parse_table_add(&pt, "PktByte3", PQ_DFL | PQ_INT, 0, &pkt_data[3], 0);
            parse_table_add(&pt, "PktByte4", PQ_DFL | PQ_INT, 0, &pkt_data[4], 0);
            parse_table_add(&pt, "PktByte5", PQ_DFL | PQ_INT, 0, &pkt_data[5], 0);
            parse_table_add(&pt, "PktByte6", PQ_DFL | PQ_INT, 0, &pkt_data[6], 0);
            parse_table_add(&pt, "PktByte7", PQ_DFL | PQ_INT, 0, &pkt_data[7], 0);
            parse_table_add(&pt, "DestProto", PQ_DFL | PQ_INT, 0, &pf_dest_proto, 0);
            parse_table_add(&pt, "MirrorProto", PQ_DFL | PQ_INT, 0, &pf_mirror_proto, 0);
            parse_table_add(&pt, "CBData", PQ_DFL | PQ_INT, 0, &pf_cb_user_data, 0);
            if (parse_arg_eq(args, &pt) < 0)
            {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            bcm_knet_filter_t_init(&filter);
            filter.type = BCM_KNET_FILTER_T_RX_PKT;
            if (pf_desc)
            {
                sal_strncpy(filter.desc, pf_desc, sizeof(filter.desc) - 1);
            }
            parse_arg_eq_done(&pt);

            filter.priority = pf_prio;
            filter.dest_type = pf_dest_type;
            filter.dest_id = pf_dest_id;
            filter.dest_proto = pf_dest_proto;
            filter.cb_user_data = pf_cb_user_data;
            /*
             * Specifying a mirror ID implies mirroring to another
             * netif.  Enabling mirroring while leaving mirror ID at
             * zero implies mirroring to the Rx API.
             */
            if (pf_mirror_id)
            {
                pf_mirror = 1;
            }
            if (pf_dest_type == BCM_KNET_DEST_T_NETIF && pf_mirror)
            {
                filter.mirror_type = BCM_KNET_DEST_T_BCM_RX_API;
                if (pf_mirror_id)
                {
                    filter.mirror_type = BCM_KNET_DEST_T_NETIF;
                    filter.mirror_id = pf_mirror_id;
                    filter.mirror_proto = pf_mirror_proto;
                }
            }
            if (pf_striptag)
            {
                filter.flags |= BCM_KNET_FILTER_F_STRIP_TAG;
            }
            if (pf_vlan >= 0)
            {
                filter.m_vlan = pf_vlan;
                filter.match_flags |= BCM_KNET_FILTER_M_VLAN;
            }
            if (pf_ingport >= 0)
            {
                filter.m_ingport = pf_ingport;
                filter.match_flags |= BCM_KNET_FILTER_M_INGPORT;
            }
            if (pf_src_modport >= 0)
            {
                filter.m_src_modport = pf_src_modport;
                filter.match_flags |= BCM_KNET_FILTER_M_SRC_MODPORT;
            }
            if (pf_src_modid >= 0)
            {
                filter.m_src_modid = pf_src_modid;
                filter.match_flags |= BCM_KNET_FILTER_M_SRC_MODID;
            }
            if (pf_reason != bcmRxReasonInvalid)
            {
                BCM_RX_REASON_SET(filter.m_reason, pf_reason);
                filter.match_flags |= BCM_KNET_FILTER_M_REASON;
            }
            if (pf_fp_rule >= 0)
            {
                filter.m_fp_rule = pf_fp_rule;
                filter.match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            }
            for (idx = 0; idx < COUNTOF(pkt_data); idx++)
            {
                pdx = pkt_offset + idx;
                if (pdx >= sizeof(filter.m_raw_data))
                {
                    cli_out("PktOffset too large - max is %d\n", (int) sizeof(filter.m_raw_data) - COUNTOF(pkt_data));
                    return CMD_USAGE;
                }
                if (pkt_data[idx] >= 0)
                {
                    filter.raw_size = pdx + 1;
                    filter.match_flags |= BCM_KNET_FILTER_M_RAW;
                    filter.m_raw_data[pdx] = (uint8) pkt_data[idx];
                    filter.m_raw_mask[pdx] = 0xff;
                }
            }
            if ((rv = bcm_knet_filter_create(unit, &filter)) < 0)
            {
                cli_out("Error creating packet filter: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            dnx_knet_show_filter(unit, &filter);
        }
        else if (sal_strcasecmp(subcmd, "destroy") == 0)
        {
            if (!ARG_CNT(args) || !isint(ARG_CUR(args)))
            {
                return (CMD_USAGE);
            }
            pf_id = parse_integer(ARG_GET(args));
            if ((rv = bcm_knet_filter_destroy(unit, pf_id)) < 0)
            {
                cli_out("Error destroying packet filter: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
        else if (sal_strcasecmp(subcmd, "show") == 0)
        {
            count = 0;
            if (bcm_knet_filter_traverse(unit, dnx_knet_trav_filter, &count) < 0)
            {
                return CMD_FAIL;
            }
            if (count == 0)
            {
                cli_out("<no filters>\n");
            }
        }
        else
        {
            cli_out("Subcommand not found: %s\n", subcmd);
            return CMD_USAGE;
        }
    }
    else
    {
        cli_out("Subcommand not found: %s\n", subcmd);
        return CMD_USAGE;
    }
    return CMD_OK;
}

#else
typedef int _knetctrl_not_empty;        /* Make ISO compilers happy. */
#endif

/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * HashDestination commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#ifdef INCLUDE_L3
#include <bcm/l3.h>
#ifdef BCM_TOMAHAWK_SUPPORT
#include <bcm_int/esw/firebolt.h>
#endif
#endif
#include <bcm/trunk.h>
#include <bcm/switch.h>

#if defined(BCM_ESW_SUPPORT) 
#include <bcm_int/esw/port.h>
#endif

#define   HASH_DESTINATION_ERROR_PRINT(_r) \
    do { \
        cli_out(\
                "HashDestination: %s\n", bcm_errmsg(_r)); \
} while(0)


#define   DYNAMIC_ETHERTYPE_ELIGIBLE 0x1
#define   DYNAMIC_ETHERTYPE_INNER    0x2

#define   ETHERTYPE_ELIGIBILITY_MAP_TABLE_SIZE_MAX  16 /* maximum ELIGIBILITY ethertype size */

#define   ETHERTYPE_IPV6 0x86dd /* ipv6 ethertype */
#define   ETHERTYPE_IPV4 0x0800 /* ipv4 ethertype */
#define   ETHERTYPE_MIN  0x0600 /* minimum ethertype for hashing */

#define   IP_PROT_TCP 0x6  /* TCP protocol number */
#define   IP_PROT_UDP 0x11 /* TCP protocol number */

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_ENDURO_SUPPORT) || \
    defined(BCM_GREYHOUND2_SUPPORT)
/* Packet info flags for HashDestination. */
#define BCM_HD_FLAGS_STR \
    "SRC_GPORT", \
    "VLAN", \
    "ETHERTYPE", \
    "SRC_MAC", \
    "DST_MAC", \
    "SRC_IP", \
    "DST_IP", \
    "SRC_IPV6", \
    "DST_IPV6", \
    "PROTOCOL", \
    "SRC_L4_PORT", \
    "DST_L4_PORT", \
    "HASH_TRUNK", \
    "HASH_MULTIPATH", \
    "UDP_SOURCE_PORT", \
    "LBID"

STATIC char *hd_flags_names[] = {
    BCM_HD_FLAGS_STR
};


STATIC char *hd_resource_group_names[] = {
#ifdef INCLUDE_L3
    "ECMP",
#endif
    "LAG",
    "HGT",
    "ANY",
    NULL
};

STATIC char *hd_pkt_forward_reason_names[] = {
    BCM_SWITCH_PKT_HASH_INFO_FWD_REASON_STRINGS,
    NULL
};

STATIC char *hd_calc_info_names[] = {
    "UDP_SOURCE_PORT",
    "LBID",
    NULL
};

#ifdef BCM_TRIUMPH3_SUPPORT
STATIC int (*bcm_hd_ethertype_set[])(int, 
                uint32, int, int*) = {
#ifdef INCLUDE_L3
    bcm_l3_egress_ecmp_ethertype_set,
#endif
    bcm_trunk_ethertype_set,
    bcm_trunk_ethertype_set,
    NULL
};

STATIC int (*bcm_hd_ethertype_get[])(int, 
                uint32*, int, int*, int*) = {
#ifdef INCLUDE_L3
    bcm_l3_egress_ecmp_ethertype_get,
#endif
    bcm_trunk_ethertype_get,
    bcm_trunk_ethertype_get,
    NULL
};
#endif

char hash_destination_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "hd <option> [args...]\n\t"
#else
#ifdef BCM_TRIUMPH3_SUPPORT
    "hd ethertype mode [Group=ECMP|LAG|HGT] [Eligible=T|F] [Inner=T|F]\n\t"
    "hd ethertype add [Group=ECMP|LAG|HGT] [EtherType=<0xHHHH>]\n\t"
    "hd ethertype delete [Group=ECMP|LAG|HGT] [EtherType=<0xHHHH>]\n\t"
    "hd ethertype show [Group=ECMP|LAG|HGT]\n\t"
#endif
    "hd dest get [Group=ECMP|LAG|HGT|ANY] [GID=<id>]\n\t"
    "         [Port=<port#>] [Module=<modid>] [Untagged=T|F]\n\t"
    "         [DestMac=<mac>] [SrcMac=<mac>] [Vlan=<vid>] [EtherType=<0xHHHH>]\n\t"
    "         [SrcIp=<ip4>] [DestIp=<ip4>] [SIp6=<ip6>] [DIp6=<ip6>]\n\t"
    "         [PROTocol=<Pid>] [L4SrcPort=<num>] [L4DstPort=<num>]\n\t"
    "         [FwdReason=Unicast|Ipmc|L2mc|Bcast|Dlf]\n\t"
    "         [CalcInfo=UDP_SOURCE_PORT|LBID]\n\t"
#endif
    "\n"
    ;


#ifdef BCM_TRIUMPH3_SUPPORT
STATIC cmd_result_t
_cmd_hd_ethertype(int unit, args_t *a)
{
    char *subcmd = NULL;
    parse_table_t  pt;
    int            r = BCM_E_UNAVAIL;
    int            i, gid, retCode = CMD_OK;
    int            group_all = 0, arg_group = -1,
                   arg_eligible = -1, arg_inner = -1, arg_ethertype = 0xffff;
    uint32         flags, origin_flags;
    int ethertype_count = -1, cnt = 0;
    int ethertype_array[ETHERTYPE_ELIGIBILITY_MAP_TABLE_SIZE_MAX];

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    
    if (ARG_CNT(a) == 0) {
        group_all = 1;
    } else {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "Group", PQ_DFL|PQ_MULTI,
                0, &arg_group, hd_resource_group_names);

        if (!sal_strcasecmp(subcmd, "mode")) {
            /* BCM.0> hd eth mode ... */
            parse_table_add(&pt, "Eligible", PQ_DFL|PQ_BOOL,
                    0, &arg_eligible, NULL);
            parse_table_add(&pt, "Inner", PQ_DFL|PQ_BOOL,
                    0, &arg_inner, NULL);
        } else 
        if (!sal_strcasecmp(subcmd, "add") ||
            !sal_strcasecmp(subcmd, "delete")) {
            /* BCM.0> hd eth add/delete ... */
            parse_table_add(&pt, "EtherType", PQ_DFL|PQ_HEX,
                    0, &arg_ethertype, NULL);
        }

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        if (arg_group < 0) {
            group_all = 1;
        }
    }

    for (gid = 0; gid < COUNTOF(hd_resource_group_names) - 1; gid++) {
        if (group_all == 1 || arg_group == gid) {
#ifdef INCLUDE_L3
            if (!sal_strcasecmp(hd_resource_group_names[gid], "ECMP")) {
                flags = BCM_L3_ECMP_DYNAMIC_ETHERTYPE_RESILIENT;
            } else 
#endif
            {
                flags = BCM_TRUNK_DYNAMIC_ETHERTYPE_RESILIENT;
                if (!sal_strcasecmp(hd_resource_group_names[gid], "LAG")) {
                    flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_TRUNK;
                } else if (!sal_strcasecmp(hd_resource_group_names[gid], "HGT")) {
                    flags |= BCM_TRUNK_DYNAMIC_ETHERTYPE_FABRIC_TRUNK; /*HGT*/
                } else {
                    r = BCM_E_PARAM;
                    HASH_DESTINATION_ERROR_PRINT(r);
                    return CMD_FAIL;
                }
            }

            ethertype_count = -1;

            r = bcm_hd_ethertype_get[gid](unit, 
                    &flags, ethertype_count, ethertype_array, &ethertype_count);
            if (BCM_FAILURE(r)) {
                HASH_DESTINATION_ERROR_PRINT(r);
                return CMD_FAIL;
            }

            origin_flags = flags;

            if (!sal_strcasecmp(subcmd, "mode")) {
                if (arg_eligible < 0 && arg_inner < 0) {
                    return CMD_USAGE;
                }

                flags &= ~(DYNAMIC_ETHERTYPE_INNER | DYNAMIC_ETHERTYPE_ELIGIBLE);

                if (arg_eligible) {
                    flags |= DYNAMIC_ETHERTYPE_ELIGIBLE;
                }

                if (arg_inner) {
                    flags |= DYNAMIC_ETHERTYPE_INNER;
                }

                if (flags == origin_flags) {
                    continue;
                }
            } else 
            if (!sal_strcasecmp(subcmd, "add")) {
                if (arg_ethertype == 0xffff) {
                    return CMD_USAGE;
                }
                
                if (ethertype_count >= ETHERTYPE_ELIGIBILITY_MAP_TABLE_SIZE_MAX) {
                    r = BCM_E_FULL;
                    HASH_DESTINATION_ERROR_PRINT(r);
                    return CMD_FAIL;
                }
                for (i = 0; i < ethertype_count; i++) {
                    if (ethertype_array[i] == arg_ethertype) {
                        r = BCM_E_EXISTS;
                        HASH_DESTINATION_ERROR_PRINT(r);
                        return CMD_FAIL;
                    }
                }
                ethertype_array[i] = arg_ethertype;
                ethertype_count++;
            } else
            if (!sal_strcasecmp(subcmd, "delete")) {
                if (arg_ethertype == 0xffff) {
                    return CMD_USAGE;
                }

                if (ethertype_count <= 0) {
                    r = BCM_E_EMPTY;
                    HASH_DESTINATION_ERROR_PRINT(r);
                    return CMD_FAIL;
                }

                for (i = 0; i < ethertype_count; i++) {
                    if (ethertype_array[i] == arg_ethertype) {
                        break;
                    }
                }
                if (i == ethertype_count) {
                    r = BCM_E_NOT_FOUND;
                    HASH_DESTINATION_ERROR_PRINT(r);
                    return CMD_FAIL;
                }
                
                ethertype_array[i] = ethertype_array[ethertype_count - 1];
                ethertype_count--;
            }

            r = bcm_hd_ethertype_set[gid](unit, 
                    flags, ethertype_count, ethertype_array);
            if (BCM_FAILURE(r)) {
                HASH_DESTINATION_ERROR_PRINT(r);
                return CMD_FAIL;
            }

            if (!sal_strcasecmp(subcmd, "show")) {
                /* BCM.0> hd eth show ... */
                cli_out("  %s:\n", hd_resource_group_names[gid]);
                cli_out("    EtherType: %s %s    Cnt:%d\n\t", 
                        flags & DYNAMIC_ETHERTYPE_INNER ? "INNER":"OUTER",
                        flags & DYNAMIC_ETHERTYPE_ELIGIBLE ? "Eligible":"Ineligible",
                        ethertype_count);
                for (i = 0, cnt = 0; i < ethertype_count; i++) {
                    cnt++;
                    cli_out("0x%04x %s",
                            ethertype_array[i], (cnt % 4 == 0) ? "\n\t":"");
                }
                cli_out("\n");
            }
        }
    }
    
    return CMD_OK;
}
#endif


STATIC cmd_result_t
_cmd_hd_dest_get(int unit, args_t *a)
{
    parse_table_t  pt;
    int            r = BCM_E_UNAVAIL;
    int            i, retCode = CMD_OK;
    int            arg_group = -1, arg_gid = -1;
    int            arg_untagged = FALSE;
    int            arg_port = BCM_GPORT_INVALID, arg_modid = BCM_MODID_INVALID,
                   arg_vlan = VLAN_ID_INVALID, arg_ethertype = 0xffff; 
    int            arg_fwd_reason = bcmSwitchPktHashInfoFwdReasonUnicast;
    int            arg_calc_info = -1;
    sal_mac_addr_t default_macaddr = {0, 0, 0, 0, 0, 0x1};
    sal_mac_addr_t arg_srcmac = {0, 0, 0, 0, 0, 0x1};
    sal_mac_addr_t arg_dstmac = {0, 0, 0, 0, 0, 0x1};
    bcm_trunk_t    tid;
    bcm_trunk_chip_info_t   ti;
    bcm_trunk_info_t   t_add_info;
    bcm_trunk_member_t *member_array = NULL;
    int            member_count = 0;
#ifdef INCLUDE_L3
    bcm_ip_t       arg_sip_addr = 0, arg_ip_addr = 0;
    bcm_ip6_t      arg_sip6_addr, arg_ip6_addr, default_ip6_addr;
    int            arg_protocol = -1, arg_l4srcport = -1, arg_l4dstport = -1;

    int            intf_count = 0;
    bcm_if_t       *mpath_egr = NULL;
#endif

    bcm_switch_pkt_info_t pkt_info;
    bcm_gport_t    dst_gport = BCM_GPORT_INVALID;
    bcm_if_t       dst_intf = BCM_IF_INVALID;

    if (ARG_CNT(a) == 0) {
        return CMD_USAGE;
    }

#ifdef INCLUDE_L3
    sal_memset(arg_sip6_addr, 0, BCM_IP6_ADDRLEN);
    sal_memset(arg_ip6_addr, 0, BCM_IP6_ADDRLEN);
    sal_memset(default_ip6_addr, 0, BCM_IP6_ADDRLEN);
#endif

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Group",     PQ_DFL|PQ_MULTI,
            0, &arg_group, hd_resource_group_names);
    parse_table_add(&pt, "GID",       PQ_DFL|PQ_HEX,
            0, &arg_gid, NULL);
    parse_table_add(&pt, "Port",      PQ_DFL|PQ_PORT, 
            0, &arg_port, NULL);
    parse_table_add(&pt, "Modid",     PQ_DFL|PQ_INT, 
            0, &arg_modid, NULL);
    parse_table_add(&pt, "Untagged",  PQ_DFL|PQ_BOOL,
            0, &arg_untagged, NULL);
    parse_table_add(&pt, "DestMac",   PQ_DFL|PQ_MAC,
            0, &arg_dstmac, NULL);
    parse_table_add(&pt, "SrcMac",    PQ_DFL|PQ_MAC,
            0, &arg_srcmac, NULL);
    parse_table_add(&pt, "Vlan",      PQ_DFL|PQ_INT,
            0, &arg_vlan, NULL);
    parse_table_add(&pt, "EtherType", PQ_DFL|PQ_HEX,
            0, &arg_ethertype, NULL);
#ifdef INCLUDE_L3
    parse_table_add(&pt, "SrcIp",     PQ_DFL|PQ_IP,
            0, &arg_sip_addr, NULL);
    parse_table_add(&pt, "DestIp",    PQ_DFL|PQ_IP,
            0, &arg_ip_addr, NULL);
    parse_table_add(&pt, "SIp6",      PQ_DFL|PQ_IP6,
            0, (void *)&arg_sip6_addr, NULL);
    parse_table_add(&pt, "DIp6",      PQ_DFL|PQ_IP6,
            0, (void *)&arg_ip6_addr, NULL);
    parse_table_add(&pt, "PROTocol",  PQ_DFL|PQ_INT, 
            0, &arg_protocol, NULL);
    parse_table_add(&pt, "L4SrcPort", PQ_DFL|PQ_INT, 
            0, &arg_l4srcport, NULL);
    parse_table_add(&pt, "L4DstPort", PQ_DFL|PQ_INT, 
            0, &arg_l4dstport, NULL);
#endif
    parse_table_add(&pt, "FwdReason", PQ_DFL|PQ_MULTI,
            0, &arg_fwd_reason, hd_pkt_forward_reason_names);
    parse_table_add(&pt, "CalcInfo",  PQ_DFL|PQ_MULTI,
            0, &arg_calc_info, hd_calc_info_names);

    if (!parseEndOk(a, &pt, &retCode)) {
        return retCode;
    }

    if (arg_group < 0 || 
        arg_group > (COUNTOF(hd_resource_group_names) - 1) ||
        (arg_gid < 0 && sal_strcasecmp(hd_resource_group_names[arg_group], "ANY"))) {
        return CMD_USAGE;
    }

    bcm_switch_pkt_info_t_init(&pkt_info);

#ifdef INCLUDE_L3
    if (!sal_strcasecmp(hd_resource_group_names[arg_group], "ECMP")) {
        pkt_info.flags |= BCM_SWITCH_PKT_INFO_HASH_MULTIPATH;

        r = bcm_l3_egress_multipath_get(unit, arg_gid, 0, NULL,
                                         &intf_count);
        mpath_egr = sal_alloc((intf_count * sizeof(int)), 
                "ecmp group");
        if (mpath_egr == NULL) {
            r = BCM_E_MEMORY;
            HASH_DESTINATION_ERROR_PRINT(r);
            return CMD_FAIL;
        }
        r = bcm_l3_egress_multipath_get(unit, arg_gid, intf_count, 
                                         mpath_egr, &intf_count);
        if (BCM_FAILURE(r)) {
            HASH_DESTINATION_ERROR_PRINT(r);
            retCode = CMD_FAIL;
            goto error_cleanup;
        }

        pkt_info.mpintf = arg_gid;
    } else
#endif
    if (!sal_strcasecmp(hd_resource_group_names[arg_group], "LAG") || 
        !sal_strcasecmp(hd_resource_group_names[arg_group], "HGT")) {

        pkt_info.flags |= BCM_SWITCH_PKT_INFO_HASH_TRUNK;

        if (BCM_GPORT_IS_TRUNK(arg_gid)) {
            tid = BCM_GPORT_TRUNK_GET(arg_gid);
        } else {
            tid = arg_gid;
        }

        if ((r = bcm_trunk_chip_info_get(unit, &ti)) < 0) {
            HASH_DESTINATION_ERROR_PRINT(r);
            return CMD_FAIL;
        }

        if ((!sal_strcasecmp(hd_resource_group_names[arg_group], "LAG") &&
            (tid < ti.trunk_id_min || tid > ti.trunk_id_max)) ||
            (!sal_strcasecmp(hd_resource_group_names[arg_group], "HGT") &&
            (tid < ti.trunk_fabric_id_min || tid > ti.trunk_fabric_id_max))) {
            r = BCM_E_PARAM;
            HASH_DESTINATION_ERROR_PRINT(r);
            return CMD_FAIL;
        }

        member_array = sal_alloc(
                sizeof(bcm_trunk_member_t) * BCM_TRUNK_MAX_PORTCNT,
                "member array");
        if (NULL == member_array) {
            r = BCM_E_MEMORY;
            HASH_DESTINATION_ERROR_PRINT(r);
            return CMD_FAIL;
        }

        r = bcm_trunk_get(unit, tid, &t_add_info, BCM_TRUNK_MAX_PORTCNT,
                member_array, &member_count);
        if (BCM_FAILURE(r)) {
            HASH_DESTINATION_ERROR_PRINT(r);
            retCode = CMD_FAIL;
            goto error_cleanup;
        }

        BCM_GPORT_TRUNK_SET(pkt_info.trunk_gport, 
                            tid);
    } else if (!sal_strcasecmp(hd_resource_group_names[arg_group], "ANY")) {
        if (arg_calc_info < 0 || 
            arg_calc_info > (COUNTOF(hd_calc_info_names) - 1)) {
            r = BCM_E_PARAM;
            HASH_DESTINATION_ERROR_PRINT(r);
            return CMD_FAIL;
        }
        if (!sal_strcasecmp(hd_calc_info_names[arg_calc_info], "UDP_SOURCE_PORT")) {
            pkt_info.flags |= BCM_SWITCH_PKT_INFO_HASH_UDP_SOURCE_PORT;
        } else if (!sal_strcasecmp(hd_calc_info_names[arg_calc_info], "LBID")) {
            pkt_info.flags |= BCM_SWITCH_PKT_INFO_HASH_LBID;
        } else {
            r = BCM_E_PARAM;
            HASH_DESTINATION_ERROR_PRINT(r);
            return CMD_FAIL;
        }
    } else {
        r = BCM_E_PARAM;
        HASH_DESTINATION_ERROR_PRINT(r);
        return CMD_FAIL;
    }

    if (BCM_GPORT_IS_SET(arg_port)) {
#if defined(BCM_ESW_SUPPORT)
        if (SOC_IS_ESW(unit)) {
            int tgid, id;

            r = _bcm_esw_gport_resolve(unit, arg_port, &arg_modid, 
                                       &arg_port, &tgid, &id);
            if ((tgid != BCM_TRUNK_INVALID) || (id != -1)) {
                r = BCM_E_PORT;
                HASH_DESTINATION_ERROR_PRINT(r);
                retCode = CMD_FAIL;
                goto error_cleanup;
            }

            pkt_info.src_gport = arg_port;
        } else 
#endif
        {
            HASH_DESTINATION_ERROR_PRINT(r);
            retCode = CMD_FAIL;
            goto error_cleanup;
        }
    } else if (arg_modid == BCM_MODID_INVALID) {
        (void)bcm_stk_my_modid_get(unit, 
                                &arg_modid);
        r = _bcm_esw_port_gport_validate(unit, arg_port, 
                                &arg_port);
        if (BCM_FAILURE(r)) {
            HASH_DESTINATION_ERROR_PRINT(r);
            retCode = CMD_FAIL;
            goto error_cleanup;
        }

        BCM_GPORT_LOCAL_SET(pkt_info.src_gport, arg_port);
    } else if (arg_modid >= 0 && arg_port >= 0) {
        int mod_is_local;

        r = _bcm_esw_modid_is_local(unit, arg_modid, &mod_is_local);
        if (BCM_FAILURE(r)) {
            HASH_DESTINATION_ERROR_PRINT(r);
            retCode = CMD_FAIL;
            goto error_cleanup;
        }
        if (mod_is_local) {
            BCM_GPORT_MODPORT_SET(pkt_info.src_gport, 
                                    arg_modid, arg_port);
        } else {
            BCM_GPORT_PROXY_SET(pkt_info.src_gport, 
                                    arg_modid, arg_port);
        }
    } else {
        r = BCM_E_PORT;
        HASH_DESTINATION_ERROR_PRINT(r);
        retCode = CMD_FAIL;
        goto error_cleanup;
    }

    pkt_info.flags |= BCM_SWITCH_PKT_INFO_SRC_GPORT;

    if (ENET_CMP_MACADDR(arg_dstmac, default_macaddr)) {
        pkt_info.flags |= BCM_SWITCH_PKT_INFO_DST_MAC;
        sal_memcpy(pkt_info.dst_mac, 
                            arg_dstmac, sizeof(sal_mac_addr_t));
    }

    if (ENET_CMP_MACADDR(arg_srcmac, default_macaddr)) {
        pkt_info.flags |= BCM_SWITCH_PKT_INFO_SRC_MAC;
        sal_memcpy(pkt_info.src_mac, 
                            arg_srcmac, sizeof(sal_mac_addr_t));
    }

    if (arg_untagged) {
        pkt_info.flags |= BCM_SWITCH_PKT_INFO_VLAN;
        pkt_info.vid = VLAN_ID_NONE;
    } else {
        if (VLAN_ID_VALID(arg_vlan)) {
            pkt_info.flags |= BCM_SWITCH_PKT_INFO_VLAN;
            pkt_info.vid = arg_vlan;
        }
    }

    if ((arg_ethertype < 0xffff) &&
        (arg_ethertype >= ETHERTYPE_MIN)) {
        pkt_info.flags |= BCM_SWITCH_PKT_INFO_ETHERTYPE;
        pkt_info.ethertype = arg_ethertype;

#ifdef INCLUDE_L3
        if (pkt_info.ethertype == ETHERTYPE_IPV4 ||
            pkt_info.ethertype == ETHERTYPE_IPV6) {

            if (pkt_info.ethertype == ETHERTYPE_IPV4 && 
                arg_sip_addr != 0) {
                pkt_info.flags |= BCM_SWITCH_PKT_INFO_SRC_IP;
                pkt_info.sip = arg_sip_addr;
            }

            if (pkt_info.ethertype == ETHERTYPE_IPV4 && 
                arg_ip_addr != 0) {
                pkt_info.flags |= BCM_SWITCH_PKT_INFO_DST_IP;
                pkt_info.dip = arg_ip_addr;
            }

            if (pkt_info.ethertype == ETHERTYPE_IPV6 && 
                !BCM_IP6_ADDR_EQ(arg_sip6_addr, default_ip6_addr)) {
                pkt_info.flags |= BCM_SWITCH_PKT_INFO_SRC_IPV6;
                sal_memcpy(pkt_info.sip6, arg_sip6_addr, BCM_IP6_ADDRLEN);
            }

            if (pkt_info.ethertype == ETHERTYPE_IPV6 && 
                !BCM_IP6_ADDR_EQ(arg_ip6_addr, default_ip6_addr)) {
                pkt_info.flags |= BCM_SWITCH_PKT_INFO_DST_IPV6;
                sal_memcpy(pkt_info.dip6, arg_ip6_addr, BCM_IP6_ADDRLEN);
            }

            if (arg_protocol >= 0 && arg_protocol <= 0xff) {
                pkt_info.flags |= BCM_SWITCH_PKT_INFO_PROTOCOL;
                pkt_info.protocol = arg_protocol & 0xff;

                if (pkt_info.protocol == IP_PROT_TCP ||
                    pkt_info.protocol == IP_PROT_UDP) {
                    if (arg_l4srcport != -1) {
                        pkt_info.flags |= BCM_SWITCH_PKT_INFO_SRC_L4_PORT;
                        pkt_info.src_l4_port = arg_l4srcport & 0xffff;
                    }
                    if (arg_l4dstport != -1) {
                        pkt_info.flags |= BCM_SWITCH_PKT_INFO_DST_L4_PORT;
                        pkt_info.dst_l4_port = arg_l4dstport & 0xffff;
                    }
                }
            }
        }
#endif
    }

    pkt_info.fwd_reason = arg_fwd_reason;

    if (LOG_CHECK(BSL_LS_APPL_COMMON | BSL_VERBOSE)) {
        cli_out("flags:  0x%08x\n", pkt_info.flags);
        for (i = 0; i < 8 * sizeof(pkt_info.flags); i++) {
            if (pkt_info.flags & (0x1 << i)) {
                if (i < COUNTOF(hd_flags_names)) {
                    cli_out("%s ", hd_flags_names[i]);
                } else {
                    cli_out("%s (%d) ", "UNKNOWN", i);
                }
            }
        }
        cli_out("\n");
    }

    if ((r = bcm_switch_pkt_info_hash_get(unit, &pkt_info, 
                 &dst_gport, &dst_intf)) < 0) {
        HASH_DESTINATION_ERROR_PRINT(r);
        retCode = CMD_FAIL;
        goto error_cleanup;
    }

#ifdef INCLUDE_L3
    if (pkt_info.flags & BCM_SWITCH_PKT_INFO_HASH_MULTIPATH) {
        cli_out("\n%s (%s):\n", 
                "HashDestination", 
                hd_resource_group_names[arg_group]);
        for (i = 0; i < intf_count; i++) {
#ifdef BCM_TOMAHAWK_SUPPORT
            if (soc_feature(unit, soc_feature_hierarchical_ecmp) &&
                BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit, mpath_egr[i])) {
                bcm_if_t *mpath_egr_l2 = NULL;
                int intf_count_l2 = 0, j;
                r = bcm_l3_egress_multipath_get(unit, mpath_egr[i], 0, NULL,
                                                &intf_count_l2);
                mpath_egr_l2 = sal_alloc((intf_count_l2 * sizeof(int)),
                                          "ecmp group level2");
                if (mpath_egr_l2 == NULL) {
                    r = BCM_E_MEMORY;
                    HASH_DESTINATION_ERROR_PRINT(r);
                    return CMD_FAIL;
                }
                r = bcm_l3_egress_multipath_get(unit, mpath_egr[i], intf_count_l2,
                                                mpath_egr_l2, &intf_count_l2);
                if (BCM_FAILURE(r)) {
                    HASH_DESTINATION_ERROR_PRINT(r);
                    sal_free(mpath_egr_l2);
                    retCode = CMD_FAIL;
                    goto error_cleanup;
                }
                for (j = 0; j < intf_count_l2; j++) {
                    cli_out("        %d %s\n", mpath_egr_l2[j],
                            mpath_egr_l2[j] == dst_intf ? "destination":"");
                }
                sal_free(mpath_egr_l2);
            } else
#endif
            {
                cli_out("        %d %s\n", mpath_egr[i],
                        mpath_egr[i] == dst_intf ? "destination":"");
            }
        }
    } else
#endif
    if (pkt_info.flags & BCM_SWITCH_PKT_INFO_HASH_TRUNK) {
        cli_out("\n%s (%s):\n", 
                "HashDestination", 
                hd_resource_group_names[arg_group]);

        for (i = 0; i < member_count; i++) {
            bcm_gport_t gport = member_array[i].gport;
            if (BCM_GPORT_IS_DEVPORT(gport)) {
                cli_out("        0x%08x (tp[%d]=%d,tdev[%d]=%d) %s\n",
                        gport, 
                        i, BCM_GPORT_DEVPORT_PORT_GET(gport), 
                        i, BCM_GPORT_DEVPORT_DEVID_GET(gport),
                        gport == dst_gport ? "destination":"");
            } else {
                cli_out("        0x%08x (tp[%d]=%d,tmod[%d]=%d) %s\n",
                        gport, 
                        i, BCM_GPORT_MODPORT_PORT_GET(gport), 
                        i, BCM_GPORT_MODPORT_MODID_GET(gport),
                        gport == dst_gport ? "destination":"");
            }
        }
    } else 
    if ((pkt_info.flags & BCM_SWITCH_PKT_INFO_HASH_UDP_SOURCE_PORT) ||
        (pkt_info.flags & BCM_SWITCH_PKT_INFO_HASH_LBID)) {
        cli_out("\n%s (%s):\n", 
                "HashDestination", 
                hd_resource_group_names[arg_group]);

        cli_out("        CalcInfo (%s):  %d(0x%x)\n", 
            hd_calc_info_names[arg_calc_info], dst_intf, dst_intf);
    } 

error_cleanup:
    if (member_array != NULL) {
        sal_free(member_array);
    }
#ifdef INCLUDE_L3
    if (mpath_egr != NULL) {
        sal_free(mpath_egr);
    }
#endif

    return retCode;
}


STATIC cmd_result_t
_cmd_hd_dest(int unit, args_t *a)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    /* BCM.0> hd dest get ... */
    if (!sal_strcasecmp(subcmd, "get") ||
        !sal_strcasecmp(subcmd, "calc")) {
        return _cmd_hd_dest_get(unit, a);
    }

    return CMD_USAGE;
}


cmd_result_t
cmd_hash_destination(int unit, args_t *a)
{
    char           *subcmd = NULL;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

#ifdef BCM_TRIUMPH3_SUPPORT
    if (!sal_strcasecmp(subcmd, "eth") ||
        !sal_strcasecmp(subcmd, "ethertype")) {
        /* BCM.0> hd eth ... */
        return _cmd_hd_ethertype(unit, a);
    }
#endif

    if (!sal_strcasecmp(subcmd, "dest") ||
        !sal_strcasecmp(subcmd, "info")) {
        /* BCM.0> hd dest ... */
        return _cmd_hd_dest(unit, a);
    }

    return CMD_USAGE;
}
#endif /* BCM_TRIDENT_SUPPORT || BCM_GREYHOUND2_SUPPORT */


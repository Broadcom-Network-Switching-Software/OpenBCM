/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MIM CLI commands
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <soc/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l3.h>
#include <bcm/mim.h>
#include <bcm/l2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/port.h>

#ifdef INCLUDE_L3
#if defined(BCM_TRIUMPH2_SUPPORT)

#define ENV_VPN_TYPE_ID "vpn_type_id"
#define ENV_MIM_TYPE_ID "mim_port_id"
#define ENV_MCAST_TYPE_ID "mcast_type_id"


enum _mim_tr2_cmd_t {
    MIM_INIT = 1,
    MIM_DETACH,
    MIM_VPN_CREATE,
    MIM_VPN_DESTROY,
    MIM_VPN_SHOW,
    MIM_VPN_PORT_ADD,
    MIM_VPN_PORT_DEL,
    MIM_TUNNEL_L2_ADD,
    MIM_MCAST_GROUP_CREATE,
    MIM_MCAST_GROUP_DEL,
    MIM_MCAST_GROUP_PORT_ADD,
    MIM_MCAST_GROUP_PORT_DEL,
    MIM_MCAST_GROUP_ADDR,
    MIM_OPTIONS
};

#define BCM_MIM_PORT                   1
#define BCM_MIM_PORT_MATCH             2
#define BCM_MIM_SWITCH                 3
#define BCM_MIM_EGRESS_LABEL           4
#define BCM_MIM_SWITCH_ACTION          5
#define MAX_FLAG_LENGTH               35 

#define MIM_ROE(op, arg)     \
    do {               \
        int __rv__;    \
        __rv__ = op arg;    \
        if (BCM_FAILURE(__rv__)) { \
            cli_out("MIM_CLI: Error: " #op " failed, %s\n", \
                    bcm_errmsg(__rv__)); \
            return CMD_FAIL; \
        } \
    } while(0)
#define MIM_CMD(_s)                         \
    if ((cmd = ARG_CUR(a)) == NULL) {        \
        ARG_PREV(a);  \
        cli_out("MIM_CLI: Error: Missing arg after %s\n", ARG_CUR(a)); \
        return CMD_USAGE;                    \
    } else if(sal_strcasecmp(cmd, _s) == 0)

#define PT_ADD(_pt, _id, _typ, _arg) \
    parse_table_add(&_pt, (_id), PQ_DFL | (_typ), \
                    (void *)(_arg), (void*)(_arg), 0)

#define PT_INIT(_u, _pt) parse_table_init(_u, &_pt); \

#define PT_DONE(_pt) parse_arg_eq_done(&pt);

#define PT_PARSE(_a, _pt) \
    do {     \
        if (parse_arg_eq(_a, &_pt) < 0) { \
            cli_out("MIM_CLI: Error: Invalid option or expression: %s\n", \
                    ARG_CUR(_a)); \
            PT_DONE(_pt); \
            return CMD_USAGE; \
        } \
    } while(0)

typedef struct _bcm_tr2_mim_flag_s {
    char      *name;
    uint32     val; 
} _bcm_tr2_mim_flag_t;

STATIC _bcm_tr2_mim_flag_t mim_port_flags[] = {
    {"RePLace", BCM_MIM_PORT_REPLACE},
    {"WithID", BCM_MIM_PORT_WITH_ID},
    {"DRop", BCM_MIM_PORT_DROP},
    {"Access", BCM_MIM_PORT_TYPE_ACCESS},
    {"BackBone", BCM_MIM_PORT_TYPE_BACKBONE},
    {"Peer", BCM_MIM_PORT_TYPE_PEER},
    {"MatchServiceVlanTpid", BCM_MIM_PORT_MATCH_SERVICE_VLAN_TPID},
    {"EgressTunnelServiceVlanReplace",  BCM_MIM_PORT_EGRESS_TUNNEL_SERVICE_REPLACE},
    {"EgressLabel", BCM_MIM_PORT_EGRESS_LABEL},
    {"EgressServiceVlanTagged",  BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TAGGED},
    {"EgressServiceVlanAdd", BCM_MIM_PORT_EGRESS_SERVICE_VLAN_ADD},
    {"EgressServiceVlanReplace", BCM_MIM_PORT_EGRESS_SERVICE_VLAN_REPLACE},
    {"EgressServiceVlanDelete", BCM_MIM_PORT_EGRESS_SERVICE_VLAN_DELETE},
    {"EgressServiceVlanTpidReplace",BCM_MIM_PORT_EGRESS_SERVICE_VLAN_TPID_REPLACE},
    {"EgressTunnelDestMacUseService", BCM_MIM_PORT_EGRESS_TUNNEL_DEST_MAC_USE_SERVICE},
    {NULL, 0}
};

STATIC _bcm_tr2_mim_flag_t mim_port_match_flags[] = {
    {"Port", BCM_MIM_PORT_MATCH_PORT},
    {"PortVlan", BCM_MIM_PORT_MATCH_PORT_VLAN},
    {"PortVlanStacked", BCM_MIM_PORT_MATCH_PORT_VLAN_STACKED},
    {"TunnelVlanSrcMac", BCM_MIM_PORT_MATCH_TUNNEL_VLAN_SRCMAC},
    {"Label", BCM_MIM_PORT_MATCH_LABEL},
    {NULL, 0}
};

int        _bcm_tr2_mim_cli_vpn_create(int unit, args_t * a);
int        _bcm_tr2_mim_cli_vpn_destroy(int unit, args_t * a);
int        _bcm_tr2_mim_cli_vpn_show(int unit, args_t * a);
int        _bcm_tr2_mim_cli_vpn_port_add(int unit, args_t * a);
int        _bcm_tr2_mim_cli_vpn_port_del(int unit, args_t * a);
int        _bcm_tr2_mim_cli_tunnel_l2_add(int unit, args_t * a);
int        _bcm_tr2_mim_cli_mcast_group_create(int unit, args_t * a);
int        _bcm_tr2_mim_cli_mcast_group_del(int unit, args_t * a);
int        _bcm_tr2_mim_cli_mcast_group_port_add_del(int unit, args_t * a);
int        _bcm_tr2_mim_cli_mcast_group_addr(int unit, args_t * a);
int        _bcm_tr2_mim_cli_print_options(int unit, args_t * a);
int        _bcm_tr2_mim_cli_action(int unit, args_t * a);
int        _bcm_tr2_mim_cli_parse_flags(char *elo_s, int type);


char       if_tr2_mim_usage[] = "Only for XGS4 devices.\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  mim <option> [args...]\n"
#else
    "  mim init\n\t"
    "        - Init MIM software system.\n\t"
    "  mim detach\n\t"
    "        - Detach MIM software system.\n\t"
    "  mim vpn create VPN=<vpn_id> BroadcastGroup=<bcast_id>\n\t"
    "                 UnknownMulticastGroup=<umc_id>\n\t"
    "                 UnknownUnicastGroup=<uuc_id>\n\t"
    "                 LookupId=<isid>\n\t"
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "                 Policer=<policer_id>\n\t"
#endif
    "        - Create vpn with the given id, the ISID, broadcast group,\n\t"
    "          unknown multicast group and unknown unicast group.\n\t"
    "  mim vpn port add VPN=<vpn_id> Port=<Mim_port> PhysPort=<port>\n\t"
    "                    PortMatch=<pm> IntfFlaGs=<flags> IntfCLass=<class>\n\t"
    "                    MatchVlan=<vid> MatchInnerVlan=<ivid>\n\t"
    "                    MatchLabel=<label> MatchTunnelVlan=<vlan>\n\t"
    "                    MatchTunnelSrcMAC=<mac> MatchServiceTPID=<tpid>\n\t"
    "                    EgressTunnelVlan=<vlan> EgressTunnelSrcMAC=<mac>\n\t"
    "                    EgressTunnelDstMAC=<mac> EgressTunnelService=<isid>\n\t"
    "                    EgressServiceTPID=<tpid> EgressServiceVID=<vid>\n\t"
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    "                    Policer=<policer_id>\n\t"
#endif
    "        - Add a port to VPN.\n\t"
    "  mim vpn port delete VPN=<vpn_id> MimPort=<mim_port_id>\n\t"
    "        - Delete an MIM port from vpn.\n\t"
    "  mim vpn destroy VPN=<vpn_id>\n\t"
    "        - Destroy a vpn.\n\t"
    "  mim vpn show VPN=<vpn_id>\n\t"
    "        - Show VPN info.\n\t"
    "  mim tunnel l2 add Mac=<mac> Vlan=<vlan>\n\t"
    "        - Add L2 address for MIM lookup.\n\t"
    "  mim mcast group create [l2|mim] McastGroup=<mcast_group>\n\t"
    "        - Create a multicast group.\n\t"
    "  mim mcast group delete McastGroup=<mcast_group>\n\t"
    "        - Delete a multicast group.\n\t"
    "  mim mcast group port add McastGroup=<mcast_type_id>\n\t"
    "                            MimPort=<mim_port_id>\n\t"
    "        - Add MIM port to multicast group.\n\t"
    "  mim mcast group port delete  McastGroup=<mcast_type_id>\n\t"
    "                                MimPort=<mim_port_id>\n\t"
    "        - Delete MIM port from multicast group\n\t"
    "  mim mcast group addr McastGroup=<mcast_type_id>\n\t"
    "                        Mac=<mcast mac> VPN=<vpn_type_id>\n\t"
    "        - Add multicast group Mac address.\n\t"
    "  mim options Type=[PORT|PortMatch]\n\t"
    "        - List various options/flags used in mim commands.\n\t" "\n"
#endif
          ;

cmd_result_t
if_tr2_mim(int unit, args_t * a)
{
    int        action = 0;
    cmd_result_t cmd_rv = CMD_OK;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (ARG_CUR(a) == NULL) {
        return CMD_USAGE;
    }

    action = _bcm_tr2_mim_cli_action(unit, a);

    switch (action) {
        case MIM_INIT:
            MIM_ROE(bcm_mim_init, (unit));
            break;
        case MIM_DETACH:
            MIM_ROE(bcm_mim_detach, (unit));
            break;
        case MIM_VPN_CREATE:
            cmd_rv = _bcm_tr2_mim_cli_vpn_create(unit, a);
            break;
        case MIM_VPN_DESTROY:
            cmd_rv = _bcm_tr2_mim_cli_vpn_destroy(unit, a);
            break;
        case MIM_VPN_SHOW:
            cmd_rv = _bcm_tr2_mim_cli_vpn_show(unit, a);
            break;
        case MIM_VPN_PORT_ADD:
            cmd_rv = _bcm_tr2_mim_cli_vpn_port_add(unit, a);
            break;
        case MIM_VPN_PORT_DEL:
            cmd_rv = _bcm_tr2_mim_cli_vpn_port_del(unit, a);
            break;
        case MIM_TUNNEL_L2_ADD:
            cmd_rv = _bcm_tr2_mim_cli_tunnel_l2_add(unit, a);
            break;
        case MIM_MCAST_GROUP_CREATE:
            cmd_rv = _bcm_tr2_mim_cli_mcast_group_create(unit, a);
            break;
        case MIM_MCAST_GROUP_DEL:
            cmd_rv = _bcm_tr2_mim_cli_mcast_group_del(unit, a);
            break;
        case MIM_MCAST_GROUP_PORT_ADD:
        case MIM_MCAST_GROUP_PORT_DEL:
            cmd_rv = _bcm_tr2_mim_cli_mcast_group_port_add_del(unit, a);
            break;
        case MIM_MCAST_GROUP_ADDR:
            cmd_rv = _bcm_tr2_mim_cli_mcast_group_addr(unit, a);
            break;
        case MIM_OPTIONS:
            cmd_rv = _bcm_tr2_mim_cli_print_options(unit, a);
            break;
        default:
            cli_out("Error:Unknown MIM command %d\n", action);
            return CMD_USAGE;
    }

    ARG_DISCARD(a);
    return cmd_rv;
}

int
_bcm_tr2_mim_cli_vpn_create(int unit, args_t * a)
{
    parse_table_t pt;
    int lkid, vpn_id = BCM_VLAN_INVALID;
    bcm_mim_vpn_config_t vpn_info;
    bcm_multicast_t temp, bcast_group = BCM_VLAN_INVALID;
    bcm_multicast_t umc_group = BCM_VLAN_INVALID;
    bcm_multicast_t uuc_group = BCM_VLAN_INVALID;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_policer_t policer_id=0;
#endif
    bcm_mim_vpn_config_t_init(&vpn_info);

    ARG_NEXT(a);
    if (ARG_CNT(a) < 5) {
        cli_out("MIM_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_ADD(pt, "LookupId", PQ_INT, &lkid);
    PT_ADD(pt, "BcastGroup", PQ_INT, &bcast_group);
    PT_ADD(pt, "UnknownMcastGroup", PQ_INT, &umc_group);
    PT_ADD(pt, "UnknownUcastGroup", PQ_INT, &uuc_group);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    PT_ADD(pt, "Policer", PQ_INT, &policer_id);
#endif
    PT_PARSE(a, pt);
    PT_DONE(pt);
    temp = bcast_group;
    MIM_ROE(bcm_multicast_create,
             (unit, BCM_MULTICAST_TYPE_MIM |
              BCM_MULTICAST_WITH_ID, &bcast_group));
    if (umc_group != temp) {
        MIM_ROE(bcm_multicast_create,
                 (unit, BCM_MULTICAST_TYPE_MIM |
                  BCM_MULTICAST_WITH_ID, &umc_group));    
    } else {
        umc_group = bcast_group;
    }
    if (uuc_group != temp) {
        MIM_ROE(bcm_multicast_create,
                 (unit, BCM_MULTICAST_TYPE_MIM |
                  BCM_MULTICAST_WITH_ID, &uuc_group));    
    } else {
        uuc_group = bcast_group;
    }
    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MIM_VPN_MIM | BCM_MIM_VPN_WITH_ID;
    vpn_info.lookup_id = lkid;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_unicast_group = uuc_group;
    vpn_info.unknown_multicast_group = umc_group;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    vpn_info.policer_id = policer_id;
#endif
    MIM_ROE(bcm_mim_vpn_create, (unit, &vpn_info));
    var_set_integer(ENV_VPN_TYPE_ID, vpn_info.vpn, TRUE, FALSE);
    cli_out("env var added : $vpn_type_id=0x%08x\n", vpn_info.vpn);
    var_set_integer(ENV_MCAST_TYPE_ID, bcast_group, TRUE, FALSE);
    cli_out("env var added : $bcast_type_id=0x%08x\n", bcast_group);
    var_set_integer(ENV_MCAST_TYPE_ID, umc_group, TRUE, FALSE);
    cli_out("env var added : $umc_type_id=0x%08x\n", umc_group);
    var_set_integer(ENV_MCAST_TYPE_ID, uuc_group, TRUE, FALSE);
    cli_out("env var added : $uuc_type_id=0x%08x\n", uuc_group);
    return CMD_OK;
}

int
_bcm_tr2_mim_cli_vpn_destroy(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MIM_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MIM_ROE(bcm_mim_vpn_destroy, (unit, vpn_id));
    cli_out("Destroyed vpn id=0x%08x\n", vpn_id);

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_vpn_show(int unit, args_t * a)
{
    int        i;
    int        count;
    int        max = 10;
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_mim_port_t port_array[10];
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_mim_vpn_config_t vpn_info;
    bcm_mim_vpn_config_t_init(&vpn_info);
#endif

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MIM_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MIM_ROE(bcm_mim_port_get_all, (unit, vpn_id, max,
                                     port_array, &count));
    cli_out("VPN id=0x%08x, Ports :%d\n", vpn_id, count);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    MIM_ROE(bcm_mim_vpn_get, (unit, vpn_id, &vpn_info));
    cli_out("Policer id=0x%x\n", vpn_info.policer_id);
#endif
    for (i = 0; i < count; i++) {
        cli_out("\n MIM Port ID: %x", port_array[i].mim_port_id);
        cli_out("\n flags: %x", port_array[i].flags);
        cli_out("\n criteria: %d", port_array[i].criteria);
        cli_out("\n port: %x", port_array[i].port);
        cli_out("\n match_vlan: %d", port_array[i].match_vlan);
        cli_out("\n match_inner_vlan: %d", port_array[i].match_inner_vlan);
        cli_out("\n match_label: %d", port_array[i].match_label);
        cli_out("\n encap_id: %d\n", port_array[i].encap_id);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        cli_out("\nPolicer id=0x%x\n", port_array[i].policer_id);
#endif
    }

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_vpn_port_add(int unit, args_t * a)
{
    parse_table_t pt;
    int vlan = BCM_VLAN_INVALID;
    int ivlan = BCM_VLAN_INVALID;
    int mtvlan = BCM_VLAN_INVALID;
    int etvlan = BCM_VLAN_INVALID;
    int esvid = BCM_VLAN_INVALID;
    bcm_port_t port = -1;
    bcm_port_t phys_port = -1;
    int        icl = 0;
    char      *pm_s = 0;
    uint32     pm = 0;
    char      *ifl_s = 0;
    uint32     ifl = 0;
    int       mstpid = 0x8100;
    int       estpid = 0x8100;
    bcm_mim_port_t mport;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_mac_t  mtsmac, etsmac, etdmac;
    int        elkid = 0;
    bcm_mpls_label_t label = 0;
    bcm_gport_t gp = BCM_GPORT_INVALID; 
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_policer_t policer_id=0;
#endif
   
    sal_memset(mtsmac, 0, sizeof(bcm_mac_t));
    sal_memset(etsmac, 0, sizeof(bcm_mac_t));
    sal_memset(etdmac, 0, sizeof(bcm_mac_t));
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_ADD(pt, "Port", PQ_INT, &port);
    PT_ADD(pt, "PhysPort", PQ_INT, &phys_port);
    PT_ADD(pt, "PortMatch", PQ_STRING, &pm_s);
    PT_ADD(pt, "IntfFlaGs", PQ_STRING, &ifl_s);
    PT_ADD(pt, "IntfCLass", PQ_INT, &icl);
    PT_ADD(pt, "MatchVlan", PQ_INT, &vlan);
    PT_ADD(pt, "MatchInnerVlan", PQ_INT, &ivlan);
    PT_ADD(pt, "MatchLabel", PQ_INT, &label);
    PT_ADD(pt, "MatchTunnelVlan", PQ_INT, &mtvlan);
    PT_ADD(pt, "MatchTunnelSrcMAC", PQ_MAC, mtsmac);
    PT_ADD(pt, "MatchServiceTPID", PQ_INT, &mstpid);
    PT_ADD(pt, "EgressTunnelVlan", PQ_INT, &etvlan);
    PT_ADD(pt, "EgressTunnelSrcMAC", PQ_MAC, etsmac);
    PT_ADD(pt, "EgressTunnelDstMAC", PQ_MAC, etdmac);
    PT_ADD(pt, "EgressTunnelService", PQ_INT, &elkid);
    PT_ADD(pt, "EgressServiceTPID", PQ_INT, &estpid);
    PT_ADD(pt, "EgressServiceVID", PQ_INT, &esvid);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    PT_ADD(pt, "Policer", PQ_INT, &policer_id);
#endif

    PT_PARSE(a, pt);

    if (pm_s) {
        pm = _bcm_tr2_mim_cli_parse_flags(pm_s, BCM_MIM_PORT_MATCH);
    }
    if (ifl_s) {
        ifl = _bcm_tr2_mim_cli_parse_flags(ifl_s, BCM_MIM_PORT);
    }
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "Adding port %d to vpn 0x%08x, PhysPort=%d, PortMatch=%s,"
                         "IntfFLaGs=%s MatchVlan=%d MatchInnerVlan=%d\n"
                         "MatchLabel=0x%x MatchTunnelVlan=%d icl=%d\n"
                         "MatchTunnelSrcMAC=%02x:%02x:%02x:%02x:%02x:%02x\n"
                         "MatchServiceTPID=%04x EgressTunnelVlan=%d\n"
                         "EgressTunnelSrcMAC=%02x:%02x:%02x:%02x:%02x:%02x\n"
                         "EgressTunnelDstMAC=%02x:%02x:%02x:%02x:%02x:%02x\n"
                         "EgressTunnelService=%d\n"
                         "EgressServiceTPID=%04x EgressServiceVID=%d\n"),
              port, vpn_id, phys_port, pm_s, ifl_s, vlan, ivlan,
              label, mtvlan, icl, mtsmac[0], mtsmac[1], mtsmac[2],
              mtsmac[3], mtsmac[4], mtsmac[5], mstpid, etvlan, etsmac[0],
              etsmac[1], etsmac[2], etsmac[3], etsmac[4], etsmac[5],
              etdmac[0], etdmac[1], etdmac[2], etdmac[3], etdmac[4],
              etdmac[5], elkid, estpid, esvid));

    PT_DONE(pt);
    bcm_mim_port_t_init(&mport);
    if (ifl & (BCM_MIM_PORT_REPLACE | BCM_MIM_PORT_WITH_ID)) {
        BCM_GPORT_MIM_PORT_ID_SET(mport.mim_port_id, port);
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "mim port=%08x\n"), port));
    }
    MIM_ROE(bcm_port_gport_get, (unit, phys_port, &gp));
    mport.port = gp;
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "gport_get gp=%08x\n"), gp));
    mport.criteria = pm;
    mport.flags = ifl;
    mport.if_class = icl;
    mport.match_vlan = vlan;
    mport.match_inner_vlan = ivlan;
    mport.match_label = label;
    mport.match_tunnel_vlan = mtvlan;
    memcpy(mport.match_tunnel_srcmac, mtsmac, sizeof(sal_mac_addr_t));
    mport.match_service_tpid = mstpid;
    mport.egress_tunnel_vlan = etvlan;
    memcpy(mport.egress_tunnel_srcmac, etsmac, sizeof(sal_mac_addr_t));
    memcpy(mport.egress_tunnel_dstmac, etdmac, sizeof(sal_mac_addr_t));
    mport.egress_tunnel_service = elkid;
    mport.egress_service_tpid = estpid;
    mport.egress_service_vlan = esvid;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    mport.policer_id = policer_id;
#endif
    MIM_ROE(bcm_mim_port_add, (unit, vpn_id, &mport));
    var_set_integer(ENV_MIM_TYPE_ID, mport.mim_port_id, TRUE, FALSE);
    cli_out("env var added : $mim_port_id=0x%08x\n", mport.mim_port_id);

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_vpn_port_del(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    ARG_NEXT(a);

    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_ADD(pt, "MimPort", PQ_INT, &gp);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MIM_ROE(bcm_mim_port_delete, (unit, vpn_id, gp));

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_tunnel_l2_add(int unit, args_t * a)
{
    bcm_mac_t  mac;
    parse_table_t pt;
    int        vlan = BCM_VLAN_INVALID;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "Mac", PQ_MAC, &mac);
    PT_ADD(pt, "Vlan", PQ_INT, &vlan);
    PT_PARSE(a, pt);
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "l2 add mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d\n"),
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vlan));
    PT_DONE(pt);
    MIM_ROE(bcm_l2_tunnel_add, (unit, mac, vlan));

    return CMD_OK;

}

int
_bcm_tr2_mim_cli_mcast_group_create(int unit, args_t * a)
{
    int        flags = -1;
    int        mcast_group = BCM_VLAN_INVALID;
    char      *cmd;
    parse_table_t pt;

    ARG_NEXT(a);                /* mim,l2 etc. */
    MIM_CMD("l2") {
        flags = BCM_MULTICAST_TYPE_L2;
    }
    MIM_CMD("mim") {
        flags = BCM_MULTICAST_TYPE_MIM;
    }
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_PARSE(a, pt);
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "mcast group %d\n"), mcast_group));
    flags |= BCM_MULTICAST_WITH_ID;
    PT_DONE(pt);
    MIM_ROE(bcm_multicast_create, (unit, flags, &mcast_group));
    var_set_integer(ENV_MCAST_TYPE_ID, mcast_group, TRUE, FALSE);
    cli_out("env var added : $mcast_type_id=0x%08x\n", mcast_group);

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_mcast_group_del(int unit, args_t * a)
{
    int        mcast_group = BCM_VLAN_INVALID;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MIM_ROE(bcm_multicast_destroy, (unit, mcast_group));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "mcast group %d destroyed\n"), mcast_group));

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_mcast_group_port_add_del(int unit, args_t * a)
{
    int        add = 0;
    int        mcast_group = BCM_VLAN_INVALID;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_trunk_t trunk_id;
    int        id;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    bcm_gport_t mim_port;
    bcm_if_t   encap_id;
    parse_table_t pt;
    char      *cmd;

    ARG_NEXT(a);
    MIM_CMD("add") {
        add = 1;
    }
    MIM_CMD("delete") {
        add = 0;
    }
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_ADD(pt, "MimPort", PQ_INT, &mim_port);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MIM_ROE(_bcm_esw_gport_resolve,
             (unit, mim_port, &modid, &port, &trunk_id, &id));
    MIM_ROE(bcm_port_gport_get, (unit, port, &gp));
    MIM_ROE(bcm_multicast_mim_encap_get,
             (unit, mcast_group, gp, mim_port, &encap_id));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "MIM_CLI: mim mcast group port %s mim"
                         "group 0x%x gport 0x%0x mim_port 0x%08xnh %d\n"),
              add ? "add" : "delete", mcast_group, gp, mim_port,
              encap_id));
    if (add) {
        MIM_ROE(bcm_multicast_egress_add,
                 (unit, mcast_group, gp, encap_id));
    } else {
        MIM_ROE(bcm_multicast_egress_delete,
                 (unit, mcast_group, gp, encap_id));
    }

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_mcast_group_addr(int unit, args_t * a)
{
    int        mcast_group = BCM_VLAN_INVALID;
    int        vpn;
    bcm_mac_t  mac;
    bcm_l2_addr_t l2_addr;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_ADD(pt, "Mac", PQ_MAC, mac);
    PT_ADD(pt, "VPN", PQ_INT, &vpn);
    PT_PARSE(a, pt);

    bcm_l2_addr_t_init(&l2_addr, mac, vpn);
    l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mcast_group;
    PT_DONE(pt);
    MIM_ROE(bcm_l2_addr_add, (unit, &l2_addr));
    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "mcast l2 addr add=%02x:%02x:%02x:%02x:%02x:%02x \n"),
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    cli_out("MIM_CLI: mc_group 0x%08x vpn 0x%08x\n", mcast_group, vpn);
    PT_DONE(pt);

    return CMD_OK;
}

int        
_bcm_tr2_mim_cli_print_options(int unit, args_t * a)
{
    int i;
    char     *type_s = 0;
    parse_table_t pt;
    _bcm_tr2_mim_flag_t *flags = 0;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "Type", PQ_STRING, &type_s);
    PT_PARSE(a, pt);

    if (parse_cmp("PortMatch", type_s, '\0')) {
        flags = mim_port_match_flags;
    } else if (parse_cmp(type_s, "PORT", '\0')) {
        flags = mim_port_flags;
    } 
    PT_DONE(pt);

    if (flags) {
        for (i = 0; flags[i].name != NULL; i++) {
            cli_out("\t%-25s  0x%08x\n", flags[i].name, flags[i].val);
        }
    } else {
        cli_out("Port Flags:\n\t");
        for (i = 0; mim_port_flags[i].name != NULL; i++) {
            cli_out("%-35s  0x%08x\n\t", mim_port_flags[i].name, 
                    mim_port_flags[i].val);
        }
        cli_out("\nPort Match Flags:\n\t");
        for (i = 0; mim_port_match_flags[i].name != NULL; i++) {
            cli_out("%-35s  0x%08x\n\t", mim_port_match_flags[i].name, 
                    mim_port_match_flags[i].val);
        }
        cli_out("\n");
    }

    return CMD_OK;
}

int
_bcm_tr2_mim_cli_action(int unit, args_t * a)
{
    char      *cmd;
    int        action = -1;

    MIM_CMD("init") {
        action = MIM_INIT;
    }

    MIM_CMD("detach") {
        action = MIM_DETACH;
    }

    MIM_CMD("options") {
        action = MIM_OPTIONS;
    }

    MIM_CMD("vpn") {
        ARG_NEXT(a);
        MIM_CMD("create") {
            action = MIM_VPN_CREATE;
        }
        MIM_CMD("destroy") {
            action = MIM_VPN_DESTROY;
        }
        MIM_CMD("show") {
            action = MIM_VPN_SHOW;
        }
        MIM_CMD("port") {
            ARG_NEXT(a);
            MIM_CMD("add") {
                action = MIM_VPN_PORT_ADD;
            }
            MIM_CMD("delete") {
                action = MIM_VPN_PORT_DEL;
            }
        }
    }

    MIM_CMD("tunnel") {
        ARG_NEXT(a);
        MIM_CMD("l2") {
            ARG_NEXT(a);
            MIM_CMD("add") {
                action = MIM_TUNNEL_L2_ADD;
            }
        }
    }

    MIM_CMD("mcast") {
        ARG_NEXT(a);
        MIM_CMD("group") {
            ARG_NEXT(a);
            MIM_CMD("create") {
                action = MIM_MCAST_GROUP_CREATE;
            }
            MIM_CMD("delete") {
                action = MIM_MCAST_GROUP_DEL;
            }
            MIM_CMD("port") {
                action = MIM_MCAST_GROUP_PORT_ADD;
            }
            MIM_CMD("addr") {
                action = MIM_MCAST_GROUP_ADDR;
            }
        }
    }

    return action;
}

int
_bcm_tr2_mim_cli_parse_flags(char *input_s, int type)
{
    int cnt;
    int len;
    char *p;
    char f_name[MAX_FLAG_LENGTH];
    _bcm_tr2_mim_flag_t *flags;
    uint32 result = 0;
    int done = 0;
    int found = 0;

    if (isint(input_s)) {
        result = parse_integer(input_s);
        return result;
    }

    switch (type) {
        case BCM_MIM_PORT:
            flags = mim_port_flags;
            break;
        case BCM_MIM_PORT_MATCH:
            flags = mim_port_match_flags;
            break;
        default:
            flags = 0;
    } /* switch (type) */

    if(!flags) {
        cli_out("MIM CLI: Error: Discarded unrecognized Flags\n\t %s\n", 
                input_s);
        return 0;
    }

    while (!done) {
        p = (char*)strcaseindex(input_s,",");
        sal_memset(f_name, 0, MAX_FLAG_LENGTH); 
        if (p) {
            strncpy(f_name, input_s, p-input_s);
            input_s = p + 1;
        } else {
            len = (sal_strlen(input_s) >= MAX_FLAG_LENGTH) ? 
                   (MAX_FLAG_LENGTH - 1) : sal_strlen(input_s);
            strncpy(f_name, input_s, len);
            done = 1;
        }
        found = 0;
        for (cnt = 0; flags[cnt].name ; cnt++) {
            if (parse_cmp(flags[cnt].name, f_name, '\0')) {
                result |= flags[cnt].val;
                found = 1;
            }
        }
        if (!found) {
            cli_out("MIM_CLI: flag %s not recognized, discarded\n", f_name);
        }
    }
    return result;

}
#endif /* BCM_TRIUMPH2_SUPPORT */
#endif /* INCLUDE_L3 */

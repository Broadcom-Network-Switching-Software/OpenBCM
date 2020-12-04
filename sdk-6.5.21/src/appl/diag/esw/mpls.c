/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MPLS CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#if defined(BCM_TRIUMPH_SUPPORT)
#include <soc/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/l3.h>
#include <bcm/mpls.h>
#include <bcm/l2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mpls.h>

#ifdef INCLUDE_L3

#if defined(BCM_TRIUMPH_SUPPORT)

#define ENV_VPN_TYPE_ID "vpn_type_id"
#define ENV_MPLS_TYPE_ID "mpls_port_id"
#define ENV_MCAST_TYPE_ID "mcast_type_id"


enum _mpls_tr_cmd_t {
    MPLS_INIT = 1,
    MPLS_CLEANUP,
    MPLS_VPN_CREATE_VPLS,
    MPLS_VPN_CREATE_VPWS,
    MPLS_VPN_CREATE_L3,
    MPLS_VPN_DESTROY,
    MPLS_VPN_SHOW,
    MPLS_VPN_PORT_ADD,
    MPLS_VPN_PORT_DEL,
    MPLS_TUNNEL_INIT_SET,
    MPLS_TUNNEL_INIT_CLEAR,
    MPLS_TUNNEL_SWITCH_ADD,
    MPLS_TUNNEL_SWITCH_DEL,
    MPLS_TUNNEL_L2_ADD,
    MPLS_EXPMAP_CREATE,
    MPLS_EXPMAP_DESTROY,
    MPLS_EXPMAP_SET,
    MPLS_EXPMAP_SHOW,
    MPLS_MCAST_GROUP_CREATE,
    MPLS_MCAST_GROUP_DEL,
    MPLS_MCAST_GROUP_PORT_ADD,
    MPLS_MCAST_GROUP_PORT_DEL,
    MPLS_MCAST_GROUP_ADDR,
    MPLS_OPTIONS
};

#define BCM_MPLS_PORT                   1
#define BCM_MPLS_PORT_MATCH             2
#define BCM_MPLS_SWITCH                 3
#define BCM_MPLS_EGRESS_LABEL           4
#define BCM_MPLS_SWITCH_ACTION          5
#define MAX_FLAG_LENGTH                 25

#define MPLS_ROE(op, arg)     \
    do {               \
        int __rv__;    \
        __rv__ = op arg;    \
        if (BCM_FAILURE(__rv__)) { \
            cli_out("MPLS_CLI: Error: " #op " failed, %s\n", \
                    bcm_errmsg(__rv__)); \
            return CMD_FAIL; \
        } \
    } while(0)
#define MPLS_CMD(_s)                         \
    if ((cmd = ARG_CUR(a)) == NULL) {        \
        ARG_PREV(a);  \
        cli_out("MPLS_CLI: Error: Missing arg after %s\n", ARG_CUR(a)); \
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
            cli_out("MPLS_CLI: Error: Invalid option or expression: %s\n", \
                    ARG_CUR(_a)); \
            PT_DONE(_pt); \
            return CMD_USAGE; \
        } \
    } while(0)

typedef struct _bcm_tr_mpls_flag_s {
    char      *name;
    uint32     val; 
} _bcm_tr_mpls_flag_t;

_bcm_tr_mpls_flag_t port_flags[] = {
    {"RePLace", BCM_MPLS_PORT_REPLACE},
    {"WithID", BCM_MPLS_PORT_WITH_ID},
    {"DRop", BCM_MPLS_PORT_DROP},
    {"NetWork", BCM_MPLS_PORT_NETWORK},
    {"ControlWord", BCM_MPLS_PORT_CONTROL_WORD},
    {"SEQuenced", BCM_MPLS_PORT_SEQUENCED},
    {"CouNTed",  BCM_MPLS_PORT_COUNTED},
    {"IntPriSet", BCM_MPLS_PORT_INT_PRI_SET},
    {"IntPriMap",  BCM_MPLS_PORT_INT_PRI_MAP},
    {"ColorMap", BCM_MPLS_PORT_COLOR_MAP},
    {"EgressTunnel", BCM_MPLS_PORT_EGRESS_TUNNEL},
    {"ServiceTagged", BCM_MPLS_PORT_SERVICE_TAGGED},
    {"ServiceVlanAdd",BCM_MPLS_PORT_SERVICE_VLAN_ADD},
    {"ServiceVlanReplace", BCM_MPLS_PORT_SERVICE_VLAN_REPLACE},
    {"ServiceVlanDelete",  BCM_MPLS_PORT_SERVICE_VLAN_DELETE},
    {"ServiceVlanTpidReplace", BCM_MPLS_PORT_SERVICE_VLAN_TPID_REPLACE},
    {"InnerVlanPreserve", BCM_MPLS_PORT_INNER_VLAN_PRESERVE},
    {NULL, 0}
};

_bcm_tr_mpls_flag_t port_match_flags[] = {
    {"Port", BCM_MPLS_PORT_MATCH_PORT},
    {"PortVlan", BCM_MPLS_PORT_MATCH_PORT_VLAN},
    {"PortVlanStacked", BCM_MPLS_PORT_MATCH_PORT_VLAN_STACKED},
    {"Label", BCM_MPLS_PORT_MATCH_LABEL},
    {"LabelPort", BCM_MPLS_PORT_MATCH_LABEL_PORT},
    {"LabelVlan", BCM_MPLS_PORT_MATCH_LABEL_VLAN},
    {NULL, 0}
};

_bcm_tr_mpls_flag_t switch_flags[] = {
    {"LookupInnerLabel", BCM_MPLS_SWITCH_LOOKUP_SECOND_LABEL},
    {"CouNTed", BCM_MPLS_SWITCH_COUNTED},
    {"IntPriSet", BCM_MPLS_SWITCH_INT_PRI_SET},
    {"IntPriMap", BCM_MPLS_SWITCH_INT_PRI_MAP},
    {"ColorMap", BCM_MPLS_SWITCH_COLOR_MAP},
    {"OUTerEXP", BCM_MPLS_SWITCH_OUTER_EXP},
    {"OUTerTTL", BCM_MPLS_SWITCH_OUTER_TTL},
    {"INnerEXP", BCM_MPLS_SWITCH_INNER_EXP},
    {"INnerTTL", BCM_MPLS_SWITCH_INNER_TTL},
    {"TTLDECrement", BCM_MPLS_SWITCH_TTL_DECREMENT},
    {NULL, 0},
};

_bcm_tr_mpls_flag_t egr_label_flags[] = {
    {"TTLSET", BCM_MPLS_EGRESS_LABEL_TTL_SET},
    {"TTLCoPy", BCM_MPLS_EGRESS_LABEL_TTL_COPY},
    {"TTLDECrement", BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT},
    {"EXPSET", BCM_MPLS_EGRESS_LABEL_EXP_SET},
    {"EXPREMark", BCM_MPLS_EGRESS_LABEL_EXP_REMARK},
    {"EXPCoPy", BCM_MPLS_EGRESS_LABEL_EXP_COPY},
    {"PRISET", BCM_MPLS_EGRESS_LABEL_PRI_SET},
    {"PRIREMark", BCM_MPLS_EGRESS_LABEL_PRI_REMARK},
    {NULL, 0}
};

_bcm_tr_mpls_flag_t action_flags[] = {
    {"SWAP", BCM_MPLS_SWITCH_ACTION_SWAP},
    {"PHP", BCM_MPLS_SWITCH_ACTION_PHP},
    {"POP", BCM_MPLS_SWITCH_ACTION_POP},
    {NULL, 0}
};


int        _bcm_tr_mpls_cli_vpn_create_vpws(int unit, args_t * a);
int        _bcm_tr_mpls_cli_vpn_create_vpls(int unit, args_t * a);
int        _bcm_tr_mpls_cli_vpn_create_l3(int unit, args_t * a);
int        _bcm_tr_mpls_cli_vpn_destroy(int unit, args_t * a);
int        _bcm_tr_mpls_cli_vpn_show(int unit, args_t * a);
int        _bcm_tr_mpls_cli_vpn_port_add(int unit, args_t * a);
int        _bcm_tr_mpls_cli_vpn_port_del(int unit, args_t * a);
int        _bcm_tr_mpls_cli_tunnel_init_set(int unit, args_t * a);
int        _bcm_tr_mpls_cli_tunnel_init_clear(int unit, args_t * a);
int        _bcm_tr_mpls_cli_tunnel_switch_add(int unit, args_t * a);
int        _bcm_tr_mpls_cli_tunnel_switch_del(int unit, args_t * a);
int        _bcm_tr_mpls_cli_tunnel_l2_add(int unit, args_t * a);
int        _bcm_tr_mpls_cli_expmap_create(int unit, args_t * a);
int        _bcm_tr_mpls_cli_expmap_destroy(int unit, args_t * a);
int        _bcm_tr_mpls_cli_expmap_set(int unit, args_t * a);
int        _bcm_tr_mpls_cli_expmap_show(int unit, args_t * a);
int        _bcm_tr_mpls_cli_mcast_group_create(int unit, args_t * a);
int        _bcm_tr_mpls_cli_mcast_group_del(int unit, args_t * a);
int        _bcm_tr_mpls_cli_mcast_group_port_add_del(int unit, args_t * a);
int        _bcm_tr_mpls_cli_mcast_group_addr(int unit, args_t * a);
int        _bcm_tr_mpls_cli_print_options(int unit, args_t * a);
int        _bcm_tr_mpls_cli_action(int unit, args_t * a);
int        _bcm_tr_mpls_cli_parse_flags(char *elo_s, int type);


char       if_tr_mpls_usage[] = "Only for XGS4 devices.\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "  mpls <option> [args...]\n"
#else
    "  mpls init\n\t"
    "        - Init MPLS software system.\n\t"
    "  mpls cleanup\n\t"
    "        - Detach MPLS software system.\n\t"
    "  mpls vpn create vpws VPN=<vpn_id>\n\t"
    "        - Create VPWS vpn with given id.\n\t"
    "  mpls vpn create l3 VPN=<vpn_id>\n\t"
    "        - Create L3 vpn with given id.\n\t"
#if !defined(BCM_KATANA_SUPPORT) || !defined(BCM_TRIUMPH3_SUPPORT)
    "  mpls vpn create vpls VPN=<vpn_id> BcastGroup=<bcast_id>\n\t"
    "        - Create VPLS vpn with given id  and broadcast group.\n\t"
    "  mpls vpn port add VPN=<vpn_id> Port=<port> PortMatch=<pm>\n\t"
    "                    IntfFlaGs=<flags> VCInitLabel=<vcinit_label>\n\t"
    "                    EgrLabelOptions=<labelopt>\n\t"
    "                    VCTermLabel=<vcterm_label> Vlan=<vlan>\n\t"
    "                    EgrTTL=<egress_ttl> QoSMapID=<qos_map_id>\n\t"
    "                    ServiceVID=<vlan> ServiceTPID=<tpid>\n\t"
    "                    IntfCLass=<val> EXPMapPtr=<exp_ptr> IntPRI=<pri>\n\t"
    "                    InnerVLAN=<vid> MTU=<val>EgrObj=<egress_obj_id>\n\t"
    "        - Add a port to VPN.\n\t"
#else
    "  mpls vpn create vpls VPN=<vpn_id> BcastGroup=<bcast_id>\n\t"
    "                    [Policer=<policer_id>]\n\t"
    "        - Create VPLS vpn with given id  and broadcast group.\n\t"
    "  mpls vpn port add VPN=<vpn_id> Port=<port> PortMatch=<pm>\n\t"
    "                    IntfFlaGs=<flags> VCInitLabel=<vcinit_label>\n\t"
    "                    EgrLabelOptions=<labelopt>\n\t"
    "                    VCTermLabel=<vcterm_label> Vlan=<vlan>\n\t"
    "                    EgrTTL=<egress_ttl> QoSMapID=<qos_map_id>\n\t"
    "                    ServiceVID=<vlan> ServiceTPID=<tpid>\n\t"
    "                    IntfCLass=<val> EXPMapPtr=<exp_ptr> IntPRI=<pri>\n\t"
    "                    InnerVLAN=<vid> MTU=<val>EgrObj=<egress_obj_id>\n\t"
    "                    Policer=<policer_id>\n\t"
    "        - Add a port to VPN.\n\t"
#endif /* BCM_KATANA_SUPPORT or BCM_TRIUMPH3_SUPPORT */
    "  mpls vpn port delete VPN=<vpn_id> MplsPort=<mpls_port_id>\n\t"
    "        - Delete an MPLS port from vpn.\n\t"
    "  mpls vpn destroy VPN=<vpn_id>\n\t"
    "        - Destroy a vpn.\n\t"
    "  mpls vpn show VPN=<vpn_id>\n\t"
    "        - Show VPN info.\n\t"
    "  mpls tunnel init set EgrTunnelLabel=<egr_tun_label>\n\t"
    "                       EgrLabelOptions=<egr_label_opt>\n\t"
    "                       EgrTTL=<egre_ttl> INtf=<intf_id>\n\t"
    "        - Initiate tunnel.\n\t"
    "  mpls tunnel init clear INtf=<intf_id>\n\t"
    "        - Clear tunnel.\n\t"
    "  mpls tunnel switch add SwitchOptions=<label_switch_opt>\n\t"
    "                         ACTion=<act> Port=<gport>\n\t"
    "                         EXPMapPtr=<exp_ptr> IntPRI=<pri>\n\t"
    "                         IngLabel=<label> IngInnerLabel=<inner_label>\n\t"
    "                         IngIntf=<ingress_interface>\n\t"
    "                         EgrLabel=<egr_label>\n\t"
    "                         EgrLabelOptions=<labelopt> EgrLabelTTL=<ttl>\n\t"
    "                         EgrLabelEXP=<exp>\n\t"
    "                         EgrObject=<egr_object_id> VPN=<vpn_id>\n\t"
    "        - Add ILM for ingress label.\n\t"
    "  mpls tunnel switch del IngLabel=<label>\n\t"
    "        - Delete ILM.\n\t"
    "  mpls tunnel l2 add Mac=<mac> Vlan=<vlan>\n\t"
    "        - Add L2 address for MPLS lookup.\n\t"
    "  mpls expmap create [ing|egr] ExpMapID=<id>\n\t"
    "        - Create EXP map.\n\t"
    "  mpls expmap destroy [ing|egr] ExpMapID=<id>\n\t"
    "        - Delete EXP map.\n\t"
    "  mpls expmap set [ing|egr] ExpMapID=<expid> PRIority=<pri>\n\t"
    "                            Color=<color> PKtPriority=<pri>\n\t"
    "                            EXP=<exp> PKtCfi=<cfi>\n\t"
    "        - Set exp map.\n\t"
    "  mpls expmap show [ing|egr] ExpMapID=<expid> PRIority=<pri>\n\t"
    "        - Show exp map.\n\t"
    "  mpls mcast group create [l2|vpls] McastGroup=<mcast_group>\n\t"
    "        - Create a multicast group.\n\t"
    "  mpls mcast group delete McastGroup=<mcast_group>\n\t"
    "        - Delete a multicast group.\n\t"
    "  mpls mcast group port add McastGroup=<mcast_type_id>\n\t"
    "                            MplsPort=<mpls_port_id>\n\t"
    "        - Add MPLS port to multicast group.\n\t"
    "  mpls mcast group port delete  McastGroup=<mcast_type_id>\n\t"
    "                                MplsPort=<mpls_port_id>\n\t"
    "        - Delete MPLS port from multicast group\n\t"
    "  mpls mcast group addr McastGroup=<mcast_type_id>\n\t"
    "                        Mac=<mcast mac> VPN=<vpn_type_id>\n\t"
    "        - Add multicast group Mac address.\n\t"
    "  mpls options Type=[PORT|PortMatch|MplsSwitch|MplsEgressLabel]\n\t"
    "        - List various options/flags used in mpls commands.\n\t" "\n"
#endif
          ;

cmd_result_t
if_tr_mpls(int unit, args_t * a)
{
    int        action = 0;
    cmd_result_t cmd_rv = CMD_OK;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (ARG_CUR(a) == NULL) {
        return CMD_USAGE;
    }

    action = _bcm_tr_mpls_cli_action(unit, a);

    switch (action) {
        case MPLS_INIT:
            MPLS_ROE(bcm_mpls_init, (unit));
            break;

        case MPLS_CLEANUP:
            MPLS_ROE(bcm_mpls_cleanup, (unit));
            break;

        case MPLS_VPN_CREATE_VPWS:
            cmd_rv = _bcm_tr_mpls_cli_vpn_create_vpws(unit, a);
            break;
        case MPLS_VPN_CREATE_VPLS:
            cmd_rv = _bcm_tr_mpls_cli_vpn_create_vpls(unit, a);
            break;
        case MPLS_VPN_CREATE_L3:
            cmd_rv = _bcm_tr_mpls_cli_vpn_create_l3(unit, a);
            break;
        case MPLS_VPN_DESTROY:
            cmd_rv = _bcm_tr_mpls_cli_vpn_destroy(unit, a);
            break;
        case MPLS_VPN_SHOW:
            cmd_rv = _bcm_tr_mpls_cli_vpn_show(unit, a);
            break;
        case MPLS_VPN_PORT_ADD:
            cmd_rv = _bcm_tr_mpls_cli_vpn_port_add(unit, a);
            break;
        case MPLS_VPN_PORT_DEL:
            cmd_rv = _bcm_tr_mpls_cli_vpn_port_del(unit, a);
            break;
        case MPLS_TUNNEL_INIT_SET:
            cmd_rv = _bcm_tr_mpls_cli_tunnel_init_set(unit, a);
            break;
        case MPLS_TUNNEL_INIT_CLEAR:
            cmd_rv = _bcm_tr_mpls_cli_tunnel_init_clear(unit, a);
            break;
        case MPLS_TUNNEL_SWITCH_ADD:
            cmd_rv = _bcm_tr_mpls_cli_tunnel_switch_add(unit, a);
            break;
        case MPLS_TUNNEL_SWITCH_DEL:
            cmd_rv = _bcm_tr_mpls_cli_tunnel_switch_del(unit, a);
            break;
        case MPLS_TUNNEL_L2_ADD:
            cmd_rv = _bcm_tr_mpls_cli_tunnel_l2_add(unit, a);
            break;
        case MPLS_EXPMAP_CREATE:
            cmd_rv = _bcm_tr_mpls_cli_expmap_create(unit, a);
            break;
        case MPLS_EXPMAP_DESTROY:
            cmd_rv = _bcm_tr_mpls_cli_expmap_destroy(unit, a);
            break;
        case MPLS_EXPMAP_SET:
            cmd_rv = _bcm_tr_mpls_cli_expmap_set(unit, a);
            break;
        case MPLS_EXPMAP_SHOW:
            cmd_rv = _bcm_tr_mpls_cli_expmap_show(unit, a);
            break;
        case MPLS_MCAST_GROUP_CREATE:
            cmd_rv = _bcm_tr_mpls_cli_mcast_group_create(unit, a);
            break;
        case MPLS_MCAST_GROUP_DEL:
            cmd_rv = _bcm_tr_mpls_cli_mcast_group_del(unit, a);
            break;
        case MPLS_MCAST_GROUP_PORT_ADD:
        case MPLS_MCAST_GROUP_PORT_DEL:
            cmd_rv = _bcm_tr_mpls_cli_mcast_group_port_add_del(unit, a);
            break;
        case MPLS_MCAST_GROUP_ADDR:
            cmd_rv = _bcm_tr_mpls_cli_mcast_group_addr(unit, a);
            break;
        case MPLS_OPTIONS:
            cmd_rv = _bcm_tr_mpls_cli_print_options(unit, a);
            break;
        default:
            cli_out("Error:Unknown MPLS command %d\n", action);
            return CMD_USAGE;
    }

    ARG_DISCARD(a);
    return cmd_rv;
}

int
_bcm_tr_mpls_cli_vpn_create_vpws(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_vpn_config_t_init(&vpn_info);

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MPLS_VPN_VPWS | BCM_MPLS_VPN_WITH_ID;
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_vpn_id_create, (unit, &vpn_info));
    var_set_integer(ENV_VPN_TYPE_ID, vpn_info.vpn, TRUE, FALSE);
    cli_out("env var added: $vpn_type_id=0x%08x\n", vpn_info.vpn);

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_vpn_create_vpls(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_policer_t policer_id=0;
#endif
    bcm_mpls_vpn_config_t vpn_info;
    bcm_multicast_t bcast_group = BCM_VLAN_INVALID;
    bcm_mpls_vpn_config_t_init(&vpn_info);

    ARG_NEXT(a);
#if !defined(BCM_KATANA_SUPPORT) || !defined(BCM_TRIUMPH3_SUPPORT)
    if (ARG_CNT(a) != 2) 
    {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
#else
    if (ARG_CNT(a) < 2) 
    {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
#endif
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_ADD(pt, "BcastGroup", PQ_INT, &bcast_group);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    PT_ADD(pt, "Policer", PQ_HEX, &policer_id);
#endif
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(bcm_multicast_create,
             (unit, BCM_MULTICAST_TYPE_VPLS |
              BCM_MULTICAST_WITH_ID, &bcast_group));
    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MPLS_VPN_VPLS | BCM_MPLS_VPN_WITH_ID;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    vpn_info.policer_id = policer_id;
#endif
    MPLS_ROE(bcm_mpls_vpn_id_create, (unit, &vpn_info));
    var_set_integer(ENV_VPN_TYPE_ID, vpn_info.vpn, TRUE, FALSE);
    cli_out("env var added : $vpn_type_id=0x%08x\n", vpn_info.vpn);

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_vpn_create_l3(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_vpn_config_t_init(&vpn_info);

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MPLS_VPN_L3 | BCM_MPLS_VPN_WITH_ID;
    MPLS_ROE(bcm_mpls_vpn_id_create, (unit, &vpn_info));
    var_set_integer(ENV_VPN_TYPE_ID, vpn_info.vpn, TRUE, FALSE);
    cli_out("env var added : $vpn_type_id=0x%08x\n", vpn_info.vpn);

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_vpn_destroy(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_vpn_id_destroy, (unit, vpn_id));
    cli_out("Destroyed vpn id=0x%08x\n", vpn_id);

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_vpn_show(int unit, args_t * a)
{
    int        i;
    int        count;
    int        max = 10;
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_mpls_port_t port_array[10];
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_mpls_vpn_config_t vpn_info;
    bcm_mpls_vpn_config_t_init(&vpn_info);
#endif

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_port_get_all, (unit, vpn_id, max,
                                     port_array, &count));
    cli_out("VPN id=0x%08x, Ports :%d\n", vpn_id, count);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    MPLS_ROE(bcm_mpls_vpn_id_get, (unit, vpn_id, &vpn_info));
    cli_out("Policer id=0x%x\n", vpn_info.policer_id);
#endif
    for (i = 0; i < count; i++) {
        cli_out("\n MPLS Port ID: %d", port_array[i].mpls_port_id);
        cli_out("\n Egress Tunnel If: %d", port_array[i].egress_tunnel_if);
        cli_out("\n flags: %X", port_array[i].flags);
        cli_out("\n service_tpid: %d", port_array[i].service_tpid);
        cli_out("\n match_vlan: %d", port_array[i].match_vlan);
        cli_out("\n mpls_label: %d", port_array[i].match_label);
        cli_out("\n encap_id: %d", port_array[i].encap_id);
        cli_out("\n failover_id: %d", port_array[i].failover_id);
        cli_out("\n failover_port_id: %d\n", port_array[i].failover_port_id);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        cli_out("Policer id=0x%x\n", port_array[i].policer_id);
#endif
    }

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_vpn_port_add(int unit, args_t * a)
{
    bcm_port_t port = -1;
    int        icl = 0;
    int        exp = 0;
    int        ipri = 0;
    int        ivlan = BCM_VLAN_INVALID;
    int        mtu = 0;
    char      *pm_s = 0;
    uint32     pm = 0;
    char      *ifl_s = 0;
    uint32     ifl = 0;
    uint32     vcil = 0;
    char      *elo_s = 0;
    uint32     elo = 0;
    uint32     vctl = 0;
    int        vlan = BCM_VLAN_INVALID;
    int        svid = BCM_VLAN_INVALID;
    int        stpid = 0x8100;
    uint32     ettl = 0;
    int        qosMapId=0;
    uint32     eo = 0;
    bcm_mpls_port_t mport;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    bcm_policer_t policer_id=0;
#endif

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_ADD(pt, "Port", PQ_PORT, &port);
    PT_ADD(pt, "PortMatch", PQ_STRING, &pm_s);
    PT_ADD(pt, "IntfFlaGs", PQ_STRING, &ifl_s);
    PT_ADD(pt, "VCInitLabel", PQ_INT, &vcil);
    PT_ADD(pt, "EgrLabelOptions", PQ_STRING, &elo_s);
    PT_ADD(pt, "VCTermLabel", PQ_INT, &vctl);
    PT_ADD(pt, "Vlan", PQ_INT, &vlan);
    PT_ADD(pt, "EgrTTL", PQ_INT, &ettl);
    PT_ADD(pt, "QoSMapID", PQ_INT, &qosMapId);
    PT_ADD(pt, "ServiceVID", PQ_INT, &svid);
    PT_ADD(pt, "ServiceTPID", PQ_INT, &stpid);
    PT_ADD(pt, "IntfCLass", PQ_INT, &icl);
    PT_ADD(pt, "EXPMapPtr", PQ_INT, &exp);
    PT_ADD(pt, "IntPRI", PQ_INT, &ipri);
    PT_ADD(pt, "InnerVLAN", PQ_INT, &ivlan);
    PT_ADD(pt, "MTU", PQ_INT, &mtu);
    PT_ADD(pt, "EgrObj", PQ_INT, &eo);
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    PT_ADD(pt, "Policer", PQ_INT, &policer_id);
#endif
    PT_PARSE(a, pt);

    if (pm_s) {
        pm = _bcm_tr_mpls_cli_parse_flags(pm_s, BCM_MPLS_PORT_MATCH);
    }
    if (elo_s) {
        elo = _bcm_tr_mpls_cli_parse_flags(elo_s, BCM_MPLS_EGRESS_LABEL);
    }
    if (ifl_s) {
        ifl = _bcm_tr_mpls_cli_parse_flags(ifl_s, BCM_MPLS_PORT);
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Adding port %d to vpn 0x%08x, PortMatch=%s,"
                            "IntfFLaGs=%s VCInitLabel=0x%x\n"
                            "EgrLabelOptions=%s elo=%x VCTermLabel=0x%x"
                            "Vlan=%d EgrTTL=%d QoSMapID=%d EgrObj=%d ServiceVID=%d\n"
                            "ServiceTPID=%04x icl=%d exp=%d ipri=%d"
                            "ivlan=%d mtu=%d\n"),
                 port, vpn_id, pm_s, ifl_s, vcil, elo_s,
                 elo, vctl, vlan, ettl, qosMapId, eo, svid, stpid,
                 icl, exp, ipri, ivlan, mtu));

    PT_DONE(pt);
    bcm_mpls_port_t_init(&mport);
    if (ifl & (BCM_MPLS_PORT_REPLACE | BCM_MPLS_PORT_WITH_ID)) {
        mport.mpls_port_id = port;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "mpls port=%08x\n"), port));
    } else if (BCM_GPORT_IS_SET(port)) {
        gp = port;
    } else {
        MPLS_ROE(bcm_port_gport_get, (unit, port, &gp));
    }
    mport.port = gp;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "gport_get gp=%08x\n"), gp));

    mport.criteria = pm;
    mport.flags = ifl;
    mport.if_class = icl;
    mport.exp_map = exp;
    mport.int_pri = ipri;
    mport.match_inner_vlan = ivlan;
    mport.mtu = mtu;
    mport.match_label = vctl;
    mport.match_vlan = vlan;
    mport.service_tpid = stpid;
    mport.egress_service_vlan = svid;
    mport.egress_tunnel_if = eo;
    mport.egress_label.label = vcil;
    mport.egress_label.flags = elo;
    mport.egress_label.ttl = ettl;
    mport.egress_label.qos_map_id = qosMapId;
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    mport.policer_id = policer_id;
#endif
    MPLS_ROE(bcm_mpls_port_add, (unit, vpn_id, &mport));
    var_set_integer(ENV_MPLS_TYPE_ID, mport.mpls_port_id, TRUE, FALSE);
    cli_out("env var added : $mpls_port_id=0x%08x\n", mport.mpls_port_id);

    return CMD_OK;

}

int
_bcm_tr_mpls_cli_vpn_port_del(int unit, args_t * a)
{
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    ARG_NEXT(a);

    PT_INIT(unit, pt);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_ADD(pt, "MplsPort", PQ_INT, &gp);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_port_delete, (unit, vpn_id, gp));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_tunnel_init_set(int unit, args_t * a)
{
    uint32     elo = 0;
    char      *elo_s = 0;
    uint32     etl = 0;
    uint32     ettl = 0;
    uint32     intf_id = 0;
    bcm_mpls_egress_label_t tunnel_label;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "EgrTunnelLabel", PQ_INT, &etl);
    PT_ADD(pt, "EgrLabelOptions", PQ_STRING, &elo_s);
    PT_ADD(pt, "EgrTTL", PQ_INT, &ettl);
    PT_ADD(pt, "INtf", PQ_INT, &intf_id);
    PT_PARSE(a, pt);
    if (elo_s) {
        elo = _bcm_tr_mpls_cli_parse_flags(elo_s, BCM_MPLS_EGRESS_LABEL);
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Adding INtf=0x%x EgrTunnelLabel=0x%x "
                            "EgrLabelOptions=%s elo=%d EgrTTL=%d\n"),
                 intf_id, etl, elo_s, elo, ettl));
    bcm_mpls_egress_label_t_init(&tunnel_label);
    tunnel_label.flags = elo;
    tunnel_label.label = etl;
    tunnel_label.ttl = ettl;
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_tunnel_initiator_set,
             (unit, intf_id, 1, &tunnel_label));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_tunnel_init_clear(int unit, args_t * a)
{
    parse_table_t pt;
    uint32     intf_id = 0;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "INtf", PQ_INT, &intf_id);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_tunnel_initiator_clear, (unit, intf_id));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Tunnel init clear intf_id=%d\n"), intf_id));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_tunnel_switch_add(int unit, args_t * a)
{
    uint32     so = 0;
    char      *so_s = 0;
    uint32     act = 0;
    char      *act_s = 0;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    int        exp_ptr = 0;
    int        ipri = 0;
    int        il = 0;
    int        iil = 0;
    int        el = 0;
    int        elo = 0;
    int        elttl = 0;
    int        elexp = 0;
    int        eo = 0;
    char      *elo_s = 0;
    bcm_mpls_tunnel_switch_t msw;
    parse_table_t pt;
    int        vpn_id = BCM_VLAN_INVALID;
    int        ing_intf = -1;
    int        mode = 0;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "SwitchOptions", PQ_STRING, &so_s);
    PT_ADD(pt, "ACTion", PQ_STRING, &act_s);
    PT_ADD(pt, "Port", PQ_PORT, &gp);
    PT_ADD(pt, "EXPMapPtr", PQ_INT, &exp_ptr);
    PT_ADD(pt, "IntPRI", PQ_INT, &ipri);
    PT_ADD(pt, "IngLabel", PQ_HEX, &il);
    PT_ADD(pt, "IngInnerLabel", PQ_HEX, &iil);
    PT_ADD(pt, "IngIntf", PQ_INT, &ing_intf);
    PT_ADD(pt, "EgrLabel", PQ_INT, &el);
    PT_ADD(pt, "EgrLabelOptions", PQ_STRING, &elo_s);
    PT_ADD(pt, "EgrLabelTTL", PQ_INT, &elttl);
    PT_ADD(pt, "EgrLabelEXP", PQ_INT, &elexp);
    PT_ADD(pt, "EgrObject", PQ_HEX, &eo);
    PT_ADD(pt, "VPN", PQ_INT, &vpn_id);
    PT_PARSE(a, pt);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "switch_add  so=0x%x so_s=%s act=%d il=0x%x "
                            "iil=0x%x el=0x%x elttl=%d elexp=%d elo_s=%s\n"
                            "elo=%x eo=%d gp=%x exp_ptr=%d ipri=%d\n"),
                 so, so_s, act, il, iil, el, elttl, elexp, elo_s,
                 elo, eo, gp, exp_ptr, ipri));

    if (act_s) {
        act = _bcm_tr_mpls_cli_parse_flags(act_s, BCM_MPLS_SWITCH_ACTION);
    }
    if (so_s) {
        so = _bcm_tr_mpls_cli_parse_flags(so_s, BCM_MPLS_SWITCH);
    }
    if (elo_s) {
        elo = _bcm_tr_mpls_cli_parse_flags(elo_s, BCM_MPLS_EGRESS_LABEL);
    }
    bcm_mpls_tunnel_switch_t_init(&msw);
    msw.flags = so;
    msw.action = act;
    msw.label = il;
    msw.port = gp;
    msw.exp_map = exp_ptr;
    msw.int_pri = ipri;
    msw.egress_label.label = el;
    msw.egress_label.flags = elo;
    msw.egress_label.ttl = elttl;
    msw.egress_label.exp = elexp;
    msw.egress_if = eo;
    msw.vpn = vpn_id;

    BCM_IF_ERROR_RETURN(bcm_switch_control_get(unit, 
                                               bcmSwitchL3IngressMode, 
                                               &mode));

    /* L3 Ingress Mode enabled */
    if (mode) {
        if (ing_intf == -1) {
            cli_out("L3IngressMode is set - "
                    "Invalid ingress interface(IngIntf)\n");
            return CMD_FAIL;
        }
        msw.ingress_if = ing_intf;
    } 

    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_tunnel_switch_add, (unit, &msw));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_tunnel_switch_del(int unit, args_t * a)
{
    int        il = 0;
    bcm_mpls_tunnel_switch_t msw;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "IngLabel", PQ_HEX, &il);
    PT_PARSE(a, pt);
    bcm_mpls_tunnel_switch_t_init(&msw);
    msw.label = il;
    msw.port = BCM_GPORT_INVALID;
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_tunnel_switch_delete, (unit, &msw));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_tunnel_l2_add(int unit, args_t * a)
{
    bcm_mac_t  mac;
    parse_table_t pt;
    int        vlan = BCM_VLAN_INVALID;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "Mac", PQ_MAC, &mac);
    PT_ADD(pt, "Vlan", PQ_INT, &vlan);
    PT_PARSE(a, pt);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "l2 add mac=%02x:%02x:%02x:%02x:%02x:%02x vlan=%d\n"),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], vlan));
    PT_DONE(pt);
    MPLS_ROE(bcm_l2_tunnel_add, (unit, mac, vlan));

    return CMD_OK;

}

int
_bcm_tr_mpls_cli_expmap_create(int unit, args_t * a)
{
    char      *cmd;
    int        flags = 0;
    int        exp_map_id = BCM_VLAN_INVALID;
    parse_table_t pt;

    ARG_NEXT(a);                /* ing/egr */
    MPLS_CMD("ing") {
        flags = BCM_MPLS_EXP_MAP_INGRESS;
    }
    MPLS_CMD("egr") {
        flags = BCM_MPLS_EXP_MAP_EGRESS;
    }
    ARG_NEXT(a);                /* ing/egr */
    PT_INIT(unit, pt);
    PT_ADD(pt, "ExpMapID", PQ_INT, &exp_map_id);
    PT_PARSE(a, pt);
    flags |= BCM_MPLS_EXP_MAP_WITH_ID;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "exp_map_id %d\n"), exp_map_id));
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_exp_map_create, (unit, flags, &exp_map_id));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_expmap_destroy(int unit, args_t * a)
{
    char      *cmd;
    int        type = 0;
    int        exp_map_id = BCM_VLAN_INVALID;
    parse_table_t pt;

    ARG_NEXT(a);                /* ing/egr */
    MPLS_CMD("ing") {
        type = _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;
    }
    MPLS_CMD("egr") {
        type = _BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS;
    }
    ARG_NEXT(a);                /* ing/egr */
    PT_INIT(unit, pt);
    PT_ADD(pt, "ExpMapID", PQ_INT, &exp_map_id);
    PT_PARSE(a, pt);
    exp_map_id |= type;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "exp_map_id %d\n"), exp_map_id));
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_exp_map_destroy, (unit, exp_map_id));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_expmap_set(int unit, args_t * a)
{
    char      *cmd;
    int        type = 0;
    int        color = 0;
    int        priority = 0;
    int        exp = 0;
    int        pkt_pri = 0;
    int        pkt_cfi = 0;
    int        exp_map_id = BCM_VLAN_INVALID;
    bcm_mpls_exp_map_t map;
    parse_table_t pt;

    ARG_NEXT(a);                /* ing/egr */
    MPLS_CMD("ing") {
        type = 0x100;/*_BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;*/
    }
    MPLS_CMD("egr") {
        type = 0;/*_BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS;*/
    }
    ARG_NEXT(a);                /* ing/egr */
    PT_INIT(unit, pt);
    PT_ADD(pt, "ExpMapID", PQ_INT, &exp_map_id);
    PT_ADD(pt, "PRIority", PQ_INT, &priority);
    PT_ADD(pt, "EXP", PQ_INT, &exp);
    PT_ADD(pt, "Color", PQ_INT, &color);
    PT_ADD(pt, "PKtPriority", PQ_INT, &pkt_pri);
    PT_ADD(pt, "PKtCfi", PQ_INT, &pkt_cfi);
    PT_PARSE(a, pt);
    exp_map_id |= type;
    bcm_mpls_exp_map_t_init(&map);
    map.priority = priority;
    map.exp = exp;
    map.color = color;
    map.pkt_pri = pkt_pri;
    map.pkt_cfi = pkt_cfi;
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "exp_map_id %d priority %d exp %d\n"
                            "Color %d PKtPriority %d PKtCfi %d\n"),
                 exp_map_id, priority, exp, color, pkt_pri, pkt_cfi));
    PT_DONE(pt);
    MPLS_ROE(bcm_mpls_exp_map_set, (unit, exp_map_id, &map));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_expmap_show(int unit, args_t * a)
{
    int        type = 0;
    int        exp = 0;
    int        priority = 0;
    int        color = 0;
    int        exp_map_id = BCM_VLAN_INVALID;
    bcm_mpls_exp_map_t map;
    char      *cmd;
    parse_table_t pt;

    ARG_NEXT(a);                /* ing/egr */
    MPLS_CMD("ing") {
        type = 0x100;/*_BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_INGRESS;*/
        ARG_NEXT(a);
        PT_INIT(unit, pt);
        PT_ADD(pt, "ExpMapID", PQ_INT, &exp_map_id);
        PT_ADD(pt, "EXP", PQ_INT, &exp);
        PT_PARSE(a, pt);
        exp_map_id |= type;
        bcm_mpls_exp_map_t_init(&map);
        map.exp = exp;
        PT_DONE(pt);
        MPLS_ROE(bcm_mpls_exp_map_get, (unit, exp_map_id, &map));
        cli_out("exp_map_id %d priority %d Color %d\n",
                exp_map_id, map.priority,map.color);
        cli_out("PKtPriority %d PKtCfi %d exp %d\n",
                map.pkt_pri, map.pkt_cfi, map.exp);
        return CMD_OK;
    }

    MPLS_CMD("egr") {
        type = 0;/*_BCM_TR_MPLS_EXP_MAP_TABLE_TYPE_EGRESS;*/
        ARG_NEXT(a);
        PT_INIT(unit, pt);
        PT_ADD(pt, "ExpMapID", PQ_INT, &exp_map_id);
        PT_ADD(pt, "PRIority", PQ_INT, &priority);
        PT_ADD(pt, "Color", PQ_INT, &color);
        PT_PARSE(a, pt);
        exp_map_id |= type;
        bcm_mpls_exp_map_t_init(&map);
        map.priority = priority;
        map.color = color;
        PT_DONE(pt);
        MPLS_ROE(bcm_mpls_exp_map_get, (unit, exp_map_id, &map));
        cli_out("exp_map_id %d priority %d Color %d\n",
                exp_map_id, map.priority, map.color);
        cli_out("PKtPriority %d PKtCfi %d exp %d\n",
                map.pkt_pri, map.pkt_cfi, map.exp);
    }

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_mcast_group_create(int unit, args_t * a)
{
    int        flags = -1;
    int        mcast_group = BCM_VLAN_INVALID;
    char      *cmd;
    parse_table_t pt;

    ARG_NEXT(a);                /* vpls,l2 etc. */
    MPLS_CMD("l2") {
        flags = BCM_MULTICAST_TYPE_L2;
    }
    MPLS_CMD("vpls") {
        flags = BCM_MULTICAST_TYPE_VPLS;
    }
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_PARSE(a, pt);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mcast group %d\n"), mcast_group));
    flags |= BCM_MULTICAST_WITH_ID;
    PT_DONE(pt);
    MPLS_ROE(bcm_multicast_create, (unit, flags, &mcast_group));
    var_set_integer(ENV_MCAST_TYPE_ID, mcast_group, TRUE, FALSE);
    cli_out("env var added : $mcast_type_id=0x%08x\n", mcast_group);

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_mcast_group_del(int unit, args_t * a)
{
    int        mcast_group = BCM_VLAN_INVALID;
    parse_table_t pt;

    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(bcm_multicast_destroy, (unit, mcast_group));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mcast group %d destroyed\n"),
                            mcast_group));

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_mcast_group_port_add_del(int unit, args_t * a)
{
    int        add = 0;
    int        mcast_group = BCM_VLAN_INVALID;
    bcm_module_t modid;
    bcm_port_t port;
    bcm_trunk_t trunk_id;
    int        id;
    bcm_gport_t gp = BCM_GPORT_INVALID;
    bcm_gport_t mpls_port;
    bcm_if_t   encap_id;
    parse_table_t pt;
    char      *cmd;

    ARG_NEXT(a);
    MPLS_CMD("add") {
        add = 1;
    }
    MPLS_CMD("delete") {
        add = 0;
    }
    ARG_NEXT(a);
    PT_INIT(unit, pt);
    PT_ADD(pt, "McastGroup", PQ_INT, &mcast_group);
    PT_ADD(pt, "MplsPort", PQ_INT, &mpls_port);
    PT_PARSE(a, pt);
    PT_DONE(pt);
    MPLS_ROE(_bcm_esw_gport_resolve,
             (unit, mpls_port, &modid, &port, &trunk_id, &id));
    MPLS_ROE(bcm_port_gport_get, (unit, port, &gp));
    MPLS_ROE(bcm_multicast_vpls_encap_get,
             (unit, mcast_group, gp, mpls_port, &encap_id));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MPLS_CLI: mpls mcast group port %s vpls"
                            "group 0x%x gport 0x%0x mpls_port 0x%08xnh %d\n"),
                 add ? "add" : "delete", mcast_group, gp, mpls_port,
                 encap_id));
    if (add) {
        MPLS_ROE(bcm_multicast_egress_add,
                 (unit, mcast_group, gp, encap_id));
    } else {
        MPLS_ROE(bcm_multicast_egress_delete,
                 (unit, mcast_group, gp, encap_id));
    }

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_mcast_group_addr(int unit, args_t * a)
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
    MPLS_ROE(bcm_l2_addr_add, (unit, &l2_addr));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "mcast l2 addr add=%02x:%02x:%02x:%02x:%02x:%02x \n"),
                 mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]));
    cli_out("MPLS_CLI: mc_group 0x%08x vpn 0x%08x\n", mcast_group, vpn);
    PT_DONE(pt);

    return CMD_OK;
}

int        
_bcm_tr_mpls_cli_print_options(int unit, args_t * a)
{
    int i;
    char     *type_s = 0;
    parse_table_t pt;
    _bcm_tr_mpls_flag_t *flags = 0;

    ARG_NEXT(a);
    if (ARG_CUR(a) == NULL) {
        cli_out("MPLS_CLI: Missing arguments\n");
        return CMD_USAGE;
    }
    PT_INIT(unit, pt);
    PT_ADD(pt, "Type", PQ_STRING, &type_s);
    PT_PARSE(a, pt);

    if (parse_cmp("PortMatch", type_s, '\0')) {
        flags = port_match_flags;
    } else if (parse_cmp(type_s, "PORT", '\0')) {
        flags = port_flags;
    } else if (parse_cmp("MplsSwitch", type_s, '\0')) {
        flags = switch_flags;
    } else if (parse_cmp("MplsEgressLabel", type_s, '\0')) {
        flags = egr_label_flags;
    } else if (parse_cmp("MplsSwitchAction", type_s, '\0')) {
        flags = action_flags;
    }
    PT_DONE(pt);

    if (flags) {
        for (i = 0; flags[i].name != NULL; i++) {
            cli_out("\t%-25s  0x%08x\n", flags[i].name, flags[i].val);
        }
    } else {
        cli_out("Port Flags:\n\t");
        for (i = 0; port_flags[i].name != NULL; i++) {
            cli_out("%-25s  0x%08x\n\t", port_flags[i].name, 
                    port_flags[i].val);
        }
        cli_out("\nPort Match Flags:\n\t");
        for (i = 0; port_match_flags[i].name != NULL; i++) {
            cli_out("%-25s  0x%08x\n\t", port_match_flags[i].name, 
                    port_match_flags[i].val);
        }
        cli_out("\nSwitch Flags:\n\t");
        for (i = 0; switch_flags[i].name != NULL; i++) {
            cli_out("%-25s  0x%08x\n\t", switch_flags[i].name, 
                    switch_flags[i].val);
        }
        cli_out("\nEgress Label Flags:\n\t");
        for (i = 0; egr_label_flags[i].name != NULL; i++) {
            cli_out("%-25s  0x%08x\n\t", egr_label_flags[i].name, 
                    egr_label_flags[i].val);
        }
        cli_out("\nSwitch Action Flags:\n\t");
        for (i = 0; action_flags[i].name != NULL; i++) {
            cli_out("%-25s  0x%08x\n\t", action_flags[i].name, 
                    action_flags[i].val);
        }
        cli_out("\n");
    }

    return CMD_OK;
}

int
_bcm_tr_mpls_cli_action(int unit, args_t * a)
{
    char      *cmd;
    int        action = -1;

    MPLS_CMD("init") {
        action = MPLS_INIT;
    }

    MPLS_CMD("cleanup") {
        action = MPLS_CLEANUP;
    }

    MPLS_CMD("options") {
        action = MPLS_OPTIONS;
    }

    MPLS_CMD("vpn") {
        ARG_NEXT(a);
        MPLS_CMD("create") {
            ARG_NEXT(a);
            MPLS_CMD("vpws") {
                action = MPLS_VPN_CREATE_VPWS;
            }
            MPLS_CMD("vpls") {
                action = MPLS_VPN_CREATE_VPLS;
            }
            MPLS_CMD("l3") {
                action = MPLS_VPN_CREATE_L3;
            }
        }
        MPLS_CMD("destroy") {
            action = MPLS_VPN_DESTROY;
        }
        MPLS_CMD("show") {
            action = MPLS_VPN_SHOW;
        }
        MPLS_CMD("port") {
            ARG_NEXT(a);
            MPLS_CMD("add") {
                action = MPLS_VPN_PORT_ADD;
            }
            MPLS_CMD("delete") {
                action = MPLS_VPN_PORT_DEL;
            }
        }
    }

    MPLS_CMD("tunnel") {
        ARG_NEXT(a);
        MPLS_CMD("init") {
            ARG_NEXT(a);
            MPLS_CMD("set") {
                action = MPLS_TUNNEL_INIT_SET;
            }
            MPLS_CMD("clear") {
                action = MPLS_TUNNEL_INIT_CLEAR;
            }
        }
        MPLS_CMD("switch") {
            ARG_NEXT(a);
            MPLS_CMD("add") {
                action = MPLS_TUNNEL_SWITCH_ADD;
            }
            MPLS_CMD("del") {
                action = MPLS_TUNNEL_SWITCH_DEL;
            }
        }
        MPLS_CMD("l2") {
            ARG_NEXT(a);
            MPLS_CMD("add") {
                action = MPLS_TUNNEL_L2_ADD;
            }
        }
    }

    MPLS_CMD("expmap") {
        ARG_NEXT(a);
        MPLS_CMD("create") {
            action = MPLS_EXPMAP_CREATE;
        }
        MPLS_CMD("destroy") {
            action = MPLS_EXPMAP_DESTROY;
        }
        MPLS_CMD("set") {
            action = MPLS_EXPMAP_SET;
        }
        MPLS_CMD("show") {
            action = MPLS_EXPMAP_SHOW;
        }
    }

    MPLS_CMD("mcast") {
        ARG_NEXT(a);
        MPLS_CMD("group") {
            ARG_NEXT(a);
            MPLS_CMD("create") {
                action = MPLS_MCAST_GROUP_CREATE;
            }
            MPLS_CMD("delete") {
                action = MPLS_MCAST_GROUP_DEL;
            }
            MPLS_CMD("port") {
                action = MPLS_MCAST_GROUP_PORT_ADD;
            }
            MPLS_CMD("addr") {
                action = MPLS_MCAST_GROUP_ADDR;
            }
        }
    }

    return action;
}

int
_bcm_tr_mpls_cli_parse_flags(char *input_s, int type)
{
    int cnt;
    char *p;
    char f_name[MAX_FLAG_LENGTH];
    _bcm_tr_mpls_flag_t *flags;
    uint32 result = 0;
    int done = 0;
    int found = 0;

    if (isint(input_s)) {
        result = parse_integer(input_s);
        return result;
    }

    switch (type) {
        case BCM_MPLS_PORT:
            flags = port_flags;
            break;
        case BCM_MPLS_PORT_MATCH:
            flags = port_match_flags;
            break;
        case BCM_MPLS_SWITCH:
            flags = switch_flags;
            break;
        case BCM_MPLS_EGRESS_LABEL:
            flags = egr_label_flags;
            break;
        case BCM_MPLS_SWITCH_ACTION:
            flags = action_flags;
            break;
        default:
            flags = 0;
    } /* switch (type) */

    if(!flags) {
        cli_out("MPLS CLI: Error: Discarded unrecognized Flags\n\t %s\n", 
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
    /*    coverity[parameter_as_source]    */
            strcpy(f_name, input_s);
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
            cli_out("MPLS_CLI: flag %s not recognized, discarded\n", f_name);
        }
    }
    return result;

}
#endif /* BCM_TRIUMPH_SUPPORT */
#endif /* INCLUDE_L3 */

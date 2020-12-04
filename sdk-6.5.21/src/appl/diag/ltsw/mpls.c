/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MPLS CLI commands
 */

#include <bsl/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/bslcons.h>
#include <appl/diag/ltsw/cmdlist.h>
#include <bcm/mpls.h>
#include <bcm/tunnel.h>
#include <bcm/nat.h>
#include <bcm/ipmc.h>
#include <bcm/stack.h>

#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/util.h>
#include <bcm_int/ltsw/mpls_int.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/tunnel.h>
#include <bcm_int/ltsw/feature.h>

#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <sal/sal_types.h>

typedef struct cmd_mpls_flag_s {
    char      *name;
    uint32     val;
} cmd_mpls_flag_t;

static cmd_mpls_flag_t port_flags[] = {
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
    {"", 0}
};

static cmd_mpls_flag_t port_match_flags[] = {
    {"Port", BCM_MPLS_PORT_MATCH_PORT},
    {"PortGroupVlan", BCM_MPLS_PORT_MATCH_PORT_GROUP_VLAN},
    {"PortGroupInnerVlan", BCM_MPLS_PORT_MATCH_PORT_GROUP_INNER_VLAN},
    {"PortGroupVlanStacked", BCM_MPLS_PORT_MATCH_PORT_GROUP_VLAN_STACKED},
    {"Label", BCM_MPLS_PORT_MATCH_LABEL},
    {"LabelPortGroup", BCM_MPLS_PORT_MATCH_LABEL_PORT_GROUP},
    {"", 0}
};

static cmd_mpls_flag_t switch_flags[] = {
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
    {"", 0},
};

static cmd_mpls_flag_t egr_label_flags[] = {
    {"TTLSET", BCM_MPLS_EGRESS_LABEL_TTL_SET},
    {"TTLCoPy", BCM_MPLS_EGRESS_LABEL_TTL_COPY},
    {"TTLDECrement", BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT},
    {"EXPSET", BCM_MPLS_EGRESS_LABEL_EXP_SET},
    {"EXPREMark", BCM_MPLS_EGRESS_LABEL_EXP_REMARK},
    {"EXPCoPy", BCM_MPLS_EGRESS_LABEL_EXP_COPY},
    {"PRISET", BCM_MPLS_EGRESS_LABEL_PRI_SET},
    {"PRIREMark", BCM_MPLS_EGRESS_LABEL_PRI_REMARK},
    {"", 0}
};

static cmd_mpls_flag_t action_flags[] = {
    {"SWAP", BCM_MPLS_SWITCH_ACTION_SWAP},
    {"PHP", BCM_MPLS_SWITCH_ACTION_PHP},
    {"POP", BCM_MPLS_SWITCH_ACTION_POP},
    {"", 0}
};

static cmd_mpls_flag_t tunnel_encap_flags[] = {
    {"Entropy", BCM_MPLS_TUNNEL_ENCAP_ENTROPY_ENABLE},
    {"QOSREMark", BCM_MPLS_TUNNEL_ENCAP_QOS_EXP_REMARK},
    {"", 0}
};


#define BCM_MPLS_PORT                   1
#define BCM_MPLS_PORT_MATCH             2
#define BCM_MPLS_SWITCH                 3
#define BCM_MPLS_EGRESS_LABEL           4
#define BCM_MPLS_SWITCH_ACTION          5
#define BCM_MPLS_TUNNEL_ENCAP           6

#define MAX_FLAG_LENGTH                 25

static cmd_result_t
cmd_ltsw_mpls_vpn_create_vpws(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_mpls_vpn_config_t vpn_info;
    int rv;
    cmd_result_t retcode;
    int vpn_id = BCM_VLAN_INVALID;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VPN", PQ_DFL | PQ_INT, 0, &vpn_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_mpls_vpn_config_t_init(&vpn_info);

    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MPLS_VPN_VPWS | BCM_MPLS_VPN_WITH_ID;
    rv = bcm_mpls_vpn_id_create(unit, &vpn_info);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating VPWS VPN: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_vpn_create_vpls(int unit, args_t *arg)
{
    parse_table_t pt;
    bcm_mpls_vpn_config_t vpn_info;
    int rv;
    cmd_result_t retcode;
    int vpn_id = BCM_VLAN_INVALID;
    bcm_multicast_t bcast_group = BCM_VLAN_INVALID;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VPN", PQ_DFL | PQ_INT, 0, &vpn_id, 0);
    parse_table_add(&pt, "BroadcastGroup", PQ_DFL | PQ_INT, 0, &bcast_group, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    rv = bcm_multicast_create(unit,
                              BCM_MULTICAST_TYPE_VPLS | BCM_MULTICAST_WITH_ID,
                              &bcast_group);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating Multicast group: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    bcm_mpls_vpn_config_t_init(&vpn_info);
    vpn_info.vpn = vpn_id;
    vpn_info.flags = BCM_MPLS_VPN_VPLS | BCM_MPLS_VPN_WITH_ID;
    vpn_info.broadcast_group = bcast_group;
    vpn_info.unknown_unicast_group = bcast_group;
    vpn_info.unknown_multicast_group = bcast_group;

    rv = bcm_mpls_vpn_id_create(unit, &vpn_info);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating VPLS VPN: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_mpls_vpn_create(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls vpn create subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "vpws") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_create_vpws(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "vpls") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_create_vpls(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_mpls_vpn_destroy(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    int vpn_id = BCM_VLAN_INVALID;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VPN", PQ_DFL | PQ_INT, 0, &vpn_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    rv = bcm_mpls_vpn_id_destroy(unit, vpn_id);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error destroying VPN: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_vpn_show(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    int vpn_id = BCM_VLAN_INVALID;
    int max = 10;
    bcm_mpls_port_t port_array[10];
    int count;
    int i;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VPN", PQ_DFL | PQ_INT, 0, &vpn_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    rv = bcm_mpls_port_get_all(unit, vpn_id, max,
                               port_array, &count);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error show VPN: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("VPN id=0x%08x, Ports :%d\n", vpn_id, count);

    for (i = 0; i < count; i++) {
        cli_out("\n MPLS Port ID: 0x%08x", port_array[i].mpls_port_id);
        cli_out("\n Match criteria: 0x%08x", port_array[i].criteria);
        cli_out("\n Egress Tunnel If: 0x%08x", port_array[i].egress_tunnel_if);
        cli_out("\n flags: 0x%08x", port_array[i].flags);
        cli_out("\n service_tpid: 0x%x", port_array[i].service_tpid);
        cli_out("\n match_vlan: 0x%x", port_array[i].match_vlan);
        cli_out("\n port_group: 0x%x", port_array[i].port_group);
        cli_out("\n port: 0x%x", port_array[i].port);
        cli_out("\n mpls_label: 0x%4x", port_array[i].match_label);
        cli_out("\n encap_id: 0x%08x", port_array[i].encap_id);
        cli_out("\n failover_id: 0x%08x", port_array[i].failover_id);
        cli_out("\n failover_port_id: 0x%08x\n", port_array[i].failover_port_id);
    }

    return CMD_OK;
}

int
cmd_ltsw_mpls_cli_parse_flags(char *input_s, int type)
{
    int cnt;
    char *p;
    char f_name[MAX_FLAG_LENGTH];
    cmd_mpls_flag_t *flags;
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
        case BCM_MPLS_TUNNEL_ENCAP:
            flags = tunnel_encap_flags;
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

static cmd_result_t
cmd_ltsw_mpls_vpn_port_add(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    int vpn_id = BCM_VLAN_INVALID;
    bcm_port_t port = -1;
    bcm_port_t mpls_port_id = -1;
    int        icl = 0;
    int        port_group = 0;
    int        exp = 0;
    int        ipri = 0;
    int        ivlan = BCM_VLAN_INVALID;
    int        mtu = 0;
    char      *pm_s = NULL;
    uint32     pm = 0;
    char      *pflags_str = NULL;
    uint32     port_flags = 0;
    uint32     vcil = 0;
    char      *elo_s = NULL;
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
    uint32     encap_id = 0;
    bcm_gport_t tunnel_id;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VPN", PQ_INT, 0, &vpn_id, 0);
    parse_table_add(&pt, "Port", PQ_PORT, 0, &port, 0);
    parse_table_add(&pt, "MplsPortId", PQ_INT, 0, &mpls_port_id, 0);
    parse_table_add(&pt, "PortGroup", PQ_INT, 0, &port_group, 0);
    parse_table_add(&pt, "PortMatch", PQ_STRING, NULL, &pm_s, 0);
    parse_table_add(&pt, "PortFlaGs", PQ_STRING, NULL, &pflags_str, 0);
    parse_table_add(&pt, "VCInitLabel", PQ_INT, 0, &vcil, 0);
    parse_table_add(&pt, "EgrLabelOptions", PQ_STRING, NULL, &elo_s, 0);
    parse_table_add(&pt, "VCTermLabel", PQ_INT, 0, &vctl, 0);
    parse_table_add(&pt, "Vlan", PQ_INT, 0, &vlan, 0);
    parse_table_add(&pt, "TunnelId", PQ_INT, 0, &tunnel_id, 0);
    parse_table_add(&pt, "EgrTTL", PQ_INT, 0, &ettl, 0);
    parse_table_add(&pt, "QoSMapID", PQ_INT, 0, &qosMapId, 0);
    parse_table_add(&pt, "ServiceVID", PQ_INT, 0, &svid, 0);
    parse_table_add(&pt, "ServiceTPID", PQ_INT, 0, &stpid, 0);
    parse_table_add(&pt, "IntfCLass", PQ_INT, 0, &icl, 0);
    parse_table_add(&pt, "EXPMapPtr", PQ_INT, 0, &exp, 0);
    parse_table_add(&pt, "IntPRI", PQ_INT, 0, &ipri, 0);
    parse_table_add(&pt, "InnerVLAN", PQ_INT, 0, &ivlan, 0);
    parse_table_add(&pt, "MTU", PQ_INT, 0, &mtu, 0);
    parse_table_add(&pt, "EgrObj", PQ_INT, 0, &eo, 0);
    parse_table_add(&pt, "EncapId", PQ_INT, 0, &encap_id, 0);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(arg, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }


    if (pm_s) {
        pm = cmd_ltsw_mpls_cli_parse_flags(pm_s, BCM_MPLS_PORT_MATCH);
    }
    if (elo_s ) {
        elo = cmd_ltsw_mpls_cli_parse_flags(elo_s, BCM_MPLS_EGRESS_LABEL);
    }
    if (pflags_str) {
        port_flags = cmd_ltsw_mpls_cli_parse_flags(pflags_str, BCM_MPLS_PORT);
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Adding port %d to vpn 0x%08x, PortMatch=%s,"
                            "PortFlaGs=%s VCInitLabel=0x%x\n"
                            "EgrLabelOptions=%s elo=%x VCTermLabel=0x%x"
                            "Vlan=%d EgrTTL=%d QoSMapID=%d EgrObj=%d ServiceVID=%d\n"
                            "ServiceTPID=%04x icl=%d exp=%d ipri=%d"
                            "ivlan=%d mtu=%d\n"),
                 port, vpn_id, pm_s, pflags_str, vcil, elo_s,
                 elo, vctl, vlan, ettl, qosMapId, eo, svid, stpid,
                 icl, exp, ipri, ivlan, mtu));

    bcm_mpls_port_t_init(&mport);

    if (BCM_GPORT_IS_SET(port)) {
        gp = port;
    } else {
        rv = bcm_port_gport_get(unit, port, &gp);
        if (BCM_FAILURE(rv)) {
            cli_out("%s: Error get gport from port: %s\n",
                ARG_CMD(arg), bcm_errmsg(rv));
            return (CMD_FAIL);
        }
    }

    if (BCM_GPORT_IS_MPLS_PORT(mpls_port_id)) {
        mport.mpls_port_id = mpls_port_id;
        mport.flags |= BCM_MPLS_PORT_WITH_ID;
    }
    mport.port = gp;
    mport.criteria = pm;
    mport.flags |= port_flags;
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
    mport.encap_id     = encap_id;
    mport.port_group   = port_group;
    mport.tunnel_id    = tunnel_id;
    rv = bcm_mpls_port_add(unit, vpn_id, &mport);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error add port to VPN: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_mpls_vpn_port_delete(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    int vpn_id = BCM_VLAN_INVALID;
    bcm_gport_t gp = BCM_GPORT_INVALID;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "VPN", PQ_DFL | PQ_INT, 0, &vpn_id, 0);
    parse_table_add(&pt, "MplsPort", PQ_DFL | PQ_INT, 0, &gp, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    rv = bcm_mpls_port_delete(unit, vpn_id, gp);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting MPLS port from VPN: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_vpn_port(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls vpn port subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_port_add(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_port_delete(unit, arg)) < 0) {
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
 *      cmd_ltsw_mpls_vpn
 * Description:
 *      Service routine used to manipulate MPLS VPNy.
 * Parameters:
 *      unit      - (IN) Device number.
 *      arg       - (IN) Command arguments.
 * Returns:
 *      CMD_XXX
 */
static cmd_result_t
cmd_ltsw_mpls_vpn(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls vpn subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_create(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "destroy") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_destroy(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_show(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "port") == 0) {
        if ((r = cmd_ltsw_mpls_vpn_port(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_mpls_tunnel_init_set(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    uint32     elo = 0;
    char      *elo_s = 0;
    uint32     etl = 0;
    uint32     ettl = 0;
    uint32     intf_id = 0;
    bcm_mpls_egress_label_t tunnel_label;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EgrTunnelLabel", PQ_INT, 0, &etl, 0);
    parse_table_add(&pt, "EgrLabelOptions", PQ_STRING, 0, &elo_s, 0);
    parse_table_add(&pt, "EgrTTL", PQ_INT, 0, &ettl, 0);
    parse_table_add(&pt, "INtf", PQ_INT, 0, &intf_id, 0);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(arg, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }


    if (elo_s) {
        elo = cmd_ltsw_mpls_cli_parse_flags(elo_s, BCM_MPLS_EGRESS_LABEL);
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

    rv = bcm_mpls_tunnel_initiator_set(unit, intf_id, 1, &tunnel_label);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error setting tunnel initiators: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_tunnel_init_clear(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    uint32     intf_id = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "INtf", PQ_INT, 0, &intf_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    rv = bcm_mpls_tunnel_initiator_clear(unit, intf_id);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error Clearing tunnel initiators: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_tunnel_init(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls vpn subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "set") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_init_set(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "clear") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_init_clear(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_mpls_tunnel_switch_add(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    int vpn_id = BCM_VLAN_INVALID;
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
    int        elttl = 0;
    int        elexp = 0;
    int        eo = 0;
    int        elo = 0;
    char      *elo_s = 0;
    bcm_mpls_tunnel_switch_t msw;
    int ing_intf = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "SwitchOptions", PQ_STRING, 0, &so_s, 0);
    parse_table_add(&pt, "ACTion", PQ_STRING, 0, &act_s, 0);
    parse_table_add(&pt, "Port", PQ_PORT, 0, &gp, 0);
    parse_table_add(&pt, "EXPMapPtr", PQ_INT, 0, &exp_ptr, 0);
    parse_table_add(&pt, "IntPRI", PQ_INT, 0, &ipri, 0);
    parse_table_add(&pt, "IngLabel", PQ_HEX, 0, &il, 0);
    parse_table_add(&pt, "IngInnerLabel", PQ_HEX, 0, &iil, 0);
    parse_table_add(&pt, "IngIntf", PQ_INT, 0, &ing_intf, 0);
    parse_table_add(&pt, "EgrLabel", PQ_INT, 0, &el, 0);
    parse_table_add(&pt, "EgrLabelOptions", PQ_STRING, 0, &elo_s, 0);
    parse_table_add(&pt, "EgrLabelTTL", PQ_INT, 0, &elttl, 0);
    parse_table_add(&pt, "EgrLabelEXP", PQ_INT, 0, &elexp, 0);
    parse_table_add(&pt, "EgrObject", PQ_HEX, 0, &eo, 0);
    parse_table_add(&pt, "VPN", PQ_INT, 0, &vpn_id, 0);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(arg, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }


    if (act_s) {
        act = cmd_ltsw_mpls_cli_parse_flags(act_s, BCM_MPLS_SWITCH_ACTION);
    }
    if (so_s) {
        so = cmd_ltsw_mpls_cli_parse_flags(so_s, BCM_MPLS_SWITCH);
    }
    if (elo_s) {
        elo = cmd_ltsw_mpls_cli_parse_flags(elo_s, BCM_MPLS_EGRESS_LABEL);
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

    rv = bcm_mpls_tunnel_switch_add(unit, &msw);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error adding MPLS tunnel switch: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_tunnel_switch_del(int unit, args_t *arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    bcm_mpls_tunnel_switch_t msw;
    int il = 0;

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "IngLabel", PQ_HEX, 0, &il, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_mpls_tunnel_switch_t_init(&msw);
    msw.label = il;
    msw.port = BCM_GPORT_INVALID;

    rv = bcm_mpls_tunnel_switch_delete(unit, &msw);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting MPLS tunnel switch: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}


static cmd_result_t
cmd_ltsw_mpls_tunnel_switch(int unit, args_t *arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls tunnel switch subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "add") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_switch_add(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "del") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_switch_del(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_mpls_tunnel_encap_create(int unit, args_t * arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    char      *egr_lable_flags_s = NULL;
    char      *encap_flag_s = NULL;
    uint32     egr_tunnel_label = 0;
    uint32     encap_id = 0;
    uint32     egr_ttl = 0;
    bcm_mpls_tunnel_encap_t tunnel_encap;
    uint32 encap_flags = 0;
    uint32 egr_label_flags = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EncapId", PQ_INT, 0, &encap_id, 0);
    parse_table_add(&pt, "EncapFlaGs", PQ_STRING, 0, &encap_flag_s, 0);
    parse_table_add(&pt, "EgrTunnelLabel", PQ_INT, 0, &egr_tunnel_label, 0);
    parse_table_add(&pt, "EgrLabelOptions", PQ_STRING, 0, &egr_lable_flags_s, 0);
    parse_table_add(&pt, "EgrTTL", PQ_INT, 0, &egr_ttl, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    if (egr_lable_flags_s) {
        egr_label_flags = cmd_ltsw_mpls_cli_parse_flags(egr_lable_flags_s,
                                                        BCM_MPLS_EGRESS_LABEL);
    }

    if (encap_flag_s) {
        encap_flags = cmd_ltsw_mpls_cli_parse_flags(encap_flag_s,
                                                    BCM_MPLS_TUNNEL_ENCAP);
    }

    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    tunnel_encap.tunnel_id = encap_id;
    tunnel_encap.flags = encap_flags;
    tunnel_encap.num_labels = 1;
    tunnel_encap.label_array[0].label = egr_tunnel_label;
    tunnel_encap.label_array[0].flags = egr_label_flags;
    tunnel_encap.label_array[0].ttl = egr_ttl;

    rv = bcm_mpls_tunnel_encap_create(unit,
                                      BCM_MPLS_TUNNEL_ENCAP_OPTIONS_WITH_ID,
                                      &tunnel_encap);

    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error creating tunnel encap: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_mpls_tunnel_encap_show(int unit, args_t * arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    uint32     encap_id = 0;
    bcm_mpls_tunnel_encap_t tunnel_encap;
    int i;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EncapId", PQ_INT, 0, &encap_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    bcm_mpls_tunnel_encap_t_init(&tunnel_encap);
    tunnel_encap.tunnel_id = encap_id;

    rv = bcm_mpls_tunnel_encap_get(unit, &tunnel_encap);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting tunnel encap: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    cli_out("\nTunnel ID: 0x%08x", tunnel_encap.tunnel_id);
    cli_out("\nFlags: 0x%08x", tunnel_encap.flags);
    cli_out("\nTunnel labels number: 0x%08x", tunnel_encap.num_labels);

    for (i = 0; i < tunnel_encap.num_labels; i++) {
        cli_out("\n    Label[%d]:", i);
        cli_out("  label: 0x%08x", tunnel_encap.label_array[i].label);
        cli_out("  flags: 0x%x", tunnel_encap.label_array[i].flags);
        cli_out("  ttl: 0x%x\n", tunnel_encap.label_array[i].ttl);
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_mpls_tunnel_encap_destroy(int unit, args_t * arg)
{
    parse_table_t pt;
    int rv;
    cmd_result_t retcode;
    uint32     encap_id = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "EncapId", PQ_INT, 0, &encap_id, 0);

    if(!parseEndOk(arg, &pt, &retcode)) {
        return retcode;
    }

    rv = bcm_mpls_tunnel_encap_destroy(unit, encap_id);
    if (BCM_FAILURE(rv)) {
        cli_out("%s: Error deleting tunnel encap: %s\n",
            ARG_CMD(arg), bcm_errmsg(rv));
        return (CMD_FAIL);
    }

    return CMD_OK;
}

static cmd_result_t
cmd_ltsw_mpls_tunnel_encap(int unit, args_t * arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls tunnel encap subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_encap_create(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "delete") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_encap_destroy(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "show") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_encap_show(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}


static cmd_result_t
cmd_ltsw_mpls_tunnel(int unit, args_t * arg)
{
    int r;
    char *subcmd;

    subcmd = ARG_GET(arg);
    if (subcmd == NULL) {
        cli_out("%s: ERROR: Missing mpls tunnel subcommand\n", ARG_CMD(arg));
        return CMD_FAIL;
    }

    if (sal_strcasecmp(subcmd, "init") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_init(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "switch") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_switch(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else if (sal_strcasecmp(subcmd, "encap") == 0) {
        if ((r = cmd_ltsw_mpls_tunnel_encap(unit, arg)) < 0) {
            goto bcm_err;
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;

bcm_err:
   return CMD_FAIL;
}

static cmd_result_t
cmd_ltsw_mpls_print_options(int unit, args_t * arg)
{
    char     *type_s = 0;
    parse_table_t pt;
    cmd_mpls_flag_t *flags = NULL;
    int i;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Type", PQ_STRING, "None", &type_s, NULL);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(arg, &pt)) {
        cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(arg), ARG_CUR(arg));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if (sal_strcasecmp("PortMatch", type_s) == 0) {
        flags = port_match_flags;
    } else if (sal_strcasecmp(type_s, "PORT") == 0) {
        flags = port_flags;
    } else if (sal_strcasecmp("MplsSwitch", type_s) == 0) {
        flags = switch_flags;
    } else if (sal_strcasecmp("MplsEgressLabel", type_s) == 0) {
        flags = egr_label_flags;
    } else if (sal_strcasecmp("MplsSwitchAction", type_s) == 0) {
        flags = action_flags;
    } else if (sal_strcasecmp("MplsTunnelEncap", type_s) == 0) {
        flags = tunnel_encap_flags;
    }

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
        cli_out("\nTunnel Encap Flags:\n\t");
        for (i = 0; tunnel_encap_flags[i].name != NULL; i++) {
            cli_out("%-25s  0x%08x\n\t", tunnel_encap_flags[i].name,
                    tunnel_encap_flags[i].val);
        }
        cli_out("\n");
    }

    return CMD_OK;
}

cmd_result_t
cmd_ltsw_mpls(int unit, args_t * arg)
{
    char *subcmd = NULL;
    int rv;

    if ((subcmd = ARG_GET(arg)) == NULL) {
        return CMD_USAGE;
    }

    if (!sal_strcasecmp(subcmd, "init")) {
        if ((rv = bcm_mpls_init(unit)) < 0) {
            cli_out("%s: error initializing: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        }
        return CMD_OK;
    } else if (!sal_strcasecmp(subcmd, "cleanup")) {
        if ((rv = bcm_mpls_cleanup(unit)) < 0) {
            cli_out("%s: error clean: %s\n", ARG_CMD(arg), bcm_errmsg(rv));
        }
        return CMD_OK;
    } else if (!sal_strcasecmp(subcmd, "vpn")) {
        return cmd_ltsw_mpls_vpn(unit, arg);
    } else if (!sal_strcasecmp(subcmd, "tunnel")) {
        return cmd_ltsw_mpls_tunnel(unit, arg);
    }  else if (!sal_strcasecmp(subcmd, "options")) {
        return cmd_ltsw_mpls_print_options(unit, arg);
    } else {
        cli_out("Unsupported command. \n");
        return CMD_OK;
    }

    return CMD_OK;
}

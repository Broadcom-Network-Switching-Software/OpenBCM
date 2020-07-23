/** \file diag_dnx_trap.c
 *
 * main file for trap diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "include/bcm_int/dnx/rx/rx_trap.h"
#include "diag_dnx_trap.h"
/** bcm */
#include <shared/bslnames.h>
#include <bcm/tunnel.h>
#include <bcm/l3.h>
/** sal */
#include <sal/appl/sal.h>
/*
 * }
 */
/*
 * Defines
 * {
 */
/*
 * }
 */
/*
 * External
 * {
 */

/*
 * }
 */

/*
 *  See diag_dnx_trap.h for info
 */
shr_error_e
diag_dnx_rx_trap_outrif_create_example(
    int unit,
    bcm_if_t * rif_p)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_l3_intf_t l3_intf;
    bcm_mac_t da = { 0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a };

    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_t_init(&l3eg);

    sal_memcpy(l3eg.mac_addr, da, sizeof(bcm_mac_t));
    l3eg.encap_id = 0;
    l3eg.vlan = 100;
    l3eg.flags = 0;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null));

    bcm_l3_intf_t_init(&l3_intf);

    /*
     * Create IP tunnel initiator for encapsulating GRE4oIPv4 tunnel header
     */
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip = 0xabcdef;
    tunnel_init.sip = 0xefcdab;
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type = bcmTunnelTypeGreAnyIn4;
    tunnel_init.ttl = 64;
    tunnel_init.l3_intf_id = l3eg.encap_id;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    SHR_IF_ERR_EXIT(bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel_init));

    *rif_p = l3_intf.l3a_intf_id;

exit:
    SHR_FUNC_EXIT;

}

/*
 *  See diag_dnx_trap.h for info
 */
shr_error_e
diag_dnx_rx_trap_rif_destroy_example(
    int unit,
    bcm_if_t * rif_p,
    bcm_rx_trap_lif_type_t rif_type)
{
    bcm_tunnel_terminator_t tunnel_term;
    bcm_l3_intf_t l3_intf;
    SHR_FUNC_INIT_VARS(unit);

    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = 0xabcdef;
    tunnel_term.dip_mask = 0xffffff;
    tunnel_term.sip = 0xefcdab;
    tunnel_term.sip_mask = 0xffffff;
    tunnel_term.vrf = 1;
    tunnel_term.type = bcmTunnelTypeGreAnyIn4;

    if (rif_type == bcmRxTrapLifTypeInRif)
    {
        SHR_IF_ERR_EXIT(bcm_tunnel_terminator_delete(unit, &tunnel_term));
    }
    else
    {
        bcm_l3_intf_t_init(&l3_intf);
        l3_intf.l3a_intf_id = *rif_p;
        SHR_IF_ERR_EXIT(bcm_tunnel_initiator_clear(unit, &l3_intf));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See diag_dnx_trap.h for info
 */
shr_error_e
diag_dnx_rx_trap_inlif_create_example(
    int unit,
    bcm_gport_t * lif_p)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    bcm_gport_t mpls_port_id_ingress = 9999;
    SHR_FUNC_INIT_VARS(unit);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_CROSS_CONNECT;
    mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, mpls_port_id_ingress);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.encap_id = 8888;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 3456;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, 50000);

    SHR_IF_ERR_EXIT(bcm_mpls_port_add(unit, 0, &mpls_port));

    *lif_p = mpls_port.mpls_port_id;

exit:
    SHR_FUNC_EXIT;

}

/*
 *  See diag_dnx_trap.h for info
 */
shr_error_e
diag_dnx_rx_trap_lif_destroy_example(
    int unit,
    bcm_gport_t * lif_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_mpls_port_delete(unit, 0, *lif_p));

exit:
    SHR_FUNC_EXIT;
}

void
dnx_egress_trap_id_to_string(
    int trap_id,
    char *trap_id_str)
{

    if ((trap_id == BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT) || (trap_id == BCM_RX_TRAP_EG_TRAP_ID_DEFAULT) || (trap_id == 0))
    {
        sal_strncpy(trap_id_str, "DEFAULT", DIAG_DNX_TRAP_ID_STRING_SIZE);
    }
    else if (trap_id == BCM_RX_TRAP_EG_TX_TRAP_ID_DROP || (trap_id == 1))
    {
        sal_strncpy(trap_id_str, "DROP", DIAG_DNX_TRAP_ID_STRING_SIZE);
    }
    else
    {
        sal_snprintf(trap_id_str, DIAG_DNX_TRAP_ID_STRING_SIZE, "%d", trap_id);
    }

}

const sh_sand_enum_t Trap_block_enum_table[] = {
    /**String      Value                                      Description*/
    {"IRPP", DNX_RX_TRAP_BLOCK_INGRESS, "Ingress block"}
    ,
    {"ERPP", DNX_RX_TRAP_BLOCK_ERPP, "ERPP block"}
    ,
    {"ETPP", DNX_RX_TRAP_BLOCK_ETPP, "ETPP block"}
    ,
    {"ALL", DNX_RX_TRAP_BLOCK_NUM_OF, "All blocks"}
    ,
    {NULL}
};

static sh_sand_man_t sh_dnx_trap_list_man = {
    .brief = "List traps per different parameters",
    .full = "List trap per type and block\n",
};

static sh_sand_man_t sh_dnx_trap_map_man = {
    .brief = "Map info of traps per different parameters",
    .full = "Map info of trap per type and block\n",
};

static sh_sand_man_t sh_dnx_trap_action_man = {
    .brief = "Show trap's action info",
    .full = "Show trap's action info per block and trap_id\n",
};

static sh_sand_man_t sh_dnx_trap_prog_man = {
    .brief = "Show programmable traps info",
    .full = "Show programmable trap info per index\n",
};

static sh_sand_man_t sh_dnx_trap_protocol_man = {
    .brief = "Show protocol traps info",
    .full = "Show protocol trap info per type, profile and args\n",
};

static sh_sand_man_t sh_dnx_trap_last_pkt_man = {
    .brief = "Show last packet trap info",
    .full = "Show last packet trap info per block\n",
};

/**
 * \brief DNX Traps diagnostics
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for Traps diagnostic commands
 */
sh_sand_cmd_t sh_dnx_trap_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"LIST", NULL, sh_dnx_trap_list_cmds, NULL, &sh_dnx_trap_list_man}
    ,
    {"MAPping", NULL, sh_dnx_trap_map_cmds, NULL, &sh_dnx_trap_map_man}
    ,
    {"ACTion", NULL, sh_dnx_trap_action_cmds, NULL, &sh_dnx_trap_action_man}
    ,
    {"PRoGram", NULL, sh_dnx_trap_prog_cmds, NULL, &sh_dnx_trap_prog_man}
    ,
    {"PRoTocol", NULL, sh_dnx_trap_protocol_cmds, NULL, &sh_dnx_trap_protocol_man}
    ,
    {"MTU", NULL, sh_dnx_trap_mtu_cmds, NULL, &sh_dnx_trap_mtu_cmd_man}
    ,
    {"LIF", sh_dnx_trap_lif_cmd, NULL, NULL, &sh_dnx_trap_lif_cmd_man}
    ,
    {"LAST", NULL, sh_dnx_trap_last_pkt_cmds, NULL, &sh_dnx_trap_last_pkt_man}
    ,
    {"SVTag", sh_dnx_trap_svtag_cmd, NULL, NULL, &sh_dnx_trap_svtag_cmd_man}
    ,
    {NULL}
};

sh_sand_man_t sh_dnx_trap_man = {
    .brief = "trap commands",
};

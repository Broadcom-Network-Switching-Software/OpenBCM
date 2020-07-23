/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/** \file diag_dnx_pp_rpf.c
 *
 * RPF PP Diagnostic Command.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include "diag_dnx_pp.h"
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/failover/failover.h>
#include <soc/dnx/dbal/dbal_string_apis.h>
#include <bcm_int/dnx/instru/instru_visibility.h>
/*************
 * DEFINES   *
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGPPDNX

#define DIAG_DNX_PP_VIS_RPF_NOF_COLUMNS  10

/*************
 *  MACROS  *
 */

#define L3_EGRESS_GET_MC_RPF_TYPE(flags) ((flags & BCM_L3_MC_RPF_EXPLICIT) == BCM_L3_MC_RPF_EXPLICIT) ? DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT : \
        ((flags & BCM_L3_MC_RPF_EXPLICIT_ECMP) == BCM_L3_MC_RPF_EXPLICIT_ECMP) ? DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT_ECMP : \
        ((flags & BCM_L3_MC_NO_RPF) == BCM_L3_MC_NO_RPF) ? DBAL_ENUM_FVAL_MC_RPF_MODE_DISABLE :\
        ((flags & BCM_L3_MC_RPF_SIP_BASE) == BCM_L3_MC_RPF_SIP_BASE) ? DBAL_ENUM_FVAL_MC_RPF_MODE_USE_SIP : DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED

#define FEC_NOF_HIERARCHY  DBAL_ENUM_FVAL_HIERARCHY_LEVEL_NO_HIERARCHY
/*************
 * TYPEDEFS  *
 */

/*************
 * GLOBALS   *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief
 * Callback function for configuring and sending a packet in order to test rpf diagnostic.
 */
static shr_error_e
diag_dnx_pp_vis_rpf_config_cb(
    int unit)
{
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t match_info;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;
    bcm_l3_intf_t l3if;
    bcm_l3_ingress_t l3_ing_if;
    bcm_l3_host_t host;
    bcm_mac_t my_mac = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
    bcm_mac_t out_mac = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };
    bcm_mac_t next_hop = { 0x00, 0x0c, 0x00, 0x05, 0x00, 0x00 };
    bcm_switch_fec_property_config_t fec_config;
    int in_port = 200;
    int out_port = 201;
    int vlan = 100;
    int vlan_out = 250;
    int vrf = 1;
    int fec_id;
    int fec_rpf_strict;
    int encap_id = 0x1234;
    uint32 in_ttl = 100;
    uint32 sip = 0x80202020;
    uint32 dip = 0x7fffff01;
    uint32 flags;
    char *my_mac_str = "00:0c:00:02:00:00";
    char *sa_mac_str = "00:00:07:00:01:00";
    rhhandle_t packet_h = NULL;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;

    SHR_FUNC_INIT_VARS(unit);

    fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
    SHR_IF_ERR_EXIT(bcm_switch_fec_property_get(unit, &fec_config));

    fec_id = fec_config.start;
    fec_rpf_strict = fec_id + 2;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vlan;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));
    SHR_IF_ERR_EXIT(bcm_vlan_gport_add(unit, vlan, in_port, 0));

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, my_mac, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = l3if.l3a_vid = vlan;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;   /* must, as we update exist RIF */
    l3_ing_if.vrf = vrf;
    l3_ing_if.urpf_mode = bcmL3IngressUrpfStrict;
    SHR_IF_ERR_EXIT(bcm_l3_ingress_create(unit, &l3_ing_if, &vlan));

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vlan_port));

    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    match_info.port = out_port;
    SHR_IF_ERR_EXIT(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info));

    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = vlan;
    l3eg.port = out_port;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &fec_rpf_strict));

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    sal_memcpy(l3if.l3a_mac_addr, out_mac, sizeof(bcm_mac_t));
    l3if.l3a_intf_id = l3if.l3a_vid = vlan_out;
    SHR_IF_ERR_EXIT(bcm_l3_intf_create(unit, &l3if));

    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf = vlan_out;
    l3eg.encap_id = encap_id;
    l3eg.port = out_port;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &fec_id));

    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, next_hop, sizeof(bcm_mac_t));
    l3eg.encap_id = encap_id;
    l3eg.vlan = vlan_out;
    SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid_null));

    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = dip;
    host.l3a_vrf = vrf;
    host.l3a_intf = fec_id;
    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host));

    bcm_l3_host_t_init(&host);
    host.l3a_ip_addr = sip;
    host.l3a_vrf = vrf;
    host.l3a_intf = fec_rpf_strict;
    SHR_IF_ERR_EXIT(bcm_l3_host_add(unit, &host));

    SHR_IF_ERR_EXIT(bcm_port_get(unit, in_port, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(diag_sand_packet_create(unit, &packet_h));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "PTCH_2"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "PTCH_2.PP_SSP", &mapping_info.pp_port, 16));
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "ETH1"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.DA", my_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_str(unit, packet_h, "ETH1.SA", sa_mac_str));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "ETH1.VLAN.VID", (uint32 *) &vlan, 12));
    /*
     * Add IPv4 header.
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_proto_add(unit, packet_h, "IPv4"));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.SIP", &sip, 32));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.DIP", &dip, 32));
    SHR_IF_ERR_EXIT(diag_sand_packet_field_add_uint32(unit, packet_h, "IPv4.TTL", &in_ttl, 8));

    /*
     * Send packet twice, to avoid learning signals
     * Use RESUME flag - to clean signals before sending the packet
     */
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_h, SAND_PACKET_RX | SAND_PACKET_RESUME));
    SHR_IF_ERR_EXIT(diag_sand_packet_send(unit, in_port, packet_h, SAND_PACKET_RX | SAND_PACKET_RESUME));

exit:
    sal_free(packet_h);
    SHR_FUNC_EXIT;
}

sh_sand_man_t dnx_pp_vis_rpf_man = {
    .brief = "Last packet hierarchical data",
    .full = "Present information for each hierarchy on RPF selection.",
    .init_cb = diag_dnx_pp_vis_rpf_config_cb,
    .synopsis = "[core=<core_id>]",
    .examples = "\n" "core=0"
};

/**
 * \brief
 *    Retrieve the data for a FEC in a particular hierarchy needed for a single row in the table.
 * \param [in] unit - unit ID.
 * \param [in] fec_id - Index of the FEC.
 * \param [in] hierarchy_id - Zero-based value of the hierarchy ID.
 * \param [out] destination - Destination which is result of the current FEC.
 * \param [out] out_lif - Global outlif of the FEC.
 * \param [out] out_lif_2nd - Global outlif second of the FEC.
 * \param [out] multicast_rpf_type - RPF Type for multicast.
 * \param [out] fail_id - Protection pointer.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
dnx_pp_rpf_add_fec_info(
    int unit,
    uint32 *fec_id,
    uint32 hierarchy_id,
    uint32 *destination,
    uint32 *out_lif,
    uint32 *out_lif_2nd,
    uint32 *multicast_rpf_type,
    uint32 *fail_id)
{
    int fail_enabled = 0;
    bcm_l3_egress_t egr_entry;
    SHR_FUNC_INIT_VARS(unit);

    bcm_l3_egress_t_init(&egr_entry);

    /*
     *  Get the FEC entry
     */
    SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, *fec_id, &egr_entry));
    if (egr_entry.failover_id != 0)
    {
        SHR_IF_ERR_EXIT(bcm_failover_get(unit, egr_entry.failover_id, &fail_enabled));
        if (fail_enabled == 0)
        {
            *fec_id = egr_entry.failover_if_id;
            bcm_l3_egress_t_init(&egr_entry);
            SHR_IF_ERR_EXIT(dnx_l3_egress_fec_info_get(unit, *fec_id, &egr_entry));
        }
    }
    *destination = egr_entry.port;
    *multicast_rpf_type = L3_EGRESS_GET_MC_RPF_TYPE(egr_entry.flags);
    *out_lif = egr_entry.intf;
    *out_lif_2nd = egr_entry.encap_id;
    *fail_id = egr_entry.failover_id;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  The function returns the number of signals and their values.
 * \param [in] unit - unit ID.
 * \param [in] core_id - core ID.
 * \param [out] nof_signals -Number of signals.
 * \param [out] signal_result - Signal values.
 * \param [out] signal_result_stack - Signal values.
 *
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_pp_vis_rpf_sig(
    int unit,
    int core_id,
    int *nof_signals,
    bcm_instru_vis_signal_result_t * signal_result,
    bcm_instru_vis_signal_result_t * signal_result_stack)
{
    bcm_instru_vis_signal_key_t signal_key[6];
    bcm_instru_vis_signal_key_t signal_key_stack[6];
    int nof_signals_temp, nof_signals_stack_temp;
    char *fec_substages[3] = { "FEC1", "FEC2", "FEC3" };
    SHR_FUNC_INIT_VARS(unit);

    nof_signals_temp = sizeof(signal_key) / sizeof(signal_key[0]);
    if (dnx_data_debug.general.mdb_debug_signals_get(unit))
    {
        nof_signals_stack_temp = sizeof(signal_key_stack) / sizeof(signal_key_stack[0]);
    }
    else
    {
        nof_signals_stack_temp = sizeof(bcm_instru_vis_signal_key_t) * 5 / sizeof(signal_key_stack[0]);
    }

    sal_memset(signal_key, 0, sizeof(bcm_instru_vis_signal_key_t) * (nof_signals_temp));
    sal_memset(signal_key_stack, 0, sizeof(bcm_instru_vis_signal_key_t) * (nof_signals_stack_temp));
    *nof_signals = nof_signals_temp + nof_signals_stack_temp;

    sal_strncpy(signal_key[0].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[0].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[0].signal, "RPF_Dst_Valid", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get(unit, core_id, BCM_INSTRU_VIS_SIGNAL_FLAG_RETURN_ON_1ST_ERROR, 1,
                                                  signal_key, signal_result));
    if (signal_result[0].value[0] == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "No valid RPF lookup has been found \n");
    }

    sal_strncpy(signal_key[0].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[0].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[0].signal, "RPF_Dst.Value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key[1].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[1].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[1].signal, "RPF_Out_LIF", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key[2].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[2].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER), BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[2].signal, "RPF_Dst.Value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key[3].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[3].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER), BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[3].signal, "Fwd_Action_CPU_Trap_Code", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key[4].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[4].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER), BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[4].signal, "MC_RPF_Mode", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key[5].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_VTT4),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[5].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_VTT4), BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key[5].signal, "In_LIFs.0", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key_stack[0].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key_stack[0].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key_stack[0].signal, "Fwd_Action_Dst.Value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key_stack[1].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key_stack[1].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key_stack[1].signal, "RPF_Default_Route_Found", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    sal_strncpy(signal_key_stack[2].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key_stack[2].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_FER),
                BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
    sal_strncpy(signal_key_stack[2].signal, "Fwd_Action_Dst.Value", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

    if (dnx_data_debug.general.mdb_debug_signals_get(unit))
    {
        sal_strncpy(signal_key_stack[3].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                    BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[3].to, fec_substages[0], BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[3].signal, "If_1_Key", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[4].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                    BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[4].to, fec_substages[1], BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[4].signal, "If_1_Key", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[5].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_FER),
                    BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[5].to, fec_substages[2], BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[5].signal, "If_1_Key", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
    }
    else
    {
        sal_strncpy(signal_key_stack[3].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                    BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[3].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                    BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[3].signal, "RPF_Dst.Type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);

        sal_strncpy(signal_key_stack[4].block, dnx_pp_stage_block_name(unit, DNX_PP_STAGE_IFWD2),
                    BCM_INSTRU_VIS_BLOCK_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[4].from, dnx_pp_stage_name(unit, DNX_PP_STAGE_IFWD2),
                    BCM_INSTRU_VIS_STAGE_NAME_MAX_SIZE - 1);
        sal_strncpy(signal_key_stack[4].signal, "Fwd_Action_Dst.Type", BCM_INSTRU_VIS_SIGNAL_NAME_MAX_SIZE - 1);
    }
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get
                    (unit, core_id, BCM_INSTRU_VIS_SIGNAL_FLAG_RETURN_ON_1ST_ERROR, nof_signals_temp, signal_key,
                     signal_result));
    SHR_IF_ERR_EXIT(bcm_dnx_instru_vis_signal_get
                    (unit, core_id, BCM_INSTRU_VIS_SIGNAL_FLAG_RETURN_ON_1ST_ERROR, nof_signals_stack_temp,
                     signal_key_stack, signal_result_stack));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that presents the RPF FER selection
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_pp_vis_rpf_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_mc = 0;
    uint8 out_lif_passed = 0;
    int default_rpf_en = 1;
    int hier_id;
    int idx;
    int core_in;
    int core_id;
    int nof_signals;
    int pkt_is_trapped = 0;
    int fwd_type;
    int rv = 0;
    uint32 uc_rpf_mode;
    uint32 rpf_destination;
    uint32 mc_rpf_mode;
    uint32 def_route_hit = 0;
    uint32 fwd_act_dst;
    uint32 fec_interfaces[3] = { 0 };
    bcm_if_t in_lif = 0;
    const int nof_hierarchies = FEC_NOF_HIERARCHY;
    uint32 rpf_fec[FEC_NOF_HIERARCHY];

    bcm_instru_vis_signal_result_t signal_result[6];
    bcm_instru_vis_signal_result_t signal_result_stack[6];
    bcm_l3_ingress_t l3_ing_intf;

    char sub_titles[3][19] = { "1st hierarchy", "2nd hierarchy", "3rd hierarchy" };

    char *column_names[DIAG_DNX_PP_VIS_RPF_NOF_COLUMNS] =
        { "Hierarchy ID", "RPF Type", "RPF FEC ID", "RPF out-LIF", "FEC destination",
        "GLOB_OUT_LIF", "GLOB_OUT_LIF_2ND", "Protection", "Default_Route", "Result"
    };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /** determine if core parameter was given*/
    SH_SAND_GET_INT32("core", core_in);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, TRUE);

        /*
         * call signal result function
         */
        SHR_IF_ERR_EXIT(sh_dnx_pp_vis_rpf_sig(unit, core_id, &nof_signals, signal_result, signal_result_stack));

        /** retrieve the forward action destination */
        dnx_dbal_fields_enum_value_get(unit, "DESTINATION_ENCODING", "FEC", &fwd_type);

        rpf_destination = signal_result[0].value[0];
        in_lif = signal_result[5].value[0];
        fwd_act_dst = signal_result_stack[0].value[0];
        def_route_hit = signal_result_stack[1].value[0];

        /**If the in_lif is not in rif range add the encoding to it*/
        if (in_lif >= dnx_data_l3.rif.max_nof_rifs_get(unit))
        {
            BCM_L3_ITF_SET(in_lif, BCM_L3_ITF_TYPE_LIF, in_lif);
        }
        /**Get ingress interface in order to get urpf_mode*/
        bcm_l3_ingress_t_init(&l3_ing_intf);
        rv = bcm_l3_ingress_get(unit, in_lif, &l3_ing_intf);
        /** Check if the entry was found*/
        if (rv != _SHR_E_NONE)
        {
            printf("Ingress interface found in signals %d does not exists \n", in_lif);
            SHR_EXIT();
        }

        uc_rpf_mode = l3_ing_intf.urpf_mode;
        in_lif = BCM_L3_ITF_VAL_GET(in_lif);

        /**Check the rpf type- unicast or multicast rpf*/
        if (signal_result[4].value[0] == DBAL_ENUM_FVAL_MC_RPF_MODE_RESERVED)
        {
            PRT_TITLE_SET("Unicast RPF selection information, CORE ID = %d", core_id);
        }
        else
        {
            PRT_TITLE_SET("Multicast RPF selection information, CORE ID = %d", core_id);
            rpf_destination = fwd_act_dst;
            is_mc = 1;
        }

        if (dnx_data_debug.general.mdb_debug_signals_get(unit))
        {
            fec_interfaces[0] = signal_result_stack[3].value[0];
            fec_interfaces[1] = signal_result_stack[4].value[0];
            fec_interfaces[2] = signal_result_stack[5].value[0];
        }
        /** Check whether the destination type is fec*/
        if (is_mc == 0)
        {
            if (dnx_data_debug.general.mdb_debug_signals_get(unit))
            {
                if (fec_interfaces[0] == 0 && fec_interfaces[1] == 0 && fec_interfaces[2] == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Missing RPF destination FEC resolution \n");
                }
            }
            else
            {
                if (signal_result_stack[3].value[0] != fwd_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "RPF destination type is not FEC \n");
                }
            }
            if (uc_rpf_mode == bcmL3IngressUrpfDisable)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Unicast RPF type was not set for ingress interface \n");
            }
            SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchL3UrpfDefaultRoute, &default_rpf_en));
        }
        else
        {
            if (dnx_data_debug.general.mdb_debug_signals_get(unit))
            {
                if (fec_interfaces[0] == 0 && fec_interfaces[1] == 0 && fec_interfaces[2] == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "RPF destination type is not FEC \n");
                }
            }
            else
            {
                if (signal_result_stack[4].value[0] != fwd_type)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "RPF destination type is not FEC \n");
                }
            }
            SHR_IF_ERR_EXIT(bcm_switch_control_get(unit, bcmSwitchL3McRpfDefaultRoute, &default_rpf_en));
        }

        /** Add columns*/
        for (idx = 0; idx < DIAG_DNX_PP_VIS_RPF_NOF_COLUMNS; idx++)
        {
            PRT_COLUMN_ADD("%s", column_names[idx]);
        }
        /** Iterate over the 3 hierarchies*/
        for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
        {
            uint8 is_allocated = FALSE;
            uint32 hierarchy;
            uint32 out_lif;
            uint32 out_lif_2nd;
            uint32 failover_id;
            int rpf_is_trapped = 0;
            int fail_enabled = 0;
            char mc_rpf[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
            char uc_rpf_type[DBAL_MAX_PRINTABLE_BUFFER_SIZE];

            /**Check if FEC is allocated*/
            SHR_IF_ERR_EXIT(dnx_algo_l3_fec_is_allocated(unit, BCM_L3_ITF_VAL_GET(rpf_destination), &is_allocated));

            if (is_allocated == 1)
            {
                if ((BCM_L3_ITF_VAL_GET(rpf_destination) >> 1) != fec_interfaces[hier_id])
                {
                    LOG_CLI(("WARNING! Incorrect calculation of SUPER FEC ID: expected 0x%04x but is actually 0x%04x\n",
                             (rpf_destination >> 1), fec_interfaces[hier_id]));
                }
                SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get
                                (unit, BCM_L3_ITF_VAL_GET(rpf_destination), &hierarchy));
                /** In case the FEC is matching the current hierarchy continue with the FER process. */
                if (hier_id != hierarchy)
                {
                    continue;
                }
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

                rpf_fec[hier_id] = BCM_L3_ITF_VAL_GET(rpf_destination);
                /** Get FEC data*/
                SHR_IF_ERR_EXIT(dnx_pp_rpf_add_fec_info(unit, &rpf_fec[hier_id], hier_id,
                                                        &rpf_destination, &out_lif, &out_lif_2nd, &mc_rpf_mode,
                                                        &failover_id));
                /** Hierarchy ID*/
                PRT_CELL_SET("%s", sub_titles[hier_id]);
                /** RPF type */
                if (is_mc == TRUE)
                {
                    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_SUPER_FEC_1ST_HIERARCHY,
                                                                          DBAL_FIELD_MC_RPF_MODE,
                                                                          &mc_rpf_mode, NULL, 0, FALSE, mc_rpf));
                    PRT_CELL_SET("%s", mc_rpf);
                }
                else
                {
                    uint32 uc_rpf;
                    if (uc_rpf_mode == bcmL3IngressUrpfLoose)
                    {
                        uc_rpf = DBAL_ENUM_FVAL_RPF_MODE_LOOSE;
                    }
                    else
                    {
                        uc_rpf = DBAL_ENUM_FVAL_RPF_MODE_STRICT;
                    }
                    SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_IN_LIF_PROFILE_TABLE,
                                                                          DBAL_FIELD_RPF_MODE,
                                                                          &uc_rpf, NULL, 0, FALSE, uc_rpf_type));
                    PRT_CELL_SET("%s", uc_rpf_type);
                }
                /** FEC ID */
                PRT_CELL_SET("%u", rpf_fec[hier_id]);
                /** RPF out LIF */
                PRT_CELL_SET("0x%04x", signal_result[1].value[0]);
                /**FEC destination*/
                PRT_CELL_SET("0x%04x", rpf_destination);
                /**Global Out_Lif*/
                PRT_CELL_SET("0x%04x", out_lif);
                /**Second Global Out_Lif*/
                PRT_CELL_SET("0x%04x", out_lif_2nd);
                /**Protection*/
                if (failover_id != 0)
                {
                    SHR_IF_ERR_EXIT(bcm_failover_get(unit, failover_id, &fail_enabled));
                    if (fail_enabled == 0)
                    {
                        PRT_CELL_SET("SECONDARY");
                    }
                    else
                    {
                        PRT_CELL_SET("PRIMARY");
                    }
                }
                else
                {
                    PRT_CELL_SET("Disabled");
                }
                /**Default Route Enable/disable*/
                PRT_CELL_SET("%s", (def_route_hit == 1) ? "Enabled" : "Disabled");

                /**RPF Check result*/
                if (is_mc == 1)
                {
                    if (mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_EXPLICIT)
                    {
                        if (!BCM_GPORT_IS_FORWARD_PORT(rpf_destination))
                        {
                            /**Carry out RPF check*/
                            if (((BCM_L3_ITF_VAL_GET(out_lif) != in_lif) && signal_result[1].value[0] != in_lif)
                                || (default_rpf_en == 0 && def_route_hit == 1))
                            {
                                rpf_is_trapped = 1;
                            }
                        }
                        else
                        {
                            rpf_is_trapped = -1;
                        }
                    }
                    else if (mc_rpf_mode == DBAL_ENUM_FVAL_MC_RPF_MODE_USE_SIP)
                    {
                        if (out_lif_passed == 0)
                        {
                            /**Carry out RPF check*/
                            if ((BCM_L3_ITF_VAL_GET(out_lif) != in_lif && signal_result[1].value[0] != in_lif)
                                || (default_rpf_en == 0 && def_route_hit == 1))
                            {
                                rpf_is_trapped = 1;
                            }
                        }
                        else
                        {
                            rpf_is_trapped = -1;
                        }
                        out_lif_passed = 1;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported MC RPF type\n");
                    }
                }
                else
                {
                    if (uc_rpf_mode == bcmL3IngressUrpfStrict)
                    {
                        if (out_lif_passed == 0)
                        {
                            /**Carry out RPF check*/
                            if ((BCM_L3_ITF_VAL_GET(out_lif) != in_lif && signal_result[1].value[0] != in_lif)
                                || (default_rpf_en == 0 && def_route_hit == 1))
                            {
                                rpf_is_trapped = 1;
                            }
                        }
                        else
                        {
                            rpf_is_trapped = -1;
                        }
                        out_lif_passed = 1;
                    }
                    else if (uc_rpf_mode == bcmL3IngressUrpfLoose)
                    {
                        if (default_rpf_en == 0 && def_route_hit == 1)
                        {
                            rpf_is_trapped = 1;
                        }
                        else
                        {
                            rpf_is_trapped = -1;
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Not supported UC RPF type\n");
                    }
                }
                /** Write down result from RPF check*/
                if (rpf_is_trapped == 1)
                {
                    PRT_CELL_SET("FAIL");
                    pkt_is_trapped = 1;
                }
                else
                {
                    if (rpf_is_trapped == -1)
                    {
                        PRT_CELL_SET("N/A");
                    }
                    else
                    {
                        PRT_CELL_SET("PASS");
                    }
                }
                /** Get next FEC if any, otherwise break*/
                if (BCM_GPORT_IS_FORWARD_PORT(rpf_destination))
                {
                    rpf_destination = BCM_GPORT_FORWARD_PORT_GET(rpf_destination);
                }
                else
                {
                    break;
                }
            }
            else
            {
                printf("FEC with ID 0x%04x is not allocated \n", rpf_destination);
                break;
            }
        }
        PRT_COMMITX;

        /**Verify that the diagnostic found the same result as the signals indicate*/
        if (pkt_is_trapped == 1)
        {
            if (signal_result[3].value[0] != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_UC_LOOSE_RPF_FAIL
                && signal_result[3].value[0] != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_UC_STRICT_RPF_FAIL
                && signal_result[3].value[0] != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_MC_USE_SIP_RPF_FAIL
                && signal_result[3].value[0] != DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_MC_EXPLICIT_RPF_FAIL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! The diagnostic command found that the packet is trapped, but the signal indicates it was not.\n");
            }
        }
        else
        {
            if (signal_result[3].value[0] == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FLP_UC_LOOSE_RPF_FAIL
                || signal_result[3].value[0] == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_UC_STRICT_RPF_FAIL
                || signal_result[3].value[0] == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_MC_USE_SIP_RPF_FAIL
                || signal_result[3].value[0] == DBAL_ENUM_FVAL_INGRESS_TRAP_ID_FER_MC_EXPLICIT_RPF_FAIL)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! The diagnostic command found that the packet is not trapped, but the signal indicates it was.\n");
            }
        }
        if (is_mc)
        {
            /**compare the fwd_act_dest signal from fer with last destination from FEC*/
            if (signal_result_stack[2].value[0] != (rpf_destination & 0xFFFFFF))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! The rpf destination found by the diagnostic command is not the same as the one from the signals \n");
            }
        }
        else
        {
            /**compare the rpf_dest signal from fer with last destination from FEC*/
            if (signal_result[2].value[0] != (rpf_destination & 0xFFFFFF))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Error! The rpf destination found by the diagnostic command is not the same as the one from the signals \n");
            }
        }
    }/** of ITERATOR for cores */
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_pp_vis_rpf_options[] = {
    {NULL}
};

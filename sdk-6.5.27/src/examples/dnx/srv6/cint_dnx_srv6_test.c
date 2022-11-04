/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */

/*
 * This CINT use below illustration as an example
 *  +-+-+-+-+-+-+-+
 *  |      ARP    |
 *  +-+-+-+-+-+-+-+
 *  | SRv6 tunnel |
 *  +-+-+-+-+-+-+-+
 *  |  SRH base   |
 *  +-+-+-+-+-+-+-+
 *  |     SID0    |  -> it's called last SID or final SID in configuration
 *  +-+-+-+-+-+-+-+
 *  |     SID1    |
 *  +-+-+-+-+-+-+-+
 *  |     SID2    |
 *  +-+-+-+-+-+-+-+
 *  |     SID3    |
 *  +-+-+-+-+-+-+-+
 *  |   IP/ETH    |
 *  +-+-+-+-+-+-+-+
*/

/*
 * Required Cints:
 * cint ../../../../src/examples/dnx/utility/cint_dnx_util_srv6.c
 */


struct srv6_stat_s
{
    int stat_id;
    int pp_profile_id;
    int counter_command;
    int engine_id;
    int database_id;
    int is_etpp;
};

srv6_stat_s srv6_stat_properties =
{
    /**SRH base LIF stat_id */
    1,
    /**pp_profile_id*/
    1,
    /**counter_command*/
    1,
    /**engine_id*/
    1,
    /**database_id*/
    1,
    /**ETPP or IRPP stat */
    0
};

/* Init struc for replace tests */
bcm_srv6_sid_initiator_info_t sid_encap_info;
bcm_srv6_srh_base_initiator_info_t srh_base_encap_info;
bcm_gport_t srv6_tunnel_id;
bcm_gport_t last_sid_tunnel_id;
bcm_gport_t first_sid_tunnel_id;
bcm_gport_t egress_native_ac;
bcm_gport_t srv6_srh_base_tunnel_id;
bcm_gport_t srv6_func_tunnel_id;
bcm_gport_t egress_outer_ac;
bcm_gport_t srv6_gsid_func_tunnel_id;

/* ARP_PLUS_AC with stat */
int arp_ac_stat;

/* Init new SIDs for replace test */
bcm_ip6_t new_SID[4] =
{{ 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00 },
 { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x11, 0x11 },
 { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x22, 0x22, 0x22, 0x22 },
 { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x33, 0x33, 0x33, 0x33 }};

/* Global VLAN action id */
int eve_action_id = 3;

/* Global vars for egress qos test */
int egress_qos_model;
int egress_ttl_model;
int egress_qos_val;
int egress_ttl_val;

/*
 *    estimate encapsulatio size for ipv6 tunnel.
 *    Assuming it point to SRH, which point to ARP.
 *    since srh point from ipv6 tunnel, it is essential to determine the estimated encap size.
 */
int
srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(
        int unit,
        int nof_sids,
        int * estimated_encap_size)
{
    uint32 ipv6_encap_size = 0;
    uint32 sid_encap_size = 0;
    uint32 min_arp_encap_size = 14;
    uint32 srv6_encap_size = 0;
    int reduce_mode_enabled;
    uint32 estimated_encap_size_required =
    		*dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    if (estimated_encap_size_required)
    {
        ipv6_encap_size = *dnxc_data_get(unit, "lif", "out_lif", "global_ipv6_outlif_bta_sop", NULL);
        sid_encap_size = *dnxc_data_get(unit, "lif", "out_lif", "global_srv6_outlif_bta_sop", NULL);
        /** since all out lifs are pointed from the ipv6 tunnel, need to estimate the entire encap size */
        /** ARP = min 14, IPv6 = 40, srh = 16*nof_sids + 8 */
        BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduce_mode_enabled), "");
        srv6_encap_size = reduce_mode_enabled ?
                (8 + sid_encap_size*(nof_sids-1)) : (8 + sid_encap_size*(nof_sids));
        *estimated_encap_size = ipv6_encap_size + srv6_encap_size + min_arp_encap_size;
    }

    return BCM_E_NONE;
}
/*
 *    Configure CRPS entry and bind stat PP profile to LIF
 */
int
srv6_stat_config(int unit, bcm_gport_t gport_id) {

    bcm_stat_pp_info_t stat_info;
    bcm_stat_pp_profile_info_t pp_profile;
    int pp_profile_id;

    cint_stat_pp_config_info_default.pp_profile = srv6_stat_properties.pp_profile_id;
    cint_stat_pp_config_info_default.counter_command = srv6_stat_properties.counter_command;
    cint_stat_pp_config_info_default.crps_engine_id = srv6_stat_properties.engine_id;
    cint_stat_pp_config_info_default.database_id = srv6_stat_properties.database_id;
    cint_stat_pp_config_info_default.stat_id = srv6_stat_properties.stat_id;
    cint_stat_pp_config_info_default.expansion_flag = 1;

    /* Configure crps entry */
    BCM_IF_ERROR_RETURN_MSG(cint_stat_pp_config_crps(unit, cint_stat_pp_config_info_default,
                                  (srv6_stat_properties.is_etpp?bcmStatCounterInterfaceEgressTransmitPp:bcmStatCounterInterfaceIngressReceivePp)
                                  , BCM_CORE_ALL), "");

    /** Configure pp profile */
    BCM_IF_ERROR_RETURN_MSG(cint_stat_pp_create_pp_profile(unit, cint_stat_pp_config_info_default,
                                        (srv6_stat_properties.is_etpp?bcmStatCounterInterfaceEgressTransmitPp:bcmStatCounterInterfaceIngressReceivePp)
                                        , &pp_profile, &pp_profile_id), "");

    /** Bind profile to lif */
    stat_info.stat_pp_profile = pp_profile_id;
    stat_info.stat_id = srv6_stat_properties.stat_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_gport_stat_set(unit, gport_id, BCM_CORE_ALL,
                            (srv6_stat_properties.is_etpp?bcmStatCounterInterfaceEgressTransmitPp:bcmStatCounterInterfaceIngressReceivePp)
                            , stat_info), "");

    return BCM_E_NONE;
}

/*
 *    Configuring ISR (Ingress SRv6 Tunnel)
 *    This will create encapsulation of SRH,SIDs and IPv4/IPv6/Bridge onto the packet
 *    The difference between this cint with srv6_ingress_tunnel_config is that:
 *        Only one global out lif is used, in egress, the EEDB encapsulation is:
 *        SRH Base -> SID3 -> SID2 -> SID1 -> SID0 -> SRv6 Tunnel -> ARP+AC(or ARP + ESEM AC)
 */
int
srv6_ingress_tunnel_test(
                            int unit,
                            int in_port,             /** Incoming port of SRV6 Ingress Tunnel */
                            int out_port,            /** Outgoing port of SRV6 Ingress Tunnel */
                            int nof_sids,
                            int is_eth_fwd,
                            int is_arp_plus_ac)      /** Indication if it's ARP+AC, or AC coming from ESEM */
{

    char error_msg[200]={0,};
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
    int ive_edit_profile = 5;
    int action_id = 6;
    int new_outer_vid = 10;
    int new_inner_vid = 9;
    int tag_format = 8; /* default 0x8100 */
    bcm_vlan_port_t vlan_port;

    char *proc_name = "srv6_ingress_tunnel_test";
    if (nof_sids < 2 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 2 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port of SRV6 Ingress Tunnel %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]), error_msg);

    /** SRV6 Ingress Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    /* allow 2 TAG add */
    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION | BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    if (is_arp_plus_ac) {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
            "create egress object ARP only, in SRV6 Ingress Tunnel");
    } else {
        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;
        bcm_l3_egress_t_init(&l3eg);
        sal_memcpy(l3eg.mac_addr, cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], 6);
        l3eg.encap_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
        l3eg.vlan = cint_srv6_tunnel_info.tunnel_vid;
        l3eg.encap_access = bcmEncapAccessAc;
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid), "create egress object");

        cint_srv6_tunnel_info.tunnel_arp_id[INGRESS] = l3eg.encap_id;

        /* Create ESEM AC */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.vsi = cint_srv6_tunnel_info.tunnel_vid;
        vlan_port.port = out_port;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

        /* Save ac for EVE */
        egress_outer_ac = vlan_port.vlan_port_id;
    }

    /*
     * 6. SRV6 FEC Entries
     */

    /*
     * Update FEC IDS
     */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(unit), "");

    bcm_gport_t gport;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0, fec_flags2,
                                           0, 0,&encoded_fec1), "create egress object FEC_1 only");


    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add IPv4 host entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC to bring out IPV6, SRH*/
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
    {
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1, 0, 0),
            "for SRV6 Ingress Tunnel");

    }
    else
    {
        snprintf(error_msg, sizeof(error_msg), "VRF:%d", cint_srv6_tunnel_info.vrf_in[INGRESS]);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.route_ipv4_mask, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]), error_msg);
    }

    /*
     * 9. Add IPv6 host entry
     */
    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
    {
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1), error_msg);
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg), "VRF:%d", cint_srv6_tunnel_info.vrf_in[INGRESS]);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.route_ipv6_mask, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]), error_msg);
    }

    /*
     * 10. Add MACT entry
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

    uint32 l2_flags = 0;
    if (*dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
    {
        l2_flags |= BCM_L2_STATIC;
    }
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, dmac, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    l2_addr.port = gport;
    l2_addr.flags = l2_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");


    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }
        else
        {
            sid_info.next_encap_id = 0;
        }
        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");

    srv6_srh_base_tunnel_id = srh_base_info.tunnel_id;
    /*
     * 11. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = is_eth_fwd ? bcmTunnelTypeEthSR6:bcmTunnelTypeSR6;
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;

    
    if (!*dnxc_data_get (unit, "dev_init", "general", "application_v2", NULL))
    {
        /* since srh point from ipv6 tunnel, it is essential to determine the estimated encap size */
        BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * 12. Add egress native AC for native EVE
     */
    if (is_eth_fwd)
    {
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
        vlan_port.port = tunnel_init_set.tunnel_id;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

        /* Save native ac for EVE */
        egress_native_ac = vlan_port.vlan_port_id;
    }

    /*
     * 13. Add ingress AC IVE
     */
    /* set edit profile for ingress/egress LIF */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = new_outer_vid;
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = in_port_intf[0];
    port_trans.vlan_edit_class_id = ive_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit, &port_trans), "");

    /* Create action ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id), "");

    /* Set translation action class (map edit_profile & tag_format to action_id) */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ive_edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 *    Configuring ISR (Ingress SRv6 Tunnel)
 *    This will create encapsulation of SRH,SIDs and IPv4/IPv6/Bridge onto the packet
 *    In this example, the last DIP is brought out by SRv6 tunnel
 *       and ARP points to AC (different from ARP+AC or ARP+ESEM AC)
 *    FWD(L2/IPv4/IPv6)-> hier FECs(SRH->SIDs, IP tunnel) + out_lif(ARP)
 */
int
srv6_ingress_tunnel_arp_to_ac(
                            int unit,
                            int in_port,             /** Incoming port of SRV6 Ingress Tunnel */
                            int out_port,            /** Outgoing port of SRV6 Ingress Tunnel */
                            int nof_sids)
{

    char error_msg[200]={0,};
    char *proc_name = "srv6_ingress_tunnel_arp_to_ac";
    if (nof_sids < 1 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 1 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port of SRV6 Ingress Tunnel %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]), error_msg);

    /** SRV6 Ingress Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     * 5. Create ARP and set next pointer to AC
     */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /* Save ac for EVE */
    egress_outer_ac = vlan_port.vlan_port_id;

    int arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];

    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], 6);
    l3eg.encap_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    l3eg.vlan = cint_srv6_tunnel_info.tunnel_vid;
    l3eg.vlan_port_id = egress_outer_ac;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid), "create egress object");

    cint_srv6_tunnel_info.tunnel_arp_id[INGRESS] = l3eg.encap_id;

    /*
     * 6. SRV6 FEC Entries
     */

    /*
     * Update FEC IDS
     */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(unit), "");

    bcm_gport_t gport;
    bcm_gport_t gport_l2vpn;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    int encoded_fec1_l2vpn;
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID]);
    BCM_GPORT_FORWARD_PORT_SET(gport_l2vpn, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_L2VPN_FEC_ID]);

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0 , srv6_basic_lif_encode, gport, 0, fec_flags2,
                                           0, 0,&encoded_fec1), "create egress object FEC_1 only");

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_L2VPN_FEC_ID], 0 , srv6_basic_lif_encode, gport_l2vpn, 0, fec_flags2,
                                           0, 0,&encoded_fec1_l2vpn), "create egress object FEC_1_L2VPN only");

    int srv6_first_lif_encode;
    int encoded_fec2;
    int encoded_fec2_l2vpn;
    BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY, fec_flags2,
                                           0, 0,&encoded_fec2), "create egress object FEC_2 only, fec");

    BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_L2VPN_FEC_ID], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY, fec_flags2,
                                           0, 0,&encoded_fec2_l2vpn), "create egress object FEC_2_L2VPN only, fec");

    /*
     * 7. Add IPv4 host entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC to bring out IPV6, SRH*/
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport),
        "for SRV6 Ingress Tunnel");


    /*
     * 8. Add IPv6 host entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport), error_msg);

    /*
     * 9. Add MACT entry
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_L2VPN_FEC_ID]);

    uint32 l2_flags = 0;
    if (*dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
    {
        l2_flags |= BCM_L2_STATIC;
    }
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_fwd_mac, cint_srv6_tunnel_info.l2_fwd_vsi);
    l2_addr.port = gport;
    l2_addr.encap_id = arp_id;
    l2_addr.flags = l2_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    /*
     * 10. define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx = 1; /* last SID will be provided by IPv6 DIP */

    for (sid_idx; sid_idx < nof_sids; sid_idx++)
    {

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }
        else
        {
            sid_info.next_encap_id = 0;
        }
        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /*
     * 11. define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** set number of SIDs, last SID is brought out by DIP, so here minus 1*/
    srh_base_info.nof_sids = nof_sids - 1;
    /** we will pass the global-out-LIF id */
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    /** if no SIDs present, don't point to garbage but NULL */
    if (nof_sids == 1)
    {
        srh_base_info.next_encap_id = 0;
    }
    else
    {
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);
    }

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");

    /*
     * 12. Configuring ISR SRv6 Tunnel
     */
    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
    tunnel_init_set.encap_access = bcmEncapAccessTunnel4; /* Set DIP to phase 6 */
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeEthSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
    tunnel_init_set.encap_access = bcmEncapAccessTunnel4; /* Set DIP to phase 6 */

    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID | BCM_TUNNEL_INIT_STAT_ENABLE;
    /** convert 1st SID global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_L2VPN_GLOBAL_LIF]);

    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /*
     * 12. Add egress native AC for native EVE
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.port = tunnel_init_set.tunnel_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /* Save native ac for EVE */
    egress_native_ac = vlan_port.vlan_port_id;

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/* Used in replace test */
int
srv6_sid_encap_create(int unit) {

    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_encap_info), "for SID");

    return BCM_E_NONE;
}

/* Used in replace test */
int
srv6_srh_base_encap_create(int unit) {
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_encap_info), "");

    return BCM_E_NONE;
}

/*
 *    Configuring ESR (Egress SRv6 Tunnel), two modes
 *       USP: 1st PASS, direct traffic to RCY port, terminate SRv6 tunnel & SRH
 *            2nd PASS, FWD based on ETH/IPv4/IPv6
 *       PSP: enable SOC appl_param_srv6_psp_enable, terminate SRH & forward
 */
int
srv6_egress_tunnel_test(
    int unit,
    int in_port,
    int rch_port,
    int out_port,
    int skip_rif)
{
    char error_msg[200]={0,};
    int in_vlan = 15;
    int out_vlan = 100;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    int vsi = 15;
    int encap_id = 0x1384;
    int fec;
    int encoded_fec;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 host = 0x7fffff02;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_ip6_t default_ipv6_route ={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    bcm_ip6_t default_ipv6_mask = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    char *proc_name;

    proc_name = "srv6_egress_tunnel_test";

    if(sbfd_reflector_ipv6_with_srv6)
    {
        intf_out_mac_address[0] = 0;
        intf_out_mac_address[1] = 0;
        intf_out_mac_address[2] = 0;
        intf_out_mac_address[3] = 0;
        intf_out_mac_address[4] = 0xcd;
        intf_out_mac_address[5] = 0x1d;
    }
    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, vsi), "intf_in");

    /*
     * 2. Set Out-Port default properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", intf_in);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_in, intf_in_mac_address), error_msg);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, intf_out, intf_out_mac_address), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Create ARP and set its properties
     */
    if (arp_ac_stat) {
        flags2 |= BCM_L3_FLAGS2_EGRESS_STAT_ENABLE;
    }
    if(skip_rif == 1)
    {
        printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                    proc_name, encap_id, flags2);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, intf_out, 0, flags2),
            "create egress object ARP only");
    }
    else
    {
        printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                    proc_name, encap_id, flags2);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, out_vlan, 0, flags2),
            "create egress object ARP only");
    }

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */

    /* Get FEC ID according to the current static FEC allocation  */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");

    printf("%s(): Create main FEC and set its properties.\r\n",proc_name);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    int encap_id_itf;

    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_L3_ITF_SET(encap_id_itf, BCM_L3_ITF_TYPE_LIF, encap_id);
    if(skip_rif)
    {

        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, 0 , encap_id_itf, gport, 0, fec_flags2,
                                               0, 0,&encoded_fec), "create egress object FEC only");
    }
    else
    {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, fec, intf_out , encap_id, gport, 0, fec_flags2,
                                               0, 0,&encoded_fec), "create egress object FEC only");
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);


    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
    {
        /*
         * 7. Add IPv4 host entry
         */
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, host, vrf, encoded_fec, 0, 0), "");

        /*
         * 8. Add IPv6 fwd entries, for PSP serves as next SID lookup, for USP serves as 2nd pass lookup
         */
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, encoded_fec);
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, encoded_fec), error_msg);
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg), "VRF:%d", vrf);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, host, cint_srv6_tunnel_info.route_ipv4_mask, vrf, encoded_fec), error_msg);
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, encoded_fec);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_ip_route_basic_ipv6_mask, vrf, encoded_fec), error_msg);
    }

    /* route */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", vrf, encoded_fec);
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, vrf, encoded_fec), error_msg);

    /* default route */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, default_ipv6_route, default_ipv6_mask, vrf, encoded_fec), error_msg);

    /*
     * 9. Add MACT entry
     */
    uint32 l2_flags = 0;
    if (*dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
    {
        l2_flags |= BCM_L2_STATIC;
    }
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_termination_mact_fwd_address, vsi);
    l2_addr.port = out_port;
    l2_addr.flags = l2_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");

    /*
     * 10. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, rch_port), "recycle_port");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", rch_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, rch_port),"");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 *    Configuring SRv6 usid endpoint function
 */
int
srv6_usid_endpoint_example(
                    int unit,
                    int in_port,        /** Incoming port of SRV6 End Point */
                    int out_port,       /** Outgoing port of SRV6 End Point */
                    int srh_present,    /** If set, SRH header is present in uSID packet */
                    int tunnel_type,
                    int is_default)     /** If set, USIDwith cascaded lookup is performed */
{
    char error_msg[200]={0,};
    char *proc_name = "srv6_usid_endpoint_example";
    bcm_ip6_t ipv6_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    int is_prefix_48b_enabled = *dnxc_data_get(unit, "srv6", "general", "srv6_usid_prefix_48b_enable", NULL);

    /*
     * 0. Set legal FEC IDs
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_srv6_update_fecs_id(unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** End-Point-1 Eth RIF based on Port + VLAN ID so that we can re-use same port */
    BCM_IF_ERROR_RETURN_MSG(in_port_vid_intf_set(unit, in_port, cint_srv6_tunnel_info.tunnel_vid[END_POINT_1_VID], cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]), "");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port of SRV6 Ingress Tunnel %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** End-Point-1 My-MAC - set to SRV6 Ingress Tunnel next hop MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1], cint_srv6_tunnel_info.intf_in_mac_address[END_POINT_1]), error_msg);

    /** End-Point-1 out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.intf_out_mac_address[END_POINT_1]), "intf_out");

    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;
    /** End-Point-1 VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_1];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "End-Point-1");

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], cint_srv6_tunnel_info.arp_next_hop_mac[END_POINT_1], cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], 0, flags2),
        "create egress object ARP only, in End-Point-1");

    /*
     * 6. SRV6 FEC Entries
     */
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    bcm_gport_t gport;
    /** End-Point-1 FEC Entry - tying end-point-1 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only for End-Point-1: fec = %d, intf_out = %d, encap_id = %d, out_port = %d",
        cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], gport, 0, fec_flags2), error_msg);

    /*
     * 7. Add route and host entry
     */
    /** add End-Point-1's next IP (u-SID) as host to do forwarding on to FEC */
    if(is_default)
    {
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d",
            cint_srv6_tunnel_info.vrf_in[NO_DEFAULT], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_1_DIP], ipv6_mask, cint_srv6_tunnel_info.vrf_in[NO_DEFAULT], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), error_msg);
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d",
            cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_1_DIP], ipv6_mask, cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), error_msg);
    }

    /*
     * 8. End-Point configurations
     */
    /** End-Point-1: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    BCM_IF_ERROR_RETURN_MSG(srv6_tunnel_termination(unit, END_POINT_1_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type, is_default), "");

    /** End-Point-2: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    BCM_IF_ERROR_RETURN_MSG(srv6_tunnel_termination(unit, END_POINT_2_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type, is_default), "");

    /** End-Point-3: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    BCM_IF_ERROR_RETURN_MSG(srv6_tunnel_termination(unit, END_POINT_3_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type, is_default), "");

    /** End-Point-4: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    BCM_IF_ERROR_RETURN_MSG(srv6_tunnel_termination(unit, END_POINT_4_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type, is_default), "");

    /** End-Point-5: Add MyDIP of current End-Point, getting same VRF for all End-Points */
    BCM_IF_ERROR_RETURN_MSG(srv6_tunnel_termination(unit, END_POINT_5_DIP, 0, srh_present, cint_srv6_tunnel_info.vrf_in[END_POINT_1], tunnel_type, is_default), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 *    Configuring SRv6 usid egress usp cross connect
 */
int
srv6_usid_egress_cross_connect(
                    int unit,
                    int in_port,      /** Incoming port of SRV6 Egress */
                    int rch_port,     /** Rycling port number for 1st Pass USP in Egress */
                    int out_port,     /** Outgoing port of SRV6 Egress */
                    int srh_present,  /** If set, SRH header is present in uSID packet */
                    int is_default)   /** If set, USIDwith cascaded lookup is performed */
{
    char error_msg[200]={0,};
    char *proc_name = "srv6_usid_egress_cross_connect";
    bcm_tunnel_type_t tunnel_type = bcmTunnelTypeIpAnyIn6;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    int flags = (is_default) ? BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM : 0;
    int is_prefix_48b_enabled = *dnxc_data_get(unit, "srv6", "general", "srv6_usid_prefix_48b_enable", NULL);

    /*
     * 0. Set legal FEC IDs
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_srv6_update_fecs_id(unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** Egress Eth RIF based on Port + VLAN ID */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d, intf_in = %d", in_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(in_port_vid_intf_set(unit, in_port, cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), error_msg);

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    /** Egress out_port */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /** RCH Eth_Rif My MAC  */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.eth_rif_intf_in[RCH]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[RCH], cint_srv6_tunnel_info.intf_in_mac_address[RCH]), error_msg);

    /** Egress out-port SA  */
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;
    /** Egress Tunnel - 1st pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d",
        cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.vrf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), error_msg);

    /** Egress Tunnel - RCH - 2nd pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[RCH];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[RCH];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Create ARP and set its properties
     */
    /** Egress ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d, vsi = %d",
        cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], 0,0), error_msg);

    /*
     * 6. SRV6 FEC Entries
     */
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    bcm_gport_t gport;
    /*
     * On 1st Pass Egress, no FEC entry  because no more SIDs left to enter FWD on and then FEC,
     * in VTT5 there's LIF P2P which gives the destination right away
     */
    /** Egress 2nd Pass FEC Entry - due to Forwarding on IPv4 Layer above SRv6  */
    int egress_encoded_fec1;
    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS_OUT_VID], cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], gport, 0, fec_flags2, 0, 0,&egress_encoded_fec1),
        "create egress object FEC only");

    /*
     * 7. Cross connect, no host/route entry needed
     */
    /*
     * 8. Configure the SRv6 ESR USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */
    {

        /** Set Out-Port default properties for the recycled packets*/
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, rch_port), "recycle_port");
        if (!ibch1_supported)
        {
            /** Configure RCH port of Extended Termination type */
            snprintf(error_msg, sizeof(error_msg), "rch_port %d", rch_port);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
        }
        /** Set port class in order for PMF to crop RCH ad IRPP */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

        /** Create termination for next protocol nof_sids to RCH port and encap_id */
        int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
        int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
        int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
        BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
                unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, rch_port),"");
    }

    /*
     * 9. Egress extended termination
     */
    bcm_ip6_t sip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    bcm_ip6_t ip6_usid_prefix_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_func = {0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_usid_48b_prefix_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_48b_prefix_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};

    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = tunnel_type;
    tunnel_term_set.flags = flags | BCM_TUNNEL_TERM_MICRO_SEGMENT_ID | BCM_TUNNEL_TERM_CROSS_CONNECT;
    if(srh_present) {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    if (is_prefix_48b_enabled) {
        if (is_default) {
            sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_4_DIP], 16);
        } else {
            sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_5_DIP], 16);
        }
    } else {
        if (is_default) {
            sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_5_DIP], 16);
        } else {
            sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[EGRESS_DIP], 16);
        }
    }
    sal_memcpy(tunnel_term_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, (is_prefix_48b_enabled ? ip6_usid_48b_prefix_mask : ip6_usid_prefix_mask), 16);
    sal_memcpy(tunnel_term_set.sip6_mask, sip6_mask, 16);
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    /* Following is for configuring for the IPv6 LIF, to increase the strength of its TTL
     * so that terminated IPv6 header's TTL would get to sysh, from which the IPv6 TTL decrease
     * would decrease from
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    srv6_tunnel_term_id = tunnel_term_set.tunnel_id;

    if (is_default)
    {
        /*
        * Create function lif lookup
        */
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;
        tunnel_term_set.default_tunnel_id = srv6_tunnel_term_id;
        if (is_prefix_48b_enabled) {
            sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_4_DIP], 16);
        } else {
            sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[END_POINT_5_DIP], 16);
        }
        sal_memcpy(tunnel_term_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, (is_prefix_48b_enabled ? ip6_mask_48b_prefix_func : ip6_mask_func), 16);
        sal_memcpy(tunnel_term_set.sip6_mask, sip6_mask, 16);
        tunnel_term_set.vrf = 0;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;

        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");
        srv6_tunnel_term_id = tunnel_term_set.tunnel_id;
    }

    /*
     * 10. Add cross connect
     */
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_vswitch_cross_connect_t_init(&cc_gports);
    BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
    cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    cc_gports.port1 = srv6_tunnel_term_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ESR USP and multicast to access & SRv6
 */
int
srv6_egress_tunnel_split_horizon(
    int unit,
    int in_port_egress, /** Incoming port of SRV6 Egress */
    int out_port_egress, /** Outgoing port of SRV6 Egress */
    int out_port, /** Outgoing port of SRV6 Ingress Tunnel */
    int nof_sids)
{
    char error_msg[200]={0,};
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int sid_idx;
    int mc_id = cint_srv6_tunnel_info.l2_termination_vsi;
    bcm_vlan_port_t vlan_port;
    char *proc_name = "srv6_egress_tunnel_split_horizon";
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port_egress, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties
     */
    /* Egress tunnel out port */
    snprintf(error_msg, sizeof(error_msg), "out_port %d", out_port_egress);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port_egress), error_msg);

    /* Ingress tunnel out port */
    snprintf(error_msg, sizeof(error_msg), "out_port %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /* Egress tunnel out rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]), "intf_out");

    /* Ingress tunnel out rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_srv6_tunnel_info.recycle_port), "recycle_port");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", cint_srv6_tunnel_info.recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,cint_srv6_tunnel_info.recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, cint_srv6_tunnel_info.recycle_port),"");

     /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 7. No need to create FEC, since it's multicast
     */

    /*
     * 8. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */


    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */
    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");


    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeEthSR6;
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;
    BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");


    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * Add egress native AC
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    vlan_port.port = tunnel_init_set.tunnel_id;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    egress_native_ac = vlan_port.vlan_port_id;

    /*
     * 9. Create multicast group and add entries
     */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &mc_id, 0), "");

    bcm_gport_t ports[2];
    int encaps[2];

    /** egress out port */
    BCM_GPORT_MODPORT_SET(ports[0], unit, out_port_egress);
    encaps[0] = 0;

    /** ingress tunnel */
    BCM_GPORT_MODPORT_SET(ports[1], unit, out_port);
    encaps[1] = cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF];

    BCM_IF_ERROR_RETURN_MSG(multicast__add_multicast_entry(unit, mc_id, ports, encaps, 2, 0), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}


/*
 * Configuring ESR USP and IPMC forward
 */
int
srv6_egress_tunnel_ipmc_fwd(
    int unit,
    int in_port,
    int out_port_1,
    int out_port_2)
{
    char error_msg[200]={0,};
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    bcm_gport_t gport;
    int encap_id = 0x1384;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int sid_idx;
    uint32 dip_mask = 0xFFFFFFFF;
    bcm_ip6_t ipv6_mc = {0xFF, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    uint32 sip_mask = 0x0;

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties
     */
    /* Egress tunnel out port */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port_1);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port_1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", out_port_2);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port_2), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /* Egress tunnel out rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_srv6_tunnel_info.recycle_port), "recycle_port");
    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", cint_srv6_tunnel_info.recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,cint_srv6_tunnel_info.recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");
    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, cint_srv6_tunnel_info.recycle_port),"");

    /*
     * 6. Create ARP and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS],
                                            cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0, flags2), "create egress object ARP only");

    /*
     * 7. No need to create FEC, since it's multicast
     */

    /*
     * 8. Create multicast group and add entries
     */
    int flags = BCM_MULTICAST_WITH_ID;
    uint32 mc_flags = 0;
    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    } else {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP;
    }
    flags |= mc_flags;

    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group), "");

    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.port = out_port_1;
    rep_array.encap1 = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    snprintf(error_msg, sizeof(error_msg), "in Port= %d", out_port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_ipmc_info.mc_group, mc_flags, 1, &rep_array), error_msg);

    rep_array.port = out_port_2;
    rep_array.encap1 = cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS];
    snprintf(error_msg, sizeof(error_msg), "in Port= %d", out_port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_ipmc_info.mc_group, mc_flags, 1, &rep_array), error_msg);

    /* Add IPMC entry */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, mc group = %d", vrf, cint_ipmc_info.mc_group);
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, dip_mask, cint_ipmc_info.sip, sip_mask,
                       0, vrf, cint_ipmc_info.mc_group, 0,0), error_msg);

     /* Add IPv6 MC entry */
    uint16 compressed_sip_mask = 0;
    bcm_ip6_t sip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_ip6_t sip1 = {0x02, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    bcm_ip6_t sipv6_mask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL)))
    {
        sal_memcpy(sip, sip1, 16);
        sal_memcpy(sipv6_mask, full_mask, 16);
        compressed_sip_mask = 0xFFFF;
    }
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, mc group = %d", vrf, cint_ipmc_info.mc_group);
    BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, ipv6_mc, full_mask, sip, sipv6_mask, 0, vrf, 0, compressed_sip_mask, cint_ipmc_info.mc_group, 0, 0, 0), error_msg);

    return BCM_E_NONE;
}

/*
 * Configuring ISR and IPMC forward
 */
int
srv6_ingress_tunnel_ipmc_fwd(
                            int unit,
                            int in_port,
                            int out_port_1,
                            int out_port_2,
                            int nof_sids,
                            int nof_cores,
                            int rcy_port_core_0,
                            int rcy_port_core_1)
{
    char error_msg[200]={0,};
    bcm_gport_t gport;
    int in_vlan = 15;
    uint32 dip_mask = 0xFFFFFFFF;
    uint32 sip_mask = 0x0;
    bcm_ip6_t ipv6_mc = {0xFF, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13};
    bcm_ip6_t full_mask = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bcm_ip6_t sip = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_ip6_t sip1 = {0x02, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    bcm_ip6_t sipv6_mask = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    bcm_gport_t out_ac;
    uint16 compressed_sip_mask = 0;

    char *proc_name = "srv6_ingress_tunnel_ipmc_fwd";

    if (*(dnxc_data_get(unit, "l3", "fwd", "ipmc_config_cmprs_vrf_group", NULL)))
    {
        sal_memcpy(sip, sip1, 16);
        sal_memcpy(sipv6_mask, full_mask, 16);
        compressed_sip_mask = 0xFFFF;
    }

    if (nof_sids < 2 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 2 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), "intf_in");

    /* Create AC to match port+vlan and set ingress network group */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vlan_port.match_vlan = in_vlan;
    vlan_port.vsi = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    vlan_port.port = in_port;
    vlan_port.ingress_network_group_id = 2;

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port of SRV6 Ingress Tunnel %d", out_port_1);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port_1), error_msg);

    /* Create VSI */
    snprintf(error_msg, sizeof(error_msg), "create VLAN %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), error_msg);

    /* Create out AC for bridge fallback egress split horizon */
    /* use EEDB, since for J2C, "esem" "feature" "etps_properties_assignment" -> out_lif profile use EES, not ETPS*/
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.egress_network_group_id = 2;

    snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);
    out_ac=vlan_port.vlan_port_id;

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]), error_msg);

    /** SRV6 Ingress Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    ingress_rif.flags |= BCM_L3_INGRESS_IPMC_BRIDGE_FALLBACK; /* enable RIF bridge fallback */
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */
    uint32 flags2 = 0;
    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 6. No need to create FEC, since it's multicast
     */

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;
        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");


    /*
     * 7. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id,
                            cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;


    /*
     * 8. Multicast group and entry
     */
    /* Create multicast group */
    int flags = BCM_MULTICAST_WITH_ID;
    uint32 mc_flags = 0;
    if (*dnxc_data_get(unit, "multicast", "params", "mcdb_formats_v2", NULL)) {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_ELEMENT_STATIC;
    } else {
        mc_flags = BCM_MULTICAST_INGRESS_GROUP;
    }
    flags |= mc_flags;

    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group), "");

    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.port = out_port_2;
    rep_array.encap1 = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]; /* fall to bridge */

    snprintf(error_msg, sizeof(error_msg), "in Port= %d", out_port_2);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_ipmc_info.mc_group, mc_flags, 1, &rep_array), error_msg);

    bcm_if_t rep_idx = 0x300100;

    rep_array.port = out_port_1;
    rep_array.encap1 = rep_idx;

    snprintf(error_msg, sizeof(error_msg), "in Port= %d", out_port_1);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_ipmc_info.mc_group, mc_flags, 1, &rep_array), error_msg);

    /* Add PPMC entry */
    int cuds[2];
    BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_idx, 2, cuds), "");

    /* Add IPMC entry */
    BCM_IF_ERROR_RETURN_MSG(add_ipv4_ipmc(unit, cint_ipmc_info.mc_dip, dip_mask, cint_ipmc_info.sip, sip_mask,
                       0, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_ipmc_info.mc_group, 0, 0), "");

    /* Add IPv6 MC entry */
    BCM_IF_ERROR_RETURN_MSG(add_ipv6_ipmc(unit, ipv6_mc, full_mask, sip, sipv6_mask, 0, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, compressed_sip_mask, cint_ipmc_info.mc_group, 0, 0, 0), "");

    /*
     * 9. Multicast for bridge fallback
     */
    /* Create multicast group */
    cint_ipmc_info.mc_group = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_create(unit, flags, &cint_ipmc_info.mc_group), "");

    /* Add copies */
    bcm_multicast_replication_t replications[3];
    set_multicast_replication(&replications[0], out_port_1, -1);
    set_multicast_replication(&replications[1], out_port_2, (out_ac & 0xfffff)); /* dropped by split horizon */
    set_multicast_replication(&replications[2], in_port, 0); /* dropped by same interface filter */

    BCM_IF_ERROR_RETURN_MSG(bcm_multicast_add(unit, cint_ipmc_info.mc_group, mc_flags, 3, replications), "");

    /* Configure split horizon */
    bcm_switch_network_group_config_t network_group_config;
    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = 2;
    network_group_config.config_flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit, 2, &network_group_config), "");

    if (*dnxc_data_get(unit, "l3", "feature", "mc_bridge_fallback", NULL)) {
        /* use field processor to perform bridge fallback:
           for ipvx routing packets with no hit in forwarding, and for vsi profile:
           fwd to recycle port and build recycle header, update vsi and pop lif */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port_core_0, bcmPortControlRecycleApp, bcmPortControlRecycleAppBridgeFallback),
            "with type bcmPortControlRecycleApp");
        /** Set port class in order for PMF to crop RCH ad IRPP */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,rcy_port_core_0, bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

        if (nof_cores > 1)
        {
            BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rcy_port_core_1, bcmPortControlRecycleApp, bcmPortControlRecycleAppBridgeFallback),
                "with type bcmPortControlRecycleApp");
            /** Set port class in order for PMF to crop RCH ad IRPP */
            BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, rcy_port_core_1, bcmPortClassFieldIngressPMF3TrafficManagementPortCs, 4), "");
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, in_port, bcmPortControlSystemPortInjectedMap, 1),
            "with type bcmPortControlSystemPortInjectedMap");


        /* Create a control lif and set it's in_lif orientation as required*/
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY | BCM_VLAN_PORT_RECYCLE;
        vlan_port.ingress_network_group_id = 2;
        vlan_port.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        vlan_port.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
        vlan_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;

        snprintf(error_msg, sizeof(error_msg), "(port = %d)", vlan_port.port);
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), error_msg);

        /* configure vsi profile for vsis */
        int vsi_profile = 1;
        BCM_IF_ERROR_RETURN_MSG(vlan__control_vlan_vsi_profile_set(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], vsi_profile), "");

        BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_main(unit), "");

        BCM_IF_ERROR_RETURN_MSG(cint_field_bridge_fallback_entry_add(unit, nof_cores,rcy_port_core_0,
            rcy_port_core_1, vsi_profile), "");
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ingress tunnel ROO
 *   1st pass: IP host -> FEC for native ARP (1st hierarchy) + RCH EE FEC (2nd hierarchy)
 *   2nd pass: RCH EE FEC -> IP tunnel FEC (1st hierarchy) + SRH FEC (2nd hierarchy)
 *             SRH FEC -> (SRH -> SID list -> ARP)
 */
int
srv6_ingress_tunnel_roo(
        int unit,
        int in_port,
        int out_port,
        int rch_port,
        int nof_sids,
        int last_sid_is_dip)
{
    char error_msg[200]={0,};
    char *proc_name = "srv6_ingress_tunnel_roo";
    int first_fec_in_hier;
    bcm_gport_t gport;
    int reduced_mode = 0;
    int native_arp_id;
    bcm_mac_t native_next_hop_mac = {0x00, 0x00, 0x00, 0x11, 0xCD, 0x1D};
    int native_eth_rif = 20;
    bcm_mac_t native_eth_rif_mac = {0x00, 0x00, 0x08, 0x00, 0x01, 0x01};
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    uint32 estimate_encap_size_required =
        *dnxc_data_get(unit, "lif", "out_lif", "global_lif_allocation_estimated_encap_size_enable", NULL);

    BCM_IF_ERROR_RETURN_MSG(bcm_switch_control_get(unit, bcmSwitchSRV6ReducedModeEnable, &reduced_mode),
        "bcmSwitchSRV6ReducedModeEnable");

    if(nof_sids > 4)
    {
        printf("Error, nof_sids in the RCYed pass is limited to 4, %d\n",nof_sids);
        return -1;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port of SRV6 Ingress Tunnel %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]), error_msg);

    /** SRV6 Ingress Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /** Native eth rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, native_eth_rif, native_eth_rif_mac), "native rif");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    if (!ibch1_supported)
    {
        /*
         * 5. Configure recycle port
         */
        snprintf(error_msg, sizeof(error_msg), "rch_port %d", rch_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncapUncollapse), error_msg);
    }
    /** Set port class in order for PMF to crop RCH at IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    /*
     * Configure native ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &native_arp_id, native_next_hop_mac, native_eth_rif, BCM_L3_NATIVE_ENCAP, BCM_L3_FLAGS2_VLAN_TRANSLATION),
        "for native ARP");

    /*
     * Configure overlay ARP entry
     */
    uint32 flags = BCM_L3_WITH_ID;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    int arp_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], 0, flags2),
        "for overlay ARP");

    /*
     * 7. SRV6 FEC Entries
     */

    int global_lif_encoded;
    int encoded_fec_ip_tunnel;
    int encoded_fec_srh;
    int encoded_fec_rch;
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    /* Update FEC ids */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &first_fec_in_hier), "0");

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID] = first_fec_in_hier + 0x100;
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID] = first_fec_in_hier + 0x200;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &first_fec_in_hier), "0");

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID] = first_fec_in_hier + 0x1;
    int native_fec_id = first_fec_in_hier + 0x2;

    /*
     * Create SRH second pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, fec_flags2,
                                           0, 0,&encoded_fec_srh), "create egress object FEC_2 only");

    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_SECOND_PASS_FEC_ID]);

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID], 0 , global_lif_encoded, gport, 0, fec_flags2,
                                           0, 0,&encoded_fec_ip_tunnel), "create egress object FEC_1 only");

    /* Create 1st_to_2nd pass RCH EE(extended encap), destination is 2nd pass IP tunnel FEC */
    bcm_l2_egress_t recycle_entry_1_to_2;
    bcm_l2_egress_t_init(&recycle_entry_1_to_2);
    recycle_entry_1_to_2.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT;
    recycle_entry_1_to_2.dest_encap_id = 0;
    BCM_GPORT_FORWARD_PORT_SET(recycle_entry_1_to_2.dest_port, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_IP_TUNNEL_FEC_ID]);
    recycle_entry_1_to_2.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry_1_to_2), "");

    /*
     * Create RCH_1_2 pass FEC
     */
    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, recycle_entry_1_to_2.encap_id);
    BCM_GPORT_LOCAL_SET(gport, rch_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID], 0 , global_lif_encoded, gport, BCM_L3_2ND_HIERARCHY, fec_flags2,
                                           0, 0,&encoded_fec_rch), "create egress object FEC_3 only");

    int encoded_fec;

    BCM_L3_ITF_SET(global_lif_encoded, BCM_L3_ITF_TYPE_LIF, native_arp_id);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_RCH_1_2_PASS_FEC_ID]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, native_fec_id, 0 , global_lif_encoded, gport, 0, fec_flags2, 0, 0,&encoded_fec),
        "create egress object FEC_2 only");

    /*
     * 8. Configuring ISR SRv6 Tunnel
     *
     *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
     *    - creation of x2 local-out-LIFs to return following entries from EEDB:
     *       1. SRH Base -> SID(4) ..-> SID(0)
     *       2. IPv6 Tunnel -> ARP+AC
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    /** Add the L2VPN tunnel */
    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeEthSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** check if last SID is brought by IPv6 Tunnel */
    if (last_sid_is_dip)
    {
        sal_memcpy(tunnel_init_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], 16);
    }
    tunnel_init_set.flags = BCM_TUNNEL_WITH_ID;
    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    if(estimate_encap_size_required)
    {
        /* Estimation is 0, all estimations are taken into account in the SRH outlif */
        tunnel_init_set.estimated_encap_size = 0;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;
    bcm_srv6_sid_initiator_info_t_init(&sid_info);
    int next_sid_tunnel_id;
    int sid_idx;
    int sid_location;
    int sid_start_idx = SID0;
    if (last_sid_is_dip)
    {
        sid_start_idx = SID1; /* SID0 (last one) may be given by IPv6 Tunnel */
        nof_sids = nof_sids - 1;
    }
    int arp_as_ift;

    BCM_L3_ITF_SET(arp_as_ift, BCM_L3_ITF_TYPE_LIF, arp_id);
    /* Last SID points to ARP */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, arp_as_ift);

    printf("Creating 2nd pass SRH tunnel, nof sid = %d\n", nof_sids);
    int sid_count = 0;
    for (sid_count = 0; sid_count < nof_sids; sid_count++)
    {
        sid_idx = sid_start_idx - sid_count;
        bcm_srv6_sid_initiator_info_t_init(&sid_info);

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[sid_idx], 16);
        /** set to EEDB entry of SID0 */
        if (last_sid_is_dip)
        {
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count - 1;
        } else
        {
            sid_info.encap_access = bcmEncapAccessTunnel4 - sid_count;
        }

        /** next_encap_id set to l3 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d 2nd pass", sid_count);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);
        next_sid_tunnel_id = sid_info.tunnel_id;
    }

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_2ND_PASS_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, next_sid_tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */
    if(estimate_encap_size_required)
    {
        /*
         * This is the last pass, estimation is:
         * nof_sids*16 + 8(SRH) + 40(IP header) + ARP(at least 14B)
         */
        srh_base_info.estimated_encap_size = (nof_sids*16 + 8) + 40 + 14 - 16*reduced_mode;
    }

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "2nd pass");

    /*
     * 9. Add route entries pointing to native ARP FEC
     */
    /* IPv4 host */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec, 0, 0),
        "for SRV6 Ingress Tunnel");

    /* IPv6 host */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec),
        "for SRV6 Ingress Tunnel");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring egress tunnel ROO
 */
int
srv6_egress_tunnel_roo(
                            int unit,
                            int core_port,
                            int access_port)
{
    char error_msg[200]={0,};
    bcm_gport_t gport;
    int flags;
    int vrf = 1;
    int native_vlan = 30;
    int out_vlan = 103;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_mac_t native_eth_rif_mac = {0x00, 0x00, 0x00, 0x00, 0xAA, 0xBB};
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    char *proc_name = "srv6_egress_tunnel_roo";

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /* SRV6 Egress Tunnel in port */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), "intf_in");

    /*
     * 2. Egress tunnel out port
     */
    snprintf(error_msg, sizeof(error_msg), "of SRV6 Ingress Tunnel %d", core_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, access_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /* SRV6 Egress Tunnel in RIF */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /* native RIF */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", native_vlan);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, native_vlan, native_eth_rif_mac), error_msg);

    /* SRV6 Egress Tunnel out RIF  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    /* Egress tunnel in RIF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /* native RIF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = native_vlan;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     * 5. Create ARP and set its properties
     */

    /* Access side ARP */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], out_vlan, 0, flags2),
        "create egress object ARP only");

    /*
     * 6. SRV6 FEC Entries
     */

    /*
     * Get FEC ID
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]), "");

    /* Create access FEC, out of overlay */
    flags = 0;
    flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_GPORT_LOCAL_SET(gport, access_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], cint_srv6_tunnel_info.tunnel_arp_id[EGRESS],
                                     0, gport, flags,flags2), "create access FEC");

    /*
     * 7. Add route entry, routing into overlay
     */


    if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
    {
        /* Add IPv4 host entry */
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, vrf, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], 0, 0),
            "for SRV6 Ingress Tunnel");


        /* Add IPv6 host entry */
        BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, vrf, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]), "");
    }
    else
    {
        snprintf(error_msg, sizeof(error_msg), "VRF:%d", vrf);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.route_ipv4_mask, vrf, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]), error_msg);
        BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.route_ipv6_mask, vrf, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]), error_msg);
    }

    /*
     * 8. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_srv6_tunnel_info.recycle_port), "");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", cint_srv6_tunnel_info.recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortClassFieldIngressPMF3TrafficManagementPortCs, 4), "");
    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, cint_srv6_tunnel_info.recycle_port),"");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ESR USP and into MPLS tunnel
 */
int
srv6_egress_tunnel_into_mpls(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int nof_tunnel,
                            int is_p2p)
{
    char error_msg[200]={0,};
    bcm_gport_t gport;
    int flags;
    int vrf = 1;
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport_fec;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    char *proc_name = "srv6_egress_tunnel_into_mpls";

    /* Configure SRv6 tunnel to be P2P */
    if (is_p2p)
    {
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
        sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
        tunnel_term_set.vrf = vrf;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;

        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    /* In this cint, all ports, RIF, ARP, FEC configurations are done in mpls_util_main */

    /*
     * 1. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_srv6_tunnel_info.recycle_port), "");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", cint_srv6_tunnel_info.recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,cint_srv6_tunnel_info.recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, cint_srv6_tunnel_info.recycle_port),"");

    /*
     * 2. Configure MPLS tunnel encapsulation
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_deep_stack_ipv6_example(unit,core_port_1,core_port_2,nof_tunnel,0), "");

    /* Add cross connect entry for P2P */
    if (is_p2p)
    {
        BCM_GPORT_FORWARD_PORT_SET(gport_fec, mpls_util_entity[0].fecs[0].fec_id);

        bcm_vswitch_cross_connect_t_init(&cc_gports);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = tunnel_term_set.tunnel_id;
        cc_gports.port2 = gport_fec;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ESR USP and into VPLS tunnel
 */
int
srv6_egress_tunnel_into_vpls(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int is_p2p)
{
    char error_msg[200]={0,};
    bcm_gport_t gport;
    int flags;
    int vrf = 1;
    cint_srv6_tunnel_info.l2_termination_vsi = 5;
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport_fec;
    int fec_id;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    char *proc_name = "srv6_egress_tunnel_into_vpls";

    /* Configure SRv6 tunnel to be P2P */
    if (is_p2p)
    {
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
        sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
        tunnel_term_set.vrf = vrf;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;

        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
    }

    /*
     * 1. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_srv6_tunnel_info.recycle_port), "");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", cint_srv6_tunnel_info.recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,cint_srv6_tunnel_info.recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, cint_srv6_tunnel_info.recycle_port),"");

    printf("Finished RCH\n");

    /*
     * 2. Configure VPLS basic
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_main(unit,core_port_1,core_port_2), "");

    /* In this cint,out port, ARP, FEC, MACT are configured in vpls_main, only need to create in port and RIF here*/

    /*
     * 3. Set In-Port to In ETh-RIF
     */
    /* SRV6 Egress Tunnel in port */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port_1, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), "intf_in");

    /*
     * 4. Create ETH-RIF and set its properties
     */
    /* SRV6 Egress Tunnel in RIF */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /*
     * 5. Set Incoming ETH-RIF properties
     */
    /* Egress tunnel in RIF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     *Add cross connect entry for P2P
     *SRv6 tunnel P2P only supports port or FEC, so here create a new FEC pointing to PWE encap
     * and set PWE's next pointer to MPLS tunnel
     */
    if (is_p2p)
    {

        /* Replace PWE encap with next pointer to MPLS tunnel */
        pwe_encap.flags |= (BCM_MPLS_PORT_REPLACE | BCM_MPLS_PORT_WITH_ID);
        pwe_encap.egress_tunnel_if = mpls_encap_tunnel[0].tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_encapsulation(unit, &pwe_encap), "");

        /* Create FEC pointing to PWE */
        BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");

        fec_id = fec_id + 10;
        /* Create access FEC, out of overlay */
        flags = 0;
        int flags2 = 0;
        if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
        {
            flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
        }

        BCM_GPORT_LOCAL_SET(gport, core_port_2);
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, fec_id, pwe_encap.encap_id | 0x40000000, 0, gport, flags, flags2),
            "create access FEC");

        bcm_vswitch_cross_connect_t_init(&cc_gports);
        BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, fec_id);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = tunnel_term_set.tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ESR USP and into SRv6 tunnel
 */
int
srv6_egress_tunnel_into_srv6(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int next_protocol_is_l2,
                            int is_p2p,
                            int nof_sids)
{
    char error_msg[200]={0,};
    int vrf = 1;                /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_vswitch_cross_connect_t cc_gports;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    int sid_idx;
    char *proc_name = "srv6_egress_tunnel_into_srv6";
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /* Configure SRv6 tunnel to be P2P */
    if (is_p2p)
    {
        bcm_tunnel_terminator_t_init(&tunnel_term_set);
        tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
        sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
        sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
        sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
        tunnel_term_set.vrf = vrf;
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_CROSS_CONNECT;
        if (next_protocol_is_l2) {
            tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
            tunnel_term_set.type = bcmTunnelTypeEthIn6;
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

        if (!next_protocol_is_l2) {
            l3_ingress_intf_init(&ingress_rif);
            ingress_rif.vrf = vrf;
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
            BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
        }
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port_1, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties
     */
    /* Ingress tunnel out port */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port %d", core_port_2);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, core_port_2), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /* Egress tunnel in rif */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /* Ingress tunnel out rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_srv6_tunnel_info.recycle_port), "");

    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", cint_srv6_tunnel_info.recycle_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, cint_srv6_tunnel_info.recycle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,cint_srv6_tunnel_info.recycle_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
            unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, cint_srv6_tunnel_info.recycle_port),"");

     /*
     * 6. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 6. SRV6 FEC Entries
     */

    int fec_flags2 = 0;

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

     /* Update FEC IDS */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(unit), "");

    int srv6_basic_lif_encode;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0, fec_flags2),
        "create egress object FEC_1 only");


    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    if (!is_p2p) {
        /*
         * 8. Add host entries
         */

        /** Skip host in L3 for APP_V2 */
        if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL) && add_ipv6_host_entry)
        {
            /** IPv4 host */
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS],
                               cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0, 0), "for SRV6 Ingress Tunnel");


            /** IPv6 host */
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.vrf_in[INGRESS],
                               cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]), "");
        }
        else
        {
            /** L3 Routing */
            snprintf(error_msg, sizeof(error_msg), "VRF:%d", vrf);
            BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.route_ipv4_mask,
                    cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]), error_msg);


            BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.route_ipv6_mask,
                    cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]), error_msg);
        }

        /*
         * 9. Add MACT entry
         */
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

        uint32 l2_flags = 0;
        if (*dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
        {
            l2_flags |= BCM_L2_STATIC;
        }
        bcm_l2_addr_t l2_addr;
        bcm_l2_addr_t_init(&l2_addr, dmac, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
        l2_addr.port = gport;
        l2_addr.flags = l2_flags;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    }

    /*
     * 10. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */


    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);
    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */
    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");



    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    if(next_protocol_is_l2)
    {
        tunnel_init_set.type = bcmTunnelTypeEthSR6;
    }
    else
    {
        tunnel_init_set.type = bcmTunnelTypeSR6;
    }
    tunnel_init_set.l3_intf_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;

    
    if (!*dnxc_data_get (unit, "dev_init", "general", "application_v2", NULL))
    {
        BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");


    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;

    /* Add cross connect entry for P2P */
    if (is_p2p)
    {
        bcm_vswitch_cross_connect_t_init(&cc_gports);
        BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = tunnel_term_set.tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ISR after MPLS termination
 */
int
srv6_ingress_tunnel_with_mpls_termination(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int nof_sids)
{
    char error_msg[200]={0,};
    int vrf = 100; /* use value in init_default_mpls_params */
    bcm_gport_t gport;
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0,0,0xFF,0x55};
    int sid_idx;
    char *proc_name = "srv6_ingress_tunnel_with_mpls_termination";


    /* In this cint, in port, RIF, are done in mpls_util_main */
    /*
     * 1. Create RIF
     */

    /* Ingress tunnel out rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 2. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 3. SRV6 FEC Entries
     */

     /* Update FEC IDS */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(unit), "");

    int srv6_basic_lif_encode;
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0, fec_flags2),
        "create egress object FEC_1 only");


    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    /*
     * 4. Add host entries
     */
    /** IPv4 host */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, vrf,
                       cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0, 0), "for SRV6 Ingress Tunnel");

    /** IPv6 host */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, ip6_dip, vrf,
                       cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]), "");

    /*
     * 5. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);
    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */
    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;


        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");


    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    tunnel_init_set.l3_intf_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;


    /*
     * 6. Configure MPLS tunnel termination
     */
    BCM_IF_ERROR_RETURN_MSG(mpls_util_main(unit,core_port_1,core_port_2), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configuring ISR after MPLS swap
 */
int
srv6_ingress_tunnel_with_mpls_swap(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int nof_sids)
{
    char error_msg[200]={0,};
    int swapped_label = 5555;
    int swapping_label = 6666;
    bcm_gport_t gport;
    uint32 flags2 = 0;
    int sid_idx;
    char *proc_name = "srv6_ingress_tunnel_with_mpls_swap";

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port_1, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, core_port_2), "intf_out out_port of SRV6 Ingress Tunnel");

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]), error_msg);

    /** SRV6 Ingress Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 6. SRV6 FEC Entries
     */

     /* Update FEC IDS */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(unit), "");

    int srv6_basic_lif_encode;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0, flags2),
        "create egress object FEC_1 only");


    /*
     * 7. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);
    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */
    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;


        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");


    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    tunnel_init_set.l3_intf_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * 8. Create MPLS LSR entry
     */

    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.label = swapped_label;
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.egress_label.label = swapping_label;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    /*DESTINATION(FEC) + EEI*/
    entry.egress_if = cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID];

    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_tunnel_switch_create(unit, &entry), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}


/*
 * Configuring ISR after VPLS termination
 */
int
srv6_ingress_tunnel_with_vpls_termination(
                            int unit,
                            int core_port_1,
                            int core_port_2,
                            int is_p2p,
                            int nof_sids)
{
    char error_msg[200]={0,};
    bcm_gport_t gport;
    bcm_vswitch_cross_connect_t cc_gports;
    int vsi = 5; /* use value in vpls_main */
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    bcm_mac_t dmac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x30};
    int sid_idx;
    char *proc_name = "srv6_ingress_tunnel_with_vpls_termination";

    /* In this cint, in port, RIF, are done in vpls_main */
    /*
     * 1. Create RIF
     */

    /* Ingress tunnel out rif */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 2. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     */

    /** SRV6 Ingress Tunnel ARP create */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 3. SRV6 FEC Entries
     */

    /* Update FEC IDS */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(unit), "");

    int srv6_basic_lif_encode;
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, core_port_2);

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, gport, 0, fec_flags2),
        "create egress object FEC_1 only");


    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */
    if (!is_p2p) {
        /*
         * 4. Add MACT entry
         */
        BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

        uint32 l2_flags = 0;
        if (*dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
        {
            l2_flags |= BCM_L2_STATIC;
        }
        bcm_l2_addr_t l2_addr;
        bcm_l2_addr_t_init(&l2_addr, dmac, vsi);
        l2_addr.port = gport;
        l2_addr.flags = l2_flags;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    }

    /*
     * 5. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */


    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */
    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(unit, &srh_base_info), "");


    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeEthSR6;
    tunnel_init_set.l3_intf_id = cint_srv6_tunnel_info.tunnel_arp_id[INGRESS];
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");


    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;
    /*
     * 6. Configure VPLS tunnel termination
     */
    params_set = 1;
    init_default_vpls_params(unit);
    if (is_p2p)
    {
        pwe_term.flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;
        vpn = 0;
    }

    ac_port.port = core_port_2;
    pwe_port.port = core_port_1;

    /* In port */
    BCM_IF_ERROR_RETURN_MSG(vpls_configure_port_properties(unit, pwe_port, 1 /* create_rif */ ), "");

    /*
     * create vlan based on the vsi (vpn)
     */
    if (!is_p2p)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, vpn), "");
        pwe_term.forwarding_domain = vpn;
    }

    /* In RIF */
    BCM_IF_ERROR_RETURN_MSG(vpls_create_l3_interfaces(unit, &pwe_port), "");

    /* Configure a termination label for the ingress flow */
    BCM_IF_ERROR_RETURN_MSG(vpls_create_termination_stack(unit, mpls_term_tunnel), "");

    /* configure PWE tunnel - ingress flow */
    BCM_IF_ERROR_RETURN_MSG(vpls_mpls_port_add_termination(unit, &pwe_term), "");

    /*
     *Add cross connect entry for P2P
     */
    if (is_p2p)
    {
        bcm_vswitch_cross_connect_t_init(&cc_gports);
        BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
        cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        cc_gports.port1 = pwe_term.mpls_port_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * This function creates an SRv6 Endpoint, SRv6 tunnel termination with 96b/64b locator or function LIF
 *   and cross connect to destination
 */
int
srv6_endpoint_tunnel_cross_connect_prefix_function(int unit, int next_protocol_is_l2, int is_termination, int do_not_use_default_lif)
{
    /* In this example, use a prefix less than 96b or 64b */
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_mask_96_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_96_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00};
    bcm_ip6_t ip6_mask_64_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_64_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_dip_96 = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_dip_64 = {0x11,0x22,0x33,0x44,0xFF,0xFE,0xFF,0x15,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_ip6_t ip6_dip_next_endpoint = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0,0,0xFF,0x13};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    uint32 flags = 0;
    bcm_gport_t default_tunnel;
    int fec;
    bcm_vswitch_cross_connect_t gports;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec), "");
    if (!is_termination) {
        fec += 0x500;
    }
    if (do_not_use_default_lif) {
        flags = BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
    }
    /*
     * 1. Create 96 locator tunnel VRF is 10
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID | BCM_TUNNEL_TERM_CROSS_CONNECT | flags;
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_loc, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    if (!next_protocol_is_l2 && !do_not_use_default_lif)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 10;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    /* Get out port from fec */
    int encoded_fec;
    BCM_L3_ITF_SET(encoded_fec, BCM_L3_ITF_TYPE_FEC, fec);
    bcm_l3_egress_t l3_egress;
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_egress_get(unit,encoded_fec, &l3_egress), "");
    if (!do_not_use_default_lif)
    {
        /* Add cross connect */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = tunnel_term_set.tunnel_id;
        if (next_protocol_is_l2) {
            gports.port2=l3_egress.port;
        } else {
            BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
        }
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), "");
    }
    default_tunnel = tunnel_term_set.tunnel_id;

    /*
     * 2. Create 96 func tunnel VRF is 20
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_CROSS_CONNECT;
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    tunnel_term_set.default_tunnel_id = default_tunnel;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    if (!next_protocol_is_l2)
    {
        srv6_term_tunnel_id = tunnel_term_set.tunnel_id;
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 20;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    /* Add cross connect */
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = tunnel_term_set.tunnel_id;
    if (next_protocol_is_l2) {
        gports.port2=l3_egress.port;
    } else {
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
    }
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), "");

    /*
     * 3. Create 64 locator tunnel VRF is 30
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID | BCM_TUNNEL_TERM_CROSS_CONNECT | flags;
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_loc, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    if (!next_protocol_is_l2 && !do_not_use_default_lif)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 30;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    if (!do_not_use_default_lif)
    {
        /* Add cross connect */
        bcm_vswitch_cross_connect_t_init(&gports);
        gports.port1 = tunnel_term_set.tunnel_id;
        if (next_protocol_is_l2) {
            gports.port2=l3_egress.port;
        } else {
            BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
        }
        gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), "");
    }

    default_tunnel = tunnel_term_set.tunnel_id;

    /*
     * 4. Create 96 func tunnel VRF is 40
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_CROSS_CONNECT;
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    tunnel_term_set.default_tunnel_id = default_tunnel;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 40;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    /* Add cross connect */
    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = tunnel_term_set.tunnel_id;
    if (next_protocol_is_l2) {
        gports.port2=l3_egress.port;
    } else {
        BCM_GPORT_FORWARD_PORT_SET(gports.port2, fec);
    }
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &gports), "");

    return BCM_E_NONE;
}

/*
 * This function creates an SRv6 Endpoint, SRv6 tunnel termination with 96b/64b locator or function LIF
 */
int
srv6_endpoint_tunnel_prefix_function(int unit, int next_protocol_is_l2, int is_termination)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_mask_96_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_96_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00};
    bcm_ip6_t ip6_mask_64_loc = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_mask_64_func = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_dip_96 = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_dip_64 = {0x12,0x34,0,0,0xFF,0xFE,0xFF,0x15,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    bcm_gport_t default_tunnel;

    /*
    * 1. Create locator 96b tunnel
    */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_96_LOCATOR_SEGMENT_ID;
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_loc, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_STAT_ENABLE;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    srv6_term_tunnel_id = tunnel_term_set.tunnel_id;

    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 10;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    default_tunnel = tunnel_term_set.tunnel_id;

    /*
     * 2. Create function tunnel for 96b locator
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    tunnel_term_set.default_tunnel_id = default_tunnel;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_96, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_96_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_STAT_ENABLE;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    srv6_func_tunnel_id = tunnel_term_set.tunnel_id;

    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 20;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    /*
    * 3. Create locator 64b tunnel
    */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.flags = BCM_TUNNEL_TERM_UP_TO_64_LOCATOR_SEGMENT_ID;
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_loc, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 30;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    default_tunnel = tunnel_term_set.tunnel_id;

    /*
     * 2. Create function tunnel for 64b locator
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    tunnel_term_set.default_tunnel_id = default_tunnel;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip_64, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask_64_func, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelPipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    if(is_termination)
    {
        tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    }
    /** 'vlan' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_protocol_is_l2)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    if (!next_protocol_is_l2)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = 40;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    return BCM_E_NONE;
}

/*
 *    Configuring SRv6 GSID endpoint function
 */
int
srv6_gsid_endpoint_test(
                    int unit,
                    int in_port,
                    int out_port,
                    int do_not_use_default_lif,
                    int is_prefix_64b)
{
    char error_msg[200]={0,};
    char *proc_name = "srv6_gsid_endpoint_test";
    bcm_ip6_t ip6_classic_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    /* use 48b locator for both 48b and 64b prefix */
    bcm_ip6_t ip6_gsid_prefix_locator_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_sip  = {0};
    /* compressible SRv6 SID for GSID 3 ~ 0 */
    bcm_ip6_t gsid[4] = {{0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x33, 0x33, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03},
                         {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x22, 0x22, 0xAA, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
                         {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x11, 0x11, 0xAA, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01},
                         {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};
    bcm_ip6_t gsid_prefix_64b[4] = {{0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x33, 0x33, 0xAA, 0x03, 0x00, 0x00, 0x00, 0x03},
                                    {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x22, 0x22, 0xAA, 0x02, 0x00, 0x00, 0x00, 0x02},
                                    {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x11, 0x11, 0xAA, 0x01, 0x00, 0x00, 0x00, 0x01},
                                    {0xAA, 0xAA, 0xBB, 0xBB, 0xCC, 0xCC, 0xDD, 0xDD, 0x00, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00}};
    /** GSID function mask*/
    bcm_ip6_t ip6_gsid_prefix_function_mask = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_gsid_prefix_64b_function_mask = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00};

    bcm_ip6_t dip_ipv6_for_no_default = {0xaa,0xaa,0xbb,0xbb,0xcc,0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    int i;

    /*
     * 0. Set legal FEC IDs
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_srv6_update_fecs_id(unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** End-Point-1 Eth RIF based on Port + VLAN ID so that we can re-use same port */
    BCM_IF_ERROR_RETURN_MSG(in_port_vid_intf_set(unit, in_port, cint_srv6_tunnel_info.tunnel_vid[ENDPOINT_GSID1], cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1]), "in_port");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    snprintf(error_msg, sizeof(error_msg), "intf_out out_port of SRV6 Ingress Tunnel %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** End-Point-1 My-MAC - set to SRV6 Ingress Tunnel next hop MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.intf_in_mac_address[ENDPOINT_GSID1]), error_msg);

    /** End-Point-1 out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID1], cint_srv6_tunnel_info.intf_out_mac_address[ENDPOINT_GSID1]), "intf_out");

    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;
    /** End-Point-1 VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[ENDPOINT_GSID1];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "End-Point-1");

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */
    uint32 flags2 = 0;
    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1], cint_srv6_tunnel_info.arp_next_hop_mac[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], 0, flags2),
        "create egress object ARP only, in End-Point-1");

    /*
     * 6. SRV6 FEC Entries
     */
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }

    bcm_gport_t gport;
    /** End-Point-1 FEC Entry - tying end-point-1 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only for End-Point-1: fec = %d, intf_out = %d, encap_id = %d, out_port = %d",
        cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1], out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_arp_id[ENDPOINT_GSID1], gport, 0, fec_flags2), error_msg);

    /*
     * 7. Add host entries for GSID and classic SID
     */
    for (i=0; i < 4; i++) {
        snprintf(error_msg, sizeof(error_msg), "for GSID1: vrf = %d, fec = %d",
            cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
        if (is_prefix_64b) {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, gsid_prefix_64b[i], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), error_msg);
        } else {
            BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, gsid[i], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), error_msg);
        }
    }

    snprintf(error_msg, sizeof(error_msg), "for GSID1: vrf = %d, fec = %d",
        cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[DIP_ENDPOINT_CLASSIC], cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), error_msg);

    /* Add FWD entry for DIP in IP header if function lif is not hit and no default lif is used */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, dip_ipv6_for_no_default, mask, cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), "");

    /*
     * 8. End-Point configurations
     */
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_classic_mask, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_locator_mask, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6, gsid[0], 16);

    tunnel_term_set.flags = BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID;

    if (do_not_use_default_lif) {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
    }
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1];
    /* Following is for configuring for the IPv6 LIF, to increase the strength of its TTL
     * so that terminated IPv6 header's TTL would get to sysh, from which the IPv6 TTL decrease
     * would decrease from
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "for locator lif");

    /*
     * Configure the termination resulting LIF's FODO to VRF
     */
    if (!do_not_use_default_lif) {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1];
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    /* Save the last tunnel id for replacement test */
    srv6_tunnel_id = tunnel_term_set.tunnel_id;

    /* Function LIF */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_classic_mask, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);

    if (is_prefix_64b) {
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_64b_function_mask, 16);
        sal_memcpy(tunnel_term_set.dip6, gsid_prefix_64b[0], 16);
    } else {
        sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_function_mask, 16);
        sal_memcpy(tunnel_term_set.dip6, gsid[0], 16);
    }

    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1];
    tunnel_term_set.default_tunnel_id = srv6_tunnel_id;
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "for function lif.");

    /*
     * Configure the termination resulting LIF's FODO to VRF
     */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[ENDPOINT_GSID1];
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");

    /* Save the tunnel id for replacement test */
    srv6_gsid_func_tunnel_id = tunnel_term_set.tunnel_id;

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * Configures GSID tunnel termination
 */
int
srv6_gsid_tunnel_termination(
                    int unit,
                    int end_point_id,
                    int is_egress_tunnel,
                    int vrf,
                    int next_header_is_eth,
                    int is_p2p,
                    int is_gsid_eoc,
                    int default_lif_for_cascading,
                    int do_not_use_default_lif,
                    int is_prefix_64b)
{
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int locator_lif_extended_termination = do_not_use_default_lif ? 0 : 1;
    bcm_ip6_t ip6_classic_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    /** GSID function mask*/
    bcm_ip6_t ip6_gsid_prefix_function_mask = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t ip6_gsid_prefix_64b_function_mask = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00};

    /*
     * Configure the keys for the resulting LIF on termination (SIP, DIP, VRF)
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    if (is_prefix_64b) {
        sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip_64b[end_point_id], 16);
        sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.ipv6_gsid_prefix_locator_mask[GSID_PREFIX_64B], 16);
    } else {
        sal_memcpy(tunnel_term_set.dip6, cint_srv6_tunnel_info.tunnel_ip6_dip[end_point_id], 16);
        sal_memcpy(tunnel_term_set.dip6_mask, cint_srv6_tunnel_info.ipv6_gsid_prefix_locator_mask[GSID_PREFIX_48B], 16);
    }

    sal_memcpy(tunnel_term_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_classic_mask, 16);
    /** if GSID, use a 64b locator mask on DIP, apply GSID flags, and check if GSID is EOC */
    tunnel_term_set.flags = BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID;
    if (is_gsid_eoc) {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID_END_OF_COMPRESSION;
    }
    if (is_p2p) {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_CROSS_CONNECT;
    }

    tunnel_term_set.vrf = vrf;
    /* Following is for configuring for the IPv6 LIF, to increase the strength of its TTL
     * so that terminated IPv6 header's TTL would get to sysh, from which the IPv6 TTL decrease
     * would decrease from
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;

    if(default_lif_for_cascading)
    {
        if (is_prefix_64b)
        {
            sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_64b_function_mask, 16);
        }
        else
        {
            sal_memcpy(tunnel_term_set.dip6_mask, ip6_gsid_prefix_function_mask, 16);
        }
    }

    if(is_egress_tunnel)
    {
        tunnel_term_set.flags |= (locator_lif_extended_termination) ? BCM_TUNNEL_TERM_EXTENDED_TERMINATION : 0;
        /* If it's ESR USP, shortpipe and uniform are supported
         * Uniform: take QoS info from SRv6 tunnel
         * Shortpipe: take QoS info from fwd layer
         */
        tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    }

    /** 'vsi' - if configured, it sets the termination LIF property FODO to be this VLAN value */
    if (next_header_is_eth)
    {
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
    }

    if(default_lif_for_cascading)
    {
        if (next_header_is_eth) {
            tunnel_term_set.type = bcmTunnelTypeEthCascadedFunct;
        } else {
            tunnel_term_set.type = bcmTunnelTypeCascadedFunct;
        }
        tunnel_term_set.default_tunnel_id = default_tunnel_for_gsid_cascading;
        tunnel_term_set.flags &= ~(BCM_TUNNEL_TERM_GENERALIZED_SEGMENT_ID);
    }

    if (do_not_use_default_lif) {
        tunnel_term_set.flags |= BCM_TUNNEL_TERM_CASCADED_MISS_DISABLE_TERM;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term_set), "");

    /*
     * Configure the termination resulting LIF's FODO to VRF
     *  - not needed if resulting LIF's FODO is to be VSI (when ETHoSRv6), because
     *    in this case it's set by 'bcm_tunnel_terminator_create' above with 'vlan' property
     */
    if ((!next_header_is_eth) && (!do_not_use_default_lif))
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    srv6_tunnel_id = tunnel_term_set.tunnel_id;

    if(!default_lif_for_cascading) {
        default_tunnel_for_gsid_cascading = tunnel_term_set.tunnel_id;
    }

    return BCM_E_NONE;
}

/*
 *    Configuring SRv6 gsid egress usp
 */
int
srv6_gsid_egress_test(
                    int unit,
                    int in_port,      /** Incoming port of SRV6 Egress */
                    int rch_port,     /** Rycling port number for 1st Pass USP in Egress */
                    int out_port,     /** Outgoing port of SRV6 Egress */
                    int is_p2p,
                    int next_header_is_eth,
                    int do_not_use_default_lif,
                    int is_prefix_64b)
{
    char error_msg[200]={0,};
    char *proc_name = "srv6_gsid_egress_test";
    int vrf_in[4] = {10,20,30,40};
    int i;
    bcm_ip6_t dip_ipv6_for_no_default = {0xaa,0xaa,0xbb,0xbb,0xcc,0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip6_t mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /*
     * 0. Set legal FEC IDs
     */
    BCM_IF_ERROR_RETURN_MSG(dnx_srv6_update_fecs_id(unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** Egress Eth RIF based on Port + VLAN ID */
    snprintf(error_msg, sizeof(error_msg), "in_port = %d, intf_in = %d", in_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(in_port_vid_intf_set(unit, in_port, cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), error_msg);

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    /** Egress out_port */
    snprintf(error_msg, sizeof(error_msg), "out_port = %d", out_port);
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, out_port), error_msg);

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);

    /** RCH Eth_Rif My MAC  */
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.eth_rif_intf_in[RCH]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[RCH], cint_srv6_tunnel_info.intf_in_mac_address[RCH]), error_msg);

    /** Egress out-port SA  */
    snprintf(error_msg, sizeof(error_msg), "intf_out = %d", cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]), error_msg);

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    l3_ingress_intf ingress_rif;
    /** Egress Tunnel - 1st pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d, vrf = %d",
        cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.vrf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), error_msg);

    /** Egress Tunnel - RCH - 2nd pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[RCH];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[RCH];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "intf_out");

    /*
     * 5. Create ARP and set its properties
     */
    uint32 flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;
    /** Egress ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d, vsi = %d",
        cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], 0,flags2), error_msg);

    /*
     * 6. SRV6 FEC Entries
     */

    bcm_gport_t gport;
    /*
     * On 1st Pass Egress, no FEC entry  because no more SIDs left to enter FWD on and then FEC,
     * in VTT5 there's LIF P2P which gives the destination right away
     */
    /** Egress 2nd Pass FEC Entry - due to Forwarding on IPv4 Layer above SRv6  */
    uint32 fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    int egress_encoded_fec;

    /** Egress 2nd Pass FEC Entry - due to Forwarding on IPv4 Layer above SRv6  */
    BCM_GPORT_LOCAL_SET(gport, out_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS_OUT_VID], cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], gport, 0, fec_flags2,
                                                               0, 0,&egress_encoded_fec), "create egress object FEC only");

    /*
     * 7. Configure the SRv6 ESR USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */
    {

        /** Set Out-Port default properties for the recycled packets*/
        BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, rch_port), "");
        if (!ibch1_supported)
        {
            /** Configure RCH port of Extended Termination type */
            snprintf(error_msg, sizeof(error_msg), "for rch_port %d", rch_port);
            BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
        }
        /** Set port class in order for PMF to crop RCH ad IRPP */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

        /** Create termination for next protocol nof_sids to RCH port and encap_id */
        int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
        int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
        int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
        BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
                unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, rch_port),"");
    }

    bcm_vswitch_cross_connect_t cc_gports;
    bcm_vswitch_cross_connect_t_init(&cc_gports);
    cc_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    /*
     * 8. Egress extended termination
     */
    /** to allow termination of GSID because of SL == 0 && SI==0 */
    int locator_lif_vrf_update = do_not_use_default_lif ? 0 : 1;
    srv6_gsid_tunnel_termination(unit, DIP_EGRESS, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], next_header_is_eth, is_p2p, 0, 0, do_not_use_default_lif, is_prefix_64b);
    if (!next_header_is_eth && locator_lif_vrf_update)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf_in[0];
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, srv6_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    if (is_p2p && locator_lif_vrf_update) {
        if (next_header_is_eth) {
            BCM_GPORT_LOCAL_SET(cc_gports.port2, out_port);
        } else {
            BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
        }
        cc_gports.port1 = srv6_tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    /** Function LIF */
    srv6_gsid_tunnel_termination(unit, DIP_EGRESS, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], next_header_is_eth, is_p2p, 0, 1, 0, is_prefix_64b);
    if (!next_header_is_eth)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf_in[1];
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, srv6_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    if (is_p2p) {
        if (next_header_is_eth) {
            BCM_GPORT_LOCAL_SET(cc_gports.port2, out_port);
        } else {
            BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
        }
        cc_gports.port1 = srv6_tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    /** to allow termination of GSID because of SL == 0 && EOC */
    bcm_ip6_t dip_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    sal_memcpy(cint_srv6_tunnel_info.tunnel_ipv6_dip_mask, dip_mask, 16);
    srv6_gsid_tunnel_termination(unit, DIP_EGRESS_EOC, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], next_header_is_eth, is_p2p, 1, 0, do_not_use_default_lif, is_prefix_64b);

    if (!next_header_is_eth && locator_lif_vrf_update)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf_in[2];
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, srv6_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    if (is_p2p && locator_lif_vrf_update) {
        if (next_header_is_eth) {
            BCM_GPORT_LOCAL_SET(cc_gports.port2, out_port);
        } else {
            BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
        }
        cc_gports.port1 = srv6_tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    /** Function LIF */
    srv6_gsid_tunnel_termination(unit, DIP_EGRESS_EOC, 1, cint_srv6_tunnel_info.vrf_in[EGRESS], next_header_is_eth, is_p2p, 1, 1, 0, is_prefix_64b);
    if (!next_header_is_eth)
    {
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = vrf_in[3];
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, srv6_tunnel_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingress_rif), "");
    }

    if (is_p2p) {
        if (next_header_is_eth) {
            BCM_GPORT_LOCAL_SET(cc_gports.port2, out_port);
        } else {
            BCM_GPORT_FORWARD_PORT_SET(cc_gports.port2, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
        }
        cc_gports.port1 = srv6_tunnel_id;
        BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_cross_connect_add(unit, &cc_gports), "");
    }

    /*
     * 9. Add fwd entries
     */
    if (!is_p2p) {
        if (*dnxc_data_get(unit, "l3", "fwd", "host_entry_support", NULL))
        {
            for (i=0; i < 4; i++) {
                /** add Egress 2nd-Pass, IPv4 Packet DIP as host to do forwarding on to FEC when IPv4 is next header over SRv6*/
                BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, vrf_in[i], egress_encoded_fec, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0), "");

                /** add Egress 2nd-Pass, IPv6 Packet DIP as host to do forwarding on to FEC when IPv6 is next header over SRv6 */
                BCM_IF_ERROR_RETURN_MSG(add_host_ipv6_encap_dest(unit, cint_srv6_tunnel_info.route_ipv6_dip, vrf_in[i], egress_encoded_fec, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0), "");

                /** add Egress 2nd-Pass, IPv6 Packet DIP as route */
                BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_ip_route_basic_ipv6_mask, vrf_in[i], egress_encoded_fec), "");
            }
        }
        else
        {
            bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
            uint32 ip4_mask = 0xFFFFFFFF;
            int _l3_itf;
            BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, egress_encoded_fec);
            for (i=0; i < 4; i++) {
                /** add Egress 2nd-Pass, IPv4 Packet DIP as host to do forwarding on to FEC when IPv4 is next header over SRv6*/
                BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, ip4_mask, cint_srv6_tunnel_info.vrf_in[i], _l3_itf), "");

                /** add Egress 2nd-Pass, IPv6 Packet DIP as host to do forwarding on to FEC when IPv6 is next header over SRv6 */
                BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip, ip6_mask, cint_srv6_tunnel_info.vrf_in[i], _l3_itf), "");
                /** add Egress 2nd-Pass, IPv6 Packet DIP as route */
                BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_ip_route_basic_ipv6_mask, vrf_in[i], egress_encoded_fec), "");
            }
        }
        /* Add MACT entry */
        uint32 l2_flags = 0;
        if (*dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
        {
            l2_flags |= BCM_L2_STATIC;
        }
        bcm_l2_addr_t l2_addr;
        bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_termination_mact_fwd_address, cint_srv6_tunnel_info.l2_termination_vsi);
        l2_addr.port = out_port;
        l2_addr.flags = l2_flags;
        BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2_addr), "");
    }

    /* Add FWD entry for DIP in IP header if function lif is not hit and no default lif is used */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, dip_ipv6_for_no_default, mask, cint_srv6_tunnel_info.vrf_in[RCH], egress_encoded_fec), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 *    Multiple device SRv6 endpoint example
 */
int
multi_dev_srv6_endpoint_example(
    int ingress_unit,
    int egress_unit,
    int in_port,
    int out_port)
{
    char error_msg[200]={0,};
    bcm_gport_t gport;
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    l3_ingress_intf ingress_itf;
    l3_ingress_intf_init(&ingress_itf);

    /*
     * Update FEC IDS
     */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(ingress_unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(ingress_unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(egress_unit, out_port), "intf_out");

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(ingress_unit, cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1], cint_srv6_tunnel_info.intf_in_mac_address[END_POINT_1]), error_msg);

    /** SRV6 Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(egress_unit, cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.intf_out_mac_address[END_POINT_1]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_itf.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_1];
    ingress_itf.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1];
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.vrf_in[END_POINT_1]);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(ingress_unit, &ingress_itf), error_msg);

    /*
     * 5. Create FEC and set its properties
     */
    uint32 flag2 = 0;
    if (*dnxc_data_get(ingress_unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flag2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only: fec = %d", cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(ingress_unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], out_port, 0, flag2), error_msg);

    /*
     * 6. Create ARP and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d", cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(egress_unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], cint_srv6_tunnel_info.arp_next_hop_mac[END_POINT_1], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1]), error_msg);

    /*
     * 7. Add Route and host entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(ingress_unit, cint_ip_route_basic_ipv6_route, cint_ip_route_basic_ipv6_mask, cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), "");

    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(ingress_unit, cint_ip_route_basic_ipv6_host, cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]), "");

    /*
     * 8. SRv6 endpoint
     */
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_1];
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(ingress_unit, &tunnel_term_set), "");

    /* Update tunnel's VRF */
    l3_ingress_intf_init(&ingress_itf);
    ingress_itf.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_1];;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_itf.intf_id, tunnel_term_set.tunnel_id);
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", ingress_itf.intf_id);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(ingress_unit, &ingress_itf), error_msg);

    return BCM_E_NONE;
}

/*
 *    Multiple device SRv6 ingress tunnel example
 */
int
multi_dev_srv6_ingress_tunnel_example(
                            int ingress_unit,
                            int egress_unit,
                            int in_port,
                            int out_port,
                            int nof_sids,
                            int is_eth_fwd)
{
    char error_msg[200]={0,};
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
    int ive_edit_profile = 5;
    int action_id = 6;
    int new_outer_vid = 10;
    int new_inner_vid = 9;
    int tag_format = 8; /* default 0x8100 */
    bcm_vlan_port_t vlan_port;
    l3_ingress_intf ingress_rif;

    char *proc_name = "multi_dev_srv6_ingress_tunnel_example";
    if (nof_sids < 2 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 2 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }

    /*
     * Update FEC IDS
     */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(ingress_unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(ingress_unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(egress_unit, out_port), "intf_out");

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** SRV6 Ingress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(ingress_unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]), error_msg);

    /** SRV6 Ingress Tunnel out-port SA  */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(egress_unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(ingress_unit, &ingress_rif), "SRV6 Ingress Tunnel");

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */
    uint32 flags2;
    /* allow 2 TAG add */
    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION | BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(egress_unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2),
        "create egress object ARP only, in SRV6 Ingress Tunnel");

    /*
     * 6. SRV6 FEC Entries
     */
    bcm_gport_t gport;
    int srv6_basic_lif_encode;
    int encoded_fec1;
    uint32 flag2 = 0;
    if (*dnxc_data_get(ingress_unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flag2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create_inner(ingress_unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], srv6_basic_lif_encode, 0, out_port, 0, flag2,
                                           0, 0,&encoded_fec1), "create egress object FEC_1 only");


    /*
     * We don't need 2nd FEC here since in egress SRH base points to the 1st SID
     */

    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add IPv4 host entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC to bring out IPV6, SRH*/
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(ingress_unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1, 0, 0),
        "for SRV6 Ingress Tunnel");


    /*
     * 9. Add IPv6 host entry
     */
    snprintf(error_msg, sizeof(error_msg), "vrf = %d, fec = %d", cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1);
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(ingress_unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], encoded_fec1), error_msg);

    /*
     * 10. Add MACT entry
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);

    uint32 l2_flags = 0;
    if (*dnxc_data_get(ingress_unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
    {
        l2_flags |= BCM_L2_STATIC;
    }
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_fwd_mac, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    l2_addr.port = gport;
    l2_addr.flags = l2_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(ingress_unit, &l2_addr), "");

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t sid_info;

    /** to store next SID GPORT and init for last SID (sid_idx = 0 in below config) to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_sid_tunnel_id, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS]);

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** don't use any special flags */
        sid_info.flags = 0;

        /** must set the tunnel id to 0 if not using WITH_ID flag */
        sid_info.tunnel_id = 0;

        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (next_sid_tunnel_id != 0)
        {
            /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
            BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);
        }
        else
        {
            sid_info.next_encap_id = 0;
        }
        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        snprintf(error_msg, sizeof(error_msg), "for SID%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_sid_initiator_create(egress_unit, &sid_info), error_msg);


        /** save the last sid encap id , used for replace */
        if (sid_idx == 0) {
            last_sid_tunnel_id = sid_info.tunnel_id;
        }
        /** save the first sid encap id , used for replace */
        if (sid_idx == (nof_sids-1)) {
            first_sid_tunnel_id = sid_info.tunnel_id;
        }
    }

    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;
    bcm_srv6_srh_base_initiator_info_t_init(&srh_base_info);
    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;

    /* Set SRH base next_encap_id to be 1st SID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(srh_base_info.next_encap_id, sid_info.tunnel_id);

    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    BCM_IF_ERROR_RETURN_MSG(bcm_srv6_srh_base_initiator_create(egress_unit, &srh_base_info), "");

    srv6_srh_base_tunnel_id = srh_base_info.tunnel_id;

    /*
     * 11. Configuring ISR SRv6 Tunnel
          creating EEDB entries for SRH Base -> SIDs[0..2] -> IPv6 Tunnel (SRv6 Type)
     */

    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;

    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = is_eth_fwd ? bcmTunnelTypeEthSR6:bcmTunnelTypeSR6;
    tunnel_init_set.flags = BCM_TUNNEL_INIT_STAT_ENABLE | BCM_TUNNEL_WITH_ID; /* Enable PP STAT */
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(tunnel_init_set.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_IP_TUNNEL_GLOBAL_LIF]);
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(tunnel_init_set.l3_intf_id, srh_base_info.tunnel_id);

    /* SRv6 tunnel QoS & TTL, initialize as uniform */
    tunnel_init_set.ttl = egress_ttl_val;
    tunnel_init_set.dscp = egress_qos_val;
    tunnel_init_set.egress_qos_model.egress_qos = egress_qos_model;
    tunnel_init_set.egress_qos_model.egress_ttl = egress_ttl_model;
    /* since srh point from ipv6 tunnel, it is essential to determine the estimated encap size */
    BCM_IF_ERROR_RETURN_MSG(srv6_estimate_encap_size_ipv6_tunnel_point_to_srh(egress_unit, nof_sids, &tunnel_init_set.estimated_encap_size), "");
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(egress_unit, &intf, &tunnel_init_set), "for IPv6 Tunnel");

    /* Save SRv6 tunnel ID for replace purpose */
    srv6_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * 12. Add ingress AC IVE
     */
    /* set edit profile for ingress LIF */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = new_outer_vid;
    port_trans.new_inner_vlan = new_inner_vid;
    port_trans.gport = in_port_intf[0];
    port_trans.vlan_edit_class_id = ive_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(ingress_unit, &port_trans), "");

    /* Create action ID */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(ingress_unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id),
        "on ingress_unit");

    if (ingress_unit != egress_unit)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(egress_unit, BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id),
            "on egress_unit");
    }

    /* Set translation action class (map edit_profile & tag_format to action_id) */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = ive_edit_profile;
    action_class.tag_format_class_id = tag_format;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_INGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(ingress_unit, &action_class), "");

    /* Set translation action on egress unit */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x9100;
    action.dt_outer = bcmVlanActionReplace;
    action.dt_inner = bcmVlanActionAdd;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(egress_unit,BCM_VLAN_ACTION_SET_INGRESS,action_id,&action), "");

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 *    Multiple device SRv6 egress tunnel example
 */
int
multi_dev_srv6_egress_tunnel_example(
                            int ingress_unit,
                            int egress_unit,
                            int in_port,
                            int rch_port,
                            int out_port,
                            int is_eth_fwd)
{
    char error_msg[200]={0,};
    char *proc_name = "multi_dev_srv6_egress_tunnel_example";
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    uint32 ibch1_supported = *dnxc_data_get(ingress_unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);

    /*
     * Update FEC IDS
     */
    BCM_IF_ERROR_RETURN_MSG(srv6_update_fecs_values(ingress_unit), "");

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(ingress_unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]), "intf_in");

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(egress_unit, out_port), "intf_out");

    /*
     * 3. Create ETH-RIF and set its properties
     */
    /** SRV6 Egress Tunnel My-MAC */
    snprintf(error_msg, sizeof(error_msg), "intf_in %d", cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(ingress_unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]), error_msg);


    /** out_port SA */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(egress_unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]), "intf_out");

    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    snprintf(error_msg, sizeof(error_msg), "intf_in = %d", cint_srv6_tunnel_info.vrf_in[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(ingress_unit, &ingress_rif), error_msg);

    /*
     * 6. Create FEC and set its properties
     */
    uint32 flag2 = 0;
    if (*dnxc_data_get(ingress_unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flag2 = BCM_L3_FLAGS2_FWD_ONLY;
    }
    snprintf(error_msg, sizeof(error_msg), "create egress object FEC only: fec = %d", cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(ingress_unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], out_port, 0, flag2), error_msg);

    /*
     * 7. Create ARP and set its properties
     */
    snprintf(error_msg, sizeof(error_msg), "create egress object ARP only: encap_id = %d", cint_srv6_tunnel_info.tunnel_arp_id[EGRESS]);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(egress_unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS]), error_msg);

    /*
     * 8. Add IPv4 host entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(ingress_unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], 0, 0),
        "for SRV6 Ingress Tunnel");

    /*
     * 9. Add IPv6 host entry
     */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv6(ingress_unit, cint_srv6_tunnel_info.route_ipv6_dip, cint_srv6_tunnel_info.vrf_in[EGRESS], cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]), "");

    /*
     * 9. Add MACT entry
     */
    uint32 l2_flags = 0;
    if (*dnxc_data_get(ingress_unit, "l2", "general", "separate_fwd_learn_mact", NULL) == 0)
    {
        l2_flags |= BCM_L2_STATIC;
    }
    bcm_l2_addr_t l2_addr;
    bcm_l2_addr_t_init(&l2_addr, cint_srv6_tunnel_info.l2_termination_mact_fwd_address, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    l2_addr.port = out_port;
    l2_addr.flags = l2_flags;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(ingress_unit, &l2_addr), "");

    /*
     * 10. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    BCM_IF_ERROR_RETURN_MSG(out_port_set(ingress_unit, rch_port), "recycle_port");
    if (!ibch1_supported)
    {
        /** Configure RCH port of Extended Termination type */
        snprintf(error_msg, sizeof(error_msg), "for rch_port %d", rch_port);
        BCM_IF_ERROR_RETURN_MSG(bcm_port_control_set(ingress_unit, rch_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedTerm), error_msg);
    }
    /** Set port class in order for PMF to crop RCH ad IRPP */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(ingress_unit,rch_port,bcmPortClassFieldIngressPMF3TrafficManagementPortCs,4), "");

    /** Create termination for next protocol nof_sids to RCH port and encap_id */
    int max_nof_terminated_usp_sids = *(dnxc_data_get (ingress_unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (ingress_unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;
    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
    		ingress_unit, nof_2pass_recycle_entries, max_nof_terminated_usp_sids_1pass, rch_port),"");

    /*
     * 8. SRv6 tunnel termination
     */
    bcm_tunnel_terminator_t tunnel_term_set;
    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeIpAnyIn6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    tunnel_term_set.flags = BCM_TUNNEL_TERM_EXTENDED_TERMINATION | BCM_TUNNEL_TERM_STAT_ENABLE;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelShortpipe;
    tunnel_term_set.ingress_qos_model.ingress_phb = bcmQosIngressModelShortpipe;
    tunnel_term_set.ingress_qos_model.ingress_remark = bcmQosIngressModelShortpipe;
    if (is_eth_fwd)
    {
        tunnel_term_set.vlan = cint_srv6_tunnel_info.l2_termination_vsi;
        tunnel_term_set.type = bcmTunnelTypeEthIn6;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(ingress_unit, &tunnel_term_set), "");

    if (!is_eth_fwd)
    {
        /* Update tunnel's VRF */
        l3_ingress_intf_init(&ingress_rif);
        ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];;
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
        snprintf(error_msg, sizeof(error_msg), "intf_in = %d", ingress_rif.intf_id);
        BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(ingress_unit, &ingress_rif), error_msg);
    }

    printf("%s(): Exit\r\n",proc_name);
    return BCM_E_NONE;
}

/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * VXLAN global info structure
 */
struct cint_vxlan_global_info_s
{
    bcm_switch_network_group_t vxlan_network_group_id; /* ingres/egress orientation for VXLAN tunnel LIF */
    bcm_switch_network_group_t vxlan_network_group_1;  /* VXLAN network group 1 */
    bcm_switch_network_group_t vxlan_network_group_2;  /* VXLAN network group 2 */
    bcm_switch_network_group_t ac_network_group;       /* AC LIF network group 0 */
    int inner_vlan;                                    /* Virtual machine VID */
    int eth_rif_intf_core;                             /* Core RIF */
    int eth_rif_intf_virtual_machine;                  /* Virtual Machine RIF */
    int eth_rif_virtual_machine_vrf;                   /* router instance VRF resolved at VTT1 */
    int eth_rif_core_vrf;                              /* VRF after termination,resolved as Tunnel-InLif property */
    bcm_mac_t core_mac_address;                        /* mac for next hop */
    bcm_mac_t virtual_machine_mac_address;             /* tunnel next hop MAC DA */
    bcm_mac_t virtual_machine_da;                      /* Remote VM DA */
    bcm_ip_t tunnel_dip;                               /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;                          /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                               /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;                          /* tunnel SIP mask */
    int arp_id;                                        /* Id for ARP entry */
    int fec_id;                                        /* FEC id */
    int vpn_id;                                        /* VPN ID */
    int vni;                                           /* VNI of the VXLAN */
    int vxlan_tunnel_type;                             /* VxLAN or VxLAN-GPE */
    dnx_utils_vxlan_learn_mode_t learn_mode;           /* vxlan learn mode */

};

cint_vxlan_global_info_s cint_vxlan_global_info = {
    /*
     * vxlan_network_group_id | _1 | _2 | ac_network_group | inner_vlan
     */
    1, 1, 2, 0, 510,
    /*
     * eth_rif_intf_core | eth_rif_intf_virtual_machine
     */
    15, 100,
    /*
     * eth_rif_virtual_machine_vrf | eth_rif_core_vrf
     */
    5, 1,
    /*
     * core_mac_address | virtual_machine_mac_address | virtual_machine_da
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0x00, 0xf1},
    /*
     * tunnel_dip
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel_dip_mask
     */
    0xffffffff,
    /*
     * tunnel_sip
     */
    0xAC000001 /* 172.0.0.1 */ ,
    /*
     * tunnel SIP mask
     */
    0xffffffff,
    /*
     * arp_id
     */
    0,
    /*
     * fec id
     */
        40981,
    /*
     * VPN ID
     */
    15,
    /*
     * VNI
     */
    5000,
    /*
     * vxlan_tunnel_type
     */
    bcmTunnelTypeVxlanGpe,
    /*
     * learn mode
     */
    LEARN_FEC_ONLY
}
;

/*
 * Reusable VXLAN-GPE L2 tunnel termination example config with parameters:
 *  unit                 - relevant unit
 *  core_port            - port where core routers and aggregation switches are connected.
 *  virtual_machine_port - port where virtual machines are connected.
 */
int
vxlan_split_horizon_termination(
    int unit,
    int core_port,
    int virtual_machine_port)
{

    char error_msg[200]={0,};
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_gport_t vlan_port_id;


    /*
     * Build L2 VPN
     */
    snprintf(error_msg, sizeof(error_msg), "vpn=%d", cint_vxlan_global_info.vpn_id);
    BCM_IF_ERROR_RETURN_MSG(vxlan_open_vpn(unit, cint_vxlan_global_info.vpn_id, cint_vxlan_global_info.vni), error_msg);

    /*
     * Create for MC VPN ID
     */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, cint_vxlan_global_info.vpn_id, 0), error_msg);

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, core_port, cint_vxlan_global_info.eth_rif_intf_core), "core");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, virtual_machine_port), "vm_port");

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vxlan_global_info.eth_rif_intf_core,
                             cint_vxlan_global_info.core_mac_address), "eth_rif_intf_core");

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vxlan_global_info.eth_rif_intf_virtual_machine,
                             cint_vxlan_global_info.virtual_machine_mac_address), "eth_rif_intf_virtual_machine");

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_global_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_global_info.eth_rif_core_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf), "intf_out");

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_terminator_create(unit, &term_tunnel_id), "");

    /*
     * Create ARP entry and set its properties
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit,
                                       BCM_L3_EGRESS_ONLY,
                                       &(cint_vxlan_global_info.arp_id),
                                       cint_vxlan_global_info.virtual_machine_mac_address,
                                       cint_vxlan_global_info.eth_rif_intf_virtual_machine),
                                       "create egress object ARP only");

    /*
     * Create the tunnel initiator
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_initiator_create(unit, cint_vxlan_global_info.arp_id, &encap_tunnel_intf, &encap_tunnel_id), "");

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vxlan_global_info.fec_id), "");
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vxlan_global_info.fec_id, encap_tunnel_intf,
                                     0, virtual_machine_port, 0, fwd_flag), "create egress object FEC only");

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id), "");

    /*
     * Create and configure VLAN port
     */
    BCM_IF_ERROR_RETURN_MSG(vlan_port_add(unit, virtual_machine_port, &vlan_port_id), "");

    /*
     * add to vswitch 
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, cint_vxlan_global_info.vpn_id, vlan_port_id), "");

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_l2_addr_add(unit, virtual_machine_port), "");

    return BCM_E_NONE;
}

/*
 * Configure the orientation filters:
 * filter AC inLif (network group:    0) to VxLAN outLif (network group : 1)
 * filter VxLAN inLif(network group : 1) to AC outLif(network group: 0)
 * filter VxLAN inLif(network group : 1) to VxLAN outLif(network group: 2)
 */
int
vxlan_orientation_filters_init(
    int unit)
{

    bcm_switch_network_group_config_t network_group_config;

    /* drop the packet */
    uint32 flags = BCM_SWITCH_NETWORK_GROUP_EGRESS_PRUNE_ENABLE;

    /* set network_group orientations for VxLAN tunnel LIFs */
    cint_vxlan_global_info.vxlan_network_group_1 = 1;
    cint_vxlan_global_info.vxlan_network_group_2 = 2;
    /* set the AC network group */
    cint_vxlan_global_info.ac_network_group = 0;

    char* proc_name;
    proc_name = "vxlan_orientation_filters_init";

    printf("%s(): Add orientation filter 1: from AC (%d) to VXLAN (%d) \n",
           proc_name, cint_vxlan_global_info.ac_network_group, cint_vxlan_global_info.vxlan_network_group_1);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = cint_vxlan_global_info.vxlan_network_group_1;
    network_group_config.config_flags          = flags;

    /** Set orientation filter: filter AC inLif (network group: 0) to VxLAN outLif (network group : 1) */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             cint_vxlan_global_info.ac_network_group,
                                             &network_group_config), "");

    printf("%s(): Add orientation filter 2: from VXLAN (%d) to AC (%d) \n",
           proc_name, cint_vxlan_global_info.vxlan_network_group_1, cint_vxlan_global_info.ac_network_group);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = cint_vxlan_global_info.ac_network_group; 
    network_group_config.config_flags          = flags;

    /** Set orientation filter: filter VxLAN inLif(network group : 2) to AC outLif(network group: 0) */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             cint_vxlan_global_info.vxlan_network_group_1,
                                             &network_group_config), "");

    printf("%s(): Add orientation filter 3: from VXLAN (%d) to VXLAN (%d) \n",
           proc_name, cint_vxlan_global_info.vxlan_network_group_1, cint_vxlan_global_info.vxlan_network_group_2);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = cint_vxlan_global_info.vxlan_network_group_2;
    network_group_config.config_flags          = flags;

    /**.Set orientation filter: filter VxLAN inLif(network group : 1) to VxLAN outLif(network group: 2) */
    BCM_IF_ERROR_RETURN_MSG(bcm_switch_network_group_config_set(unit,
                                             cint_vxlan_global_info.vxlan_network_group_1,
                                             &network_group_config), "");

    return BCM_E_NONE;
}

/*
 * Create an MACT entry for VXLAN tunnel, mapped to the VPN 
 */
int
vxlan_l2_addr_add(
    int unit,
    int vxlan_port_id)
{

    bcm_l2_addr_t l2addr;


    bcm_l2_addr_t_init(&l2addr, cint_vxlan_global_info.virtual_machine_da, cint_vxlan_global_info.vpn_id);
    l2addr.port  = vxlan_port_id;
    l2addr.vid   = cint_vxlan_global_info.vpn_id;
    l2addr.flags = BCM_L2_STATIC;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2addr), "");

    return BCM_E_NONE;
}

/*
 * Add VXLAN port properties and configure the tunnel terminator In-Lif,
 * tunnel initiatior Out-Lif and the VXLAN network group orientation
 */
int
vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    int tunnel_encapsulation_id)
{
    char error_msg[200]={0,};

    dnx_utils_vxlan_port_add_s vxlan_port_add;


    dnx_utils_vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn              = cint_vxlan_global_info.vpn_id;
    vxlan_port_add.in_port          = core_port;
    vxlan_port_add.in_tunnel        = tunnel_term_id;
    /* in case it's learn FEC only. Don't set the tunnel outlif. */
    if (cint_vxlan_global_info.learn_mode != LEARN_FEC_ONLY) {
       vxlan_port_add.out_tunnel       = tunnel_encapsulation_id;
    }
    vxlan_port_add.flags            = 0;
    vxlan_port_add.network_group_id = cint_vxlan_global_info.vxlan_network_group_id;

    /* in case it's not outlif + port, set flag: BCM_VXLAN_PORT_EGRESS_TUNNEL to indicate FEC is valid */
    if (cint_vxlan_global_info.learn_mode != LEARN_SYM_OUTLIF_PORT) {
        BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_global_info.fec_id);
    }
    snprintf(error_msg, sizeof(error_msg), "in_gport=0x%08x", tunnel_term_id);
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_vxlan_port_add(unit, vxlan_port_add), error_msg);

    return BCM_E_NONE;
}


/*
 * Create and configure VXLAN encapsulation tunnel
 */
int
vxlan_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int *encap_tunnel_id)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;


    /*
     * Create IP tunnel initiator for encapsulating Vxlan tunnel header
     */
    bcm_l3_intf_t_init(&l3_intf);

    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = cint_vxlan_global_info.tunnel_dip;
    tunnel_init.sip        = cint_vxlan_global_info.tunnel_sip;
    tunnel_init.flags      = 0;
    tunnel_init.dscp       = 10;
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.type       = cint_vxlan_global_info.vxlan_tunnel_type;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl        = 64;
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init), "");

    *tunnel_intf     = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;

    return BCM_E_NONE;
}

/* 
 * Main function for configuring the VXLAN VPN
 */
int
vxlan_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{

    bcm_vxlan_vpn_config_t vpn_config;


    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags                   = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn                     = vpn_id;
    vpn_config.broadcast_group         = vpn_id;
    vpn_config.unknown_unicast_group   = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid                    = vni;

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit, &vpn_config), "");

    return BCM_E_NONE;
}

/*
 * Create and configure VXLAN termination tunnel
 */
int
vxlan_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id)
{

    bcm_tunnel_terminator_t tunnel_term;


    /*
     * Create IP tunnel terminator for SIP,DIP, VRF lookup in TCAM
     */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip      = cint_vxlan_global_info.tunnel_dip;
    tunnel_term.dip_mask = cint_vxlan_global_info.tunnel_dip_mask;
    tunnel_term.sip      = cint_vxlan_global_info.tunnel_sip;
    tunnel_term.sip_mask = cint_vxlan_global_info.tunnel_sip_mask;
    tunnel_term.vrf      = cint_vxlan_global_info.eth_rif_core_vrf;
    tunnel_term.type     = cint_vxlan_global_info.vxlan_tunnel_type;

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit, &tunnel_term), "");
    *term_tunnel_id = tunnel_term.tunnel_id;
    return BCM_E_NONE;
}

/*
 * Create and configure VLAN port
 */
int
vlan_port_add(
    int unit,
    bcm_gport_t in_port,
    bcm_gport_t * port_id)
{

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);


    /*
     * the match criteria is port:1, out-vlan:510 
     */
    vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port        = in_port;
    vlan_port.match_vlan  = cint_vxlan_global_info.inner_vlan;
    vlan_port.flags       = 0;
    vlan_port.vsi         = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /*
     * handle of the created gport 
     */
    *port_id = vlan_port.vlan_port_id;

    return BCM_E_NONE;
}

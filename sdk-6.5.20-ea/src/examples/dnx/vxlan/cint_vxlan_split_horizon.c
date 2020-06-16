/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
    bcmTunnelTypeVxlanGpe
};

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

    int rv;
    char *proc_name;
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;

    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);

    bcm_gport_t vlan_port_id;

    proc_name = "vxlan_split_horizon_termination";

    /*
     * Build L2 VPN
     */
    rv = vxlan_open_vpn(unit, cint_vxlan_global_info.vpn_id, cint_vxlan_global_info.vni);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_open_vpn, vpn=%d, \n", proc_name, cint_vxlan_global_info.vpn_id);
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, core_port, cint_vxlan_global_info.eth_rif_intf_core);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set - core\n", proc_name);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set vm_port \n", proc_name);
        return rv;
    }

    /*
     * Create Routing interface for the tunnel's DIP. We will use it as ingress ETH-RIF to perform
     * LL termination (my-mac procedure), to enable the IPv4 routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_global_info.eth_rif_intf_core,
                             cint_vxlan_global_info.core_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_core \n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel termination.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_vxlan_global_info.eth_rif_intf_virtual_machine,
                             cint_vxlan_global_info.virtual_machine_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create eth_rif_intf_virtual_machine\n", proc_name);
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    ingr_itf.intf_id = cint_vxlan_global_info.eth_rif_intf_core;
    ingr_itf.vrf = cint_vxlan_global_info.eth_rif_core_vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_ingress_rif_set intf_out \n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    rv = vxlan_tunnel_terminator_create(unit, &term_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function vxlan_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    /*
     * Create ARP entry and set its properties
     */
    rv = l3__egress_only_encap__create(unit,
                                       BCM_L3_EGRESS_ONLY,
                                       &(cint_vxlan_global_info.arp_id),
                                       cint_vxlan_global_info.virtual_machine_mac_address,
                                       cint_vxlan_global_info.eth_rif_intf_virtual_machine);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n", proc_name);
        return rv;
    }

    /*
     * Create the tunnel initiator
     */
    rv = vxlan_tunnel_initiator_create(unit, cint_vxlan_global_info.arp_id, &encap_tunnel_intf, &encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function tunnel_initiator_configure \n", proc_name);
        return rv;
    }

    /*
     * Create FEC and configure its: Outlif (ARP), Destination port
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vxlan_global_info.fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = l3__egress_only_fec__create(unit, cint_vxlan_global_info.fec_id, encap_tunnel_intf,
                                     0, virtual_machine_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel initiatior Out-Lif
     */
    rv = vxlan_port_add(unit, core_port, term_tunnel_id, encap_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan_port_add \n", proc_name);
    }

    /*
     * Create and configure VLAN port
     */
    rv = vlan_port_add(unit, virtual_machine_port, &vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vlan_port_add \n", proc_name);
    }

    /*
     * add to vswitch 
     */
    rv = bcm_vswitch_port_add(unit, cint_vxlan_global_info.vpn_id, vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /*
     * Create an MACT entry for VXLAN tunnel, mapped to the VPN
     */
    rv = vxlan_l2_addr_add(unit, virtual_machine_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, mact_entry_add\n", proc_name);
        return rv;
    }

    return rv;
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
    int rv = BCM_E_NONE;

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
    rv = bcm_switch_network_group_config_set(unit,
                                             cint_vxlan_global_info.ac_network_group,
                                             &network_group_config);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_switch_network_group_config_set\n", proc_name);
        return rv;
    }

    printf("%s(): Add orientation filter 2: from VXLAN (%d) to AC (%d) \n",
           proc_name, cint_vxlan_global_info.vxlan_network_group_1, cint_vxlan_global_info.ac_network_group);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = cint_vxlan_global_info.ac_network_group; 
    network_group_config.config_flags          = flags;

    /** Set orientation filter: filter VxLAN inLif(network group : 2) to AC outLif(network group: 0) */
    rv = bcm_switch_network_group_config_set(unit,
                                             cint_vxlan_global_info.vxlan_network_group_1,
                                             &network_group_config);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_switch_network_group_config_set\n", proc_name);
        return rv;
    }

    printf("%s(): Add orientation filter 3: from VXLAN (%d) to VXLAN (%d) \n",
           proc_name, cint_vxlan_global_info.vxlan_network_group_1, cint_vxlan_global_info.vxlan_network_group_2);

    bcm_switch_network_group_config_t_init(&network_group_config);
    network_group_config.dest_network_group_id = cint_vxlan_global_info.vxlan_network_group_2;
    network_group_config.config_flags          = flags;

    /**.Set orientation filter: filter VxLAN inLif(network group : 1) to VxLAN outLif(network group: 2) */
    rv = bcm_switch_network_group_config_set(unit,
                                             cint_vxlan_global_info.vxlan_network_group_1,
                                             &network_group_config);
    if (rv != BCM_E_NONE) {
        printf("%s(): Error, bcm_switch_network_group_config_set\n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Create an MACT entry for VXLAN tunnel, mapped to the VPN 
 */
int
vxlan_l2_addr_add(
    int unit,
    int vxlan_port_id)
{
    int rv = BCM_E_NONE;

    bcm_l2_addr_t l2addr;

    char* proc_name;
    proc_name = "vxlan_l2_addr_add";

    bcm_l2_addr_t_init(&l2addr, cint_vxlan_global_info.virtual_machine_da, cint_vxlan_global_info.vpn_id);
    l2addr.port  = vxlan_port_id;
    l2addr.vid   = cint_vxlan_global_info.vpn_id;
    l2addr.flags = BCM_L2_STATIC;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_l2_addr_add \n", proc_name);
        return rv;
    }

    return rv;
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
    int rv = BCM_E_NONE;

    vxlan_port_add_s       vxlan_port_add;

    char* proc_name;
    proc_name = "vxlan_port_add";

    vxlan_port_s_clear(&vxlan_port_add);
    vxlan_port_add.vpn              = cint_vxlan_global_info.vpn_id;
    vxlan_port_add.in_port          = core_port;
    vxlan_port_add.in_tunnel        = tunnel_term_id;
    vxlan_port_add.out_tunnel       = tunnel_encapsulation_id;
    vxlan_port_add.flags            = 0;
    vxlan_port_add.network_group_id = cint_vxlan_global_info.vxlan_network_group_id;
    BCM_L3_ITF_SET(vxlan_port_add.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_global_info.fec_id);

    
    rv = vxlan__vxlan_port_add(unit, vxlan_port_add);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, vxlan__vxlan_port_add, in_gport=0x%08x\n", proc_name, tunnel_term_id);
        return rv;
    }

    return rv;
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
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    char* proc_name;
    proc_name = "vxlan_tunnel_initiator_create";

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

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_tunnel_initiator_create \n", proc_name);
    }

    *tunnel_intf     = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;

    return rv;
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
    int rv = BCM_E_NONE;

    bcm_vxlan_vpn_config_t vpn_config;

    char *proc_name;
    proc_name = "vxlan_open_vpn";

    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags                   = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn                     = vpn_id;
    vpn_config.broadcast_group         = vpn_id;
    vpn_config.unknown_unicast_group   = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid                    = vni;

    rv = bcm_vxlan_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_vxlan_vpn_create \n", proc_name);
        return rv;
    }

    return rv;
}

/*
 * Create and configure VXLAN termination tunnel
 */
int
vxlan_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id)
{
    int rv = BCM_E_NONE;

    bcm_tunnel_terminator_t tunnel_term;

    char* proc_name;
    proc_name = "vxlan_tunnel_terminator_create";

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

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_tunnel_terminator_create \n", proc_name);
        return rv;
    }

    *term_tunnel_id = tunnel_term.tunnel_id;

     /** In JR2 VRF is a tunnel-LIF property set by bcm_l3_ingress_create API */
    bcm_l3_ingress_t ing_intf;
    bcm_l3_ingress_t_init(&ing_intf);
    ing_intf.flags = BCM_L3_INGRESS_WITH_ID;    /* must, as we update exist RIF */
    ing_intf.vrf   = cint_vxlan_global_info.eth_rif_virtual_machine_vrf;

    /*
     * Convert tunnel's GPort ID to intf ID 
     */
    bcm_if_t intf_id;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);
    rv = bcm_l3_ingress_create(unit, ing_intf, intf_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_l3_ingress_create\n", proc_name);
        return rv;
    }

    return rv;
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
    int rv = BCM_E_NONE;

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    char* proc_name;
    proc_name = "vlan_port_add";

    /*
     * the match criteria is port:1, out-vlan:510 
     */
    vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port        = in_port;
    vlan_port.match_vlan  = cint_vxlan_global_info.inner_vlan;
    vlan_port.flags       = 0;
    vlan_port.vsi         = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rv;
    }

    /*
     * handle of the created gport 
     */
    *port_id = vlan_port.vlan_port_id;

    return rv;
}
